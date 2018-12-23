-- default script; used if [DefaultPage] source=*.lua in dicom.ini

HTML('Content-type: text/html\n\n');

HTML('Default script: reached because <i>[DefaultPage] source=*.lua</i> is set in <b>dicom.ini</b>');
HTML('<br>')
HTML('<br>')
HTML('<br>')

HTML('<A href=dgate.exe?mode=top>Enter dicom server<BR></A>')
HTML('<br>')
HTML('<A href=dgate.exe?mode=top&key=aap:noot>Enter dicom server to select data<BR></A>')
HTML('<br>')
HTML('<A href=newweb/dgate.exe>New scripted web interface<BR></A>')
HTML('<br>')
