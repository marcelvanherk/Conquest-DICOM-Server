-- mvh 20160124: Created
-- 20220827   mvh   Made dgate extension more generic, allows deployment as app
-- 20230625   mvh   Made all links relative

local query_pid = '';
local query_pna = '';
local query_pst = '';
local query_sta = '';
 
function changeuidback_(uid, stage, Type)
  return servercommand('lua:'..
	  [[
	  return changeuidback(']]..uid..[[', ']]..stage..[[', ']]..Type..[[')
	  ]])
end
 
function changeuid_(uid, stage)
  return servercommand('lua:'..
	  [[
	  return changeuid(']]..uid..[[', '', ']]..stage..[[')
	  ]])
end

function querytrialpatients()
  local list = {}
  for i=1, 1000 do
    local name = 
      servercommand('lua:'..
	  [[
	  local a=dbquery('UIDMods', 'DISTINCT Stage, NewUID', "MODType='PatientID' and Stage LIKE ']].. CGI('trial')..[['");
	  if a[ ]]..i..[[ ] and a[ ]]..i..[[ ][2] then return a[ ]]..i..[[ ][2] else return '' end
	  ]]
                   ) or ''
    if name~='' then
      list[i] = name
    else
      break
    end
  end
  return list
end;

function counttrialstudies(pat, stage)
  local list = {}
  local count = 
      servercommand('lua:'..
	  [[
	  local a=dbquery('DICOMStudies', 'StudyInsta', "PatientID=']]..changeuidback_(pat, stage, 'PatientID')..[['");
	  return #a
	  ]]
                   ) or 0
  return count
end;

function counttrialseries(pat, stage)
  local list = {}
  local count = 
      servercommand('lua:'..
	  [[
	  local a=dbquery('DICOMSeries', 'SeriesInst', "SeriesPat=']]..changeuidback_(pat, stage, 'PatientID')..[['");
	  return #a
	  ]]
                   ) or 0
  return count
end;

function counttrialobjects(pat, stage)
  local list = {}
  local count = 
      servercommand('lua:'..
	  [[
	  local a=dbquery('DICOMImages', 'SOPInstanc', "ImagePat=']]..changeuidback_(pat, stage, 'PatientID')..[['");
	  return #a
	  ]]
                   ) or 0
  return count
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
</head> 
<body BGCOLOR='CFDFCF'>
]]
)

HTML("<H1>Welcome to Conquest DICOM server - version %s</H1>", version)


local trials=querytrialpatients() 

print("<table class='altrowstable' id='alternatecolor' rules=all border=1>");

HTML("<caption>list of subjects (%s) on anonymization system</caption>", #trials)

print("<tr><td>Subject id<td>#studies<td>#series<td>#objects</tr>"); 

for i=1,#trials do
  t = string.format("<a href=?%s&mode=wadostudyviewer&study=%s:&anonymize=%s|%s title='click to see trial patients'>%s</a>", 
				  '', changeuidback_(trials[i],CGI('trial'), 'PatientID'), trials[i],CGI('trial'),trials[i]);
  s = string.format("<tr><td>%s<td>%s<td>%s<td>%s</tr>",t,counttrialstudies(trials[i],CGI('trial')),counttrialseries(trials[i],CGI('trial')),counttrialobjects(trials[i],CGI('trial')));
  print(s)
end 

print
[[
</table>
</body>
</html>
]]
