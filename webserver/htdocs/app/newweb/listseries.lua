-- mvh 20151206: fix 1.4.17d compatibility
-- 20160709   mvh   fix for PatientID with spaces in it
-- 20160717   mvh   Added experimental renderseries
-- 20160830   mvh   Removed that again
-- 20170305   mvh   start with send etc functions (1.4.19a)
-- 20170430   mvh   Indicate viewer in menu
-- 20180203   mvh   Removed opacity control for s[1] which does not exist
-- 20181215   mvh   Added remotequery to limit dependency on cgi functionality
-- 20181230   mvh   Removed dicomquery, only kept remotequery
-- 20190112   mvh   Use | to separate items to help with special characters in patientID
-- 20200307   mvh   Avoid query with '***'
-- 20201025   mvh   Standardised header
-- 20220827   mvh   Made dgate extension more generic, allows deployment as app
-- 20220830   mvh   Add serieslink to add e.g. wadoseriesviewer
-- 20230625   mvh   Made all links relative; enable default viewer link

local query_pid = '';
local query_pna = '';
local query_pst = '';
local query_sta = '';

local viewer=gpps('webdefaults', 'viewer', '');
local serieslink=gpps('webdefaults', 'serieslink', '<TD><A target="_blank" href=/app/newweb/?mode=wadoseriesviewer&series={PatientID}:{SeriesInstanceUID}>View</A>');
 
function InitializeVar()
 if (CGI('patientidmatch') ~='' and CGI('patientidmatch') ~='*') then
     query_pid='*'..CGI('patientidmatch')..'*'
 else
  query_pid='*' 
 end

 if (patientnamematch ~= '' and CGI('patientnamematch') ~='*') then
     query_pna='*'..CGI('patientnamematch')..'*' ;
 else 
  query_pna='*' 
 end

 if (CGI('studydatematch') ~= '' and CGI('studydatematch') ~='*') then
     query_pst='*'..CGI('studydatematch')..'*' ;
 else
   query_pst='*' 
 end

 if (CGI('startdatematch') ~= '' and CGI('startdatematch') ~='*') then
     query_sta='*'..CGI('startdatematch')..'*' ;
 else 
  query_sta='*' 
 end
end;
 

function mc(fff)
  return fff or ''
end;

function remotequery(ae, level, q)
  local remotecode =
[[
  local ae=']]..ae..[[';
  local level=']]..level..[[';
  local q=]]..q:Serialize()..[[;
  local q2=DicomObject:new(); for k,v in pairs(q) do q2[k]=v end;
  local r = dicomquery(ae, level, q2):Serialize();
  local s=tempfile('txt') local f=io.open(s, "wb") f:write(r) returnfile=s f:close();
]]
  local f = loadstring('return '..servercommand('lua:'..remotecode));
  if f then return f() end
end

function queryserie_remote()
  InitializeVar()
  local s = servercommand('get_param:MyACRNema')

  local q=CGI('query')
  --print(q) 
  local i, j = string.find(q, "DICOMStudies.patientid = '")

  local k, l = string.find(q, "' and")
  local pid=(string.sub(q, j+1,k-1))       --> patientid
  
  i, j = string.find(q, "DICOMSeries.studyinsta = '")
  local siuid=(string.sub(q, j+1))       --> studyinsta
  siuid = string.gsub(siuid, "'", '')

  local b=DicomObject:new();
  b.QueryRetrieveLevel='SERIES'
  b.PatientName = ''
  b.PatientID        = pid
  b.StudyDate        = ''; 
  b.StudyInstanceUID = siuid;
  b.StudyDescription = '';
  b.SeriesDescription= '';
  b.SeriesInstanceUID= '';
  b.SeriesDate= '';
  b.Modality         = '';
  b.SeriesTime       = '';

  local seriest=remotequery(s, 'SERIES', b);
  return seriest
end

HTML("Content-type: text/html\nCache-Control: no-cache\n");

print(
[[
<html>
<head>
<title>Conquest DICOM server - version]]..version..[[</title>]]..
[[<style type="text/css">
body { font:10pt Verdana; }
a { color:blue; }
#content { background-color:#dddddd; width:200px; margin-top:2px; }
</style>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
]]
)

print([[
<!-- Javascript goes in the document HEAD -->
<script type="text/javascript">
function altRows(id){
	if(document.getElementsByTagName){  
		
		var table = document.getElementById(id);  
		var rows = table.getElementsByTagName("tr"); 
		 
		for(i = 0; i < rows.length; i++){          
			if(i % 2 == 0){
				rows[i].className = "evenrowcolor";
			}else{
				rows[i].className = "oddrowcolor";
			}      
		}
	}
}
window.onload=function(){
	altRows('alternatecolor');
}
</script>
]])

