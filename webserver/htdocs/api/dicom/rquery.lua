function remotequery(ae, level, q, dicomweb)
  local isdicomweb=true
   if (dicomweb == nil or dicomweb == '') then isdicomweb= false end;
  local remotecode = 
[[
  local ae=']]..ae..[[';
  local level=']]..level..[[';
  local dicomweb=]]..tostring(isdicomweb)..[[;
  local q2=DicomObject:new(']]..q..[[');
  local r = dicomquery(ae, level, q2):Serialize(true,true,dicomweb);
  local s=tempfile('txt') local f=io.open(s, "wb") f:write(r) returnfile=s f:close();
]]
 local f = servercommand('lua:'..remotecode)
 return f
end;

function remotemetadata(ae, level, st, se, sop)
  local remotecode = 
[[
  tags={"SpecificCharacterSet","SOPClassUID","SOPInstanceUID","StudyDate","ContentDate","StudyTime",
    "ContentTime","AccessionNumber","Modality","ConversionType","ReferringPhysicianName","PatientName",
    "PatientID","PatientBirthDate","PatientSex","BodyPartExamined","SecondaryCaptureDeviceManufacturer",
    "SecondaryCaptureDeviceManufacturerModelName","StudyInstanceUID","SeriesInstanceUID","StudyID",
    "SeriesNumber","InstanceNumber","PatientOrientation","SamplesPerPixel","PhotometricInterpretation",
    "Rows","Columns","BitsAllocated","BitsStored","HighBit","PixelRepresentation",
    "SmallestImagePixelValue","LargestImagePixelValue"} -- "PixelData"
  local ae=']]..ae..[[';
  local level=']]..level..[[';
  local q2=DicomObject:new();
  for k, v in ipairs(tags) do q2[v]="" end
  q2.QueryRetrieveLevel='IMAGE'
  q2.StudyInstanceUID=']]..st..[['
  q2.SeriesInstanceUID=']]..(se or '')..[['
  q2.SOPInstanceUID=']]..(sop or '')..[['
  local r = dicomquery(ae, 'IMAGE', q2)
  local d = DicomObject:new()
  d:Read(':'..r[0].SOPInstanceUID)
  for i=0, #r-1 do 
    for k, v in ipairs(tags) do
      if r[i][v]==nil then
        r[i][v]=d[v] or 'x'
      end
    end
  end
  a=DicomObject:newarray()
  for i=0, #r-1 do 
    if r[i].Modality~='RTSTRUCT' then a:Add(r[i]) end
  end
  r = a:Serialize(true,false,true)
  local s=tempfile('txt') local f=io.open(s, "wb") f:write(r) returnfile=s f:close()
]]
 local f = servercommand('lua:'..remotecode)
 return f
end

function echo(server)
local ae = server or servercommand('get_param:MyACRNema')
local remotecode =
[[
local ae=']]..ae..[[';
if (dicomecho(ae)) then return 1 else return 0 end;
]]
b=servercommand('lua:'..remotecode)
io.write(b or '')
end;

  
function patients(server,params)
local ae = server or servercommand('get_param:MyACRNema')
b=remotequery(ae, 'PATIENT', params)
io.write(b or '')
end;

function studies(server,params,dicomweb)
local ae = server or servercommand('get_param:MyACRNema')
b=remotequery(ae, 'STUDY', params, dicomweb)
io.write(b or '')
end;

function series(server,params, dicomweb)
local ae = server or servercommand('get_param:MyACRNema')
b=remotequery(ae, 'SERIES', params, dicomweb)
io.write(b or '')
end;

function images(server,params, dicomweb)
local ae = server or servercommand('get_param:MyACRNema')
b=remotequery(ae, 'IMAGE', params, dicomweb);
io.write(b or '')
end;

function metadata(server, st)
local ae = server or servercommand('get_param:MyACRNema')
b=remotemetadata(ae, 'STUDY', st);
io.write(b)
end
