#
# 19990415: Documented previous changes: optimizer on; removed some goals from all
# 20080113: Adapted for MS7 compiler
# 20100119: Enabled JPEG2000 by setting HAVE_LIBJASPER
# 20100826: Set HAVE_J2K
# 20130429: Linker does not need /Zi /MT (lsp)
#
#
# Makefile
#
# University of California Davis Medical Center
# DICOM Network Transport Library
#
# System : WindowsNT/MSVC
# Endian : Big Endian
# Mode   : Production

LITTLE_ENDIAN=1
BIG_ENDIAN=2

##### Sub Library?

SUBCFLAGS = 
#SUBCFLAGS = -I..

####################################################################
MACHINE=WIN32
ENDIAN=$(LITTLE_ENDIAN)
CC = cl
CPP = cl
CFLAGS = -c /Zi /MT -D$(MACHINE) -DWINDOWS -D_WINDOWS -DHAVE_LIBJASPER -DHAVE_J2K -DNATIVE_ENDIAN=$(ENDIAN) $(SUBCFLAGS) /O2
# /O2 added (mvh)

LD = cl
#LFLAGS = /Zi /MT
LFLAGS =
LIBS = dicom.lib wsock32.lib

CFILES_UTIL = safemem.cxx uniq.cxx util.cxx
CFILES_SESSION = socket.cxx
CFILES_PDU = aaac.cxx aarj.cxx aarq.cxx buffer.cxx endian.cxx filepdu.cxx \
			flpdu.cxx pdata.cxx pdu.cxx rtc.cxx trnsyn.cxx
CFILES_DIMSEC = dimsec.cxx
CFILES_SOP = verify.cxx storage.cxx qrsop.cxx
CFILES_DDO = deivr.cxx
CFILES_IODCORE = cabs.cxx tvr.cxx vrref.cxx
CFILES_IODSUPPORT = 
CFILES_IODMODULES =

#CFILES_CONG = cong.cxx
CFILES_CONGCORE = congiod.cxx

CFILES_CONG = 	\
			$(CFILES_UTIL) $(CFILES_SESSION) \
			$(CFILES_PDU) $(CFILES_DIMSEC) \
			$(CFILES_SOP) $(CFILES_DDO)

CFILES_CONG = cong.cxx

CFILES_TEST = test.cxx
CFILES_PTEST = ptest.cxx

CFILES_TESTPR = dicomp.cxx printsop.cxx

.SUFFIXES:	.obj .c .cxx

.cxx.obj:
	$(CPP) $(CFLAGS) $*.cxx

.c.obj:
	$(CC) $(CFLAGS) $*.c

.cco.cxx:
	$(CCOC) $* < $*.cco

goal:	dicom.lib test.exe dicomp.exe
# ptest.exe dicomp.exe sendimage.exe

# Regular Debug build
#libdicom.so.1.1:	\
#			$(CFILES_UTIL:.cxx=.o) $(CFILES_SESSION:.cxx=.o) \
#			$(CFILES_PDU:.cxx=.o) $(CFILES_DIMSEC:.cxx=.o) \
#			$(CFILES_SOP:.cxx=.o) $(CFILES_DDO:.cxx=.o)
#	ld -o libdicom.so.1.1 \
#			$(CFILES_UTIL:.cxx=.o) $(CFILES_SESSION:.cxx=.o) \
#			$(CFILES_PDU:.cxx=.o) $(CFILES_DIMSEC:.cxx=.o) \
#			$(CFILES_SOP:.cxx=.o) $(CFILES_DDO:.cxx=.o)

# Special Build for release
dicom.lib: \
			$(CFILES_CONG:.cxx=.obj)
	lib /OUT:dicom.lib \
			$(CFILES_CONG:.cxx=.obj)


test.exe:	dicom.lib test.obj
	$(LD) $(LFLAGS) test.obj $(LIBS)  /FmTEST.map

ptest.exe:	dicom.lib ptest.obj
	$(LD) $(LFLAGS) ptest.obj $(LIBS)

sendimage.exe:	dicom.lib sendimage.obj
	$(LD) $(LFLAGS) sendimage.obj $(LIBS)
	
dicomp.exe:	dicom.lib $(CFILES_TESTPR:.cxx=.obj)
	$(LD) $(LFLAGS) $(CFILES_TESTPR:.cxx=.obj) $(LIBS) /Fmdicomp.map

