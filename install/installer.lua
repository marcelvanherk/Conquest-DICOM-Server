-- install conquest prerequisites (if needed) and server on Linux
--
-- can be run in Zerobrane studio with wx as GUI (zbstudio installer.lua, F5, F5)
-- or can be run in command line with stdio GUI (lua installer.lua)
-- only suitable for Linux
--
-- When run from inside server folder uses the settings there
-- When run from outside server folder creates a new server folder with AE name
-- Note: supports multiple servers but only one web interface
--
-- mvh 20230903 creation
-- mvh 20230909 extended to fairly complete installer
-- mvh 20230909 fixed passing port to dicom.ini; fixed web install
-- mvh 20230910 databases, verbose mode, command line options
-- mvh 20230911 Fix editfile, update help
-- mvh 20230911 Fix mariadb package mariadb-server
-- mvh 20230916 Changed input order for linker for older gcc; add /usr/sbin to apache command paths
-- mvh 20230919 Fixed dbaseiii config; added null database
-- mvh 20230920 Small reconfigure of above; -r recompiles without asking

package.path = package.path .. ';../lua/?.lua'
--package.cpath = package.path .. ';clibs/lib?.so'

-- try to load wx (comes with zbstudio)
local wx
pcall(function() wx = require("wx") end)

-- ask question with text answer
function ask(question, info)
  if wx==nil then
    io.write(question)
    if info then print(info) end
    local ans = io.read()
    return ans
  else
    local UI = {}
    UI.text=''
    UI.MyFrame1 = wx.wxFrame (wx.NULL, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxSize( 500,150 ), wx.wxDEFAULT_FRAME_STYLE+wx.wxTAB_TRAVERSAL )
    UI.MyFrame1:SetSizeHints( wx.wxDefaultSize, wx.wxDefaultSize )
    UI.bSizer1 = wx.wxBoxSizer( wx.wxVERTICAL )
    UI.m_staticText1 = wx.wxStaticText( UI.MyFrame1, wx.wxID_ANY, question, wx.wxDefaultPosition, wx.wxDefaultSize, 0 )
    UI.m_staticText1:Wrap( -1 )
    UI.bSizer1:Add( UI.m_staticText1, 0, wx.wxALL + wx.wxALIGN_CENTER_HORIZONTAL, 5 )
    if info then
      UI.m_staticText2 = wx.wxStaticText( UI.MyFrame1, wx.wxID_ANY, info, wx.wxDefaultPosition, wx.wxDefaultSize, 0 )
      UI.m_staticText2:Wrap( -1 )
      UI.bSizer1:Add( UI.m_staticText2, 0, wx.wxALL + wx.wxALIGN_CENTER_HORIZONTAL, 5 )
    end
    UI.m_textCtrl1 = wx.wxTextCtrl( UI.MyFrame1, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxSize( 300,-1 ), wx.wxTE_PROCESS_ENTER )
    UI.bSizer1:Add( UI.m_textCtrl1, 0, wx.wxALL + wx.wxALIGN_CENTER_HORIZONTAL, 5 )
    UI.gSizer1 = wx.wxGridSizer( 1, 2, 0, 0 )
    UI.m_buttonOK = wx.wxButton( UI.MyFrame1, wx.wxID_ANY, "OK", wx.wxDefaultPosition, wx.wxDefaultSize, 0 )
    UI.gSizer1:Add( UI.m_buttonOK, 0, wx.wxALL + wx.wxALIGN_CENTER_HORIZONTAL, 5 )
    UI.m_buttonCancel = wx.wxButton( UI.MyFrame1, wx.wxID_ANY, "Cancel", wx.wxDefaultPosition, wx.wxDefaultSize, 0 )
    UI.gSizer1:Add( UI.m_buttonCancel, 0, wx.wxALL + wx.wxALIGN_CENTER_HORIZONTAL, 5 )
    UI.bSizer1:Add( UI.gSizer1, 1, wx.wxEXPAND, 5 )
    UI.MyFrame1:SetSizer( UI.bSizer1 )
    UI.MyFrame1:Layout()

    UI.m_textCtrl1:Connect( wx.wxEVT_COMMAND_TEXT_ENTER, function(event)
      event:Skip()
      UI.text = UI.m_textCtrl1:GetValue()
      UI.MyFrame1:Close()
    end )
	
    UI.m_buttonOK:Connect( wx.wxEVT_COMMAND_BUTTON_CLICKED, function(event)
      event:Skip()
      UI.text = UI.m_textCtrl1:GetValue()
      UI.MyFrame1:Close()
    end )
    
    UI.m_buttonCancel:Connect( wx.wxEVT_COMMAND_BUTTON_CLICKED, function(event)
      event:Skip()
      UI.MyFrame1:Close()
      return ''
    end )
    
    --UI.MyFrame1:Centre(wx.wxBOTH)
    UI.MyFrame1:Show(true)
    wx.wxGetApp():MainLoop()
    return UI.text
  end
end

local sudopw = ''
local toinstall = ''
local phpversion = ''
local luaversion = ''
local sep = '/'
local verbose=false
local mariadb_packages = ' mariadb-server libmariadb-dev libmariadb-dev-compat '

-- run a shell command
function runquiet(s)
  if verbose then print(s) end
  local f, t
  
  if string.find(s, 'sudo')==1 and sudopw=='' then
    sudopw = ask('Give sudo password for installation operations: ')
    if sudopw=='' then return end
  end
  
  if string.find(s, 'sudo')==1 and sudopw~='' then
    t = sudopw
  end
  
  if t then 
    f = io.popen(s, 'w'); 
    f:write(t) 
  else
    f = io.popen(s);
  end
  
  if f then
    local s=f:read('*all')
    f:close()
    return s
  else 
    return ''
  end
end

local server = string.gsub(runquiet('pwd'), '\n', '')
local servername = 'CONQUESTSRV1'
local serverport = '5678'
local database   = 'sqlite'
local recompile  = false
local reconfigure = false
local myconf = {}

for k, v in ipairs(arg) do 
  if v=='-v' or v=='--verbose' then verbose=true end
  if v=='-d' or v=='--database' then database = arg[k+1] or 'sqlite' end
  if v=='-r' or v=='--recompile' then recompile=true end
  if v=='-c' or v=='--configure' then reconfigure=true end
  if v=='-h' or v=='--help' then 
    print('installer program for Conquest DICOM server on Linux')
    print('run as: lua5.1 installer.lua options')
    print('  options:')
    print('  -v --verbose')
    print('  -d --database sqlite|mariadb|dbaseiii|pgsql|null')
    print('  -r --recompile (needed when changing database)')
    print('  -c --configure (force reconfigure)')
    os.exit()
  end
end

function fileexists(f)
  local h=io.open(f)
  if h then 
    h:close() 
    return 1 
  end
end

function directoryexists(f)
  return os.rename(f, f)
end

-- copy file f to g
function copyfile(f, g)
  if verbose then print('copying', f, 'to', g) end
  local h=io.open(f, [[rb]])
  if h==nil then
    print('Cannot copy file: ' .. f)
    return false
  end
  local s=h:read([[*a]])
  h:close()
  h=io.open(g, [[wb]])
  if h==nil then
    print('Cannot write file: ' .. g)
    return false
  end
  h:write(s)
  h:close()
  return true
end

-- substitute mask with new value in textfile
function editfile(f, mask, value)
  if verbose then print('adapting', f, mask, value) end
  local s=''
  for x in io.lines(f) do 
    x = string.gsub(x, '\n', '')
    x = string.gsub(x, '\r', '')
    x = string.gsub(x, mask, value)
    s = s..x.."\r\n" 
  end
  local h=io.open(f, [[wt]])
  if h then h:write(s) h:close() end
end

function getfile(f, mask)
  local y
  local h = io.open(f, [[rt]])
  if h then
    h:close()
    for x in io.lines(f) do
      x = string.gsub(x, '\n', '')
      x = string.gsub(x, '\r', '')
      x = string.match(x, mask)
      y = y or x
    end
  end
  return y
end

function create_server_file(f, s)
  if verbose then print('creating', f) end
  f = io.open(f, [[wt]]); 
  local s = string.gsub(s, '\n', '\r\n')
  f:write(s); 
  f:close()  
end

local y = ask('This will install Conquest dicom server and its prerequisites - continue? y/n: ')
if y~='y' then
  os.exit()
end

