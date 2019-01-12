------------------------------------------------------------------------------
-- dwv-simplistic (medical viewer using DWV) lua script 
--  for integration in a Conquest PACS server.
--
-- Usage:
-- 1. copy this file onto your web server
-- 2. in the 'dicom.ini' of your web server, create the dwv-simplistic viewer:
-- >> [dwv-simplistic]
-- >> source = dwvviewer_starter.lua
-- And set it as the default viewer:
-- >> [webdefaults]
-- >> ...
-- >> viewer = dwv-simplistic
-- 3. copy the dwv-simplistic source code from one of its release available at
-- https://github.com/ivmartel/dwv-simplistic/releases
-- in a 'dwv-simplistic' folder in the web folder of your web server. 
-- It should be accessible via '[server address]/dwv-simplistic'.
--
-- This script relies on the 'Dictionary' variable being set correctly.
----------------------------------------------------------------------------
-- mvh 20181124 Created by backporting ivmartel's script, 
--              reports error for study use
-- mvh 20181213 server_name does not copy port; use relative link instead
-- mvh 20181227 Use remotequery to limit contralize access to server 
-- mvh 20180112 Fix to allow : in patientID

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
dwv1 =
[[
<!DOCTYPE html>
<html>

<head>
<title>DICOM Web Viewer</title>
<meta charset="UTF-8">
<link type="text/css" rel="stylesheet" href="/dwv-simplistic/css/style.css">

<!-- Third party (dwv) -->
<script type="text/javascript" src="/dwv-simplistic/node_modules/i18next/i18next.min.js"></script>
<script type="text/javascript" src="/dwv-simplistic/node_modules/i18next-xhr-backend/i18nextXHRBackend.min.js"></script>
<script type="text/javascript" src="/dwv-simplistic/node_modules/i18next-browser-languagedetector/i18nextBrowserLanguageDetector.min.js"></script>
<script type="text/javascript" src="/dwv-simplistic/node_modules/jszip/dist/jszip.min.js"></script>

<!-- Decoders -->
<script type="text/javascript" src="/dwv-simplistic/node_modules/dwv/decoders/pdfjs/jpx.js"></script>
<script type="text/javascript" src="/dwv-simplistic/node_modules/dwv/decoders/pdfjs/util.js"></script>
<script type="text/javascript" src="/dwv-simplistic/node_modules/dwv/decoders/pdfjs/arithmetic_decoder.js"></script>
<script type="text/javascript" src="/dwv-simplistic/node_modules/dwv/decoders/pdfjs/jpg.js"></script>
<script type="text/javascript" src="/dwv-simplistic/node_modules/dwv/decoders/rii-mango/lossless-min.js"></script>

<!-- dwv -->
<script type="text/javascript" src="/dwv-simplistic/node_modules/dwv/dist/dwv.min.js"></script>
<!-- Launch the app -->
<script type="text/javascript" src="/dwv-simplistic/src/appgui.js"></script>

<script type="text/javascript">
// main application (global to be accessed from html)
var dwvApp = new dwv.App();
// start app function
function startApp() {
    // initialise the application
    dwvApp.init({
        "containerDivId": "dwv",
        "fitToWindow": true,
        "tools": ["Scroll", "ZoomAndPan", "WindowLevel"],
        "isMobile": true,
        "skipLoadUrl": true
    });
    // activate tools on load end
    dwvApp.addEventListener('load-end', function (/*event*/) {
        document.getElementById('tools').disabled = false;
        document.getElementById('reset').disabled = false;
    });
    var inputUrls = [
**** listofurls *****
    ];

    if( inputUrls && inputUrls.length > 0 ) dwvApp.loadURLs(inputUrls);
}; // end startApp

// Image decoders (for web workers)
dwv.image.decoderScripts = {
    "jpeg2000": "/dwv-simplistic/node_modules/dwv/decoders/pdfjs/decode-jpeg2000.js",
    "jpeg-lossless": "/dwv-simplistic/node_modules/dwv/decoders/rii-mango/decode-jpegloss.js",
    "jpeg-baseline": "/dwv-simplistic/node_modules/dwv/decoders/pdfjs/decode-jpegbaseline.js"
};
// status flags
var domContentLoaded = false;
var i18nInitialised = false;
// launch when both DOM and i18n are ready
function launchApp() {
    if ( domContentLoaded && i18nInitialised ) {
        startApp();
    }
}
// i18n ready?
dwv.i18nOnInitialised( function () {
    i18nInitialised = true;
    launchApp();
});

// check browser support
dwv.browser.check();
// initialise i18n
dwv.i18nInitialise("auto", "/dwv-simplistic/node_modules/dwv");
// DOM ready?
document.addEventListener("DOMContentLoaded", function (/*event*/) {
    domContentLoaded = true;
    launchApp();
});
</script>

</head>
<body>

<!-- DWV -->
<div id="dwv">

<!-- Toolbar -->
<div class="toolbar">
  <select id="tools" name="tools" onChange="dwvApp.onChangeTool({currentTarget: {value: this.value}})" disabled>
    <option value="Scroll" data-i18n="tool.Scroll.name">Scroll</option>
    <option value="WindowLevel" data-i18n="tool.WindowLevel.name">WindowLevel</option>
    <option value="ZoomAndPan" data-i18n="tool.ZoomAndPan.name">ZoomAndPan</option>
  </select>
  <button id="reset" value="Reset" onClick="dwvApp.onDisplayReset()" data-i18n="basics.reset" disabled>Reset</button>
</div>
<!-- Layer Container -->
<div class="layerContainer">
<div class="dropBox">Drag and drop data here.</div>
<canvas class="imageLayer">Only for HTML5 compatible browsers...</canvas>
</div><!-- /layerContainer -->

</div><!-- /dwv -->

</body>
</html>
]]

-- Main

if series2=='' then
  errorpage('this viewer does not support study level viewing')
  return
end

-- Get ids
local patientid = string.gsub(series2, ':[^:]-$', '')
local seriesuid = string.gsub(series2, '^.*:', '')
local studyuid = getstudyuid(patientid, seriesuid)
local images = queryimages(patientid, seriesuid)

-- create the url root for the js array
local urlRoot = script_name
urlRoot = urlRoot .. '?requestType=WADO&contentType=application/dicom'
urlRoot = urlRoot .. '&seriesUID=' .. seriesuid
urlRoot = urlRoot .. '&studyUID=' .. studyuid

-- Generate html
HTML('Content-type: text/html\n\n')

dwv1 = split(dwv1, '\n')
for k,v in ipairs(dwv1) do
  if string.find(v, '**** listofurls *****', 1, true) then
    for i=1, #images do
      print('      "'..urlRoot .. '&objectUID=' .. images[i].SOPInstanceUID..'",')
    end
  else
    print(v)
  end
end
