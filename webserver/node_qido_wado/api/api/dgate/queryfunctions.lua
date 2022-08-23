
function getframe(ae, bd, st, se, sop, fr)
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
io.write("--"..bd.."\r\n")
io.write("Content-Type: application/dicom\r\n");
io.write("Content-Transfer-Encoding: binary\r\n\r\n")
io.write(f)
io.write("--"..bd.."--\r\n\r\n")
end



function getinstance(ae, bd, st, se, sop)
  local remotecode = 
    [[
      local ae=']]..(ae or servercommand('get_param:MyACRNema'))..[[';
      local obd=']]..bd..[['
      local q2=DicomObject:new();
      q2.QueryRetrieveLevel='IMAGE'
      q2.StudyInstanceUID=']]..st..[['
      q2.SeriesInstanceUID=']]..(se or '')..[['
      q2.SOPInstanceUID=']]..(sop or '')..[['
      local r = dicomget(ae, 'IMAGE', q2)
      local s=tempfile('.txt')
      local t=tempfile('.txt')
      f = io.open(s, "wb")
      for i=0, #r-1 do
        r[i]:Write(t)
        f:write("--"..obd.."\r\n")
        f:write("Content-Type: application/dicom\r\n")
        f:write("Content-Transfer-Encoding: binary\r\n\r\n")
        g = io.open(t, "rb")
        f:write(g:read('*a'))
        g:close()
      end
      f:write("--"..obd.."--\r\n\r\n")
      f:close()
      os.remove(t)
      returnfile=s
    ]]
     local f = servercommand('lua:'..remotecode, 'binary')
     io.write(f)
end




function readinstance(studyuid,seriesuid,SOPInstanceUID)
  local imagelocation = studyuid..'\\\\'..seriesuid..'\\\\'..SOPInstanceUID
  local remotecode =
[[
  local x=newdicomobject(); 
  local outfile = tempfile('.dcm')
  x:Read("]]..imagelocation..[["); 
  x=x:Compress('j2')
  x:Write(outfile)
  returnfile = outfile
  ]]
  b=servercommand('lua:'..remotecode,'binary')
  io.write(b or '')
end


function readframe(studyInstanceUid,seriesInstanceUid,sopInstanceUid,frame)
  local imagelocation = studyInstanceUid..'\\\\'..seriesInstanceUid..'\\\\'..sopInstanceUid
  local remotecode =
[[
  local x=newdicomobject(); 
  local outfile = tempfile('.dcm')
  local oframe=']]..frame..[['
  x:Read("]]..imagelocation..[[");
 
  x:Script('save frame '..oframe..' to '..outfile)
  returnfile = outfile
  ]]
  b=servercommand('lua:'..remotecode,'binary')
  io.write(b or '')
end

function readslice(pslice)
  local slice = pslice   --or '86557:1.3.46.670589.30.1.6.1.116520970982.1481396381703.2' -- must be valid patid and sopinstanceuid
  local remotecode =
[[
  local ofile=']]..slice..[[';
  local outfile = tempfile('.dcm')
  local x = DicomObject:new()
  x:Read(ofile)
  x=x:Compress('j2')
  x:Write(outfile)
  returnfile = outfile
]]
  b=servercommand('lua:'..remotecode,'binary')
  io.write(b or '')
end
 
function converteslice(pgraphic,psize,pslice)
  local frame=0
  local graphic = pgraphic or  'jpg'
  local size = psize or 512
  local slice = pslice  -- must be valid patid and sopinstanceuid
  if (graphic=='gif') then frame=10005 end
  local remotecode =
