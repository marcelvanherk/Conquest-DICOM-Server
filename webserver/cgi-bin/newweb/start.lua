-- mvh 20151206: fix 1.4.17d compatibility
-- mvh 20160124: added anonymizer trial viewer; enabled after any named anonymizing
-- mvh 20160709: fix error message if AE not in acrnema.map
-- mvh 20170211: Protect counttrials against missing UIDmods table
-- mvh 20180204: Some cleanup, report when ladle is used
-- mvh 20181111: Added missing Global.WebCodeBase for conquest icon
-- mvh 20181230: Removed no longer needed acrnema.map test, all access is remote
-- mvh 20191017: Added submit button to upload file
-- mvh 20200113: Skip '(empty)' and '' as trial name count all others in counttrials
-- mvh 20200115: Added dbquery 
-- mvh 20200117: Added dbqueryluaformat, dicomquery, dicomqueryluaformat
-- mvh 20200119: dicomquery returns json if dgate supports Serialize(true)
-- mvh 20200125: Added dicommove
-- mvh 20200126: Added sql and servercommand; but disable when readOnly set
-- mvh 20200214: Removes os. call for ladle
-- mvh 20200301: Added swupdate command; note requires 777 access /home/tmp in linux
-- mvh 20200302: Other test for linux, use os.getenv('SCRIPT_FILENAME'); directly copy to web 
-- mvh 20200303: Added serverpage command: writes without header; uploadsql
-- mvh 20200305: Added project parameter to swupdate; added dbquerysqldownload
-- mvh 20200307: Use tempdir from dicom.ini
-- mvh 20200308: Note: NEVER use tempfile call unless in dicom server!, Fixed dbquerysqldownload
--               Fix UIDMODS case

webscriptaddress = webscriptaddress or webscriptadress or 'dgate.exe'
local ex = string.match(webscriptaddress, 'dgate(.*)')
local readOnly = gpps('webdefaults', 'readOnly', '0')~='0'

function tempfile()
  error('tempfile not supported in CGI')
end

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

-- report error string on a otherwise empty reddish page
function errorpage(s)
  HTML('Content-type: text/html\n\n');
  print(
  [[
  <HEAD><TITLE>Version ]].. (version or '-')..[[ WEB interface</TITLE></HEAD>
  <BODY BGCOLOR='FFDFCF'>
  <H3>Conquest ]].. (version or '-') ..[[ WEB interface</H3>
  <H3>]].. s .. [[</H3>
  </BODY>
  ]])
end

local s    = servercommand('get_param:MyACRNema')
local port = gpps('sscscp', 'TCPPort', '');
local ip   = gpps('sscscp', 'WebServerFor', '')
local found= false

function counttrials()
  local list = {}
  local count = 
      servercommand('lua:'..
	  [[
	  local a=dbquery('UIDMODS', 'DISTINCT Stage') or {}
	  local count=0
	  for i=1, #a do
            if a[i][1]~='' and a[i][1]~='(empty)' then count=count+1 end
	  end
	  return count
	  ]]
                   ) or 0
  return tonumber(count)
end;

function remotedbquery(tab, fields, q, inluaformat)
  local remotecode =
[[
  local tab="]]..tab..[[";
  local fields="]]..fields..[[";
  local q="]]..q..[[";
  local r = dbquery(tab, fields, q)
  if r[1]==null then return "{}" end
  for i=1,#r do 
    r[i] = '{"'..table.concat(r[i], '", "')..'"}'
  end
  r='{'..table.concat(r, ',')..'}'
  local s=tempfile('.txt') local f=io.open(s, "wb") f:write(r) returnfile=s f:close();
]]
  if inluaformat==nil then
    local f = loadstring('return '..servercommand('lua:'..remotecode));
    if f then return f() end
  else
    return servercommand('lua:'..remotecode)
  end
end