-----------------------------------------
-----------------------------------------
function compile(param, conf, server)
  if verbose then print('Compiling', param, conf.DB) end
  local conf= conf or {}
  if param=='jpeg6c' then
    runquiet('mkdir -p '..server..'/src/dgate/build')
    runquiet('rm -f '..server..'/src/dgate/build/libjpeg.a')
    runquiet('sh -c "cd '..server..'/src/dgate/jpeg-6c; chmod 777 configure; ./configure; make; cp libjpeg.a '..server..'/src/dgate/build"')
    if fileexists(server..'/src/dgate/build'..'/libjpeg.a') then
      print('[OK] Compiled jpeg library')
    end
  end
  -----------------------------------------
  if param=='charls' then
    runquiet('mkdir -p '..server..'/src/dgate/build')
    runquiet('rm -f '..server..'/src/dgate/build/charls.o')
    runquiet('g++ -w -o '..server..'/src/dgate/build/charls.o -c '..server..'/src/dgate/charls/all.cpp -I'..server..'/src/dgate/charls')
    if fileexists(server..'/src/dgate/build'..'/charls.o') then
      print('[OK] Compiled charls library (jpegLS)')
    end
  end
  -----------------------------------------
  if param=='openjpeg' then
    runquiet('mkdir -p '..server..'/src/dgate/build')
    runquiet('rm -f '..server..'/src/dgate/build/openjpeg.o')
    runquiet('gcc -o '..server..'/src/dgate/build/openjpeg.o -c '..server..'/src/dgate/openjpeg/all.c -I'..server..'/src/dgate/openjpeg')
    if fileexists(server..'/src/dgate/build'..'/openjpeg.o') then
      print('[OK] Compiled openjpeg library (jpeg2000)')
    end
  end
  -----------------------------------------
  if param=='lua' then
    runquiet('mkdir -p '..server..'/src/dgate/build')
    runquiet('rm -f '..server..'/src/dgate/build/lua.o')
    runquiet('gcc -o '..server..'/src/dgate/build/lua.o -c '..server..'/src/dgate/lua_5.1.5/all.c -I'..server..'/src/dgate/lua_5.1.5 -DLUA_USE_DLOPEN -DLUA_USE_POSIX')
    if fileexists(server..'/src/dgate/build'..'/lua.o') then
      print('[OK] Compiled lua library (scripting)')
    end
  end
  -----------------------------------------
  if param=='luasocket' then
    runquiet('chmod 777 '..server..'/src/dgate/luasocket/amake.sh')
    runquiet('cd '..server..'/src/dgate/luasocket|./amake.sh')
    runquiet('cp '..server..'/src/dgate/luasocket/luasocket.a '..server..'/src/dgate/build')
    if fileexists(server..'/src/dgate/build'..'/luasocket.a') then
      print('[OK] Compiled luasocket library (network)')
    end
  end
  -----------------------------------------
  if param=='sqlite3' then
    runquiet('mkdir -p '..server..'/src/dgate/build')
    runquiet('rm -f '..server..'/src/dgate/build/sqlite3.o')
    runquiet('gcc -DTHREADSAFE=1 -DHAVE_USLEEP -o '..server..'/src/dgate/build/sqlite3.o -c '..server..'/src/dgate/sqlite3/sqlite3.c -I'..server..'/src/dgate/sqlite3')
    if fileexists(server..'/src/dgate/build'..'/sqlite3.o') then
      print('[OK] Compiled sqlite3 library (database)')
    end
  end
  -----------------------------------------
  if param=='dgate' then
    -- both dbaseiii and sqlite3 database drivers are compiled by default
    local dbo = server..'/src/dgate/build/sqlite3.o '
    local dbl = ''
    local dbi = '-I'..server..'/src/dgate/sqlite3 '
    local dbd = '-DUSESQLITE '
    if (conf.DB or CGI('DB', ''))=='mysql' then
      dbo = '-lmysqlclient '
      dbl = '-L/usr/local/mysql/lib -L/usr/lib/mysql '
      dbi = '-I/usr/local/mysql/include -I/usr/include/mysql '
      dbd = '-DUSEMYSQL '     
    end
    if (conf.DB or CGI('DB', ''))=='mariadb' then
      dbo = '-lmariadb '
      dbl = '-L/usr/lib/x86_64-linux-gnu '
      dbi = '-I/usr/include/mariadb -I/usr/include/mariadb/mysql -I/usr/include/mysql -I/usr/local/mariadb/include '
      dbd = '-DUSEMYSQL '     
    end
    if (conf.DB or CGI('DB', ''))=='pgsql' then
      dbo = '-lpq '
      dbl = '-L/usr/local/pgsql/lib '
      dbi = '-I/usr/include/postgresql/ ' 
      dbd = '-DPOSTGRES '     
    end
    runquiet('mkdir -p '..server..'/src/dgate/build')
    runquiet('rm -f '..server..'/src/dgate/build/dgate')
    runquiet(
    'g++ -w -std=c++11 -DUNIX -DNATIVE_ENDIAN=1 '..
    '-Wno-multichar -Wno-write-strings -Wno-format-overflow -DHAVE_LIBJPEG '..
    '-DHAVE_LIBCHARLS '..
    '-DHAVE_LIBOPENJPEG2 '..
    dbd..
    dbi..
    --server..'src/dgate/build/lua.o '..
    --server..'src/dgate/build/luasocket.a '..
  
    server..'/src/dgate/build/charls.o '..
    server..'/src/dgate/build/openjpeg.o '..
  
    '-o '..server..'/src/dgate/build/dgate '..
    '-I'..server..'/src/dgate/src '..
    server..'/src/dgate/src/total.cpp '..
    '-llua5.1 '..
    '-I'..server..'/src/dgate/lua_5.1.5 '..
    '-I'..server..'/src/dgate/dicomlib '..
  
    '-I'..server..'/src/dgate/charls '..
    '-I'..server..'/src/dgate/openjpeg '..
  
    '-I'..server..'/src/dgate/jpeg-6c '..
    '-L'..server..'/src/dgate/jpeg-6c -ljpeg '..
    dbl..
    dbo..
    '-lpthread -ldl '
    )
    if fileexists(server..'/src/dgate/build'..'/dgate') then
      print('[OK] Compiled dgate application (server core) for '..conf.DB)
    end
  end
  -----------------------------------------
  if param=='dgatesmall' then
    runquiet('mkdir -p '..server..'/src/dgate/build')
    runquiet('rm -f '..server..'/src/dgate/build/dgatesmall')
    runquiet(
    'g++ -w -DUNIX -DNATIVE_ENDIAN=1 -Wno-write-strings -DNOINTJPEG '..
    '-Wno-multichar -Wno-format-overflow '..
    -- server..'/src/dgate/build/lua.o '..
    -- server..'/src/dgate/build/luasocket.a '..
    '-o '..server..'/src/dgate/build/dgatesmall '..
    '-I'..server..'/src/dgate/src '..
    server..'/src/dgate/src/total.cpp '..
    '-llua5.1 -lpthread -ldl '..
    '-I'..server..'/src/dgate/dicomlib '..
    '-I'..server..'/src/dgate/lua_5.1.5 '..
    '-I'..server..'/src/dgate/jpeg-6c '..
    '-L'..server..'/src/dgate/jpeg-6c -ljpeg '
    )
    if fileexists(server..'/src/dgate/build'..'/dgatesmall') then
      print('[OK] Compiled dgatesmall application (server client)')
    end
  end
  -----------------------------------------
  if param=='servertask' then
    if fileexists(server..'/src/servertask/servertask') then
      runquiet('rm -f '..server..'/src/servertask/servertask')
    end
    runquiet('chmod 777 '..server..'/src/servertask/make.sh');
    runquiet('cd '..server..'/src/servertask;./make.sh');
    if fileexists(server..'/src/servertask/servertask') then
      runquiet('chmod 777 '..server..'/src/servertask/servertask');
      print('[OK] compiled servertask (for web api)')
    else
      print('[ERROR] servertask (for web api) not compiled')
    end
  end
  -----------------------------------------
  if param=='dgateall' then
    compile('jpeg6c', conf, server)
    compile('openjpeg', conf, server)
    compile('charls', conf, server)
    compile('sqlite3', conf, server)
    compile('dgate', conf, server)
    copyfile(server..'/src/dgate/build/dgate', server..'/dgate')
    runquiet('chmod 777 '..server..'/dgate')

    compile('servertask', conf, server)
    runquiet('sudo -S cp '..server..'/src/servertask/servertask /var/www/html/api/dicom/servertask');
  end
end

-----------------------------------------
-----------------------------------------
-----------------------------------------

function CGI(a, b) return b end

-- functions for initial server config
function create_server_dicomini(conf, server)
  conf = conf or {}
  local sqlite, dbaseiii, mysql, pgsql, mariadb
  sqlite=0; if (conf.DB or CGI('DB', ''))=='sqlite' then dbaseiii=0; sqlite=1 end
  dbaseiii=1; if (conf.DB or CGI('DB', ''))=='dbaseiii' then dbaseiii=1 end
  mysql=0; if (conf.DB or CGI('DB', ''))=='mysql' then dbaseiii=0; mysql=1 end
  mariadb=0; if (conf.DB or CGI('DB', ''))=='mariadb' then dbaseiii=0; mariadb=1 end
  pgsql=0; if (conf.DB or CGI('DB', ''))=='pgsql' then dbaseiii=0; pgsql=1 end
  local doublebackslashtodb = 0
  local useescapestringconstants = 0
  if (conf.SE or CGI('SE', ''))=='' then dbaseiii=0 end
  if pgsql==1 then useescapestringconstants=1 end
  if mysql==1 or pgsql==1 or mariadb==1 then doublebackslashtodb=1 end

  if not fileexists(server..'/dicom.ini') or reconfigure then
    create_server_file(server..'/dicom.ini', [[
# This file contains configuration information for the DICOM server
# Do not edit unless you know what you are doing
# Autogenerated for 1.5.0c

[sscscp]
MicroPACS                = sscscp

# Network configuration: server name and TCP/IP port#
MyACRNema                = ]]..(conf.AE or CGI('AE', 'CONQUESTSRV1'))..[[

TCPPort                  = ]]..(conf.PORT or CGI('PORT', '5678'))..[[


# Host, database, username and password for database
SQLHost                  = ]]..(conf.SH or CGI('SH', 'localhost'))..[[

SQLServer                = ]]..(conf.SE or CGI('SE', server..'/data'..sep..'dbase'..sep..'conquest.db3'))..[[

Username                 = ]]..(conf.SU or CGI('SU', 'conquest'))..[[

Password                 = ]]..(conf.SP or CGI('SP', 'conquest'))..[[

SqLite                   = ]]..sqlite..[[

MySQL                    = ]]..math.max(mariadb, mysql)..[[

Postgres                 = ]]..pgsql..[[

DoubleBackSlashToDB      = ]]..doublebackslashtodb..[[

UseEscapeStringConstants = ]]..useescapestringconstants..[[


# Configure server
ImportExportDragAndDrop  = 1
ZipTime                  = 05:
UIDPrefix                = 1.2.826.0.1.3680043.2.135.150.]]..os.time()..[[

EnableComputedFields     = 1

FileNameSyntax           = ]]..(conf.FN or CGI('FN', '9'))..[[


# Configuration of compression for incoming images and archival
DroppedFileCompression   = ]]..(conf.CP or CGI('CP', 'un'))..[[

IncomingCompression      = ]]..(conf.CP or CGI('CP', 'un'))..[[

ArchiveCompression       = as

# For debug information
PACSName                 = ]]..(conf.AE or CGI('AE', 'CONQUESTSRV1'))..[[

OperatorConsole          = 127.0.0.1
DebugLevel               = 0

# Configuration of disk(s) to store images
MAGDeviceFullThreshHold  = 30
MAGDevices               = 1
MAGDevice0               = ]]..(conf.server or server)..[[/data]]..(conf.sep or sep)..[[

[lua]
association = package.path=package.path..';'..Global.basedir..'lua/?.lua'

]])
  end
