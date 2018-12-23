-- worldmatch starter in zerobrane studio; mvh 20121128
-- note priority of searching worldmatch = c:, d:, e:
-- mvh 20130219
-- returns TWO interpreters - conquestdicomutil.lua will select the other one
-- 1: uses dgate --luastart: --> establishes connection to a running server
-- 2: uses dgate --dolua: --> runs as a standalone lus dicom toolbox
-- mvh 20130322: added iup.LoopStep as optional yield function to enable iup from zbs
-- mvh 20130522: changed appdir to e.g., c:\dicomserver; fixed \\
-- mvh 20130618: fixes for linux; use /dgate and set path and cpath
-- mvh 20141124: remove done() call and set checkcount to 1
-- mvh 20160213: made appdirectory overrideable (and name) for specialized interpreters
-- mvh 20160319: Small fix in there
-- mvh 20180109: Version runs from /interpreters and reads server folder
--               No kill of server (set onexit in zbs1.70 mobdebug)
-- mvh 20180110: Use Global.Basedir; idea ask for port number
-- mvh 20180111: Do not use appdir; interferes with other interpreters
-- mvh 20180111: Use ide:ExecuteCommand to start without flashing windows
-- mvh 20180113: Read port from config; give message if conquest not found
-- mvh 20180115: Fix timing issue in startup; note: need to update mobdebug in server to fix exit() issue
-- mvh 20181119: Fix for linux 

local exe
local condir = appdir or wx.wxGetCwd()..'/interpreters'
if not wx.wxFileExists(condir..'/dgate') then
  condir = appdir or wx.wxGetCwd()..'\\interpreters'
end

local function exePath(appdir)
  if appdir and wx.wxFileExists(appdir..[[\dgate.exe]]) then
    return appdir..[[\dgate.exe]]
  elseif appdir and wx.wxFileExists(appdir..[[\dgate64.exe]]) then
    return appdir..[[\dgate64.exe]]
  elseif appdir and wx.wxFileExists(appdir..[[/dgate]]) then
    return appdir..[[/dgate]]
  end
end


local conquestport = '5678'

ide.config.getconquestport = function()
  if ide.config.conquest then 
    return ide.config.conquest.port or conquestport
  else
    return conquestport
  end
end

return {
  name = "Conquest (local)",
  description = "Connect to running Conquest DICOM server",
  appdirectory = condir,
  api = {"wxwidgets", "baselib", "conquest"},
  frun = function(self,wfilename,rundebug)
    if wfilename==nil then return end
    exe = exe or exePath(self.appdirectory)
    local filepath = wfilename:GetFullPath()
    local script
    if rundebug then
      DebuggerAttachDefault({redirect = "c"})
      script = rundebug
    else
      -- if running on Windows and can't open the file, this may mean that
      -- the file path includes unicode characters that need special handling
      local fh = io.open(filepath, "r")
      if fh then fh:close() end
      if ide.osname == 'Windows' and pcall(require, "winapi")
      and wfilename:FileExists() and not fh then
        winapi.set_encoding(winapi.CP_UTF8)
        filepath = winapi.short_path(filepath)
      end

      script = ('dofile [[%s]]'):format(filepath)
    end

    ClearOutput() 
    DisplayOutputLn('Searching for Conquest server') 
    
    -- read server folder over TCP/IP connection for use in search paths    
    local serverdir=''
    if wx.wxFileExists(condir..'/dgate') then
      local f = io.popen(condir..'/dgate -p'..ide.config.getconquestport()..' "--lua:return Global.Basedir"')
      serverdir = f:read('*all')
      f:close()
    else
      local busy=true
      ide:ExecuteCommand(condir..'\\dgate.exe -p'..ide.config.getconquestport()..' "--lua:return Global.Basedir"', condir, function(a) serverdir=a busy=false end, 
      function() busy=false if serverdir=='' then
        ClearOutput() DisplayOutputLn('No running Conquest server found at port: '..ide.config.getconquestport())
        end end
      )
      if busy==true then
        wx.wxMilliSleep(500)
      end
    end

    local code = ([[xpcall(function() io.stdout:setvbuf('no');
    package.path = package.path ..\";\" .. \"%q/?.lua\";
    package.cpath = package.cpath ..\";\" .. \"%q/?.so\";
    require('mobdebug').checkcount = 1;
    require('mobdebug').yield=function() if iup then iup.LoopStep() end end;
    require('mobdebug').onexit=function() require('mobdebug').done() error() end
    %s ;
    --package.loaded.mobdebug.done() ; 
    package.loaded.mobdebug=nil ; 
    collectgarbage('collect') end, function(err) print(debug.traceback(err)) end)]]):format(serverdir..'/lua', serverdir..'/clibs', script)
    local cmd = '"'..exe..'" -p'..ide.config.getconquestport()..' --luastart:"'..code..'"'
    
    ide:ExecuteCommand(cmd,self:fworkdir(wfilename),nil,
      function() 
        ide.debugger.pid = nil 
        if serverdir~='' then 
          ClearOutput() 
          DisplayOutputLn('Conquest folder: '..serverdir) 
          DisplayOutputLn('Conquest port: '..ide.config.getconquestport()) 
       end 
      end)
    return nil
  end,
  fprojdir = function(self,wfilename)
    return wfilename:GetPath(wx.wxPATH_GET_VOLUME)
  end,
  fworkdir = function (self,wfilename)
    return ide.config.path.projectdir or wfilename:GetPath(wx.wxPATH_GET_VOLUME)
  end,
  hasdebugger = true,
  fattachdebug = function(self) DebuggerAttachDefault({redirect = "c"}) end,
  scratchextloop = false,
},
{
  name = "Conquest Dicom utility",
  description = "Conquest lua interpreter",
  appdirectory = appdir,
  api = {"wxwidgets", "baselib", "conquest"},
  frun = function(self,wfilename,rundebug)
    exe = exe or exePath(self.appdirectory)
    local filepath = wfilename:GetFullPath()
    local script
    if rundebug then
      DebuggerAttachDefault({redirect = "n"})
      script = rundebug
    else
      -- if running on Windows and can't open the file, this may mean that
      -- the file path includes unicode characters that need special handling
      local fh = io.open(filepath, "r")
      if fh then fh:close() end
      if ide.osname == 'Windows' and pcall(require, "winapi")
      and wfilename:FileExists() and not fh then
        winapi.set_encoding(winapi.CP_UTF8)
        filepath = winapi.short_path(filepath)
      end

      script = ('dofile [[%s]]'):format(filepath)
    end
    local code = ([[xpcall(function() io.stdout:setvbuf('no'); 
    require('mobdebug').yield=function() if iup then iup.LoopStep() end end;
    %s ;  
    package.loaded.mobdebug.done() ; 
    package.loaded.mobdebug=nil ; 
    collectgarbage('collect') end, function(err) print(debug.traceback(err)) end)]]):format(script)
    local cmd = '"'..exe..'" --dolua:"'..code..'"'
    return CommandLineRun(cmd,self:fworkdir(wfilename),true,false,nil,nil,
      function() ide.debugger.pid = nil end)
  end,
  fprojdir = function(self,wfilename)
    return wfilename:GetPath(wx.wxPATH_GET_VOLUME)
  end,
  fworkdir = function (self,wfilename)
    return ide.config.path.projectdir or wfilename:GetPath(wx.wxPATH_GET_VOLUME)
  end,
  hasdebugger = true,
  fattachdebug = function(self) DebuggerAttachDefault({redirect = "n"}) end,
  scratchextloop = false,
}
