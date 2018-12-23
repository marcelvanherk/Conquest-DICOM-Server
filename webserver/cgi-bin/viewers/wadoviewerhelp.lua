-- mvh 20171011: for 1.4.19b
-- mvh 20180113: Fixed minor inconsistencies; added frame help

HTML("Content-type: text/html\nCache-Control: no-cache\n");
HTML("<HEAD>")
print [[<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">]]
HTML("<TITLE>Conquest DICOM server - version %s</TITLE>", version)
	
print([[<style type="text/css">
	body { font:10pt Verdana; }
	a { color:blue; }
	#content { background-color:#dddddd; width:200px; margin-top:2px; }
	h1 {background-color:#6495ed;}
	</style>]])

HTML("</HEAD>")
		 		
HTML("<BODY BGCOLOR='CFDFCF'>")
HTML("<H1>Welcome to Conquest DICOM server - version %s</H1>", version)
HTML("<IMG SRC='%sconquest.jpg' ALT='Written by Conquest Project'>", Global.WebCodeBase)
HTML("<HR>")

HTML([[
On the <b>study</b> viewer, the left size of the page 
shows a list of all series in the selected study. Clicking 
the <b>arrow</b> next to a series brings it up in the 
image viewer on the right side of the page. 
In the <b>series</b> level viewer, the series list 
is absent but other viewer functions are identical.
<BR><BR>
By hovering the mouse over various elements around the  
viewer pane a <b><i>hint</i></b> line is displayed 
below the viewer pane providing hints for use of the 
mouse and the keyboard.
]])

HTML([[
<H4>Slice</H4>To page through the slices of a series you can hover the 
mouse over the <b>slice</b> text above the viewer or the 
viewer area itself and rotate the mouse wheel. Alternatively
you can use the <i>page up/down</i> keyboard keys. If you 
click on the <b>slice</b> text you can enter the requested slice 
number. Below the viewer area you can use the <b><</b> and 
<b>></b> buttons to slice down or up, or select a slice
from the dropdown list between these buttons, that 
shows the slice number and coordinate of all slices.
]])

HTML([[
<H4>Zoom and pan</H4>To zoom the slices of a series you can hover the 
mouse over the <b>zoom</b> text above the viewer and rotate the mouse wheel. Alternatively
you can use the <i>I</i> and <i>O</i> keyboard keys for 
zoom in and out respectively. Keyboard keys <i>F</i> resets
the zoom. If you click on the 
<b>zoom</b> text you can enter the requested zoom factor numerically. 
After zooming you can use the <i>arrow</i> keys to pan 
the image. 
]])

HTML([[
<H4>Level and window</H4>To change the contrast of the slices of a series 
you can hover the 
mouse over the <b>level</b> or <b>window</b> text above the viewer and rotate the mouse wheel. Alternatively
you can use the <i>B</i> and <i>D</i> keyboard keys for 
brighter and darker respectively and <i>H</i> and <i>L</i> keyboard keys for 
higher and lower contrast. Keyboard keys <i>P</i> load the 
preset level and window, and <i>A</i> disables windowing at all. 
Key <i>N</i> makes the display negative. 
If you click on the 
<b>level</b> or <b>window</b> text you can enter the 
requested level and window values numerically. 
]])

HTML([[
<H4>Frame</H4><b>For a multiframe object only</b>. To page through the frames in an object you can hover the mouse over the <b>frame</b> text above the viewer and rotate the mouse wheel. Alternatively you can use the <i>U/V</i> keyboard keys. If you click on the <b>frame</b> text you can enter the requested frame number. If this is the only image in the series you can also use the wheel over the viewer pane itself or the <b><</b> and <b>></b> buttons below the viewer, or the page up/down keys. 
]])

HTML([[
<H4>Other functions</H4>If you check <b>Auto save</b> changes 
to level, window, slice, zoom and pan are saved and restored for each 
series. Link <b>[show header]</b> or keyboard key <i>Q</i> lists 
the textual image header. Link <b>[help]</b> displays this text.
]])

HTML("</BODY>")
