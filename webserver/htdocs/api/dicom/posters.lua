-- add and process a file stored in path (may be zip)
-- script has importconverter format e.g. start with lua: if needed
function attachfile(path, script, ext)
  script = '[['..script..']]' -- allow ' and " in script
  ext = ext or ".dcm"
  remotecode = [[
    local script = ]]..script..[[;
    local dat=Command:GetVR(0x9999,0x0402,true);
    local filename=tempfile(']]..ext..[[');
    local f=io.open(filename,"wb");f:write(dat);f:close();
    servercommand('attachfile:'..filename..','..script)
    os.remove(filename);
  ]]
  io.write(servercommand('lua:'..remotecode, '<'..path))
end

-- add and process a dicom file stored in path (may be zip)
-- script is in lua format working on global data; return string taken as JSON response
function attachdicomfile(path, script)
  script = '[['..script..']]' -- allow ' and " in script
  ext = ".dcm"
  local remotecode = [[
    local script = ]]..script..[[;
    local dat=Command:GetVR(0x9999,0x0402,true);
    local filename=tempfile(']]..ext..[[');
    local f=io.open(filename,"wb");f:write(dat);f:close();
    data=DicomObject:new()
    data:Read(filename)
    os.remove(filename);
    return loadstring(script)()
  ]]
  io.write(servercommand('lua:'..remotecode, '<'..path))
end

-- STOW a single dicom object stored in path; fix its PatientName and PatientID
function poststow(path)
  local code = [[
    local q=DicomObject:new()
    q.StudyInstanceUID=data.StudyInstanceUID
    q.PatientID=""; q.PatientName=""
    local ae = servercommand('get_param:MyACRNema')
    r=dicomquery(ae, "STUDY", q)
    if r[0] then 
      data.PatientID=r[0].PatientID
      data.PatientName=r[0].PatientName 
    end;
    addimage(data)
    local s='"'..data.SOPInstanceUID..'"'
    if (#s % 2)~=0 then s=s..' ' end
    return s
  ]]
  attachdicomfile(path, code)
end
