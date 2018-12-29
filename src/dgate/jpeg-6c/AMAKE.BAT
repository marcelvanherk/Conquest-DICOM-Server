REM 20100116: mvh modified to use bcb's libjpeg
REM 20100119: mvh generate 32 and 64 bit library

set spath=%path%
call ..\asetcompiler64.bat
set path=%compiler%;%path%

rem nmake -fmakefile.vc clean
del *.obj *.lib
nmake -fmakefile.vc conlibs=bufferoverflowu.lib ccompiler=%ccompiler%
copy libjpeg.lib ..\build64

set path=%spath%
call ..\asetcompiler32.bat
set path=%compiler%;%path%

rem nmake -fmakefile.vc clean
del *.obj *.lib
nmake -fmakefile.vc ccompiler=%ccompiler%
copy libjpeg.lib ..\build32

rem nmake -fmakefile.vc clean
del *.obj *.lib *.pdb
set path=%spath%
