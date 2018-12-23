set spath=%path%
call ..\asetcompiler64.bat
set path=%compiler%;%path%
set include=%include%;..\lua_5.1.5
cl /Zi /MT /nologo /O2 /W3 /c /D_CRT_SECURE_NO_DEPRECATE luasocket.c auxiliar.c buffer.c except.c inet.c io.c mime.c options.c select.c tcp.c timeout.c udp.c wsocket.c
lib -out:luasocket.lib *.obj
copy luasocket.lib ..\build64

set path=%spath%
call ..\asetcompiler32.bat
set watcom=c:\lang\ms8
set path=%compiler%;%path%
set include=%include%;..\lua_5.1.5
cl /Zi /MT /nologo /O2 /W3 /c /D_CRT_SECURE_NO_DEPRECATE luasocket.c auxiliar.c buffer.c except.c inet.c io.c mime.c options.c select.c tcp.c timeout.c udp.c wsocket.c
lib -out:luasocket.lib *.obj
copy luasocket.lib ..\build32
set path=%spath%

del *.obj
del *.pdb
del *.lib
del *.idb
