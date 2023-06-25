-- mvh 20151206: fix 1.4.17d compatibility
-- 20160709   mvh   fix for PatientID with spaces in it
-- 20170305   mvh   start with send etc functions (1.4.19)
-- 20170306   mvh   Implement delete, send, anonymize and changeid; no progress yet
-- 20170307   mvh   More work on commands; zipper requires dgate source code change
-- 20170430   mvh   Zipper applied cgibinary mode in 1.4.19a; viewers work in separate windows
-- 20180203   mvh   Adapt zipper for Ladle mode (if write); removed zip.cq call
-- 20180203   mvh   Try popup window for series and study viewer; provide filename for zippers
-- 20181124   mvh   Added readOnly (no modify/delete) and viewOnly (no download) flags, pass compress to viewers
-- 20181215   mvh   Added remotequery and remote get_amap to depend less on web cgi functionality
-- 20181230   mvh   Removed dicomquery, only kept remotequery
-- 20190112   mvh   Use | to separate items to help with special characters in patientID
-- 20200112   mvh   Added optional stage to zipanonymized
-- 20200113   mvh   Small fix in that
-- 20200307   mvh   Avoid query with '***'
-- 20200618   mvh   Fixed hardcoded CONQUESTSRV1 in 'deleter'
-- 20220827   mvh   Made dgate extension more generic, allows deployment as app
-- 20220830   mvh   Fix proposed patientID; included uids after |
-- 20230625   mvh   Made all links relative

version = version  or ''

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
  query_pid='' 
 end

 if (patientnamematch ~= '' and CGI('patientnamematch') ~='*') then
     query_pna='*'..CGI('patientnamematch')..'*' ;
 else 
  query_pna='' 
 end

 if (CGI('studydatematch') ~= '' and CGI('studydatematch') ~='*') then
     query_pst='*'..CGI('studydatematch')..'*' ;
 else
   query_pst='' 
 end

 if (CGI('startdatematch') ~= '' and CGI('startdatematch') ~='*') then
     query_sta='*'..CGI('startdatematch')..'*' ;
 else 
  query_sta='' 
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

function querypats_remote()
  InitializeVar()
  local s = servercommand('get_param:MyACRNema')
  local b=DicomObject:new();
  b.QueryRetrieveLevel='PATIENT'
  b.PatientID        = query_pid;
  b.PatientName      = query_pna;
  b.PatientSex = '';
  b.PatientBirthDate = '';
  local patist=remotequery(s, 'PATIENT', b);
  return patist
end;

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

if CGI('parameter', '')~='zip' and CGI('parameter', '')~='zipanonymized' then
  HTML("Content-type: text/html\nCache-Control: no-cache\n")
end

----------------------------------------
-- Subfunctions for sending etc
----------------------------------------

local readOnly = gpps('webdefaults', 'readOnly', '0')~='0'
local viewOnly = gpps('webdefaults', 'viewOnly', '0')~='0'

if CGI('parameter', '')=='nop' then
  print('<i>no action taken</i>')
  return
end  

