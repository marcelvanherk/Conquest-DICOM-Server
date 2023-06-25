-- mvh 20151206: fix 1.4.17d compatibility
-- 20160709   mvh   fix for PatientID with spaces in it
-- 20170305   mvh   start with send etc functions (1.4.19a)
-- 20170430   mvh   Indicate viewer in menu
-- 20180203   mvh   Removed opacity control for s[1] which does not exist
-- 20181215   mvh   Added remotequery to depend less on web cgi functionality
-- 20181230   mvh   Removed dicomquery, only kept remotequery
-- 20190112   mvh   Use | to separate items to help with special characters in patientID
-- 20200307   mvh   Avoid query with '***'
-- 20201025   mvh   Standardised header
-- 20220827   mvh   Made dgate extension more generic, allows deployment as app
-- 20220830   mvh   Add studylink to add e.g. ohif (example commented out)
-- 20220905   mvh   Fix display of ModalitiesInStudy
-- 20230625   mvh   Made all links relative; enable default ohif link

local query_pid = '';
local query_pna = '';
local query_pst = '';
local query_sta = '';

local studyviewer=gpps('webdefaults', 'studyviewer', '');
local studylink=gpps('webdefaults', 'studylink', '<TD><A target="_blank" href=/app/ohif/viewer/{StudyInstanceUID}>Ohif</A>');

function mycomp(a,b)
    if a.PatientName == nil and b.PatientName == nil then
      return false
    end
    if a.PatientName == nil then
      return true
    end
    if b.PatientName == nil then
      return false
    end
    return a.PatientName < b.PatientName
  end
  
function InitializeVar()
 if (CGI('patientidmatch') ~='' and CGI('patientidmatch') ~='*') then
     query_pid='*'..CGI('patientidmatch')..'*'
 else
    q=CGI('query')
	i, j = string.find(q, "patientid = '")
    if j~=nil then
	 s=string.sub(q, j+1)
	 k, l = string.find(s, "'")
     pid=(string.sub(s, 1,l-1))       --> patientid
     if pid == nil or pid == '' then
	 query_pid=''
     else query_pid=pid
     end
   else	 query_pid=''
   end
 end
  
 if (patientnamematch ~= '' and CGI('patientnamematch') ~='*') then
     query_pna='*'..CGI('patientnamematch')..'*' ;
 else 
  query_pna='' 
 end

 if (CGI('studydatematch') ~= '') then
   if (string.len(CGI('studydatematch'))<8) and CGI('studydatematch') ~='*'then
     query_pst='*'..CGI('studydatematch')..'*'
   else
     query_pst=CGI('studydatematch')
   end
 else
   query_pst='' 
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

function querystudy_remote()
  InitializeVar()
  local s = servercommand('get_param:MyACRNema')
  
  local b=DicomObject:new();
  b.QueryRetrieveLevel='STUDY'
  b.ModalitiesInStudy=''
  b.StudyInstanceUID='' 
  b.PatientID=query_pid
  b.PatientName=query_pna
  b.StudyDate = query_pst
  b.StudyDescription=''

  local patist=remotequery(s, 'STUDY', b);
  return patist
end;

HTML("Content-type: text/html\nCache-Control: no-cache\n");

print(
[[
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<title>Conquest DICOM server - version]]..version..[[</title>]]..
[[<style type="text/css">
body { font:10pt Verdana; }
a { color:blue; }
#content { background-color:#dddddd; width:200px; margin-top:2px; }
</style>
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
<option value=viewerstudy>View with ]]..studyviewer..[[</option>')
<option value=nop>Cancel</option>')
</select>
</p>
]], i, i, i, i, item, i)
end

HTML("<H1>Welcome to Conquest DICOM server - version %s</H1>", version)
local pats=querystudy_remote() 
if query_pna ~= '' then
  table.sort(pats, mycomp)
end


print("<table class='altrowstable' id='alternatecolor' RULES=ALL BORDER=1>");

if (query ~= '') then   
	HTML("<Caption>List of selected studies (%s) on local server</caption>", #pats)
else   
	HTML("<Caption>List of all studies (%s) on local server</caption>", #pats)
end

local linkheader=''
if studylink~='' then linkheader='<TD>' end

HTML("<TR><TD>Patient ID<TD>Name<TD>Study Date<TD>Study description<TD>Study modality<TD>Menu%s</TR>", linkheader)

for i=1,#pats do
  local t = string.format("<A HREF=?%s&mode=listseries&key=%s&query=DICOMStudies.patientid+=+'%s'+and+DICOMSeries.studyinsta+=+'%s' title='Click to see series'>%s</A>", '', tostring(key or ''),string.gsub(pats[i].PatientID, ' ', '+'),mc(pats[i].StudyInstanceUID),mc(pats[i].PatientID))
  
  local link = studylink
  link = string.gsub(link, '{StudyInstanceUID}', pats[i].StudyInstanceUID)
  link = string.gsub(link, '{PatientID}', pats[i].PatientID)
  
  local s = string.format("<TR><TD>%s<TD>%s<TD>%s<TD>%s<TD>%s%s%s</TR>",t,mc(pats[i].PatientName),mc(pats[i].StudyDate),
    mc(pats[i].StudyDescription),mc(pats[i].ModalitiesInStudy),
    dropdown(i, string.gsub(pats[i].PatientID, ' ', '+')..'|'..pats[i].StudyInstanceUID),
    link
    )
  
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
