set spath=%path%
call ..\asetcompiler64.bat
set path=%compiler%;%path%
set include=%include%
%ccompiler% /Zi /MT /nologo /O2 /W3 /c /D_CRT_SECURE_NO_DEPRECATE all.c
copy all.obj ..\build64\lua.obj

set path=%spath%
call ..\asetcompiler32.bat
set watcom=c:\lang\ms8
set path=%compiler%;%path%
set include=%include%
%ccompiler% /Zi /MT /nologo /O2 /W3 /c /D_CRT_SECURE_NO_DEPRECATE all.c
copy all.obj ..\build32\lua.obj
set path=%spath%

del *.pdb
del *.obj