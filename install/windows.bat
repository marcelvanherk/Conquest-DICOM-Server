if exist c:\xampp\cgi-bin set cgi=c:\xampp\cgi-bin\service
if exist "c:\program files\apache" set cgi="c:\program files\apache\cgi-bin"

copy ..\install32\dgate.exe .
copy ..\dgate.dic .
start "Conquest service control manager, close when done installing" dgate.exe

md %cgi%
copy dicom.ini %cgi%
copy service.lua %cgi%
copy ..\dgate.dic %cgi%
copy ..\install32\dgate.exe %cgi%
start "" "http://127.0.0.1/cgi-bin/service/dgate.exe"
