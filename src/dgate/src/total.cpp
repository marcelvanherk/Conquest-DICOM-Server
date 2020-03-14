/* 
20020822	mvh	This file compiles to TOTAL.EXE = DGATE.EXE with, e.g., BC55 
20050102	mvh	Added NOINTJPEG flag: jpeg8/12/16 sources cannot be compiled at once
20050107	mvh	Changed order of compile
20050111	mvh	Added npipe.cpp and gpps.cpp for LINUX
20050118	mvh	Documented how to compile under linux and with visual c++
20050121	mvh	Changed filename to lower case
20050130	mvh	Added xvgifwr.c
20070315	mvh	bcb added DARWIN stuff
20070330	mvh	bcb added DARWIN compile example
20080820	mvh	Solaris compile example
20080910	bcb 	fixed DARWIN compile example for build version of mysql
20081016	mvh	Fix link to xvgifwr.cpp; 64 bit example code
20090209	mvh	Added jpeg_encoder.cpp
20090411	mvh	Added second SUN example
20100120	bcb	removed DARWIN (PPC, 10.4, now old) compile example.  JPEG lib. is internal, so undef NOINTJPEG if used.
20100123	mvh	Include stddef here for windows to avoid jasper.h compile problem
20100124	mvh	Also use gpps for windows.
20100224	bcb	Added defines for OpenJPEG
20100706	bcb	Allow both libaries at once
20100721	mvh	Merged
20100815	mvh	bcb added two defines; Merged
20140417        ea      Change the order of include files.
20160219        mvh     Use Bruce Bartons new jpeg sources.
			Note: CHARLS requires Visual studio 2015 to compile
20160220        mvh     Allow build without jpeg support
20160229        mvh     use LIBOPENJPEG2 define; add __iob_func to allow mixed VS2008/2015 compile
20160306        mvh     Use OPJ_STATIC link all codecs statically
20181217        mvh     Added printsop.cxx
20190109        mvh     Make uncompconv unconditional; is used in converter to bitmap image
20200203	mvh	Added LUA51EXTERN option for dynamic loading of lua5.1.dll
20200314	mvh	Added JPEGIGNORELOSSLESSJFIFCOLORSPACE
*/

/* Example to compile with BC55:
   bcc32 -v 							# source level debugging
	 -g255 							# max number of warnings
	 -lS:1000000 						# stack size
         -P 							# force C++ compile
	 -DWINDOWS -DWIN32 -DNATIVE_ENDIAN=1 -DNOINTJPEG	# defines
         -I\bc55\include;\conquest\cqdicom;\conquest\dgate 	# include directories (bcc, dicom lib, and dgate parts)
         -L\bc55\lib						# library directory
         total.cpp 						# this source file
         cw32mt.lib 						# multi-thread library

   Example to compile under linux:
   g++ -DUNIX -DNATIVE_ENDIAN=1 -DNOINTJPEG total.cpp -o dgate -lpthread

   Example to compile with visual c++:
   set path=e:\lang\msdev\bin
   set include=e:\lang\msdev\include
   set lib=e:\lang\msdev\lib
   cl /Zi /MD /O2 /DWIN32 /DWINDOWS /DNATIVE_ENDIAN=1 /DNOINTJPEG /FeDGATE.EXE total.cpp odbc32.lib odbccp32.lib wsock32.lib advapi32.lib user32.lib kernel32.lib

   Example to compile with visual c++ (64 bit):
   set path=e:\lang\ms8amd64\bin
   set include=e:\lang\ms8amd64\include;\quirt\comps\exe\dgate;\quirt\comps\dll\cqdicom
   set lib=e:\lang\ms8amd64\lib
   cl -c -MD -O2 -DWIN32 -DWINDOWS -DNATIVE_ENDIAN=1 -DNOINTJPEG -DUSESQLITE ..\total.cpp ..\sqlite3.c
   link /OUT:dgate.exe total.obj sqlite3.obj odbc32.lib odbccp32.lib wsock32.lib advapi32.lib user32.lib kernel32.lib bufferoverflowu.lib
	
   Example to compile under SOLARIS 10:

   /usr/sfw/bin/g++ -DUNIX -DNATIVE_ENDIAN=1 -DNOINTJPEG -DSOLARIS total.cpp -o dgate -lpthread -lsocket -lnsl -lposix4

   or 

   /opt/SUNWspro/bin/CC -w -DUNIX -DNATIVE_ENDIAN=1 -DNOINTJPEG -DSOLARIS total.cpp -o dgate -lpthread -lsocket -lnsl -lposix4
*/

