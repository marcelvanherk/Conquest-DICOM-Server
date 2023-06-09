-- Service web page for conquest installation
-- 20170211 Marcel van Herk 
-- 20170212 Find server and web interface folders; config compression and FileNameSyntax
-- 20170212 Process files in service controller context for correct linux rights
-- 20170222 Added database config; layout
-- 20170222 Added dgatesop.lst, jpeg default enabled for now
-- 20170223 Also write all settings on create dicom.ini; first uidprefix to .1419.time
--          Block dbaseiii config; misses code to denormalize database
-- 20170224 Added dbtest; added simple acrnema.map editing
-- 20170225 Store config in config.ini for better speed; started on compile options for linux
-- 20170226 Added sendservercommand; uses file to allow returning long strings; changevis onload
-- 20170226 Added optional compile under windows; note: output somehow blocked after running amake.bat
-- 20170227 Compiles under linux; fix postgres config
-- 20170228 Compile now through table in status section; db defaults; fix compile on windows
-- 20170302 Move scm location to install folder; show config and compile if needed
-- 20170303 Configure more generic cgiweb; Added welcome mode that checks progress of installation
-- 20170304 Welcome is used to guide user through installation, warn if regen about to overwrite
--          Split off webconfig; chmod 777 dgate
-- 20170501 Release 1.4.19a
-- 20171013 Added missing " in dicom.sql's last line (not in release 1.4.19b)
-- 20180315 Added C-Get in dgatesop.lst generator
-- 20181110 Updated dgatesop.lst and dicom.sql version number
-- 20181111 Unconditional update web interface when configuring; fix name of a lua file
-- 20181115 Use mode=start in webserver link to work in ladle; skip web folder test for ladle
-- 20181117 Added luasocket to linux compile
-- 20181123 For 1.4.19c; Added configuration of web viewers
-- 20181124 Added readOnly and viewOnly web server config, make copyfile unconditional (!)
--          Potential (not yet developed) viewers: wadoviewer_starter, dwvviewer_starter, 
--          altviewer_starter, imagejviewer_starter, flexviewer_starter
-- 20181125 Start and stop conquestdicomserver in windows mode; stop requires modification of GUI
-- 20181126 Added config of Papaya viewer and Cornerstone viewer
-- 20181129 Copy acrnema.map to web server to deal with SeLinux security; added Iframe viewer
-- 20181229 Fix cornerstone_starter, iframe_starter, added dumpvars; take dwv and papaya out of study viewers
--          Added WebScriptAddress and WebCodeBase to cgi-bin/dicom.ini (for Weasis)
-- 20181230 Removed acrnema.map from web install; no longer needed
-- 20191019 Also copy jpg(s) in htdocs folder for web install
-- 20200110 For 1.5.0beta; add ';basedir/lua/?.lua' to package.path in dicom.ini; db rev 19
-- 20200203 Added lua5.1.dll to web install; copy database drivers when installing on windows
-- 20200215 Link against installed lua5.1 library in linux
-- 20200216 Remove luasocket, lua from compile linked externally
-- 20200216 Added -Wno-format-overflow
-- 20200218 Allow to run direcly from dgate as command line utility
-- 20200219 Reordered function to allow install from command line
-- 20200220 Report compiler and nmake versions
-- 20200226 Configuration parameters to top of file to support moved install functions
-- 20200226 Added htmlweb variable to fix that in linux htdocs is not filled
--          Added and tested subfunction e.g. ./dgatesmall -w. --dolua:service.lua subfunction compiledgates
-- 20200228 Prefer /var/www/html/ from command line for htmlweb if found
-- 20200308 Prefer /var/www/html/ from web interface, use passed temp, addlocalfile and attachfile exceptions
-- 20220829 Explictly state it configures CGI; Add jpg and ico to create_newweb_cgi
--          Added favicon, cgi install source is htdocs/app/newweb; avoid null tempdir
--          Write and configure /app/newweb, disable (but keep) config cgi interface
--          Updated version and date; note: temp is still null
-- 20230607 Install dicom api and ohif app, enable ohif links from newweb; read newweb setting from app folder

local server = 'unknown'
local sep, dsep = '\\', '\\\\'
local dgate  = 'dgate'
local cgiclient  = 'dgate'
local cgiweb = 'unknown'
local htmlweb = 'unknown'

function errorpage(s)
  HTML('Content-type: text/html\n\n');
  print(
  [[
  <HEAD><TITLE>Version ]].. (version or '-')..[[ Install</TITLE></HEAD>
  <BODY BGCOLOR='FFDFCF'>
  <H3>Conquest ]].. (version or '-') ..[[ Install</H3>
  <H3>]].. s .. [[</H3>
  </BODY>
  ]])
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

function runquiet(s)
  local f=io.popen(s); 
  if f then 
    return f:read('*all')
  else 
    return ''
  end
end

local runLocal=false

function statuspage(s)
  print([[<H3>]].. s .. [[</H3>]])
end

-- write file in context of the service control server (with more rights)
function create_server_file(f, s)
  if runLocal then
    f = io.open(f, [[wt]]); 
    f:write(s); 
    f:close()  
  else
    servercommand('lua:local s=[['..s..']]; f = io.open([['..f..']], [[wt]]); f:write(s); f:close()')
  end
end

-- edit file in context of the service control server (with more rights)
function editfile(f, mask, value)
  if runLocal then
    local s=''
    for x in io.lines(f) do 
      x = string.gsub(x, mask, value)
      s = s..x.."\\n" 
    end
    local h=io.open(f, [[wt]])
    if h then h:write(s) h:close() end
  else
    servercommand('lua:local mask=[['..mask..']]; local value=[['..value..']]; local s=[[]]; '..
    'for x in io.lines([['..f..']]) '.. 
    'do x = string.gsub(x, mask, value); s = s..x.."\\n" end; '..
    'local h=io.open([['..f..']], [[wt]]);'..
    'if h then h:write(s) h:close() end')
  end
end

-- test file exists in context of the service control server (with more rights)
function fileexists(f)
  if runLocal then
    local h=io.open(f)
    if h then 
      h:close() 
      return 1 
    end
  else
    return servercommand('lua:local h=io.open([['..f..']]);'..
    'if h then h:close() return 1 else return 0 end')=="1"
  end
end

-- copy file in context of the service control server (with more rights)
function copyfile(f, g)
  if runLocal then
    local h=io.open(f, [[rb]])
    local s=h:read([[*a]])
    h:close()
    h=io.open(g, [[wb]])
    h:write(s)
    h:close()
  else
    servercommand('lua:'..
    'local h=io.open([['..f..']], [[rb]]) '..
    'local s=h:read([[*a]]) '..
    'h:close() '..
  --  'h=io.open([['..g..']]) '..
  --  'if h==nil then '..
    '  local h=io.open([['..g..']], [[wb]]) '..
    '  h:write(s) '..
    '  h:close() '..
  --  'else '..
  --  'h:close() '..
  --  'end'
    '')
  end
end

-- read file item in context of the service control server (with more rights)
function getfile(f, mask)
  if runLocal then
    local y
    local h = io.open([['..f..']], [[rt]])
    if h then
      h:close()
      for x in io.lines([['..f..']]) do
        x = string.match(x, [['..mask..']])
        y = y or x
      end
    end
    return y
  else
    return servercommand('lua:'..
    'local y;'..
    'h = io.open([['..f..']], [[rt]]) '..
    'if h then '..
    '  h:close() '..
    '  for x in io.lines([['..f..']]) do'..
    '    x = string.match(x, [['..mask..']])'..
    '    y = y or x'..
    '  end '..
    'end '..
    'return y')
  end
end

-- read entire file in context of the service control server (with more rights)
function getfilecontents(f)
  if runLocal then
    local r
    local h = io.open(f)
    if h then
      r = h:read("*all")
      h:close()
      return r
    end
  else
    return servercommand('lua:'..
    'local r;'..
    'local h = io.open([['..f..']]) '..
    'if h then '..
    '  r = h:read("*all") '..
    '  h:close() '..
    ' s=("'..server..'x.txt") f=io.open(s, "wb") f:write(r) returnfile=s f:close() ' ..
    'end ')
  end
end

-- execute statement in context of the service control server (with more rights)
function sendservercommand(f)
  return servercommand("lua:f=io.popen([["..f.."]]) r=f:read('*all') f:close() s='"..server.."x.txt' f=io.open(s, 'wt') f:write(r) returnfile=s f:close()")
end

-- functions for initial server config
function create_server_dicomini()
  local sqlite, dbaseiii, mysql, pgsql
  sqlite=0; if CGI('DB', '')=='sqlite' then dbaseiii=0; sqlite=1 end
  dbaseiii=1; if CGI('DB', '')=='dbaseiii' then dbaseiii=1 end
  mysql=0; if CGI('DB', '')=='mysql' then dbaseiii=0; mysql=1 end
  pgsql=0; if CGI('DB', '')=='pgsql' then dbaseiii=0; pgsql=1 end
  local doublebackslashtodb = 0
  local useescapestringconstants = 0
  if CGI('SE', '')=='' then dbaseiii=0 end
  if pgsql==1 then useescapestringconstants=1 end
  if mysql==1 or pgsql==1 then doublebackslashtodb=1 end

  if not fileexists(server..'dicom.ini') then
    create_server_file(server..'dicom.ini', [[
# This file contains configuration information for the DICOM server
# Do not edit unless you know what you are doing
# Autogenerated for 1.5.0c

[sscscp]
MicroPACS                = sscscp

# Network configuration: server name and TCP/IP port#
MyACRNema                = ]]..CGI('AE', 'CONQUESTSRV1')..[[

TCPPort                  = ]]..CGI('PORT', '5678')..[[


# Host, database, username and password for database
SQLHost                  = ]]..CGI('SH', 'localhost')..[[

SQLServer                = ]]..CGI('SE', server..'database.db3')..[[

Username                 = ]]..CGI('SU', 'conquest')..[[

Password                 = ]]..CGI('SP', 'conquest')..[[

SqLite                   = ]]..sqlite..[[

MySQL                    = ]]..mysql..[[

Postgres                 = ]]..pgsql..[[

DoubleBackSlashToDB      = ]]..doublebackslashtodb..[[

UseEscapeStringConstants = ]]..useescapestringconstants..[[


# Configure server
ImportExportDragAndDrop  = 1
ZipTime                  = 05:
UIDPrefix                = 1.2.826.0.1.3680043.2.135.150.]]..os.time()..[[

EnableComputedFields     = 1

FileNameSyntax           = ]]..CGI('FN', '9')..[[


# Configuration of compression for incoming images and archival
DroppedFileCompression   = ]]..CGI('CP', 'un')..[[

IncomingCompression      = ]]..CGI('CP', 'un')..[[

ArchiveCompression       = as

# For debug information
PACSName                 = ]]..CGI('AE', 'CONQUESTSRV1')..[[

OperatorConsole          = 127.0.0.1
DebugLevel               = 0

# Configuration of disk(s) to store images
MAGDeviceFullThreshHold  = 30
MAGDevices               = 1
MAGDevice0               = ]]..server..[[data]]..sep..[[

[lua]
association = package.path=package.path..';'..Global.basedir..'lua/?.lua'

]])
  end