if CGI('parameter', '')=='sender' then
  print('Send '..CGI('item', '') .. ' to destination: ')
  for i=0,99 do
    -- a = get_amap(i)
    a = servercommand('lua: return get_amap('..i..')')
    if a==nil then break end
    if string.match(a, '*') then break end
    print('<br>'..a)
    print([[<a href=# onclick="servicecommand('send&item=]]..CGI('item')..[[&Destination=]]..a..[[')">Go</a>]])
  end
  print([[<br><a href=# onclick="servicecommand('nop')">Cancel</a>]])
  return
end  
if CGI('parameter', '')=='send' then
  local items= split(CGI('item'), '|')
  local script = string.format('%s,%s,%s,%s,%s,%s',servercommand('get_param:MyACRNema'),CGI('Destination'),
    items[1],items[2] or '',items[3] or '',items[4] or '')
  servercommand('luastart:servercommand[[move:'..script..']]')
  return
end

if CGI('parameter', '')=='anonymizer' then
  print('New ID for anonymized: '..CGI('item', '').. ' ')
  print('<input id=newid value="'..string.gsub(CGI('item', ''), '|.+', '').. '">')
  if (readOnly) then 
    print([[<a href=# onclick="servicecommand('nop&item=]]..CGI('item')..[[&newid='+document.getElementById('newid').value)">Not allowed</a><br>]])
  else
    print([[<a href=# onclick="servicecommand('anonymize&item=]]..CGI('item')..[[&newid='+document.getElementById('newid').value)">Go</a><br>]])
  end
  print([[<a href=# onclick="servicecommand('nop')">Cancel</a><br>]])
  return
end  
if CGI('parameter', '')=='anonymize' then
  local items= split(CGI('item'), '|')
  local script = string.format('%s,%s,%s,%s,1,lua/anonymize_script.lua(%s)',
    items[1],items[2] or '',items[3] or '',items[4] or '', CGI('newid'))
  servercommand('luastart:servercommand[[modifier:'..script..']]');
  return
end

if CGI('parameter', '')=='changerid' then
  print('New patient ID for: '..CGI('item', '').. ' ')
  print('<input id=newid value="'..string.gsub(CGI('item', ''), '|.+', '').. '">')
  if (readOnly) then 
    print([[<a href=# onclick="servicecommand('nop&item=]]..CGI('item')..[[&newid='+document.getElementById('newid').value)">Not allowed</a><br>]])
  else
    print([[<a href=# onclick="servicecommand('changeid&item=]]..CGI('item')..[[&newid='+document.getElementById('newid').value)">Go</a><br>]])
  end
  print([[<a href=# onclick="servicecommand('nop')">Cancel</a><br>]])
  return
end  
if CGI('parameter', '')=='changeid' then
  local items= split(CGI('item'), '|')
  local script = string.format([[%s,%s,%s,%s,1,lua "script('newuids');Data.PatientID='%s'"]],
    items[1],items[2] or '',items[3] or '',items[4] or '',CGI('newid'))
  servercommand('luastart:servercommand[[modifier:'..script..']]');
  return
end

if CGI('parameter', '')=='deleter' then
  local items= split(CGI('item'), '|')
  local n = 
    servercommand('lua:s=newdicomobject();s.PatientID=[['..(items[1] or '')..
                ']];s.StudyInstanceUID="'..(items[2] or '')..
		'";s.SeriesInstanceUID="'..(items[3] or '')..
		'";s.SOPInstanceUID="'..(items[4] or '')..
		'";s.QueryRetrieveLevel="IMAGE'..
		'";a=dicomquery("'..servercommand('get_param:MyACRNema')..'", "IMAGE", s);return #a')
  print('really delete: '..CGI('item', '') .. ' with '..n..' images ? ')
  if (readOnly) then 
    print([[<a href=# onclick="servicecommand('nop&item=]]..CGI('item')..[[')">Not allowed</a><br>]]) 
  else
    print([[<a href=# onclick="servicecommand('delete&item=]]..CGI('item')..[[')">Go</a><br>]]) 
  end
  print([[<a href=# onclick="servicecommand('nop')">Cancel</a><br>]])
  return
end  
if CGI('parameter', '')=='delete' then
  local items= split(CGI('item'), '|')
  servercommand('lua:s=newdicomobject();s.PatientID=[['..(items[1] or '')..
                ']];s.StudyInstanceUID="'..(items[2] or '')..
		'";s.SeriesInstanceUID="'..(items[3] or '')..
		'";s.SOPInstanceUID="'..(items[4] or '')..
		'";dicomdelete(s)')
  print("Deleted " .. CGI('item'))
  return
end

if CGI('parameter', '')=='zipperanonymized' then
  print('Zip anonymized for: '..CGI('item', '').. ' ')
  print('<input id=newid value="'..string.gsub(CGI('item', ''), '|.+', '').. '">')
  if (viewOnly) then 
    print([[<a href=# onclick="servicecommand('nop&item=]]..CGI('item')..[[&newid='+document.getElementById('newid').value+'&dum=.zip')">Not allowed</a><br>]])
  else
    print([[<a href=# onclick="opencommand('zipanonymized&item=]]..CGI('item')..[[&newid='+document.getElementById('newid').value+'&dum=.zip')">Go</a><br>]])
  end
  print([[<a href=# onclick="servicecommand('nop')">Cancel</a><br>]])
  return
end  
if CGI('parameter', '')=='zipanonymized' then
  local items= split(CGI('item'), '|')
  local stage=''
  if CGI('stage')~='' then stage = '|' .. CGI('stage') end
  if write then
    local tempname = 'z.zip'
    local script=string.format('%s,%s,%s,%s,%s,lua/anonymize_script.lua(%s%s)', items[1] or '', items[2] or '', items[3] or '', items[4] or '', tempname, CGI('newid'), stage)
    newdicomobject():Script('rm '..tempname)
    servercommand([[export:]]..script)
    write('HTTP/1.1 200/OK\r\nServer: Ladle\r\nContent-type: application/zip\r\nContent-Disposition: attachment; filename="'..CGI('newid')..'.zip"\r\n\r\n')
    local f=io.open(tempname, 'rb')
    if f then
      write(f:read('*all'))
      f:close()
    end
  else
    local script=string.format('%s,%s,%s,%s,cgi,lua/anonymize_script.lua(%s%s)', items[1] or '', items[2] or '', items[3] or '', items[4] or '', CGI('newid'), stage)
    servercommand([[export:]]..script, 'cgibinary')
  end
  return
end

if CGI('parameter', '')=='zipper' then
  print('Zip: '..CGI('item', '').. ' ')
  if (viewOnly) then 
    print([[<a href=# onclick="servicecommand('nop&item=]]..CGI('item')..[[&dum=.zip')">Not allowed</a><br>]])
  else
    print([[<a href=# onclick="opencommand('zip&item=]]..CGI('item')..[[&dum=.zip')">Go</a><br>]])
  end
  print([[<a href=# onclick="servicecommand('nop')">Cancel</a><br>]])
  return
end  
if CGI('parameter', '')=='zip' then
  local items= split(CGI('item'), '|')
  if write then
    local tempname = 'z.zip'
    local script=string.format('%s,%s,%s,%s,%s', items[1] or '', items[2] or '', items[3] or '', items[4] or '', tempname)
    newdicomobject():Script('rm '..tempname)
    servercommand([[export:]]..script)
    write('HTTP/1.1 200/OK\r\nServer: Ladle\r\nContent-type: application/zip\r\nContent-Disposition: attachment; filename="'..items[1]..'.zip"\r\n\r\n')
    local f=io.open(tempname, 'rb')
    if f then
      write(f:read('*all'))
      f:close()
    end
  else
    local script=string.format('%s,%s,%s,%s,cgi', items[1] or '', items[2] or '', items[3] or '', items[4] or '')
    servercommand([[export:]]..script, 'cgibinary')
  end
  return
end  

if CGI('parameter', '')=='viewerstudy' then
  local studyviewer=gpps('webdefaults', 'studyviewer', '');
  local compress=gpps('webdefaults', 'compress', '');
  local items= split(CGI('item'), '|')
  local u = string.format("?level=Study&mode=%s&study=%s:%s&compress=%s", studyviewer, string.gsub(items[1], ' ', '+'),items[2],compress); 
  print([[<a href=# onclick="window.open(']]..u..[[', 'title', 'toolbar=no')">Open study viewer</a><br>]])
  print([[<img src="data:image/gif;base64,R0lGODlhAQABAIAAAAAAAP///yH5BAEAAAAALAAAAAABAAEAAAIBRAA7" onload="window.open(']]..u..[[', 'title', 'toolbar=no');" />]])
  return
end  

if CGI('parameter', '')=='viewerseries' then
  local viewer=gpps('webdefaults', 'viewer', '');
  local compress=gpps('webdefaults', 'compress', '');
  local items= split(CGI('item'), '|')
  local u = string.format("?level=Series&mode=%s&series=%s:%s&compress=%s", viewer, string.gsub(items[1], ' ', '+'),items[3],compress); 
  print([[<a href=# onclick="window.open(']]..u..[[', 'title', 'toolbar=no')">Open series viewer</a><br>]])
  print([[<img src="data:image/gif;base64,R0lGODlhAQABAIAAAAAAAP///yH5BAEAAAAALAAAAAABAAEAAAIBRAA7" onload="window.open(']]..u..[[', 'title', 'toolbar=no');" />]])
  return
end  

----------------------------------------

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
function progressivecommand(a) {
  xmlhttp = new XMLHttpRequest(); 
  xmlhttp.open('GET',']]..[[?mode=listpatients&parameter='+a, true);
  xmlhttp.timeout = 60000
  xmlhttp.send()
  xmlhttp.onreadystatechange = function() {
    if (this.readyState > 2) {
      var s = this.responseText.toString()
      document.getElementById("statusarea").innerHTML = s;
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

HTML("<H1>Welcome to Conquest DICOM server - version %s</H1>", version)

local pats=querypats_remote() 
if query_pna ~= '' then
  table.sort(pats, mycomp)
end

print("<table class='altrowstable' id='alternatecolor' RULES=ALL BORDER=1>");

if (query ~= '') then  
	HTML("<Caption>List of selected patients (%s) on local server</caption>", #pats)
else   
	HTML("<Caption>List of all patients (%s) on local server</caption>", #pats)
end

print("<TR><TD>Patient ID<TD>Name<TD>Sex<TD>Birth Date<TD>Menu</TR>"); 

function dropdown(i, item)
  return string.format([[
<td>
<p id='aap%d' onmouseover="var s=document.getElementById('aap%d').children; s[0].style.opacity=1" onmouseout="var s=document.getElementById('aap%d').children; s[0].style.opacity=0.1">
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

for i=1,#pats do
  t = string.format("<A HREF=?%s&mode=liststudies&key=%s&query=patientid+=+'%s' title='Click to see studies'>%s</A>", '', tostring(key or ''),string.gsub(pats[i].PatientID, ' ', '+'),mc(pats[i].PatientID));
  s = string.format("<TR><TD>%s<TD>%s<TD>%s<TD>%s%s</TR>",t,mc(pats[i].PatientName),mc(pats[i].PatientSex),mc(pats[i].PatientBirthDate), dropdown(i, pats[i].PatientID));
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
