By release, I mean the dwv-0.5.1.zip file that you can download there: https://github.com/ivmartel/dwv/releases/tag/v0.5.1.
The zip contains more than what you need, the bare minimum is: 'ext' and 'resources' folder, 'dwv-0.5.1.min.js' and 'style.css'

Put the sources in this folder without the dwv-0.5.1 subfolder and use dwv005.lua

Or put the sources here from dwv-0.16.0 without the dwv-0.16.0 subfolder and use dwv016.lua

Marcel

getting it to work was a bit of a puzzle. Here is a new dwv016.lua:

Note: dwv-0.16.0.min.js is not part of the source code, downloaded separately

Usage:

In the dicom.ini of your web server, create the dwv016 viewer:
[dwv016]
source = viewers/dwv016.lua

And set it as the default viewer:
[webdefaults]
viewer = dwv016

Then copy the DWV 0.16.0 distribution files (e.g. with the src, ext and viewers folders) into a 'dwv016' folder in the web folder of your web server.

Note: it is still missing a few resources because the path is not set correctly. E.g. it cannot find http://127.0.0.1/resources/touch_drag.png
