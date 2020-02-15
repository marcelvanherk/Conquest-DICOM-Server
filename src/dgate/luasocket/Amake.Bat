set spath=%path%
call ..\asetcompiler64.bat
set path=%compiler%;%path%
set include=%include%;..\lua_5.1.5
%ccompiler% /Zi /MT /nologo /O2 /W3 /c /D_CRT_SECURE_NO_DEPRECATE luasocket.c auxiliar.c buffer.c except.c inet.c io.c mime.c options.c select.c tcp.c timeout.c udp.c wsocket.c
lib -out:luasocket.lib *.obj
copy luasocket.lib ..\build64
link /dll /machine:AMD64 /out:..\build64\core.dll /export:luaopen_socket_core luasocket.lib lua5.1.64.lib wsock32.lib

set path=%spath%
call ..\asetcompiler32.bat
set watcom=c:\lang\ms8
set path=%compiler%;%path%
set include=%include%;..\lua_5.1.5
%ccompiler% /Zi /MT /nologo /O2 /W3 /c /D_CRT_SECURE_NO_DEPRECATE luasocket.c auxiliar.c buffer.c except.c inet.c io.c mime.c options.c select.c tcp.c timeout.c udp.c wsocket.c
lib -out:luasocket.lib *.obj
copy luasocket.lib ..\build32
link /dll /machine:X86 /out:..\build32\core.dll /export:luaopen_socket_core luasocket.lib lua5.1.32.lib wsock32.lib

set path=%spath%

del *.obj
del *.pdb
del *.idb
del luasocket.lib

set spath=%path%
