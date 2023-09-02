if exist c:\xampp\htdocs set www=c:\xampp\htdocs\app\service
if exist "c:\program files\apache" set www="c:\program files\apache\htdocs\app\service"
if exist c:\xampp8\htdocs set www=c:\xampp8\htdocs\app\service

copy ..\install32\dgate.exe .
copy ..\dgate.dic .
start "Conquest service control manager, close when done installing" dgate.exe

md %www%\..
md %www%
copy dicom.ini %www%
copy service.lua %www%
copy dgate.php %www%
copy index.php %www%
copy config.php %www%
copy .htaccess %www%
copy ..\dgate.dic %www%
copy ..\install32\dgate.exe %www%
start "" "http://127.0.0.1/app/service/"
