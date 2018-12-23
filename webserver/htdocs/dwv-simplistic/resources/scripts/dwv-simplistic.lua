--
-- dwv-simplistic (medical viewer using DWV) lua script 
--  for integration in a Conquest PACS server.
--
-- Usage:
-- 1. copy this file onto your web server
-- 2. in the 'dicom.ini' of your web server, create the dwv-simplistic viewer:
-- >> [dwv-simplistic]
-- >> source = dwv-simplistic.lua
-- And set it as the default viewer:
-- >> [webdefaults]
-- >> ...
-- >> viewer = dwv-simplistic
-- 3. copy the dwv-simplistic source code from one of its release available at
-- https://github.com/ivmartel/dwv-simplistic/releases
-- in a 'dwv-simplistic' folder in the web folder of your web server. 
-- It should be accessible via '[server address]/dwv-simplistic'.
--
-- This script relies on the 'kFactorFile', 'ACRNemaMap' and 'Dictionary'
-- variables being set correctly.

-- Get ids

local patientid = string.gsub(series2, ':.*$', '')
local seriesuid = string.gsub(series2, '^.*:', '')

-- Functions declaration

function getstudyuid()
  local a, b, s
  s = servercommand('get_param:MyACRNema')
  b = newdicomobject()
  b.PatientID = patientid
  b.SeriesInstanceUID = seriesuid
  b.StudyInstanceUID = ''
  a = dicomquery(s, 'SERIES', b)
  return a[0].StudyInstanceUID
end

function queryimages()
  local images, imaget, b, s
  s = servercommand('get_param:MyACRNema')
  b = newdicomobject()
  b.PatientID = patientid
  b.SeriesInstanceUID = seriesuid
  b.SOPInstanceUID = ''
  images = dicomquery(s, 'IMAGE', b)

  imaget={}
  for k=0,#images-1 do
    imaget[k+1]={}
    imaget[k+1].SOPInstanceUID = images[k].SOPInstanceUID
  end
  table.sort(imaget, function(a,b) return a.SOPInstanceUID < b.SOPInstanceUID end)

  return imaget
end

-- Main

local studyuid = getstudyuid()
local images = queryimages()
-- create the url lua array
local urlRoot = webscriptadress
urlRoot = urlRoot .. '?requestType=WADO&contentType=application/dicom'
urlRoot = urlRoot .. '&seriesUID=' .. seriesuid
urlRoot = urlRoot .. '&studyUID=' .. studyuid
local urls = {}
for i=1, #images do
  urls[i] = urlRoot .. '&objectUID=' .. images[i].SOPInstanceUID
end

-- Generate html

HTML('Content-type: text/html\n\n')

-- paths with extra /dwv
print([[
<!DOCTYPE html>
<html>

<head>
<title>DICOM Web Viewer</title>
<meta charset="UTF-8">
<link type="text/css" rel="stylesheet" href="/dwv-simplistic/css/style.css">
]])

print([[
<!-- Third party (dwv) -->
<script type="text/javascript" src="/dwv-simplistic/node_modules/i18next/i18next.min.js"></script>
<script type="text/javascript" src="/dwv-simplistic/node_modules/i18next-xhr-backend/i18nextXHRBackend.min.js"></script>
<script type="text/javascript" src="/dwv-simplistic/node_modules/i18next-browser-languagedetector/i18nextBrowserLanguageDetector.min.js"></script>
<script type="text/javascript" src="/dwv-simplistic/node_modules/jszip/dist/jszip.min.js"></script>
]])

print([[
<!-- Decoders -->
<script type="text/javascript" src="/dwv-simplistic/node_modules/dwv/decoders/pdfjs/jpx.js"></script>
<script type="text/javascript" src="/dwv-simplistic/node_modules/dwv/decoders/pdfjs/util.js"></script>
<script type="text/javascript" src="/dwv-simplistic/node_modules/dwv/decoders/pdfjs/arithmetic_decoder.js"></script>
<script type="text/javascript" src="/dwv-simplistic/node_modules/dwv/decoders/pdfjs/jpg.js"></script>
<script type="text/javascript" src="/dwv-simplistic/node_modules/dwv/decoders/rii-mango/lossless-min.js"></script>
]])

print([[
<!-- dwv -->
<script type="text/javascript" src="/dwv-simplistic/node_modules/dwv/dist/dwv.min.js"></script>
<!-- Launch the app -->
<script type="text/javascript" src="/dwv-simplistic/src/appgui.js"></script>
]])

print([[
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
]])
-- create javascript url array
print([[
    var inputUrls = [
]])
for i=1, #images do
  print('      "'..urls[i]..'",')
end
print([[
    ];
]])
-- load data
print([[
    if( inputUrls && inputUrls.length > 0 ) dwvApp.loadURLs(inputUrls);
}; // end startApp
]])

print([[
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
]])

print([[
// check browser support
dwv.browser.check();
// initialise i18n
dwv.i18nInitialise("auto", "/dwv-simplistic/node_modules/dwv");
// DOM ready?
document.addEventListener("DOMContentLoaded", function (/*event*/) {
    domContentLoaded = true;
    launchApp();
});
]])

print([[
</script>
]])

print([[
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
]])
