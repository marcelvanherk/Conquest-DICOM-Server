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
  local s=tempfile('.txt') local f=io.open(s, "wb") f:write(r) returnfile=s f:close();
]]
 local f = servercommand('lua:'..remotecode)
 return f
end;

function remotemetadata(ae, level, st, se, sop)
  local remotecode = 
[[
  local ae=']]..ae..[[';
  local level=']]..level..[[';
  local q2=DicomObject:new();
  q2.QueryRetrieveLevel='IMAGE'
  q2.StudyInstanceUID=']]..st..[['
  q2.SeriesInstanceUID=']]..(se or '')..[['
  q2.SOPInstanceUID=']]..(sop or '')..[['
  q2['9999,0202']='-Private,7FE0,30060039'
  local r = dicomget(ae, 'IMAGE', q2)
  r = r:Serialize(true,false,true)
  local s=tempfile('.txt') local f=io.open(s, "wb") f:write(r) returnfile=s f:close()
]]
 local f = servercommand('lua:'..remotecode)
 return f
end

function getinstance(ae, st, se, sop)
  local remotecode = 
[[
  local ae=']]..(ae or servercommand('get_param:MyACRNema'))..[[';
  local q2=DicomObject:new();
  q2.QueryRetrieveLevel='IMAGE'
  q2.StudyInstanceUID=']]..st..[['
  q2.SeriesInstanceUID=']]..(se or '')..[['
  q2.SOPInstanceUID=']]..(sop or '')..[['
  local r = dicomget(ae, 'IMAGE', q2)
  local s=tempfile('.txt') 
  r[0]:Write(s)
  returnfile=s
]]
 local f = servercommand('lua:'..remotecode, 'binary')
 io.write(f)
end

function getframe(ae, st, se, sop, fr)
  local remotecode = 
[[
  local ae=']]..(ae or servercommand('get_param:MyACRNema'))..[[';
  local q2=DicomObject:new();
  q2.QueryRetrieveLevel='IMAGE'
  q2.StudyInstanceUID=']]..st..[['
  q2.SeriesInstanceUID=']]..(se or '')..[['
  q2.SOPInstanceUID=']]..(sop or '')..[['
  local fr=]]..(fr or 1)..[[
  local r = dicomget(ae, 'IMAGE', q2)
  local s=tempfile('.txt') 
  f = io.open(s, 'wb')
  f:write(r[0]:GetImage(tonumber(fr)-1));
  f:close()
  returnfile=s
]]
 local f = servercommand('lua:'..remotecode, 'binary')
 io.write(f)
end

function remotethumbs(ae, level, studyuid, serieuid, instuid, frame)
  local remotecode = 
[[
  local oframe=']]..frame..[[';  
  local ae=']]..ae..[[';
  local level=']]..level..[[';
  q=DicomObject:new()
  q.SeriesInstanceUID=']]..serieuid..[['
  q.StudyInstanceUID=']]..studyuid..[['
  q.SOPInstanceUID=']]..instuid..[['
  q.QueryRetrieveLevel=level
  q["9999,0c00"]='ImageNumber' -- database field name to sort
  r=dicomquery(ae, level, q)
  n=math.floor(#r/2)
  -- generate thumbnail
  x=DicomObject:new()
  outfile = tempfile('.jpg')
  x:Read(':'..r[n].SOPInstanceUID)
  if (oframe) then
    x:Script('save jpg frame '..oframe..' to '..outfile)
  else 
    x:script('save jpg to '..outfile)
  end    
  returnfile = outfile
]]
b=servercommand('lua:'..remotecode,'binary')
io.write(b or '')
end;

function thumbs(server,studyuid, serieuid, instuid, frame)
  local ae = server or servercommand('get_param:MyACRNema')
  remotethumbs(ae,'IMAGE',studyuid or '', serieuid or '', instuid or '', frame or '0');
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

function metadata(server, st, se, sop)
local ae = server or servercommand('get_param:MyACRNema')
local lev='STUDY'
if se then lev = 'SERIES' end
if sop then lev = 'IMAGE' end
b=remotemetadata(ae, lev, st, se, sop);
io.write(b)
end
