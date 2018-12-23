--[[
Some utility functions used by nifty.lua and niftydicom.lua
-- 20130701  mvh  added secdiff
-- 20130702  mvh  catch missing input file in filecopy
-- 20130715  mvh  filecopy uses small buffer when copying entire file; added secadd
-- 20130823  mvh  added filesize()
-- 20160221  mvh  Fix pattern for LuaJIT compatibility
]]

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

function osexecute(cmd)
  local t=os.clock()
  local prog = string.gsub(cmd, "%.exe.*$", "")
  prog = string.gsub(prog, "^.*\\", "");
  print('running', cmd)
  if system then 
    system(cmd) 
  else
    os.execute(cmd)
  end
  print(prog, 'took', os.clock()-t, 's')
end  

function fileexists(f)
  local h = io.open(f)
  if h==nil then 
    return false
  else
    h:close()
    return true
  end
end

function filesize(f)
  local h = io.open(f)
  if h==nil then 
    return 0
  else
    local s = h:seek('end', 0)
    h:close()
    return s
  end
end
  

function filecopy(a, b, start, length)
  start  = start  or 0
  
  local f=io.open(a, 'rb') 
  if f==nil then
    print('cannot open source file for copying: ' .. a)
    return
  end
  
  local g=io.open(b, 'wb')
  if g==nil then
    print('cannot open target file for copying: ' .. b)
    return
  end
  
  f:seek('set', start)
  while true do
    local block=f:read(length or 8192)
    if not block then break end
    g:write(block)
    if length then break end
  end
    
  f:close();
  g:close();
end

function secdiff(initialTime,finalTime)
  local initialHour=tonumber(string.sub(initialTime,1,2)) *3600
  local initialMinute=tonumber(string.sub(initialTime,3,4))*60
  local initialSecond=tonumber(string.sub(initialTime,5,99))

  local finalHour=tonumber(string.sub(finalTime,1,2))*3600
  local	finalMinute=tonumber(string.sub(finalTime,3,4))*60
  local	finalSecond=tonumber(string.sub(finalTime,5,99))

  local	totalInitialTime=initialHour+initialMinute+initialSecond
  local	totalFinalTime=finalHour+finalMinute+finalSecond
  return totalFinalTime-totalInitialTime
end

function secadd(initialTime, s_to_add)
  local initialHour=tonumber(string.sub(initialTime,1,2)) *3600
  local initialMinute=tonumber(string.sub(initialTime,3,4))*60
  local initialSecond=tonumber(string.sub(initialTime,5,99))

  local	totalInitialTime=initialHour+initialMinute+initialSecond + s_to_add
  return string.format('%02d%02d%02f', 
    math.floor(totalInitialTime/3600),
    math.floor((totalInitialTime%3600)/60),
    totalInitialTime%60)
end

