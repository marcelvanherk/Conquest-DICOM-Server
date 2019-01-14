--[[ ---------------------------------------------------------------
This is a small installer script to enable editing and debugging [Lua](http://www.lua.org)
scripts for [Conquest Dicom Server](http://ingenium.home.xs4all.nl/dicom.html) (`V 1.4.19c up`) in the fantastic
[ZeroBrane Studio](http://studio.zerobrane.com) (`V 1.70 up`) development environment. Use as follows:

Open **ZeroBrane Studio**. On linux, run **ZeroBrane Studio** with sudo to succesfully install as follows: **sudo zbstudio**.
Load this **install.lua** file with `File - Open`

Then click this link --> [install now](macro:shell(dofile(ide:GetDocument(ide:GetEditor()):GetFilePath()))) <--

After 5 seconds, ZeroBrane Studio will reopen ready to run demo scripts
and develop for **Conquest Dicom Server**. As installed, Zerobrane Studio
communicates with a running Conquest Dicom Server and offers code
completion and full debugging facilities.
---------------------------------------------------------------- ]]--

--[[ **Updates:**
20130322: mvh Only modify interpreter if copied anew
20130322: mvh Known issue: install new system affects other system
20130522: mvh Project folder now e.g. c:\dicomserver, fixed writing appdir
20130618: mvh Modifications for linux
20130619: mvh Auto detect directory separator from ide.Editorname
20130904: mvh Do not copy socket.lua anymore; fixed %. escape; added wordwrap
20140610: mvh Use install link, force copy of interpreters such that they are configured
              Installer no longer depends on name of install script; displays wait message
20140618: mvh Protect SetTabText for older ZBS versions
20151115: mvh Install socket/core.so for linux system (assumed 64 bits)
20180113: mvh Use conquest_auto; no configuration needed (port 5678, modify in user.lua), startzconquest plugin
20181119: mvh Copy dgate binary and dgate.dic; install dgate.exe and dgate. remove socket
20190114: mvh Fix install of dgate.exe on windows
]]

  -- configuration
local config =
{ appname='Conquest Dicom Server',
  api={'conquest.lua'},
  interpreter={'conquest_auto.lua', 'dicom.ini', 'dgate.dic', 'dgate.exe', 'dgate'},
  interpretername='conquest_auto',
  packages={'wordwrap.lua', 'startzconquest.lua'},
  demos={'welcome.lua', 'livecoding.lua'},
  installdir='ZeroBraneStudio'
}

if ide:GetDocument(ide:GetEditor()).SetTabText then
  ide:GetDocument(ide:GetEditor()):SetTabText('PROCESSING .. WAIT A FEW SECONDS PLEASE');
end

local z = config.installdir
local ds = '\\'
if string.sub(ide.editorFilename, 1, 1)=='/' then ds = '/' end

-- Clear the local console tab for our messages
ide.frame.bottomnotebook.shellbox:ClearAll()

-- Detect ZeroBrane Studio Folder
local zbs = ide.editorFilename
if string.find(zbs, '.exe') then
  zbs = string.gsub(zbs, 'zbstudio%.exe$', '')
else
  zbs = string.gsub(zbs, 'zbstudio$', '')
end
print('Installing into ZeroBrane Studio at: ' .. zbs)

-- Detect app folder (assumes install file is open and on top in ide)
local app = ''; local installfile = ds..z..ds..'.-lua'
app = ide:GetDocument(ide:GetEditor()):GetFilePath()
app = string.gsub(app, installfile, '')
print(config.appname .. ' is at: ' .. app)

-- copy binaries needed for running dgate as gateway
if ds=='\\' then
  wx.wxCopyFile(app..ds..'install32'..ds..'dgate.exe', app..ds..'ZeroBraneStudio'..ds..'dgate.exe')
else
  wx.wxCopyFile(app..ds..'dgate',     app..ds..'ZeroBraneStudio'..ds..'dgate')
end
wx.wxCopyFile(app..ds..'dgate.dic', app..ds..'ZeroBraneStudio'..ds..'dgate.dic')

-- Copy file from to but do not replace newer one unless `unsafe` is passed
function safecopy(file, from, to, unsafe)
  local ffr = GetFileModTime(from..file)
  local fto = GetFileModTime(to..file)
  if ffr==nil then
    print('Cannot install file: ' .. from..file)
    return false
  end
  if fto==nil then
    wx.wxCopyFile(from .. file, to .. file)
    print('Copied file: ' .. to..file)
    return true
  elseif GetFileModTime(from..file):GetTicks()>=GetFileModTime(to..file):GetTicks() or unsafe then
    wx.wxCopyFile(from .. file, to .. file)
    print('Updated file: ' .. to..file)
    return true
  else
    print("Didn't replace newer file: " .. to..file)
    return false
  end
end

-- install files from zbs to app
safecopy('mobdebug.lua', zbs..'lualibs'..ds..'mobdebug'..ds, app..ds..'lua'..ds)

-- install files from app to zbs
for k,v in ipairs(config.api) do
  safecopy(v, app..ds..z..ds, zbs..'api'..ds..'lua'..ds)
end
for k,v in ipairs(config.packages) do
  safecopy(v, app..ds..z..ds,     zbs..'packages'..ds)
end
for k,v in ipairs(config.interpreter) do
  safecopy(v, app..ds..z..ds,  zbs..'interpreters'..ds, true)
end

-- start setting the ZBS configuration
SettingsSaveAll();
ide.settings:SetPath('/editor')
ide.settings:Write('interpreter', config.interpretername)

-- add project folder
ide.settings:SetPath('/projectsession')
local f, org = ide.settings:Read(tostring(1))
  for i=1, 100 do
    local f, p = ide.settings:Read(tostring(i))
    if f==false then
      ide.settings:Write(tostring(i), org)
      ide.settings:Write(tostring(1), app)
    break
  end
  if p==app then break end
end

-- prepare to open a list of documents
ide.settings:DeleteGroup('/session')
ide.settings:SetPath('/session')
for k, v in ipairs(config.demos) do
  ide.settings:Write(tostring(k*2-1), app..ds..'lua'..ds..v)
  ide.settings:Write(tostring(k*2), app..ds..'luasamples'..ds..v)
end
ide.settings:Write('index', 0)
ide.settings:Flush()

-- restart ZBS
print('Updated ZBS session; restarting ZBS in 5s'); wx.wxYield(); wx.wxSleep(5)
local pntsh=''
if ds=='/' then pntsh = '.sh' end
wx.wxExecute(ide.editorFilename .. pntsh .. ' -cfg singleinstance=false', wx.wxEXEC_ASYNC);
wx.wxSleep(1)
os.exit()
