-- 20151206   mvh   fix 1.4.17d compatibility
-- 20160709   mvh   fix for PatientID with spaces in it
-- 20160723   mvh   changed incorrect dum heading for image url
-- 20181215   mvh   Added remotequery to limit dependency on cgi functionality
-- 20181230   mvh   Removed dicomquery, only kept remotequery
-- 20200127   mvh   List ImageID instead of PatientName; fix sort if no InstanceNumber
-- 20200307   mvh   Avoid query with '***'
-- 20201025   mvh   Standardised header
-- 20220827   mvh   Made dgate extension more generic, allows deployment as app
-- 20230625   mvh   Made all links relative

local query_pid = '';
local query_pna = '';
local query_pst = '';
local query_sta = '';
 
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
 
function mcoalesce(fff)
  if fff =='' or fff == nil then
    return '';
  end; 
 local res = tostring(fff);
 return res
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
  local patis, b, s, pid, q, i, j, k, l, siuid;
 
  InitializeVar()
  s = servercommand('get_param:MyACRNema')

  q=CGI('query')
  --print(q) 
  i, j = string.find(q, "DICOMStudies.patientid = '")

  k, l = string.find(q, "' and")
  pid=(string.sub(q, j+1,k-1))       --> patientid
  
  i, j = string.find(q, "DICOMSeries.seriesinst = '")
  siuid=(string.sub(q, j+1))       --> seriesinst
  siuid = string.gsub(siuid, "'", '')
  --print(pid,siuid)

  b=DicomObject:new();
  b.QueryRetrieveLevel='IMAGE'
  b.PatientID        = pid
  b.SeriesInstanceUID = siuid
  b.SOPInstanceUID   = '';
  b.InstanceNumber = '';
  b.SliceLocation = '';
  b.PatientName=''
  b.ImageID=''
  b.ImageDate=''
  b.StudyInstanceUID=''
  local imaget=remotequery(s, 'IMAGE', b);
  table.sort(imaget, function(a,b) return (tonumber(a.InstanceNumber) or 0)<(tonumber(b.InstanceNumber) or 0) end)
  return imaget
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


print(
[[
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

print
[[
<SCRIPT language=JavaScript>
function PopupCenter(pageURL, title,w,h)
{ var left = (screen.width/2)-(w/2);
  var top = (screen.height/2)-(h/2);
  var targetWin = window.open (pageURL, title, 'toolbar=no, location=no, directories=no, status=no, menubar=no, scrollbars=no, resizable=no, copyhistory=no, width='+w+', height='+h+', top='+top+', left='+left);
}
</SCRIPT>
]]


--Print to iframe
print([[
<SCRIPT language=JavaScript>
function DisplayImage(source_image)
{
  document.getElementById("myframe").src   = source_image;
}
</SCRIPT>
]])


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


HTML("</head>");
HTML("<body BGCOLOR='CFDFCF'>");
HTML("<H1>Welcome to Conquest DICOM server - version %s</H1>", version)

local pats=queryimagem_remote()
local a = #pats

if #pats>50 then
  local skip = math.ceil(#pats/50)
  local pats2 = {}
  for i=0, math.floor(#pats/skip)-1 do
    pats2[#pats2+1] = pats[i*skip+1]
  end
  
  pats = {};
  for i=1, #pats2 do
    pats[#pats+1] = pats2[i]
  end
end
local b = #pats

HTML("<table  class='altrowstable' id='alternatecolor'  RULES=ALL BORDER=1>");
HTML("<Caption>List of images with thumbnails on local server (%s out of %s)</caption>", b, a);
HTML("<TR><TD>Patient ID<TD>Image ID<TD>Date<TD>Image number<TD>Slice location<TD>Icon</TR>");
	
for i=1,#pats do
  studyuid=pats[i].StudyInstanceUID;
  seriesUID=pats[i].SeriesInstanceUID;
  oiuid=pats[i].SOPInstanceUID
  sl=mcoalesce(pats[i].PatientID)..':'..mcoalesce(pats[i].SOPInstanceUID);

  local url_header 
  --url_header = '<A href="#" onClick="javascript:PopupCenter('.."'" - doesn´t format the header
  url_header = '<A href="#" onClick="javascript:getTextSync('.."'"  -- to format the header
  url_header = url_header .. '?requestType=WADO&contentType=text/html'
  url_header = url_header .. '&seriesUID=' .. seriesUID
  url_header = url_header .. '&studyUID=' .. studyuid
  url_header = url_header .. '&objectUID=' .. oiuid
  url_header = url_header .. "&dum=.txt',500, 500)"..'">'
	
	
	
  url_img = '<A href="#"  title="Click to see Image"  onClick="javascript:PopupCenter('.."'"
  url_img = url_img .. '?requestType=WADO&contentType=image/jpeg'
  url_img = url_img .. '&seriesUID=' .. seriesUID
  url_img = url_img .. '&studyUID=' .. studyuid
  url_img = url_img .. '&objectUID=' .. oiuid
  url_img = url_img .. "&dum=.jpg', 'myframe', 500, 500)"..'">'
	
  --if want print into iframe -see on end of code to iframe settings
  --[[ 
  url_img = '<A href="#" onClick="javascript:DisplayImage('.."'"
  url_img = url_img .. '?requestType=WADO&contentType=image/gif'
  url_img = url_img .. '&seriesUID=' .. seriesUID
  url_img = url_img .. '&studyUID=' .. studyuid
  url_img = url_img .. '&objectUID=' .. oiuid
  url_img = url_img .. "&dum=.txt')"..'">'
  --]]	
	
  t = url_img..string.format("%s</A>",mcoalesce(pats[i].PatientID));
						   
  v = string.format("<IMG SRC=?%s&mode=slice&slice=%s:%s&size=%s&graphic=%s width='100%%' height='%s' alt='' title='Click to see header'></A>", '', string.gsub(pats[i].PatientID, ' ', '+'),  mcoalesce(pats[i].SOPInstanceUID),iconsize, graphic, iconsize);
  v = url_header..v
 
  s = string.format("<TR><TD>%s<TD>%s<TD>%s<TD>%s<TD>%s<TD>%s</TR>",t,mcoalesce(pats[i].ImageID), mcoalesce(pats[i].ImageDate),mcoalesce(pats[i].InstanceNumber), mcoalesce(pats[i].SliceLocation),v);
  sl2=sl
  print(s)
end 
	
-- if want print image into iframe
--print([[
--<iframe id="myframe" frameborder="0" HEIGHT=]]..size..[[WIDTH=]]..size..[[>
--<p>Your browser does not support iframes.</p>
--</iframe>-
--]])
--
print
[[
</table>
</body>
</html>
]]



