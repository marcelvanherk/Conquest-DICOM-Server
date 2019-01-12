-- Utility functions for test suite
-- 1.5.0alpha
-- mvh 20190111 Avoid writing many items in acrnema.map that appear as leaks
-- mvh 20190112 Allow database to be configured in test server

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

-- return DicomArray with a new series with n images
function MakeSeries(n, Pat, Study)
  n = n or 10
  Study = Study or genuid()
  Pat = Pat or genuid()
  local r = DicomObject:newarray()
  local a = DicomObject:new()
  a:Read('0009703828:1.3.46.670589.5.2.10.2156913941.892665339.718742')
  a.StudyInstanceUID = Study
  a.PatientID = Pat
  local ser = genuid()
  for i=1, n do
    local b = a:Copy()
    b.SeriesInstanceUID = ser
    b.SOPInstanceUID = genuid()
    r:Add(b)
  end
  return r
end

-- return DicomArray with a new study of n series with n images
function MakeStudy(n, Pat, Date)
  n = n or 10
  Pat = Pat or genuid()
  local r = DicomObject:newarray()
  local a = DicomObject:new()
  local Study = genuid()
  a:Read('0009703828:1.3.46.670589.5.2.10.2156913941.892665339.718742')
  a.StudyInstanceUID = Study
  a.PatientID = Pat
  if Date then a.StudyDate = Date end
  for j=1, n do
    local ser = genuid()
    for i=1, n do
      local b = a:Copy()
      b.SeriesInstanceUID = ser
      b.SOPInstanceUID = genuid()
      r:Add(b)
    end
  end
  return r
end

-- store DicomArray into local server
function AddArray(array)
  for i=0, #array-1 do
    array[i]:AddImage()
  end
end

-- assert number of images for patient on server is n
function CountImagesInPatient(p, n, server)
  server = server or 'CONQUESTSRV1'
  local q=DicomObject:new()
  q.PatientID=p
  q.QueryRetrieveLevel='IMAGE'
  q.SOPInstanceUID=''
  local m = #dicomquery(server, 'IMAGE', q)
  assert(m==n, 'expecting '..n..' but found '..m..' images')
end

-- delete all images for patient
function DeletePatient(p)
  local q=DicomObject:new()
  q.PatientID=p
  dicomdelete(q)
end

-- stop temporary test server
function QuitTestServer()
  system('dgate64.exe -wtestserver --quit:')
  sleep(100)
  os.execute('del /q/s testserver\\*.*')
  os.execute('rmdir /q/s testserver')
end

-- make and run temporary test server (AE testserver), configure compression level
function RunTestServer(serversidecompression, database)
  serversidecompression = serversidecompression or 'UN'
  database = database or 'SqLite'
  QuitTestServer()
  
  local db=''
  if database=='SqLite' then
    db = 'SqLite=1\nSQLServer=testserver\\testserver.db3'
  elseif database=='DBase' then
    db = 'SQLServer=testserver\\'
  elseif database=='MySQL' then
    db = 'MySQL=1\nSQLServer=test\nUsername=root\nPassword=\nDoubleBackSlashToDB=1'
  elseif database=='PostGres' then
    db = 'PostGres=1\nSQLServer=test\nUsername=postgres\nPassword=postgres\nDoubleBackSlashToDB=1\nUseEscapeStringConstants=1'
  elseif database=='ODBC' then
    db = 'SQLServer=test'
  end
  
  -- add testserver to ACRNEMA.MAP in memory (last item, be aware of conflicting wildcard entries)
  for i=1, 99 do
    if get_amap(i)=='testserver' then break end
    if i==99 then servercommand('put_amap:99,testserver,127.0.0.1,4433') end
  end
  
  -- create minimal server (4 files and a data folder)
  script('mkdir testserver')
  script('mkdir testserver\\data')
  filecopy('acrnema.map',  'testserver\\acrnema.map')
  filecopy('dgate.dic',    'testserver\\dgate.dic')
  filecopy('dicom.sql',    'testserver\\dicom.sql')
  local syntax = '4'
  if string.sub(serversidecompression, 1, 1)=='n' then syntax=3 end
  
  local f = io.open('testserver\\dicom.ini', 'wt')
  f:write([[
[sscscp]
MicroPACS                = sscscp
TCPPort                  = 4433
]]..db..[[

MAGDevice0               = testserver\Data\
FileNameSyntax           = ]]..syntax..[[

incomingcompression      = ]]..serversidecompression..[[

]])
  f:close()
    
  -- create serverside test program to uncompress all images
  f = io.open('testserver\\test.lua', 'wt')
  f:write([[
d = DicomObject:new()
for line in io.lines('testserver\\files.txt') do
  d:Read(line)
  d:Script('compression un')
  print(line)
end
  ]])
  f:close()
    
  -- initialize the test server's database and start it
  system('dgate64.exe -wtestserver -v -r')
  servercommand("luastart:os.execute('dgate64.exe -wtestserver -v')")
  sleep(100)
end
