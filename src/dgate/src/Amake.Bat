rem put Msvcrt.lib before odbc32.lib for newer versions of visual studio
rem add legacy_stdio_definitions.lib for ms19

copy ..\openjpeg\opj_config.h.ms8 ..\openjpeg\opj_config.h
copy ..\openjpeg\opj_inttypes.h.ms8 ..\openjpeg\opj_inttypes.h
copy ..\openjpeg\opj_config_private.h.ms8 ..\openjpeg\opj_config_private.h

set spath=%path%
call ..\asetcompiler64.bat
set path=%compiler%;%path%
set include=%include%;..\dicomlib;..\jpeg-6c;..\sqlite3;C:\mysql-5.0.22-win32\include;..\lua_5.1.5;..\openjpeg;..\charls
%ccompiler% /Zi /MT /nologo /O2 /W2 /c /D_CRT_SECURE_NO_DEPRECATE -DWIN32 -DWINDOWS -DNATIVE_ENDIAN=1 -DUSESQLITE -DUSEMYSQL -DPOSTGRES -DHAVE_LIBJPEG -DHAVE_LIBCHARLS -DHAVE_LIBOPENJPEG2 total.cpp
copy total.obj ..\build64\total.obj
link /OUT:..\build64\dgate.exe /MAP:..\build64\dgate.map /DEBUG ..\build64\total.obj        ..\build64\sqlite3.obj ..\build64\lua.obj odbc32.lib odbccp32.lib wsock32.lib advapi32.lib user32.lib kernel32.lib ..\build64\charls.obj ..\build64\libjpeg.lib ..\build64\luasocket.lib ..\build64\openjp2.obj
copy ..\build64\dgate.exe ..\dgate64.exe

set path=%spath%
call ..\asetcompiler32.bat
set path=%compiler%;%path%
set include=%include%;..\dicomlib;..\jpeg-6c;..\sqlite3;C:\mysql-5.0.22-win32\include;..\lua_5.1.5;..\openjpeg;..\charls
%ccompiler% /Zi /MT /nologo /O2 /W2 /c /D_CRT_SECURE_NO_DEPRECATE -DWIN32 -DWINDOWS -DNATIVE_ENDIAN=1 -DUSESQLITE -DUSEMYSQL -DPOSTGRES -DHAVE_LIBJPEG -DHAVE_LIBCHARLS -DHAVE_LIBOPENJPEG2 total.cpp
copy total.obj ..\build32\total.obj
link /OUT:..\build32\dgate.exe /MAP:..\build32\dgate.map /DEBUG ..\build32\total.obj        ..\build32\sqlite3.obj ..\build32\lua.obj odbc32.lib odbccp32.lib wsock32.lib advapi32.lib user32.lib kernel32.lib ..\build32\libjpeg.lib  ..\build32\charls.obj ..\build32\luasocket.lib ..\build32\openjp2.obj
copy ..\build32\dgate.exe ..\dgate.exe

set path=%spath%
del *.obj
del *.pdb

copy ..\openjpeg\opj_config.h.linux ..\openjpeg\opj_config.h
copy ..\openjpeg\opj_inttypes.h.linux ..\openjpeg\opj_inttypes.h
copy ..\openjpeg\opj_config_private.h.linux ..\openjpeg\opj_config_private.h
