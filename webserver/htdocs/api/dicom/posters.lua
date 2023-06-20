-- mvh 20230620 fix for linux (/ vs \)

function iowrite(a)
  if io then io.write(a)
  else write(a) end
end

-- add and process a file stored in path (may be zip)
-- script has importconverter format e.g. start with lua: if needed
function attachfile(path, script, ext)
  script = '[['..script..']]' -- allow ' and " in script
  ext = ext or ".dcm"
  remotecode = [[
    local script = ]]..script..[[;
    local dat=Command:GetVR(0x9999,0x0402,true);
    local s=Command["9999,0404"] or '0'
    if s=='1' then dat=string.sub(dat, 1, -2) end
    local filename=tempfile(']]..ext..[[');
    local f=io.open(filename,"wb");f:write(dat);f:close();
    servercommand('attachfile:'..filename..','..script)
    os.remove(filename);
  ]]
  iowrite(servercommand('lua:'..remotecode, '<'..path))
end

-- add and process a dicom file stored in path
-- script is in lua format working on global Data; return string taken as JSON response
function attachdicomfile(path, script)
  script = '[['..script..']]' -- allow ' and " in script
  ext = ".dcm"
  local remotecode = [[
    local script = ]]..script..[[;
    local dat=Command:GetVR(0x9999,0x0402,true);
    local s=Command["9999,0404"] or '0'
    if s=='1' then dat=string.sub(dat, 1, -2) end
    local filename=tempfile(']]..ext..[[');
    local f=io.open(filename,"wb");f:write(dat);f:close();
    readdicom(filename)
    os.remove(filename);
    return loadstring(script)()
  ]]
  iowrite(servercommand('lua:'..remotecode, '<'..path))
end

-- STOW a single dicom object stored in path; fix its PatientName and PatientID
function poststow(path)
  local code = [[
    local q=DicomObject:new()
    q.StudyInstanceUID=Data.StudyInstanceUID
    q.PatientID=""; q.PatientName=""
    local ae = servercommand('get_param:MyACRNema')
    r=dicomquery(ae, "STUDY", q)
    if r[0] then 
      Data.PatientID=r[0].PatientID
      Data.PatientName=r[0].PatientName 
    end;
    addimage(Data)
    local s='"'..Data.SOPInstanceUID..'"'
    if (#s % 2)~=0 then s=s..' ' end
    return s
  ]]
  attachdicomfile(path, code)
end

-- run a script in the server; can return information
function runscript(script)
  local remotecode = [[
    local dat=Command:GetVR(0x9999,0x0402,true);
    local s=Command["9999,0404"] or '0'
    if s=='1' then dat=string.sub(dat, 1, -2) end
    local filename=tempfile('.lua');
    local f=io.open(filename,"wb");f:write(dat);f:close();
    local a=dofile(filename);
    os.remove(filename);
    return a
  ]]
  iowrite(servercommand('lua:'..remotecode, '<'..script));
end

-- start a script as background task in the server; return uid of job
-- use progress(percentage) from script
function startscript(script)
  local remotecode1 = [[
    return tempfile('.lua');
  ]]
  local remotecode2 = [[
    local dat=Command:GetVR(0x9999,0x0402,true);
    local s=Command["9999,0404"] or '0'
    if s=='1' then dat=string.sub(dat, 1, -2) end
    local f=io.open(filename,"wb");f:write(dat);f:close();
    function progress(n) local f=io.open(filename, "w") f:write(n) f:close() end
    local a=dofile(filename);
    os.remove(filename);
    return a
  ]]
  local fn = servercommand('lua:'..remotecode1);
  local uid = '"' .. string.match(string.gsub(fn, '\\', '/'), '.+/(.-)%.lua') .. '"'
  iowrite(uid)
  servercommand('luastart:local filename=[['..fn..']];'..remotecode2, '<'..script);
end

-- return progress indicator of job (0 started, to 100 end)
function readprogress(uid)
  local remotecode = 
    "local uid=[["..uid.."]];" .. [[
    local fn = tempfile('.lua')
    local u1 = string.match(string.gsub(fn, '\\', '/'), '.+/(.-)%.lua')
    local filename = string.gsub(fn, u1, uid)
    local f = io.open(filename, 'r')
    if f==nil then
      return 100
    end
    local s = f:read('*a')
    f:close()
    return tonumber(s) or 0
  ]]
  local s = servercommand('lua:'..remotecode);
  iowrite(s)
end

-- write progress value of job (0 started, to 100 end)
function writeprogress(uid, val)
  local remotecode = 
    "local uid=[["..uid.."]];" ..
    "local val="..val..";" .. [[
    local fn = tempfile('.lua')
    local u1 = string.match(string.gsub(fn, '\\', '/'), '.+/(.-)%.lua')
    local filename = string.gsub(fn, u1, uid)
    local f = io.open(filename, 'w')
    f:write(val)
    f:close()
    return val
  ]]
  local s = servercommand('lua:'..remotecode);
  iowrite(s)
end
