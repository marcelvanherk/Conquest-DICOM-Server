--
-- DICOM Web Viewer (DWV) lua script for integration in a Conquest PACS server. 
--
-- Usage
-- In the dicom.ini of your web server, create the dwv viewer:
-- >> [dwv016]
-- >> source = viewers/dwv016.lua
-- And set it as the default viewer:
-- >> [webdefaults]
-- >> ...
-- >> viewer = dwv016
-- This script relies on the 'kFactorFile', 'ACRNemaMap' and 'Dictionary'
-- variables being set correctly.
-- Then copy the DWV distribution files (e.g. with the src, ext and viewers folders) 
--   into a 'dwv016' folder in the web folder of your web server.

-- Get ids
--series2 = series2 or '20000321:1.2.840.113619.2.22.287.1.529.1.20000719.265546'
--webscriptadress = webscriptadress or 'http://127.0.0.1/dgate.exe'
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

-- create the HTML page, taken from dwv sample page
-- Note: dwv-0.16.0.min.js is not part of the source code, downloaded separately
HTML('Content-type: text/html\n\n')

local page_1 = 
[[
<!DOCTYPE html>
<html manifest="cache.manifest">

<head>
<title>DICOM Web Viewer</title>
<meta charset="UTF-8">
<meta name="description" content="DICOM Web Viewer (DWV) static version">
<meta name="keywords" content="DICOM,HTML5,JavaScript,medical,imaging,DWV">
<link type="text/css" rel="stylesheet" href="/dwv016/css/style.css">
<style type="text/css" >
body { background-color: #222; color: white;
  margin: 10px; padding: 0; font-size: 80%; }
#pageHeader h1 { display: inline-block; margin: 0; color: #fff; }
#pageHeader a { color: #ddf; }
#pageHeader .toolbar { display: inline-block; float: right; }
.toolbox li:first-child { list-style-type: none; padding-bottom: 10px; margin-left: -20px; }
#pageMain { position: absolute; height: 92%; width: 99%; bottom: 5px; left: 5px; background-color: #333; }
.infotl { color: #333; text-shadow: 0 1px 0 #fff; }
.infotr { color: #333; text-shadow: 0 1px 0 #fff; }
.dropBox { margin: 20px; }
</style>
<link type="text/css" rel="stylesheet" href="/dwv016/ext/jquery-ui/themes/ui-darkness/jquery-ui-1.11.4.min.css">
<!-- Third party -->
<script type="text/javascript" src="/dwv016/ext/jquery/jquery-2.1.4.min.js"></script>
<script type="text/javascript" src="/dwv016/ext/jquery-ui/jquery-ui-1.11.4.min.js"></script>
<script type="text/javascript" src="/dwv016/ext/flot/jquery.flot.min.js"></script>
<script type="text/javascript" src="/dwv016/ext/kinetic/kinetic-v5.1.1-06.10.min.js"></script>
<script type="text/javascript" src="/dwv016/ext/i18next/i18next.min.js"></script>
<script type="text/javascript" src="/dwv016/ext/i18next/i18nextXHRBackend.min.js"></script>
<script type="text/javascript" src="/dwv016/ext/i18next/i18nextBrowserLanguageDetector.min.js"></script>
<script type="text/javascript" src="/dwv016/ext/magic-wand/magic-wand.js"></script>
<!-- decoders -->
<script type="text/javascript" src="/dwv016/ext/pdfjs/jpx.js"></script>
<script type="text/javascript" src="/dwv016/ext/pdfjs/util.js"></script>
<script type="text/javascript" src="/dwv016/ext/pdfjs/arithmetic_decoder.js"></script>
<script type="text/javascript" src="/dwv016/ext/rii-mango/lossless-min.js"></script>
<script type="text/javascript" src="/dwv016/ext/notmasteryet/jpg.js"></script>

<!-- Source -->
<script type="text/javascript" src="/dwv016/dwv-0.16.0.min.js"> </script>

<!-- Launch the app -->
<script type="text/javascript" src="/dwv016/src/tools/windowLevel.js"></script>
<script type="text/javascript" src="/dwv016/viewers/static/appgui.js"></script>
<script type="text/javascript">

// launch when page is loaded
// start app function
function startApp() {
    // gui setup
    dwv.gui.setup();

    // main application
    var myapp = new dwv.App();
    // initialise the application
    myapp.init({
        "containerDivId": "dwv",
        "fitToWindow": true,
        "gui": ["tool", "load", "help", "undo", "version", "tags"],
        "loaders": ["File", "Url"],
        "tools": ["Scroll", "WindowLevel", "ZoomAndPan", "Draw", "Livewire", "Filter", "Floodfill"],
        "filters": ["Threshold", "Sharpen", "Sobel"],
        "shapes": ["Line", "Protractor", "Rectangle", "Roi", "Ellipse"],
        "isMobile": false
    });

    // help
    // TODO Seems accordion only works when at end...
    $("#accordion").accordion({ collapsible: "true", active: "false", heightStyle: "content" });

    if( inputUrls && inputUrls.length > 0 ) myapp.loadURLs(inputUrls);
}

// Image decoders (for web workers)
dwv.image.decoderScripts = {
    "jpeg2000": "../../ext/pdfjs/decode-jpeg2000.js",
    "jpeg-lossless": "../../ext/rii-mango/decode-jpegloss.js",
    "jpeg-baseline": "../../ext/notmasteryet/decode-jpegbaseline.js"
};

// check browser support
dwv.browser.check();

// initialise i18n
dwv.i18nInitialise('auto', '../dwv016');

// launch when page is loaded
$(document).ready( function()
{
    // and i18n is loaded
    dwv.i18nOnLoaded( startApp );
});

]]

local page_2 =
[[
</script>
</head>

<body>

<!-- DWV -->
<div id="dwv">

<div id="pageHeader">

<!-- Title -->
<h1>DWV <span class="dwv-version"></span></h1>

<!-- Toolbar -->
<div class="toolbar"></div>

</div><!-- /pageHeader -->

<div id="pageMain">

<!-- Open file -->
<div class="openData" title="File">
<div class="loaderlist"></div>
<div id="progressbar"></div>
</div>

<!-- Toolbox -->
<div class="toolList" title="Toolbox"></div>

<!-- History -->
<div class="history" title="History"></div>

<!-- Tags -->
<div class="tags" title="Tags"></div>

<!-- Help -->
<div class="help" title="Help"></div>

<!-- Layer Container -->
<div class="layerDialog" title="Image">
<div class="dropBox"></div>
<div class="layerContainer">
<canvas class="imageLayer">Only for HTML5 compatible browsers...</canvas>
<div class="drawDiv"></div>
<div class="infoLayer">
<div class="infotl"></div>
<div class="infotr"></div>
<div class="infobl"></div>
<div class="infobr"><div class="plot"></div></div>
</div><!-- /infoLayer -->
</div><!-- /layerContainer -->
</div><!-- /layerDialog -->

</div><!-- /pageMain -->

</div><!-- /dwv -->

</body>
</html>
]]

local js_code = split(page_1, '\n')
for k,v in ipairs(js_code) do print(v) end

-- create javascript url array
print([[    var inputUrls = []])
for i=1, #images do
  print('      "'..urls[i]..'",')
end
print([[    ];]])

local js_code = split(page_2, '\n')
for k,v in ipairs(js_code) do print(v) end
