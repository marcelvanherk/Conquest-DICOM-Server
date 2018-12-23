HTML('Content-type: text/html\n\n');

HTML('<HEAD><TITLE>Conquest DICOM server - version ' .. CGI('size') .. CGI('windowname') .. '</TITLE></HEAD>');
HTML('<BODY BGCOLOR=CFDFCF>');
HTML('<H2>Conquest DICOM server - version '..CGI('series') .. ' and AiViewer v1.00</H2>');
print(servercommand('get_amaps:%s,'));
HTML('<APPLET CODEBASE='..CGI('webcodebase')..'dicomviewer100 CODE="dicomviewer.Viewer.class"');
HTML('NAME="Viewer.java" WIDTH="100%" HEIGHT="90%" HSPACE=0 VSPACE=0 ALIGN=middle >');
HTML('<PARAM NAME = tmpSize VALUE = 10>');
HTML('<PARAM NAME = NUM VALUE = 0>');
HTML('<PARAM NAME = currentNo VALUE = 0>');
-- HTML('<PARAM NAME = dicURL VALUE = '%webcodebase%dicomviewer100/Dicom.dic'>');
HTML('<PARAM NAME = imgURL0 VALUE = ');
-- servercommand('imagelister:local|'..CGI('patid')..'|'..CGI('seruid')..'|:'..CGI('webscriptadress')..'?'..CGI('extra')..'&mode=dicom&slice=%s:%s&compress=un*');
HTML('>');
HTML('</APPLET>');
HTML('</BODY>');
