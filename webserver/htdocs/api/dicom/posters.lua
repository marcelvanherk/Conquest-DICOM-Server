-- script has Data object to work with
function postwithscript(path, script)
  remotecode = [[
    local dat=Command:GetVR(0x9999,0x0402,true);
    local filename=tempfile(".dcm");
    local f=io.open(filename,"wb");f:write(dat);f:close();
    local Data=DicomObject:new();
    Data:Read(filename);
    os.remove(filename);
  ]] .. script
  io.write(servercommand('lua:'..remotecode, '<'..path))
end

-- STOW a single dicom object stored in path; fix its PatientName and PatientID
function poststow(path)
  remotecode = [[
    print(Data.Modality);
    local q=DicomObject:new();
    q.StudyInstanceUID=Data.StudyInstanceUID;
    q.PatientID=""; q.PatientName="";
    r=dicomquery("CONQUESTSRV1", "STUDY", q);
    if r[0] then 
      Data.PatientID=r[0].PatientID;
      Data.PatientName=r[0].PatientName 
    end;
    addimage(Data);
    return(Data.SOPInstanceUID)
  ]]
  postwithscript(path, remotecode)
end

-- add a file stored in path (may be zip)
function addlocalfile(path)
  remotecode = [[
    local dat=Command:GetVR(0x9999,0x0402,true);
    local filename=tempfile(".dcm");
    local f=io.open(filename,"wb");f:write(dat);f:close();
    servercommand('addimagefile:'..filename)
    os.remove(filename);
  ]] .. script
  io.write(servercommand('lua:'..remotecode, '<'..path))
end

-- add and process a file stored in path (may be zip)
-- script has importconverter format e.g. start with lua: if needed
function attachfile(path, script)
  remotecode = [[
    local dat=Command:GetVR(0x9999,0x0402,true);
    local filename=tempfile(".dcm");
    local f=io.open(filename,"wb");f:write(dat);f:close();
    servercommand('attachfile:'..filename..','..script)
    os.remove(filename);
  ]] .. script
  io.write(servercommand('lua:'..remotecode, '<'..path))
end
