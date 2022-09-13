-- Ladle version of DICOMWeb api and redirector for all web apps
-- mvh 20220912 First version
-- mvh 20220913 Fixed wadors frame access

---------------------------------------------
--preflight
routes:options('/rs/studies/:suid/series/:euid/instances/:ouid', function (params)
   write('Access-Control-Allow-Headers: *')
   write('Access-Control-Allow-Origin: *')
   write('Content-Type: multipart/related\r\n\r\n')
end )

routes:options('/.*', function (params)
   write('Access-Control-Allow-Origin: *\r\n\r\n')
end )

routes:options('/', function (params)
   write('Access-Control-Allow-Origin: *')
   write('Content-Type: application/json\r\n\r\n')
   local res = 
[[
    <resources base="http:/api/dicom">  
      <resource path="">  
      </resource>  
      <resource path="wadouri">  
      </resource>  
      <resource path="rs">  
      </resource>  
    </resources>
]]
     write(res)
  end )

---------------------------------------------
-- Static route: / (homepage)
routes:get('/', function (params)
   write( 'Conquest webserver running on Ladle; partially supports wado-rs, wado-uri and qido-rs as /api/dicom')
end )
routes:get('/api/dicom/', function (params)
   write( 'Conquest DICOMWeb api; partially supports wado-rs, wado-uri and qido-rs')
end )

---------------------------------------------
-- Static route: /app/newweb/ (dgate.exe cgi emulated app)
routes:get('/app/newweb/', function (params)
   if request.query_string=='' then request.query_string='mode=start' request.query.mode='start' end
   redirect( '/app/newweb/dgate.exe?' .. request.query_string)
end )
routes:get('/app/newweb/dgate.exe', function (params)
   redirect( '/app/newweb/dgate.exe?' .. request.query_string)
end )
routes:get('/app/dgate.exe', function (params) -- when opening without trailing /
   redirect( '/app/newweb/dgate.exe?' .. request.query_string)
end )

-- Static route: overrule conquest jpeg
routes:get('/app/newweb/conquest.jpg', function (params)
   redirect('/app/newweb/CIMG3558.jpg')
end )
routes:get('/app/conquest.jpg', function (params) -- when opening without trailing /
   redirect('/app/newweb/CIMG3558.jpg') 
end )

---------------------------------------------
-- Static route: ohif viewer
routes:get('/app/ohif/', function (params)
   redirect( '/app/ohif/index.html')
end )
routes:get('/app/ohif/ohif1.03_min.js', function (params)
   redirect( '/app/ohif/ohif1.03_min.js')
end )
routes:get('/app/ohif/viewer/ohif1.03_min.js', function (params)
   redirect( '/app/ohif/ohif1.03_min.js')
end )
routes:get('/app/ohif/viewer/:suid', function (params)
   redirect( '/app/ohif/index.html')
end )

---------------------------------------------
-- Static route: luis viewer
routes:get('/app/luiz/', function (params)
   redirect( '/app/luiz/index.html')
end )
routes:get('/app/luiz/*rest', function (params)
   redirect( '/app/luiz/'..params.rest)
end )

---------------------------------------------------------------
--DICOMWeb
---------------------------------------------------------------

-- wado though uri: wadouri
routes:get('/api/dicom/wadouri/', function (params)
   redirect('/app/newweb/dgate.exe')
end )

-- query all studies
routes:get('/api/dicom/rs/studies', function (params)
   querystudies(request.query)
end )

-- query series of a study
routes:get('/api/dicom/rs/studies/:suid/series', function (params)
   queryseries(request.query, params.suid)
end )

-- query all series
routes:get('/api/dicom/rs/series', function (params)
   queryseries(request.query, '')
end )

-- query instances of series
routes:get('/api/dicom/rs/studies/:suid/series/:euid/instances', function (params)
   queryinstances(request.query, params.suid, params.euid)
end )

-- query all instances
routes:get('/api/dicom/rs/instances', function ()
   queryinstances(request.query, '', '')
end )

-- metadata of a study
routes:get('/api/dicom/rs/studies/:suid/metadata', function (params)
   metadata(request.query,params.suid,'','')
end )

-- metadata of a series
routes:get('/api/dicom/rs/studies/:suid/series/:euid/metadata', function (params)
   metadata(request.query, params.suid,params.euid,'')
end )

-- metadata of an instance
routes:get('/api/dicom/rs/studies/:suid/series/:euid/instances/:ouid/metadata', function (params)
   metadata(request.query, params.suid,params.euid,params.ouid)
end )

-- single frame (binary pixel data)
routes:get('/api/dicom/rs/studies/:suid/series/:euid/instances/:ouid/frames/:frame', function (params)
   frame(request.query, params.suid,params.euid,params.ouid,params.frame)
end )

-- dicom instance
routes:get('/api/dicom/rs/studies/:suid/series/:euid/instances/:ouid', function (params)
   instances(request.query, params.suid,params.euid,params.ouid)
end )

-- dicom instances of series
routes:get('/api/dicom/rs/studies/:suid/series/:euid', function (params)
   instances(request.query, params.suid,params.euid,'')
end )

