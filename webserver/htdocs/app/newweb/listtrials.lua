-- mvh 20160124: created
-- mvh 20200112: Skip (empty) as possible trial name, count all others
-- 20220827   mvh   Made dgate extension more generic, allows deployment as app
-- 20230625   mvh   Made all links relative

local query_pid = '';
local query_pna = '';
local query_pst = '';
local query_sta = '';
 
function querytrials()
  local list = {}
  for i=1, 1000 do
    local name = 
      servercommand('lua:'..
	  [[
	  local a=dbquery('UIDMods', 'DISTINCT Stage', "Stage LIKE ']].. CGI('trialmatch')..'%'..[['");
	  if a[ ]]..i..[[ ] then return a[ ]]..i..[[ ][1] else return '' end
	  ]]
                   ) or ''
    if name~='' and name~='(empty)' then
      list[i] = name
    else
      break
    end
  end
  return list
end;

function counttrialpatients(stage)
  local count = 
      servercommand('lua:'..
	  [[
	  local a=dbquery('UIDMods', 'DISTINCT NewUID', "MODType='PatientID' and Stage=']]..stage..[['");
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


local trials=querytrials() 

print("<table class='altrowstable' id='alternatecolor' rules=all border=1>");

HTML("<caption>list of all trials (%s) on anonymization system</caption>", #trials)

print("<tr><td>trials id<td>patients</tr>"); 

for i=1,#trials do
  t = string.format("<a href=?%s&mode=listtrialpatients&key=%s&trial=%s title='click to see trial patients'>%s</a>", '', tostring(key or ''),trials[i],trials[i]);
  if trials[i]~='(empty)' then
    s = string.format("<tr><td>%s<td>%s</tr>",t,counttrialpatients(trials[i]));
    print(s)
  end
end 

print
[[
</table>
</body>
</html>
]]
