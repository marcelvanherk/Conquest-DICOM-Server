-- put functions here you want to use in ImportConverters, QueryConverters etc or generic lua scripts running in
-- this instance of Conquest dicom server; example of loading in dicom.ini: [lua]\nassociation=require('association')
-- In examples dicom.ini lines preceded by [lua]\n most be put in the [lua] section at the end of the file

-- mvh 20200125 created to support multithreaded move initiated by script

-- splits a move command into multiple connections (typically ~2 if reciever supports to avoid too much overhead)
-- simple use: RetrieveConverter0=lua:mtmove(2) or [lua]\nRetrieveConverter0=mtmove(2)
-- advanced use: RetrieveConverter0=lua:mtmove(2, function() return ({NEARSERVER=2, FARSERVER=5})[Command.MoveDestination] end)
function mtmove(nthreads, filter)
  if filter then nthreads = filter() or 0 end      -- custom filter to enable/configure
  if nthreads<2 then return end;                   -- disabled by configuration
  if Data['9999,0b00'] then return end;            -- avoid recursion
  Data['9999,0b00']='0/'..nthreads                 -- send first part on current move
  local s={};                                      -- to collect code of lua script for thread
  s[1]='local dst="'..Command.MoveDestination..'"' -- code to transmit move destination
  s[2]='local ddo='..Data:Serialize()              -- code to transmit move query
  s[3]='local ddo2=DicomObject:new()'              -- code to convert to DicomObject
  s[4]='for k,v in pairs(ddo) do ddo2[k]=ddo[k] end'
  -- s[5] is placeholder for variable code
  s[6]='print("start move "..ddo2["9999,0b00"].." to "..dst)' -- code to print for log
  s[7]='dicommove(Global.MyACRNema, dst, ddo2)'               -- code to initiate the move

  for i=1, nthreads-1 do                           -- start nthreads-1 new threads
    s[5]=string.format('ddo2["9999,0b00"]="%d/%d"',i,nthreads)  -- code to send each part
    servercommand('luastart:'..table.concat(s, ' '))   -- run all lines of code on a new thread
  end
end

--possible extra parameters of dicommove or to be set in RetrieveConverter0
--Command['9999,0500']=50
--Command['9999,0b00']='0/10'
--Command['9999,0a00']='TEST'
--Command['9999,0700']='N2'
--Command['9999,0900']='set PatientID to "aap"'
--Command['9999,0200']=256
--destroy()

-- read and scan acrnema.map, mainly useful to get extra parameters not interpreted by dicom server 
-- but made available to configure lua scripts such as mthread above.
function read_amap()
  local map = {}
  local incomment=false
  for t in io.lines(Global.BaseDir..'acrnema.map') do
    if string.match(t, '/%*') then
      incomment=true
    elseif string.match(t, '%*/') then
      incomment=false
    elseif incomment==false then
      local a,b,c,d,e,f,g,h = string.match(t, string.rep('([0-9a-zA-Z_%.%*]*)%s*', 8))
      if a~='' then
        a = {a,b,c,d,e,f,g,h}
        table.insert(map, a) 
      end
    end
  end
  return map
end

-- creates this global in each association
acrnema_map=read_amap()

-- find a server in acrnema_map and read parameters and extra paremeters
-- E.g. the following code adds number of move threads as 5th column to acrnema.map, default 1:
-- RetrieveConverter0=lua:mtmove(2, function() find_amap(Command.MoveDestination)[5] end)
function find_amap(name)
  for k,v in ipairs(acrnema_map) do 
    if string.find(name, string.gsub(v[1], '%*', '.*')) then
      return v
    end
  end
  return {}
end

-- removePrivateInfo()
-- example use: QueryResultConverter0 = lua:removePrivateInfo()
-- advanced use: QueryResultConverter0 = lua:removePrivateInfo(function() find_amap(Association.Calling)[6]))
-- This does not work yet, script not loaded there ...... RetrieveResultConverter0 = removePrivateInfo()

function removePrivateInfo(filter)
  if filter then if (filter() or 1)==0 then return end end  -- custom filter to enable/configure 1 is allow, 0 not
  if Data.PatientName then Data.PatientName = '' end
  if Data.PatientAddress then Data.PatientAddress = '' end
end

-- Some utility functions
-- return true if file exists
function fileexists(f)
  local h = io.open(f)
  if h==nil then 
    return false
  else
    h:close()
    return true
  end
end

-- return size of file in bytes
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
  
-- copy entire or part of file
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

print('association.lua extensions loaded')
