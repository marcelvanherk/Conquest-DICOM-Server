set spath=%path%
call ..\dgate\asetcompiler32.bat
set path=%compiler%;%path%
rem set include=%compiler%\include
cl /Zi /MT /nologo /O2 /W2 /c /I..\dgate\dicomlib /I..\dgate\lua_5.1.5 /D_CRT_SECURE_NO_DEPRECATE /DWIN32 /DWINDOWS /D_WINDOWS ..\dgate\lua_5.1.5\all.c
cl /Zi /MT /nologo /O2 /W2 /c /I..\dgate\dicomlib /I..\dgate\lua_5.1.5 /D_CRT_SECURE_NO_DEPRECATE /DWIN32 /DWINDOWS /D_WINDOWS ..\dgate\dicomlib\cong.cxx
cl /Zi /MT /nologo /O2 /W2 /c /I..\dgate\dicomlib /I..\dgate\lua_5.1.5 /D_CRT_SECURE_NO_DEPRECATE /DWIN32 /DWINDOWS /D_WINDOWS /DDLL servertask.cxx
link /dll /out:cqdicom.dll /def:cqdicom.def cong.obj servertask.obj wsock32.lib
cl /Zi /MT /nologo /O2 /W2 /c /I..\dgate\dicomlib /I..\dgate\lua_5.1.5 /D_CRT_SECURE_NO_DEPRECATE /DWIN32 /DWINDOWS /D_WINDOWS /DEXE servertask.cxx
link /out:servertask.exe cong.obj servertask.obj wsock32.lib all.obj

set spath=%spath%
del *.pdb
del *.obj
del *.exp
del *.lib

rem FOR /L %i IN (1,1,1000) DO servertask "servercommand[[lua:print(%i)]]" - takes 14s
rem FOR /L %i IN (1,1,1000) DO servertask "print(%i)" - takes 7s
