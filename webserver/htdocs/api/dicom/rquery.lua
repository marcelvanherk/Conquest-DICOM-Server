-- dicom api, lua parts remote controlling running dicom server
-- 20220911 mvh made compatible with Ladle web server
-- 20220919 mvh only provide rquery; used for all queries; 
-- 20221017 mvh added remotemove, remotezip, remotemodalities

function iowrite(a)
  if io then io.write(a)
  else write(a) end
end

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

function rquery(server,params,level)
  local ae = server or servercommand('get_param:MyACRNema')
  local b=remotequery(ae, level, params, true)
  iowrite(b or '')
end;

function getmetadata(server, st, se, sop)
  local ae = server or servercommand('get_param:MyACRNema')
  local remotecode = 
[[
  local ae=']]..ae..[[';
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
 iowrite(f or '')
end

function getinstances(ae, bd, st, se, sop)
  local remotecode = 
[[
  local ae=']]..(ae or servercommand('get_param:MyACRNema'))..[[';
  local bd=']]..bd..[['
  local q2=DicomObject:new();
  q2.QueryRetrieveLevel='IMAGE'
  q2.StudyInstanceUID=']]..st..[['
  q2.SeriesInstanceUID=']]..(se or '')..[['
  q2.SOPInstanceUID=']]..(sop or '')..[['
  q2["9999,0c00"]='ImageNumber' -- database field name to sort
  local r = dicomget(ae, 'IMAGE', q2)
  local s=tempfile('.txt') 
  local t=tempfile('.txt')
  f = io.open(s, "wb")
  for i=0, #r-1 do
    r[i]:Write(t)
    f:write("--"..bd.."\r\n")
    f:write("Content-Type: application/dicom\r\n")
    f:write("Content-Transfer-Encoding: binary\r\n\r\n")
    g = io.open(t, "rb")
    f:write(g:read('*a'))
    g:close()
  end
  f:write("--"..bd.."--\r\n\r\n")
  f:close()
  os.remove(t)
  returnfile=s
]]
 local f = servercommand('lua:'..remotecode, 'binary')
 iowrite(f or '')
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
 iowrite(f or '')
end

function getthumbnail(server, studyuid, serieuid, instuid, frame, size)
  local ae = server or servercommand('get_param:MyACRNema')
  local remotecode = 
[[
  local ae=']]..ae..[[';
  local frame=]]..(frame or 0)..[[;  
  local size=]]..(size or 128)..[[;
  q=DicomObject:new()
  q.SeriesInstanceUID=']]..serieuid..[['
  q.StudyInstanceUID=']]..studyuid..[['
  q.SOPInstanceUID=']]..instuid..[['
  q.QueryRetrieveLevel='IMAGE'
  q["9999,0c00"]='ImageNumber' -- database field name to sort
  r=dicomquery(ae, 'IMAGE', q)
  n=math.floor(#r/2)
  -- generate thumbnail
  outfile = tempfile('.jpg')
  x=dicomget(ae, 'IMAGE', r[n])
  x[0]:Script('save jpg size '..size..' frame '..frame..' to '..outfile)
  returnfile = outfile
]]
local b=servercommand('lua:'..remotecode,'binary')
iowrite(b or '')
end;

----- for non qido api

function remotemodalities()
  local remotecode =
[[
  JSON=require('lua/JSON')
  local t={}
  for j=0, 1000 do
    local a,i,p,c=get_amap(j)
    if string.find(a, '*') then break end
    table.insert(t, {ae=a, ip=i, port=p, compression=c})
  end
  return JSON:encode(t)
]]
  iowrite(servercommand('lua:'..remotecode))
end

function remoteecho(server)
  local ae = server or servercommand('get_param:MyACRNema')
  local remotecode = [[local ae=']]..ae..[[' if (dicomecho(ae)) then return 1 else return 0 end]]
  local b=servercommand('lua:'..remotecode)
  iowrite(b or '')
end;

function remotemove(from, to, q)
  local from = from or servercommand('get_param:MyACRNema')
  local remotecode =
[[
  local from=']]..from..[[';
  if from=='null' then from=Global.MyACRNema end
  local to=']]..to..[[';
  local q=DicomObject:new(']]..q..[[');
  return dicommove(from, to, q, 0);
]]
  iowrite(servercommand('lua:'..remotecode))
end

function remotezip(patid, studyuid, serieuid, instuid, script)
  local s=string.format('%s,%s,%s,%s,cgi,%s', patid or '', studyuid or '', serieuid or '', instuid or '', script or '')
  local b=servercommand([[export:]]..s, 'binary')
  b = b:sub(b:find('\n\n')+2)
  iowrite(b or '')
end