[[
  local ofile=']]..slice..[[';
  local osize=']]..size..[[';
  local ographic=']]..graphic..[[';
  local otemp=']]..'.'..graphic..[['
  local outfile = tempfile(otemp)
  local oframe=']]..frame..[['
  local x = DicomObject:new()
  x:Read(ofile)
  local c = (x.WindowCenter or 0)     if c=='' then c=0 end
  local w = (x.WindowWidth or  0)     if w=='' then w=0 end
  local s = (x.RescaleSlope or 1)     if s=='' then s=1 end
  local i = (x.RescaleIntercept or 0) if i=='' then i=0 end

  if string.find(c, '%\\') then
    c = string.match(c, '(.-)\\')
  end
  if string.find(w, '%\\') then
    w = string.match(w, '(.-)\\')
  end

  w = tonumber(w)/tonumber(s)
  c = tonumber(c)/tonumber(s)

  x:Script('save '..ographic..' level '..math.floor(c)..' window '..(math.ceil(w/2))..' size '..osize..' frame '..oframe..' to '..outfile)
  returnfile = outfile
]]
  b=servercommand('lua:'..remotecode,'binary')
  io.write(b or '')
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

function metadata(server, level, st, se, sop)
  local ae = server or servercommand('get_param:MyACRNema')
  b=remotemetadata(ae, level, st, se, sop);
  io.write(b or '')
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
  local s=tempfile('txt') local f=io.open(s, "wb") f:write(r) returnfile=s f:close();
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


function wadouricommand(obj, region, rows, columns, contentType, anonymize,  bridge,
  windowCenter,windowWidth,frameNumber,imageQuality)
  bridge=bridge or ''
  region = string.gsub(region, ',', '/')
  windowCenter = math.floor(windowCenter+0.5)
  windowWidth = math.floor(windowWidth+0.5)
  local remotecode=
  [[
  local d = newdicomobject()
  d:Read("]]..obj..[[");
  local region=']]..region..[[';
  local rows=']]..rows..[[';
  local columns=']]..columns..[[';
  contentType=']]..contentType..[[';
  anonymize=']]..anonymize..[[';
  bridge=']]..bridge..[[';
  windowCenter=']]..windowCenter..[[';
  windowWidth=']]..windowWidth..[[';
  frameNumber=']]..frameNumber..[[';
  imageQuality=']]..imageQuality..[[';
  
  if tonumber(rows)==0 then rows = d.Rows  end
  if tonumber(columns)==0 then columns = d.Columns end
  local size=string.format("%d/%d", tonumber(rows or 512), tonumber(columns or 512))
  local nf = tonumber(d.NumberOfFrames or '0')

  if region~='' then
    local startx, endx, starty, endy
    local sx, ex, sy, ey = string.match(region, "(.+)%/(.+)%/(.+)%/(.+)")
    startx = math.floor(tonumber(sx)*tonumber(d.Columns) + 0.5)
    endx = math.floor(tonumber(ex)*tonumber(d.Columns)+ 0.5)
    starty = math.floor(tonumber(sy)*tonumber(d.Rows) + 0.5)
    endy = math.floor(tonumber(ey)*tonumber(d.Rows) + 0.5)
    d:Script(string.format('crop %d,%d,%d,%d', startx, starty, endx, endy))
  end  

  if anonymize=='yes' then 
    d:Script("call anonymize_script.cq")
  elseif anonymize~='' then 
    d:Script(string.format("lua/anonymize_script.lua(%s)", anonymize))
  end
  local tempname = tempfile('.out')
  -- local tempname ='w.out'
  -- d:Script('rm w.out')

  if contentType=="image/gif" and nf<2 then
    d:Script(string.format("save gif size %s frame %s quality %s level %d window %d to %s", rows, frameNumber, imageQuality, windowCenter, windowWidth, tempname))
  elseif contentType=="image/bmp" and nf<2 then
    d:Script(string.format("save gif size %s frame %s quality %s level %d window %d to %s", rows, frameNumber, imageQuality, windowCenter, windowWidth, tempname))
  elseif contentType=="image/gif" and nf>2 then
    d:Script(string.format("save gif size %s frame %d quality %s level %d window %d to %s", rows, 10000+5, imageQuality, windowCenter, windowWidth, tempname))
  elseif contentType=="image/jpeg" or contentType=="image/on" or contentType=="image/jp2" then
    d:Script(string.format("save jpg size %s frame %d quality %s level %d window %d to %s", rows, frameNumber, imageQuality, windowCenter, windowWidth, tempname))
  elseif contentType=="application/dicom" then
    if presentationUID=="1.2.840.10008.1.2.4.50" then d:Script("compression j3")
    elseif presentationUID=="1.2.840.10008.1.2.4.51" then d:Script("compression j4")
    elseif presentationUID=="1.2.840.10008.1.2.4.53" then d:Script("compression j5")
    elseif presentationUID=="1.2.840.10008.1.2.4.55" then d:Script("compression j1")
    elseif presentationUID=="1.2.840.10008.1.2.4.57" then d:Script("compression j2")
    elseif presentationUID=="1.2.840.10008.1.2.4.70" then d:Script("compression j1")
    elseif presentationUID=="1.2.840.10008.1.2.4.80" then d:Script("compression js")
    elseif presentationUID=="1.2.840.10008.1.2.4.81" then d:Script("compression j7")
    elseif presentationUID=="1.2.840.10008.1.2.4.90" then d:Script("compression jk")
    elseif presentationUID=="1.2.840.10008.1.2.4.91" then d:Script("compression jl")
    else d:Script("compression un") end
    d:Write(tempname)
  elseif contentType=="video/mpeg" and nf>=2 then
  elseif contentType=="text/plain" then
    d:Dump(tempname)
  elseif contentType=="text/html" then
    d:Dump(tempname)
  end
  returnfile=tempname
  ]]
  f=servercommand('lua:'..remotecode,'binary')
  io.write(f or '')
