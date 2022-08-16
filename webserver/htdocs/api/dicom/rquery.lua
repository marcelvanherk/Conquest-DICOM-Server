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

function remotemetadata(ae, level, st)
  local remotecode = 
[[
  local ae=']]..ae..[[';
  local level=']]..level..[[';
  local q2=DicomObject:new();
  q2.QueryRetrieveLevel='IMAGE'
  q2.SeriesInstanceUID=''
  q2.StudyInstanceUID=']]..st..[['
  q2.InstanceNumber=''
  q2.Rows=''
  q2.Columns=''
  q2.PatientID=''
  q2.PatientName=''
  q2.SOPInstanceUID=''
  q2.NumberOfFrames=''
  q2.BitsStored=''
  q2.SamplesPerPixel=''
  q2.SOPClassUID=''
  q2.SliceLocation=''
  q2.Modality=''
  q2.FrameOfReferenceUID=''
  q2.PatientPosition=''
  q2.SeriesTime=''
  q2.SeriesDate=''
  q2.StudyTime=''
  q2.StudyDate=''
  q2.ImageType=''
  local r = dicomquery(ae, 'IMAGE', q2)
  local d = DicomObject:new()
  d:Read(':'..r[0].SOPInstanceUID)
  for i=0, #r-1 do 
    r[i].WindowCenter=d.WindowCenter 
    r[i].WindowWidth=d.WindowWidth
    r[i].RescaleIntercept=d.RescaleIntercept
    r[i].RescaleSlope=d.RescaleSlope
    r[i].HighBit=d.HighBit
    r[i].PixelRepresentation=d.PixelRepresentation
    r[i].BitsAllocated=d.BitsAllocated
    r[i].PixelSpacing=d.PixelSpacing
    r[i].PhotometricInterpretation = d.PhotometricInterpretation
    r[i].ImageOrientationPatient = d.ImageOrientationPatient
    r[i].ImagePositionPatient = d.ImagePositionPatient
    r[i].SliceThickness = d.SliceThickness
    r[i].PatientOrientation = d.PatientOrientation
    r[i].ContrastBolusAgent = d.ContrastBolusAgent
    r[i].AccessionNumber = d.AccessionNumber
    r[i].ImageComments = d.ImageComments
    r[i]:SetVR(0x8,0x1140,d:GetVR(0x8,0x1140))
  end
  r = r:Serialize(true,false,true)
  local s=tempfile('txt') local f=io.open(s, "wb") f:write(r) returnfile=s f:close()
]]
 local f = servercommand('lua:'..remotecode)
 return f
end;

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
