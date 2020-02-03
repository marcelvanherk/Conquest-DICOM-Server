copy ..\dgate\dgate.exe .\install32
copy ..\dgate\dgate64.exe .\install64
copy ..\servertask\cqdicom.dll .
copy ..\dgateserv\win32\dgateserv.exe .
copy ..\gui\win32\conquestdicomserver.exe .
copy ..\doc\windowsmanual.pdf .
copy ..\doc\DicomConformance_FilesLST_Changes.pdf .
copy ..\doc\linuxmanual.pdf .

copy dgate.dic .\linux
REM copy dgate.dic .\webserver\cgi-bin
REM copy dgate.dic .\webserver\cgi-bin\newweb

del .\webserver\cgi-bin\dgate.exe
del .\webserver\cgi-bin\newweb\dgate.exe