function table_print (tt)
  if type(tt) == "table" then
    local sb = {'{'}
    for key, value in pairs (tt) do
      if type (value) == "table" then
        table.insert(sb, key .. "={");
        table.insert(sb, table_print (value))
        table.insert(sb, "}");
      elseif "number" == type(key) then
        table.insert(sb, string.format("\"%s\"", tostring(value)))
      elseif "number" == type(value) then
        table.insert(sb, string.format(
            "%s = %s", tostring (key), tostring(value)))
      else
        table.insert(sb, string.format(
            "%s=\"%s\"", tostring (key), tostring(value)))
      end
      table.insert(sb, ",");
    end
    table.insert(sb, "}");
    return table.concat(sb)
  else
    return tt
  end
end

function remotequery(ae, level, q, jsn)
  local remotecode =
[[
  local ae=']]..ae..[[';
  local level=']]..level..[[';
  local q=]]..table_print(q)..[[;
  local jsn=]]..tostring(jsn or false)..[[;
  local q2=DicomObject:new(); for k,v in pairs(q) do q2[k]=v end;
  local r = dicomquery(ae, level, q2):Serialize(jsn);
  local s=tempfile('txt') local f=io.open(s, "wb") f:write(r) returnfile=s f:close();
]]
  return servercommand('lua:'..remotecode)
end

function remotemove(from, to, q, xtra)
  if extra=='' then extra='{}' end
  local remotecode =
[[
  local from=']]..from..[[';
  local to=']]..to..[[';
  local q=]]..table_print(q)..[[;
  local extra=]]..table_print(xtra)..[[;
  local q2=DicomObject:new(); for k,v in pairs(q) do q2[k]=v end;
  local extra2=DicomObject:new(); for k,v in pairs(extra) do extra2[k]=v end;
  return dicommove(from, to, q2, 0, extra2);
]]
  return servercommand('lua:'..remotecode)
end

function remotesql(q)
  return servercommand('lua:sql([['..q..']])')
end

function fileexists(f)
  return servercommand('lua:local h=io.open([['..f..']], [[r]]);'..
  'if h then h:close() return 1 else return 0 end')=="1" and true or false
end

function copyfile(f, g)
    servercommand('lua:'..
    'local h=io.open([['..f..']], [[rb]]) '..
    'local s=h:read([[*a]]) '..
    'h:close() '..
    'local j=io.open([['..g..']], [[wb]]) '..
    'j:write(s) '..
    'j:close() '..
    'print([[copied from '..f..']]) '..
    'print([[copied to '..g..']]) '..
    '')
    if fileexists(f)==false then error('nothing to copy') end
    if fileexists(g)==false then error('failed copy '..f..' to '..g) end
end

if CGI('parameter')=='test' then
  HTML('Content-type: application/json\n\n');
  print(servercommand('get_param:MyACRNema'))
  return
