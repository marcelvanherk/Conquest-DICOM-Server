-- mvh 20181130 Created from wadostudyviewer.lua
-- mvh 20181227 Use remotequery to limit contralize access to server 
-- mvh 20180112 Fix to allow : in patientID

-- defaults to allow debugging in zbs
study2 = study2 or 'EG2005:'
version = version or ''
size = size or 560
 
-- split query information
local patid = string.gsub(study2, ':[^:]-$', '')
local studyuid = string.gsub(study2, '^.*:', '')

------------------------------------------------------------------------
-- support functions 
------------------------------------------------------------------------

-- split string into pieces, return as table
function split(str, pat)
   local t = {} 
   local fpat = "(.-)" .. pat
   local last_end = 1
   local s, e, cap = str:find(fpat, 1)
   while s do
      if s ~= 1 or cap ~= "" then
	 table.insert(t,cap)
      end
      last_end = e+1
      s, e, cap = str:find(fpat, last_end)
   end
   if last_end <= #str then
      cap = str:sub(last_end)
      table.insert(t, cap)
   end
   return t
end

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

-- report error string on a otherwise empty reddish page
function errorpage(s)
  HTML('Content-type: text/html\n\n');
  print(
  [[
  <HEAD><TITLE>Version ]].. (version or '-')..[[ WADO viewer</TITLE></HEAD>
  <BODY BGCOLOR='FFDFCF'>
  <H3>Conquest ]].. (version or '-') ..[[ WADO viewer</H3>
  <H3>]].. s .. [[</H3>
  </BODY>
  ]])
end

-- get list of series
function queryseries(source)
  local patis, b, i, j, k, l;
 
  b=newdicomobject();
  b.QueryRetrieveLevel='SERIES'
  b.PatientName = ''
  b.PatientID        = patid
  b.StudyDate        = ''; 
  b.StudyInstanceUID = studyuid;
  b.StudyDescription = '';
  b.SeriesDescription= '';
  b.SeriesInstanceUID= '';
  b.SeriesDate= '';
  b.Modality         = '';
  b.SeriesTime       = '';
  b.NumberOfSeriesRelatedInstances = '';
  series=remotequery(source, 'SERIES', b);
  if series==nil then
   errorpage('Iframe viewer error, cannot query server at series level - check ACRNEMA.MAP for: ' .. source)
   return
  end

  -- convert returned DDO (userdata) to table; needed to allow table.sort
  return series
end

-- get list of images (called for each series)
function queryimages(source, patid, seriesuid)
  local images,imaget,b;
  
  b=newdicomobject();
  b.PatientID        = patid
  b.SeriesInstanceUID= seriesuid
  b.InstanceNumber   = ''
  b.SOPInstanceUID   = ''
  b.SliceLocation   = ''
  b.ImageComments   = ''
  b.NumberOfFrames = ''

  images=remotequery(source, 'IMAGE', b);
  if images==nil then
   errorpage('Iframe viewer error, cannot query server at image level: ' .. source)
   return
  end

  table.sort(images, function(a,b) return a.InstanceNumber<b.InstanceNumber end)
  return images
end

------------------------------------------------------------------------
-- Start main code
------------------------------------------------------------------------

-- get server version, displayed and used to block buggy zoom function
local serverversion = servercommand('display_status:');
if serverversion==nil then
  errorpage('the server is not running')
  return
end
serverversion = string.match(serverversion, 'version ([%d%.%a]*)');
local viewer=gpps('webdefaults', 'viewer', '');
local source = servercommand('get_param:MyACRNema')

anonymizer = ''
if CGI('anonymize')~='' then
  anonymizer = '&anonymize='..CGI('anonymize')
end

---------------------------------------------------------------------------------
-- generate HTML page
---------------------------------------------------------------------------------

HTML("Content-type: text/html\nCache-Control: no-cache\n");

print(
[[
<html>
<head>
<title>Iframe study viewer - version]]..version..[[</title>]]..
[[<style type="text/css">
body { font:10pt Verdana; }
a { color:blue; }
#content { background-color:#dddddd; width:200px; margin-top:2px; }
</style>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
]]
)

print([[
<script>
var size = ]]..size..[[;
var serverversion = ']]..serverversion..[[';
var script_name = ']]..(script_name or '')..[[';
var anonymizer = ']]..(anonymizer or '')..[[';
var patid = ']]..(patid or '')..[[';
</script>
]])

-- here is all the js code defined; long string is rendered line by line below
local js_code=
[[
<script>
// variable block
var modality = '';
var seriesuid = '';
var studyuid = '';
var seriesno=1;
var nseries=1;

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

// loads a series
function loadseries()
{ for (var i=1; i<=nseries; i++)
    document.getElementById("A"+i).innerHTML = "View";

  document.getElementById("A"+seriesno).innerHTML = "---->";

  document.getElementById("myframe").src   = script_name+'?mode=wadoseriesviewer'+
  '&series='+patid+':'+seriesuid;
}


</script>
]]
js_code = split(js_code, '\n')
for k,v in ipairs(js_code) do print(v) end

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
	min-width: 20px;
	max-width: 180px;
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
]])

print(string.format("<H2>Conquest IFrame study viewer - version %s</H2>", version))

-- render table with available series

local pats=queryseries(source) 

local info = string.format("seriesuid='%s';studyuid='%s';modality='%s';nseries=%d;",
  pats[1].SeriesInstanceUID, pats[1].StudyInstanceUID, pats[1].Modality, #pats)

if anonymizer~='' then
  pats[1].PatientID   = string.sub(anonymizer, 12, 99)
  pats[1].PatientName = ''
end

print("<div style='position: absolute; left: 10px; width: 550px'>")
print("<table class='altrowstable' id='alternatecolor' RULES=ALL BORDER=1>");
print(string.format("<Caption>List of series on local server for ID=%s, name=%s</caption>", pats[1].PatientID, pats[1].PatientName));
print("<TR><TD>Date<TD>Time<TD>Description<TD>Modality</TR>");
	
for i=1,#pats do
  u = string.format([[<A id=A]]..i..[[ onclick="seriesno=%d;seriesuid='%s';studyuid='%s';modality='%s';loadseries();">View</A>]], 
      i, pats[i].SeriesInstanceUID, pats[i].StudyInstanceUID, pats[i].Modality); 
  s = string.format("<TR><TD>%s<TD>%s<TD>%s<TD>%s<br>(%d)<TD>%s</TR>",pats[i].SeriesDate, pats[i].SeriesTime,
  pats[i].SeriesDescription,pats[i].Modality, 0+pats[i].NumberOfSeriesRelatedInstances,u);
  print(s)
end 

print('</table>')
print("</div>");

-- generate window for displaying image and interaction controls
print[[

<div style='position: absolute; top: 40px; left: 620px' id=lowerpage>
<iframe id=myframe BORDER=0 WIDTH=560 HEIGHT=650 scrolling="no" style=''>
<p>Your browser does not support iframes.</p>
</iframe>
]]

-- info to autoload first series for display
local info = string.format("seriesuid='%s';studyuid='%s';modality='%s';nseries=%d;",
  pats[1].SeriesInstanceUID, pats[1].StudyInstanceUID, pats[1].Modality, #pats)

print([[
<i id=hint></i>
</div>

<SCRIPT language=JavaScript>
  document.onload=]]..info..[[loadseries();
</SCRIPT>

</body>
</html>
]])