end

function create_newweb_app(conf, server)
  conf = conf or {}
  local list = {'start','listpatients','liststudies','listseries','listimageswiththumbs','listimages','listtrials','listtrialpatients',
                 'wadostudyviewer','wadoseriesviewer','wadoviewerhelp','slice','weasis_starter',
                 'wadoviewer_starter', 'dwvviewer_starter', 'altviewer_starter', 'imagejviewer_starter',
	         'flexviewer_starter', 'papaya_starter', 'cornerstone_starter', 'iframe_starter', 'dumpvars',
		 'conquest', 'favicon', --jpg/ico
		 'index', 'dgate', 'singlefilelogin', --php
		 '.htaccess',
	       }
  if true then -- not fileexists(cgiweb..'newweb'..sep..'dicom.ini') then
    runquiet('sudo -S mkdir -p '..conf.htmlweb..'/app')
    runquiet('sudo -S mkdir -p '..conf.htmlweb..'/app'..sep..'newweb')

    local s='exceptions='
    local source=server..'/webserver'..sep..'htdocs'..sep..'app'..sep..'newweb'..sep
    local dest=conf.htmlweb..'/app'..sep..'newweb'..sep

    for _,v in ipairs(list) do
      if fileexists(source..v..'.lua') then
        runquiet('sudo -S cp '..source..v..'.lua '..dest..v..'.lua')
        s=s..v..','
      end
      if fileexists(source..v..'.php') then
        runquiet('sudo -S cp '..source..v..'.php '..dest..v..'.php')
      end
      if fileexists(source..v) then
        runquiet('sudo -S cp '..source..v..' '..dest..v)
      end
      if fileexists(source..v..'.jpg') then
        runquiet('sudo -S cp '..source..v..'.jpg '..conf.htmlweb..sep..v..'.jpg')
        runquiet('sudo -S cp '..source..v..'.jpg '..dest..v..'.jpg')
      end
      if fileexists(source..v..'.ico') then
        runquiet('sudo -S cp '..source..v..'.ico '..conf.htmlweb..sep..v..'.ico')
        runquiet('sudo -S cp '..source..v..'.ico '..dest..v..'.ico')
      end
      runquiet('sudo -S cp '..server..'/dgate '..dest..'dgate')
    end
    s = s .. 'addlocalfile,attachfile'
    --s = string.sub(s, 1, #s-1)

    create_server_file(source..'config.php', [[
<?php
  $folder = '.';				// where are the newweb files
  $exe    = 'dgate -p]]..(conf.PORT or CGI('PORT', '5678'))..[[ -q127.0.0.1';
  $quote  = '""';				// quotes in command line

  if (PHP_OS_FAMILY != 'Windows') {		// On Linux:
    $exe = './' . $exe;				// start as ./servertask
    $quote = '\"';				// quotes in command line
  }

  $userlogin = false;				// uses single file login system
  $wplogin   = false;				// uses wordpress login system
  $cors      = false;				// allow cross site use
]])

    create_server_file(source..'dicom.ini', [[
#mvh 20220829  for 1.5.0c

[sscscp]
MicroPACS                = sscscp
ACRNemaMap               = acrnema.map
Dictionary               = dgate.dic
WebServerFor             = 127.0.0.1
TCPPort                  = ]]..(conf.PORT or CGI('PORT', '5678'))..[[

WebScriptAddress         = /app/newweb/
WebCodeBase              = /
TempDir                  = ]]..(conf.TEMP or CGI('TEMP', '/temp') or 'temp') ..[[


[webdefaults]
size     = 560
dsize    = 0
compress = un
iconsize = 84
graphic  = jpg
readOnly = ]]..(conf.READONLY or CGI('READONLY', '0'))..[[

viewOnly = ]]..(conf.VIEWONLY or CGI('VIEWONLY', '0'))..[[

viewer   = ]]..(conf.SVIEWER or CGI('SVIEWER', 'wadoseriesviewer'))..[[

studyviewer = ]]..(conf.VIEWER or CGI('VIEWER', 'wadostudyviewer'))..[[

studylink = '<TD><A target="_blank" href=/app/ohif/viewer/{StudyInstanceUID}>Ohif</A>'
serieslink = '<TD><A target="_blank" href=/app/newweb/?mode=wadoseriesviewer&series={PatientID}:{SeriesInstanceUID}>View</A>'

[DefaultPage]
source = *.lua

[AnyPage]
source = start.lua
]]..s)

    runquiet('sudo -S cp '..source..'config.php '..dest..'config.php')
    runquiet('sudo -S cp '..source..'dgate.php '..dest..'dgate.php')
    runquiet('sudo -S cp '..source..'dicom.ini '..dest..'dicom.ini')
    runquiet('sudo -S cp '..source..'dgate.dic '..dest..'dgate.dic')
  end
end

function create_dicom_api(conf, server)
  conf = conf or {}
  local list = {'posters','test','rquery','index','qido','servertask','posters','wado','readme','.htaccess','Router'}
  local exts={'.html', '.lua', '', '.php', '.exe'}
  runquiet('sudo -S mkdir -p '..conf.htmlweb..'/api')
  runquiet('sudo -S mkdir -p '..conf.htmlweb..'/api'..sep..'dicom')

  local s='exceptions='
  local source=server..'/webserver'..sep..'htdocs'..sep..'api'..sep..'dicom'..sep
  local dest=conf.htmlweb..sep..'api'..sep..'dicom'..sep

  for _,v in ipairs(list) do
    for _,w in ipairs(exts) do
      if fileexists(source..v..w) then
        runquiet('sudo -S cp '..source..v..w..' '..dest..v..w)
      end
    end
  end

  create_server_file(source..'config.php', [[
<?php
  $exe    = 'servertask -p]]..(conf.PORT or CGI('PORT', '5678'))..[[ -q127.0.0.1';
  $quote  = '""';				// quotes in command line

  if (PHP_OS_FAMILY != 'Windows') {		// On Linux:
    $exe = './' . $exe;				// start as ./servertask
    $quote = '\"';				// quotes in command line
  }

  $userlogin = false;				// uses single file login system
  $wplogin   = false;				// uses wordpress login system
]])

  runquiet('sudo -S cp '..source..'config.php'..' '..dest..'config.php')
  runquiet('sudo -S chmod 777 '..dest..'servertask')
end

function create_ohif_app(conf, server)
  local list = {'ohif1.03_min','index','.htaccess'}
  local exts={'.html', '.js', ''}
  runquiet('sudo -S mkdir -p '..conf.htmlweb..'/app')
  runquiet('sudo -S mkdir -p '..conf.htmlweb..'/app'..sep..'ohif')

  local source=server..'/webserver'..sep..'htdocs'..sep..'app'..sep..'ohif'..sep
  local dest=conf.htmlweb..sep..'app'..sep..'ohif'..sep

  for _,v in ipairs(list) do
    for _,w in ipairs(exts) do
      if fileexists(source..v..w) then
        runquiet('sudo -S cp '..source..v..w..' '..dest..v..w)
      end
    end
  end
end

function create_server_dicomsql(server)
  if not fileexists(server..'/dicom.sql') then
    create_server_file(server..'/dicom.sql', [[
/*
#       DICOM Database layout
#     Example version for all SQL servers (mostly normalized)
#
#	(File DICOM.SQL)
#	** DO NOT EDIT THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING **
#
#	Version with modality moved to the series level and EchoNumber in image table
#	Revision 3: Patient birthday and sex, bolus agent, correct field lengths
#	Revision 4: Studymodality, Station and Department in study
#	            Manufacturer, Model, BodyPart and Protocol in series
#	            Acqdate/time, coil, acqnumber, slicelocation and pixel info in images
#       Notes for revision 4:
#         InstitutionalDepartmentName in study (should officially be in series, but eFilm expects it in study)
#         StationName is in study              (should officially be in series, but more useful in study)
#	Revision 5: Added patientID in series and images for more efficient querying
#	Revision 6: Added frame of reference UID in series table
#	Revision 7: Added ImageType in image table, StudyModality to 64 chars, AcqDate to SQL_C_DATE
#	Revision 8: Denormalized study table (add patient ID, name, birthdate) to show consistency problems
#	Revision 10: Fixed width of ReceivingCoil: to 16 chars
#	Revision 13: Added ImageID to image database
#	Revision 14: Added WorkList database with HL7 tags
#	Revision 16: Moved Stationname and InstitutionalDepartmentName to series table
#	Revision 17: EchoNumber, ReqProcDescription to 64 characters; StudyModality, EchoNumber, ImageType to DT_MSTR; use Institution instead of InstitutionalDepartmentName
#	Revision 18: DT_STR can now be replaced by DT_ISTR to force case insensitive searches
#	Revision 19: Use QRows and QColumns as field names
#
#
#	5 databases need to be defined:
#
#		*Patient*
#			*Study*
#				*Series*
#					*Image*
#			*WorkList*
#
#
# The last defined element of Study is a link back to Patient
# The last defined element of Series is a link back to Study
# The last defined element of Image is a link back to Series
#
#
# Format for DICOM databases :
#	{ Group, Element, Column Name, Column Length, SQL-Type, DICOM-Type }
# Format for Worklist database :
#	{ Group, Element, Column Name, Column Length, SQL-Type, DICOM-Type, HL7 tag}
#	HL7 tags include SEQ.N, SEQ.N.M, SEQ.N.DATE, SEQ.N.TIME, *AN, *UI
*/

*Patient*
{
	{ 0x0010, 0x0020, "PatientID", 64, SQL_C_CHAR, DT_STR },
	{ 0x0010, 0x0010, "PatientName", 64, SQL_C_CHAR, DT_STR },
        { 0x0010, 0x0030, "PatientBirthDate", 8, SQL_C_DATE, DT_DATE },
	{ 0x0010, 0x0040, "PatientSex", 16, SQL_C_CHAR, DT_STR }
}

*Study*
{
	{ 0x0020, 0x000d, "StudyInstanceUID", 64, SQL_C_CHAR, DT_UI },
	{ 0x0008, 0x0020, "StudyDate", 8, SQL_C_DATE, DT_DATE },
	{ 0x0008, 0x0030, "StudyTime", 16, SQL_C_CHAR, DT_TIME },
	{ 0x0020, 0x0010, "StudyID", 16, SQL_C_CHAR, DT_STR },
	{ 0x0008, 0x1030, "StudyDescription", 64, SQL_C_CHAR, DT_STR },
	{ 0x0008, 0x0050, "AccessionNumber", 16, SQL_C_CHAR, DT_STR },
	{ 0x0008, 0x0090, "ReferPhysician", 64, SQL_C_CHAR, DT_STR },
	{ 0x0010, 0x1010, "PatientsAge", 16, SQL_C_CHAR, DT_STR },
	{ 0x0010, 0x1030, "PatientsWeight", 16, SQL_C_CHAR, DT_STR },
	{ 0x0008, 0x0061, "StudyModality", 64, SQL_C_CHAR, DT_MSTR },

	{ 0x0010, 0x0010, "PatientName", 64, SQL_C_CHAR, DT_STR },
        { 0x0010, 0x0030, "PatientBirthDate", 8, SQL_C_DATE, DT_DATE },
	{ 0x0010, 0x0040, "PatientSex", 16, SQL_C_CHAR, DT_STR }

	{ 0x0010, 0x0020, "PatientID", 64, SQL_C_CHAR, DT_STR }
}

*Series*
{
	{ 0x0020, 0x000e, "SeriesInstanceUID", 64, SQL_C_CHAR, DT_UI },
	{ 0x0020, 0x0011, "SeriesNumber", 12, SQL_C_CHAR, DT_STR },
	{ 0x0008, 0x0021, "SeriesDate", 8, SQL_C_DATE, DT_DATE },
	{ 0x0008, 0x0031, "SeriesTime", 16, SQL_C_CHAR, DT_TIME },
	{ 0x0008, 0x103e, "SeriesDescription", 64, SQL_C_CHAR, DT_STR },
	{ 0x0008, 0x0060, "Modality", 16, SQL_C_CHAR, DT_STR },
	{ 0x0018, 0x5100, "PatientPosition", 16, SQL_C_CHAR, DT_STR },
	{ 0x0018, 0x0010, "ContrastBolusAgent", 64, SQL_C_CHAR, DT_STR },
	{ 0x0008, 0x0070, "Manufacturer", 64, SQL_C_CHAR, DT_STR },
	{ 0x0008, 0x1090, "ModelName", 64, SQL_C_CHAR, DT_STR },
	{ 0x0018, 0x0015, "BodyPartExamined", 64, SQL_C_CHAR, DT_STR },
	{ 0x0018, 0x1030, "ProtocolName", 64, SQL_C_CHAR, DT_STR },
	{ 0x0008, 0x1010, "StationName", 16, SQL_C_CHAR, DT_STR },
	{ 0x0008, 0x0080, "Institution", 64, SQL_C_CHAR, DT_STR },
	{ 0x0020, 0x0052, "FrameOfReferenceUID", 64, SQL_C_CHAR, DT_UI },
	{ 0x0010, 0x0020, "SeriesPat", 64, SQL_C_CHAR, DT_STR },
	{ 0x0020, 0x000d, "StudyInstanceUID", 64, SQL_C_CHAR, DT_UI }
}

*Image*
{
	{ 0x0008, 0x0018, "SOPInstanceUID", 64, SQL_C_CHAR, DT_UI },
	{ 0x0008, 0x0016, "SOPClassUID", 64, SQL_C_CHAR, DT_UI },
	{ 0x0020, 0x0013, "ImageNumber", 12, SQL_C_CHAR, DT_STR },
	{ 0x0008, 0x0023, "ImageDate", 8, SQL_C_DATE, DT_DATE },
	{ 0x0008, 0x0033, "ImageTime", 16, SQL_C_CHAR, DT_TIME },
	{ 0x0018, 0x0086, "EchoNumber", 64, SQL_C_CHAR, DT_MSTR },
	{ 0x0028, 0x0008, "NumberOfFrames", 12, SQL_C_CHAR, DT_STR },
	{ 0x0008, 0x0022, "AcqDate", 8, SQL_C_DATE, DT_DATE },
	{ 0x0008, 0x0032, "AcqTime", 16, SQL_C_CHAR, DT_TIME },
	{ 0x0018, 0x1250, "ReceivingCoil", 16, SQL_C_CHAR, DT_STR },
	{ 0x0020, 0x0012, "AcqNumber", 12, SQL_C_CHAR, DT_STR },
	{ 0x0020, 0x1041, "SliceLocation", 16, SQL_C_CHAR, DT_STR },
	{ 0x0028, 0x0002, "SamplesPerPixel", 5, SQL_C_CHAR, DT_UINT16 },
	{ 0x0028, 0x0004, "PhotoMetricInterpretation", 16, SQL_C_CHAR, DT_STR },
	{ 0x0028, 0x0010, "QRows", 5, SQL_C_CHAR, DT_UINT16 },
	{ 0x0028, 0x0011, "QColumns", 5, SQL_C_CHAR, DT_UINT16 },
	{ 0x0028, 0x0101, "BitsStored", 5, SQL_C_CHAR, DT_UINT16 },
	{ 0x0008, 0x0008, "ImageType", 128, SQL_C_CHAR, DT_MSTR },
	{ 0x0054, 0x0400, "ImageID", 16, SQL_C_CHAR, DT_STR },
	{ 0x0010, 0x0020, "ImagePat", 64, SQL_C_CHAR, DT_STR },
	{ 0x0020, 0x000e, "SeriesInstanceUID", 64, SQL_C_CHAR, DT_UI }
}

*WorkList*
{
 	{ 0x0008, 0x0050, "AccessionNumber",    16, SQL_C_CHAR, DT_STR,  "OBR.3" },
 	{ 0x0010, 0x0020, "PatientID",          64, SQL_C_CHAR, DT_STR,  "PID.4" },
 	{ 0x0010, 0x0010, "PatientName",        64, SQL_C_CHAR, DT_STR,  "PID.5" },
        { 0x0010, 0x0030, "PatientBirthDate",    8, SQL_C_DATE, DT_DATE, "PID.7" },
 	{ 0x0010, 0x0040, "PatientSex",         16, SQL_C_CHAR, DT_STR,  "PID.8" },

 	{ 0x0010, 0x2000, "MedicalAlerts",      64, SQL_C_CHAR, DT_STR,  "---" },
 	{ 0x0010, 0x2110, "ContrastAllergies",  64, SQL_C_CHAR, DT_STR,  "---" },
 	{ 0x0020, 0x000d, "StudyInstanceUID",   64, SQL_C_CHAR, DT_UI,   "---" },
 	{ 0x0032, 0x1032, "ReqPhysician",       64, SQL_C_CHAR, DT_STR,  "OBR.16" },
 	{ 0x0032, 0x1060, "ReqProcDescription", 64, SQL_C_CHAR, DT_STR,  "OBR.4.1" },

 	{ 0x0040, 0x0100, "--------",   0, SQL_C_CHAR, DT_STARTSEQUENCE, "---" },
 	{ 0x0008, 0x0060, "Modality",           16, SQL_C_CHAR, DT_STR,  "OBR.21" },
 	{ 0x0032, 0x1070, "ReqContrastAgent",   64, SQL_C_CHAR, DT_STR,  "---" },
 	{ 0x0040, 0x0001, "ScheduledAE",        16, SQL_C_CHAR, DT_STR,  "---" },
 	{ 0x0040, 0x0002, "StartDate",           8, SQL_C_DATE, DT_DATE, "OBR.7.DATE" },
 	{ 0x0040, 0x0003, "StartTime",          16, SQL_C_CHAR, DT_TIME, "OBR.7.TIME" },
 	{ 0x0040, 0x0006, "PerfPhysician",      64, SQL_C_CHAR, DT_STR,  "---" },
 	{ 0x0040, 0x0007, "SchedPSDescription", 64, SQL_C_CHAR, DT_STR,  "---" },
 	{ 0x0040, 0x0009, "SchedPSID",          16, SQL_C_CHAR, DT_STR,  "OBR.4" },
 	{ 0x0040, 0x0010, "SchedStationName",   16, SQL_C_CHAR, DT_STR,  "OBR.24" },
 	{ 0x0040, 0x0011, "SchedPSLocation",    16, SQL_C_CHAR, DT_STR,  "---" },
 	{ 0x0040, 0x0012, "PreMedication",      64, SQL_C_CHAR, DT_STR,  "---" },
 	{ 0x0040, 0x0400, "SchedPSComments",    64, SQL_C_CHAR, DT_STR,  "---" },
 	{ 0x0040, 0x0100, "---------",    0, SQL_C_CHAR, DT_ENDSEQUENCE, "---" },

        { 0x0040, 0x1001, "ReqProcID",          16, SQL_C_CHAR, DT_STR,  "OBR.4.0" },
 	{ 0x0040, 0x1003, "ReqProcPriority",    16, SQL_C_CHAR, DT_STR,  "OBR.27" }
}]])
  end
end

function create_server_acrnema(conf, server)
  conf = conf or {}
  if not fileexists(server..'/acrnema.map') then
    create_server_file(server..'/acrnema.map', [[
/* **********************************************************
 *                                                          *
 * DICOM AE (Application entity) -> IP address / Port map   *
 * (This is file ACRNEMA.MAP)                               *
 *                                                          *
 * All DICOM systems that want to retrieve images from the  *
 * Conquest DICOM server must be listed here with correct   *
 * AE name, (IP adress or hostname) and port number. The    *
 * first entry is the Conquest server itself. The last ones *
 * with * show wildcard mechanism. Add new entries above.   *
 *                                                          *
 * The syntax for each entry is :                           *
 *   AE   <IP adress|Host name>   port number   compression *
 *                                                          *
 * For compression see manual. Values are un=uncompressed;  *
 * ul=littleendianexplicit,ub=bigendianexplicit,ue=both     *
 * j2=lossless jpeg;j3..j6=lossy jpeg;n1..n4=nki private    *
 * js   =lossless jpegLS;  j7=lossy jpegLS                  *
 * jk   =lossless jpeg2000;jl=lossy jpeg2000                *
 * J3NN..j6NN, JLNN or J7NN overrides quality factor to NN% *
 *                                                          *
 ********************************************************** */

]]..(conf.AE or CGI('AE', 'CONQUESTSRV1'))..[[	127.0.0.1	]]..(conf.PORT or CGI('PORT', '5678'))..[[	un

V*	        	*               1234            un
W*	        	*               666             un
S*		        *               5678            un
]])
  end
end

function create_server_dgatesop(s, server)
s = s or '#'
  if not fileexists(server..'/dgatesop.lst') then
    create_server_file(server..'/dgatesop.lst', [[
#
# DICOM Application / sop / transfer UID list.
#
# This list is used by the CheckedPDU_Service ( "filename" ) service
# class.  All incoming associations will be verified against this
# file.
#
# Revision 2: disabled GEMRStorage and GECTStorage
# Revision 3: extended with new sops and with JPEG transfer syntaxes
# Revision 4: added Modality Worklist query
# Revision 5: Erdenay added UIDS for CTImageStorageEnhanced, RawDataStorage, 
#             SpatialRegistrationStorage, SpatialFiducialsStorage, DeformableSpatialRegistrationStorage, 
#             SegmentationStorage, SurfaceSegmentationStorage, RTIonPlanStorage, RTIonBeamsTreatmentRecordStorage    
# Revision 6: X-RayRadiationDoseSR, BreastTomosynthesisImageStorage and others
# Revision 7: JpegLS and BigEndianExplicit enabled
# Revision 8: Added C-GET
#
#None				none				RemoteAE
#None				none				LocalAE
#DICOM				1.2.840.10008.3.1.1.1		application
Verification			1.2.840.10008.1.1		sop
RETIRED_StoredPrintStorage                           1.2.840.10008.5.1.1.27		sop
RETIRED_HardcopyGrayscaleImageStorage                1.2.840.10008.5.1.1.29		sop
RETIRED_HardcopyColorImageStorage                    1.2.840.10008.5.1.1.30		sop
ComputedRadiographyImageStorage                      1.2.840.10008.5.1.4.1.1.1		sop
DigitalXRayImageStorageForPresentation               1.2.840.10008.5.1.4.1.1.1.1		sop
DigitalXRayImageStorageForProcessing                 1.2.840.10008.5.1.4.1.1.1.1.1		sop
DigitalMammographyXRayImageStorageForPresentation    1.2.840.10008.5.1.4.1.1.1.2		sop
DigitalMammographyXRayImageStorageForProcessing      1.2.840.10008.5.1.4.1.1.1.2.1		sop
DigitalIntraOralXRayImageStorageForPresentation      1.2.840.10008.5.1.4.1.1.1.3		sop
DigitalIntraOralXRayImageStorageForProcessing        1.2.840.10008.5.1.4.1.1.1.3.1		sop
CTImageStorage                                       1.2.840.10008.5.1.4.1.1.2		sop
EnhancedCTImageStorage                               1.2.840.10008.5.1.4.1.1.2.1		sop
RETIRED_UltrasoundMultiframeImageStorage             1.2.840.10008.5.1.4.1.1.3		sop
UltrasoundMultiframeImageStorage                     1.2.840.10008.5.1.4.1.1.3.1		sop
MRImageStorage                                       1.2.840.10008.5.1.4.1.1.4		sop
EnhancedMRImageStorage                               1.2.840.10008.5.1.4.1.1.4.1		sop
MRSpectroscopyStorage                                1.2.840.10008.5.1.4.1.1.4.2		sop
EnhancedMRColorImageStorage                          1.2.840.10008.5.1.4.1.1.4.3		sop
RETIRED_NuclearMedicineImageStorage                  1.2.840.10008.5.1.4.1.1.5		sop
RETIRED_UltrasoundImageStorage                       1.2.840.10008.5.1.4.1.1.6		sop
UltrasoundImageStorage                               1.2.840.10008.5.1.4.1.1.6.1		sop
EnhancedUSVolumeStorage                              1.2.840.10008.5.1.4.1.1.6.2		sop
SecondaryCaptureImageStorage                         1.2.840.10008.5.1.4.1.1.7		sop
MultiframeSingleBitSecondaryCaptureImageStorage      1.2.840.10008.5.1.4.1.1.7.1		sop
MultiframeGrayscaleByteSecondaryCaptureImageStorage  1.2.840.10008.5.1.4.1.1.7.2		sop
MultiframeGrayscaleWordSecondaryCaptureImageStorage  1.2.840.10008.5.1.4.1.1.7.3		sop
MultiframeTrueColorSecondaryCaptureImageStorage      1.2.840.10008.5.1.4.1.1.7.4		sop
RETIRED_StandaloneOverlayStorage                     1.2.840.10008.5.1.4.1.1.8		sop
RETIRED_StandaloneCurveStorage                       1.2.840.10008.5.1.4.1.1.9		sop
DRAFT_WaveformStorage                                1.2.840.10008.5.1.4.1.1.9.1		sop
TwelveLeadECGWaveformStorage                         1.2.840.10008.5.1.4.1.1.9.1.1		sop
GeneralECGWaveformStorage                            1.2.840.10008.5.1.4.1.1.9.1.2		sop
AmbulatoryECGWaveformStorage                         1.2.840.10008.5.1.4.1.1.9.1.3		sop
HemodynamicWaveformStorage                           1.2.840.10008.5.1.4.1.1.9.2.1		sop
CardiacElectrophysiologyWaveformStorage              1.2.840.10008.5.1.4.1.1.9.3.1		sop
BasicVoiceAudioWaveformStorage                       1.2.840.10008.5.1.4.1.1.9.4.1		sop
GeneralAudioWaveformStorage                          1.2.840.10008.5.1.4.1.1.9.4.2		sop
ArterialPulseWaveformStorage                         1.2.840.10008.5.1.4.1.1.9.5.1		sop
RespiratoryWaveformStorage                           1.2.840.10008.5.1.4.1.1.9.6.1		sop
RETIRED_StandaloneModalityLUTStorage                 1.2.840.10008.5.1.4.1.1.10		sop
RETIRED_StandaloneVOILUTStorage                      1.2.840.10008.5.1.4.1.1.11		sop
GrayscaleSoftcopyPresentationStateStorage            1.2.840.10008.5.1.4.1.1.11.1		sop
ColorSoftcopyPresentationStateStorage                1.2.840.10008.5.1.4.1.1.11.2		sop
PseudoColorSoftcopyPresentationStateStorage          1.2.840.10008.5.1.4.1.1.11.3		sop
BlendingSoftcopyPresentationStateStorage             1.2.840.10008.5.1.4.1.1.11.4		sop
XAXRFGrayscaleSoftcopyPresentationStateStorage       1.2.840.10008.5.1.4.1.1.11.5		sop
RETIRED_XASinglePlaneStorage                         1.2.840.10008.5.1.4.1.1.12	sop
XRayAngiographicImageStorage                         1.2.840.10008.5.1.4.1.1.12.1		sop
EnhancedXAImageStorage                               1.2.840.10008.5.1.4.1.1.12.1.1		sop
XRayRadiofluoroscopicImageStorage                    1.2.840.10008.5.1.4.1.1.12.2		sop
EnhancedXRFImageStorage                              1.2.840.10008.5.1.4.1.1.12.2.1		sop
RETIRED_XRayAngiographicBiPlaneImageStorage          1.2.840.10008.5.1.4.1.1.12.3		sop
XRay3DAngiographicImageStorage                       1.2.840.10008.5.1.4.1.1.13.1.1		sop
XRay3DCraniofacialImageStorage                       1.2.840.10008.5.1.4.1.1.13.1.2		sop
#BreastTomosynthesisImageStorage                      1.2.840.10008.5.1.4.1.1.13.1.3		sop
NuclearMedicineImageStorage                          1.2.840.10008.5.1.4.1.1.20		sop
RawDataStorage                                       1.2.840.10008.5.1.4.1.1.66		sop
SpatialRegistrationStorage                           1.2.840.10008.5.1.4.1.1.66.1		sop
SpatialFiducialsStorage                              1.2.840.10008.5.1.4.1.1.66.2		sop
DeformableSpatialRegistrationStorage                 1.2.840.10008.5.1.4.1.1.66.3		sop
SegmentationStorage                                  1.2.840.10008.5.1.4.1.1.66.4		sop
SurfaceSegmentationStorage                           1.2.840.10008.5.1.4.1.1.66.5		sop
RealWorldValueMappingStorage                         1.2.840.10008.5.1.4.1.1.67		sop
RETIRED_VLImageStorage                               1.2.840.10008.5.1.4.1.1.77.1		sop
VLEndoscopicImageStorage                             1.2.840.10008.5.1.4.1.1.77.1.1		sop
VideoEndoscopicImageStorage                          1.2.840.10008.5.1.4.1.1.77.1.1.1		sop
VLMicroscopicImageStorage                            1.2.840.10008.5.1.4.1.1.77.1.2		sop
VideoMicroscopicImageStorage                         1.2.840.10008.5.1.4.1.1.77.1.2.1		sop
VLSlideCoordinatesMicroscopicImageStorage            1.2.840.10008.5.1.4.1.1.77.1.3		sop
VLPhotographicImageStorage                           1.2.840.10008.5.1.4.1.1.77.1.4		sop
VideoPhotographicImageStorage                        1.2.840.10008.5.1.4.1.1.77.1.4.1		sop
OphthalmicPhotography8BitImageStorage                1.2.840.10008.5.1.4.1.1.77.1.5.1		sop
OphthalmicPhotography16BitImageStorage               1.2.840.10008.5.1.4.1.1.77.1.5.2		sop
StereometricRelationshipStorage                      1.2.840.10008.5.1.4.1.1.77.1.5.3		sop
OphthalmicTomographyImageStorage                     1.2.840.10008.5.1.4.1.1.77.1.5.4		sop
VLWholeSlideMicroscopyImageStorage                   1.2.840.10008.5.1.4.1.1.77.1.6		sop
RETIRED_VLMultiFrameImageStorage                     1.2.840.10008.5.1.4.1.1.77.2		sop
LensometryMeasurementsStorage                        1.2.840.10008.5.1.4.1.1.78.1		sop
AutorefractionMeasurementsStorage                    1.2.840.10008.5.1.4.1.1.78.2		sop
KeratometryMeasurementsStorage                       1.2.840.10008.5.1.4.1.1.78.3		sop
SubjectiveRefractionMeasurementsStorage              1.2.840.10008.5.1.4.1.1.78.4		sop
VisualAcuityMeasurementsStorage                      1.2.840.10008.5.1.4.1.1.78.5		sop
SpectaclePrescriptionReportStorage                   1.2.840.10008.5.1.4.1.1.78.6		sop
OphthalmicAxialMeasurementsStorage                   1.2.840.10008.5.1.4.1.1.78.7		sop
IntraocularLensCalculationsStorage                   1.2.840.10008.5.1.4.1.1.78.8		sop
MacularGridThicknessAndVolumeReportStorage           1.2.840.10008.5.1.4.1.1.79.1		sop
OphthalmicVisualFieldStaticPerimetryMeasurementsSt.  1.2.840.10008.5.1.4.1.1.80.1		sop
DRAFT_SRTextStorage                                  1.2.840.10008.5.1.4.1.1.88.1		sop
DRAFT_SRAudioStorage                                 1.2.840.10008.5.1.4.1.1.88.2		sop
DRAFT_SRDetailStorage                                1.2.840.10008.5.1.4.1.1.88.3		sop
DRAFT_SRComprehensiveStorage                         1.2.840.10008.5.1.4.1.1.88.4		sop
BasicTextSRStorage                                   1.2.840.10008.5.1.4.1.1.88.11		sop
EnhancedSRStorage                                    1.2.840.10008.5.1.4.1.1.88.22		sop
ComprehensiveSRStorage                               1.2.840.10008.5.1.4.1.1.88.33		sop
ProcedureLogStorage                                  1.2.840.10008.5.1.4.1.1.88.40		sop
MammographyCADSRStorage                              1.2.840.10008.5.1.4.1.1.88.50		sop
KeyObjectSelectionDocumentStorage                    1.2.840.10008.5.1.4.1.1.88.59		sop
ChestCADSRStorage                                    1.2.840.10008.5.1.4.1.1.88.65		sop
XRayRadiationDoseSRStorage                           1.2.840.10008.5.1.4.1.1.88.67		sop
ColonCADSRStorage                                    1.2.840.10008.5.1.4.1.1.88.69		sop
ImplantationPlanSRDocumentStorage                    1.2.840.10008.5.1.4.1.1.88.70		sop
EncapsulatedPDFStorage                               1.2.840.10008.5.1.4.1.1.104.1		sop
EncapsulatedCDAStorage                               1.2.840.10008.5.1.4.1.1.104.2		sop
PositronEmissionTomographyImageStorage               1.2.840.10008.5.1.4.1.1.128		sop
RETIRED_StandalonePETCurveStorage                    1.2.840.10008.5.1.4.1.1.129		sop
EnhancedPETImageStorage                              1.2.840.10008.5.1.4.1.1.130		sop
BasicStructuredDisplayStorage                        1.2.840.10008.5.1.4.1.1.131		sop
RTImageStorage                                       1.2.840.10008.5.1.4.1.1.481.1		sop
RTDoseStorage                                        1.2.840.10008.5.1.4.1.1.481.2		sop
RTStructureSetStorage                                1.2.840.10008.5.1.4.1.1.481.3		sop
RTBeamsTreatmentRecordStorage                        1.2.840.10008.5.1.4.1.1.481.4		sop
RTPlanStorage                                        1.2.840.10008.5.1.4.1.1.481.5		sop
RTBrachyTreatmentRecordStorage                       1.2.840.10008.5.1.4.1.1.481.6		sop
RTTreatmentSummaryRecordStorage                      1.2.840.10008.5.1.4.1.1.481.7		sop
RTIonPlanStorage                                     1.2.840.10008.5.1.4.1.1.481.8		sop
RTIonBeamsTreatmentRecordStorage                     1.2.840.10008.5.1.4.1.1.481.9		sop
DRAFT_RTBeamsDeliveryInstructionStorage              1.2.840.10008.5.1.4.34.1		sop
GenericImplantTemplateStorage                        1.2.840.10008.5.1.4.43.1		sop
ImplantAssemblyTemplateStorage                       1.2.840.10008.5.1.4.44.1		sop
ImplantTemplateGroupStorage                          1.2.840.10008.5.1.4.45.1		sop
#GEMRStorage                                         1.2.840.113619.4.2		sop
#GECTStorage                                         1.2.840.113619.4.3		sop
GE3DModelObjectStorage                               1.2.840.113619.4.26		sop
GERTPlanStorage                                      1.2.840.113619.5.249		sop
GERTPlanStorage2                                     1.2.840.113619.4.5.249		sop
GESaturnTDSObjectStorage                             1.2.840.113619.5.253		sop
Philips3DVolumeStorage                               1.2.46.670589.5.0.1		sop
Philips3DObjectStorage                               1.2.46.670589.5.0.2		sop
PhilipsSurfaceStorage                                1.2.46.670589.5.0.3		sop
PhilipsCompositeObjectStorage                        1.2.46.670589.5.0.4		sop
PhilipsMRCardioProfileStorage                        1.2.46.670589.5.0.7		sop
PhilipsMRCardioImageStorage                          1.2.46.670589.5.0.8		sop
PatientRootQuery                                     1.2.840.10008.5.1.4.1.2.1.1	sop
PatientRootRetrieve                                  1.2.840.10008.5.1.4.1.2.1.2	sop
PatientRootGet                                       1.2.840.10008.5.1.4.1.2.1.3	sop
StudyRootQuery                                       1.2.840.10008.5.1.4.1.2.2.1	sop
StudyRootRetrieve                                    1.2.840.10008.5.1.4.1.2.2.2	sop
StudyRootGet                                         1.2.840.10008.5.1.4.1.2.2.3	sop
PatientStudyOnlyQuery                                1.2.840.10008.5.1.4.1.2.3.1	sop
PatientStudyOnlyRetrieve                             1.2.840.10008.5.1.4.1.2.3.2	sop
PatientStudyOnlyGet                                  1.2.840.10008.5.1.4.1.2.3.3	sop
FindModalityWorkList                                 1.2.840.10008.5.1.4.31		sop
PatientRootRetrieveNKI                               1.2.826.0.1.3680043.2.135.1066.5.1.4.1.2.1.2	sop
StudyRootRetrieveNKI                                 1.2.826.0.1.3680043.2.135.1066.5.1.4.1.2.2.2	sop
PatientStudyOnlyRetrieveNKI                          1.2.826.0.1.3680043.2.135.1066.5.1.4.1.2.3.2	sop
BasicGrayscalePrintManagementMeta                    1.2.840.10008.5.1.1.9			sop
BasicColorPrintManagementMeta                        1.2.840.10008.5.1.1.18			sop
BasicFilmSession                                     1.2.840.10008.5.1.1.1			sop
BasicFilmBox                                         1.2.840.10008.5.1.1.2			sop
BasicGrayscaleImageBox                               1.2.840.10008.5.1.1.4			sop
BasicColorImageBox                                   1.2.840.10008.5.1.1.4.1			sop
BasicPrinter                                         1.2.840.10008.5.1.1.16			sop
LittleEndianImplicit                                 1.2.840.10008.1.2	transfer
]]..s..[[LittleEndianExplicit                                 1.2.840.10008.1.2.1	transfer
]]..s..[[BigEndianExplicit                                    1.2.840.10008.1.2.2	transfer
]]..s..[[JPEGBaseLine1                                        1.2.840.10008.1.2.4.50	transfer	LittleEndianExplicit
]]..s..[[JPEGExtended2and4                                    1.2.840.10008.1.2.4.51	transfer	LittleEndianExplicit
#JPEGExtended3and5                                   1.2.840.10008.1.2.4.52	transfer	LittleEndianExplicit
]]..s..[[JPEGSpectralNH6and8                                  1.2.840.10008.1.2.4.53	transfer	LittleEndianExplicit
#JPEGSpectralNH7and9                                 1.2.840.10008.1.2.4.54	transfer	LittleEndianExplicit
]]..s..[[JPEGFulllNH10and12                                   1.2.840.10008.1.2.4.55	transfer	LittleEndianExplicit
#JPEGFulllNH11and13                                  1.2.840.10008.1.2.4.56	transfer	LittleEndianExplicit
]]..s..[[JPEGLosslessNH14                                     1.2.840.10008.1.2.4.57	transfer	LittleEndianExplicit
#JPEGLosslessNH15                                    1.2.840.10008.1.2.4.58	transfer	LittleEndianExplicit
#JPEGExtended16and18                                 1.2.840.10008.1.2.4.59	transfer	LittleEndianExplicit
#JPEGExtended17and19                                 1.2.840.10008.1.2.4.60	transfer	LittleEndianExplicit
#JPEGSpectral20and22                                 1.2.840.10008.1.2.4.61	transfer	LittleEndianExplicit
#JPEGSpectral21and23                                 1.2.840.10008.1.2.4.62	transfer	LittleEndianExplicit
#JPEGFull24and26                                     1.2.840.10008.1.2.4.63	transfer	LittleEndianExplicit
#JPEGFull25and27                                     1.2.840.10008.1.2.4.64	transfer	LittleEndianExplicit
#JPEGLossless28                                      1.2.840.10008.1.2.4.65	transfer	LittleEndianExplicit
#JPEGLossless29                                      1.2.840.10008.1.2.4.66	transfer	LittleEndianExplicit
]]..s..[[JPEGLossless                                         1.2.840.10008.1.2.4.70	transfer	LittleEndianExplicit
]]..s..[[JPEGLS_Lossless                                      1.2.840.10008.1.2.4.80	transfer	LittleEndianExplicit
]]..s..[[JPEGLS_Lossy                                         1.2.840.10008.1.2.4.81	transfer	LittleEndianExplicit
#RLELossless                                         1.2.840.10008.1.2.5	transfer	LittleEndianExplicit
#LittleEndianExplicitDeflated                        1.2.840.10008.1.2.1.99	transfer	LittleEndianExplicit
]]..s..[[JPEG2000LosslessOnly                                 1.2.840.10008.1.2.4.90	transfer	LittleEndianExplicit
]]..s..[[JPEG2000                                             1.2.840.10008.1.2.4.91	transfer	LittleEndianExplicit
]])
  end
end


-----------------------------------------
-----------------------------------------
-----------------------------------------

-- get OS
local osname
local fh,err = assert(io.popen("uname -o 2>/dev/null","r"))
if fh then
  osname = fh:read()
  fh:close()
end
osname = osname or "Windows"

-- test prerequisites

print('Checking prerequisites....')

print('[OK] Operating system: '..osname)
if osname=='Windows' then
  print('[ERROR] This script is intended for Linux only')
  os.exit()
end

runquiet('make --version >t.txt 2>nul')
local resp = {}
for v in io.lines('t.txt') do table.insert(resp, v) end
if resp[1] then print('[OK] '..resp[1])
else 
  toinstall= toinstall..' make'
  print('[ERROR] make not installed')
end

runquiet('g++ --version >t.txt 2>nul')
resp = {} 
for v in io.lines('t.txt') do table.insert(resp, v) end    
if resp[1] then print('[OK] '..resp[1])
else 
  toinstall= toinstall..' g++'
  print('[ERROR] no g++')
end

runquiet('gcc --version >t.txt 2>nul')
resp = {}
for v in io.lines('t.txt') do table.insert(resp, v) end
if resp[1] then print('[OK] '..resp[1])
else 
  print('[ERROR] no gcc')
end

runquiet('7za >t.txt 2>nul')
resp = {}
for v in io.lines('t.txt') do table.insert(resp, v) end
if resp[2] then print('[OK] '..resp[2])
else 
  toinstall= toinstall..' p7zip-full'
  print('[ERROR] no 7za')
end 

runquiet('unzip -v >t.txt 2>nul')
resp = {}
for v in io.lines('t.txt') do table.insert(resp, v) end
if resp[1] then print('[OK] '..resp[1])
else 
  toinstall= toinstall..' unzip'
  print('[ERROR] No unzip')
end

runquiet('git --version >t.txt 2>nul')
resp = {}
for v in io.lines('t.txt') do table.insert(resp, v) end
if resp[1] then print('[OK] '..resp[1])
else 
  toinstall= toinstall..' git'
  print('[ERROR] No git')
end

runquiet('mariadb --version >t.txt 2>nul')
resp = {}
for v in io.lines('t.txt') do table.insert(resp, v) end
if resp[1] then print('[OK] '..resp[1])
else 
  if database=='mariadb' then
    toinstall= toinstall..mariadb_packages
    print('[ERROR] No Mariadb')
  end
end

runquiet('psql --version >t.txt 2>nul')
resp = {}
for v in io.lines('t.txt') do table.insert(resp, v) end
if resp[1] then print('[OK] '..resp[1])
else 
  if database=='pgsql' then
    toinstall= toinstall..' postgresql libpq-dev'
    print('[ERROR] No PostgreSQL')
  end
end

runquiet('lua5.1 -v 2>t.txt 1>nul')
resp = {}
for v in io.lines('t.txt') do table.insert(resp, v) end
luaversion = string.match(resp[1], 'Lua (%d%.%d).*$')
if luaversion=='5.1' then 
  print('[OK] '..resp[1])
else 
  toinstall= toinstall..' lua5.1'
  print('[ERROR] No Lua5.1')
end

if not fileexists('/usr/lib/x86_64-linux-gnu/liblua5.1.so') then
  toinstall= toinstall..' liblua5.1-0 lua-socket'
end

runquiet('export PATH="/usr/sbin:$PATH"; apache2 -v >t.txt 2>nul')
resp = {}
for v in io.lines('t.txt') do table.insert(resp, v) end
if resp[1] then 
  print('[OK] Apache '..resp[1])
  runquiet('export PATH="/usr/sbin:$PATH"; a2query -m | grep "php"  >t.txt 2>nul')
  resp = {}
  for v in io.lines('t.txt') do table.insert(resp, v) end
  if resp[1] then print('[OK] Apache PHP '..resp[1])
    phpversion = string.match(resp[1], '.*php(%d%.%d).*$')
  else 
    toinstall= toinstall..' php libapache2-mod-php'
    print('[ERROR] No apache PHP')
  end
else 
  toinstall= toinstall..' apache2 php libapache2-mod-php'
  print('[ERROR] No apache2')
end

if fileexists('t.txt') then runquiet('rm t.txt') end
if fileexists('nul') then runquiet('rm nul') end

if toinstall~='' then
  local y = ask('Continue installing following packages? y/n: ', toinstall)
  if y=='y' then
    runquiet('sudo -S apt update')
    runquiet('sudo -S apt -y install'..toinstall)
  end
  
  -- create correctly named version of liblua5.1.so
  if not fileexists('/usr/lib/x86_64-linux-gnu/liblua5.1.so') then
    print('*** Updating liblua link')
    runquiet('sudo -S ln -s /usr/lib/x86_64-linux-gnu/liblua5.1.so.0 /usr/lib/x86_64-linux-gnu/liblua5.1.so')
  end

  -- get PHP version
  runquiet('a2query -m | grep "php"  >t.txt 2>nul')
  resp = {}
  for v in io.lines('t.txt') do table.insert(resp, v) end
  if resp[1] then
    phpversion = string.match(resp[1], '.*php(%d%.%d).*$')
  end
  
  print('*** Patching php settings for '..phpversion)
  runquiet([[sudo -S sed -i 's/AllowOverride None/AllowOverride All/g' /etc/apache2/apache2.conf]])
  runquiet([[sudo -S sed -i 's/memory_limit = 128M/memory_limit = 512M/g' /etc/php/]]..phpversion..[[/apache2/php.ini]])
  runquiet([[sudo -S sed -i 's/upload_max_filesize = 2M/upload_max_filesize = 250M/g' /etc/php/]]..phpversion..[[/apache2/php.ini]])
  runquiet([[sudo -S sed -i 's/post_max_size = 8M/post_max_size = 250M/g' /etc/php/]]..phpversion..[[/apache2/php.ini]])
  runquiet([[sudo -S a2enmod rewrite]])
  
  print('*** Restarting apache')
  runquiet([[sudo -S systemctl restart apache2]])

  print('[DONE] **** re-run script to continue ****')
  return
end

-- check some lua stuff
if pcall(function()require('socket') end) then
    print('[OK] lua-socket')
  --else
    --print('[ERROR] lua-socket')
  end
if pcall(function()require('iuplua') end) then
  print('[OK] iuplua')
--else
  --print('[ERROR] iuplua')
end

if pcall(function()require('persistence') end) then
  print('[OK] persistence.lua')
--else
  --print('[ERROR] persistence.lua')
end

if pcall(function()require('wx') end) then
  print('[OK] wx')
--else
--  print('[ERROR] wx')
end

if server then 
  if fileexists(server .. '/ConquestDICOMServer.exe') then
    print('[OK] Server is located at: '..server)
  else
    server = string.gsub(runquiet('cd ..;pwd'), '\n', '')
    if fileexists(server .. '/ConquestDICOMServer.exe') then
      print('[OK] Server is located at: '..server)
    else
      local y=ask('Server not found - download Conquest DICOM server? y/n: ')
      if y=='y' then
        servername=ask('Give server AE (also folder name) (CONQUESTSRV1): ')
	if servername=='' then servername='CONQUESTSRV1' end
	if directoryexists(servername) then
          local y=ask('Folder exists - overwite? Yes/No: ')
	  if y~='Yes' then os.exit() end
          runquiet('sudo -S rm -R '..servername)
	end
        runquiet('git clone https://github.com/marcelvanherk/Conquest-DICOM-Server '..servername)
	server = string.gsub(runquiet('cd '..servername..';pwd'), '\n', '')
	print('[OK] Server folder: '..server)
      else
        os.exit()
      end
    end
  end
end

myconf = {AE=servername, DB=database}
if database=='pgsql' then myconf.SE='conquest' end
if database=='mysql' then myconf.SE='conquest' end
if database=='mariadb' then myconf.SE='conquest' end
if database=='dbaseiii' then myconf.SE=server..'/data'..sep..'dbase'..sep end
if database=='sqlite' then myconf.SE=server..'/data'..sep..'dbase'..sep..'conquest.db3' end
if database=='null' then myconf.SE='' end

if recompile and fileexists(server..'/dgate') then 
  os.remove(server..'/dgate')
  compile('dgateall', myconf, server)
  if fileexists(server..'/dgate') then
    print('[OK] server compiled')
  end
end

if fileexists(server..'/dgate') then
  print('[OK] server already compiled, delete dgate if need to recompile')
else
  local y=ask('Compile the server (this takes a while) y/n: ')
  if y=='y' then
    compile('dgateall', myconf, server)
    if fileexists(server..'/dgate') then
      print('[OK] server compiled')
    end
  end
end

if fileexists(server..'/dicom.ini') and not reconfigure then
  print('[OK] server already configured, delete dicom.ini to reconfigure')
  servername = getfile(server..'/dicom.ini', 'MyACRNema *= (.*)')
  serverport = getfile(server..'/dicom.ini', 'TCPPort *= (.*)')
  local sqlite, dbaseiii, mysql, pgsql, mariadb
  sqlite=getfile(server..'/dicom.ini', 'SqLite *= (.*)') or '0'
  dbaseiii=getfile(server..'/dicom.ini', 'DbaseIII *= (.*)') or '0'
  mariadb=getfile(server..'/dicom.ini', 'MySQL *= (.*)') or '0'
  pgsql=getfile(server..'/dicom.ini', 'Postgres *= (.*)') or '0'
  if sqlite then database='sqlite' end
  if dbaseiii then database='dbaseiii' end
  if mysql then database='mysql' end
  if mariadb then database='mariadb' end
  if pgsql then database='pgsql' end
else
  runquiet('sudo -S systemctl stop '..servername..'.service')
  while true do
    serverport=ask('Give server port (5678): ')
    if serverport=='' then serverport='5678' end
    if string.find(runquiet('ss -l -n | grep :'..serverport), serverport) then
      print('[ERROR] This port is in use!')
    else
      break
    end
  end

  local y=ask('Configure the server? y/n: ')
  if y=='y' then
    myconf = {AE=servername, DB=database, PORT=serverport, htmlweb='/var/www/html'}
    if database=='pgsql' then myconf.SE='conquest' end
    if database=='mysql' then myconf.SE='conquest' end
    if database=='mariadb' then myconf.SE='conquest' end
    if database=='dbaseiii' then myconf.SE=server..'/data'..sep..'dbase'..sep end
    if database=='sqlite' then myconf.SE=server..'/data'..sep..'dbase'..sep..'conquest.db3' end
    if database=='null' then myconf.SE='' end

    if not fileexists(server..'/dicom.sql') then
      create_server_dicomsql(server)
    end
  
    if not fileexists(server..'/dgatesop.lst') then
      create_server_dgatesop('', server) -- enable jpeg by default
    else
      --
    end
  
    if not fileexists(server..'/dicom.ini') or reconfigure then
      create_server_dicomini(myconf, server)
      if database=='pgsql' then
        runquiet([[cd /;sudo -u postgres -H psql -c "create role conquest login password 'conquest' valid until 'infinity'" 2>/dev/null]])
        runquiet([[cd /;sudo -u postgres -H psql -c "create database conquest owner conquest" 2>/dev/null]])
      elseif database=='mariadb' or database=='mysql' then
        runquiet([[sudo mysql -e "create user if not exists 'conquest'@'localhost' identified by 'conquest'"]])
        runquiet([[sudo mysql -e "create database if not exists conquest"]])
        runquiet([[sudo mysql -e "grant all on conquest.* to 'conquest'@'localhost'"]])
      end
    else
      local sqlite, dbaseiii, mysql, pgsql, mariadb
      sqlite=0; if (myconf.DB or CGI('DB', ''))=='sqlite' then dbaseiii=0; sqlite=1 end
      dbaseiii=1; if (myconf.DB or CGI('DB', ''))=='dbaseiii' then dbaseiii=1 end
      mysql=0; if (myconf.DB or CGI('DB', ''))=='mysql' then dbaseiii=0; mysql=1 end
      mariadb=0; if (myconf.DB or CGI('DB', ''))=='mariadb' then dbaseiii=0; mariadb=1 end
      pgsql=0; if (myconf.DB or CGI('DB', ''))=='pgsql' then dbaseiii=0; pgsql=1 end
      local doublebackslashtodb = 0
      local useescapestringconstants = 0
      if (myconf.SE or CGI('SE', ''))=='' then dbaseiii=0 end
      if pgsql==1 then useescapestringconstants=1 end
      if mysql==1 or pgsql==1 or mariadb==1 then doublebackslashtodb=1 end
  
      editfile(server..'/dicom.ini', '(MyACRNema *= ).*', '%1' .. (myconf.AE or CGI('AE', servername)))
      editfile(server..'/dicom.ini', '(TCPPort *= ).*', '%1' .. (myconf.PORT or CGI('PORT', serverport)))
                        
      editfile(server..'/dicom.ini', '(DroppedFileCompression *= ).*', '%1' .. (myconf.CP or CGI('CP', serverport)))
      editfile(server..'/dicom.ini', '(IncomingCompression *= ).*', '%1' .. (myconf.CP or CGI('CP', serverport)))
                        
      editfile(server..'/dicom.ini', '(FileNameSyntax *= ).*', '%1' .. (myconf.FN or CGI('FN', '9')))
      editfile(server..'/dicom.ini', '(MAGDevice0 *= ).*', '%1' .. (myconf.M0 or CGI('M0', server .. '/data' .. sep)))
                        
      editfile(server..'/dicom.ini', '(SqLite *= ).*', '%1' .. sqlite)
      editfile(server..'/dicom.ini', '(MySQL *= ).*', '%1' .. math.max(mariadb, mysql))
      editfile(server..'/dicom.ini', '(Postgres *= ).*', '%1' .. pgsql)
      editfile(server..'/dicom.ini', '(DoubleBackSlashToDB *= ).*', '%1' .. doublebackslashtodb)
      editfile(server..'/dicom.ini', '(UseEscapeStringConstants *= ).*', '%1' .. useescapestringconstants)
      editfile(server..'/dicom.ini', '(SQLServer *= ).*', '%1' .. (myconf.SE or CGI('SE', '')))
                        
      editfile(server..'/dicom.ini', '(SQLHost *= ).*', '%1' .. (myconf.SH or CGI('SH', '127.0.0.1')))
      editfile(server..'/dicom.ini', '(Username *= ).*', '%1' .. (myconf.SU or CGI('SU', '')))
      editfile(server..'/dicom.ini', '(Password *= ).*', '%1' .. (myconf.SP or CGI('SP', '')))
    end
  
    if not fileexists(server..'/acrnema.map') then
      create_server_acrnema(myconf, server)
    else
      editfile(server..'/acrnema.map', '^([%d%a]+)%s+(127%.0%.0%.1)%s+(%d+)%s+([%d%a]+)', 
      (myconf.AE or CGI('AE', servername))..'		%2	'..(myconf.PORT or CGI('PORT', serverport))..'		%4')
    end

    if fileexists(server..'/dicom.ini') then
      print('[OK] server configured')
    else
      print('[OK] server NOT configured')
    end
  end
end

if fileexists('/var/www/html/app/newweb/dicom.ini') and not reconfigure then
  print('[OK] web server already configured, delete /var/www/html/app and /api to reconfigure')
else
  local y=ask('Configure the web server? y/n: ')
  myconf = {AE=servername, DB=database, PORT=serverport, htmlweb='/var/www/html'}
  if y=='y' then
    create_newweb_app(myconf, server)
    create_dicom_api(myconf, server)
    create_ohif_app(myconf, server)
    if fileexists('/var/www/html/app/newweb/dicom.ini') then
      print('[OK] web server configured')
    else
      print('[OK] web server NOT configured')
    end
  end
end

local y=ask('Regen the database, this can take a long time if there is loads of data? y/n: ')
if y=='y' then
  runquiet('mkdir -p '..server..'/data/dbase')
  print(runquiet(server..'/dgate -w'..server..' -v -r'))
end

local y=ask('Install and start the server? y/n: ')
if y=='y' then
  copyfile(server..'/conquest.service', servername..'.service')
  editfile(servername..'.service', '%${CONQUEST}', server) 

  if fileexists('/etc/systemd/system/'..servername..'.service') then
    runquiet('sudo -S systemctl stop '..servername..'.service')
    runquiet('sudo -S systemctl disable '..servername..'.service')
    runquiet('sudo -S rm /etc/systemd/system/'..servername..'.service')
    runquiet('sudo -S systemctl daemon-reload')
  end
  
  runquiet('sudo -S cp '..servername..'.service /etc/systemd/system/'..servername..'.service')
  runquiet('sudo -S systemctl daemon-reload')
  runquiet('sudo -S systemctl start '..servername..'.service')
  runquiet('sudo -S systemctl enable '..servername..'.service')
  if runquiet('sudo -S systemctl status '..servername..'.service') then
    print ('[OK] '..servername..'.service installed and running')
  end
end

local y=ask('Enter web interface? y/n: ')
if y=='y' then
  runquiet('sensible-browser http://localhost/app/newweb')
end
