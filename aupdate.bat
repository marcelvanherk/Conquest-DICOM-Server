copy ..\dgate\dgate.exe .\install32
copy ..\dgate\dgate64.exe .\install64
copy ..\gui\win32\conquestdicomserver.exe .
copy ..\doc\windowsmanual.pdf .
copy ..\doc\DicomConformance_FilesLST_Changes.pdf .
copy ..\doc\linuxmanual.pdf .

copy dgate.dic .\linux
copy dgate.dic .\webserver\cgi-bin
copy dgate.dic .\webserver\cgi-bin\newweb

del .\webserver\cgi-bin\dgate.exe
del .\webserver\cgi-bin\newweb\dgate.exe
