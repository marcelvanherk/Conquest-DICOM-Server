copy opj_config.h.ms8 opj_config.h
copy opj_inttypes.h.ms8 opj_inttypes.h
copy opj_config_private.h.ms8 opj_config_private.h

set spath=%path%
call ..\asetcompiler64.bat
set path=%compiler%;%path%
set include=%include%
%ccompiler% /Zi /MT /nologo /O2 /W3 /c /D_CRT_SECURE_NO_DEPRECATE /DNDEBUG all.c
copy all.obj ..\build64\openjp2.obj

set path=%spath%
call ..\asetcompiler32.bat
set path=%compiler%;%path%
set include=%include%
%ccompiler% /Zi /MT /nologo /O2 /W3 /c /D_CRT_SECURE_NO_DEPRECATE /DNDEBUG all.c
copy all.obj ..\build32\openjp2.obj

set path=%spath%
del *.pdb
del *.obj

copy opj_config.h.linux opj_config.h
copy opj_inttypes.h.linux opj_inttypes.h
copy opj_config_private.h.linux opj_config_private.h
