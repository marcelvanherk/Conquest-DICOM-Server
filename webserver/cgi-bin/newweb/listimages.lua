-- 20160723   mvh   changed incorrect dum heading for image url
-- 20170305   mvh   start with send etc functions (1.4.19)
-- 20180203   mvh   Removed opacity control for s[1] which does not exist
-- 20181215   mvh   Added remotequery to limit dependency on cgi functionality
-- 20181230   mvh   Removed dicomquery, only kept remotequery
-- 20190112   mvh   Use | to separate items to help with special characters in patientID
-- 20200127   mvh   List ImageID instead of PatientName; fix sort if no InstanceNumber
-- 20200307   mvh   Avoid query with '***'
-- 20201025   mvh   Standardised header
-- 20230625   mvh   Made all links relative

local query_pid = '';
local query_pna = '';
local query_pst = '';
local query_sta = '';
local studyuid = '';
local seriesUID = '';
local oiuid = '';

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
end;
   
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

function queryimagem_remote()
  InitializeVar()
  local s = servercommand('get_param:MyACRNema')

  local q=CGI('query', query)
  i, j = string.find(q, "DICOMStudies.patientid = '")

  local k, l = string.find(q, "' and")
  local pid=(string.sub(q, j+1,k-1))       --> patientid
  
  i, j = string.find(q, "DICOMSeries.seriesinst = '")
  local siuid=(string.sub(q, j+1))       --> seriesinst
  siuid = string.gsub(siuid, "'", '')
  --print(pid,siuid)

  local b=DicomObject:new();
  b.QueryRetrieveLevel='IMAGE'
  b.PatientID        = pid
  b.SeriesInstanceUID = siuid
  b.SOPInstanceUID   = '';
  b.InstanceNumber = '';
  b.SliceLocation = '';
  b.PatientName=''
  b.ImageDate=''
  b.ImageID=''
  b.StudyInstanceUID ='' 
  local imaget=remotequery(s, 'IMAGE', b);
  table.sort(imaget, function(a,b) return (tonumber(a.InstanceNumber) or 0)<(tonumber(b.InstanceNumber) or 0) end)
  return imaget
end

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
]]
)


print(
[[
<SCRIPT type="text/javascript">
function PopupCenter(pageURL, title,w,h)
{ var left = (screen.width/2)-(w/2);
  var top = (screen.height/2)-(h/2);
  var targetWin = window.open (pageURL, title, 'toolbar=no, location=no, directories=no, status=no, menubar=no, scrollbars=no, resizable=no, copyhistory=no, width='+w+', height='+h+', top='+top+', left='+left);
}
</SCRIPT>
]]
)

print(
[[
<SCRIPT type="text/javascript">
function getTextSync(url,w,h)
{   
    var left = (screen.width/2)-(w/2);
    var top = (screen.height/2)-(h/2);
    var teste = "ola"
    var request = new XMLHttpRequest();  // Create new request
    request.open("GET", url, false);     // Pass false for synchronous
    request.send(null);                  // ok Send the request now
	
    if (request.status !== 200) throw new Error(request.statusText);
    var type = request.getResponseHeader("Content-Type")
    if (!type.match(/^text/))
      throw new Error("Expected textual response; got: " + type);
	
    var newWindow = window.open("","Header",'toolbar=no, location=no, directories=no, status=no, menubar=no, scrollbars=yes, resizable=yes, copyhistory=no, width='+w+', height='+h+', top='+top+', left='+left);
    var text = request.responseText
	
    var html = '<html><head></head><body><H1>Header</H1><b><pre>'+ text +"</pre></b>"
    html = html+"</body></html>"
	
    newWindow.document.open()
    newWindow.document.write(html)
    newWindow.document.close()
}
</SCRIPT>
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
<option value=nop>Cancel</option>')
</select>
</p>
]], i, i, i, i, item, i)
end

HTML("</head>");
HTML("<body BGCOLOR='CFDFCF'>");
HTML("<H1>Welcome to Conquest DICOM server - version %s</H1>", version)

local pats=queryimagem_remote() 
--table.sort(pats, function(a,b) return a.SOPInstanceUID<b.SOPInstanceUID end)
--table.sort(pats, mycomp)


print("<table class='altrowstable' id='alternatecolor' RULES=ALL BORDER=1>");
HTML("<Caption>List of images on local server (%s)</caption>", #pats);
HTML("<TR><TD>Patient ID<TD>Image ID<TD>Date<TD>Image number<TD>Slice location<TD>Header<TD>Menu</TR>");
	
local studyviewer=gpps('webdefaults', 'studyviewer', '');


for i=1,#pats do
  
  studyuid = pats[i].StudyInstanceUID;
  seriesUID = pats[i].SeriesInstanceUID;
  oiuid = pats[i].SOPInstanceUID;
	
  local url_header
  local url_img
  --url_header = '<A href="#" onClick="javascript:PopupCenter('.."'"
  url_header = '<A href="#" onClick="javascript:getTextSync('.."'"
  url_header = url_header .. '?requestType=WADO&contentType=text/html'
  url_header = url_header .. '&seriesUID=' .. seriesUID
  url_header = url_header .. '&studyUID=' .. studyuid
  url_header = url_header .. '&objectUID=' .. oiuid
  url_header = url_header .. "&dum=.txt', 500, 500)"..'">'
	
  url_img = '<A href="#" onClick="javascript:PopupCenter('.."'"
  url_img = url_img .. '?requestType=WADO&contentType=image/gif'
  url_img = url_img .. '&seriesUID=' .. seriesUID
  url_img = url_img .. '&studyUID=' .. studyuid
  url_img = url_img .. '&objectUID=' .. oiuid
  url_img = url_img .. "&dum=.gif', 'myframe', 500, 500)"..'" title="click to see image">'

  local t,v,s
  
  t = url_img..string.format("%s</A>",mc(pats[i].PatientID));
 
  v = url_header.."Header</A>";
  s = string.format("<TR><TD>%s<TD>%s<TD>%s<TD>%s<TD>%s<TD>%s%s</TR>",t,mc(pats[i].ImageID), 
            mc(pats[i].ImageDate), mc(pats[i].InstanceNumber), mc(pats[i].SliceLocation), v,
	    dropdown(i, string.gsub(pats[i].PatientID, ' ', '+')..'|||'..pats[i].SOPInstanceUID));
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