-- dicom instances of study
routes:get('/api/dicom/rs/studies/:suid', function (params)
   instances(request.query, params.suid,'','')
end )

-- thumbnail of a frame
routes:get('/api/dicom/rs/studies/:suid/series/:euid/instances/:ouid/thumbnail/frames/:frame', function (params)
   thumbnail(request.query, params.suid,params.euid,params.ouid,params.frame,128)
end )

-- thumbnail of an image
routes:get('/api/dicom/rs/studies/:suid/series/:euid/instances/:ouid/thumbnail', function (params)
   thumbnail(request.query, params.suid,params.euid,params.ouid,0,128)
end )

-- thumbnail for series (middle one)
routes:get('/api/dicom/rs/studies/:suid/series/:euid/thumbnail', function (params)
   thumbnail(request.query, params.suid,params.euid,'',0,128)
end )

-- thumbnail for study (middle one)
routes:get('/api/dicom/rs/studies/:suid/thumbnail', function (params)
   thumbnail(request.query, params.suid,'','',0,128)
end );

------------------------------------------------------------
-- dicom api, Ladle version, orginal source code in qido.php 
------------------------------------------------------------

function processData(params, tags, query, level)
  include('/api/dicom/rquery.lua')
  for k, v in ipairs(tags) do
    params[v]=''
  end
  params["99990C01"]=params.limit
  params["99990C02"]=params.offset
  params.limit=nil
  params.offset=nil
  for k, v in pairs(query) do
    params[k]=v
  end
  local t = JSON:encode(params)
  write('HTTP/1.1 200/OK\r\nServer: Ladle\r\n')
  write('Access-Control-Allow-Origin: *\r\n')
  write('Content-Type: application/json\r\n\r\n')
  rquery(nil, t, level)
end    

function querystudies(query)
   local defaultTags = {
     "00080005", "00080020", "00080030",
     "00080050", "00080054", "00080056",
     "00080061", "00080090", "00081190",
     "00080201", "00100010", "00100020",
     "00100030", "00100040", "0020000D",
     "00200010", "00201206", "00201208"}
   processData(query, defaultTags, {["99990C00"]="PatientName"}, "STUDY")
end

function queryseries(query, suid)
   local defaultTags = {
    "00080005", "00080054", "00080056",
    "00080060", "0008103E", "00081190",
    "0020000E", "00200011", "00201209",
    "00080201", "00400244", "00400245",
    "00400275", "00400009", "00401001"}
   processData(query, defaultTags, {["99990C00"]="SeriesTime", StudyInstanceUID=suid}, "SERIES")
end

function queryinstances(query, suid, euid)
   local defaultTags = {
    "00080016", "00080018", "00080056",
    "00080201", "00081190", "00200013",
    "00280010", "00280011", "00280100",
    "00280008"}
   processData(query, defaultTags, {["99990C00"]="ImageNumber", StudyInstanceUID=suid, SeriesInstanceUID=euid}, "IMAGE")
end

function metadata(query,st,se,sop)
  include('/api/dicom/rquery.lua')
  write('HTTP/1.1 200/OK\r\nServer: Ladle\r\n')
  write('Access-Control-Allow-Origin: *\r\n')
  write('Content-Type: application/json\r\n\r\n')
  getmetadata(nil,st,se,sop)
end

function generateRandomString(length)
  local res=''
  for i=1, length do
    local x=math.random(36)
    res = res .. string.sub('abcdefghijklmnopqrstuvwxyz0123456789',x,x)
  end
  return res
end

function instances(query,st,se,sop)
  include('/api/dicom/rquery.lua')
  local bd = generateRandomString(32)
  write('HTTP/1.1 200/OK\r\nServer: Ladle\r\n')
  write('Access-Control-Allow-Headers: *\r\n')
  write('Access-Control-Allow-Origin: *\r\n')
  write('Content-Type: multipart/related; boundary='..bd..'\r\n\r\n')
  getinstances(nil,bd,st,se,sop)
end

function frame(query,st,se,sop,fr)
  include('/api/dicom/rquery.lua')
  local bd = 'Hallo'..generateRandomString(32)
  write('HTTP/1.1 200/OK\r\nServer: Ladle\r\n')
  write('Access-Control-Allow-Headers: *\r\n')
  write('Access-Control-Allow-Origin: *\r\n')
  write('Content-Type: multipart/related; boundary='..bd..'\r\n\r\n')
  
  write("--"..bd.."\r\n")
  write("Content-Type: application/binary\r\n")
  write('Access-Control-Allow-Origin: *')
  write("Content-Transfer-Encoding: binary\r\n\r\n")
  getframe(nil,st,se,sop,fr)
  write("--"..bd.."--\r\n\r\n")
end

function thumbnail(query,st,se,sop,fr,sz) 
  include('/api/dicom/rquery.lua')
  write('HTTP/1.1 200/OK\r\nServer: Ladle\r\n')
  write('Access-Control-Allow-Headers: *\r\n')
  write('Access-Control-Allow-Origin: *\r\n')
  write('Content-Type: image/jpeg\r\n\r\n')
  getthumbnail(nil,st,se,sop,fr,sz)
end
