rem for 1.4.19

md build32
md build64

cd charls
del *.obj *.exe *.lib
call amake
del *.obj *.exe *.lib
cd ..

cd dicomlib
del *.obj *.exe *.lib
call amake
del *.obj *.exe *.lib
cd ..

cd jpeg-6c
del *.obj *.exe *.lib
call amake
del *.obj *.exe *.lib
cd ..

cd lua_5.1.5
del *.obj *.exe *.lib
call amake
del *.obj *.exe *.lib
cd ..

cd luasocket
del *.obj *.exe *.lib
call amake
del *.obj *.exe *.lib
cd ..

cd openjpeg
del *.obj *.exe *.lib
call amake
del *.obj *.exe *.lib
cd ..

cd sqlite3
del *.obj *.exe *.lib
call amake
del *.obj *.exe *.lib
cd ..

cd src
del *.obj *.exe *.lib
call amake
del *.obj *.exe *.lib
cd ..

del build32\*.*
del build64\*.*
rmdir build32
rmdir build64