#define _SH_DENYRW      0x10    /* deny read/write mode */
#define _SH_DENYWR      0x20    /* deny write mode */
#define _SH_DENYRD      0x30    /* deny read mode */
#define _SH_DENYNO      0x40    /* deny none mode */
#define _P_WAIT         0

#ifdef DARWIN /* a flavor of bsd unix */
#define BSD
#endif
#ifdef BSD
#define UNIX
#endif

#ifdef BCB_FIX // Became tired of retyping on different systems.
#define DATE_FIX // Changes a leading 0 of any date to a 1 (Allowed by the DICOM Box).
#define FUJI_FIX // Allows decompressing Fuji AC3 jpeg compressed,not encapsulated images.
#define NOVARAD_FIX // Works around the intentional corruptions NovaPacs (NovaRad) adds to the images.
#endif

#define JPEGIGNORELOSSLESSJFIFCOLORSPACE	// fixes issue with BomberBugs image 12345, is lossless but states YcR coding

#ifdef WINDOWS
#include <stddef.h>
#include <stdio.h>

#if _MSC_VER > 1700
  FILE _iob[] = { *stdin, *stdout, *stderr }; 
  extern "C" FILE * __cdecl __iob_func(void) { return _iob; }
#endif

#endif

#ifdef HAVE_LIBJPEG
#undef NOINTJPEG
// Used to make Libjpeg use int as UINT.
#ifndef UINT_MAX
#define UINT_MAX 0xffffffff
#endif
#endif

#ifdef DHAVE_LIBOPENJPEG2
#define HAVE_J2K
#endif

#ifdef HAVE_LIBJASPER
#ifdef HAVE_LIBOPENJPEG2
#define  HAVE_BOTH_J2KLIBS
#else // Only one lib
#define HAVE_J2K
#endif
#endif
#include "qrsop.cxx"
#include "filepdu.cxx"
#include "vrtosql.cpp"
#include "parse.cpp"
#include "loadddo.cpp"
#include "lex.cpp"
#include "dprintf.cpp"
#include "dgatefn.cpp"
#include "device.cpp"
#include "amap.cpp"
#include "dbsql.cpp"
#include "rtc.cxx"
#include "flpdu.cxx"
#ifdef LUA51EXTERN
#include "lua_dyn.c"
#endif

#include "storage.cxx"
#include "verify.cxx"
#include "dimsen.cxx"
#include "dimsec.cxx"
#include "util.cxx"		// zeromem, spacemem
#include "uniq.cxx"		// uniq16, uniq8
//#include "safemem.cxx"
#include "endian.cxx"
#include "trnsyn.cxx"
#include "pdu.cxx"
#include "pdata.cxx"
#include "aarj.cxx"
#include "aaac.cxx"
#include "aarq.cxx"
#include "deivr.cxx"
#include "socket.cxx"
#include "buffer.cxx"
#include "odbci.cpp"
#include "nkiqrsop.cpp"
#include "printsop.cxx"

#if defined(HAVE_LIBOPENJPEG2) || defined(HAVE_LIBJPEG) || defined(HAVE_LIBCHARLS)
#define strnieq(a, b, c) stricmp(a,b)
#define strnlen32u(a, b) strlen(a)
#define strcpylim(a, b, c) strcpy(a, b)
#define OPJ_STATIC

#include "imagedata.cpp"
#include "jpegconv.cpp"
#endif

#include "uncompconv.cpp"

#include "regen.cpp"
#include "dgate.cpp"
#include "xvgifwr.cpp"
#ifndef HAVE_LIBJPEG
#include "jpeg_encoder.cpp"
#endif
#include "gpps.cpp"

#ifdef UNIX
#  include "npipe.cpp"
#endif
