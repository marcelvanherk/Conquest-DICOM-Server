set spath=%path%
call ..\asetcompiler64.bat
set path=%compiler%;%path%
rem set include=%compiler%\include
%ccompiler% /Zi /MT /nologo /O2 /W2 /c /D_CRT_SECURE_NO_DEPRECATE /DWIN32 /DWINDOWS /D_WINDOWS cong.cxx
lib /out:..\build64\dicom.lib cong.obj

set path=%spath%
call ..\asetcompiler32.bat
set path=%compiler%;%path%
rem set include=%compiler%\include
%ccompiler% /Zi /MT /nologo /O2 /W2 /c /D_CRT_SECURE_NO_DEPRECATE /DWIN32 /DWINDOWS /D_WINDOWS cong.cxx
lib /out:..\build32\dicom.lib cong.obj

set spath=%spath%
del *.pdb
del *.obj
