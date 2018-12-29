rem 20160424 added HAVE_USLEEP

set spath=%path%
call ..\asetcompiler64.bat
set path=%compiler%;%path%
set include=%include%
%ccompiler% /Zi /MT /nologo /O2 /W3 /c /D_CRT_SECURE_NO_DEPRECATE /DHAVE_USLEEP sqlite3.c
copy sqlite3.obj ..\build64

set spath=%path%
call ..\asetcompiler32.bat
set path=%compiler%;%path%
set include=%include%
%ccompiler% /Zi /MT /nologo /O2 /W3 /c /D_CRT_SECURE_NO_DEPRECATE /DHAVE_USLEEP sqlite3.c
copy sqlite3.obj ..\build32

del *.pdb
del *.obj
