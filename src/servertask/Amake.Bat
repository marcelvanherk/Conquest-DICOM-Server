set spath=%path%
call ..\dgate\asetcompiler32.bat
set path=%compiler%;%path%
rem set include=%compiler%\include
cl /Zi /MT /nologo /O2 /W2 /c /I..\dgate\dicomlib /D_CRT_SECURE_NO_DEPRECATE /DWIN32 /DWINDOWS /D_WINDOWS ..\dgate\dicomlib\cong.cxx
cl /Zi /MT /nologo /O2 /W2 /c /I..\dgate\dicomlib /D_CRT_SECURE_NO_DEPRECATE /DWIN32 /DWINDOWS /D_WINDOWS servertask.cxx
link /dll /out:cqdicom.dll /def:servertask.def cong.obj servertask.obj wsock32.lib

set spath=%spath%
del *.pdb
del *.obj
