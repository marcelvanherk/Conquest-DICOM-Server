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

local exe

local function exePath(appdir)
  if appdir and wx.wxFileExists(appdir..[[\dgate.exe]]) then
    return appdir..[[\dgate.exe]]
  elseif appdir and wx.wxFileExists(appdir..[[\dgate64.exe]]) then
    return appdir..[[\dgate64.exe]]
  elseif appdir and wx.wxFileExists(appdir..[[/dgate]]) then
    return appdir..[[/dgate]]
--  elseif wx.wxfileexists([[c:\dicomserver\dgate.exe]]) then
--    return [[c:\dicomserver\dgate.exe]]
--  elseif wx.wxfileexists([[d:\dicomserver\dgate.exe]]) then
--    return [[d:\dicomserver\dgate.exe]]
--  elseif wx.wxfileexists([[e:\dicomserver\dgate.exe]]) then
--    return [[e:\dicomserver\dgate.exe]]
--  elseif wx.wxfileexists([[c:\dicomserver\dgate64.exe]]) then
--    return [[c:\dicomserver\dgate64.exe]]
--  elseif wx.wxfileexists([[d:\dicomserver\dgate64.exe]]) then
--    return [[d:\dicomserver\dgate64.exe]]
--  elseif wx.wxfileexists([[e:\dicomserver\dgate64.exe]]) then
--    return [[e:\dicomserver\dgate64.exe]]
  end
end

return {
  name = "Conquest Dicom server",
  description = "Connect to running Conquest DICOM server",
  appdirectory = appdir,
  api = {"wxwidgets", "baselib", "conquest"},
  frun = function(self,wfilename,rundebug)
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
    local code = ([[xpcall(function() io.stdout:setvbuf('no');
    package.path = package.path ..\";\" .. \"%q/?.lua\";
    package.cpath = package.cpath ..\";\" .. \"%q/?.so\";
    require('mobdebug').checkcount = 1;
    require('mobdebug').yield=function() if iup then iup.LoopStep() end end;
    %s ;
    --package.loaded.mobdebug.done() ; 
    package.loaded.mobdebug=nil ; 
    collectgarbage('collect') end, function(err) print(debug.traceback(err)) end)]]):format(appdir..'/lua', appdir..'/clibs', script)
    local cmd = '"'..exe..'" --luastart:"'..code..'"'
    -- return
    CommandLineRun(cmd,self:fworkdir(wfilename),true,false,nil,nil,
      function() ide.debugger.pid = nil end)
    ClearOutput();
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
