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
  q2.Colums=''
  q2.PatientID=''
  q2.PatientName=''
  q2.SOPInstanceUID=''
  local r = dicomquery(ae, 'IMAGE', q2)
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