end;



function wadouri(tbl)
  loadstring("t = "..tbl)()   -- transform string to table t
  local contentType=t.contentType or "image/jpeg"
  local rows=t.rows or "0"
  local columns=t.columns or "0"
  local region=t.region or  ""
  local windowCenter=t.windowCenter or "0"
  local windowWidth=t.windowWidth or "0"
  local frameNumber=t.frameNumber or "0"
  local anonymize=t.anonymize or ""
  local imageQuality=t.imageQuality or "100"
  local charset=t.charset or ""
  local presentationUID=t.presentationUID or ""
  local bridge=t.bridge or ""
  local obj = t.studyUID..'\\\\'..t.seriesUID..'\\\\'..t.objectUID
  wadouricommand(obj, region, rows, columns, contentType, anonymize, bridge,
    windowCenter,windowWidth,frameNumber,imageQuality)
end;




function wadocgicommand(obj, lwfq, size, region, contentType, transferSyntax, anonymize, annotation, bridge)
  local command=string.format("wadorequest:%s,%s,%s,%s,%s,%s,%s,%s,%s", obj, lwfq, size, region, contentType, transferSyntax, anonymize, annotation, bridge)
  servercommand(command,'cgibinary')
end;

function wadocgi(tbl)
  loadstring("t = "..tbl)()   -- transform string to table t
  local obj=string.format("%s\\%s\\%s", t.studyUID, t.seriesUID, t.objectUID)
  local contentType=t.contentType or "image/jpeg"
  local rows=t.rows or "0"
  local columns=t.columns or "0"
  local size=string.format("%d/%d", tonumber(rows), tonumber(columns))
  local region = string.gsub(tregion or "", ',', '/')
  local windowCenter = math.floor(t.windowCenter+0.5)
  local windowWidth = math.floor(t.windowWidth+0.5)
  local frameNumber=t.frameNumber or "0"
  local imageQuality=t.imageQuality or "100"
  local lwfq=string.format("%d/%d/%d/%d", windowCenter, windowWidth, tonumber(frameNumber), tonumber(imageQuality))
  local anonymize=t.anonymize or ""
  local charset=t.charset or ""
  local presentationUID=t.presentationUID or ""
  local bridge=t.bridge or ""
  local transferSyntax=t.transferSyntax or ""
  local annotation = string.gsub(t.annotation or '', ',', '/')
  wadocgicommand(obj, lwfq, size, region, contentType, transferSyntax, anonymize, annotation, bridge)
end;
