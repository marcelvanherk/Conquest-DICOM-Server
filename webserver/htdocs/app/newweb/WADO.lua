-- drop in replacement for dgate built-in 'wado' mode for pure lua webservers
-- mvh 20180130
-- mvh 20180131 some more protection against missing elements
-- mvh 20180202 delete output file and gracefully fail if not created
-- mvh 20180203 Added local to f
-- mvh 20181111 Use wadorequest for all images to fix zoom error and for convenience
-- mvh 20220911 Read wadorequest using servercommand return value to avoid temp file

local studyUID=CGI("studyUID",    "")
local seriesUID =CGI("seriesUID",    "")
local objectUID=CGI("objectUID",    "")
local contentType=CGI("contentType",    "")
local rows=CGI("rows",  "0")
local columns=CGI("columns", "0")
local region=CGI("region",    "")
local windowCenter=CGI("windowCenter",    "0")
local windowWidth=CGI("windowWidth",    "0")
local frameNumber=CGI("frameNumber",    "0")
local transferSyntax=CGI("transferSyntax",    "")
local anonymize=CGI("anonymize",    "")
local annotation=CGI("annotation",    "")
local imageQuality=CGI("imageQuality",  "100")
local charset=CGI("charset",    "")
local presentationUID=CGI("presentationUID",    "")
local bridge=CGI("bridge", '')

region = string.gsub(region, ',', '/')
annotation = string.gsub(annotation, ',', '/')
windowCenter = math.floor(windowCenter+0.5)
windowWidth = math.floor(windowWidth+0.5)

local obj=string.format("%s\\%s\\%s", studyUID, seriesUID, objectUID)

function scommand(c)
  local lwfq=string.format("%d/%d/%d/%d", windowCenter, windowWidth, tonumber(frameNumber), tonumber(imageQuality))
  local size=string.format("%d/%d", tonumber(rows), tonumber(columns))
  local command=string.format("wadorequest:%s,%s,%s,%s,%s,%s,%s,%s,%s", obj, lwfq, size, region, contentType, transferSyntax, anonymize, annotation, bridge)
  
  HTML('')
  local a=servercommand(command..'','binary')--,w.out')
  --local f=io.open('w.out', 'rb')
  --local a = f:read('*all')
  --f:close()
  local i=string.find(a, '\n\n')
  a = string.sub(a, i+2)
  write(a)
end

if contentType=="text/plain" or contentType=="text/html" then
  local d = newdicomobject()
  d:Read(obj) -- ignores bridge
  HTML("Content-type: %s\nAccess-Control-Allow-Origin: *\n\n", contentType)
  d:Dump('w.out')
  local f=io.open('w.out', 'rb')
  if f then
    write(f:read('*all'))
    f:close()
  end
else
  scommand(command)
  return
end

--[[
if contentType=="image/bmp" and tonumber(frameNumber)<0 then
  scommand(command)
elseif contentType=="image/jpeg" and tonumber(frameNumber)<0 then
  scommand(command)
else
  local d = newdicomobject()
  d:Read(obj) -- ignores bridge
  
  if tonumber(rows)==0 then rows = d.Rows or 0 end
  if tonumber(columns)==0 then columns = d.Columns or 0 end
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
  
  local tempname = 'w.out'
  d:Script('rm w.out')

  if contentType=="image/gif" and nf<2 then
    HTML('')
    d:Script(string.format("save gif size %s frame %s quality %s level %d window %d to %s", rows, frameNumber, imageQuality, windowCenter, windowWidth, tempname))
  elseif contentType=="image/bmp" and nf<2 then
    HTML('')
    d:Script(string.format("save gif size %s frame %s quality %s level %d window %d to %s", rows, frameNumber, imageQuality, windowCenter, windowWidth, tempname))
  elseif contentType=="image/gif" and nf>2 then
    HTML('')
    d:Script(string.format("save gif size %s frame %d quality %s level %d window %d to %s", rows, 10000+5, imageQuality, windowCenter, windowWidth, tempname))
  elseif contentType=="image/jpeg" or contentType=="image/on" or contentType=="image/jp2" then
    HTML('')
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
    -- else if (r3[0] == 'n')  {recompress(&pDDO, r3, "", FALSE, &PDU); SimplifyDicom(pDDO);}
    HTML('')
    d:Write(tempname)
  elseif contentType=="video/mpeg" and nf>=2 then
    HTML('')
    --
  elseif contentType=="text/plain" then
    HTML("Content-type: text/plain\nAccess-Control-Allow-Origin: *\n\n")
    d:Dump(tempname)
  elseif contentType=="text/html" then
    HTML("Content-type: text/html\nAccess-Control-Allow-Origin: *\n\n")
    d:Dump(tempname)
  end
  local f=io.open(tempname, 'rb')
  if f then
    write(f:read('*all'))
    f:close()
  end
end
]]