print([[
<script language=JavaScript>
function servicecommand(a) {
  xmlhttp = new XMLHttpRequest(); 
  xmlhttp.open('GET',']]..[[?mode=listpatients&parameter='+a, true);
  xmlhttp.timeout = 60000
  xmlhttp.send()
  xmlhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var s = this.responseText.toString()
      document.getElementById("statusarea").innerHTML = s.replace("DICOM ERROR Protocol error 0 in PDU:Read", "");
    }
  }
}  
</script>
]])

print([[
<script language=JavaScript>
function opencommand(a) {
  window.open (']]..[[?mode=listpatients&parameter='+a);
}  
</script>
]])

print([[
<!-- CSS goes in the document HEAD or added to your external stylesheet -->
<style type="text/css">
table.altrowstable {
	font-family: verdana,arial,sans-serif;
	font-size:16px;
	color:#333333;
	border-width: 1px;
	border-color: #a9c6c9;
	border-collapse: collapse;
}
table.altrowstable th {
	border-width: 1px;
	padding: 8px;
	border-style: solid;
	border-color: #a9c6c9;
}
table.altrowstable td {
	border-width: 1px;
	padding: 8px;
	border-style: solid;
	border-color: #a9c6c9;
}
.oddrowcolor{
	background-color:#d4e3e5;
}
.evenrowcolor{
	background-color:#c3dde0;
}
table.altrowstable tr:first-child {
	font-weight:bold;
}
table.altrowstable Caption {
    font-weight:bold;
    color: yellow;
    background: green;
}
</style>
</head> 
<body BGCOLOR='CFDFCF'>
]]
)

function dropdown(i, item)
  return string.format([[
<td>
<p id='aap%d' onmouseover="var s=document.getElementById('aap%d').children; s[0].style.opacity=1;" onmouseout="var s=document.getElementById('aap%d').children; s[0].style.opacity=0.1;">
<select name=selectaction style="opacity:0.1; width:40" onchange="servicecommand(document.getElementById('aap%d').children[0].value+ '&item='+'%s');document.getElementById('aap%d').children[0].selectedIndex=0" >
<option value=nop>-</option>')
<option value=sender>Send</option>')
<option value=changerid>Change Patient ID</option>')
<option value=anonymizer>Anonymize</option>')
<option value=deleter>Delete</option>')
<option value=zipper>Zip</option>')
<option value=zipperanonymized>Zip anonymized</option>')
<option value=viewerseries>View with ]]..viewer..[[</option>')
<option value=nop>Cancel</option>')
</select>
</p>
]], i, i, i, i, item, i)
end

HTML("<H1>Welcome to Conquest DICOM server - version %s</H1>", version)

local pats=queryserie_remote() 

print("<table class='altrowstable' id='alternatecolor' RULES=ALL BORDER=1>");

local linkheader=''
if serieslink~='' then linkheader='<TD>' end

HTML("<Caption>List of series (%s) on local server</caption>", #pats);
HTML("<TR><TD>Patient ID<TD>Name<TD>Series date<TD>Series time<TD>Series description<TD>Modality<TD>Thumbs<TD>Menu%s</TR>",linkheader);
	
for i=1,#pats do
  local t = string.format("<A HREF=?%s&mode=listimages&key=%s&query=DICOMStudies.patientid+=+'%s'+and+DICOMSeries.seriesinst+=+'%s' title='Click to see images'>%s</A>", '', tostring(key or ''),string.gsub(pats[i].PatientID, ' ', '+'),mc(pats[i].SeriesInstanceUID),mc(pats[i].PatientID))
  
  local v = string.format("<A HREF=?%s&mode=listimageswiththumbs&query=DICOMStudies.patientid+=+'%s'+and+DICOMSeries.seriesinst+=+'%s'&size=%s>Thumbs</A>", 
       '', string.gsub(pats[i].PatientID, ' ', '+'),mc(pats[i].SeriesInstanceUID),size)
       
  local link = serieslink
  link = string.gsub(link, '{SeriesInstanceUID}', pats[i].SeriesInstanceUID)
  link = string.gsub(link, '{StudyInstanceUID}', pats[i].StudyInstanceUID)
  link = string.gsub(link, '{PatientID}', pats[i].PatientID)

  s = string.format("<TR><TD>%s<TD>%s<TD>%s<TD>%s<TD>%s<TD>%s<TD>%s%s%s</TR>",t,mc(pats[i].PatientName),
    mc(pats[i].SeriesDate),mc(pats[i].SeriesTime), mc(pats[i].SeriesDescription),mc(pats[i].Modality),v,
    dropdown(i, string.gsub(pats[i].PatientID, ' ', '+')..'||'..pats[i].SeriesInstanceUID),
    link);
  print(s)
end 

print
[[
</table>
<p id=statusarea>
<i>Extra information to be shown here</i>
</p>
</body>
</html>
]]