end

function create_newweb_cgi()
  local list = {'start','listpatients','liststudies','listseries','listimageswiththumbs','listimages','listtrials','listtrialpatients',
                 'wadostudyviewer','wadoseriesviewer','wadoviewerhelp','slice','weasis_starter',
                 'wadoviewer_starter', 'dwvviewer_starter', 'altviewer_starter', 'imagejviewer_starter',
	         'flexviewer_starter', 'papaya_starter', 'cornerstone_starter', 'iframe_starter', 'dumpvars',
		 'conquest', 'favicon' --jpg/ico
	       }

  if true then -- not fileexists(cgiweb..'newweb'..sep..'dicom.ini') then
    sendservercommand('mkdir '..cgiweb..'newweb')

    local s='exceptions='
    local source=server..'webserver'..sep..'htdocs'..sep..'app'..sep..'newweb'..sep
    local dest=cgiweb..'newweb'..sep

    for _,v in ipairs(list) do
      if fileexists(source..v..'.lua') then
        copyfile(   source..v..'.lua', dest..v..'.lua')
        s=s..v..','
      end
      if fileexists(source..v..'.php') then
        copyfile(   source..v..'.php', dest..v..'.php')
      end
      if fileexists(source..v) then
        copyfile(   source..v, dest..v)
      end
      if fileexists(source..v..'.jpg') then
        copyfile(   source..v..'.jpg', htmlweb..v..'.jpg')
        copyfile(   source..v..'.jpg', dest..v..'.jpg')
      end
      if fileexists(source..v..'.ico') then
        copyfile(   source..v..'.ico', htmlweb..v..'.ico')
        copyfile(   source..v..'.ico', dest..v..'.ico')
      end
    end
    s = s .. 'addlocalfile,attachfile'
    --s = string.sub(s, 1, #s-1)

    create_server_file(dest..'dicom.ini', [[
#mvh 20220829  for 1.5.0c

[sscscp]
MicroPACS                = sscscp
ACRNemaMap               = acrnema.map
Dictionary               = dgate.dic
WebServerFor             = 127.0.0.1
TCPPort                  = ]]..CGI('PORT', '5678')..[[

WebScriptAddress         = http://]].. server_name .. string.gsub(string.gsub(script_name, 'service', 'newweb'), 'dgatesmall', 'dgate') .. [[

WebCodeBase              = http://]].. server_name .. '/' .. [[

TempDir                  = ]]..(CGI('TEMP', 'c:\\temp') or 'temp') ..[[


[webdefaults]
size     = 560
dsize    = 0
compress = un
iconsize = 84
graphic  = jpg
readOnly = ]]..CGI('READONLY', '0')..[[

viewOnly = ]]..CGI('VIEWONLY', '0')..[[

viewer   = ]]..CGI('SVIEWER', 'wadoseriesviewer')..[[

studyviewer = ]]..CGI('VIEWER', 'wadostudyviewer')..[[


[DefaultPage]
source = *.lua

[AnyPage]
source = start.lua
]]..s)

    copyfile(source..'dgate.dic', dest..'dgate.dic')
    --copyfile(server..sep..'acrnema.map', cgiweb..'newweb'..sep..'acrnema.map')
    if cgiclient=='dgate.exe' then
      copyfile(server..'install32'..sep..cgiclient, dest..cgiclient)
      copyfile(server..'install32'..sep..'lua5.1.dll', dest..'lua5.1.dll')
    else
      sendservercommand('cp '..server..cgiclient..' '..dest..cgiclient)
      sendservercommand('chmod 777 '..dest..cgiclient)
    end
  end
end

function create_newweb_app()
  local list = {'start','listpatients','liststudies','listseries','listimageswiththumbs','listimages','listtrials','listtrialpatients',
                 'wadostudyviewer','wadoseriesviewer','wadoviewerhelp','slice','weasis_starter',
                 'wadoviewer_starter', 'dwvviewer_starter', 'altviewer_starter', 'imagejviewer_starter',
	         'flexviewer_starter', 'papaya_starter', 'cornerstone_starter', 'iframe_starter', 'dumpvars',
		 'conquest', 'favicon', --jpg/ico
		 'index', 'dgate', 'singlefilelogin', --php
		 '.htaccess',
	       }
  if true then -- not fileexists(cgiweb..'newweb'..sep..'dicom.ini') then
    sendservercommand('mkdir '..htmlweb..'app')
    sendservercommand('mkdir '..htmlweb..'app'..sep..'newweb')

    local s='exceptions='
    local source=server..'webserver'..sep..'htdocs'..sep..'app'..sep..'newweb'..sep
    local dest=htmlweb..'app'..sep..'newweb'..sep

    for _,v in ipairs(list) do
      if fileexists(source..v..'.lua') then
        copyfile(   source..v..'.lua', dest..v..'.lua')
        s=s..v..','
      end
      if fileexists(source..v..'.php') then
        copyfile(   source..v..'.php', dest..v..'.php')
      end
      if fileexists(source..v) then
        copyfile(   source..v, dest..v)
      end
      if fileexists(source..v..'.jpg') then
        copyfile(   source..v..'.jpg', htmlweb..v..'.jpg')
        copyfile(   source..v..'.jpg', dest..v..'.jpg')
      end
      if fileexists(source..v..'.ico') then
        copyfile(   source..v..'.ico', htmlweb..v..'.ico')
        copyfile(   source..v..'.ico', dest..v..'.ico')
      end
    end
    s = s .. 'addlocalfile,attachfile'
    --s = string.sub(s, 1, #s-1)

    create_server_file(dest..'config.php', [[
<?php
  $folder = '.';				// where are the newweb files
  $exe    = 'dgate -p]]..CGI('PORT', '5678')..[[ -q127.0.0.1';
  $quote  = '""';				// quotes in command line

  if (PHP_OS_FAMILY != 'Windows') {		// On Linux:
    $exe = './' . $exe;				// start as ./servertask
    $quote = '\"';				// quotes in command line
  }

  $userlogin = false;				// uses single file login system
  $wplogin   = false;				// uses wordpress login system
  $cors      = false;				// allow cross site use
]])

    create_server_file(dest..'dicom.ini', [[
#mvh 20220829  for 1.5.0c

[sscscp]
MicroPACS                = sscscp
ACRNemaMap               = acrnema.map
Dictionary               = dgate.dic
WebServerFor             = 127.0.0.1
TCPPort                  = ]]..CGI('PORT', '5678')..[[

WebScriptAddress         = /app/newweb/
WebCodeBase              = /
TempDir                  = ]]..(CGI('TEMP', 'c:\\temp') or 'temp') ..[[


[webdefaults]
size     = 560
dsize    = 0
compress = un
iconsize = 84
graphic  = jpg
readOnly = ]]..CGI('READONLY', '0')..[[

viewOnly = ]]..CGI('VIEWONLY', '0')..[[

viewer   = ]]..CGI('SVIEWER', 'wadoseriesviewer')..[[

studyviewer = ]]..CGI('VIEWER', 'wadostudyviewer')..[[

studylink = '<TD><A target="_blank" href=/app/ohif/viewer/{StudyInstanceUID}>Ohif</A>'
serieslink = '<TD><A target="_blank" href=/app/newweb/?mode=wadoseriesviewer&series={PatientID}:{SeriesInstanceUID}>View</A>'

[DefaultPage]
source = *.lua

[AnyPage]
source = start.lua
]]..s)

    copyfile(source..'dgate.dic', dest..'dgate.dic')
    if cgiclient=='dgate.exe' then
      copyfile(server..'install32'..sep..cgiclient, dest..cgiclient)
      copyfile(server..'install32'..sep..'lua5.1.dll', dest..'lua5.1.dll')
    else
      sendservercommand('cp '..server..cgiclient..' '..dest..cgiclient)
      sendservercommand('chmod 777 '..dest..cgiclient)
    end
  end
end

function create_dicom_api()
  local list = {'posters','test','rquery','index','qido','servertask','posters','wado','readme','.htaccess','Router'}
  local exts={'.html', '.lua', '', '.php', '.exe'}
  sendservercommand('mkdir '..htmlweb..'api')
  sendservercommand('mkdir '..htmlweb..'api'..sep..'dicom')

  local s='exceptions='
  local source=server..'webserver'..sep..'htdocs'..sep..'api'..sep..'dicom'..sep
  local dest=htmlweb..'api'..sep..'dicom'..sep

  for _,v in ipairs(list) do
    for _,w in ipairs(exts) do
      if fileexists(source..v..w) then
        copyfile(   source..v..w, dest..v..w)
      end
    end
  end

  create_server_file(dest..'config.php', [[
<?php
  $exe    = 'servertask -p]]..CGI('PORT', '5678')..[[ -q127.0.0.1';
  $quote  = '""';				// quotes in command line

  if (PHP_OS_FAMILY != 'Windows') {		// On Linux:
    $exe = './' . $exe;				// start as ./servertask
    $quote = '\"';				// quotes in command line
  }

  $userlogin = false;				// uses single file login system
  $wplogin   = false;				// uses wordpress login system
]])

  if cgiclient~='dgate.exe' then
    sendservercommand('chmod 777 '..dest..'servertask')
  end
end

function create_ohif_app()
  local list = {'ohif1.03_min','index','.htaccess'}
  local exts={'.html', '.js', ''}
  sendservercommand('mkdir '..htmlweb..'app')
  sendservercommand('mkdir '..htmlweb..'app'..sep..'ohif')

  local source=server..'webserver'..sep..'htdocs'..sep..'app'..sep..'ohif'..sep
  local dest=htmlweb..'app'..sep..'ohif'..sep

  for _,v in ipairs(list) do
    for _,w in ipairs(exts) do
      if fileexists(source..v..w) then
        copyfile(   source..v..w, dest..v..w)
      end
    end
  end
end

function create_server_dicomsql()
  if not fileexists(server..'dicom.sql') then
    create_server_file(server..'dicom.sql', [[
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

function create_server_acrnema()
  if not fileexists(server..'acrnema.map') then
    create_server_file(server..'acrnema.map', [[
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

]]..CGI('AE', 'CONQUESTSRV1')..[[	127.0.0.1	]]..CGI('PORT', '5678')..[[	un

V*	        	*               1234            un
W*	        	*               666             un
S*		        *               5678            un
]])
  end
end

function create_server_dgatesop(s)
s = s or '#'
  if not fileexists(server..'dgatesop.lst') then
    create_server_file(server..'dgatesop.lst', [[
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

-- (normally) web controlled functions
function subfunctions(param)
  if param=='start' then
    if string.find(sendservercommand(server..dgate.." -w"..server.." "..[[--echo:]]..getfile(server..'dicom.ini', 'MyACRNema *= (.*)') or 'CONQUESTSRV1') or '', 'UP')==nil then
      HTML('Server is starting ...')
      if dgate=='dgate' then
        servercommand("luastart:os.execute([["..server..dgate.." -w"..server.." -v]]); return false")
	--servercommand("lua:os.execute([[sudo enable conquest.service]])")
	--servercommand("lua:os.execute([[sudo start conquest.service]])")
      else
        servercommand("luastart:os.execute([[start /D "..server.." conquestdicomserver.exe".."]]); return false")
      end
    else
      HTML('Server was already running')
    end
    return param, true
  end
  -----------------------------------------
  if param=='stop' then
    if dgate=='dgate' then
      servercommand("lua:os.execute([["..server..dgate.." -w"..server.." --quit:]]); return false")
      --servercommand("lua:os.execute([[sudo disable conquest.service]])")
      --servercommand("lua:os.execute([[sudo stop conquest.service]])")
    else
      servercommand("lua:os.execute([["..server..dgate.." \"--lua:print('$ QUITGUI $')\"]]); return false")
    end
    statuspage('Server stopped')
    return param, true
  end
  -----------------------------------------
  if param=='regen' then
    local pats=tonumber(sendservercommand(server..dgate.." -w"..server.." "..[["--lua:a=dbquery('DICOMPatients', '*');return #(a or {})"]]) or 0)
    if pats==0 then
      servercommand("lua:os.execute([["..server..dgate.." -w"..server.." -v -r]]); return false")
      statuspage([[Database regen completed - click <a href=# onclick="javascript:servicecommand('welcome')">here</a> to refresh]])
    else
      statuspage([[*** Database contains ]]..pats..[[ patient(s) ***; click <a href=# onclick="javascript:servicecommand('regen!')">here</a> to confirm]])
    end
    return param, true
  end
  -----------------------------------------
  if param=='regen!' then
    servercommand("lua:os.execute([["..server..dgate.." -w"..server.." -v -r]]); return false")
    statuspage([[Database regen completed - click <a href=# onclick="javascript:servicecommand('welcome')">here</a> to refresh]])
    return param, true
  end
  -----------------------------------------
  if param=='dbtest' then
    HTML(sendservercommand(server..dgate.." -w"..server.." -v -o"))
    return param, true
  end
  
  -----------------------------------------
  -----------------------------------------
  if param=='compilejpeg6c' then
    if dgate=='dgate' then
      sendservercommand('mkdir '..server..'src/dgate/build')
      sendservercommand('rm '..server..'src/dgate/build/libjpeg.a')
      sendservercommand('cd '..server..'src/dgate/jpeg-6c ; ./configure; make; cp libjpeg.a '..server..'src/dgate/build')
    else
      sendservercommand('mkdir '..server..'src\\dgate\\build32')
      sendservercommand('mkdir '..server..'src\\dgate\\build64')
      sendservercommand('del '..server..'src\\dgate\\build64\\libjpeg.lib')
      sendservercommand('del '..server..'src\\dgate\\build32\\libjpeg.lib')
      HTML(sendservercommand('cd '..server..'src\\dgate\\jpeg-6c & call amake.bat'))
    end
    param = 'compile'
  end
  -----------------------------------------
  if param=='compilecharls' then
    if dgate=='dgate' then
      sendservercommand('mkdir '..server..'src/dgate/build')
      sendservercommand('rm '..server..'src/dgate/build/charls.o')
      HTML(sendservercommand('g++ -o '..server..'src/dgate/build/charls.o -c '..server..'src/dgate/charls/all.cpp -I'..server..'src/dgate/charls'))
    else
      sendservercommand('mkdir '..server..'src\\dgate\\build32')
      sendservercommand('mkdir '..server..'src\\dgate\\build64')
      sendservercommand('del '..server..'src\\dgate\\build64\\charls.obj')
      sendservercommand('del '..server..'src\\dgate\\build32\\charls.obj')
      HTML(sendservercommand('cd '..server..'src\\dgate\\charls & call amake.bat'))
    end
    param = 'compile'
  end
  -----------------------------------------
  if param=='compileopenjpeg' then
    if dgate=='dgate' then
      sendservercommand('mkdir '..server..'src/dgate/build')
      sendservercommand('rm '..server..'src/dgate/build/openjpeg.o')
      HTML(sendservercommand('gcc -o '..server..'src/dgate/build/openjpeg.o -c '..server..'src/dgate/openjpeg/all.c -I'..server..'src/dgate/openjpeg'))
    else
      sendservercommand('mkdir '..server..'src\\dgate\\build32')
      sendservercommand('mkdir '..server..'src\\dgate\\build64')
      sendservercommand('del '..server..'src\\dgate\\build64\\openjp2.obj')
      sendservercommand('del '..server..'src\\dgate\\build32\\openjp2.obj')
      HTML(sendservercommand('cd '..server..'src\\dgate\\openjpeg & call amake.bat'))
    end
    param = 'compile'
  end
  -----------------------------------------
  if param=='compiledicomlib' then
    if dgate=='dgate' then
      HTML("*** error: no separate compile needed")
    else
      sendservercommand('mkdir '..server..'src\\dgate\\build32')
      sendservercommand('mkdir '..server..'src\\dgate\\build64')
      sendservercommand('del '..server..'src\\dgate\\build64\\dicom.lib')
      sendservercommand('del '..server..'src\\dgate\\build32\\dicom.lib')
      sendservercommand('cd '..server..'src\\dgate\\dicomlib & call amake.bat')
    end
    param = 'compile'
  end
  -----------------------------------------
  if param=='compilelua' then
    if dgate=='dgate' then
      sendservercommand('mkdir '..server..'src/dgate/build')
      sendservercommand('rm '..server..'src/dgate/build/lua.o')
      HTML(sendservercommand('gcc -o '..server..'src/dgate/build/lua.o -c '..server..'src/dgate/lua_5.1.5/all.c -I'..server..'src/dgate/lua_5.1.5 -DLUA_USE_DLOPEN -DLUA_USE_POSIX'))
    else
      sendservercommand('mkdir '..server..'src\\dgate\\build32')
      sendservercommand('mkdir '..server..'src\\dgate\\build64')
      sendservercommand('del '..server..'src\\dgate\\build64\\lua.obj')
      sendservercommand('del '..server..'src\\dgate\\build32\\lua.obj')
      HTML(sendservercommand('cd '..server..'src\\dgate\\lua_5.1.5 & call amake.bat'))
    end
    param = 'compile'
  end
  -----------------------------------------
  if param=='compileluasocket' then
    if dgate=='dgate' then
      sendservercommand('chmod 777 '..server..'src/dgate/luasocket/amake.sh')
      HTML(sendservercommand('cd '..server..'src/dgate/luasocket|./amake.sh'))
      sendservercommand('cp '..server..'src/dgate/luasocket/luasocket.a '..server..'src/dgate/build')
    else
      sendservercommand('mkdir '..server..'src\\dgate\\build32')
      sendservercommand('mkdir '..server..'src\\dgate\\build64')
      sendservercommand('del '..server..'src\\dgate\\build64\\luasocket.lib')
      sendservercommand('del '..server..'src\\dgate\\build32\\luasocket.lib')
      HTML(sendservercommand('mkdir '..server..'src\\dgate\\build64 & mkdir '..server..'src\\dgate\\build32'..' & cd '..server..'src\\dgate\\luasocket & call amake.bat'))
    end
    param = 'compile'
  end
  -----------------------------------------
  if param=='compilesqlite3' then
    if dgate=='dgate' then
      sendservercommand('mkdir '..server..'src/dgate/build')
      sendservercommand('rm '..server..'src/dgate/build/sqlite3.o')
      HTML(sendservercommand('gcc -DTHREADSAFE=1 -DHAVE_USLEEP -o '..server..'src/dgate/build/sqlite3.o -c '..server..'src/dgate/sqlite3/sqlite3.c -I'..server..'src/dgate/sqlite3'))
    else
      sendservercommand('mkdir '..server..'src\\dgate\\build32')
      sendservercommand('mkdir '..server..'src\\dgate\\build64')
      sendservercommand('del '..server..'src\\dgate\\build64\\sqlite3.obj')
      sendservercommand('del '..server..'src\\dgate\\build32\\sqlite3.obj')
      HTML(sendservercommand('mkdir '..server..'src\\dgate\\build64 & mkdir '..server..'src\\dgate\\build32'..' & cd '..server..'src\\dgate\\sqlite3 & call amake.bat & echo completed'))
    end
    param = 'compile'
  end
  -----------------------------------------
  if param=='compiledgate' then
    if dgate=='dgate' then
      local dbo = server..'src/dgate/build/sqlite3.o '
      local dbl = ''
      local dbi = '-I'..server..'src/dgate/sqlite3 '
      local dbd = '-DUSESQLITE '
      if CGI('DB', '')=='mysql' then
        dbo = '-lmysqlclient '
        dbl = '-L/usr/local/mysql/lib -L/usr/lib/mysql '
        dbi = '-I/usr/local/mysql/include -I/usr/include/mysql '
        dbd = '-DUSEMYSQL '     
      end
      if CGI('DB', '')=='pgsql' then
        dbo = '-lpq '
        dbl = '-L/usr/local/pgsql/lib '
        dbi = '-I/usr/include/postgresql ' 
        dbd = '-DPOSTGRES '     
      end
      sendservercommand('mkdir '..server..'src/dgate/build')
      sendservercommand('rm '..server..'src/dgate/build/dgate')
      sendservercommand(
      'g++ -std=c++11 -DUNIX -DNATIVE_ENDIAN=1 '..
      '-Wno-multichar -Wno-write-strings -Wno-format-overflow -DHAVE_LIBJPEG '..
      '-DHAVE_LIBCHARLS '..
      '-DHAVE_LIBOPENJPEG2 '..
      dbd..
      dbo..
      dbi..
      dbl..
      --server..'src/dgate/build/lua.o '..
      --server..'src/dgate/build/luasocket.a '..
  
      server..'src/dgate/build/charls.o '..
      server..'src/dgate/build/openjpeg.o '..
  
      '-o '..server..'src/dgate/build/dgate '..
      '-lpthread -ldl '..
      '-I'..server..'src/dgate/src '..
      server..'src/dgate/src/total.cpp '..
      '-llua5.1 '..
      '-I'..server..'src/dgate/lua_5.1.5 '..
      '-I'..server..'src/dgate/dicomlib '..
  
      '-I'..server..'src/dgate/charls '..
      '-I'..server..'src/dgate/openjpeg '..
  
      '-I'..server..'src/dgate/jpeg-6c '..
      '-L'..server..'src/dgate/jpeg-6c -ljpeg '
      )
    else
      sendservercommand('del '..server..'src\\dgate\\build64\\dgate.exe')
      sendservercommand('del '..server..'src\\dgate\\build32\\dgate.exe')
      HTML(sendservercommand('mkdir '..server..'src\\dgate\\build64 & mkdir '..server..'src\\dgate\\build32'..' & cd '..server..'src\\dgate\\src & call amake.bat & echo completed'))
    end
    param = 'compile'
  end
  -----------------------------------------
  if param=='compiledgatesmall' then
    if dgate=='dgate' then
      sendservercommand('mkdir '..server..'src/dgate/build')
      sendservercommand('rm '..server..'src/dgate/build/dgatesmall')
      sendservercommand(
      'g++ -DUNIX -DNATIVE_ENDIAN=1 -Wno-write-strings -DNOINTJPEG '..
      '-Wno-multichar -Wno-format-overflow '..
      -- server..'src/dgate/build/lua.o '..
      -- server..'src/dgate/build/luasocket.a '..
      '-o '..server..'src/dgate/build/dgatesmall '..
      '-I'..server..'src/dgate/src '..
      server..'src/dgate/src/total.cpp '..
      '-llua5.1 -lpthread -ldl '..
      '-I'..server..'src/dgate/dicomlib '..
      '-I'..server..'src/dgate/lua_5.1.5 '..
      '-I'..server..'src/dgate/jpeg-6c '..
      '-L'..server..'src/dgate/jpeg-6c -ljpeg '
      )
    else
      sendservercommand('del '..server..'src\\dgate\\build64\\dgate.exe')
      sendservercommand('del '..server..'src\\dgate\\build32\\dgate.exe')
      HTML(sendservercommand('mkdir '..server..'src\\dgate\\build64 & mkdir '..server..'src\\dgate\\build32'..' & cd '..server..'src\\dgate\\src & call amake.bat & echo completed'))
    end
    param = 'compile'
  end
  -----------------------------------------
  local components, objects, description, build
  if param=='compile' or param=='donecompile' or param=='rebuildgate' then
    components = {'jpeg6c', 'openjpeg', 'charls', --[['lua',]] --[['luasocket',]] 'sqlite3', 'dgate'} -- , 'dgatesmall'}
    objects = {'libjpeg.a', 'openjpeg.o', 'charls.o', --[['lua.o',]] --[['luasocket.a',]] 'sqlite3.o', 'dgate', 'dgatesmall'}
    description = {'Jpeg codec', 'Jpeg2000 codec', 'JpegLS codec', --[['Lua interpreter',]] --[['Lua communication layer',]] 'Sqlite3 database', 'Final server binary', 'Service control app'}
  
    if dgate~='dgate' then
      components[8]=nil -- no dgatesmall in windows
      table.insert(components, 7, 'dicomlib')
      table.insert(description, 7, 'dicom library')
      objects = {'libjpeg.lib', 'openjp2.obj', 'charls.obj', --[['lua.obj',]] --[['luasocket.lib',]] 'sqlite3.obj', 'dicom.lib', 'dgate.exe'}
    end
    local bits = string.sub(dgate, 6, 7)
    build = server..'src/dgate/build'..bits..'/'
    if dgate~='dgate' then build = string.gsub(build, '/', sep) end
  end
  -----------------------------------------
  if param=='rebuildgate' then
    servercommand("lua:os.execute([["..server..dgate.." -w"..server.." --quit:]]); return false")
    if dgate=='dgate' then
      sendservercommand('rm '..server..'dgate')
      sendservercommand('rm '..build..'*')
    else
      sendservercommand('del '..server..dgate)
      sendservercommand('del '..build..'*.*')
    end
    param = 'compile'
  end
  -----------------------------------------
  if param=='compile' then
    HTML("<b>Compile server components in: "..build.."</b><br>")
    
    HTML('<table>')
    HTML('<td><b>Component</b><td><b>Description</b><td><b>Done</b><td><b>Actions</b>') 
    for i=1, #components do
      HTML('<tr>')
      HTML('<td>'..components[i]..'<td>'..description[i])
      if fileexists(build..objects[i]) then
        HTML('<td><b>yes</b>')
      else
        HTML('<td>')
      end
      HTML([[<td><a href=# onclick="status('Compiling (may take a while) ...'); javascript:servicecommand('compile]]..components[i]..[[&DB='+document.getElementById('oForm').SelectDatabase.value); return false;">Compile</a>]])
  
      if components[i]=='dgate' and dgate=='dgate' then    
        HTML('<td>Note: recompile needed when changing selected database below')
      end
    end
    HTML('</table>')
  
    HTML([[<br><a href=# onclick="javascript:servicecommand('rebuildgate'); return false;">[Rebuild]</a>]])
    HTML([[<br><a href=# onclick="javascript:servicecommand('donecompile'); setTimeout(returntowelcome, 2000); return false;">[Done]</a>]])
    return param, true
  end
  -----------------------------------------
  if param=='donecompile' then
    for i=1, #components do
      if components[i]=='dgate' then
        if fileexists(build..objects[i]) then
          servercommand("lua:os.execute([["..server..dgate.." -w"..server.." --quit:]]); return false")
          sendservercommand('cp '..server..'src/dgate/build/dgate '..server)
          statuspage('Compilation completed ...')
        else
          statuspage('*** Server did not compile correctly ***')
        end
      end
    end
    return param, true
  end
  
  -----------------------------------------
  -----------------------------------------
  if param=='editacrnema' then
    local r = getfilecontents(server..'acrnema.map')
    local c = string.match(r, "(%/%*.*%*%/\n)")
    local s = string.gsub(r, "%/%*.*%*%/\n", "")
    local t= split(s, '\n')
    local t2={}
    for i=1, #t do
      if t[i]~='' then
        t2[#t2+1] = t[i]
      end
    end
    s = string.gsub(CGI('data', ''), ',', '\t\t')
    if s~='null' then t2[tonumber(CGI('item', 9999))] = s end
    create_server_file(server..'acrnema.map', c..table.concat(t2, '\n'))
    param = 'acrnema'
  end
  -----------------------------------------
  if param=='deleteacrnema' then
    local r = getfilecontents(server..'acrnema.map')
    local c = string.match(r, "(%/%*.*%*%/\n)")
    local s = string.gsub(r, "%/%*.*%*%/\n", "")
    local t= split(s, '\n')
    local t2={}
    for i=1, #t do
      if t[i]~='' then
        t2[#t2+1] = t[i]
      end
    end
    table.remove(t2, tonumber(CGI('item', 9999)))
    create_server_file(server..'acrnema.map', c..table.concat(t2, '\n'))
    param = 'acrnema'
  end
  -----------------------------------------
  if param=='addacrnema' then
    local r = getfilecontents(server..'acrnema.map')
    local c = string.match(r, "(%/%*.*%*%/\n)")
    local s = string.gsub(r, "%/%*.*%*%/\n", "")
    local t= split(s, '\n')
    local t2={}
    local star
    for i=1, #t do
      if t[i]~='' then
        t2[#t2+1] = t[i]
        if (string.find(t[i], '%*')~=nil) and (star==nil) then star = #t2 end
      end
    end
    if star==nil then star = #t2+1 end
    s = string.gsub(CGI('data', ''), ',', '\t\t')
    if s~='null' then table.insert(t2, star, s) end
    create_server_file(server..'acrnema.map', c..table.concat(t2, '\n'))
    param = 'acrnema'
  end
  -----------------------------------------
  if param=='acrnema' then
    HTML("<b>Edit known dicom providers:</b><br>")
  
    local r = getfilecontents(server..'acrnema.map')
    local s = string.gsub(r, "%/%*.*%*%/\n", "")
    local t= split(s, '\n')
    local t2={}
    for i=1, #t do
      if t[i]~='' then
        t2[#t2+1] = t[i]
      end
    end
    HTML('<table>')
    HTML('<td><b>AE</b><td><b>IP</b><td><b>Port</b><td><b>Compression</b><td><b>Actions</b>') 
    for i=1, #t2 do
      HTML('<tr>')
      local s=string.gsub(t2[i], '%s+', ',')
      local t=string.gsub(t2[i], '%s+', '<td>')
      HTML([[<td>]]..t)
      
      if i>1 then 
        HTML([[<td><a href=# onclick="javascript:var s=prompt('Edit (AE,IP,Port,Compression)',']]..s..[[');servicecommand('editacrnema&item=]]..i..[[&data='+s); return false;">Edit</a>]]) 
        HTML([[<td><a href=# onclick="javascript:servicecommand('deleteacrnema&item=]]..i..[['); return false;">Delete</a>]]) 
      else
        HTML([[<td>(not allowed)]]) 
      end
    end
    HTML('</table>')
  
    HTML([[<td><a href=# onclick="javascript:var s=prompt('New entry (AE,IP,Port,Compression)','AE,IP,Port,Comp');servicecommand('addacrnema&data='+s); return false;">[Add new entry]</a>]]) 
  
    HTML([[<br><a href=# onclick="javascript:servicecommand('doneacrnema'); setTimeout(returntowelcome, 2000); return false;">[Done]</a>]])
    return param, true
  end
  -----------------------------------------
  if param=='doneacrnema' then
    servercommand("lua:os.execute([["..server..dgate.." -w"..server.." --read_amap:]]); return false")
    statuspage('Updated known dicom providers')
    return param, true
  end
  
  -----------------------------------------
  -----------------------------------------
  -----------------------------------------
  if param=='welcome' then
    local allok = true
    if not fileexists(server..dgate) and dgate=='dgate' then
      HTML([[Welcome to <b>Conquest DICOM server</b>. Compilation is needed; select database to use below and click here to start compiling: <a href=# onclick="javascript:servicecommand('compile'); return false;">[start compiling]</a><br>]])
      allok = false
    end
    if allok and not fileexists(server..'dicom.ini') then
      HTML([[Welcome to <b>Conquest DICOM server</b>. Configuration is needed, review settings below (defaults are OK to start) and click this link: <a href=# onclick="javascript:servicecommand('config'+
      '&AE='+document.getElementById('oForm').EditAE.value+
      '&PORT='+document.getElementById('oForm').EditPort.value+
      '&FN='+document.getElementById('oForm').SelectFileNameSyntax.value+
      '&CP='+document.getElementById('oForm').SelectCompression.value+
      '&M0='+document.getElementById('oForm').EditMAGDevice0.value+
      '&DB='+document.getElementById('oForm').SelectDatabase.value+
      '&SH='+document.getElementById('oForm').EditSQLHost.value+
      '&SE='+document.getElementById('oForm').EditSQLServer.value+
      '&SU='+document.getElementById('oForm').EditSQLUsername.value+
      '&SP='+document.getElementById('oForm').EditSQLPassword.value+
      ''
      ); setTimeout(returntowelcome, 2000); return false;">[install/config]</a><br>]])
      allok = false
    end 
    if allok and sendservercommand(server..dgate.." -w"..server.." "..[["--lua:a=dbquery('DICOMPatients', '*');return #(a or {})"]])=="0" then
      HTML([[Welcome to <b>Conquest DICOM server</b>. Database regen is needed; click here to perform: <a href=# onclick="javascript:servicecommand('regen'); setTimeout(returntowelcome, 2000); return false;">[regenerate database]</a><br>]])
      allok = false
    end
    if allok and string.find(sendservercommand(server..dgate.." -w"..server.." "..[[--echo:]]..getfile(server..'dicom.ini', 'MyACRNema *= (.*)') or 'CONQUESTSRV1') or '', 'UP')==nil then
      HTML([[Welcome to <b>Conquest DICOM server</b>. Server is not running; click here to start: <a href=# onclick="javascript:servicecommand('start'); setTimeout(returntowelcome, 2000); return false;">[Start server]</a><br>]])
      allok = false
    end
    
    if allok then
      HTML('<b>Server runtime information:</b><br><br>')
      local r = sendservercommand(server..dgate.." -w"..server.." --display_status:")
      if (r or '')=='' then
        HTML('Server not running')
      else
        HTML(string.gsub(r, "Old JPEG.*", ""))
        HTML('<br>')
        HTML(string.gsub(string.gsub(r, "DICOM.*codec=%d", ""), "; Images sent=.*", ""))
        HTML('<br>')
        HTML(string.gsub(string.gsub(r, ".*Images sent=", "Images sent="), "Activity:.*", ""))
        HTML('<br>')
        HTML(string.gsub(string.gsub(r, ".*Space on", "Space on"), "", ""))
      end
    end
  
    return param, true
  end
    
  -----------------------------------------
  -----------------------------------------
  if param=='config' then
    if dgate=='dgate64.exe' then
      copyfile(server..'install64\\dgate64.exe', server..'dgate64.exe')
      copyfile(server..'install64\\lua5.1.dll', server..'lua5.1.dll')
      copyfile(server..'install64\\libmysql64.dll', server..'libmysql64.dll')
      copyfile(server..'install64\\libpq64.dll', server..'libpq64.dll')
    elseif dgate=='dgate.exe' then
      copyfile(server..'install32\\dgate.exe', server..'dgate.exe')
      copyfile(server..'install32\\lua5.1.dll', server..'lua5.1.dll')
      copyfile(server..'install64\\libmysql.dll', server..'libmysql.dll')
      copyfile(server..'install64\\libeay32.dll', server..'libeay32.dll')
      copyfile(server..'install64\\libiconv-2.dll', server..'libiconv-2.dll')
      copyfile(server..'install64\\libintl-8.dll', server..'libintl-8.dll')
      copyfile(server..'install64\\libssh2.dll', server..'libssh2.dll')
      -- note 32 bits postgres DLLs not provided
    end
  
    if not fileexists(server..'dicom.sql') then
      create_server_dicomsql()
    end
  
    if not fileexists(server..'dgatesop.lst') then
      create_server_dgatesop('') -- enable jpeg by default
    else
      --
    end
  
    if not fileexists(server..'dicom.ini') then
      create_server_dicomini()
    else
      local sqlite, dbaseiii, mysql, pgsql
      sqlite=0; if CGI('DB', '')=='sqlite' then dbaseiii=0; sqlite=1 end
      dbaseiii=1; if CGI('DB', '')=='dbaseiii' then dbaseiii=1 end
      mysql=0; if CGI('DB', '')=='mysql' then dbaseiii=0; mysql=1 end
      pgsql=0; if CGI('DB', '')=='pgsql' then dbaseiii=0; pgsql=1 end
      local doublebackslashtodb = 0
      local useescapestringconstants = 0
      if CGI('SE', '')=='' then dbaseiii=0 end
      if pgsql==1 then useescapestringconstants=1 end
      if mysql==1 or pgsql==1 then doublebackslashtodb=1 end
  
      editfile(server..'dicom.ini', '(MyACRNema *= ).*', '%1' .. CGI('AE', 'CONQUESTSRV1'))
      editfile(server..'dicom.ini', '(TCPPort *= ).*', '%1' .. CGI('PORT', '5678'))
  
      editfile(server..'dicom.ini', '(DroppedFileCompression *= ).*', '%1' .. CGI('CP', '5678'))
      editfile(server..'dicom.ini', '(IncomingCompression *= ).*', '%1' .. CGI('CP', '5678'))
  
      editfile(server..'dicom.ini', '(FileNameSyntax *= ).*', '%1' .. CGI('FN', '9'))
      editfile(server..'dicom.ini', '(MAGDevice0 *= ).*', '%1' .. CGI('M0', server .. 'data' .. sep))
  
      editfile(server..'dicom.ini', '(SqLite *= ).*', '%1' .. sqlite)
      editfile(server..'dicom.ini', '(MySQL *= ).*', '%1' .. mysql)
      editfile(server..'dicom.ini', '(Postgres *= ).*', '%1' .. pgsql)
      editfile(server..'dicom.ini', '(DoubleBackSlashToDB *= ).*', '%1' .. doublebackslashtodb)
      editfile(server..'dicom.ini', '(UseEscapeStringConstants *= ).*', '%1' .. useescapestringconstants)
      editfile(server..'dicom.ini', '(SQLServer *= ).*', '%1' .. CGI('SE', ''))
  
      editfile(server..'dicom.ini', '(SQLHost *= ).*', '%1' .. CGI('SH', '127.0.0.1'))
      editfile(server..'dicom.ini', '(Username *= ).*', '%1' .. CGI('SU', ''))
      editfile(server..'dicom.ini', '(Password *= ).*', '%1' .. CGI('SP', ''))
    end
  
    if not fileexists(server..'acrnema.map') then
      create_server_acrnema()
    else
      editfile(server..'acrnema.map', '^([%d%a]+)%s+(127%.0%.0%.1)%s+(%d+)%s+([%d%a]+)', 
      CGI('AE', 'CONQUESTSRV1')..'		%2	'..CGI('PORT', '5678')..'		%4')
    end
  
    statuspage('Server configured ... ')
    return param, true
  end
  -----------------------------------------
  if param=='webconfig' then
    if true then -- not fileexists(cgiweb..'newweb'..sep..'dicom.ini') then
      -- ladle link
      if write then
        servercommand("lua:os.execute([["..server..dgate.." -w"..server..
        " --luastart:dofile('../lua/ladle.lua')]]); return false")
      else
        --create_newweb_cgi()
        create_newweb_app()
        create_ohif_app()
        create_dicom_api()
      end
    else
      editfile(htmlweb..'app'..sep..'newweb'..sep..'dicom.ini', '(^TCPPort *= ).*', '%1' .. CGI('PORT', '5678'))
      editfile(htmlweb..'app'..sep..'newweb'..sep..'dicom.ini', '(^viewer *= ).*', '%1' .. CGI('SVIEWER', 'wadoseriesviewer'))
      editfile(htmlweb..'app'..sep..'newweb'..sep..'dicom.ini', '(^seriesviewer *= ).*', '%1' .. CGI('VIEWER', 'wadostudyviewer'))
      editfile(htmlweb..'app'..sep..'newweb'..sep..'dicom.ini', '(^TempDir *= ).*', '%1' .. (CGI('TEMP', '') or 'temp'))
    end
    statuspage('Web server configured ... ')
    return param, true
  end
end

local localConfig = {
  AE='CONQUESTSRV1',
  PORT=5678,
  DB='sqlite',
  SH='',
  SU='',
  SP='',
  FN=3,
  CP='UN',
  M0='./Data',
  READONLY=0,
  VIEWONLY=0,
  VIEWER='wadoseriesviewer',
  SVIEWER='wadostudyviewer',
}

-- Running from the command line; if so test if things are setup correctly

if CGI('xxx', 'xxx')~='xxx' then
  package.path=package.path..';../lua/?.lua';
  if string.find(arg[1], '/') then
    package.cpath=package.cpath..';/usr/lib/lib?51.so';
  else
    package.cpath=package.cpath..';../clibs/?51.dll;../clibs/?/core.dll';
  end
  
  if arg[4]=='console' then
    dofile('../lua/console.wlua')
    return
  end

  print(arg[4], arg[5])

  CGI = function(key, default)
    if localConfig[key] then
      return localConfig[key]
    else
      return default
    end
  end

  HTML = function()
  end

  print('Not running from webserver, testing dependencies for install GUI:')
  runLocal=true
  
  -- locate server folder and detect OS type
  if not string.find(arg[1], '/') and runquiet('..\\install64\\dgate64.exe -w. "--dolua:print([[[OK] ]]..version)"')~='' then
    print('[OK] 64 bits Windows')
    sep = '\\' dsep = '\\\\'
    dgate  = 'dgate64.exe'
    cgiclient = 'dgate.exe'
    local f=io.popen('cd'); server = f:read('*all') f:close() server=string.match(server, '(.*)\\.-$')..sep
  elseif not string.find(arg[1], '/') and runquiet('..\\install32\\dgate.exe -w. "--dolua:print([[[OK] ]]..version)"')~='' then
    print('[OK] 32 bits Windows')
    sep = '\\' dsep = '\\\\'
    dgate  = 'dgate.exe'
    cgiclient = 'dgate.exe'
    local f=io.popen('cd'); server = f:read('*all') f:close() server=string.match(server, '(.*)\\.-$')..sep
  elseif runquiet('../install/dgatesmall -w. "--dolua:print([[[OK] ]]..version)"')~='' then
    print('[OK] Linux')
    sep = '/' dsep = '/'
    dgate  = 'dgate'
    cgiclient = 'dgate'
    local f=io.popen('pwd'); server = f:read('*all') f:close() server=string.match(server, '(.*)%/.-$')..sep
    if os.execute('../dgate -w. "--dolua:print([[[OK] ]]..version)"')~='' then
      print('[OK] Linux dgate compiled')
    else
      print('[OK] Linux dgate NOT compiled')
    end
  else
    print('[ERROR] Unsupported OS or incomplete installation folder')
  end
  print('[OK] Server in '..server)

  -- locate web server folders using a few possible locations
  if fileexists('c:\\xampp\\apache\\conf\\httpd.conf') then
    print('[OK] Webserver in c:\\xampp\\apache\\')
    cgiweb = 'c:\\xampp\\cgi-bin\\'
    htmlweb  = 'c:\\xampp\\htdocs\\'
  elseif os.rename('/var/www/html/', '/var/www/html/')==true then
    print('[OK] Webserver in /var/www/html/')
    htmlweb  = '/var/www/html/'
    if os.rename('/var/www/cgi-bin/', '/var/www/cgi-bin/')==true then
      print('[OK] Webserver cgi-bin in /var/www/cgi-bin/')
      cgiweb  = '/var/www/cgi-bin/'
    elseif os.rename('/usr/lib/cgi-bin/', '/usr/lib/cgi-bin/')==true then
      print('[OK] Webserver cgi-bin in /usr/lib/cgi-bin/')
      cgiweb  = '/usr/lib/cgi-bin/'
    end
  elseif os.rename('/var/www/', '/var/www/')==true then
    print('[OK] Webserver in /var/www/')
    htmlweb  = '/var/www/'
    if os.rename('/var/www/cgi-bin/', '/var/www/cgi-bin/')==true then
      print('[OK] Webserver cgi-bin in /var/www/cgi-bin/')
      cgiweb  = '/var/www/cgi-bin/'
    elseif os.rename('/usr/lib/cgi-bin/', '/usr/lib/cgi-bin/')==true then
      print('[OK] Webserver cgi-bin in /usr/lib/cgi-bin/')
      cgiweb  = '/usr/lib/cgi-bin/'
    end
  elseif os.rename('/srv/www/', '/srv/www/')==true then
    print('[OK] Webserver in /srv/www')
    htmlweb  = '/srv/www/'
    if os.rename('/srv/www/cgi-bin/', '/srv/www/cgi-bin/')==true then
      print('[OK] Webserver cgi-bin in /srv/www/cgi-bin/')
      cgiweb  = '/srv/www/cgi-bin/'
    elseif os.rename('/usr/lib/cgi-bin/', '/usr/lib/cgi-bin/')==true then
      print('[OK] Webserver cgi-bin in /usr/lib/cgi-bin/')
      cgiweb  = '/usr/lib/cgi-bin/'
    end
  elseif os.rename('/srv/http/', '/srv/http/')==true then
    print('[OK] Webserver in /srv/http')
    htmlweb  = '/srv/http/'
    if os.rename('/srv/cgi-bin/', '/srv/cgi-bin/')==true then
      print('[OK] Webserver cgi-bin in /srv/cgi-bin/')
      cgiweb  = '/srv/cgi-bin/'
    elseif os.rename('/usr/lib/cgi-bin/', '/usr/lib/cgi-bin/')==true then
      print('[OK] Webserver cgi-bib in /usr/lib/cgi-bin/')
      cgiweb  = '/usr/lib/cgi-bin/'
    end
  end

  if arg[4]=='subfunction' then
    subfunctions(arg[5])
    return
  end

  if pcall(function()require('socket') end) then
    print('[OK] lua-socket')
  else
    print('[ERROR] lua-socket')
  end
  if pcall(function()require('iuplua') end) then
    print('[OK] iuplua')
  else
    print('[ERROR] iuplua')
  end
  if pcall(function()require('persistence') end) then
    print('[OK] persistence.lua')
  else
    print('[ERROR] persistence.lua')
  end
  
  if fileexists('../dicom.ini') then
    print('[OK] server configured')
  else
    print('[OK] server NOT configured')
  end

  local s = servercommand("lua:return 'service control software is running'")
  if s=='service control software is running' then
    print('[OK] server running')
  else
    print('[OK] server NOT running')
  end

  if Global then
    print('[OK] running from Dgate')
  else
    print('[ERROR] must be run from Dgate')
  end
  
  if sep=='\\' then
    local bat=[[
      @echo off
      call ..\src\dgate\asetcompiler32.bat
      "%compiler%\%ccompiler%" 1>nul 2>t.txt
      type t.txt
      del t.txt
    ]]
    create_server_file('t.bat', bat)
    local a = string.match(runquiet('t.bat'), 'Version (.+)\nCopyright')
    if a then 
      print('[OK] C32 '..a)
    else
      print('[ERROR] C32') 
    end

    bat=[[
      @echo off
      call ..\src\dgate\asetcompiler64.bat
      "%compiler%\%ccompiler%" 1>nul 2>t.txt
      type t.txt
      del t.txt
    ]]
    create_server_file('t.bat', bat)
    a = string.match(runquiet('t.bat'), 'Version (.+)\nCopyright')
    if a then 
      print('[OK] C64 '..a) 
    else
      print('[ERROR] C64') 
    end

    bat=[[
      @echo off
      call ..\src\dgate\asetcompiler32.bat
      "%compiler%\nmake" 1>nul 2>t.txt
      type t.txt
      del t.txt
    ]]
    create_server_file('t.bat', bat)
    a = string.match(runquiet('t.bat'), 'Version (.+)\nCopyright')
    if a then 
      print('[OK] M32 '..a)
    else
      print('[ERROR] M32') 
    end

    bat=[[
      @echo off
      call ..\src\dgate\asetcompiler64.bat
      "%compiler%\nmake" 1>nul 2>t.txt
      type t.txt
      del t.txt
    ]]
    create_server_file('t.bat', bat)
    a = string.match(runquiet('t.bat'), 'Version (.+)\nCopyright')
    if a then 
      print('[OK] M64 '..a)
    else
      print('[ERROR] M64') 
    end
  else
    runquiet('make --version >t.txt 2>nul')
    local resp = {}
    for v in io.lines('t.txt') do table.insert(resp, v) end
    if resp[1] then print('[OK] '..resp[1])
    else print('[ERROR] no make; install with sudo apt install make')
    end

    runquiet('g++ --version >t.txt 2>nul')
    resp = {} 
    for v in io.lines('t.txt') do table.insert(resp, v) end    
    if resp[1] then print('[OK] '..resp[1])
    else print('[ERROR] no g++; install with sudo apt install g++')
    end

    runquiet('gcc --version >t.txt 2>nul')
    resp = {}
    for v in io.lines('t.txt') do table.insert(resp, v) end
    if resp[1] then print('[OK] '..resp[1])
    else print('[ERROR] no gcc')
    end

    runquiet('7za >t.txt 2>nul')
    resp = {}
    for v in io.lines('t.txt') do table.insert(resp, v) end
    if resp[2] then print('[OK] '..resp[2])
    else print('[ERROR] no 7za; install with sudo apt install p7zip-full')
    end 

    runquiet('unzip -v >t.txt 2>nul')
    resp = {}
    for v in io.lines('t.txt') do table.insert(resp, v) end
    if resp[1] then print('[OK] '..resp[1])
    else print('[ERROR] No unzip; install with sudo apt install unzip')
    end

    runquiet('lua5.1 -v 2>t.txt 1>nul')
    resp = {}
    for v in io.lines('t.txt') do table.insert(resp, v) end
    if not string.find(resp[1], 'not found') then print('[OK] '..resp[1])
    else print('[ERROR] No Lua5.1; install with sudo apt install lua5.1')
    end

    runquiet('apache2 -v >t.txt 2>nul')
    resp = {}
    for v in io.lines('t.txt') do table.insert(resp, v) end
    if resp[1] then print('[OK] Apache '..resp[1])
    else print('[ERROR] No apache2')
    end

    runquiet('a2query -m | grep "cgi"  >t.txt 2>nul')
    resp = {}
    for v in io.lines('t.txt') do table.insert(resp, v) end
    if resp[1] then print('[OK] Apache CGI '..resp[1])
    else print('[ERROR] No apache CGI; enable with sudo a2enmod cgid')
    end

    runquiet('sudo chmod 777 ../webviewer.sh')
    runquiet('sudo chmod 777 ../maklinux')
  end

  create_server_dicomini()
  
  --local t = io.read()
  --print(t)
  
  -- arg={};arg[1]=[[8086]];dofile('../lua/ladle.lua')
  return;
end

local s = servercommand("lua:return 'service control software is running'")
if s~='service control software is running' then
  errorpage('service control software is NOT running, check windows.bat or linux.sh')
  return
end

-- locate server (assumes web interface on same machine)
if string.find(script_name, '%.exe') then
  server = servercommand([[lua:f=io.popen('cd'); return f:read('*all')]])
else
  server = servercommand([[lua:f=io.popen('pwd'); return f:read('*all')]])
end
server = string.gsub(server, '\\[^\\]-$', '\\')
server = string.gsub(server, '/[^/]-$', '/')

-- get separator character
if string.find(server, '/') then
  sep = '/'
  dsep = '/'
end

-- get configuration (cached)

if true or not fileexists(server..'config.ini') then
  create_server_file(server..'config.ini', [[
dgate = dgate
cgiclient = dgate
cgiweb = unknown
htmlweb = unknown
  ]])

  -- find out which dgate can run to determine OS and bits
  if servercommand([[lua:return(os.execute(']]..string.gsub(server, "\\", "\\\\")..[[install64\\dgate64.exe -w]]..string.gsub(server, "\\", "\\\\")..[[install -d'))]])=='0' then
    dgate  = 'dgate64.exe'
    cgiclient = 'dgate.exe'
  elseif servercommand([[lua:return(os.execute(']]..string.gsub(server, "\\", "\\\\")..[[install32\\dgate.exe -w]]..string.gsub(server, "\\", "\\\\")..[[install -d'))]])=='0' then
    dgate  = 'dgate.exe'
    cgiclient = 'dgate.exe'
  end
  
  -- locate web interface (cgi)
  local f
  if string.find(script_name, '%.exe') then
    f=io.popen('cd')
  else
    f=io.popen('pwd')
  end
  cgiweb=f:read('*all') 
  f:close()
  cgiweb = string.gsub(cgiweb, '\\[^\\]-$', '\\')
  cgiweb = string.gsub(cgiweb, '/[^/]-$', '/')

  -- locate web interface (www root)
  htmlweb = cgiweb .. '..' .. sep .. 'htdocs' .. sep
  if os.rename('/var/www/html/', '/var/www/html/')==true then
    htmlweb  = '/var/www/html/'
  elseif os.rename('/var/www/', '/var/www/')==true then
    htmlweb  = '/var/www/'
  elseif os.rename('/srv/www/', '/srv/www/')==true then
    htmlweb  = '/srv/www/'
  elseif os.rename('/srv/http/', '/srv/http/')==true then
    htmlweb  = '/srv/http/'
  end

  editfile(server..'config.ini', '(dgate *= ).*', '%1' .. dgate)
  editfile(server..'config.ini', '(cgiclient *= ).*', '%1' .. cgiclient)
  editfile(server..'config.ini', '(cgiweb *= ).*', '%1' .. cgiweb)
  editfile(server..'config.ini', '(htmlweb *= ).*', '%1' .. htmlweb)
else
  dgate = getfile(server..'config.ini', 'dgate *= (.*)') or 'unknown'
  cgiclient = getfile(server..'config.ini', 'cgiclient *= (.*)') or 'unknown'
  cgiweb = getfile(server..'config.ini', 'cgiweb *= (.*)') or 'unknown'
  htmlweb = getfile(server..'config.ini', 'htmlweb *= (.*)') or 'unknown'
end

-- generate page header
HTML("Content-type: text/html\nCache-Control: no-cache\n");

local param = CGI('parameter', '')
if param=='' then
  HTML("<HEAD>")
  print [[<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">]]
  HTML("<TITLE>Conquest DICOM control - version %s</TITLE>", version)
  HTML("</HEAD>")

  HTML([[
<SCRIPT language=JavaScript>
function servicecommand(a) {
  xmlhttp = new XMLHttpRequest(); 
  xmlhttp.open('GET',']]..cgiclient..[[?mode=service&parameter='+a, true);
  xmlhttp.timeout = 60000
  xmlhttp.send()
  xmlhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var s = this.responseText.toString()
      document.getElementById("serverstatus").innerHTML = s.replace("DICOM ERROR Protocol error 0 in PDU:Read", "");
    }
  }
}  

function status(a) {
  document.getElementById("serverstatus").innerHTML = a;
}

function changevis(a) {
  if (a=='sqlite'){
    document.getElementById('oForm').EditSQLHost.disabled = true;
    document.getElementById('oForm').EditSQLUsername.disabled = true;
    document.getElementById('oForm').EditSQLPassword.disabled = true;
  }
  else {
    document.getElementById('oForm').EditSQLHost.disabled = false;
    document.getElementById('oForm').EditSQLUsername.disabled = false;
    document.getElementById('oForm').EditSQLPassword.disabled = false;
  }
}

]])

HTML('function gup( name, url ) { // read cgi parameter')
HTML('	if (!url) url = location.href;')
HTML('	name = name.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");')
HTML('	var regexS = "[\\?&]"+name+"=([^&#]*)";')
HTML('	var regex = new RegExp( regexS );')
HTML('	var results = regex.exec( url );')
HTML('	return results == null ? null : results[1];')
HTML('}')

HTML([[
function changedefs(a) {
  if (a=='mysql'){
    document.getElementById('oForm').EditSQLHost.value = 'localhost';
    document.getElementById('oForm').EditSQLServer.value = 'conquest';
    document.getElementById('oForm').EditSQLUsername.value = 'root';
    document.getElementById('oForm').EditSQLPassword.value = 'user';
  }
  if (a=='pgsql'){
    document.getElementById('oForm').EditSQLHost.value = 'localhost';
    document.getElementById('oForm').EditSQLServer.value = 'conquest';
    document.getElementById('oForm').EditSQLUsername.value = 'postgres';
    document.getElementById('oForm').EditSQLPassword.value = 'postgres';
  }
  if (a=='sqlite'){
    document.getElementById('oForm').EditSQLHost.value = 'localhost';
    document.getElementById('oForm').EditSQLServer.value = ']]..
      string.gsub(server..'data'..sep, sep, dsep)..[[conquest.db3';
    document.getElementById('oForm').EditSQLUsername.value = '';
    document.getElementById('oForm').EditSQLPassword.value = '';
  }
}

window.onload = function() {
  changevis(document.getElementById('oForm').SelectDatabase.value);
  servicecommand('welcome');
}
  
returntowelcome = function() {
  servicecommand('welcome');
}

</SCRIPT>
]])

  HTML("<BODY BGCOLOR='CFDFDF'>")

  -- check config
  if not fileexists(server..'dgate.dic') then
    errorpage('server folder not found: ' .. server .. 'dgate.dic')
    return
  end
  if write==nil then
    if not fileexists(cgiweb..'service'..sep..'dgate.dic') then
      errorpage('web folder not found: ' .. cgiweb .. 'service'..sep..'dgate.dic')
      return
    end
  end
end


-----------------------------------------
-- subfunctions
-----------------------------------------
param = CGI('parameter', '')
if param~='' then
  local ret
  param, ret = subfunctions(param)
  if ret==true then return end
end

-------------------------------------
-- root install page
-------------------------------------

HTML("<H1>Conquest DICOM server %s - Installation</H1>", version)

HTML("<hr>")
HTML('<b>Configuration information:</b><br>')
HTML('<br>dgate executable = '..dgate)
HTML('<br>server folder = '..server)
HTML('<br>web interface = '..cgiweb..'newweb'..sep..cgiclient)
HTML('<br>web root = '..htmlweb)
HTML('<br>script_name = '..script_name)
HTML('<br>')

---------------------------------------

HTML('<p ID=serverstatus>')
HTML('Status will be displayed here - please wait ...')
HTML('</p>')
HTML("<hr>")

---------------------------------------

function selected(a, b)
  if (a=='xx') and (({un=1, j2=1, j3=1, js=1, j7=1, jk=1, jl=1})[b:lower()]==nil) then
    return ' selected="selected"'
  elseif (a=='yy') and (tonumber(b) or 999)>12 then
    return ' selected="selected"'
  elseif a:lower()==b:lower() then
    return ' selected="selected"'
  else
    return ''
  end
end

HTML('<b>Server configuration:</b><br><br>')
HTML("<FORM ID=oForm>");
HTML("<table>")

HTML("<tr>")

HTML("<td>AE<td><INPUT NAME=EditAE TYPE=Text VALUE=%s>", 
  getfile(server..'dicom.ini', 'MyACRNema *= (.*)') or 'CONQUESTSRV1')

local sqlite, dbaseiii, mysql, pgsql
sqlite=getfile(server..'dicom.ini', 'SqLite *= (.*)') or '0'
dbaseiii=getfile(server..'dicom.ini', 'DbaseIII *= (.*)') or '0'
mysql=getfile(server..'dicom.ini', 'MySQL *= (.*)') or '0'
pgsql=getfile(server..'dicom.ini', 'Postgres *= (.*)') or '0'
HTML([[<td>Database<td><SELECT NAME=SelectDatabase onchange="javascript:changevis(document.getElementById('oForm').SelectDatabase.value);changedefs(document.getElementById('oForm').SelectDatabase.value)">]])
HTML("<option value=sqlite"..selected("1", sqlite)..">SQLite (built-in)</option>")
-- HTML("<option value=dbaseiii"..selected("1", dbaseiii)..">DbaseIII (builtin)</option>")
HTML("<option value=mysql"..selected("1", mysql)..">MySQL (external)</option>")
HTML("<option value=pgsql"..selected("1", pgsql)..">Postgres (external)</option>")

HTML("</select><tr>")
HTML("<td>Port<td><INPUT NAME=EditPort TYPE=Text VALUE=%s>", 
getfile(server..'dicom.ini', 'TCPPort *= (.*)') or '5678')

HTML("<td>SQL host<td><INPUT NAME=EditSQLHost TYPE=Text VALUE=%s>", 
getfile(server..'dicom.ini', 'SQLHost *= (.*)') or 'localhost')

HTML("<tr>")
local sel =getfile(server..'dicom.ini', 'IncomingCompression *= (.*)') or 'un'
HTML("<td>Compression<td><SELECT NAME=SelectCompression>")
HTML("<option value=un"..selected("un", sel)..">Uncompressed</option>")
HTML("<option value=j2"..selected("j2", sel)..">JPEG Lossless</option>")
HTML("<option value=j3"..selected("j3", sel)..">JPEG Lossy</option>")
HTML("<option value=js"..selected("js", sel)..">JPEGLS Lossless</option>")
HTML("<option value=j7"..selected("j7", sel)..">JPEGLS Lossy</option>")
HTML("<option value=jk"..selected("jk", sel)..">JPEG2000 Lossless</option>")
HTML("<option value=jl"..selected("jl", sel)..">JPEG2000 Lossy</option>")
HTML("<option value=xx"..selected("xx", sel)..">Unknown</option>")
HTML("</SELECT>")

HTML("<td>SQL server<td><INPUT NAME=EditSQLServer TYPE=Text Size='50' VALUE=%s>", 
getfile(server..'dicom.ini', 'SQLServer *= (.*)') or server..[[data]]..sep..[[conquest.db3]])

HTML("<tr>")
sel =getfile(server..'dicom.ini', 'FileNameSyntax *= (.*)') or '9'
HTML("<td>FileNameSyntax<td><SELECT NAME=SelectFileNameSyntax>")
--HTML("<option value=0"..selected("0", sel)..">ID[8]_Name[8]\\Series#_Image#_Time.v2</option>")
--HTML("<option value=1"..selected("1", sel)..">ID[8]_Name[8]\\Series#_Image#_TimeCounter.v2</option>")
--HTML("<option value=2"..selected("2", sel)..">ID[8]_Name[8]\\Seriesuid_Series#_Image#_TimeCounter.v2</option>")
--HTML("<option value=3"..selected("3", sel)..">ID[16]\\Seriesuid_Series#_Image#_TimeCounter.v2</option>")
--HTML("<option value=4"..selected("4", sel)..">ID[16]\\Seriesuid_Series#_Image#_TimeCounter.dcm</option>")
--HTML("<option value=5"..selected("5", sel)..">Name\\Seriesuid_Series#_Image#_TimeCounter.v2</option>")
--HTML("<option value=6"..selected("6", sel)..">ID[32]\\Studyuid\\Seriesuid\\Imageuid.v2</option>")
--HTML("<option value=7"..selected("7", sel)..">Studyuid\\Seriesuid\\Imageuid.v2</option>")
HTML("<option value=8"..selected("8", sel)..">ID[32]\\Studyuid\\Seriesuid\\Imageuid.dcm</option>")
HTML("<option value=9"..selected("9", sel)..">Studyuid\\Seriesuid\\Imageuid.dcm</option>")
HTML("<option value=10"..selected("10", sel)..">Images\\Imageuid.dcm</option>")
HTML("<option value=11"..selected("11", sel)..">Name\\StudyUID\\SeriesUID\\Imageuid.dcm</option>")
HTML("<option value=12"..selected("12", sel)..">Name_ID\\Modality_StudyID\\SeriesID\\Imageuid.dcm</option>")
HTML("<option value=yy"..selected("yy", sel)..">Unknown</option>")
HTML("</SELECT>")

HTML("<td>SQL Username<td><INPUT NAME=EditSQLUsername TYPE=Text VALUE=%s>", 
getfile(server..'dicom.ini', 'Username *= (.*)') or '')

HTML("<tr>")

HTML("<td>Data (MAG0)<td><INPUT NAME=EditMAGDevice0 TYPE=Text Size='50' VALUE=%s>", 
getfile(server..'dicom.ini', 'MAGDevice0 *= (.*)') or server..[[data]]..sep)

HTML("<td>SQL Password<td><INPUT NAME=EditSQLPassword TYPE=Text VALUE=%s>", 
getfile(server..'dicom.ini', 'Password *= (.*)') or '')

HTML("</table>")
HTML("</FORM>");

---------------------------------------
HTML("<table>")

-- config options
HTML("<tr>")
HTML("<td><b>Install:</b>")
HTML([[<td><a href=# onclick="javascript:servicecommand('config'+
'&AE='+document.getElementById('oForm').EditAE.value+
'&PORT='+document.getElementById('oForm').EditPort.value+
'&FN='+document.getElementById('oForm').SelectFileNameSyntax.value+
'&CP='+document.getElementById('oForm').SelectCompression.value+
'&M0='+document.getElementById('oForm').EditMAGDevice0.value+
'&DB='+document.getElementById('oForm').SelectDatabase.value+
'&SH='+document.getElementById('oForm').EditSQLHost.value+
'&SE='+document.getElementById('oForm').EditSQLServer.value+
'&SU='+document.getElementById('oForm').EditSQLUsername.value+
'&SP='+document.getElementById('oForm').EditSQLPassword.value+
''
); setTimeout(returntowelcome, 2000); ">[install/config]</a>]])
HTML([[<td><a href=# onclick="javascript:servicecommand('dbtest'); setTimeout(returntowelcome, 2000); return false;">[test database]</a>]])
HTML([[<td><a href=# onclick="javascript:servicecommand('regen'); return false;">[regenerate (may take very long)]</a>]])
if dgate=='dgate' then
  HTML([[<td><a href=# onclick="javascript:servicecommand('compile'); return false;">[start compiling]</a>]])
end

-- control options
HTML("<tr>")
HTML("<td><b>Control:</b>")
HTML([[<td><a href=# onclick="javascript:servicecommand('acrnema'); return false;">[edit known dicom providers]</a>]])
HTML([[<td><a href=# onclick="javascript:servicecommand('start'); setTimeout(returntowelcome, 2000); return false;">[start server]</a>]])
HTML([[<td><a href=# onclick="javascript:servicecommand('stop'); setTimeout(returntowelcome, 2000); return false;">[stop server]</a>]])

-- use options
HTML("<FORM ID=wForm>");
HTML("<tr>")
HTML("<td><b>Web:</b><br>")

HTML([[<td><a href=# onclick="javascript:servicecommand('webconfig'+
'&AE='+document.getElementById('oForm').EditAE.value+
'&PORT='+document.getElementById('oForm').EditPort.value+
'&VIEWER='+document.getElementById('wForm').SelectStudyViewer.value+
'&SVIEWER='+document.getElementById('wForm').SelectSeriesViewer.value+
'&TEMP='+(gup('temp'))+
'&READONLY='+(document.getElementById('wForm').readOnly.checked?'1':'0')+
'&VIEWONLY='+(document.getElementById('wForm').viewOnly.checked?'1':'0')+
''
); setTimeout(returntowelcome, 2000); ">[configure web interface]</a>]])

if false then
  HTML([[<td>]])
  if write==nil then
    HTML([[<a href=# onclick="javascript:w=window.open(']]..
      string.gsub(string.gsub(script_name, 'service', 'newweb'), 'dgatesmall', 'dgate')..
      [[?mode=start', 'blank')">Enter cgi interface</a>]])
  else
    -- ladle link
    HTML([[<a href=# onclick="javascript:w=window.open('http://127.0.0.1:8086/cgi-bin/]]..script_name..[[?mode=start', 'blank')">Enter server's ladle web interface</a>]])
  end
end

HTML([[<td>]])
if write==nil then
  HTML([[<a href=# onclick="javascript:w=window.open('/app/newweb/']]..
    [[, 'blank')">Enter app interface</a>]])
else
  -- ladle link
  HTML([[<a href=# onclick="javascript:w=window.open('http://127.0.0.1:8086/cgi-bin/]]..script_name..[[?mode=start', 'blank')">Enter server's ladle web interface</a>]])
end

HTML([[<tr>]])

HTML("<td><b>Config:</b><br>")

sel=getfile(htmlweb..'app'..sep..'newweb'..sep..'dicom.ini', '^studyviewer *= (.*)') or 'wadostudyviewer'
HTML("<td>Study<SELECT NAME=SelectStudyViewer>")
if fileexists(server..'webserver'..sep..'cgi-bin'..sep..'newweb'..sep..'wadostudyviewer.lua') then
  HTML("<option value=wadostudyviewer"..selected("wadostudyviewer", sel)..">Conquest viewer</option>")
end
if fileexists(server..'webserver'..sep..'cgi-bin'..sep..'newweb'..sep..'wadoviewer_starter.lua') then
  HTML("<option value=wadoviewer_starter"..selected("wadoviewer_starter", sel)..">Conquest viewer</option>")
end
if fileexists(server..'webserver'..sep..'cgi-bin'..sep..'newweb'..sep..'weasis_starter.lua') then
  HTML("<option value=weasis_starter"..selected("weasis_starter", sel)..">Weasis viewer</option>")
end
if fileexists(server..'webserver'..sep..'cgi-bin'..sep..'newweb'..sep..'altviewer_starter.lua') then
  HTML("<option value=altviewer_starter"..selected("altviewer_starter", sel)..">Alt viewer</option>")
end
if fileexists(server..'webserver'..sep..'cgi-bin'..sep..'newweb'..sep..'cornerstone_starter.lua') then
  HTML("<option value=cornerstone_starter"..selected("cornerstone_starter", sel)..">Cornerstone viewer</option>")
end
if fileexists(server..'webserver'..sep..'cgi-bin'..sep..'newweb'..sep..'iframe_starter.lua') then
  HTML("<option value=iframe_starter"..selected("iframe_starter", sel)..">Iframe study viewer</option>")
end
if fileexists(server..'webserver'..sep..'cgi-bin'..sep..'newweb'..sep..'flexviewer_starter.lua') then
  HTML("<option value=flexviewer_starter"..selected("flexviewer_starter", sel)..">Flex viewer</option>")
end
HTML("</SELECT>")

local ssel=getfile(htmlweb..'app'..sep..'newweb'..sep..'dicom.ini', '^viewer *= (.*)') or 'wadoseriesviewer'
HTML("<td>Series<SELECT NAME=SelectSeriesViewer>")
if fileexists(server..'webserver'..sep..'cgi-bin'..sep..'newweb'..sep..'wadoseriesviewer.lua') then
  HTML("<option value=wadoseriesviewer"..selected("wadostudyviewer", ssel)..">Conquest viewer</option>")
end
if fileexists(server..'webserver'..sep..'cgi-bin'..sep..'newweb'..sep..'wadoviewer_starter.lua') then
  HTML("<option value=wadoviewer_starter"..selected("wadoviewer_starter", ssel)..">Conquest viewer</option>")
end
if fileexists(server..'webserver'..sep..'cgi-bin'..sep..'newweb'..sep..'weasis_starter.lua') then
  HTML("<option value=weasis_starter"..selected("weasis_starter", ssel)..">Weasis viewer</option>")
end
if fileexists(server..'webserver'..sep..'cgi-bin'..sep..'newweb'..sep..'altviewer_starter.lua') then
  HTML("<option value=altviewer_starter"..selected("altviewer_starter", ssel)..">Alt viewer</option>")
end
if fileexists(server..'webserver'..sep..'cgi-bin'..sep..'newweb'..sep..'dwvviewer_starter.lua') then
  HTML("<option value=dwvviewer_starter"..selected("dwvviewer_starter", ssel)..">DWV viewer</option>")
end
if fileexists(server..'webserver'..sep..'cgi-bin'..sep..'newweb'..sep..'papaya_starter.lua') then
  HTML("<option value=papaya_starter"..selected("papaya_starter", ssel)..">Papaya viewer</option>")
end
if fileexists(server..'webserver'..sep..'cgi-bin'..sep..'newweb'..sep..'cornerstone_starter.lua') then
  HTML("<option value=cornerstone_starter"..selected("cornerstone_starter", ssel)..">Cornerstone viewer</option>")
end
if fileexists(server..'webserver'..sep..'cgi-bin'..sep..'newweb'..sep..'flexviewer_starter.lua') then
  HTML("<option value=flexviewer_starter"..selected("flexviewer_starter", ssel)..">Flex viewer</option>")
end
if fileexists(server..'webserver'..sep..'cgi-bin'..sep..'newweb'..sep..'imagejviewer_starter.lua') then
  HTML("<option value=imagejviewer_starter"..selected("imagejviewer_starter", ssel)..">ImageJ viewer</option>")
end
HTML("</SELECT>")


local check=''
local flag=getfile(htmlweb..'app'..sep..'newweb'..sep..'dicom.ini', '^readOnly *= (%d*)') or '0'
if flag~='0' then check = ' checked' end
HTML([[<td><input type=checkbox name=readOnly]]..check..[[><label for="readOnly">No modify</label>]])

flag=getfile(htmlweb..'app'..sep..'newweb'..sep..'dicom.ini', '^viewOnly *= (%d*)') or '0'
if flag~='0' then check = ' checked' end
HTML([[<td><input type=checkbox name=viewOnly]]..check..[[><label for="viewOnly">No download</label>]])

HTML("</table>")
HTML("</FORM>");

HTML("</BODY>")
