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
-- mvh 20200310: Split uploadsql query into lines and then execute - for mysql compatibility
-- mvh 20200407: Added images target to updater
-- mvh 20200505: Added uploadtable command
-- mvh 20200514: Adjusted uploadtable for linux; backup old versions
-- mvh 20200914: Added downloadtable, requires JSON and csv
-- note: first time upload of images gives error in line 188
-- mvh 20200920: Downloadtable support .csv and .json files
-- mvh 20200921: Made upload backup switchable; avoid flickering mkdir if possible
-- mvh 20200922: Added uploadinfo; note uploadtable cannot handle big files
-- mvh+evo 20200925: Fix updater
-- mvh 20201003: Added uploadfile, uploads single file and runs any script; removed countcases
-- mvh 20201004: tidied swupdate; pass inholland always as 'project'
--               use Script(mkdir to avoid popups); removed use of c:\temp
-- mvh 20201012: Added zip and zipanonymized to reduce footprint of inholland project
-- mvh 20201024: Support _passfile_ to enable upload from php script; separate cgi-bin copy for web and dicom server
-- mvh 20201109: Use single command for uploadsql
-- mvh 20210204: Pass CGI('newname') to anonymize_script in zipanonymized
-- mvh 20210208: Fixed that
-- mvh 20220329: Upload now uses start/uploadfile command (works through php interface)
-- mvh 20220823: Added upload() function that communicates with server with a dicomecho
-- mvh 20220824: Hopefully finished upload stuff
-- mvh 20220827: Made dgate extension more generic, allows deployment as app; 
--               No longer accept cgi port and address, fix unused default in remotemove function
-- mvh 20220827: Escape \ to \\ in returned filename of upload; fix uploading even and odd data
-- mvh 20220828: Use global port and address, can come from command line
-- mvh 20220830: Popup and 'progress' bar for upload
-- mvh 20220831: Avoid overflow - don't use print() in downloadtable
-- mvh 20221012: storeclick stores link in clicks.txt
-- mvh 20230605: readOnly disables upload methods and post
-- mvh 20230625: Made all links relative

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
  if xtra=='' then xtra='{}' end
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

function safetempfile(ext)
  local f = servercommand('lua:return tempfile("' .. ext .. '")')
  if string.find(f, '\\') then return f end 
  return '/tmp/' .. math.floor(1000000*math.random()) .. ext
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

