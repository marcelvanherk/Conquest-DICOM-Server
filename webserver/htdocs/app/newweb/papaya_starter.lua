------------------------------------------------------------------------------
-- papaya (medical viewer) lua script 
--  for integration in a Conquest PACS server.
--
-- Usage:
-- 1. copy this file onto your web server
-- 2. in the 'dicom.ini' of your web server, create the papaya viewer:
-- >> [papaya_viewer]
-- >> source = papaya_starter.lua
-- And set it as the default viewer:
-- >> [webdefaults]
-- >> ...
-- >> viewer = papaya_starter
-- 3. copy the papaya source code from one of its release available at
-- https://github.com/rii-mango/Papaya/tree/master/release/current/standard
-- in a 'papaya' folder in the web folder of your web server. 
-- It should be accessible via '[server address]/papays'.
--
----------------------------------------------------------------------------
-- mvh 20181126 Created
-- mvh 20181213 server_name does not copy port; use relative link instead
-- mvh 20181227 Use remotequery to limit contralize access to server 
-- mvh 20180112 Fix to allow : in patientID
-- mvh 20230625 Made all links relative

local source = servercommand('get_param:MyACRNema')

function remotequery(ae, level, q)
  local remotecode =
[[
  local ae=']]..ae..[[';
  local level=']]..level..[[';
  local q=]]..q:Serialize()..[[;
  local q2=DicomObject:new(); for k,v in pairs(q) do q2[k]=v end;
  local r = dicomquery(ae, level, q2):Serialize();
  local s=tempfile('txt') local f=io.open(s, "wb") f:write(r) returnfile=s f:close();
]]
  local f = loadstring('return '..servercommand('lua:'..remotecode));
  if f then return f() end
end

-- Functions declaration
function getstudyuid(pat, series)
  local b = newdicomobject()
  b.PatientID = pat
  b.SeriesInstanceUID = series
  b.StudyInstanceUID = ''
  local a = remotequery(source, 'SERIES', b)
  return a[1].StudyInstanceUID
end

function queryimages(pat, series)
  local b = newdicomobject()
  b.PatientID = pat
  b.SeriesInstanceUID = series
  b.SOPInstanceUID = ''
  local images = remotequery(source, 'IMAGE', b)

  table.sort(images, function(a,b) return a.SOPInstanceUID < b.SOPInstanceUID end)
  return images
end

-- split string into pieces, return as table
function split(str, pat)
   local t = {} 
   local fpat = "(.-)" .. pat
   local last_end = 1
   local s, e, cap = str:find(fpat, 1)
   while s do
      if s ~= 1 or cap ~= "" then
	      table.insert(t,cap)
      end
      last_end = e+1
      s, e, cap = str:find(fpat, last_end)
   end
   if last_end <= #str then
      cap = str:sub(last_end)
      table.insert(t, cap)
   end
   return t
end

-- report error string on a otherwise empty reddish page
function errorpage(s)
  HTML('Content-type: text/html\n\n');
  print(
  [[
  <HEAD><TITLE>Version ]].. (version or '-')..[[ WEB interface</TITLE></HEAD>
  <BODY BGCOLOR='FFDFCF'>
  <H3>Conquest ]].. (version or '-') ..[[ WEB interface</H3>
  <H3>]].. s .. [[</H3>
  </BODY>
  ]])
end

-- this block is the full html of a dwv app, where **** listofurls ***** is replaced
-- with a list of urls
papaya1 =
[[
<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml" lang="en">
	<head>
    	<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    
    	<!-- iOS meta tags -->
    	<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no"/>
    	<meta name="apple-mobile-web-app-capable" content="yes">
    	<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
    
    	<link rel="stylesheet" type="text/css" href="/papaya/papaya.css?build=1446" />
    	<script type="text/javascript" src="/papaya/papaya.js?build=1446"></script>
    
    	<title>Papaya Viewer</title>
    
    	<script type="text/javascript">
	var params = [];
	params["images"] = 
	[
	[
**** listofurls *****
	]
	];
	params["showOrientation"] = true;
	params["radiological"] = true;
	</script>

	</head>

	<body>
		<div class="papaya" data-params="params"></div>
	</body>
</html>
]]

-- Main

if series2=='' then
  errorpage('this viewer does not support study level viewing yet')
  return
end

-- Get ids
local patientid = string.gsub(series2, ':[^:]-$', '')
local seriesuid = string.gsub(series2, '^.*:', '')
local studyuid = getstudyuid(patientid, seriesuid)
local images = queryimages(patientid, seriesuid)

-- create the url root for the js array
local urlRoot = '' -- script_name
urlRoot = urlRoot .. '?requestType=WADO&contentType=application/dicom'
urlRoot = urlRoot .. '&seriesUID=' .. seriesuid
urlRoot = urlRoot .. '&studyUID=' .. studyuid

-- Generate html
HTML('Content-type: text/html\n\n')

papaya1 = split(papaya1, '\n')
for k,v in ipairs(papaya1) do
  if string.find(v, '**** listofurls *****', 1, true) then
    for i=1, #images-1 do
      print('      "'..urlRoot .. '&objectUID=' .. images[i].SOPInstanceUID..'",')
    end
    print('      "'..urlRoot .. '&objectUID=' .. images[#images].SOPInstanceUID..'"')
  else
    print(v)
  end
end