end
if CGI('parameter')=='countcases' then
  HTML('Content-type: application/json\n\n');
  print(#remotedbquery("DICOMPatients","PatientID","PatientID like '".. CGI('study').."%'"))
  return
end
if CGI('parameter')=='dbquery' then
  HTML('Content-type: application/json\n\n')
  local maxresult=tonumber(CGI('maxresults', '9999999'))
  local r=remotedbquery(CGI('table'),CGI('fields'),CGI('query'))
  io.write('[')
  for k, v in ipairs(r) do 
    if k>maxresult then break end
    r[k] = '["'..table.concat(v, '","')..'"]'
  end
  io.write(table.concat(r, ','))
  io.write(']')
  return
end
if CGI('parameter')=='dbqueryluaformat' then
  HTML('Content-type: application/text\n\n')
  local r=remotedbquery(CGI('table'),CGI('fields'),CGI('query'))
  io.write(table_print(r))
  return
end
if CGI('parameter')=='dbquerysqldownload' then
  io.write('Content-type: application/text\nContent-Disposition: attachment;filename="'..
    CGI('filename', 'dbquery.sql')..'"\n\n');
  local r=remotedbquery(CGI('table'),CGI('fields'),CGI('query'))
  for k,v in ipairs(r) do 
    io.write([[insert into ]]..CGI('table')..[[(]]..CGI('fields')..[[) value (']]..
      table.concat(v,[[',']]).."');\n");
  end
end  
if CGI('parameter')=='dicomquery' then
  HTML('Content-type: application/json\n\n')
  local r=remotequery(CGI('AE'),CGI('level'),CGI('query'),true)
  io.write(r)
  return
end
if CGI('parameter')=='dicomqueryluaformat' then
  HTML('Content-type: application/text\n\n')
  local r=remotequery(CGI('AE'),CGI('level'),CGI('query'))
  io.write(r)
  return
end
if CGI('parameter')=='dicommove' then
  HTML('Content-type: application/json\n\n')
  local r=remotemove(CGI('from'),CGI('to'),CGI('query'),CGI('xtra'))
  io.write('"'..(r or '')..'"')
  return
end
if CGI('parameter')=='sql' then
  HTML('Content-type: application/json\n\n')
  io.write(remotesql(CGI('query')))
  return
end
if CGI('parameter')=='servercommand' then
  HTML('Content-type: application/json\n\n')
  if not readOnly then io.write(servercommand(CGI('command'))) end
  return
end
if CGI('parameter')=='serverpage' then
  if not readOnly then io.write(servercommand(CGI('command'))) end
  return
end
if CGI('parameter')=='swupdate' then
  HTML('Content-type: application/json\n\n')
  local fn, n, ds, web, cgi, target = '', '', '/'
  fn = CGI('filename', 'x.x')
  -- if true or os.rename('c:\\temp\\', 'c:\\temp\\')==true then
  if string.find(os.getenv('SCRIPT_FILENAME'), ':') then
    n='c:\\temp\\'..fn
    ds = '\\'
    web = 'c:\\xampp\\htdocs\\'
    cgi = 'c:\\xampp\\cgi-bin\\'
  else
    n=gpps('sscscp', 'tempdir', '/home/tmp')..ds..fn
    web = '/var/www/html/'
    cgi = '/usr/lib/cgi-bin/'
  end
  local f=io.open(n, 'wb')
  f:write(CGI())
  f:close()
  local g = servercommand('lua:return Global.basedir')
  project = CGI('project', 'inholland')
  
  -- Distribute file to one of these folders:
  -- try htdocs/project
  -- try htdocs/js/project
  -- try cgi-bin/project
  -- try server/lua/project
  -- try server/lua
  -- try server/src/dgate/src
  -- remaining lua into server/lua
  -- remaining wlua into server/lua
  -- remaining html into htdocs/project
  -- remaining js into htdocs/project/js
    
  if fileexists(    g..'webserver'..ds..'htdocs'..ds..project..ds..fn)==true then 
    copyfile(n,     g..'webserver'..ds..'htdocs'..ds..project..ds..fn) 
    copyfile(n,                                  web..project..ds..fn) 
    target = web..'inholland'..ds..fn
  elseif fileexists(g..'webserver'..ds..'htdocs'..ds..project..ds..'js'..ds..fn)==true then 
    copyfile(n,     g..'webserver'..ds..'htdocs'..ds..project..ds..'js'..ds..fn) 
    copyfile(n,                                  web..project..ds..'js'..ds..fn) 
    target = web..'inholland'..ds..'js'..ds..fn
  elseif fileexists(g..'webserver'..ds..'cgi-bin'..ds..project..ds..fn)==true then 
    copyfile(n,     g..'webserver'..ds..'cgi-bin'..ds..project..ds..fn) 
    copyfile(n,                                   cgi..project..ds..fn)
    target = cgi..'newweb'..ds..fn
  elseif fileexists(g..'webserver'..ds..'cgi-bin'..ds..'newweb'..ds..fn)==true then 
    copyfile(n,     g..'webserver'..ds..'cgi-bin'..ds..'newweb'..ds..fn) 
    copyfile(n,                                   cgi..'newweb'..ds..fn)
    target = cgi..'newweb'..ds..fn
  elseif fileexists(g..'lua'..ds..project..ds..fn)==true then 
    copyfile(n,     g..'lua'..ds..project..ds..fn) 
    target = g..'lua'..ds..fn
  elseif fileexists(g..'lua'..ds..fn)==true then 
    copyfile(n,     g..'lua'..ds..fn) 
    target = g..'lua'..ds..fn
  elseif fileexists(g..'src'..ds..'dgate'..ds..'src'..ds..fn)==true then 
    copyfile(n,     g..'src'..ds..'dgate'..ds..'src'..ds..fn) 
    target = g..'src'..ds..'dgate'..ds..'src'..ds..fn
  elseif string.find(fn, '.lua') then 
    copyfile(n, g..'lua'..ds..fn) 
    target = g..'lua'..ds..fn
  elseif string.find(fn, '.wlua') then 
    copyfile(n, g..'lua'..ds..fn) 
    target = g..'lua'..ds..fn
  elseif string.find(fn, '.html') then 
    copyfile(n, g..'webserver'..ds..'htdocs'..ds..project..ds..fn) 
    copyfile(n,                              web..project..ds..fn) 
    target = web..'inholland'..ds..fn
  elseif string.find(fn, '.js') then 
    copyfile(n, g..'webserver'..ds..'htdocs'..ds..project..ds..'js'..ds..fn) 
    copyfile(n,                              cgi..project..ds..'js'..de..fn) 
    target = cgi..'inholland'..ds..'js'..ds..fn
  else
    target = '** NOT FOUND **'
  end
  os.remove(n)
  if fileexists(target) then
    HTML('Update succeeded of\n\n'..fn..'\n\n--->\n\n'..target)
  else
    HTML('Update **FAILED** of\n\n'..fn..'\n\n--->\n\n'..target)
  end
  return
end

if CGI('parameter')=='uploadsql' then
  local n
  local fn = CGI('filename', 'x.x')
  HTML('Content-type: application/json\n\n')
  if string.find(os.getenv('SCRIPT_FILENAME'), ':') then
    n='c:\\temp\\'..fn
  else
    n=gpps('sscscp', 'tempdir', '/home/tmp')..'/'..fn
  end
  local f=io.open(n, 'wb')
  f:write(CGI())
  f:close()
  if CGI('ref')~='' then
    servercommand("lua:sql([[delete from UIDMODS where Stage like '"..CGI('ref').."%']])")
  end
  local g = servercommand('lua:h=io.open([['..n..']], "r") t=h:read([[*all]]) h:close() return sql(t)')
  print(g)
  os.remove(n)
  return
end

if s==nil then
  errorpage('server not running')
  return
end

local a = newdicomobject()
a.InstanceNumber = 'test'
if a.InstanceNumber ~= 'test' then
  errorpage('Web server error: old dgate.dic used')
  return
end

HTML("Content-type: text/html\nCache-Control: no-cache\n");
HTML("<HEAD>")
print [[<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">]]
HTML("<TITLE>Conquest DICOM server - version %s</TITLE>", version)
	
print([[<style type="text/css">
	body { font:10pt Verdana; }
	a { color:blue; }
	#content { background-color:#dddddd; width:200px; margin-top:2px; }
	h1 {background-color:#6495ed;}
	</style>]])

HTML("</HEAD>")
		 		
HTML("<BODY BGCOLOR='CFDFCF'>")


HTML("<H1>Welcome to Conquest DICOM server - version %s</H1>", version)
if write then 
  HTML("<H2>Written by Conquest Project - Running on Ladle</H2>")
else 
  HTML("<H2>Written by Conquest Project</H2>") 
end
HTML("<IMG SRC='%sconquest.jpg' ALT='Written by Conquest Project'>", Global.WebCodeBase)
HTML("<HR>")

HTML("<PRE>")
HTML(servercommand('display_status:'))
HTML("</PRE>")

HTML("<HR>");

HTML("<table>");
key=''  	
HTML("<tr>");
HTML("<FORM ACTION=\"dgate%s\"   onSubmit=\"if (this.patientnamematch.value == '' && this.patientidmatch.value == '') {alert('Please, fill the entry fields');return false;}\">", ex);
HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=listpatients>");
HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", port);
HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", address);
HTML("<INPUT NAME=key     TYPE=HIDDEN VALUE=%s>", key);
HTML("<td>Local Patient List");
HTML("<td>Patient ID: <INPUT NAME=patientidmatch TYPE=Text VALUE=>");
HTML("<td>Name: <INPUT NAME=patientnamematch TYPE=Text style=\"text-transform:uppercase;\" onkeyup=\"javascript:this.value=this.value.toUpperCase();\" VALUE=>");
HTML("<td>");
HTML("<td><INPUT TYPE=SUBMIT VALUE=Submit>");
HTML("</FORM>");
HTML("</tr>");	

HTML("<tr>");
HTML("<FORM ACTION=\"dgate%s\" onSubmit=\"if (this.patientnamematch.value == '' && this.patientidmatch.value == '' && this.studydatematch.value == '') {alert('Please, fill the entry fields');return false;}\">", ex);
HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=liststudies>");
HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", port);
HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", address);
HTML("<INPUT NAME=key     TYPE=HIDDEN VALUE=%s>", key);
HTML("<td>Local Studies List");
HTML("<td>Patient ID: <INPUT NAME=patientidmatch TYPE=Text VALUE=>");
HTML("<td>Name: <INPUT NAME=patientnamematch TYPE=Text style=\"text-transform:uppercase;\" onkeyup=\"javascript:this.value=this.value.toUpperCase();\" VALUE=>");
HTML("<td>Date: <INPUT NAME=studydatematch id=studydatematch TYPE=Text VALUE=>");
HTML("<td><INPUT TYPE=SUBMIT VALUE=Submit>");
HTML("<tr><td><td><td><td>")
HTML([[<INPUT TYPE=BUTTON VALUE=Today onClick="var d=new Date(); document.getElementById('studydatematch').value=(d.getFullYear()).toString()+(d.getMonth()+101).toString().substring(1,3)+(d.getDate()+100).toString().substring(1,3)">]]);
HTML([[<INPUT TYPE=BUTTON VALUE=Yesterday onClick="var d=new Date(); d=new Date(d.valueOf()-864E5); document.getElementById('studydatematch').value=(d.getFullYear()).toString()+(d.getMonth()+101).toString().substring(1,3)+(d.getDate()+100).toString().substring(1,3)">]]);
HTML([[<INPUT TYPE=BUTTON VALUE='Last 7 days' onClick="var d=new Date(); var d1=new Date(d.valueOf()-7*864E5); document.getElementById('studydatematch').value=(d1.getFullYear()).toString()+(d1.getMonth()+101).toString().substring(1,3)+(d1.getDate()+100).toString().substring(1,3)+'-'+(d.getFullYear()).toString()+(d.getMonth()+101).toString().substring(1,3)+(d.getDate()+100).toString().substring(1,3)">]]);
HTML("</FORM>");
HTML("</tr>");	

if counttrials()>0 then
  HTML("<tr>");
  HTML("<FORM ACTION=\"dgate%s\">", ex);
  HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=listtrials>");
  HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", port);
  HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", address);
  HTML("<INPUT NAME=key     TYPE=HIDDEN VALUE=%s>", key);
  HTML("<td>Local Trial List");
  HTML("<td>Trial ID: <INPUT NAME=trialmatch TYPE=Text VALUE=>");
  HTML("<td>");
  HTML("<td>");
  HTML("<td><INPUT TYPE=SUBMIT VALUE=Submit>");
  HTML("</FORM>");
  HTML("</tr>");	
end

HTML("</table>");

HTML("<FORM ACTION=\"dgate%s\" METHOD=POST ENCTYPE=\"multipart/form-data\">", ex);
HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=addlocalfile>");
HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", port);
HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", address);
HTML("Upload file to enter into server (dcm/v2/HL7/zip/7z/gz/tar): <INPUT NAME=filetoupload SIZE=40 TYPE=file VALUE=>");
HTML("<INPUT TYPE=SUBMIT VALUE=Go>");
HTML("</FORM>");

HTML("</BODY>")