-- if filename given, save data there on server side
-- if script given run the script at the end
-- script can use filename, data, returnvalue
function upload(filename, data, script)
  local remotecode =
  [[local a =Command:GetVR(0x9999, 0x401, true)
    local p = string.find(a, '\n')
    local filename = string.sub(a, 2, p-1)
    local data
    if string.sub(a, 1, 1)=='0' then 
      data = string.sub(a, p+1) 
    else 
      data = string.sub(a, p+1, -2) 
    end
    if filename~='' then
      local f=io.open(filename, 'wb')
      f:write(data)
      f:close();
    end
    local __n=#data
  ]] .. script .. [[; Command:SetVR(0x9999,0x403,__n..' bytes of file '..tostring(returnvalue)) ]]
  local x=DicomObject:new()
  x:SetVR(0x9999, 0x400, 'lua:'..remotecode)
  local n=#filename+#data
  if n%2 == 0 then
    x:SetVR(0x9999, 0x401, '0' .. filename .. '\n' .. data)
  else
    x:SetVR(0x9999, 0x401, '1' .. filename .. '\n' .. data)
  end
  local a = dicomecho(address..':'..port, x)
  return string.format('"processed: %s (sent %d bytes)"', string.gsub(a:GetVR(0x9999,0x401,true) or '', [[\]], [[\\]]), #data)
end

if CGI('parameter')=='test' then
  HTML('Content-type: application/json\n\n');
  print(servercommand('get_param:MyACRNema'))
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
    io.write([[insert into ]]..CGI('table')..[[ (]]..CGI('fields')..[[) value (']]..
      table.concat(v,[[',']]).."');\n");
  end
  return
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

if CGI('parameter')=='swupdate' and not readOnly then
  HTML('Content-type: application/json\n\n')
  local n, ds, web, cgi = '', '/'
  local fn = CGI('filename', 'x.x')
  local project = CGI('project', 'inholland')
  local n = safetempfile(".sw")
  if CGI('_passfile_', '')~='' then
    n = CGI('_passfile_', '')
  else
    local f=io.open(n, 'wb')
    f:write(CGI())
    f:close()
  end
  if string.find(os.getenv('SCRIPT_FILENAME'), ':') then
    ds = '\\'
    web = 'c:\\xampp\\htdocs\\'
    cgi = 'c:\\xampp\\cgi-bin\\'
  else
    web = '/var/www/html/'
    cgi = '/usr/lib/cgi-bin/'
  end
  local g = servercommand('lua:return Global.basedir')
  local target = ''
  
  -- update existing files (can be copied to multiple locations, only one reported)
  -- Distribute file to one of these folders:
  -- try htdocs/project
  -- try htdocs/project/js
  -- try htdocs/project/images
  -- try cgi-bin/project
  -- try cgi-bin/newweb
  -- try server/lua/project
  -- try server/lua
  if fileexists(    g..'webserver'..ds..'htdocs'..ds..project..ds..fn)==true then 
    copyfile(n,     g..'webserver'..ds..'htdocs'..ds..project..ds..fn) 
    copyfile(n,                                  web..project..ds..fn) 
    target =                                     web..project..ds..fn
  end
  if fileexists(g..'webserver'..ds..'htdocs'..ds..project..ds..'js'..ds..fn)==true then 
    copyfile(n, g..'webserver'..ds..'htdocs'..ds..project..ds..'js'..ds..fn) 
    copyfile(n,                              web..project..ds..'js'..ds..fn) 
    target =                                 web..project..ds..'js'..ds..fn
  end  
  if fileexists(g..'webserver'..ds..'htdocs'..ds..project..ds..'images'..ds..fn)==true then 
    copyfile(n, g..'webserver'..ds..'htdocs'..ds..project..ds..'images'..ds..fn) 
    copyfile(n,                              web..project..ds..'images'..ds..fn) 
    target =                                 web..project..ds..'images'..ds..fn
  end
  if fileexists(g..'webserver'..ds..'cgi-bin'..ds..project..ds..fn)==true then 
    copyfile(n, g..'webserver'..ds..'cgi-bin'..ds..project..ds..fn) 
    target =    g..'webserver'..ds..'cgi-bin'..ds..project..ds..fn
  end
  if fileexists(g..'webserver'..ds..'cgi-bin'..ds..'newweb'..ds..fn)==true then 
    copyfile(n, g..'webserver'..ds..'cgi-bin'..ds..'newweb'..ds..fn) 
    target =    g..'webserver'..ds..'cgi-bin'..ds..'newweb'..ds..fn
  end
  if fileexists(cgi..project..ds..fn)==true then 
    copyfile(n, cgi..project..ds..fn)
    target =    cgi..project..ds..fn
  end
  if fileexists(cgi..'newweb'..ds..fn)==true then 
    copyfile(n, cgi..'newweb'..ds..fn)
    target =    cgi..'newweb'..ds..fn
  end
  if fileexists(g..'lua'..ds..project..ds..fn)==true then 
    copyfile(n, g..'lua'..ds..project..ds..fn) 
    target =    g..'lua'..ds..project..ds..fn
  end
  if fileexists(g..'lua'..ds..fn)==true then 
    copyfile(n, g..'lua'..ds..fn) 
    target =    g..'lua'..ds..fn
  end

  -- upload new files
  -- remaining lua into server/lua
  -- remaining wlua into server/lua
  -- remaining html into htdocs/project
  -- remaining css into htdocs/project/js
  -- remaining js into htdocs/project/js
  -- remaining jpg into htdocs/project/images
  -- remaining png into htdocs/project/images
  if target=='' then
    if string.find(fn, '.lua') then
      copyfile(n, g..'lua'..ds..fn) 
      target =    g..'lua'..ds..fn
    elseif string.find(fn, '.wlua') then 
      copyfile(n, g..'lua'..ds..fn) 
      target =    g..'lua'..ds..fn
    elseif string.find(fn, '.html') then 
      copyfile(n, g..'webserver'..ds..'htdocs'..ds..project..ds..fn) 
      copyfile(n,                              web..project..ds..fn) 
      target =                                 web..project..ds..fn
    elseif string.find(fn, '.js') then 
      copyfile(n, g..'webserver'..ds..'htdocs'..ds..project..ds..'js'..ds..fn) 
      copyfile(n,                              web..project..ds..'js'..ds..fn) 
      target =                                 web..project..ds..'js'..ds..fn
    elseif string.find(fn, '.css') then 
      copyfile(n, g..'webserver'..ds..'htdocs'..ds..project..ds..'js'..ds..fn) 
      target =    g..'webserver'..ds..'htdocs'..ds..project..ds..'js'..ds..fn
    elseif string.find(fn, '.png') then 
      copyfile(n, g..'webserver'..ds..'htdocs'..ds..project..ds..'images'..ds..fn) 
      target =    g..'webserver'..ds..'htdocs'..ds..project..ds..'images'..ds..fn
    elseif string.find(fn, '.jpg') then 
      copyfile(n, g..'webserver'..ds..'htdocs'..ds..project..ds..'images'..ds..fn) 
      target =    g..'webserver'..ds..'htdocs'..ds..project..ds..'images'..ds..fn
    else
      target = '** NOT FOUND **'
    end
  end

  os.remove(n)
  if fileexists(target) then
    HTML('Update succeeded of\n\n'..fn..'\n\n--->\n\n'..target)
  else
    HTML('Update **FAILED** of\n\n'..fn..'\n\n--->\n\n'..target)
  end
  return
end

if CGI('parameter')=='uploadsql' and not readOnly then
  if CGI('ref')~='' then
    servercommand("lua:sql([[delete from UIDMODS where Stage like '"..CGI('ref').."%']])")
    servercommand("lua:changeuid('')") -- clear UID cache
  end
  local fn = safetempfile(".sql")
  local a
  if CGI('_passfile_', '')~='' then
    fn = CGI('_passfile_', '')
    local f=io.open(fn, 'rb')
    a = f:read('*a')
    f:close()
  else
    a = CGI()
  end
  --for v in io.lines(fn) do
  --  n= n+tonumber(servercommand("lua:return sql([["..v.."]])") or 0)
  --end
  --print(n)
  --need setting max_allowed_packet=16M in my.ini
  HTML('Content-type: application/json\n\n')
  print (upload('', a, 'returnvalue = sql(data)'))
end

if CGI('parameter')=='uploadinfo' and not readOnly then
  local web
  local ds='/'
  local fn = safetempfile(".pdf")
  local a
  if CGI('_passfile_', '')~='' then
    fn = CGI('_passfile_', '')
    local f=io.open(fn, 'rb')
    a = f:read('*a')
    f:close()
  else
    a = CGI()
  end
  if string.find(os.getenv('SCRIPT_FILENAME'), ':') then
    ds = '\\'
    web = 'c:\\xampp\\htdocs\\'
  else
    web = '/var/www/html/'
  end
  local project = CGI('project', 'inholland')
  if not fileexists(web..project..ds.."info"..ds..CGI('filename')) then
    servercommand("lua:DicomObject:new():Script([[mkdir "..web..project..ds.."info]])")
  end
  copyfile(fn, web..project..ds..'info'..ds..CGI('filename')) 
  HTML('Content-type: application/json\n\n')
  print (upload(web..project..ds.."info"..ds..CGI('filename'), a, 'returnvalue=filename'))
end

if CGI('parameter')=='uploadtable' and not readOnly then
  local ds = '/'
  local g = servercommand('lua:return Global.basedir')
  local fn = safetempfile(".csv")
  local a
  if CGI('_passfile_', '')~='' then
    fn = CGI('_passfile_', '')
    local f=io.open(fn, 'rb')
    a = f:read('*a')
    f:close()
  else
    a = CGI()
  end
  if string.find(os.getenv('SCRIPT_FILENAME'), ':') then
    ds = '\\'
  end
  if not fileexists(g.."tables"..ds..CGI('filename')) then
    servercommand("lua:DicomObject:new():Script([[mkdir "..g.."tables]])")
  end
  if CGI('backup')~='false' and fileexists(g.."tables"..ds..CGI('filename')) then
    for i=1,1000 do
      if not fileexists(g.."tables"..ds..CGI('filename')..'.'..i) then
        copyfile(g.."tables"..ds..CGI('filename'), g.."tables"..ds..CGI('filename')..'.'..i)
        break
      end
    end
  end
  HTML('Content-type: application/json\n\n')
  print (upload(g.."tables"..ds..CGI('filename'), a, 'returnvalue=filename'))
end

if CGI('parameter')=='uploadfile' and not readOnly then
  local fn = safetempfile(".tmp")
  local a
  if CGI('_passfile_', '')~='' then
    fn = CGI('_passfile_', '')
    local f=io.open(fn, 'rb')
    a = f:read('*a')
    f:close()
  else
    a = CGI()
  end
  if string.byte(a, 1)==string.byte('P') and string.byte(a, 2)==string.byte('K') then fn = fn .. '.zip' end
  if string.byte(a, 1)==0 and string.byte(a, 2)==0 then fn = fn .. '.dcm' end
  local script = CGI('script', '')
  HTML('Content-type: application/json\n\n')
  print (upload(fn, a, CGI('script', '') .. ';os.remove(filename); returnvalue=filename'))
  return
end

if CGI('parameter')=='downloadtable' then
  local JSON = require('json')
  require('csv')
  local ds = '/'
  local g = servercommand('lua:return Global.basedir')
  local fn = CGI('filename', 'x.x')
  if string.find(os.getenv('SCRIPT_FILENAME'), ':') then
    ds = '\\'
  end
  HTML('Content-type: application/json\n\n')
  if fileexists(g..'tables'..ds..fn) then
    if string.find(fn, '.csv') or string.find(fn, '.CSV') then 
      list = readCSV(g..'tables'..ds..fn) 
      
--      		if (UTF8ToDB && (*sin&128)) 
--			{ 
--			*sout++=0xc2+(*sin>0xbf);
--			*sout++=(*sin&0x3f)+0x80;
--			}

    elseif string.find(fn, '.json') or string.find(fn, '.JSON') then 
      local f = io.open(g..'tables'..ds..fn, 'r')
      local s = f:read('*all')
      f:close()
      list = JSON:decode(s)
    end 
    io.write(JSON:encode(list))
  else
    print(0)
  end
  return
end

if CGI('parameter', '')=='zip' then
  local items= split(CGI('item'), '|')
  local script=string.format('%s,%s,%s,%s,cgi', items[1] or '', items[2] or '', items[3] or '', items[4] or '')
    servercommand([[export:]]..script, 'cgibinary')
  return
end  

if CGI('parameter', '')=='zipanonymized' then
  local items= split(CGI('item'), '|')
  local stage=''
  if CGI('stage')~='' then stage = stage .. '|' .. CGI('stage') end
  if CGI('newname')~='' then stage = stage .. '|' .. CGI('newname') end
  local script=string.format('%s,%s,%s,%s,cgi,lua/anonymize_script.lua(%s%s)', items[1] or '', items[2] or '', items[3] or '', items[4] or '', CGI('newid'), stage)
  servercommand([[export:]]..script, 'cgibinary')
  return
end

if CGI('parameter', '')=='storeclick' then
  servercommand('lua:print("'..
  'series='..CGI('series')..'&slice='..CGI('slice')..'&x='..CGI('x')..'&y='..CGI('y')
  ..'")')
  servercommand('lua:f=io.open("clicks.txt", "at") f:write("'..
  'series='..CGI('series')..'&slice='..CGI('slice')..'&x='..CGI('x')..'&y='..CGI('y')
  ..'"); f:write("\\n"); f:close()')
  print 'hallo'
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
		
print([[<script>
// show modal message in div modalmessage
function message(text) {
  var s='';

  s += ('<div style="border:5px solid black; overflow:visible; position:absolute; left:5%; top:5%; width:70%; height:50%; background-color:#dbb; padding:2%; border-radius:15px">');
  s += '<br><p style="position:absolute;top:10%;left:50%;transform:translate(-50%,-50%);"><h2>';
  s += text;
  s += '</h2></p><br>';
  s += "<button id=close style='font-size:22px;position:absolute;top:50%;left:0;right:0;margin-left:auto;margin-right:auto;'>Close</button>";
  s += ('</div');
  document.getElementById('modalmessage').style.display = 'block';
  document.getElementById('modalmessage').innerHTML = s;
  document.getElementById('close').addEventListener('click', function(e) {
    document.getElementById('modalmessage').style.display = 'none';
  });
}
function upload(formname) {
  const XHR = new XMLHttpRequest();
  const FD = new FormData( document.getElementById(formname) );
  XHR.upload.onprogress = function (evt) { var per=40*(evt.loaded/evt.total); message('busy uploading '+('o').repeat(per)+('_').repeat(40-per)); }
  XHR.onreadystatechange = function () {
    if ((this.readyState === 4) && (this.status === 200)) { message(XHR.response);  }
    else if ((this.readyState === 4) && (this.status === 404)) {message('Error uploading') }
  }
  XHR.open( "POST", this.server );
  XHR.send( FD );
}
</script>
]])
 		
HTML("<BODY BGCOLOR='CFDFCF'>")

print('<div id=modalmessage style="left:0;top:0;width:100%;height:100%;position:fixed;z-index:1;padding:10%; display:none"></div>')

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
HTML("<FORM ACTION=\"\"   onSubmit=\"if (this.patientnamematch.value == '' && this.patientidmatch.value == '') {alert('Please, fill the entry fields');return false;}\">");
HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=listpatients>");
HTML("<INPUT NAME=key     TYPE=HIDDEN VALUE=%s>", key);
HTML("<td>Local Patient List");
HTML("<td>Patient ID: <INPUT NAME=patientidmatch TYPE=Text VALUE=>");
HTML("<td>Name: <INPUT NAME=patientnamematch TYPE=Text style=\"text-transform:uppercase;\" onkeyup=\"javascript:this.value=this.value.toUpperCase();\" VALUE=>");
HTML("<td>");
HTML("<td><INPUT TYPE=SUBMIT VALUE=Submit>");
HTML("</FORM>");
HTML("</tr>");	

HTML("<tr>");
HTML("<FORM ACTION=\"\" onSubmit=\"if (this.patientnamematch.value == '' && this.patientidmatch.value == '' && this.studydatematch.value == '') {alert('Please, fill the entry fields');return false;}\">");
HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=liststudies>");
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
  HTML("<FORM ACTION=\"\">");
  HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=listtrials>");
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

if not readOnly then
  HTML("<FORM ID=upx ACTION=\"\" METHOD=POST ENCTYPE=\"multipart/form-data\">");
  HTML("<INPUT NAME=mode      TYPE=HIDDEN VALUE=start>");
  HTML("<INPUT NAME=parameter TYPE=HIDDEN VALUE=uploadfile>");
  HTML("<INPUT NAME=script    TYPE=HIDDEN VALUE=servercommand('addimagefile:'..filename)>");
  HTML("Upload file to enter into server (dcm/v2/HL7/zip/7z/gz/tar): <INPUT NAME=filetoupload SIZE=40 TYPE=file VALUE=>");
  --HTML("<INPUT TYPE=SUBMIT VALUE=Go>");
  HTML('<INPUT TYPE=BUTTON VALUE="Go" onclick=message("Wait");upload("upx")>');
  HTML("</FORM>");
end

HTML("</BODY>")
