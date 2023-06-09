/*
19990317	ljz	NKI-specific code
19990903	ljz	Implemented NKI MaxRowColumn VR in DCO
19990906	ljz	Improved implementation of MaxRowColumn
19991022	ljz	Changed the root of NKI-made UIDs to the official one supplied by
			Medical Connections(UK): 1.2.826.0.1.3680043.2.135.1066.xxxxxxxxxx
19991122	ljz	Added NKI-CompressionMode 2: safer, with CRC checks
20000131	ljz	Regardless the FileCompressMode in Dicom.ini, always send images
			compressed (mode=2) when communicating with an NKI application.
20001105	mvh	Fixed where malloc and new were mixed (vr->data)
20001106	mvh	Use delete [] operation for vr->Data
20001106	mvh	Temporary fix of BC leak in CompressNKI and DecompressNKI
20001120	mvh	Fix bug - vr->data was reallocated without changing vr->length
20001127	ljz	Fix: Crashes happened when more than one 'ServerChildThread' was
			active. m_pDCO is wrongly shared by all threads !!
20010424	mvh	Plugged in faster and safer decompress
20010426	ljz	Fixed leak when connection is aborted
20010429	mvh	Interleaved CRC computation with compress code (factor 2 faster)
			Added GetADDO to allow read ahead within calling program
20010501	mvh	Merged ljz's change of 20010426                      
20010502	mvh	Added extrabytes pointer to Read and RetrieveOn
20010502	mvh	Added vr(9999,0300)=SliceLimit: send at most so many slices
20010502	mvh	Changed this vr to (9999,0500)=SliceLimit
20011219	ljz	Added extra check in case other manufacturers than NKI use
			VR (0x7fe0, 0x0010), such as ACUSON
20020317	mvh	Replace true by TRUE
20020415	mvh	Added error handling on ProcessDDO
20020429 	mvh	fixed reporting of number of complete slices (now starts with 1)
			return cancel status if C_STORE failed
20020609	mvh	Added compressor modes 3 and 4
20020609	mvh	Added VR (9999,0600): MaxCompressionLevel (default 2)
			Note: This level (max of 4) is also used when downsizing
20020613	ljz	Exported DecompressNKI and CompressNKI
20020619	ljz	Damn! ELSCINT uses (0x7fe0, 0x0010) too (see modification on 20011219)
			Extra check on zero-length pixeldata
20020930	mvh	Fixed SliceLimit VR
20021016	mvh	Removed __BORLANDC__ language construct - to fix downsize with BC55
20021115	mvh	Added Generic style retrieve classes; print error messages from c-move
20030522	ljz	Added function ComputeCRC
20030605	ljz	Fix in DecompressNKI: dgate crashed when DCMTK's 'storescu.exe' wants
			to send an NKIcompressed image to dgate; it refuses to send the pixeldata!
20030701	mvh	QualifyOn now also has compression parameter; transfer syntax work in progress
20030703	mvh	Tested recompress functions
20030704	mvh	Disabled decompression in ProcessDDO for non-nki clients; is done by recompress
			When downsizing use DecompressImage(); also supports JPEG; Removed MaybeRecompress
			Recompression for NKI clients only when downsizing; otherwise as specified in acrnema.map
20030705	mvh	Moved recompression out of ProcessDDO into StandardRetrieveNKI::Read
			Skip recompression to same level as incoming
20030706	mvh	Optional filename parameter for recompress (not used yet); set env for offis tools
20030706	mvh	Attach VRType to PDU's for implicit little endian support
20030709	mvh	Removed DCMDICPATH; is not needed
20030819	mvh	Allow longer filenames
20030905	mvh	Note: recompressfile; DecompressImageFile and CompressJPEGImageFile do not allow spaces in filename
20040401	mvh	Added Changed and ActualMode flags to compress routines; only give compress message if something changed
			Set maxcompression to 4 for NKI send
20040403	mvh	Added own SaveDICOMDataObject as in dgate.cpp
20040406	ljz	Also copy TypeCode in DecompressNKI when converting (0x7fdf, 0x0010) to (0x7fe0, 0x0010)
			Make TypeCode of (0x7fdf, 0x0010) OW instead of OB, because OFFIS refuses to compress OB
20040406	mvh	Always use EXPLICIT syntax for jpeg, fix v2 always use DUMP
			Use -F +ti option for dcmdjpeg for writing to v2: creates raw dump, implicit
			Refuse to jpeg compress V2 files and images != 16 bits; \n is all messages
			Fix error handling of CompressJpegImage
20040530	mvh	Removed unused variable
20040722	mvh	Added "nj" compression: leaves JPEG as is; else MAXNKICOMPRESSION
20041101	ljz	Added built-in JPEG and RLE decompressor
20041102	mvh	Fix problem with external JPEG compression: clear transfer syntax prior 
			to save for OFFIS tools with DICOM_CHAPTER_10_EXPLICIT
20041103	mvh	Added k1,k2,k4,k8 compressors: downsize to 1024,512,256,128 pixels if bigger to start with
20041112	mvh	Called AE SERVER~xx in C-MOVE lets xx override outgoing compression as well 
			(e.g., call CONQUESTSRV1~k4 to always get images downsized to 256x256)
20050102	mvh	Added NOINTJPEG flag to allow compile through total.cxx
20050103	mvh	Changed include file to lower case (for linux)
20050108	mvh	Adapted for linux compile (work in progress)
20050109	mvh	Added configurable TCPIPTimeOut
20050118	mvh	replaced thread local storage under linux with variables in object
20050119	mvh	added rudimentary support to start external (de)compressors under linux
20050121	mvh	Changed filename to lower case
20050122	mvh	Improved external jpeg support under Linux: load dictionary, wait until ready
20050130	mvh	Added To8BitMonochrome and ToGif
20050204	mvh	Small fix in ToGif for non-square images; also interpret RescaleIntercept as float
20050205	mvh	Added counters for compression activity
20050206	mvh	Optimized downsizing: do not decompress if no downsizing to occur
20050206	mvh	Note: replaced thread local storage under linux with variables in object is INCORRECT
			Under linux, the server now can also serve one at a time
			Fix for color to gif; added limited auto scaling in 16 bits to gif
20050211	mvh	Removed need for thread local storage
20051210	mvh	Added "vX" compression (same as as is)
20051217	mvh	Use system() to call on offis apps in linux (now works)
20051217	mvh	Use new VR::ReAlloc method to replace data in VR; fixed 2 float warnings
20051229	mvh	Fixed several leaks in (jpeg)compression code; changed alloc for nki compression
20051230	mvh	Added +sr option to dcmcjpeg: compress without changing rescaleslope/intercept
			This option is required for our viewers: slices must all have same rescale
20060219	mvh	Removed warning in writegif
20060222	mvh	Added extra checks (not on ID strings) to pass as NKI compressed data
20060311	mvh	dcmcjpeg opts string got truncated (thanks Rainer Libowski)
			fixed unused CompressJPEGImageFile (wrong exe called)
			external decompress temporary files now in printer_files (thanks Rainer Libowski)
20060324	mvh	Added StripGroup2 option to recompress; strip group 2 from outgoing c-stores if not compression "as" or "is"
20060328	mvh	Use IsAbstractSyntaxAccepted(iUID) to test if host accepts image
20060402	mvh	Fix crash when recieving or dropping jpeg compressed color images
			Consider not removing transfer syntax (0x0002, 0x0010) prior to dcmdjpeg/dcmcjpeg
			Would fix problem with external decoding color jpeg images
20060402	mvh	Changed IsAbstractSyntaxAccepted(iUID) test back (thanks Andrej Savelov)
20060405	mvh	That was not the problem
20060618	mvh	Tested IsAbstractSyntaxAccepted - seems to work. Removed need for min()
20060619	mvh	Do not remove transfer syntax before decompression (20060402 suggestion)
			Maintain original object when decompression fails, changed wording of message
			Only complain about internal decompressor for jpeg
20070104	mvh	Export automatic SaveDICOMDataObject
20070113	mvh	Never delete transfer syntax (was left in CompressJpegImage, solves coding error in jpeg store)
			Tested with $c server debug command
20070210	mvh	Export MaybeDownsize and check on 0 for passed size
20070307	mvh	Added several debug prints in retrieve, e.g. at bail out because connection dropped
20070308	bcb	removed #include <wait.h> from DARWIN
20070314	mvh	Send 0000,1030 (MoveOriginatorAE) and 0000,1031 (MoveOriginatorMessageID) in SS->Write as result of Move
20070315	mvh	Merged bcb DARWIN stuff; Set TCPIPTimeOut for PDU that moves images
20070316	mvh	Added (color) ToBMP for convert_to_bmp; added level and window to ToGif and ToBMP
20070407	mvh	Use ~p1..9A..Z to override C-MOVE port (adds 1..36 to port number)
20070902	mvh	WriteGif now cpp
20071027	mvh	Added quotes around filenames passed to dcmdjpeg and dcmcjpeg: avoids errors with spaces in paths
20071101	mvh	Enabled JPEG compression and decompression of non-16 bit data
20071104	mvh	Added DecompressNon16BitsJpeg (default TRUE)
20071105	mvh	Restore image when dcmc/djpeg failed to start
20080103	mvh	Added mode 'uj': leave jpeg as is, otherwise uncompress
			Added DecompressImage to recompress()
20080820	mvh	Solaris fix (do not use variable name SS)
20081116	mvh	Adapted for very large objects (use unsigned int for length)
20081201	mvh	Added JPG converter
20081203	mvh	Enabled internal decompressor for color, added frame extraction for jpg etc
			Add frame extraction control = 0x09999,0x0201
20090209	mvh	Added QueryMoveScript callback; removed unused variable
20090325	mvh	Added RetrieveResult converter, impact on performance to be tested
20090412	mvh	Changed jpeg encode_image mode to correct BGR RGB mixup
20090513	mvh	Added modes 's0' to 's9' call script CompressionConverter0 to 9
20090518	mvh	Added modes ka, kb and kc to downsize to 64, 32 and 16
20090921	mvh	Added ExtractFrames
20090927	mvh	Stop jpg compression for PR SR and RT
20091231	bcb	Added HAVE_LIBJPEG (version 6c!) and HAVE_LIBJASPER for external library support (beta for now)
			Changed char* to const char* and cast time_t as int for gcc4.2 warnings
20100111	mvh	Fixed downsize for 32 bits pixel; bcb const; cast of Time() to (int)
20100112	mvh	Merged
20100113	mvh	A few more errors in downsize for 32 bits
20100116	mvh	Fixed DecompressImage compile error if HAVE_LIBJPEG is defined
20100118	mvh	Added one missing else in CompressJPEG2K
20100119	bcb	The else was not really missing; added some const
20100120	mvh	Catch jpeg2000 configuration if HAVE_LIBJASPER not defined
20100121	bcb+mvh	Blocked out old jpeg decompressor if HAVE_LIBJPEG, 
			removed color checks as both decompressors do color now
		mvh	Made lossy to lossless switch a warning not error in CompressImage
			Set mode to '1' instead of 1 after switch
20100122	mvh	Note: the HAVE_LIBJPEG code supresses RLE support
20100122	mvh	Added experimental C-CANCEL support for all C-MOVES
20100123	mvh	Added counters for jpeg2000
20100124	mvh	Added GetNumberOfFrames
20100125	mvh	GetPrivateProfileString -> MyGetPrivateProfileString
20100126	mvh	Include gpps.hpp
20100207	mvh	Fixed ExtractFrame(s) for frames packaged into pixel data without sequences
20100209	mvh	Run RetrieveResult also for non-nki clients
20100210	bcb	Fix memory leak
20100214	mvh	Merged
20100223	bcb	Combined DecompressImage and DecompressImageFile 
			Add FUJI_FIX for evil LittleEndianImplicit jpeg compressed images.
20100224	bcb	Added libopenjpeg to use in place of Jasper, fixed more warnings (-Wall)
20100227	mvh	After 1.4.16alpha3: crash when jpeg compressing RT object
20100330	bcb	Frames to LIBJPEG, JASPER, and OPENJPEG
20100619	bcb	Many fixes for leaks, allow both Jasper and openJPEG as a dicom.ini choice (not finished).
20100703	mvh	Merged; snprintf to _snprintf
20100706	bcb	Merged, replaced _snprintf with memcpy.
20100721	mvh	Merged back; fixed a few ascii to int conversions with atoi
20100723	mvh	Merged bcb fixes in new version of ToJPG
20100728	bcb	Installed RLE support with HAVE_LIBJPEG, used new deivr Get's and ChangeVR's
20100815	mvh	Merged; do not use GETUINT8 for US type; added space =TRUE to setting PixelSpacing
			Simplified DecompressRLE
20100815	mvh	Fixed bug 30 in t=2127: YBR_FULL_422 images not passed to dcmcjpg
20100823	mvh	bcb moved that check to the right place
20100824	mvh	Pass local VariableVRs to script engine
20100826	mvh	bcb fixed lossy jpeg2000 for signed data
20100901	mvh	bcb Added fix for PlanarConfiguration=1 data
20100905	mvh	Removed file functions: now RecompressFile uses Recompress
20100905	mvh	Merged; Added UseBuiltInJPEG flag
20100914	bcb	Fixed RLE decode for frames, added Deplane, DecodePalette for regular and segmented palettes
20100918	mvh	Attempted merged (very difficult as fixes were made in older version)
20100920	bcb	Fixed warnings
20100922	mvh	Merged
20100928	mvh	Fixed ExtractFrame(s) for NKI compressed data
20101003	mvh	Added 9999,0900 script
20101004	mvh	Use updated gif codec; started on animated gif
20101009	mvh	Protected gif coder for out of memory
20101018	mvh	Swap BGR to RGB for ToJPG
20100920	bcb	Fixed j2k decompressing of j2k file instead of just the stream (used by Accusoft Pegasus).
20100923	mvh	ToJPG can be used to create MPG files using external tools
20101116	bcb	Add UL, UB, UI, and UE compression to fix big endian recieve, merged, warnings
20101121	mvh	Merged
20101220	mvh	Fixed color swap for jpg and bmp; fix deplane and depalette are called when needed
20101227	mvh	Fixed 'un' in bcb 'ui' etc code in recompress 
20110106	mvh	Fix ToBMP for linux 64 bits (used long)
20110119	mvh	Use ExtendedPDU_Service to pass scripting state
20110320	mvh	Fix downsize & tojpg  when #bytes in image is odd
			Only apply RescaleIntercept for CT in tojp
20110326	mvh	Increased temp string in linux ExecHidden that caused decompress crash
20110413	mvh	Allow test of jpeg or rle decompression even if no group 0002 data
20110414	mvh	Moved jpeg decompression height errors to SystemDebug
20110419	mvh	normalized line endings; taken out superfluous prints
20110423	mvh	Fixed "UJ" broken when UI etc were introduced
20110502	mvh	always write bitstored 0x0028,0x0101 when writing highbit 0x0028,0x0102
20110904	bcb	Fixed J1 and J2 distiction
20110926	mvh	Added DcmConvertPixelData for tomono and crop
20111010	mvh	Fix in J1 and J2 distiction fix: '2' was not translated into 2 invalidating subs. tests
20111011	mvh	Fix in SaveDicomDataObject (archive): 'cannot rewrite .. in v2 format' led to a double DDO->Free
20111018	mvh	Added TempDir ini file option
20111022	mvh	Added private move command option 0x9999, 0a000 ConquestMoveOriginatorAE; let retrieveconverter work with command
20111113	mvh	Restored the original float DeYBRFULL code; the modified code did not work; read VR again, is used for deplane
20120214	mvh	Allow size==0 in DICOM to image conversion
20120327	mvh	Fix LossyQuality entry in dicom.ini for built-in JPEG compression
20120401	mvh	Fix in print (crashes when modality not passed)
20120402	mvh	Default UseBuiltInJpeg properly to 0
20120624	mvh	Set Header (0028,2110) to 01 after lossy compression
20120624	mvh	Do not modify bitsstored, highbit and numberofbits after compression
20120628	mvh	Some layout of debug prints
20120630	mvh	Reverse .dcm check (dicom if not .V2); 
                        Pass lossy quality to CompressJPEGImage from compression string
20120723	mvh	Fixed 'l' or 'l' to 'l' or 'L' found by bcb
20120918	ljz	Fixed comparison with .v2 in SaveDICOMDataObject()
20121208	mvh	Block deplane if SamplesPerPixel is not 3
20130218	mvh	JPEG compression did not set right sopclass!!!!
20130823	mvh	Fixed lossy jpeg compression for 12 bits data
20130920        ea      Corrected the calculation of new pixel spacing in MaybeDownsize
20131013	mvh	Default UseBuiltInJPEG set to 1; added quality parameter to ToJPG
20140207	mvh 	Fixed RLE (signed char 127)+1 gave overflow, now use (int)cCurrent+1
20140209	mvh 	Fix decompressjpegl here: fix lossless colorspace to RGB
20140219	mvh 	Added generic DecompressNKI (works for XDR and DCM)
20140309	mvh 	Catch error if no image VR present in jpeg2000 decompression
			linux issue on previous change
20140319	mvh 	uj and nj now pass MPEG 100, 102, 103; fix in UI; WIP support UJ outgoing
20140608	mvh 	Blocked MPEG changes for now; fix VR==NUL error handling in To8bitMonochromeOrRGB
20140608	mvh     use bitsallocated in ExtractFrame to fix philips enhanced MR
20151024	mvh     Fixed out of bounds check of cropimage
20160219	mvh     Removed defines of BOOL TRUE and FALSE
20160219	mvh     Removed JASPER and OPENJPEG here; now in Bruce's JpegConv.cpp
			Added CHARLS code; fix lossy/lossless switch; 
20160220	mvh	Removed dead code
20160221	mvh	Added JASPER code back in; openjpeg2 is still buggy
20160229	mvh	Use HAVE_LIBOPENJPEG2 define
20160316	mvh	Fix autoscale of 32 bits images
20160317	mvh	Fully implemented rescale intercept if window passed; removed uint<0 test
20160318	mvh	Limit this to slope=1
20160716	mvh	Added NKI compression mode 5
20160718	mvh	Added experimental BMPStrip function
20160723	mvh	Added experimental JpegStrip function; allow multiple columns
20160724	mvh	JpegStrip now creates proper array of images
20160905	mvh	Fix stripping of space from called and calling: crashed when empty e.g. all spaces
20170304        mvh     Fix compile without jpeg codec
20170827	mvh	No longer use IU type for transfer syntax must be UI
20170921	mvh	Added gamma on display functions
20170922	mvh	Small fix
20171009	mvh     Added commented test code for crash when c-move is canceled
20180316        mvh     Started on C-GET; in same routine as C-MOVE
20181113	mvh	Fixed unsigned signed error in RGB conversion
20181123	mvh	Changed TransferSyntaxUID back to IU (is in preamble)
20181222	mvh	Added WriteGet for C-GET client; results in image information in ADDO
20190114	mvh	Set compression to "un" for C-GET (was "" behaved as as-is)
20190602	mvh	Disable jpeg compression for <8 bits; adapt To8bitMonochromeOrRGB for other than 8 or 16 bits
20201020	mvh	0x9999,0x0b000 splits move e.g. pass 0/2 to only send even, 1/2 to only send odd
20201021	mvh	Above VR may be in command OR data; added DCO to CheckObject
20201022	mvh	Added DCO to CallImportConverterN calls; read split and slicelimit below QueryMoveScript
20201025	mvh	Print reduced number of images actually sent
20200202	mvh	Fix intermittent failure of atof(pVR->Data) when Data is e.g. " 1", it reads beyond the array
20200528        mvh     Fixed Pivoglot reported use of GetAtoi instead of GetUINT16 to read bitsperpixel
20210114        mvh     Enable compression selection for C-GET (do not force "UN") as tested by Chris
20210323        mvh     Fix leak after Host '' will not accept image 
20210325        mvh     Outgoing compression with ! e.g. JK! will not propose uncompressed
20210409        mvh     Blocked out IARQ for debugging outgoing association
20210516        mvh     Blocked out recompressed message - disturbs cgi operation; 
			Note need to accept e.g. 9999,0700 c-move/get commands in data as well as command
20220818        mvh     9999,0202 scrubbing +7FE00010,0010 keeps element/group, -7FE00010,0002 removes those,
                        -Private,7FE0 removed private and pixel data; Accept more move/get controls in query
20220819        mvh     Fixed buffer overflow and leak in scrub code
20220820        mvh     Also allow scrub on typecode e.g. -SQ,OB,OW,OF,0010,00080008
20220821        mvh     Optimized scrub a bit; do not test on unused features
20230607        mvh     Set cget back compression to 'un'; acceptedCompression not set
*/

//#define bool BOOL
//#define true TRUE
//#define false FALSE

#ifndef UNUSED_ARGUMENT
#define UNUSED_ARGUMENT(x) (void)x
#endif

#include "dicom.hpp"
#include "nkiqrsop.hpp"
#include "dgate.hpp"
#include "gpps.hpp"
#include <time.h>
#include <math.h>
#include "jpegconv.hpp"

#ifdef HAVE_LIBJPEG
#include <jpeglib.h>
#include <jerror.h>
#include <setjmp.h>
#endif

int	TCPIPTimeOut = 300;				// timeout for TCP/IP in seconds

int	NKIPrivateCompress=0;				// counters
int	NKIPrivateDecompress=0;
int	DownSizeImage=0;
int	DecompressJpeg=0;
int	CompressJpeg=0;
int	DecompressJpeg2000=0;
int	CompressJpeg2000=0;
int	DecompressedRLE=0;
int	DePlaned=0;
int	DePaletted=0;
int	RecompressTime=0;
int	CompressJpegLS=0;
int	DecompressJpegLS=0;

#define MAXNKICOMPRESSION 5

static const unsigned int CRC32_table[256] = {
      0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
      0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
      0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
      0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
      0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
      0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
      0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
      0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
      0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
      0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
      0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
      0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
      0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
      0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
      0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
      0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
      0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
      0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
      0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
      0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
      0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
      0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
      0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
      0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
      0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
      0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
      0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
      0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
      0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
      0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
      0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
      0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
      0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
      0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
      0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
      0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
      0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
      0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
      0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
      0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
      0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
      0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
      0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

#ifdef HAVE_LIBJPEG

 /* Here's the extended error handler struct for libjpeg:
    (from jpeg example.c)*/
struct _jerror_mgr 
{ 
  struct jpeg_error_mgr pub;	/* "public" fields */
  jmp_buf setjmp_buffer;	/* for return to caller */
};
typedef struct _jerror_mgr * _jerror_ptr;

typedef struct 
{ 
  struct jpeg_source_mgr pub;	/* public fields */
} _jsource_mgr;

typedef _jsource_mgr * _jsrc_ptr;
#endif

#define CLRSPC_FAM_UNKNOWN 0
#define CLRSPC_FAM_GRAYSCALE 1
#define CLRSPC_FAM_RGB 2
#define CLRSPC_FAM_YBR_FULL 3
/*Skipped numbers for Jasper */
#define CLRSPC_FAM_YCBCR_ICT 6
#define CLRSPC_FAM_YCBCR_RCT 7
#define CLRSPC_FAM_YCBCR_F422 8
#define CLRSPC_FAM_YCBCR_422 9
#define CLRSPC_FAM_YCBCR_420 10

/* Following function used in 'dgate.cpp' when executing the commandline switch
   '-fa'. This is when a dicomfile has to be stored in the dicomserver.
   The problem is that filenames generated in 'dgatefn.cpp' may be not unique
   if many files are dropped in the dicomserver. The static 'counter' intended
   to solve this problem is reset to zero each time dgate is launched.
   Now this 'counter' is initialized with a CRC of the filename.
*/
unsigned int ComputeCRC(char* pcData, int iNbChars)
{ int		i;
  unsigned int	uiCRC = 0;

  for (i=0; i<iNbChars; i++)
    uiCRC = CRC32_table[(unsigned char)uiCRC ^ (unsigned char)pcData[i]] ^ ((uiCRC >> 8) & 0x00FFFFFF);
  return uiCRC;
}

/* Read and cache UseBuiltInDecompressor and DecompressNon16BitsJpeg variable from dicom.ini 
*/

#ifndef	RootConfig
extern	char	RootConfig[];
#endif
#ifndef	ConfigFile
extern	char	ConfigFile[];
#endif

BOOL UseBuiltInDecompressor(BOOL *DecompressNon16BitsJpeg = NULL)
{ char			RootSC[64];
  char			Temp[128];
  static int		res=-1, dn16=1;

  if (res==-1)
  { MyGetPrivateProfileString ( RootConfig, "MicroPACS", RootConfig,
	  	(char*) RootSC, 64, ConfigFile);

    MyGetPrivateProfileString ( RootSC, "UseBuiltInDecompressor", "1",
		(char*) Temp, 128, ConfigFile);
    res = atoi(Temp);

    MyGetPrivateProfileString ( RootSC, "UseBuiltInJPEG", "1",
		(char*) Temp, 128, ConfigFile);
    res = atoi(Temp);

    MyGetPrivateProfileString ( RootSC, "DecompressNon16BitsJpeg", "1",
		(char*) Temp, 128, ConfigFile);
    dn16 = atoi(Temp);
  }

  if (DecompressNon16BitsJpeg) *DecompressNon16BitsJpeg = dn16;
  return(res);
}

/*******************  NKI specific Retrieve Classes *************************/
int CallImportConverterN(DICOMCommandObject *DCO, DICOMDataObject *DDO, int N, char *pszModality, char *pszStationName, char *pszSop, char *patid, ExtendedPDU_Service *PDU, char *Storage, char *Script);

StandardRetrieveNKI::StandardRetrieveNKI()
{
}

void
IARQ (AAssociateRQ	&ARQ, BOOL showall);

BOOL	StandardRetrieveNKI	::	Read (
	ExtendedPDU_Service		*PDU,
	DICOMCommandObject		*DCO,
	void				*ExtraBytes,
	BOOL				cget
	)
	{
	UID	MyUID, uid, iUID, AppUID ("1.2.840.10008.3.1.1.1");
	VR	*vr;
	DICOMDataObject	DDO;
	Array < DICOMDataObject *>	ADDO;
	UINT		Index;
	BYTE		IP [ 64 ], lPort [ 64 ], ACRNema [ 17 ], MyACR[17], Compress[64];
	StandardStorage	*SStorage;
	DICOMDataObject	*iDDO;
	ExtendedPDU_Service	NewPDU;
	UINT16		Failed;
	unsigned int iVrSliceLimit = 0xffffffff;
	int		status = 0;
	UID		TrnSyntaxUID;
//	int		ContextID;
	char		*AcceptedCompress, *Compression;
	char		Called[20], Calling[20];
	BOOL		StripGroup2, Cancelled;
	int             splitinto=1, splitselect=0;

	GetUID(MyUID);

	if( ! PDU )
		return ( FALSE );

	if( ! DCO )
		return ( FALSE );

	vr = DCO->GetVR(0x0000, 0x0002);
	SetUID(uid, vr);
	if (!(MyUID == uid))
		return ( FALSE );

	strcpy(Calling, (char *)(((AAssociateAC *)PDU)->CallingApTitle));
	if (Calling[0]>32)
	  while (Calling[strlen(Calling)-1]==' ') Calling[strlen(Calling)-1] = 0;

	if (!cget)
		{
		if (! CMoveRQ :: Read (DCO, PDU, &DDO) )
			{
			SystemDebug.printf("Retrieve: wrong command\n");
			return ( FALSE ); // my SOP, but wrong command
			}
		}
	else
		{
		if (! CGetRQ :: Read (DCO, PDU, &DDO) )
			{
			SystemDebug.printf("Get: wrong command\n");
			return ( FALSE ); // my SOP, but wrong command
			}
		}

	if (!cget)
		{
		NewPDU.AttachRTC(&VRType);
		NewPDU.SetTimeOut(TCPIPTimeOut);
		}

	if (! QueryMoveScript (PDU, DCO, &DDO))
		{
		if (!cget)
			CMoveRSP :: Write (PDU, DCO, 0xc013, 0, 0, 0, 0, NULL );
		else
			CGetRSP :: Write (PDU, DCO, 0xc013, 0, 0, 0, 0 );
		SystemDebug.printf("Retrieve: operation blocked by script\n");
		return ( TRUE );
		}

        // copy optional control codes from DDO to DCO where processed
	vr = DDO.GetVR(0x9999, 0x0200);
	if (vr) 
	{ VR *vr2 = new VR(vr->Group, vr->Element, vr->Length, TRUE);
	  memcpy(vr2->Data, vr->Data, vr->Length);
	  DCO->Push(vr2);
	}
	vr = DDO.GetVR(0x9999, 0x0201);
	if (vr) 
	{ VR *vr2 = new VR(vr->Group, vr->Element, vr->Length, TRUE);
	  memcpy(vr2->Data, vr->Data, vr->Length);
	  DCO->Push(vr2);
	}
	vr = DDO.GetVR(0x9999, 0x0202);
	if (vr) 
	{ VR *vr2 = new VR(vr->Group, vr->Element, vr->Length, TRUE);
	  memcpy(vr2->Data, vr->Data, vr->Length);
	  DCO->Push(vr2);
	}
	vr = DDO.GetVR(0x9999, 0x0700);
	if (vr) 
	{ VR *vr2 = new VR(vr->Group, vr->Element, vr->Length, TRUE);
	  memcpy(vr2->Data, vr->Data, vr->Length);
	  DCO->Push(vr2);
	}
	vr = DDO.GetVR(0x9999, 0x0900);
	if (vr) 
	{ VR *vr2 = new VR(vr->Group, vr->Element, vr->Length, TRUE);
	  memcpy(vr2->Data, vr->Data, vr->Length);
	  DCO->Push(vr2);
	}

	vr = DCO->GetVR(0x9999, 0x0500);
	if (!vr) vr = DDO.GetVR(0x9999, 0x0500);
	if (vr && vr->Length) iVrSliceLimit = vr->GetUINT();

	// optional split of move (enables downsizing or multithreaded forwarding) 
	vr = DCO->GetVR(0x9999, 0x0b00);
	if (!vr) vr = DDO.GetVR(0x9999, 0x0b00);
	if (vr)
		{ 
		char split[64];
		strncpy(split, (const char*)vr->Data, vr->Length);
		split[vr->Length] = 0;
		if (strchr(split, '/')) 
			splitinto = atoi(strchr(split, '/')+1);
		splitselect = atoi(split);
		if (splitselect>=splitinto) 
			splitselect=0;
		DDO.DeleteVR(vr);
		}

	if (!cget)
		{
		vr = DCO->GetVR(0x0000, 0x0600);
		if(!vr)
			{
			CMoveRSP :: Write ( PDU, DCO, 0xc001 , 0, 0, 0, 0, NULL );
			SystemDebug.printf("Retrieve: move destination missing\n");
			return ( TRUE );
			}

		memset((void*)ACRNema, 0, 17);
		if(vr->Length > 16)
			vr->Length = 16;
		memcpy((void*)ACRNema, vr->Data, (int) vr->Length);
		if(!vr->Length)
			{
			CMoveRSP :: Write ( PDU, DCO, 0xc002 , 0, 0, 0, 0, NULL );
			SystemDebug.printf("Retrieve: move destination empty\n");
			return ( TRUE );
			}
		if(ACRNema[vr->Length-1]==' ')
			ACRNema[vr->Length-1] = '\0';

		if(!QualifyOn(ACRNema, MyACR, IP, lPort, Compress))
			{
			CMoveRSP :: Write (PDU, DCO, 0xc003, 0, 0, 0, 0, NULL );
			SystemDebug.printf("Retrieve: move destination not allowed or known\n: %s", ACRNema);
			return ( TRUE );
			}
		}
	else
		strcpy((char *)ACRNema, "C-GET client");
		
	vr = DCO->GetVR(0x9999, 0x0a00);
	if (vr)
		{ 
		strncpy((char *)MyACR, (const char*)vr->Data, vr->Length);
		MyACR[vr->Length] = 0;
		}
	
	if (! SearchOn (&DDO, &ADDO) )//bcb, mvh: fills ADDO, must delete before exit
		{
		if (!cget)
			CMoveRSP :: Write (PDU, DCO, 0xc004, 0, 0, 0, 0, NULL );
		else
			CGetRSP :: Write (PDU, DCO, 0xc004, 0, 0, 0, 0 );
		SystemDebug.printf("Retrieve: move search failed\n");
		return ( TRUE );
		}

	if (iVrSliceLimit < 0xffffffff)
		while (ADDO.GetSize() > iVrSliceLimit)
		{
			delete ADDO.Get(ADDO.GetSize()-1);
			ADDO.RemoveAt(ADDO.GetSize()-1);
		}

	if (splitinto > 1)
		{
		int n=ADDO.GetSize()-1;
		for (int i=n; i>=0; i--)
			{
			if ((i%splitinto)!=splitselect)
				{
				delete ADDO.Get(i);
				ADDO.RemoveAt(i);
				}
			}
		}
		
	if (iVrSliceLimit < 0xffffffff || splitinto > 1)
	  OperatorConsole.printf("Reduced to: %d\n", ADDO.GetSize());
			
	// If the called AE looks like SERVER~j2, then the last 2 characters override the outgoing compression set in ACRNEMA.MAP
	strcpy(Called, (char *)(((AAssociateAC *)PDU)->CalledApTitle));
	if (Called[0]>32)
	  while (Called[strlen(Called)-1]==' ') Called[strlen(Called)-1] = 0;
        Compression = strchr(Called, '~');
	if (!Compression) Compression = (char *)Compress; else Compression++;

	// If the called AE looks like SERVER~p2, then the last character overrides the outgoing port set in ACRNEMA.MAP
        if (Compression[0]=='P' || Compression[0]=='p')			// override port
		{
		sprintf((char *)lPort, "%d", atoi((char *)lPort)+Compression[1]-'0');
		Compression= (char *)Compress;				// use default compress
		}

	if (!cget)
		{
		NewPDU.SetRequestedCompressionType((char *)Compression);	// propose compression type

		NewPDU.SetApplicationContext ( AppUID );
		NewPDU.SetLocalAddress ( MyACR );
		NewPDU.SetRemoteAddress ( ACRNema );
	
		// Add all the Abstract Syntaxs we need

		Index = 0;
		while ( Index < ADDO.GetSize() )
			{
			vr = ADDO.Get ( Index ) -> GetVR(0x0008, 0x0016);
			if(!vr)
				{
				delete ADDO.Get ( Index );
				ADDO.RemoveAt ( Index );
				}
			else
				{
				SetUID ( iUID, vr );
				NewPDU.AddAbstractSyntax ( iUID );
				++Index;
				}
			}

		NewPDU.SetTimeOut(TCPIPTimeOut);

		if (!NewPDU.Connect (IP, lPort))
			{
			CMoveRSP :: Write ( PDU, DCO, 0xc005 , 0, 0, 0, 0, NULL );
			OperatorConsole.printf("Host '%s' did not accept the connection\n", ACRNema);
			while(ADDO.GetSize())//bcb, was leak if fail?
				{
				delete ADDO.Get ( 0 );
				ADDO.RemoveAt ( 0 );
				}
			return ( TRUE );
			}
		
		//IARQ ( NewPDU, TRUE );
		}

	Index = 0;
	Failed = 0;
	Cancelled = FALSE;

	while ( Index < ADDO.GetSize() )
		{

		if (Cancelled)
			{
			++Failed;
			delete ADDO.Get(Index);
			++Index;
			continue;
			}
		else
			{
			if (PDU->Link.Poll()) 
			      {
			      OperatorConsole.printf("Recieved cancel request\n", ACRNema);
			      Cancelled = TRUE;
			      }
			}
		
		vr = ADDO.Get ( Index ) -> GetVR(0x0008, 0x0016);
		SetUID ( iUID, vr );

//		ContextID = NewPDU.GetPresentationContextID(iUID);
//		if ( !ContextID )
	
		ExtendedPDU_Service	*UsedPDU = &NewPDU;
		if (cget)
			UsedPDU = PDU;

		if ( !UsedPDU->IsAbstractSyntaxAccepted(iUID) )
			{
			++Failed;
			OperatorConsole.printf("Host '%s' will not accept image\n", ACRNema);
			// Remote end did not accept this UID

			// retrieve object from readahead to be able to free it to avoid big leak!!!
			RetrieveOn (ADDO.Get(Index), &iDDO, &SStorage, DCO, &ADDO, ExtraBytes);
			delete iDDO;
			}
		else
			{
			if (!cget) 
			  AcceptedCompress = UsedPDU->GetAcceptedCompressionType(iUID);
			else AcceptedCompress = "un";

			StripGroup2 = memicmp(AcceptedCompress, "as", 2)!=0 && memicmp(AcceptedCompress, "is", 2)!=0;

			if ( !RetrieveOn (ADDO.Get(Index), &iDDO, &SStorage, DCO, &ADDO, ExtraBytes))
				++Failed;
			else
				{
				// non NKI client
			        if (DCO==NULL)
					recompress(&iDDO, AcceptedCompress, "", StripGroup2, PDU);

				// NKI client has 2 vr's to control (re)compression
				// 9999,0600=MaxSupportedCompression (old, nki only)
				// 9999,0700=RequestedCompression (new, nki or jpeg)
			        else
					{
					char mode[16];
					int MaxCompression=5;
					char script[1024];
					script[0]=0;

					// compression from transfer syntax negotiation
					strcpy(mode, AcceptedCompress);

					// optional requested custom compression
					vr = DCO->GetVR(0x9999, 0x0700);
						if (vr)
						{ 
						strncpy(mode, (const char*)vr->Data, vr->Length);
						mode[vr->Length] = 0;
						}

					// optional requested custom script
					vr = DCO->GetVR(0x9999, 0x0900);
						if (vr)
						{ 
						strncpy(script, (const char*)vr->Data, vr->Length);
						script[vr->Length] = 0;
						}

					vr = DCO->GetVR(0x9999, 0x0600);
					if (vr && vr->Length)MaxCompression = vr->GetUINT();
					if (MaxCompression > 5) MaxCompression = 5;

					// limit nki compression to MaxCompression
					if ((mode[0]=='n' || mode[0]=='N') && (mode[1]!='j' && mode[1]!='J'))
						if (atoi(mode+1) > MaxCompression)
							mode[1] = MaxCompression + '0';

					recompress(&iDDO, mode, "", StripGroup2, PDU);
					if (script[0])
						CallImportConverterN(DCO, iDDO, -1, NULL, NULL, NULL, NULL, PDU, NULL, script);
					}

// test code to simulate crash in c-move when canceled 20171009
//if (Index==100) 
//{ UsedPDU->Link.Close();return (FALSE);}

				if(!SStorage->Write(UsedPDU, iDDO, DCO->GetVR(0x0000, 0x0110), (unsigned char *)Calling))
					{
					//++Failed;
					// Remote end should accept this image.  if it did
					// not, then just bail out.  Probably means the
					// TCP/IP link has been dropped.
					Failed += (ADDO.GetSize() - Index);

					SystemDebug.printf("Retrieve: remote connection dropped after %d images, %d not sent\n", Index, Failed);

					/* LJ: This also happens when CqDicom has had 'enough'
					   e.g. when retrieving properties. It caused a small
					   leak
					*/
					status = 0xfe00;				// mvh 20020429: return cancel status
					delete iDDO;
					break;
					}
				delete iDDO;
				}
			}

//if (Index==100) 
//{ /*PDU->Link.Close();*/ return(FALSE); }

		if (!cget)
			CMoveRSP :: Write ( PDU,
					DCO,
					0xff00, ADDO.GetSize() - Index - 1,
					(UINT16) Index+1, Failed, 0,			// mvh 20020429: added +1
					ADDO.Get ( Index ));
		else
			CGetRSP :: Write ( PDU,
					DCO,
					0xff00, ADDO.GetSize() - Index - 1,
					(UINT16) Index+1, Failed, 0);


		delete ADDO.Get(Index);

		++Index;
		}


	if (!cget)
		CMoveRSP :: Write ( PDU, DCO, status, 0, (UINT16) Index, Failed, 0, NULL );	// mvh 20020429: replaced 0 by status
	else
		CGetRSP :: Write ( PDU, DCO, status, 0, (UINT16) Index, Failed, 0 );

	// In case we broke out from above..

	while ( Index < ADDO.GetSize() )
		{
		delete ADDO.Get(Index);
		++Index;
		}

	
	return ( TRUE );
	}

class	MyUnknownGet	:	public UnknownStorage
	{
	public:
				// called for each incoming DDO
		UINT16 CheckObject(DICOMCommandObject *DCO, DICOMDataObject *DDO, PDU_Service *PDU) 
			{
			recompress(&DDO, "UN", "", FALSE, (ExtendedPDU_Service *)PDU);
			return 0;
			}
	};


BOOL	StandardRetrieveNKI	::	Write (
	PDU_Service		*PDU,
	DICOMDataObject		*DDO,
	BYTE			*ACRNema )
	{
	DICOMCommandObject	*DCO;
	DICOMDataObject		*RDDO;

	if ( ! PDU )
		return ( FALSE );

	if ( ! CMoveRQ :: Write ( PDU, DDO, ACRNema ) )
		return ( FALSE );

	CallBack ( NULL, DDO );

	DCO = new DICOMCommandObject;

	while ( PDU->Read ( DCO ) )
		{
		RDDO = new DICOMDataObject;

		if (! (CMoveRSP :: Read ( DCO, PDU, RDDO) ) )
			{
			return ( FALSE );
			}
		if ( DCO->GetUINT16(0x0000, 0x0800) == 0x0101)
			{
			CallBack ( DCO, NULL );
			delete RDDO;
			if ( DCO->GetUINT16(0x0000, 0x0900) != 0x0000)
				{
				VR *vr;
				while ((vr = DCO->Pop()))
					{
					delete vr;
					}
				delete DCO;
				return ( FALSE );
				}
			delete DCO;
			return ( TRUE );
			}
		CallBack ( DCO, RDDO );
		delete RDDO;
		delete DCO;
		DCO = new DICOMCommandObject;
		}

	delete DCO;
	return ( FALSE );
	}

BOOL	StandardRetrieveNKI	::	WriteGet (
	PDU_Service		*PDU,
	DICOMDataObject		*DDO,
	Array < DICOMDataObject  *> *pADDO )
	{
	DICOMCommandObject	*DCO;
	DICOMDataObject		*RDDO;
	MyUnknownGet		SOPUnknownGet;

	if ( ! PDU )
		return ( FALSE );

	if ( ! CGetRQ :: Write ( PDU, DDO) )
		return ( FALSE );

	CallBack ( NULL, DDO );

	DCO = new DICOMCommandObject;

	while ( PDU->Read ( DCO ) )
		{
		RDDO = new DICOMDataObject;

		if ( (CGetRSP :: Read ( DCO, PDU, RDDO) ) )
			{
  			if ( DCO->GetUINT16(0x0000, 0x0900) != 0xFF00)
				{
				CallBack ( DCO, NULL );
				delete RDDO;
				if ( DCO->GetUINT16(0x0000, 0x0900) != 0x0000)
					{
					VR *vr;
					while ((vr = DCO->Pop()))
						{
						delete vr;
						}
					delete DCO;
					return ( FALSE );
					}
				delete DCO;
				return ( TRUE );
				}
			delete RDDO;
			}
		else if(SOPUnknownGet.Read(PDU, DCO, RDDO))
			{
			pADDO->Add(RDDO);
			}
		else
			{
			delete RDDO;
			return ( FALSE );
			}

		CallBack ( DCO, RDDO );
		delete DCO;
		DCO = new DICOMCommandObject;
		}

	delete DCO;
	return ( FALSE );
	}

BOOL	PatientRootRetrieveNKI	::	GetUID ( UID &uid )
	{
	uid.Set ( "1.2.826.0.1.3680043.2.135.1066.5.1.4.1.2.1.2" );
	return ( TRUE );
	}

BOOL	StudyRootRetrieveNKI	::	GetUID ( UID &uid )
	{
	uid.Set ( "1.2.826.0.1.3680043.2.135.1066.5.1.4.1.2.2.2" );
	return ( TRUE );
	}

BOOL	PatientStudyOnlyRetrieveNKI	::	GetUID ( UID &uid )
	{
	uid.Set ( "1.2.826.0.1.3680043.2.135.1066.5.1.4.1.2.3.2" );
	return ( TRUE );
	}

BOOL	PatientRootRetrieveGeneric	::	GetUID ( UID &uid )
	{
	uid.Set ( "1.2.840.10008.5.1.4.1.2.1.2" );
	return ( TRUE );
	}

BOOL	StudyRootRetrieveGeneric	::	GetUID ( UID &uid )
	{
	uid.Set ( "1.2.840.10008.5.1.4.1.2.2.2" );
	return ( TRUE );
	}

BOOL	PatientStudyOnlyRetrieveGeneric	::	GetUID ( UID &uid )
	{
	uid.Set ( "1.2.840.10008.5.1.4.1.2.3.2" );
	return ( TRUE );
	}

BOOL	PatientRootGetGeneric	::	GetUID ( UID &uid )
	{
	uid.Set ( "1.2.840.10008.5.1.4.1.2.1.3" );
	return ( TRUE );
	}

BOOL	StudyRootGetGeneric	::	GetUID ( UID &uid )
	{
	uid.Set ( "1.2.840.10008.5.1.4.1.2.2.3" );
	return ( TRUE );
	}

BOOL	PatientStudyOnlyGetGeneric	::	GetUID ( UID &uid )
	{
	uid.Set ( "1.2.840.10008.5.1.4.1.2.3.3" );
	return ( TRUE );
	}

/************ NKI specific processing of a loaded image *************************/

typedef struct
{
  unsigned int iOrgSize;	/* NOTE: this is the number of short pixels !!!! */
  unsigned int iMode;
  unsigned int iCompressedSize;	/* in bytes, not pixels */
  unsigned int iOrgCRC;
  unsigned int iCompressedCRC;	/* Excluding this header */
} NKI_MODE2;


/* coder for NKI private compressed pixel data VR 0x7fdf,0x0010
   arguments: dest    = (in) points to area where compressed destination data is written (byte)
              src     = (in) points to uncompressed source data (short)
              npixels = (in) number of pixels to compress
              iMode   = (in) type of compression (1 .. 4)

   The return value is the number of bytes in the compressed data (maximal 3*npixels+10, typical 0.52*npixels)

   if iMode == 1 then
   - The first 4 bytes contain the number of short-int pixels
   - The following 4 bytes contain iMode=1
   - The rest is the compressed image

   if iMode == 2 then
   - The first 4 bytes contain the number of short-int pixels
   - The following 4 bytes contain iMode=2
   - The following 4 bytes contain the size of the compressed image (in bytes)
   - The following 4 bytes contain the CRC of the original image
   - The following 4 bytes contain the CRC of the compressed image
   - The rest is the compressed image
   - The compressed size will be even (padded by a zero if necessary).

   if iMode == 3 then
   - The first 4 bytes contain the number of short-int pixels
   - The following 4 bytes contain iMode=3
   - The rest is the compressed image, including 4 bit differences

   if iMode == 4 then
   - The first 4 bytes contain the number of short-int pixels
   - The following 4 bytes contain iMode=4
   - The following 4 bytes contain the size of the compressed image (in bytes)
   - The following 4 bytes contain the CRC of the original image
   - The following 4 bytes contain 0
   - The rest is the compressed image, including 4 bit differences
   - The compressed size will be even (padded by a zero if necessary).

   if iMode == 5 then
   - The first 4 bytes contain the number of short-int pixels
   - The following 4 bytes contain iMode=5
   - The following 4 bytes contain the size of the compressed image (in bytes)
   - The following 4 bytes contain 0
   - The following 4 bytes contain 0
   - The rest is the compressed image, where 0..31 stores direct 13 bit value with 
   - next byte, 224-255 stores run of 2-33 long, anything else stores diff+128 
   iMode 5 only compresses the lower 13 bits of the image. The rest will be set to 0

*/

// optimized settings for the 4 bit run compressor (mode 3 and 4)

#define MINZEROS 5		// shortest RLE (2 byte overhead, but breaks 4bit run)
#define MIN4BIT  6		// shortest 4 bit run (6 bytes compressed to 5 bytes)

// This internal routine converts an 8 bit difference string into a 4 bit one
static signed char *recompress4bit(int n, signed char *dest)
{ signed char *p, *q;
  int val;

  n = n & 0xfe;
  dest -= n;
  p = dest;
  val = (((int)p[0])<<4) | (p[1]&15);
  p += 2;
  *dest++ = 0xc0;
  *dest++ = n;
  q = dest++;
  n -= 2;
  while(n>0)
  { *dest++ = (((int)p[0])<<4) | (p[1]&15);
    p += 2;
    n -= 2;
  }
  q[0] = val;

  return dest;
}

int nki_private_compress(signed char  *dest, short int  *src, int npixels, int iMode)
{ unsigned int		iCRC;
  unsigned int		iCRC2;
  register int		val, i, j;
  NKI_MODE2*		pHeader = (NKI_MODE2*)dest;

  NKIPrivateCompress++;

  /* Up till now only Mode=1 .. 5 are supported */
  if ((iMode < 1) || (iMode > 5))
    return 0;

  /* Create the header */
  pHeader->iOrgSize = npixels;
  pHeader->iMode = iMode;
  switch (iMode)
  { case 1:
      dest += 8;
      break;
    case 2:
      dest += sizeof(NKI_MODE2);
      break;
    case 3:
      dest += 8;
      break;
    case 4:
    case 5:
      dest += sizeof(NKI_MODE2);
      break;
  }

  /* Create the compressed image */

  if (iMode == 1)
  { *(short int *)dest = *src; 
    dest+=2;

    npixels--;

    do
    { val = src[1] - src[0];
      src++;
  
      if (val == 0)                            /* run length-encode zero differences */
      { for (i=2;; i++)
        { if (i>=npixels || src[i-1]!=src[-1] || i==256)
          { if (i==2) 
              *dest++=0;
            else      
            { *dest++  =  0x80;
              *dest++  = (i-1);
              npixels -= (i-2);
              src     += (i-2);
            }
            break;
          }
        }
      }    
      else if (val >= -64 && val <= 63)         /* small difference coded as one byte */
      { *dest = val;
        dest++;
      }
      else if (val >= -0x3F00 && val <= 0x3EFF) /* large differences coded as two bytes */
      { dest[0] = (val>>8) ^ 0x40;
        dest[1] = val;
        dest+=2;
      }
      else                                      /* if very large differences code abs val as three bytes */
      { *dest++ = 0x7F;
        *dest++ = src[0]>>8; 
        *dest++ = src[0];
      }
    }
    while (--npixels);
  }

  else if (iMode == 2)
  { iCRC  = 0;
    iCRC2 = 0;

    *(short int *)dest = val = *src; 
    iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char) val    ] ^ ((iCRC2 >> 8));
    iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
    iCRC  = CRC32_table[(unsigned char)iCRC  ^ (unsigned char) val    ] ^ ((iCRC  >> 8));
    iCRC  = CRC32_table[(unsigned char)iCRC  ^ (unsigned char)(val>>8)] ^ ((iCRC  >> 8));
    dest+=2;
    npixels--;

    do
    { val = src[1] - src[0];
      src++;
      iCRC  = CRC32_table[(unsigned char)iCRC  ^ (unsigned char) src[0]    ] ^ ((iCRC  >> 8));
      iCRC  = CRC32_table[(unsigned char)iCRC  ^ (unsigned char)(src[0]>>8)] ^ ((iCRC  >> 8));

      if (val == 0)                            /* run length-encode zero differences */
      { for (i=2;; i++)
        { if (i>=npixels || src[i-1]!=src[-1] || i==256)
          { if (i==2) 
            { *dest++=0;
              iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ 0    ] ^ ((iCRC2 >> 8));
            }
            else      
            { *dest++  =  0x80;
              iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ 0x80 ] ^ ((iCRC2 >> 8));
              *dest++  = (i-1);
              iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (i-1)] ^ ((iCRC2 >> 8));
              npixels -= (i-2);

              for (j=0; j<i-2; j++)
              { src++;
                iCRC = CRC32_table[(unsigned char)iCRC  ^ (unsigned char) src[0]    ] ^ ((iCRC  >> 8));
                iCRC = CRC32_table[(unsigned char)iCRC  ^ (unsigned char)(src[0]>>8)] ^ ((iCRC  >> 8));
              }
            }
            break;
          }
        }
      }    
      else if (val >= -64 && val <= 63)         /* small difference coded as one byte */
      { *dest = val;
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val     ] ^ ((iCRC2 >> 8));
        dest++;
      }
      else if (val >= -0x3F00 && val <= 0x3EFF) /* large differences coded as two bytes */
      { dest[0] = (val>>8) ^ 0x40;
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)dest[0] ] ^ ((iCRC2 >> 8));
        dest[1] = val;
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val     ] ^ ((iCRC2 >> 8));
        dest+=2;
      }
      else                                      /* if very large differences code abs val as three bytes */
      { dest[0] = 0x7F;
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ 0x7f                   ] ^ ((iCRC2 >> 8));
	val     = src[0];
        dest[1] = val>>8; 
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
        dest[2] = val;
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val     ] ^ ((iCRC2 >> 8));
	dest+=3;
      }
    }
    while (--npixels);

    pHeader->iCompressedSize = dest - (signed char*)pHeader - sizeof(NKI_MODE2);

    /* Pad it to get an even length */
    if (pHeader->iCompressedSize & 1)
    { *dest++ = 0;
      iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ 0] ^ ((iCRC2 >> 8));
      pHeader->iCompressedSize++;
    }

    pHeader->iOrgCRC        = iCRC;
    pHeader->iCompressedCRC = iCRC2;
  }

  /* Create the compressed image - compressor with added 4 bit run */
  
  else if (iMode == 3)
  { int n4bit=0;
    *(short int *)dest = *src; 
    dest+=2;
    npixels--;

    do
    { val = src[1] - src[0];
      src++;
  
      if (val == 0)                            	/* run length-encode zero differences */
      { for (i=2;; i++)
        { if (i>=npixels || src[i-1]!=src[-1] || i==256)
          { if (i<=MINZEROS) 		       	/* too short run -> write zeros */
            { for (j=0; j<i-1; j++)
              { *dest++=0;
                n4bit++;

                if(n4bit>=254)			/* maximum length 4 bit run */
                { dest  = recompress4bit(n4bit, dest);
                  n4bit = 0;
                }
              }
            }
            else      
            { if (n4bit>=MIN4BIT)		/* end (and write) 4 bit run */
                dest  = recompress4bit(n4bit, dest);

              n4bit=0;
              *dest++  =  0x80;
              *dest++  = (i-1);
            }

            npixels -= (i-2);
            src     += (i-2);
            break;
          }
        }
      }    
      else if (val >= -63 && val <= 63)         /* small difference coded as one byte */
      { if (val >= -8 && val <= 7)
        { *dest++ = val;
          n4bit++;

          if(n4bit>=254)			/* maximum length 4 bit run */
          { dest  = recompress4bit(n4bit, dest);
            n4bit=0;
          }
        }
        else if(n4bit>=MIN4BIT)			/* end and write 4 bit run */
        { j = val;
          dest  = recompress4bit(n4bit, dest);
          n4bit=0;
          *dest++ = j;
        }
        else
        { *dest++ = val;			/* end 4 bit run */
           n4bit  = 0;
        }
      }
      else if (val >= -0x3F00 && val <= 0x3EFF) /* large differences coded as two bytes */
      { j = val;

        if(n4bit>=MIN4BIT)			/* end (and write) 4 bit run */
          dest  = recompress4bit(n4bit, dest);

        n4bit=0;
        dest[0] = (j>>8) ^ 0x40;
        dest[1] = j;
        dest+=2;
      }
      else                                      /* if very large differences code abs val as three bytes */
      { j = src[0];

        if(n4bit>=MIN4BIT)			/* end (and write) 4 bit run */
          dest  = recompress4bit(n4bit, dest);

        n4bit=0;
        *dest++ = 0x7F;
        *dest++ = j>>8; 
        *dest++ = j;
      }
    }
    while (--npixels);
  }

  /* Create the compressed image - compressor with added 4 bit run and CRC */
  
  else if (iMode == 4)
  { int n4bit=0;
    iCRC  = 0;
    
    *(short int *)dest = val = *src; 
    iCRC  = CRC32_table[(unsigned char)iCRC  ^ (unsigned char) val    ] ^ ((iCRC  >> 8));
    iCRC  = CRC32_table[(unsigned char)iCRC  ^ (unsigned char)(val>>8)] ^ ((iCRC  >> 8));
    dest+=2;
    npixels--;

    do
    { val = src[1] - src[0];
      src++;
      iCRC  = CRC32_table[(unsigned char)iCRC  ^ (unsigned char) src[0]    ] ^ ((iCRC  >> 8));
      iCRC  = CRC32_table[(unsigned char)iCRC  ^ (unsigned char)(src[0]>>8)] ^ ((iCRC  >> 8));
  
      if (val == 0)                            	/* run length-encode zero differences */
      { for (i=2;; i++)
        { if (i>=npixels || src[i-1]!=src[-1] || i==256)
          { if (i<=MINZEROS) 		       	/* too short run -> write zeros */
            { for (j=0; j<i-1; j++)
              { *dest++=0;
                n4bit++;

                if(n4bit>=254)			/* maximum length 4 bit run */
                { dest  = recompress4bit(n4bit, dest);
                  n4bit = 0;
                }
              }
            }
            else      
            { if (n4bit>=MIN4BIT)		/* end (and write) 4 bit run */
                dest  = recompress4bit(n4bit, dest);

              n4bit=0;
              *dest++  =  0x80;
              *dest++  = (i-1);
            }

            npixels -= (i-2);
            for (j=0; j<i-2; j++)
            { src++;
              iCRC = CRC32_table[(unsigned char)iCRC  ^ (unsigned char) src[0]    ] ^ ((iCRC  >> 8));
              iCRC = CRC32_table[(unsigned char)iCRC  ^ (unsigned char)(src[0]>>8)] ^ ((iCRC  >> 8));
            }
            break;
          }
        }
      }    
      else if (val >= -63 && val <= 63)         /* small difference coded as one byte */
      { if (val >= -8 && val <= 7)
        { *dest++ = val;
          n4bit++;

          if(n4bit>=254)			/* maximum length 4 bit run */
          { dest  = recompress4bit(n4bit, dest);
            n4bit=0;
          }
        }
        else if(n4bit>=MIN4BIT)			/* end and write 4 bit run */
        { j = val;
          dest  = recompress4bit(n4bit, dest);
          n4bit=0;
          *dest++ = j;
        }
        else
        { *dest++ = val;			/* end 4 bit run */
           n4bit  = 0;
        }
      }
      else if (val >= -0x3F00 && val <= 0x3EFF) /* large differences coded as two bytes */
      { j = val;

        if(n4bit>=MIN4BIT)			/* end (and write) 4 bit run */
          dest  = recompress4bit(n4bit, dest);

        n4bit=0;
        dest[0] = (j>>8) ^ 0x40;
        dest[1] = j;
        dest+=2;
      }
      else                                      /* if very large differences code abs val as three bytes */
      { j = src[0];

        if(n4bit>=MIN4BIT)			/* end (and write) 4 bit run */
          dest  = recompress4bit(n4bit, dest);

        n4bit=0;
        *dest++ = 0x7F;
        *dest++ = j>>8; 
        *dest++ = j;
      }
    }
    while (--npixels);

    pHeader->iCompressedSize = dest - (signed char*)pHeader - sizeof(NKI_MODE2);

    /* Pad it to get an even length */
    if (pHeader->iCompressedSize & 1)
    { *dest++ = 0;
      pHeader->iCompressedSize++;
    }

    pHeader->iOrgCRC        = iCRC;
    pHeader->iCompressedCRC = 0;
  }

  else if (iMode == 5)				/* supersimple and fast but keeps only 13 bits */
  { ((unsigned char *)dest)[1] = ((unsigned char *)src)[0];
    ((unsigned char *)dest)[0] = ((unsigned char *)src)[1] & 31;
    dest+=2;
    npixels--;

    do
    { val = (src[1]&0x1fff) - (src[0]&0x1fff);
      src++;

      if (val == 0)                             /* run length-encode 2..33 zero differences */
      { for (i=2;; i++)
        { if (i>=npixels || src[i-1]!=src[-1] || i==33)
          { *(unsigned char*)dest++  = 224+(i-1)-1;
            npixels -= (i-2);
            src     += (i-2);
            break;
          }
        }
      }
      else if (val >= -96 && val <= 95)         /* small difference coded as one byte */
      { *(unsigned char*)dest++ = val + 128;
      }
      else					/* directly store 13 bits value */
      { ((unsigned char *)dest)[1] = ((unsigned char *)src)[0];
        ((unsigned char *)dest)[0] = ((unsigned char *)src)[1] & 31;
        dest+=2;  
      }

      /* Are we beyond the allocated memory? */
      //if ((size_t)(dest - (signed char*)pHeader) > iBufferSize)
	//return 0;
    }
    while (--npixels);

    pHeader->iCompressedSize =
      (unsigned int)(dest - (signed char*)pHeader - sizeof(NKI_MODE2));
    pHeader->iOrgCRC        = 0;
    pHeader->iCompressedCRC = 0;
  }

  return dest - (signed char*)pHeader;
}

int get_nki_private_decompressed_length(signed char *src)
{ int npixels = *(int *)src;
  return 2*npixels;
}

int get_nki_private_compress_mode(signed char *src)
{ int mode = *(int *)(src+4);
  return mode;
}

int get_nki_private_compressed_length(signed char *src)
{ int nchar = *(int *)(src+8);
  return nchar;
}

/* decoder for NKI private compressed pixel data - faster and safe version
   arguments: dest    = (in) points to area where destination data is written (short)
              src     = (in) points compressed source data (byte stream)
              size    = (in) number of bytes source data (safety)

   The return value is the number of pixels that have been processed.
 
   The compressed data looks like:
   (number of pixels)-1 times:
     OR 1 byte   = LL     7  bits signed (difference pixel[1] - pixel[0]);
     OR 2 bytes  = HHLL   15 bits signed (difference pixel[1] - pixel[0]) xored with 0x4000;
     OR 3 bytes  = 7FHHLL 16 bits absolute pixel data if 15 bits difference is exceeded
     OR 2 bytes  = 80NN   run length encode NN zero differences (max 255)
for mode 3 and 4 added:
     OR 2 bytes  = CONN   encode NN 4 bit differences (max 255)

   Performance on typical CT or MRI is >2x compression and a very good speed

   This code is not valid on HIGHENDIAN (high byte first) machines
*/

int nki_private_decompress(short int *dest, signed char *src, int size)
{ int			npixels, retvalue, mode, iMode, val, j;
  NKI_MODE2*		pHeader = (NKI_MODE2*)src;
  unsigned int		iCRC=0, iCRC2=0;
  signed char           *save, *end;

  NKIPrivateDecompress++;

  retvalue = npixels = pHeader->iOrgSize;
  iMode = pHeader->iMode;		// safety: this value is checked in case statement

  if (npixels<1) return 0;		// safety: check for invalid npixels value

  /* Up till now only Mode=1, 2, 3, and 4 are supported */

  switch (iMode)
  { case 1:
      src += 8;				// mode 1 only has 8 bytes header: iOrgSize and iMode
      end  = src + size - 3;		// for overflow check if we are close to end of input buffer
  
      *dest = *(short int *)src;
      src += 2;
      npixels--;

      do
      { if (src > end) 			// check whether the last few messages fit in input buffer
        { if (src<end+3) val = *src;
          else           val = 0;

          if (val >= -64 && val <= 63)      mode = 1;	// 7 bit difference
          else if (val==0x7f)		    mode = 3;	// 16 bit value
          else if ((val&0xff)==0x80)	    mode = 2;	// run length encoding
          else				    mode = 2;

          if (src+mode > end+3) 
            return 0;			// safety: overflow input data
        }
    
        val = *src;

        if (val >= -64 && val <= 63)	// 7 bit difference
        { dest[1] = dest[0] + val;
          dest++;
          src++;
        }
        else if (val==0x7f)		// 16 bit value
        { dest[1] = val = ((int)(((unsigned char *)src)[1])<<8) + ((unsigned char*)src)[2];
          dest++;
          src+=3;
        }
        else if ((val&0xff)==0x80)	// run length encoding
        { mode = ((unsigned char *)src)[1];
          npixels -= mode-1;
          if (npixels<=0) return 0;	// safety: overflow output data
          do 
          { dest[1] = dest[0];
            dest++;
          }
          while (--mode);
          src+=2;
        }
        else
        { signed short diff = ((val^0x40)<<8) + (unsigned char)(src[1]);
          dest[1] = dest[0] + diff;	// 15 bit difference
          dest++;
          src+=2;
        }
      }
      while (--npixels);

      break;

    case 2:
      src += sizeof(NKI_MODE2);
      save = src;
      end  = src + pHeader->iCompressedSize - 3;

      if (end > src + size - 3) 
        end = src + size - 3;		// may occur if pHeader is corrupted

      *dest = val = *(short int *)src;
      iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
      iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
      src+=2;
    
      npixels--;
    
      do
      { if (src > end) 			// check whether the last few messages fit in input buffer
        { if (src<end+3) val = *src;
          else           val = 0;

          if (val >= -64 && val <= 63)      mode = 1;	// 7 bit difference
          else if (val==0x7f)		    mode = 3;	// 16 bit value
          else if ((val&0xff)==0x80)	    mode = 2;	// run length encoding
          else				    mode = 2;

          if (src+mode > end+3) 
            break;			// safety: overflow input data
        }

        val = *src;
    
        if (val >= -64 && val <= 63)	// 7 bits difference
        { dest[1] = val = dest[0] + val;
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
          dest++;
          src++;
        }
        else if (val==0x7f)		// 16 bit value
        { dest[1] = val = ((int)(((unsigned char *)src)[1])<<8) + ((unsigned char*)src)[2];
    
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
          dest++;
          src+=3;
        }
        else if ((val&0xff)==0x80)	// run length encoding
        { mode = ((unsigned char *)src)[1];
          npixels -= mode-1;
          if (npixels<=0) break;	// safety: overflow output data
          do 
          { dest[1] = val = dest[0];
            iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
            iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
            dest++;
          }
          while (--mode);
          src+=2;
        }
        else
        { signed short diff = ((val^0x40)<<8) + ((unsigned char *)src)[1];
          dest[1] = val = dest[0] + diff;	// 15 bit difference
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
          dest++;
          src+=2;
        }
      }
      while (--npixels);
    
      if (iCRC2 != pHeader->iOrgCRC)	// if error in output CRC:
      { src = save;			// check input CRC
        while (src < end)
        { iCRC = CRC32_table[(unsigned char)iCRC ^ (unsigned char)src[0]] ^ ((iCRC >> 8));
          src++;
        }

        if (iCRC != pHeader->iCompressedCRC)
        { //OperatorConsole.printf("NKI decompression: the file is corrupted\n");
          retvalue=0;
        }
        else
        { //OperatorConsole.printf("NKI private decompression: internal error\n"); 
          retvalue=0; 
        }
      }

      break;

    case 3:
      src += 8;				// mode 3 only has 8 bytes header: iOrgSize and iMode
      end  = src + size - 3;		// for overflow check if we are close to end of input buffer
  
      *dest = *(short int *)src;
      src += 2;
      npixels--;

      do
      { if (src > end) 			// check whether the last few messages fit in input buffer
        { if (src<end+3) val = *src;
          else           val = 0;

          if (val >= -63 && val <= 63)      mode = 1;	// 7 bit difference
          else if (val==0x7f)		    mode = 3;	// 16 bit value
          else if ((val&0xff)==0x80)	    mode = 2;	// run length encoding
          else if ((val&0xff)==0xC0)	    mode = 2;	// 4 bit encoding
          else				    mode = 2;

          if (src+mode > end+3) 
            return 0;			// safety: overflow input data
        }
    
        val = *src;

        if (val >= -63 && val <= 63)	// 7 bit difference
        { dest[1] = dest[0] + val;
          dest++;
          src++;
        }
        else if (val==0x7f)		// 16 bit value
        { dest[1] = val = ((int)(((unsigned char *)src)[1])<<8) + ((unsigned char*)src)[2];
          dest++;
          src+=3;
        }
        else if ((val&0xff)==0x80)	// run length encoding
        { mode = ((unsigned char *)src)[1];
          npixels -= mode-1;
          if (npixels<=0) return 0;	// safety: overflow output data
          do 
          { dest[1] = dest[0];
            dest++;
          }
          while (--mode);
          src+=2;
        }
        else if ((val&0xff)==0xC0)	// 4 bit run
        { mode = ((unsigned char *)src)[1];
          npixels -= mode-1;
          mode/=2;
          src+=2;
          if (npixels<=0) return 0;	// safety: overflow output data
          do 
          { val = *src++;
            dest[1] = dest[0] + (val>>4);
            dest++;
            if (val&8) val |= 0xfffffff0; else val &= 0x0f;
            dest[1] = dest[0] + val;
            dest++;
          }
          while (--mode);
        }
        else
        { signed short diff = ((val^0x40)<<8) + (unsigned char)(src[1]);
          dest[1] = dest[0] + diff;	// 15 bit difference
          dest++;
          src+=2;
        }
      }
      while (--npixels);

      break;

    case 4:
      src += sizeof(NKI_MODE2);
      save = src;
      end  = src + pHeader->iCompressedSize - 3;
  
      if (end > src + size - 3) 
        end = src + size - 3;		// may occur if pHeader is corrupted

      *dest = val = *(short int *)src;
      iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
      iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
      src += 2;
      npixels--;

      do
      { if (src > end) 			// check whether the last few messages fit in input buffer
        { if (src<end+3) val = *src;
          else           val = 0;

          if (val >= -63 && val <= 63)      mode = 1;	// 7 bit difference
          else if (val==0x7f)		    mode = 3;	// 16 bit value
          else if ((val&0xff)==0x80)	    mode = 2;	// run length encoding
          else if ((val&0xff)==0xC0)	    mode = 2;	// 4 bit encoding
          else				    mode = 2;

          if (src+mode > end+3) 
            return 0;			// safety: overflow input data
        }
    
        val = *src;

        if (val >= -63 && val <= 63)	// 7 bit difference
        { dest[1] = val = dest[0] + val;
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
          dest++;
          src++;
        }
        else if (val==0x7f)		// 16 bit value
        { dest[1] = val = ((int)(((unsigned char *)src)[1])<<8) + ((unsigned char*)src)[2];
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
          dest++;
          src+=3;
        }
        else if ((val&0xff)==0x80)	// run length encoding
        { mode = ((unsigned char *)src)[1];
          npixels -= mode-1;
          if (npixels<=0) return 0;	// safety: overflow output data
          do 
          { dest[1] = val = dest[0];
            iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
            iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
            dest++;
          }
          while (--mode);
          src+=2;
        }
        else if ((val&0xff)==0xC0)	// 4 bit run
        { mode = ((unsigned char *)src)[1];
          npixels -= mode-1;
          mode/=2;
          src+=2;
          if (npixels<=0) return 0;	// safety: overflow output data
          do 
          { val = *src++;
            dest[1] = j = dest[0] + (val>>4);
            iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)j] ^ ((iCRC2 >> 8));
            iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(j>>8)] ^ ((iCRC2 >> 8));
            dest++;
            if (val&8) val |= 0xfffffff0; else val &= 0x0f;
            dest[1] = j = dest[0] + val;
            iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)j] ^ ((iCRC2 >> 8));
            iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(j>>8)] ^ ((iCRC2 >> 8));
            dest++;
          }
          while (--mode);
        }
        else
        { signed short diff = ((val^0x40)<<8) + (unsigned char)(src[1]);
          dest[1] = val = dest[0] + diff;	// 15 bit difference
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
          dest++;
          src+=2;
        }
      }
      while (--npixels);

      if (iCRC2 != pHeader->iOrgCRC)	// if error in output CRC:
       retvalue=0;

      break;

    case 5:
      src += sizeof(NKI_MODE2);
      save = src;
      end  = src + pHeader->iCompressedSize - 2;

      ((unsigned char *)dest)[0] = ((unsigned char *)src)[1];
      ((unsigned char *)dest)[1] = ((unsigned char *)src)[0];
      src += 2;
      npixels--;

      do
      { if (src > end) 			// check whether the last few messages fit in input buffer
        { if (src<end+2) val = *(unsigned char*)src;
          else           val = 128;

          if (val >= 32 && val <= 223)      mode = 1;	// 7.5 bit difference
          else if (val>=224) 	            mode = 1;	// run length encoding
          else 				    mode = 2;   // direct 13 bit value

          if (src+mode > end+2)
            return 0;			// safety: overflow input data
        }

        val = *(unsigned char*)src;

        if (val >= 32 && val <= 223)	// 7 bit difference
        { dest[1] = dest[0] + val - 128;
          dest++;
          src++;
        }
        else if (val>=224)		// run length encoding 1..33 times same pixel
        { mode = (val-224)+1;
          npixels -= mode-1;
          if (npixels<=0) return 0;	// safety: overflow output data
          do
          { dest[1] = dest[0];
            dest++;
          }
          while (--mode);
          src++;
        }
        else				// direct 13 bits data
        { ((unsigned char *)dest)[2] = ((unsigned char *)src)[1];
          ((unsigned char *)dest)[3] = ((unsigned char *)src)[0];
	  dest++;
          src+=2;
        }
      }
      while (--npixels);

      break;

   default:
      //OperatorConsole.printf("NKI private decompression: unsupported mode\n"); 
      return 0;
  }

  return retvalue;
}

BOOL DecompressNKI(DICOMDataObject* pDDO)
{ VR*		pVR;
  VR*		pNewVR;
  signed char*	pCompressed;
  unsigned int	iDecompressedLength, iCompressedLength;

  pVR = pDDO->GetVR(0x7fdf, 0x0010);
  if (pVR)
  { /* Are the 'normal' pixel-data also present? If so, then 'our' tag is not
       made by us!
    */
    pNewVR = pDDO->GetVR(0x7fe0, 0x0010);
    if (pNewVR)
      return TRUE;

    /* Acuson and Elscint use this tag: test whether we or they made it */
    if ((pVR->Length > 6) &&
        ((strncmp((char*)pVR->Data, "ACUSON", 6) == 0) ||
         (strncmp((char*)pVR->Data, "ELSCIN", 6) == 0)))
      return TRUE;

    /* no data in our tag (always starts with 2 dwords): cannot be compresssed */
    if (pVR->Length<=8)
      return TRUE;

    /* check compression level: if invalid it cannot be our data */
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
    if (pVR->Length>8 && ((unsigned int*)pVR->Data)[1]>MAXNKICOMPRESSION)
#else //Big Endian like Apple power pc
    if (pVR->Length>8 && SwitchEndian(((unsigned int*)pVR->Data)[1])>MAXNKICOMPRESSION)
#endif
      return TRUE;

    /* Decompress it */
    pCompressed         = ((signed char *)(pVR->Data));
    if (!pCompressed)
      return FALSE;
    
    iCompressedLength   = pVR->Length;
    iDecompressedLength = get_nki_private_decompressed_length(pCompressed);
    if (!iDecompressedLength)
      return FALSE;

    pNewVR = new VR(0x7fe0, 0x0010, iDecompressedLength, TRUE);
    if (!pNewVR) 
      return FALSE;

    if (!nki_private_decompress((short *)(pNewVR->Data), pCompressed, iCompressedLength))
    { delete pNewVR;
      return FALSE;
    }

    /* Push the new VR, and delete the old one */
    pDDO->Push(pNewVR);
    pDDO->DeleteVR(pVR);
  }
  return TRUE;
}

void SaveDICOMDataObject(char *Filename, DICOMDataObject* pDDO);

BOOL DecompressNKI(char *file_in, char *file_out)
{ FILE *f = fopen(file_in, "rb");
  fseek(f, 0, SEEK_END);
  int fileLength = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buffer = (char *)malloc(fileLength);
  if (!buffer)
  { fclose(f);
    return FALSE;
  }
  fread(buffer, 1, fileLength, f);
  fclose(f);
  if (fileLength>20)
  { if (memcmp(buffer, "# AVS", 5)==0)
    { int start;
      for (int i=0; i<fileLength; i++)
      if (buffer[i]==0x0c)
      { buffer[i]=0;
        char *p = strstr(buffer, "nki_compression=");
	if (p) 
	{ if (atoi(p+16)==0) 
	    return FALSE;
	  p[16]='0';
	}
        int iDecompressedLength = get_nki_private_decompressed_length((signed char *)buffer+i+2);
        int iCompressedLength = get_nki_private_compressed_length((signed char *)buffer+i+2)+20;
        if (!iDecompressedLength)
          return FALSE;
	char *buffer2 = (char *)malloc(iDecompressedLength);
        if (!buffer2)
          return FALSE;
	int InLen = nki_private_decompress((short *)buffer2, (signed char *)buffer+i+2, fileLength-i-2);
	if (InLen==0)
          return FALSE;
        FILE *g = fopen(file_out, "wb");
	fwrite(buffer, i, 1, g);
	fputc(12, g);
	fputc(12, g);
	swab(buffer2, buffer2, iDecompressedLength);
	fwrite(buffer2, iDecompressedLength, 1, g);
	printf("remaining: %d; InLen: %d; FileLen=%d; iDecompressedLength=%d", 
	      fileLength-i-2-iCompressedLength, iCompressedLength, fileLength, iDecompressedLength);
	fwrite(buffer+i+2+iCompressedLength, fileLength-i-2-iCompressedLength, 1, g);
	fclose(f);
	free(buffer);
	free(buffer2);
        return TRUE;
      }
    }
    else
    { free(buffer);
      DICOMDataObject		*pDDO;
      PDU_Service		PDU;
      PDU.AttachRTC(&VRType);
      pDDO = PDU.LoadDICOMDataObject(file_in);
      if(!pDDO)
	return FALSE;
      int Changed;
      if(!DecompressImage(&pDDO, &Changed)) return FALSE;
      SaveDICOMDataObject(file_out, pDDO);
      return TRUE;
    }
  }
  return FALSE;
}

BOOL CompressNKI(DICOMDataObject* pDDO, int CompressMode /* = 2 */)
{ VR*		pVR;
  VR*		pNewVR;
  signed char*	pCompressed;
  unsigned int	iCompressedLength;

  /* First check whether the image already contains 0x7fdf, 0x0010.
     If so, compression was already done, or 'our' VR is occupied.
     Also do not compress the 'ugly' variable-length sequence of pixeldata.
  */
  pVR = pDDO->GetVR(0x7fdf, 0x0010);
  if (pVR)
    return TRUE;

  /* Pixeldata present? */
  pVR = pDDO->GetVR(0x7fe0, 0x0010);
  if (pVR && (pVR->Length != 0xffffffff) && (pVR->Length != 0x0))
  { /* Compress it */
    pCompressed = (signed char*) malloc((pVR->Length/2) * 3 + 10);
    if (!pCompressed)
      return FALSE;

    iCompressedLength = nki_private_compress(pCompressed, (short int *)(pVR->Data), pVR->Length/2, CompressMode);
    if (iCompressedLength & 0x01)
      pCompressed[iCompressedLength++] = 0;

    /* Create  a new VR, and delete the old one */
    pNewVR = new VR(0x7fdf, 0x0010, iCompressedLength, pCompressed);
    pNewVR->TypeCode = pVR->TypeCode;
    pDDO->Push(pNewVR);
    pDDO->DeleteVR(pVR);
  }
  return TRUE;
}

/* Get a frame from a multi-frame object */

BOOL ExtractFrame(DICOMDataObject* pDDO, unsigned int Frame)
{// int		rc;
  unsigned int iNbFrames=1;
  VR		*pVR, *vrs;
  Array < DICOMDataObject *> *pADDO;

  iNbFrames = pDDO->Getatoi(0x0028, 0x0008);
  if (iNbFrames<=1 || Frame>=iNbFrames) return FALSE;

  DecompressNKI(pDDO);
  
  /* PixelData */
  pVR = pDDO->GetVR(0x7fe0, 0x0010);
  if (!pVR) return FALSE;

  pADDO = (Array<DICOMDataObject*>*)pVR->SQObjectArray;
  if (pADDO==NULL)
  { unsigned int size = pDDO->GetUINT(0x0028, 0x0010) * pDDO->GetUINT(0x0028, 0x0011) *
		pDDO->GetUINT(0x0028, 0x0002) * pDDO->GetUINT(0x0028, 0x0100/*1*/) / 8; // Assumes 8 bits byte
    // note: uses bitstored instead of bitsallocated; no problem in practice; 20140608: use bitsallocated 28,100 NOW

    if (size * iNbFrames > pVR->Length) return FALSE;
    
// repackage it crudeley but efficiently (the VR will be deleted shortly after anyway)
    memcpy(pVR->Data, ((BYTE *)pVR->Data) + size*Frame, size);
    pVR->Length = size;
  }
  else
  { if (Frame>=pADDO->GetSize()-1) return FALSE;
    Array < DICOMDataObject * > *SQE = new Array < DICOMDataObject * >;

    DICOMDataObject *D = MakeCopy(pADDO->Get(0));	// basic offset table
    SQE->Add(D);

    vrs = D->GetVR(0xfffe, 0xe000);    	// basic offset table (may be zero length)
    if (vrs->Length) vrs->Length = 4;  	// keep offset to first frame: will now be valid

    D = MakeCopy(pADDO->Get(Frame+1));	// selected frame
    SQE->Add(D);
					// repackage it
    pVR->Reset();// Say goodbye to old data.
    pVR->SQObjectArray = (void*) SQE;// Connect it to the new array.
    // No need to push, already in the dicom object
  }

  /* replace number of frames with 1 */
  pVR = pDDO->GetVR(0x0028, 0x0008);
  if (pVR)
  { pVR->ReAlloc(2);
    memcpy(pVR->Data, "1 ", 2);
  }

  return TRUE;
}

int GetNumberOfFrames(DICOMDataObject* pDDO)
{ int iNbFrames = pDDO->Getatoi(0x0028, 0x0008);
  return iNbFrames;
}

/* Crop frames from a multi-frame object */

BOOL ExtractFrames(DICOMDataObject* pDDO, unsigned int FirstFrame, unsigned int LastFrame, int skip)
{ //int		rc;
  unsigned int	iNbFrames=1, tFrames;
  VR		*pVR, *vrs;
  Array < DICOMDataObject *> *pADDO;

  iNbFrames = pDDO->Getatoi(0x0028, 0x0008);
  if (iNbFrames<=1 || FirstFrame>=iNbFrames) return FALSE;
  if (LastFrame>=iNbFrames) LastFrame = iNbFrames-1;
  if (skip<1) skip = 1;
	
  tFrames = (LastFrame - FirstFrame + 1)/skip;// How many do we need?
  if (tFrames>iNbFrames) return FALSE;// More than we have?
  if (tFrames<1) tFrames=1;// Must do at least 1.

  int count=0;

  DecompressNKI(pDDO);

  /* PixelData */
  pVR = pDDO->GetVR(0x7fe0, 0x0010);
  if (!pVR) return FALSE;

  pADDO = (Array<DICOMDataObject*>*)pVR->SQObjectArray;
  if (pADDO==NULL)
  { unsigned int size = pDDO->GetUINT(0x0028, 0x0010) * pDDO->GetUINT(0x0028, 0x0011) *
	       pDDO->GetUINT(0x0028, 0x0002) * pDDO->GetUINT(0x0028, 0x0101) / 8;
    // note: uses bitstored instead of bitsallocated; no problem in practice

#if 1
  // repackage it crudeley but efficiently (the object will soon be sent and destroyed anyway)
    while (FirstFrame <= LastFrame)
    { if (size * FirstFrame > pVR->Length) return FALSE;
      memcpy(((BYTE *)pVR->Data) + count*size, ((BYTE *)pVR->Data) + FirstFrame*size, size);
      FirstFrame += skip;
      count ++;
    }
    pVR->Length = size * count;
#else
    pVR->Length = size * count;*/
    unsigned int tSize = tFrames * size; // How big in the end.
    if (tSize & 01) tSize++;// No odd data!
    if (!(nData = (BYTE *)malloc(tSize))) return FALSE;// our new correctly allocated memory.
    nData[tSize - 1] = 0;//Doesn't hurt.
    while (FirstFrame <= LastFrame)
    { if (size * FirstFrame > tSize)
      {
        free(nData);
        return FALSE;
      }
      memcpy( nData + (count * size), ((BYTE *)pVR->Data) + (FirstFrame * size), size);
      FirstFrame += skip;
      count ++;
    }
    pVR->Reset();// Say goodbye to old data.
    pVR->Data = (void*)nData;// Give it the new data.
    pVR->Length = size;// Tell it how long.
    pVR->ReleaseMemory = TRUE;// Make it responsible for it
#endif
  }
  else
  { if (FirstFrame>=pADDO->GetSize()-1) return FALSE;

    Array < DICOMDataObject * > *SQE = new Array < DICOMDataObject * >;

    DICOMDataObject *D = MakeCopy(pADDO->Get(0));	// basic offset table
    SQE->Add(D);

    vrs = D->GetVR(0xfffe, 0xe000);    	// basic offset table (may be zero length)
    if (vrs->Length) vrs->Length = 0;  	// lose offsets (easier)

    while (FirstFrame <= LastFrame)
    { if (FirstFrame>=pADDO->GetSize()-1) return FALSE;
      D = MakeCopy(pADDO->Get(FirstFrame+1));	// selected frame
      SQE->Add(D);
      FirstFrame += skip;
    }

    // repackage it
    pVR->Reset();// Say goodbye to old data.
    pVR->SQObjectArray = (void*) SQE;// Connect it to the new array.
    // No need to push, already in the dicom object

    count = SQE->GetSize()-1;
  }

// replace number of frames with new count.
  pVR = pDDO->GetVR(0x0028, 0x0008);
/* I have large number of frames, may not work > 99 frames in, < 10 out. bcb
  sprintf((char *)pVR->Data, "%d", count);// trailing 0 overwrite?
  if (((char *)pVR->Data)[pVR->Length-1]==0) ((char *)pVR->Data)[pVR->Length-1] = ' ';
  mvh bcb correct but replaced by something less ugly...
*/
  if (pVR)
  { char str[20];
    sprintf(str, "%d", count);
    pVR->ReAlloc(strlen(str));
    memcpy(pVR->Data, (BYTE *)str, pVR->Length);
    if (((char *)pVR->Data)[pVR->Length-1]==0) ((char *)pVR->Data)[pVR->Length-1] = ' ';
  }

  pVR = new VR(0x200d, 0x3016, 0, (void *)NULL, FALSE);
  if (pVR) pDDO->DeleteVR(pVR);			// (temp) remove philips private pixel data

  return TRUE;
}

/* Downsizing of pixeldata only when VR (0x9999, 0x0200) is present in the
   DicomCommandObject. when VR (0x9999, 0x0201) is present, a single frame is extracted
   NOTE: these VR's are NKI specific.
   Note: changing a VR length leaves some memory unused, but will be freed soon after anyway
*/

int MaybeDownsize(DICOMDataObject* pDDO, DICOMCommandObject* pDCO, int size)
{ int			i, j;
  VR			*pVR1=NULL, *pVR2=NULL;
  unsigned int		iMaxRowsColumns=0;
  unsigned short	iRows, iColumns, iNewRows, iNewColumns, iDownsizeFactor;
  char			s[256];
  char*			pSeparator;
  float			fSpacingX, fSpacingY;
  unsigned int		iImageSize, iNbBytesPerPixel, iNbPixels, iSkip;
  char*			pcSrc;
  char*			pcDest;
  short*		psSrc;
  short*		psDest;
  int*			piSrc;
  int*			piDest;

  if (pDCO)
  { pVR1 = pDCO->GetVR(0x9999, 0x0200);	/* MaxRowsColumns */
    pVR2 = pDCO->GetVR(0x9999, 0x0201);	/* Frame */
  }
  else
    iMaxRowsColumns = size;

  if (pVR1) iMaxRowsColumns = pVR1->GetUINT();

  if (pVR2) ExtractFrame(pDDO, pVR2->GetUINT());

  if (!iMaxRowsColumns)
    return TRUE;			/* Downsizing to zero doesn't make sense */

  /* NumberOfFrames */
  if(pDDO->Getatoi(0x0028, 0x0008) > 1) return FALSE;/* not implemented for multi-frame */

  /* OK. Now check whether all necessary PixelData-info is present in pDDO */
  iRows    = pDDO->GetUINT16(0x0028, 0x0010);
  iColumns = pDDO->GetUINT16(0x0028, 0x0011);
  iNbPixels = iRows * iColumns;

  if ((!iNbPixels) ||				/* Image must make sense */
      ((iRows <= iMaxRowsColumns) &&
       (iColumns <= iMaxRowsColumns)))		/* Image is already 'small' enough */
    return TRUE;

  DownSizeImage++;

  if (iRows >= iColumns)
  { iDownsizeFactor = iRows / iMaxRowsColumns;
    if (iRows % iMaxRowsColumns)
      iDownsizeFactor ++;
  }
  else
  { iDownsizeFactor = iColumns / iMaxRowsColumns;
    if (iColumns % iMaxRowsColumns)
      iDownsizeFactor ++;
  }

  pVR1 = pDDO->GetVR(0x0028, 0x0030);	/* Pixelspacing */
  if (!pVR1)
    return TRUE;
  strncpy(s, (const char*)pVR1->Data, pVR1->Length);
  s[pVR1->Length] = 0;
  fSpacingX = (float)(atof(s));
  pSeparator = strchr(s, '\\');
  if (pSeparator)
    fSpacingY = (float)(atof(pSeparator + 1));
  else
    fSpacingY = fSpacingX;
  
  if (!DecompressNKI(pDDO))		/* Pixeldata must first be decompressed */
    return FALSE;

  pVR1 = pDDO->GetVR(0x7fe0, 0x0010);	/* Pixeldata */
  if (!pVR1)
    return TRUE;
  iImageSize = pVR1->Length;
  if ((iImageSize < iNbPixels) ||
      (iImageSize % iNbPixels > 1))	/* allow 1 byte padding */
    return TRUE;			/* Image doesn't make sense */
  iNbBytesPerPixel = iImageSize / iNbPixels;
  if (iNbBytesPerPixel > 4)
    return TRUE;			/* Image is too beautiful to downsize! */

  /* LETS DO IT !
     The downsized image is stored in the same memory-block as the original one:
     Just be sure to adjust the length-indicator. The image will be (re)compressed anyway
     later on.
  */

  iNewRows    = iRows / iDownsizeFactor;
  if (!iNewRows)
    iNewRows++;
  iNewColumns = iColumns / iDownsizeFactor;
  if (!iNewColumns)
    iNewColumns++;
  switch (iNbBytesPerPixel)
  { case 1:
      pcSrc  = (char*)pVR1->Data;
      pcDest = pcSrc;
      for (i=0; i<iNewRows; i++)
      { for (j=0; j<iNewColumns; j++)
	{ *pcDest++ = *pcSrc;
	  pcSrc += iDownsizeFactor;
	}
	pcSrc = (char*)pVR1->Data + iDownsizeFactor * iColumns * (i + 1);
      }
      break;
    case 2:
      psSrc  = (short*)pVR1->Data;
      psDest = psSrc;
      for (i=0; i<iNewRows; i++)
      { for (j=0; j<iNewColumns; j++)
	{ *psDest++ = *psSrc;
	  psSrc += iDownsizeFactor;
	}
	psSrc = (short*)pVR1->Data + iDownsizeFactor * iColumns * (i + 1);
      }
      break;
    case 3:
      pcSrc  = (char*)pVR1->Data;
      pcDest = pcSrc;
      iSkip = (iDownsizeFactor - 1) * 3;
      for (i=0; i<iNewRows; i++)
      { for (j=0; j<iNewColumns; j++)
	{ *pcDest++ = *pcSrc++;
	  *pcDest++ = *pcSrc++;
	  *pcDest++ = *pcSrc++;
	  pcSrc += iSkip;
	}
	pcSrc = (char*)pVR1->Data + iDownsizeFactor * 3 * iColumns * (i + 1);
      }
      break;
    case 4:
      piSrc  = (int*)pVR1->Data;
      piDest = piSrc;
      for (i=0; i<iNewRows; i++)
      { for (j=0; j<iNewColumns; j++)
	{ *piDest++ = *piSrc;
	  piSrc += iDownsizeFactor;
	}
	piSrc = (int*)pVR1->Data + iDownsizeFactor * iColumns * (i + 1);
      }
      break;
  }

  /* FINALLY: adjust Rows, Columns, Pixelspacing and (the length of) PixelData */
  pDDO->ChangeVR(0x0028, 0x0010, (UINT16)iNewRows, 'US');
  pDDO->ChangeVR(0x0028, 0x0011, (UINT16)iNewColumns, 'US');

  if (pSeparator)
    sprintf(s, "%g\\%g",  (iColumns*fSpacingY)/iNewColumns, (iRows*fSpacingX)/iNewRows);
  else
    sprintf(s, "%g", (iRows*fSpacingX)/iNewRows);
  pDDO->ChangeVR(0x0028, 0x0030, s, 'DS', TRUE);

  pVR1 = pDDO->GetVR(0x7fe0, 0x0010);	// Pixeldata
  pVR1->Length = iNewRows * iNewColumns * iNbBytesPerPixel;
  return 2;			// Downsizing actually occurred
}

/* Test if downsizing will occur */

static int TestDownsize(DICOMDataObject* pDDO, DICOMCommandObject* pDCO, int size)
{// VR*			pVR;
  unsigned int		iMaxRowsColumns;
  unsigned short	iRows, iColumns;
  int			iNbPixels;

  if (pDCO)
  { iMaxRowsColumns = pDCO->GetUINT(0x9999, 0x0200);

    if (!iMaxRowsColumns)
      return FALSE;			/* Downsizing to zero doesn't make sense */
  }
  else
    iMaxRowsColumns = size;

  /* OK. Now check whether all necessary PixelData-info is present in pDDO */
  iRows =  pDDO->GetUINT16(0x0028, 0x0010);

  iColumns = pDDO->GetUINT16(0x0028, 0x0011);

  iNbPixels = iRows * iColumns;

  if ((!iNbPixels) ||				/* Image must make sense */
      ((iRows <= iMaxRowsColumns) &&
       (iColumns <= iMaxRowsColumns)))		/* Image is already 'small' enough */
    return FALSE;

  return TRUE;				/* image would be downsized */
}

// scrub VR items using 9999,0202; use e.g. +00100010,0020000D to keep only those two; use e.g. -00100010,0020000D to delete those two
// also accepts e.g. -0002 to delete/keep whole group
int MaybeScrub(DICOMDataObject* pDDO, DICOMCommandObject* pDCO)
{ 	DICOMObject	DO2;
	VR		*vr;
	char		item[12];
	char		*list;
	BOOL		priv=false;
	char		s[256];

	vr = pDCO->GetVR(0x9999, 0x0202);
	if (vr==NULL) return false;

	list = (char *)malloc(vr->Length+6);
	list[0]=((char *)(vr->Data))[0];
	list[1]=',';
	memcpy(list+2, ((char *)(vr->Data))+1, vr->Length-1);
	list[vr->Length+1]=0;
	if (list[vr->Length]==' ') list[vr->Length]=',';
	else strcat(list, ",");
	SystemDebug.printf("Scrubbing: %s\n", list);
	
	if (strstr(list, "priv")||strstr(list, "PRIV")||strstr(list, "Priv")) 
		priv=true;

	BOOL has2=false, has4=false, has8=false;
	int n=-1;
	for (int i=0; i<strlen(list); i++)
	{ if (list[i]==',')
	  { if (n>=0) 
	    { if (i-n==9) has8=true;
	      if (i-n==5) has4=true;
	      if (i-n==3) has2=true;
	    }
            n=i;
	  }
	}

	while((vr=pDDO->Pop()))
		{
		char *p=NULL, *q=NULL, *r=NULL;
		if (has8)
		{ sprintf(item, ",%04X%04X,", vr->Group, vr->Element);
		  p=strstr(list, item);
		}
		if (has4)
		{ sprintf(item, ",%04X,", vr->Group);
		  q=strstr(list, item);
		}
		if (has2)
		{ s[0] = '\0';
		  int TypeCode = VRType.RunTimeClass(vr->Group, vr->Element, s);
		  sprintf(item, ",%c%c,", TypeCode>>8, TypeCode&0x00ff);
		  r=strstr(list, item);
		}

		BOOL odd=false;
		if (priv && (vr->Group&1)) odd=true;

	  	if ((list[0]=='-' && (p||q||r||odd)) || (list[0]=='+' && !(p||q||r||odd)))
		  delete vr;
		else 
		  DO2.Push(vr);
		}

	pDDO->Reset();
	while((vr=DO2.Pop())) pDDO->Push(vr);
	free(list);
	return true;
}


BOOL ProcessDDO(DICOMDataObject** pDDO, DICOMCommandObject* pDCO, ExtendedPDU_Service *PDU)
{ VR  *pVR;
  int dum;
  
  if (pDCO)				// nki clients
  { /* pDCO possibly contains specifications of how the NKI clients
       want the data the be sent/processed. Only downsize is 
       implemented for now.
    */

    MaybeScrub(*pDDO, pDCO);                // filter VRs

    pVR = pDCO->GetVR(0x9999, 0x0200);	// MaxRowsColumns
    if (pVR)
    { if (TestDownsize(*pDDO, pDCO, 0))
      { DecompressImage(pDDO, &dum);	// will also decompress JPEG images

	if (!MaybeDownsize(*pDDO, pDCO, 0))  // downsize
          return FALSE;
      }
    }
  }

  // De-compression is left to recompress in StandardRetrieveNKI::Read 
  // but run RetrieveResultConverter if present

  char dest[20];
  memset(dest, 0, 20);
  VR *vr;
  if (pDCO) 
  { vr = pDCO->GetVR(0x0000, 0x0600);
    if (vr) memcpy(dest, (char *)(vr->Data), vr->Length);
  }
  while (strlen(dest)>0 && dest[strlen(dest)-1]==' ') dest[strlen(dest)-1] = 0;

//  CallImportConverterN(*pDDO, 1300, NULL, dest, NULL, NULL, "calling", "called", NULL, NULL, VariableVRs);
  CallImportConverterN(pDCO, *pDDO, 1300, NULL, dest, NULL, NULL, PDU, NULL, NULL);
  return TRUE;
}

/*************************************************************************************************/

static void RgbToMono(unsigned char* pSrc, unsigned char* pDest, int iNbPixels)
{ int	i;

  for (i=0; i<iNbPixels; i++)
    pDest[i] = (pSrc[3*i] + pSrc[3*i+1] + pSrc[3*i+2] + 1)/3;
}

static bool CropImage(unsigned char* pSrc, unsigned char* pDest,
	int iSizeX, int iSizeY, int iPixelSize,
	int iStartX, int iEndX, int iStartY, int iEndY)
{ int	i, iSize;

  if ((iStartX>=iSizeX-1) || (iEndX>=iSizeX-1) || (iStartX>=iEndX) || (iStartX < 0))
    return false;
  if ((iStartY>=iSizeY-1) || (iEndX>=iSizeY-1) || (iStartX>=iEndY) || (iStartY < 0))
    return false;

  pSrc  = pSrc + (iStartY*iSizeX + iStartX) * iPixelSize;
  iSize = (iEndX - iStartX + 1) * iPixelSize;
  for (i=iStartY; i<=iEndY; i++)
  { memmove(pDest, pSrc, iSize);
    pSrc  += iSizeX * iPixelSize;
    pDest += iSize;
  }
  return true;
}

#define UNDEFINED_FLOAT	0

void SetStringVR(VR **vr, int g, int e, const char *String);

int DcmConvertPixelData(DICOMDataObject*pDDO, bool bConvertToMono, bool	bCrop,
  			int iStartX, int iEndX, int iStartY, int iEndY,
  			float fPixelSizeX, float fPixelSizeY, float fPixelSizeZ)
{ int			i;
  VR*			pVR;
  char			s[100];
  int			iSizeX, iSizeY, iBitsAllocated;
  int			iNbSamplesPerPixel = 1, iNbFrames = 1;
  int			iSrcSliceSize, iDestSliceSize;
  char			*pSrc, *pDest, *pNextSrc;
  char*			pszGridFrameOffsetVector;

  /* Anything to do? */
  if ((bConvertToMono == false) && (bCrop == false))
    return FALSE;

  if (!pDDO)
    return FALSE;

  /* Get the slice-size */
  pVR = pDDO->GetVR(0x0028,0x0010);		// Rows
  if (pVR)
  { iSizeY = *((short*)pVR->Data);
    pVR = pDDO->GetVR(0x0028,0x0011);		// Columns
    if (pVR)
    { iSizeX = *((short*)pVR->Data);
      pVR = pDDO->GetVR(0x0028,0x0100);		// BitsAllocated
      if (pVR)
      { iBitsAllocated = *((short*)pVR->Data);
	pVR = pDDO->GetVR(0x0028,0x0002);	// SamplesPerPixel
	if (pVR)
	  iNbSamplesPerPixel = *((short*)pVR->Data);
      }
    }
  }
  if (!pVR)
  { return FALSE;
  }
  iSrcSliceSize = iDestSliceSize = iSizeX * iSizeY * iNbSamplesPerPixel * iBitsAllocated / 8;
  if (bCrop)
    iDestSliceSize = (iEndX - iStartX + 1) * (iEndY - iStartY + 1) * iNbSamplesPerPixel * iBitsAllocated / 8;
  if (bConvertToMono)
    iDestSliceSize /= 3;

  /* Check whether ConvertToMonochrome is possible */
  if (bConvertToMono)
  { if ((iNbSamplesPerPixel != 3) || (iBitsAllocated != 8))
    { return FALSE;
    }
    pVR = pDDO->GetVR(0x0028,0x0004);		// PhotometricInterpretation
    if (!pVR)
    { return FALSE;
    }
    if (strncmp((char*)pVR->Data, "RGB", 3) != 0)
    { return FALSE;
    }
    iNbSamplesPerPixel = 1;
  }

  /* Get the pixels */
  pVR = pDDO->GetVR(0x7fe0,0x0010);		// PixelData
  if (!pVR)
  { return FALSE;
  }
  pSrc = (char*)pVR->Data;

  /* Maybe more than one frame */
  pVR = pDDO->GetVR(0x0028,0x0008);		// NumberOfFrames
  if (pVR)
  { strncpy(s, (char*)pVR->Data, pVR->Length);
    s[pVR->Length] = 0;
    iNbFrames = atoi(s);
  }

  /* The main loop */
  pDest = pSrc;
  for (i=0; i<iNbFrames; i++)
  { pNextSrc = pSrc;

    if (bConvertToMono)
    { RgbToMono((unsigned char*)pNextSrc, (unsigned char*)pDest,
	iSizeX * iSizeY);
      pNextSrc = pDest;
    }

    if (bCrop)
    { if (!CropImage((unsigned char*)pNextSrc, (unsigned char*)pDest,
	iSizeX, iSizeY, iNbSamplesPerPixel * iBitsAllocated / 8,
	iStartX, iEndX, iStartY, iEndY))
      { return FALSE;
      }
      pNextSrc = pDest;
    }

    pSrc  += iSrcSliceSize;
    pDest += iDestSliceSize;
  }


  /* Fix the affected VRs */
  pVR = pDDO->GetVR(0x7fe0,0x0010);		// PixelData
  pVR->Length = iNbFrames * iDestSliceSize;
  if (pVR->Length & 0x01) pVR->Length ++;

  if (bCrop)
  { pVR = pDDO->GetVR(0x0028,0x0010);		// Rows
    *(short*)pVR->Data = (iEndY - iStartY + 1);
    pVR = pDDO->GetVR(0x0028,0x0011);		// Columns
    *(short*)pVR->Data = (iEndX - iStartX + 1);
  }

  if (bConvertToMono)
  { pVR = pDDO->GetVR(0x0028,0x0002);		// SamplesPerPixel
    *(short*)pVR->Data = 1;
    pVR = pDDO->GetVR(0x0028,0x0004);		// PhotometricInterpretation
    free(pVR->Data);
    pVR->Data = strdup("MONOCHROME2 ");
    pVR->Length = 12;
  }

  if ((fPixelSizeX != UNDEFINED_FLOAT) && (fPixelSizeY != UNDEFINED_FLOAT))
  { sprintf(s, "%f\\%f", fPixelSizeX, fPixelSizeY);
    // pVR = NewStringVR(0x0028,0x0030, s);	// PixelSpacing
    SetStringVR(&pVR, 0x0008, 0x0030, s); 
    if (pDDO->GetVR(0x0028,0x0030))
      pDDO->ReplaceVR(pVR);
    else
      pDDO->Push(pVR);
  }
  if ((fPixelSizeZ != UNDEFINED_FLOAT) && (iNbFrames > 1))
  { pszGridFrameOffsetVector = (char*)malloc(50000);
    strcpy(pszGridFrameOffsetVector, "0");
    for (i=1; i<iNbFrames; i++)
      sprintf(pszGridFrameOffsetVector + strlen(pszGridFrameOffsetVector), "\\%f", i*fPixelSizeZ);
    // pVR = NewStringVR(0x3004,0x000c, pszGridFrameOffsetVector);	// GridFrameOffsetVector
    SetStringVR(&pVR, 0x3004, 0x000c, pszGridFrameOffsetVector); 
    if (pDDO->GetVR(0x3004,0x000c))
      pDDO->ReplaceVR(pVR);
    else
      pDDO->Push(pVR);
  }

  return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// JPEG and NKI utility compression code
///////////////////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>

// Runs external executable hidden
static BOOL ExecHidden(
	const char	*ProcessBinary,					// name of executable
	const char	*Arg1, const char *Arg2, const char *Arg3, const char *Arg4)	// arguments
	{
	PROCESS_INFORMATION	PINFO;
	STARTUPINFO		SINFO;
	SECURITY_ATTRIBUTES	SA;
	HANDLE			hMap;
	char			CommandLine[1024];

	memset((void*)&SINFO, 0, sizeof(STARTUPINFO));
	SINFO.cb = sizeof(STARTUPINFO);
	SINFO.wShowWindow = SW_HIDE;

	SA.nLength = sizeof (SECURITY_ATTRIBUTES);
	SA.lpSecurityDescriptor = NULL;

	sprintf(CommandLine, "%s %s \"%s\" \"%s\" %s", ProcessBinary, Arg1, Arg2, Arg3, Arg4);
	if(!CreateProcess(ProcessBinary, CommandLine, NULL, NULL, FALSE,
		DETACHED_PROCESS, NULL /* "DCMDICTPATH=offis.dic\0\0" */, NULL, &SINFO, &PINFO))
		{
		OperatorConsole.printf("***Failed to create process: %s - error %d\n", ProcessBinary, GetLastError());
		return ( FALSE );
		}
	else
		{
	  	WaitForSingleObject(PINFO.hProcess, INFINITE);
		CloseHandle(PINFO.hThread);
		CloseHandle(PINFO.hProcess);
		}

	return ( TRUE );
	}
#else

#ifndef DARWIN
#include <wait.h>
#endif //DARWIN

static BOOL ExecHidden(
	const char	*ProcessBinary,					// name of executable
	const char	*Arg1, const char *Arg2, const char *Arg3, const char *Env)	// arguments
	{
        char line[1024];
        sprintf(line, "export %s;%s %s \"%s\" \"%s\"", Env, ProcessBinary, Arg1, Arg2, Arg3);
        system(line);
        return TRUE;
	}
#endif

// Replaces PDU.SaveDICOMDataObject; automatically detects transfer syntax to use
void SaveDICOMDataObject(char *Filename, DICOMDataObject* pDDO)
	{
	int len, has_dcm_extension=0, UseChapter10=FALSE, bForcedImplicit=FALSE;
	VR *pVR;
	char s[256];
	PDU_Service PDU;

	PDU.AttachRTC(&VRType);

	len = strlen(Filename);
	if (len>3)
		has_dcm_extension = (stricmp(Filename+len-3, ".v2")!=0);

	/* Type of save depends on file name extension is .dcm (UseChapter10) 
	   and on the transfer syntax (not ImplicitLittleEndian, not
	   ExplicitLittleEndian and not ExplicitBigEndian)
	*/
	pVR = pDDO->GetVR(0x0002, 0x0010);	// TransferSyntaxUID
	if (pVR && pVR->Data)
		{
		strncpy(s, (char*)pVR->Data, pVR->Length);
		s[pVR->Length] = 0;
		if ((strcmp(s, "1.2.840.10008.1.2")   != 0) &&
		    (strcmp(s, "1.2.840.10008.1.2.1") != 0) &&
		    (strcmp(s, "1.2.840.10008.1.2.2") != 0))
			{
			if (!has_dcm_extension)
				{
				OperatorConsole.printf("Recompress: cannot rewrite jpeg/rle image in v2 format\n");
				Filename[0]=0;
				pDDO->Reset();
				return;
				}
			UseChapter10 = has_dcm_extension;
			}
		else
			{
			UseChapter10 = has_dcm_extension;
			//bForcedImplicit = TRUE; // 20040406
			}
		}
	else
		UseChapter10 = has_dcm_extension;

	if (!UseChapter10)
		PDU.SaveDICOMDataObject(Filename, ACRNEMA_VR_DUMP, pDDO);
	else if (bForcedImplicit)
	 	PDU.SaveDICOMDataObject(Filename, DICOM_CHAPTER_10_IMPLICIT, pDDO);
	else
	 	PDU.SaveDICOMDataObject(Filename, DICOM_CHAPTER_10_EXPLICIT, pDDO);
	}


/***********************************************************************************/
/*
   Code from CqGlobal.h		(CqDicom.dll)
*/
/***********************************************************************************/

#define DCM_E_OK				0
#define DCM_E_MEMORY				5
#define DCM_E_UNSUPPORTED_COMPRESSION		26
#define DCM_E_UNSUPPORTED_FORMAT		28

typedef struct
{
	char	szModality[16];
	char	szSeriesInstanceUID[256];
	float	fSpacingX;
	float	fSpacingY;
	int	iDimX;
	int	iDimY;
	int	iNbFrames;
	int	iNbTimeSequences;
	char	szPatientPosition[16];
	int	iDataSize;
	BOOL	bNkiCompressed;
	int	iPixelRepresentation;
	int	iBitsStored;
	float	fRescaleIntercept;
	float	fRescaleSlope;
	float	fDoseGridScaling;
	BOOL	bRtImagePosOK;
	float	fRtImagePosX;
	float	fRtImagePosY;
	char	szPhotometricInterpretation[20];
} SLICE_INFO;

SLICE_INFO*	getpSliceInfo(DICOMDataObject* pDDO)
{
	VR		*pVR;
	SLICE_INFO	*pSliceInfo;
	
	if(!(pSliceInfo = (SLICE_INFO*)malloc(sizeof(SLICE_INFO))))return NULL;
	memset(pSliceInfo, 0x00, sizeof(SLICE_INFO));
	
	// Rows
	pSliceInfo->iDimY = pDDO->GetUINT(0x0028, 0x0010);
	if(!pSliceInfo->iDimY)// Zero height.
	{
		free(pSliceInfo);
		return NULL;
	}
	// Columns
	pSliceInfo->iDimX = pDDO->GetUINT(0x0028, 0x0011);
	if(!pSliceInfo->iDimX)	// Zero width.
	{
		free(pSliceInfo);
		return NULL;
	}
	// BitsStored
	pSliceInfo->iBitsStored = pDDO->GetBYTE(0x0028, 0x0101);

	// NumberOfFrames
	pVR = pDDO->GetVR(0x0028, 0x0008);
	if (pVR && pVR->Length)
	{ 
		pSliceInfo->iNbFrames = pVR->Getatoi();
		pSliceInfo->iNbTimeSequences = 1;

		//  NumberOfSlices
		pVR = pDDO->GetVR(0x0054, 0x0081);
		if (pVR)
		{ /* Seems like a 3D time-sequence:
		   - Keep on using the iNbFrames member to designate the number of slices
		   - Use the iNbTimeSequences member for the number of volumes
		   */
			int iNbSlices, iNbVolumes;
			iNbSlices  = pVR->GetUINT();
			if (iNbSlices > 0)
			{ 	iNbVolumes = pSliceInfo->iNbFrames / iNbSlices;
				if (iNbVolumes * iNbSlices == pSliceInfo->iNbFrames)
				{ 
					pSliceInfo->iNbFrames = iNbSlices;
					pSliceInfo->iNbTimeSequences = iNbVolumes;
				}
			}
		}
	}
	// PhotometricInterpretation
	pVR = pDDO->GetVR(0x0028, 0x0004);
	if (pVR && pVR->Length && pVR->Data)
		strncpy(pSliceInfo->szPhotometricInterpretation,
			(char*)pVR->Data, pVR->Length<19 ? pVR->Length : 19);

	return pSliceInfo;
	
}

char * DePlane(char* data, int length)
{
	char *noPlane, *nPtr, *rPtr, *gPtr, *bPtr;

	if((noPlane = (char *) malloc(length)))
		{// Planes are 8 bit RGB (or should be).
		noPlane[length -1] = 0;// Never hurts, maybe an extra 0.
		length /= 3;// Each plane
		nPtr = noPlane;
		rPtr = data;
		gPtr = rPtr + length;
		bPtr = gPtr + length;
		for ( ;length > 0; length-- )
			{
			*nPtr++ = *rPtr++;
			*nPtr++ = *gPtr++;
			*nPtr++ = *bPtr++;
			}
		}
	return noPlane;// Memory error returns null.
}

void DeYBRFULL(char* data, int length)
{
	unsigned char *nPtr;
	int r, g, b;

	length /= 3; // each color
	nPtr = (unsigned char *)data;
	for ( ;length > 0; length-- )
		{

		r = nPtr[0]                           + 1.40200 * (nPtr[2]-128);
		g = nPtr[0] - 0.34414 * (nPtr[1]-128) - 0.71414 * (nPtr[2]-128);
		b = nPtr[0] + 1.77200 * (nPtr[1]-128);
		if (r>255) r=255;
		if (g>255) g=255;
		if (b>255) b=255;
		if (r<0) r=0;
		if (g<0) g=0;
		if (b<0) b=0;
		*nPtr++ = r;
		*nPtr++ = g;
		*nPtr++ = b;

/*		r = ((int)nPtr[0] * 128) + ((int)nPtr[2] * 179) - 22970;
		g = ((int)nPtr[0] * 128) - (44 * (int)nPtr[1]) - (91 * (int)nPtr[2]) - 17338;
		b = ((int)nPtr[0] * 128) + (277 * (int)nPtr[1]) - 29032;
		if(r & 0x8000) *nPtr++ = 0;
		else if(r & 0x4000) *nPtr++ = 255;
		else *nPtr++ = (r >> 6) & 0xFF;
		if(g & 0x8000) *nPtr++ = 0;
		else if(r & 0x4000) *nPtr++ = 255;
		else *nPtr++ = (g >> 6) & 0xFF;
		if(b & 0x8000) *nPtr++ = 0;
		else if(b & 0x4000) *nPtr++ = 255;
		else *nPtr++ = (b >> 6) & 0xFF;
*/
		}
}

static int DecompressRLE(SLICE_INFO* pSliceInfo, VR* pSequence,
			 void** ppResult, unsigned int * piResultSize)
{	int			rc = DCM_E_UNSUPPORTED_COMPRESSION;
    unsigned int    iFrameCnt, storedBitWidth, iDecompressedTotal;
	unsigned int	i, n, iNbRleSegments;
	Array<DICOMDataObject*>	*pADDO;
	DICOMDataObject*	pDDO;
	VR*			pVR;
	char			*pIn, *pOut, *outPtr, *endPtr, *pSeq, **hSeq, *cPtr, *endSeqPtr;
	int*			piHeader;
	unsigned int	iCompressedSize, iDecompressedSize, iNbCompressed, iNbDecompressed;
	char			cCurrent;
	char			*pR, *pG, *pB;
	bool			bIsRGB = false;

// Init stuff
	pOut = NULL;
    pSeq = NULL;
    endPtr = NULL;
    pG = NULL;
    pB = NULL;
    cPtr = NULL;
    iDecompressedSize = 0;
	iNbDecompressed = 0;
    *ppResult = NULL;
    *piResultSize = 0;
// Find out how big the output is.
    if(pSliceInfo->iBitsStored > 8) storedBitWidth = 2;
    else storedBitWidth = 1;
	iDecompressedTotal = pSliceInfo->iDimX * pSliceInfo->iDimY * storedBitWidth;
	if (pSliceInfo->iNbFrames)
		iDecompressedTotal *= pSliceInfo->iNbFrames;
	if (strncmp(pSliceInfo->szPhotometricInterpretation, "RGB", 3) == 0)
		bIsRGB = true;
// Of all color-representations, just support RGB for now.
	if (bIsRGB)
		iDecompressedTotal *= 3;
// Get the data.
    pADDO = (Array<DICOMDataObject*>*)pSequence->SQObjectArray;
// Frames can be RLE segments or sequences.
    for(iFrameCnt = 1; iFrameCnt < pADDO->GetSize(); iFrameCnt++)
    {
        pDDO = pADDO->Get(iFrameCnt);
        pVR  = pDDO->GetVR(0xfffe, 0xe000);
	
	/* The RLE data should start with 16 INT32 (Dicom chapter 5 annex G):
	 - 1st contains how many RLE segments there are (15 max)
	 - 2nd-16th contain offsets of each RLE segment
	 All non-used segments have offset 0 in this small table
	 Since this 16 INT32 header has a size of 64 bytes, the first offset should
	 be 64. All integers are stored in little-endian format.
	 */
	/* Check the header */
        if ((!pVR->Data) || (pVR->Length < 64))
        {
            if(pOut)free(pOut);
            return rc;
        }
        piHeader = (int*)pVR->Data;
#if NATIVE_ENDIAN == BIG_ENDIAN //Big Endian
        for(i=0; i<16; i++) piHeader[i] = SwitchEndian(piHeader[i]);
#endif
// How many segments, min 1, max of 15.
        iNbRleSegments = piHeader[0];
        if ((iNbRleSegments < 1) || (iNbRleSegments > 15))
        {
            if(pOut)free(pOut);
            return rc;
        }
// The first offset must be 64, the RLE header length.
        if (piHeader[1] != 64)
        {
            if(pOut)free(pOut);
            return rc;
        }
// Check all segments, each offset must be more than before.
	for (i=2; i<=iNbRleSegments; i++)
		if (piHeader[i] < piHeader[i-1])
        {
            if(pOut)free(pOut);
            return rc;
        }
// All unused segments are zero.
	for (i=iNbRleSegments + 1; i<=15; i++)
		if (piHeader[i] != 0)
        {
            if(pOut)free(pOut);
            return rc;
        }
// OK, it probably is RLE-compressed, time to make memory.
        if(!pOut) // Made already?
        {
            if(iDecompressedTotal & 1) iDecompressedTotal++;
            if(!(pOut = (char*)malloc(iDecompressedTotal)))
                return DCM_E_MEMORY;
            memset(pOut, 0xff, iDecompressedTotal);
            outPtr = pOut;
            endPtr = outPtr + iDecompressedTotal;
        }
        iNbCompressed   = 0;
        iCompressedSize = pVR->Length - 64;// Take out the header
        pIn = ((char*)piHeader) + 64;// Start of the data.
// Create a color buffer if needed.
        if (bIsRGB && storedBitWidth == 1 && iNbRleSegments == 3)
        {
            iDecompressedSize = pSliceInfo->iDimX * pSliceInfo->iDimY * storedBitWidth;
            if( outPtr + (iDecompressedTotal * 3) > endPtr) break; // Should fail?
            if(!(cPtr = (char*)malloc(iDecompressedTotal * 3)))
            {
                if(pOut)free(pOut);
                return DCM_E_MEMORY;
            }
            hSeq = &pR;// Make the handle's pointer valid.
            pR = cPtr;// Copy the pointer to the handle.
            pG = pR + iDecompressedSize;
            pB = pG + iDecompressedSize;
            endSeqPtr = pB + iDecompressedSize;
        }
        else
        {
         hSeq = &outPtr;// Give the pointer to the handle.
         endSeqPtr = endPtr;// End does not incrament.
        }
//	while (iNbDecompressed < iDecompressedSize)
        while (iNbCompressed < iCompressedSize)
        {
            cCurrent = pIn[iNbCompressed++];// Det the count.
            if (cCurrent >= 0)// Positive is a Literal Run.
            {// Check the length in & out?
                //cCurrent++;// The real count
                if ((iNbCompressed + (int)cCurrent + 1 <= iCompressedSize) && 
                    (*hSeq + cCurrent + 1 <= endSeqPtr))
                    { // Good, room to copy
                        memcpy(*hSeq, pIn + iNbCompressed, (int)cCurrent+1);
                        iNbCompressed   += (int)cCurrent+1;
                        *hSeq += (int)cCurrent+1;
                    }
                else // What happened, no room or seq ending 0?
                    break;// Run away!
            }
            else if (cCurrent == -128)break;// Just not allowed, may fail here.
            else// Replicate run.
            { n = ((int)-cCurrent) + 1;
                if (*hSeq + n > endPtr)// Enough room out?
                    break;
                for (i=0; i<n; i++)
                    *((*hSeq)++) = pIn[iNbCompressed];
                iNbCompressed++;
            }
        }
        if (bIsRGB && storedBitWidth == 1 && iNbRleSegments == 3)
        { /* Convert the R, G and B image planes to triplets */
            pR = cPtr;// Reset the red.
            for (i=0; i<iDecompressedSize; i++)// Already devided by 3
            {
                *outPtr++ = *pR++;
                *outPtr++ = *pG++;
                *outPtr++ = *pB++;
            }
            free(cPtr);
            cPtr = NULL;
        }
	}
    *ppResult = pOut;
    *piResultSize = iDecompressedTotal;
	return DCM_E_OK;
}

#if 0
static int DecompressRLE(SLICE_INFO* pSliceInfo, VR* pSequence,
			 void** ppResult, unsigned int * piResultSize)
{	int			rc = DCM_E_UNSUPPORTED_COMPRESSION;
	int			i, n, iNbRleSegments;
	Array<DICOMDataObject*>	*pADDO;
	DICOMDataObject*	pDDO;
	VR*			pVR;
	char			*pIn, *pOut;
	int*			piHeader;
	int			iCompressedSize, iDecompressedSize, iNbCompressed, iNbDecompressed;
	char			cCurrent;
	char			*pR, *pG, *pB;
	bool			bIsRGB = false;
	
	pADDO = (Array<DICOMDataObject*>*)pSequence->SQObjectArray;
	pDDO = pADDO->Get(1);
	pVR  = pDDO->GetVR(0xfffe, 0xe000);
	
	/* The RLE data should start with 16 INT32 (Dicom chapter 5 annex G):
	 - 1st contains how many RLE segments there are (15 max)
	 - 2nd-16th contain offsets of each RLE segment
	 All non-used segments have offset 0 in this small table
	 Since this 16 INT32 header has a size of 64 bytes, the first offset should
	 be 64. All integers are stored in little-endian format.
	 */
	/* Check the header */
	if ((!pVR->Data) || (pVR->Length < 64))
		return rc;

	piHeader = (int*)pVR->Data;
#if NATIVE_ENDIAN != LITTLE_ENDIAN //Big Endian
	for(i=0; i<16; i++) piHeader[i] = SwitchEndian(piHeader[i]);
#endif

	iNbRleSegments = piHeader[0];
	if ((iNbRleSegments < 1) || (iNbRleSegments > 15))
		return rc;

	if (piHeader[1] != 64)
			return rc;
	for (i=2; i<=iNbRleSegments; i++)
		if (piHeader[i] < piHeader[i-1])
			return rc;
	for (i=iNbRleSegments + 1; i<=15; i++)
		if (piHeader[i] != 0)
			return rc;
	
	/* OK, it probably is RLE-compressed */
	
	/* Find out how big the output is */
	iDecompressedSize = pSliceInfo->iDimX * pSliceInfo->iDimY *
	(pSliceInfo->iBitsStored / 8);
	if (pSliceInfo->iNbFrames)
		iDecompressedSize *= pSliceInfo->iNbFrames;
	if (strncmp(pSliceInfo->szPhotometricInterpretation, "RGB", 3) == 0)
		bIsRGB = true;
	/* Of all color-representations, just support RGB for now */
	
	if (bIsRGB)
		iDecompressedSize = iDecompressedSize * 3;
	pOut = (char*)malloc(iDecompressedSize);
	if (!pOut)
		return DCM_E_MEMORY;
	memset(pOut, 0xff, iDecompressedSize);
	
	iNbCompressed   = 0;
	iNbDecompressed = 0;
	iCompressedSize = pVR->Length - 64;
	pIn = ((char*)piHeader) + 64;
	while (iNbDecompressed < iDecompressedSize)
	{ cCurrent = pIn[iNbCompressed++];
		if (cCurrent >= 0)
		{ if ((iNbCompressed + ((int)cCurrent) + 1 <= iCompressedSize) &&
		      (iNbDecompressed + ((int)cCurrent) + 1 <= iDecompressedSize))
		{	memcpy(pOut + iNbDecompressed, pIn + iNbCompressed, ((int)cCurrent) + 1);
			iNbCompressed   += ((int)cCurrent) + 1;
			iNbDecompressed += ((int)cCurrent) + 1;
		}
		else
			break;
		}
		else if (cCurrent == -128)
			iNbCompressed = iNbCompressed;
		else
		{ n = ((int)-cCurrent) + 1;
			if (iNbDecompressed + n > iDecompressedSize)
				break;
			for (i=0; i<n; i++)
				pOut[iNbDecompressed + i] = pIn[iNbCompressed];
			iNbDecompressed += n;
			iNbCompressed++;
		}
	}
	
	if (bIsRGB)
	{ /* Convert the R, G and B image planes to triplets */
		pR = pOut;
		pOut = (char*)malloc(iDecompressedSize);
		if (!pOut)
		{ free(pR);
			return DCM_E_MEMORY;
		}
		pG = pR + iDecompressedSize / 3;
		pB = pG + iDecompressedSize / 3;
		for (i=0; i<iDecompressedSize / 3; i++)
		{ *pOut++ = pR[i];
			*pOut++ = pG[i];
			*pOut++ = pB[i];
		}
		pOut -= iDecompressedSize;
		free(pR);
	}
	
	
	rc = DCM_E_OK;
	
	if (rc == DCM_E_OK)
	{ *ppResult = pOut;
		*piResultSize = iDecompressedSize;
	}
	else
	{ *ppResult = NULL;
		*piResultSize = 0;
	}
	return rc;
}
#endif

// Make a palette color table for a segmented vr, don't forget to free it.
unsigned char * MakeSegTable(VR *pVR, unsigned int tableSize,unsigned int  *bitsWidth)
{
	unsigned char	*table, *table8, *table8Ptr, *inPtr, *endPtr, *tablePtr;
	unsigned int	cnt, cTableSize, step, sEnd, last;
	unsigned int	type, words;
	*bitsWidth = 1;
	
	cTableSize = tableSize * 2;// Ints.
	if(!(table = (unsigned char*)malloc(cTableSize)))
	{
		OperatorConsole.printf( "***[MakeSegTable]: Memory error.\n");
		return(NULL);
	}
	cnt = 0;
	inPtr = (unsigned char *)pVR->Data;
	endPtr = inPtr + pVR->Length;//Min type length.
	while(inPtr < endPtr)
	{
		type = *inPtr++;
		inPtr++;// To next int.
/*#if NATIVE_ENDIAN == LITTLE_ENDIAN
		words = ((UINT16*)pVR->Data)[pos];
#else*/
		words = *inPtr + (*(inPtr + 1) << 8);
		inPtr += 2;// Move char counter over to next int.
//#endif
//printf("%X ", cnt);
		if(cnt + (2 * words) > cTableSize)
		{//Overrun.
			free(table);
			OperatorConsole.printf(
				"***[MakeSegTable]: A count of %d + %d words would overrun %d of output memory.\n",
				cnt, 2 * words, cTableSize);
			return(NULL);
		}
		switch (type)
		{
			case 0://Discrete Segment Type
				if(inPtr + (2 * words) > endPtr)
				{//Overrun.
					free(table);
					OperatorConsole.printf(
					    "***[MakeSegTable]: %d words would overrun %d left of vr input.\n",
						2 * words, endPtr - inPtr);
					return(NULL);
				}
				for(;words > 0; words--)
				{
					table[cnt] = *inPtr++;
					if(table[cnt++] != *inPtr) *bitsWidth = 2;
					table[cnt++] = *inPtr++;
				}
				break;
			case 1://Linear Segment Type
				if(inPtr + 2 > endPtr)
				{//Overrun.
					free(table);
					OperatorConsole.printf(
						"***[MakeSegTable]: 1 word would overrun %d left of vr input.\n",
							       endPtr - inPtr);
				}
				if(cnt < 2)//Not allowed first.
				{
					free(table);
					OperatorConsole.printf( "***[MakeSegTable]: A Linear Segment can not be first.\n");
					return(NULL);
				}
				last = table[cnt - 2] + (table[cnt - 1] << 8);
				sEnd = *inPtr++;
				if(*inPtr != sEnd) *bitsWidth = 2;
				sEnd += ((*inPtr++) << 8);
				step = (sEnd - last)/ words;
				for(;words > 1; words--)
				{
					last += step;
					table[cnt++] = last & 0xFF;
					table[cnt++] = (last & 0xFF00) >> 8;
				}
				table[cnt++] = sEnd & 0xFF;
				table[cnt++] = (sEnd & 0xFF00) >> 8;
//				last = sEnd;
				break;
			case 2:// Indirect Sgment type
#if NATIVE_ENDIAN == LITTLE_ENDIAN
				sEnd = *((UINT*)inPtr);
#else
				sEnd = SwitchEndian(*((UINT*)inPtr));
#endif
				inPtr += 4;
				for(;words > 1; words--)
				{
					table[cnt++] = table[sEnd];
					table[cnt++] = table[sEnd];
				}
				break;
			default:
				OperatorConsole.printf( "***[MakeSegTable]: Opt code wrong or reserved type, %d.\n",type);
				return(NULL);//Opt code wrong or reserved!
				break;
		}
	}
	if(*bitsWidth == 1)
	{
		if(!(table8 = (unsigned char*)malloc(tableSize)))
		{
			free(table);
			OperatorConsole.printf( "***[MakeSegTable]: Memory error.\n");
			return(NULL);
		}
		tablePtr = table;
		table8Ptr = table8;
		for(cnt = 0; cnt < tableSize; cnt++)
		{
			*table8Ptr++ = *tablePtr++;
			tablePtr++;
		}
		return((unsigned char*)table8);
	}
	return((unsigned char*)table);
}

//Will turn any palette into Gray or RGB. The image Pixel Data (7FE0,0010) is decompressed.
bool DecodePalette(DICOMDataObject* pDDO)
{
	VR		*pVR, *vrImage;
	unsigned int	cmpts, inputArrayCnt, outSize, outLen, cnt, byteOffset, tableStep;
	unsigned int	tableSize, outBits, outBitw, allBitw, inBitw, offset, tableMin;
	unsigned char		*rTable, *gTable = NULL, *bTable = NULL, *outPtr, *outImage, *inPtr, *rPtr;
	bool		bSegmented, bLSBzero, bMSBzero;
	DICOMDataObject	*DDO;
	Array<DICOMDataObject *> *arrayImage;
// Init locals
	bSegmented =FALSE;
	inputArrayCnt = 0;
	byteOffset = 0;// Data in lsb.
	tableStep = 1;
	bLSBzero = TRUE;
	bMSBzero = TRUE;
	cmpts = 1;
// Check if its Palette Color
	pVR = pDDO->GetVR(0x0028, 0x0004);
	if(!pVR || pVR->Length != 14 || strncmp( (char *)pVR->Data, "PALETTE COLOR", 13))
	{
		OperatorConsole.printf("***[DecodePalette]: Not Palette Color.\n");
		return(FALSE);
	}
	
	// Check that is decompressed.
	pVR = pDDO->GetVR(0x0002, 0x0010);
	if(!pVR || pVR->Length > 20 || // Longer than 20 is some kind of jpeg or deflated.
	   (pVR->Length == 20 && ((char *)pVR->Data)[18] <= 32))// 1 & 2 are uncompressed LE & BE
	   return(FALSE);
// Check the samples per pixel.
	if(pDDO->GetBYTE(0x0028, 0x0002) != 1)
	{
		OperatorConsole.printf("***[DecodePalette]: Palette color cannot %d samples per pixel.\n",
				       pDDO->GetBYTE(0x0028, 0x0002));
		return(FALSE);
	}
	// If debug > 2, print info.
	if (DebugLevel > 2)
		SystemDebug.printf("Found Palette Color, depalatizing.\n");
// Look for palette stuff
	pVR = pDDO->GetVR(0x0028,0x1100); //The grayscale palette info..
	if(!pVR) pVR = pDDO->GetVR(0x0028,0x1101);//The red palette info..
	if(pVR && pVR->Length == 6)
	{
		tableSize = *(char *)pVR->Data + (((char *)pVR->Data)[1] << 8);
		tableMin = ((char *)pVR->Data)[2] + (((char *)pVR->Data)[3] << 8);
		outBits = ((char *)pVR->Data)[4];// 8 or 16 only allowed. Test?
	}
// Not palette color or Retired Large palette color. (Not supported here)
// I may add here code to fix the Photometric Interpretation.
// It would haxe to check 0x0002,0x0010 and 0x0028,0x0002 and guess.
	else
	{
		OperatorConsole.printf("***[DecodePalette]: No Palette information.\n", cmpts);
		return(FALSE);
	}
// Fix the table size.
	if(tableSize == 0)tableSize = 0x10000;// Fix the 2 byte table size.
// Count the colors.
	pVR = pDDO->GetVR(0x0028,0x1102);//The green palette info..
	if(pVR && pVR->Length == 6)cmpts++;// All are the same.
	pVR = pDDO->GetVR(0x0028,0x1103);//The blue palette info..
	if(pVR && pVR->Length == 6)cmpts++;
// Check for correct components.
	if (cmpts != 1 && cmpts != 3)
	{
		OperatorConsole.printf("***[DecodePalette]: %d components is not supported.\n", cmpts);
		return(FALSE);
	}
// If debug > 3, print info.
	if (DebugLevel > 3)
		SystemDebug.printf("Palette table size = %d, Minimum value = %d in %d bytes.\n",
			tableSize, tableMin, outBits);
// Lets look at the color palette.
	pVR = pDDO->GetVR(0x0028,0x1200); //The grayscale palette.
	if(!pVR) pVR = pDDO->GetVR(0x0028,0x1201);//The red palette.
	if(pVR && pVR->Length)
	{
		rTable = (unsigned char*)pVR->Data;
		if(pVR->TypeCode == 'OB' || outBits <= 8) outBitw = 1;//tableStep = 1.
		else 
		{
            tableStep = 2;
			if(tableSize == pVR->Length) outBitw = 2;
			else outBitw = 1;// tableSize * 2 == pVR->Length, will check later.
		}
// Check the table size.
		if(pVR->Length != tableSize * tableStep / outBitw)
		{
			OperatorConsole.printf(
				"***[DecodePalette]: Palette length should be %d, is %d, %d bits out.\n",
					       tableSize * outBits/8, pVR->Length, outBits);
			return(FALSE);
		}
// Get the  other colors.
		if(cmpts == 3)
		{
			pVR = pDDO->GetVR(0x0028,0x1202);//The green palette.
			if(!pVR || pVR->Length != tableSize * tableStep / outBitw)
			{
				OperatorConsole.printf(
					"***[DecodePalette]: Green palette length is %d, should be %d.\n",
					pVR->Length, tableSize * outBits/8);
				return(FALSE);
			}
			gTable = (unsigned char*)pVR->Data;
			pVR = pDDO->GetVR(0x0028,0x1203);//The blue palette.
			if(!pVR || pVR->Length != tableSize * tableStep / outBitw)
			{
				OperatorConsole.printf(
							"***[DecodePalette]: Blue palette length is %d, should be %d.\n",
							pVR->Length, tableSize * outBits/8);
				return(FALSE);
			}
			bTable = (unsigned char*)pVR->Data;
		}
// If debug > 3, print info.
		if (DebugLevel > 3)
			SystemDebug.printf("%d components in standard palette(s).\n",cmpts);
	}
	else //No gray or red table.
	{// Segmented look up table.
		pVR = pDDO->GetVR(0x0028,0x1220);//The gray segmented palette info..
		if(!pVR )pVR = pDDO->GetVR(0x0028,0x1221);//The red segmented palette info..
		if(!pVR || !pVR->Length)
		{
			OperatorConsole.printf("***[DecodePalette]: No palettes were found\n");
			return(FALSE);
		}
		if(!(rTable = MakeSegTable(pVR, tableSize, &outBitw)))return(FALSE);
		bSegmented = TRUE;
		allBitw = outBitw;
		if( cmpts == 3 )
		{
			if(!(pVR = pDDO->GetVR(0x0028,0x1222)) || !pVR->Length ||
			   !(gTable = MakeSegTable(pVR, tableSize, &outBitw)))
			{
				free(rTable);
				OperatorConsole.printf("***[DecodePalette]: Green palette was not found\n");
				return(FALSE);
			}
			if (allBitw != outBitw)
			{
				free(rTable);
				OperatorConsole.printf(
					"***[DecodePalette]: Green palette bit width does't match the red.\n");
				free(rTable);
				if(gTable)free(gTable);
				return(FALSE);
			}
			if(!(pVR = pDDO->GetVR(0x0028,0x1223)) || !pVR->Length ||
			   !(bTable = MakeSegTable(pVR, tableSize, &outBitw)))
			{
				OperatorConsole.printf("***[DecodePalette]: Blue palette was not found\n");
				return(FALSE);
				free(rTable);
				free(gTable);
				if(bTable)free(bTable);
			}
			if (allBitw != outBitw)
			{
				free(rTable);
				free(gTable);
				free(bTable);
				OperatorConsole.printf(
					"***[DecodePalette]: Blue palettes bit width does't match the red.\n");
				return(FALSE);
			}
		}
		tableStep = outBitw;
// If debug > 3, print info.
		if (DebugLevel > 3)
			SystemDebug.printf("%d components in segmented palette(s).\n",cmpts);
	}
// Look for 8 in 16 with zeros in one byte.
	if(tableStep == 2)
	{
        rPtr = rTable;
		for(cnt = 0; cnt < pVR->Length; cnt += 2)
		{// Look for all zeros
			if(*rPtr++ != 0)bLSBzero = FALSE;
			if(*rPtr++ != 0)bMSBzero = FALSE;
		}
        
		if(bMSBzero == TRUE)
		{
			
			if(bLSBzero == TRUE)
			{
				OperatorConsole.printf("***[DecodePalette]: The whole table is zero.\n");
				if(bSegmented == TRUE)
				{
					free(rTable);
					if(gTable)free(gTable);
					if(bTable)free(bTable);
				}
				return (FALSE);
			}
			byteOffset = 0;//LSB has the data
			outBitw = 1;
		}
		if(bLSBzero == TRUE)
		{
			byteOffset = 1;//MSB has the data
			outBitw = 1;
		}
	}
// Get the input data size.
	if(!(inBitw = pDDO->GetBYTE(0x0028, 0x0101)))inBitw = 8;
	if(inBitw > 8)inBitw = 2;
	else inBitw = 1;
// If debug > 3, print info.
	if (DebugLevel > 3)
	{
		if(outBitw == 2) SystemDebug.printf("Output data is words from \n");
		else
		{
			SystemDebug.printf("Output data is bytes in the \n");
			if(byteOffset == 0)SystemDebug.printf("LSB from \n");
			else SystemDebug.printf("MSB from \n");
		}
		if(inBitw == 1)SystemDebug.printf("8 bits in.\n");
		else SystemDebug.printf("16 bits in.\n");
	}
	// Now for the image data.
	pVR = pDDO->GetVR(0x7fe0, 0x0010); // Get the Image VR.
	vrImage = pVR;//Should be done here.
	if(pVR && pVR->Length)
	{
		if(pVR->SQObjectArray)
		{// Can be here for frames.
			arrayImage = (Array<DICOMDataObject *> *) pVR->SQObjectArray;
			while (inputArrayCnt < arrayImage->GetSize())
			{
				DDO = arrayImage->Get(inputArrayCnt++);//Get the array.
				vrImage = DDO->GetVR(0xfffe, 0xe000);//Get the data
				if(vrImage && vrImage->Length >= 0x10) break;
			}
			if(inputArrayCnt == arrayImage->GetSize())
			{
				OperatorConsole.printf("***[DecodePalette]: Could not find the image data.\n");
				if(bSegmented == TRUE)
				{
					free(rTable);
					if(gTable)free(gTable);
					if(bTable)free(bTable);
				}
				return (FALSE);
			}
		}
	}
// Set the output size
	outLen = outSize = vrImage->Length * cmpts * outBitw;
	inPtr = (unsigned char*)vrImage->Data;
	if(outLen & 1)outLen++;
	if(!(outImage = (unsigned char*)malloc(outLen)))
	{
		OperatorConsole.printf("***[DecodePalette]: Could not allocate %d bytes.\n", outLen);
		if(bSegmented == TRUE)
		{
			free(rTable);
			if(gTable)free(gTable);
			if(bTable)free(bTable);
		}
		return (FALSE);
	}
// Copy the data.
	outPtr = outImage;
//	cmpts--;// Make cmpts into a bool.
	for(cnt = 0; cnt < vrImage->Length; cnt++)
	{
		offset = *inPtr++;// Get the LSB or byte value.
		if(inBitw == 2)
		{
			offset += (*inPtr++ << 8);// Add the MSB if there.
			cnt++;
		}
		if(offset < tableMin) offset = 0;
		else offset -= tableMin;
		if( offset >= tableSize)offset = tableSize - 1;
		offset *= tableStep;// Double for words only.
		offset += byteOffset;// Off set for msb byte.
		*outPtr++ = rTable[offset];
		if(outBitw == 2)*outPtr++ = rTable[offset + 1];//Next value for words.
		if(cmpts == 3)
		{
			*outPtr++ = gTable[offset];
			if(outBitw == 2) *outPtr++ = gTable[offset + 1];//Next value for words.
			*outPtr++ = bTable[offset];
			if(outBitw == 2) *outPtr++ = bTable[offset + 1];//Next value for words.
		}
	}
// Done with the tables, free if ours.
    if(bSegmented == TRUE)
    {
        free(rTable);
        if(gTable)free(gTable);
        if(bTable)free(bTable);
    }
// Fix the last zero if needed.
	if(outSize & 1)outImage[outSize] = 0;
// Change the image data.
	vrImage->Reset(); // Deletes the pixel data
	vrImage->Length = outLen;
	vrImage->Data = outImage;
	if(outBitw == 2) vrImage->TypeCode = 'OW';
	else vrImage->TypeCode = 'OB';
	vrImage->ReleaseMemory = TRUE;//Give the memory to the vr.
// Fix the Photometric Interpretation and Samples per.
	if(cmpts == 1)pDDO->ChangeVR( 0x0028, 0x0004, "MONOCHROME2\0", 'CS');
	else
	{
		pDDO->ChangeVR( 0x0028, 0x0004, "RGB\0", 'CS');
		pDDO->ChangeVR( 0x0028, 0x0002, (UINT16)3, 'US');
	}
// Fix output widths (DecodePalette).
	if(outBitw == 2)
	{
		pDDO->ChangeVR( 0x0028, 0x0100, (BYTE)16, 'US');
		pDDO->ChangeVR( 0x0028, 0x0101, (BYTE)16, 'US');
		pDDO->ChangeVR( 0x0028, 0x0102, (BYTE)15, 'US');
	}
	else
	{
		pDDO->ChangeVR( 0x0028, 0x0100, (BYTE)8, 'US');
		pDDO->ChangeVR( 0x0028, 0x0101, (BYTE)8, 'US');
		pDDO->ChangeVR( 0x0028, 0x0102, (BYTE)7, 'US');
	}
// Delete the no longer needed palette vr's.
// Palette Color Lookup Table Descriptors
	if((pVR = pDDO->GetVR(0x0028,0x1100)) && pVR->Length)pDDO->DeleteVR(pVR);
	if((pVR = pDDO->GetVR(0x0028,0x1101)) && pVR->Length)pDDO->DeleteVR(pVR);
	if((pVR = pDDO->GetVR(0x0028,0x1102)) && pVR->Length)pDDO->DeleteVR(pVR);
	if((pVR = pDDO->GetVR(0x0028,0x1103)) && pVR->Length)pDDO->DeleteVR(pVR);
// Palette Color Lookup Table UID
	if((pVR = pDDO->GetVR(0x0028,0x1199)) && pVR->Length)pDDO->DeleteVR(pVR);
// Regular palette tables
	if((pVR = pDDO->GetVR(0x0028,0x1200)) && pVR->Length)pDDO->DeleteVR(pVR);
	if((pVR = pDDO->GetVR(0x0028,0x1201)) && pVR->Length)pDDO->DeleteVR(pVR);
	if((pVR = pDDO->GetVR(0x0028,0x1202)) && pVR->Length)pDDO->DeleteVR(pVR);
	if((pVR = pDDO->GetVR(0x0028,0x1203)) && pVR->Length)pDDO->DeleteVR(pVR);
// Segmented palette tables
	if((pVR = pDDO->GetVR(0x0028,0x1220)) && pVR->Length)pDDO->DeleteVR(pVR);
	if((pVR = pDDO->GetVR(0x0028,0x1221)) && pVR->Length)pDDO->DeleteVR(pVR);
	if((pVR = pDDO->GetVR(0x0028,0x1222)) && pVR->Length)pDDO->DeleteVR(pVR);
	if((pVR = pDDO->GetVR(0x0028,0x1223)) && pVR->Length)pDDO->DeleteVR(pVR);
	return(TRUE);
}

#ifndef NOINTJPEG
#ifndef HAVE_LIBJPEG //Not needed for the jpeg library

/***********************************************************************************/
/*
   Code from CqJPEG.cpp			(CqDicom.dll)
*/
/***********************************************************************************/

/* There was no way to incorporate following functions in one file */
extern int DecompressJPEG16(SLICE_INFO* pSliceInfo, VR* pSequence,
	void** ppResult, unsigned int * piResultSize);
extern int DecompressJPEG12(SLICE_INFO* pSliceInfo, VR* pSequence,
	void** ppResult, unsigned int * piResultSize);
extern int DecompressJPEG8(SLICE_INFO* pSliceInfo, VR* pSequence,
	void** ppResult, unsigned int * piResultSize);

/* Forward declaration of RLE decompressor */
static int DecompressRLE(SLICE_INFO* pSliceInfo, VR* pSequence,
	void** ppResult, unsigned int * piResultSize);


#define		Uint8	unsigned char
#define		Uint16	unsigned short
#define		Uint32	unsigned int


/* Following two functions are to find out which version of the ijg libraries
   to use in order to decompress.
*/
static Uint16 readUint16(const Uint8* pData)
{ return (((Uint16)(*pData) << 8) | ((Uint16)(*(pData+1))));
}

static Uint8 scanJpegDataForBitDepth(
  const Uint8 *data,
  const Uint32 fragmentLength)
{
  Uint32 offset = 0;
  while(offset+4 < fragmentLength)
  {
    switch(readUint16(data+offset))
    {
      case 0xffc0: // SOF_0: JPEG baseline
        return data[offset+4];
        /* break; */
      case 0xffc1: // SOF_1: JPEG extended sequential DCT
        return data[offset+4];
        /* break; */
      case 0xffc2: // SOF_2: JPEG progressive DCT
        return data[offset+4];
        /* break; */
      case 0xffc3 : // SOF_3: JPEG lossless sequential
        return data[offset+4];
        /* break; */
      case 0xffc5: // SOF_5: differential (hierarchical) extended sequential, Huffman
        return data[offset+4];
        /* break; */
      case 0xffc6: // SOF_6: differential (hierarchical) progressive, Huffman
        return data[offset+4];
        /* break; */
      case 0xffc7: // SOF_7: differential (hierarchical) lossless, Huffman
        return data[offset+4];
        /* break; */
      case 0xffc8: // Reserved for JPEG extentions
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffc9: // SOF_9: extended sequential, arithmetic
        return data[offset+4];
        /* break; */
      case 0xffca: // SOF_10: progressive, arithmetic
        return data[offset+4];
        /* break; */
      case 0xffcb: // SOF_11: lossless, arithmetic
        return data[offset+4];
        /* break; */
      case 0xffcd: // SOF_13: differential (hierarchical) extended sequential, arithmetic
        return data[offset+4];
        /* break; */
      case 0xffce: // SOF_14: differential (hierarchical) progressive, arithmetic
        return data[offset+4];
        /* break; */
      case 0xffcf: // SOF_15: differential (hierarchical) lossless, arithmetic
        return data[offset+4];
        /* break; */
      case 0xffc4: // DHT
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffcc: // DAC
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffd0: // RST m
      case 0xffd1:
      case 0xffd2:
      case 0xffd3:
      case 0xffd4:
      case 0xffd5:
      case 0xffd6:
      case 0xffd7: 
        offset +=2;
        break;
      case 0xffd8: // SOI
        offset +=2;
        break;
      case 0xffd9: // EOI
        offset +=2;
        break;
      case 0xffda: // SOS
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffdb: // DQT
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffdc: // DNL
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffdd: // DRI
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffde: // DHP
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffdf: // EXP
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xffe0: // APPn
      case 0xffe1:
      case 0xffe2:
      case 0xffe3:
      case 0xffe4:
      case 0xffe5:
      case 0xffe6:
      case 0xffe7:
      case 0xffe8:
      case 0xffe9:
      case 0xffea:
      case 0xffeb:
      case 0xffec:
      case 0xffed:
      case 0xffee:
      case 0xffef: 
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xfff0: // JPGn
      case 0xfff1:
      case 0xfff2:
      case 0xfff3:
      case 0xfff4:
      case 0xfff5:
      case 0xfff6:
      case 0xfff7:
      case 0xfff8:
      case 0xfff9:
      case 0xfffa:
      case 0xfffb:
      case 0xfffc:
      case 0xfffd: 
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xfffe: // COM
        offset += readUint16(data+offset+2)+2;
        break;
      case 0xff01: // TEM
        break;
      default: 
        if ((data[offset]==0xff) && (data[offset+1]>2) && (data[offset+1] <= 0xbf)) // RES reserved markers
        { 
          offset += 2;
        }
        else return 0; // syntax error, stop parsing
        break;
    }
  } // while
  return 0; // no SOF marker found
}


/* This JPEG decompression function accepts a dicom VR with pixeldata-sequence,
   and replaces the sequence by a 'normal' VR.
   returns:
   - DCM_E_UNSUPPORTED_FORMAT when the sequence was not understood
   - DCM_E_UNSUPPORTED_COMPRESSION when the data are not supported by the ijg lib
   - DCM_E_MEMORY when out of memory
   - DCM_E_OK
*/
static int DecompressJPEG(SLICE_INFO* pSliceInfo, VR* pSequence)
{ int				rc = DCM_E_UNSUPPORTED_FORMAT;
  Array<DICOMDataObject*>*	pADDO;
  DICOMDataObject*		pDDO;
  VR*				pVR;
  int				iNbBitsPerSample;
  void*				pResult = NULL;
  unsigned int			iSize = 0;

  /* Is the dicom-sequence OK? */
  pADDO = (Array<DICOMDataObject*>*)pSequence->SQObjectArray;
  if (!pADDO)
    return rc;
  pDDO = pADDO->Get(1);
  if (!pDDO)
    return rc;
  pVR  = pDDO->GetVR(0xfffe, 0xe000);
  if (!pVR)
    return rc;
  if ((!pVR->Data) || (pVR->Length <= 0))
    return rc;

  /* Does ijg support the compression? */
  iNbBitsPerSample = scanJpegDataForBitDepth((unsigned char*)pVR->Data, pVR->Length);
  if ((iNbBitsPerSample <= 0) || (iNbBitsPerSample > 16))
  { /* It is not a supported JPEG image. Maybe it is RLE compressed */
    DecompressedRLE++;
    rc = DecompressRLE(pSliceInfo, pSequence, &pResult, &iSize);
    if (rc != DCM_E_OK)
      return rc;
  }
  else
  { /* Depending on iNbBitsPerSample, a different ijg libs/includes are used */
    DecompressJpeg++;

    if (iNbBitsPerSample > 12)
      rc = DecompressJPEG16(pSliceInfo, pSequence, &pResult, &iSize);
    else if (iNbBitsPerSample > 8)
      rc = DecompressJPEG12(pSliceInfo, pSequence, &pResult, &iSize);
    else
      rc = DecompressJPEG8(pSliceInfo, pSequence, &pResult, &iSize);
  }


  /* When OK, replace the VR */
  if (rc == DCM_E_OK)
  { /* TO DO: add consistency checks iSize <--> DicomTags */
    /* Cleanup the sequence, and make a normal VR */
//    delete[] pADDO;
//    pSequence->SQObjectArray = NULL;
    pSequence->Reset();
    pSequence->Data = pResult;
    pSequence->ReleaseMemory = TRUE;
    if (iSize & 0x01)
    { ((char*)pResult)[iSize] = 0;
      iSize++;
    }
    pSequence->Length = iSize;
  }

  return rc;
}

/***********************************************************************************/
/*
   JPEG decompression interface for dgate
*/
/***********************************************************************************/

static int dcmdjpeg(DICOMDataObject* pDDO)
{
  /* Functions above (from CqDicom.dll) will do the work.
     Here:
     - Get the necessary parameters from pDDO, and put them in SLICE_INFO structure
     - Adjust some dicom-tags
  */
  char		s[100];
  int		rc;
  VR		*pVR;
  SLICE_INFO	*pSliceInfo;

  rc = DCM_E_UNSUPPORTED_FORMAT;
  if(!(pSliceInfo = getpSliceInfo(pDDO))) return rc;
  if(pSliceInfo->iBitsStored == 0) pSliceInfo->iBitsStored = 16; // Default 16 bits

  /* PixelData */
  pVR = pDDO->GetVR(0x7fe0, 0x0010);

  if(pVR)rc = DecompressJPEG(pSliceInfo, pVR); // And RLE

  free(pSliceInfo);
  if (rc != DCM_E_OK)
    return rc;

  /* Some tags need to been changed */

  /*
     The Attribute Lossy Image Compression (0028,2110) conveys that the Image has undergone
     lossy compression. It provides a means to record that the Image has been compressed (at a
     point in its lifetime) with a lossy algorithm and changes have been introduced into the pixel data.
     Once the value has been set to '01', it shall not be reset.
     Note: If an image is compressed with a lossy algorithm, the attribute Lossy Image Compression
     (0028,2110) is set to '01'. Subsequently, if the image is decompressed and transferred in
     uncompressed format, this attribute value remains '01'.
  */

  /* It seems we only have to change the TransferSyntaxUID to LittleEndianExplicit */
  pDDO->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.1\0", 'IU');

  return rc;
}

#endif // #ifndef HAVE_LIBJPEG
#endif // #ifndef NOINTJPEG

/***********************************************************************************/
/*

*/
/***********************************************************************************/

// create tempory filename in printer_files directory for (de)compression

static void NewTempFileWExt(char *name, const char *ext)
	{
	int i;
	char name2[70], szTemp[256], szRootSC[256];

	MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile);
  	MyGetPrivateProfileString(szRootSC, "TempDir", "", szTemp, 64, ConfigFile);

        if (szTemp[0]==0)
		{
		name[0]=0;
		GetPhysicalDevice("MAG0", name);
		strcat(name, "printer_files");
		}
	else
		strcpy(name, szTemp);

#ifndef WIN32
	mkdir(name, 0777);
#else
	mkdir(name);
#endif

	i = strlen(name);
	name[i]   = PATHSEPCHAR;
	name[i+1] = 0;

	GenUID(name2);
	strcat(name2, ext);

	strcat(name, name2);
	}

static void NewTempFile(char *name)
	{
	NewTempFileWExt(name, ".dcm");
	}


// Decompress NKI and/or JPEG coded image
BOOL
DecompressImage(DICOMDataObject **pDDO, int *Changed)
	{
	PDU_Service		PDU;
	char			name[1024], name2[1024];
	char			*noPlane;
	VR*			pVR;
	BOOL			nki, jpeg;
	int			rc;
	void*			pResult = NULL;
	unsigned int		iSize = 0;
	SLICE_INFO		*pSliceInfo = NULL;

	*Changed = 0;
	noPlane = NULL;

	PDU.AttachRTC(&VRType);

	// is it nki compressed ?
	nki = ((*pDDO)->GetVR(0x7fdf, 0x0010))!=NULL && ((*pDDO)->GetVR(0x7fe0, 0x0010))==NULL;

	// is it jpeg compressed ?
	pVR = (*pDDO)->GetVR(0x7fe0, 0x10);
	jpeg = (pVR && pVR->SQObjectArray);
#ifdef FUJI_FIX
	// Look for a Jfif header. (LittleEndianImplicit, jpeg compressed, how evil)
	if (!jpeg && pVR && ((unsigned char *)pVR->Data)[0] == 0xFF && ((unsigned char *)pVR->Data)[1] == 0xD8
		&& ((unsigned char*)pVR->Data)[2] == 0xFF && ((unsigned char *)pVR->Data)[3] == 0xE0
		&& ((char*)pVR->Data)[4] == 0x00 && ((char*)pVR->Data)[5] == 0x10
		&& strncmp(&((char *)pVR->Data)[6],"JFIF", 4) == 0)
	{
		jpeg = TRUE;
	}
#endif

	if(jpeg) // or RLE
	{
// distinguish RLE converted from jpeg without TrnSyntaxUID mvh 20110413
	        BOOL isrle = FALSE;
		Array < DICOMDataObject *> *pADDO;
		pVR = (*pDDO)->GetVR(0x7fe0, 0x0010);
		if (pVR && pVR->SQObjectArray)
			{
			pADDO = (Array<DICOMDataObject*>*)pVR->SQObjectArray;
			if (pADDO->GetSize() > 1)
				{
				pVR = pADDO->Get(1)->GetVR(0xfffe, 0xe000);
				if (pVR && pVR->Length>64)
					{
					int *piHeader = (int*)pVR->Data;
					int offs1;
#if NATIVE_ENDIAN == BIG_ENDIAN //Big Endian
        				offs1 = SwitchEndian(piHeader[1]);
#else
	        			offs1 = piHeader[1];
#endif
					isrle = offs1==64;
					}
				}
			}
// end distinguish RLE converted from jpeg without TrnSyntaxUID mvh 20110413

		pVR = (*pDDO)->GetVR(0x0002, 0x0010);
		if ((!pVR || (pVR->Length == 20 && ((char*)pVR->Data)[18] == '5')) && isrle) //RLE or no 0x0002, 0x0010
		{
			rc = DCM_E_UNSUPPORTED_FORMAT;

			if((pSliceInfo = getpSliceInfo(*pDDO)))
			{
				if(pSliceInfo->iBitsStored == 0)
					pSliceInfo->iBitsStored = 16; // Default 16 bits

				pVR = (*pDDO)->GetVR(0x7fe0, 0x0010);
				if(pVR) rc = DecompressRLE(pSliceInfo, pVR, &pResult, &iSize);
				free(pSliceInfo);
				if (rc == DCM_E_OK)// RLE decompressed
				{
					// Fix the image
					pVR->Reset();
					pVR->Data = pResult;
					pVR->ReleaseMemory = TRUE;
					if (iSize & 0x01)
					{
						((char*)pResult)[iSize] = 0;
						iSize++;
					}
					pVR->Length = iSize;
					if(!(pVR = (*pDDO)->GetVR(0x0002, 0x0010)))
					{
						pVR = new VR(0x0002, 0x0010, 0, (void *)NULL, FALSE);
						(*pDDO)->Push(pVR);
					}
					pVR->ReAlloc(20);
					memcpy((char *)pVR->Data, "1.2.840.10008.1.2.1\0",20);// LittleEndianExplict
					DecompressedRLE++;
					*Changed = 1;
					jpeg = FALSE;// Check for planes
				}
			}
		}
#ifdef HAVE_LIBCHARLS // JPEG LS stuff
                else if(pVR && pVR->Length == 22 && ((char*)pVR->Data)[20] == '8')//80 or 81 is jpeg ls.
                {
                    if(!DecompressJPEGLS(*pDDO))
                    {
                        OperatorConsole.printf("***[DecompressImage]: failed to decompress JpegLS\n");
//                        delete pDDO;//bcb Do not delete what is not ours!
                        return ( FALSE );
                    }
                DecompressJpegLS++;
                *Changed = 1;
                return ( TRUE );
                }
#endif
#ifdef HAVE_J2K // JPEG 2000 stuff
		else if(pVR && pVR->Length == 22 && ((char*)pVR->Data)[20] == '9')//90 or 91 is jpeg 2000.
		{
#ifndef HAVE_BOTH_J2KLIBS
#ifdef HAVE_LIBJASPER
			if(!DecompressJPEG2K(*pDDO))
#endif
#ifdef HAVE_LIBOPENJPEG2
			if(!DecompressJPEG2Ko(*pDDO))
#endif
#else //HAVE_BOTH_J2KLIBS
			if(1)
			{
				if(!DecompressJPEG2Ko(*pDDO))
				{
					OperatorConsole.printf("***[DecompressImage]:OpenJPEG failed to decompress JP2000\n");
					//delete pDDO;
					return FALSE;
				}
			}
			else if(!DecompressJPEG2K(*pDDO))
#endif
			{
				OperatorConsole.printf("***[DecompressImage]: failed to decompress JP2000\n");
				//delete pDDO;
				return ( FALSE );
			}
			DecompressJpeg2000++;
			*Changed = 1;
			return ( TRUE );
		}
#endif //End HAVE_J2K
	}

	if (nki && !jpeg)		// nki compressed
		{
		DecompressNKI(*pDDO);
                *Changed = 1;
		nki = FALSE; 		//Check for planes next
		}
	
	if (!nki && !jpeg) 		// not compressed
		{
		// Check if its Palette Color
		pVR = (*pDDO)->GetVR(0x0028, 0x0004);
		if(pVR && pVR->Length == 14 && strncmp( (char *)pVR->Data, "PALETTE COLOR", 13) == 0)
			{
			DecodePalette(*pDDO);
			DePaletted++;
			// palette is never planar; fix for colorzipped.tar test images
			(*pDDO)->ChangeVR( 0x0028, 0x0006, (UINT8)0, 'US');// Not any more.
			*Changed = 1;
			}

		//Planar check, done after RLE (& deflate?) decompression; only for RGB or YBR
		if((*pDDO)->GetBYTE(0x0028, 0x0006))
		  if((*pDDO)->GetBYTE(0x0028, 0x0002)==3)
			{
			pVR = (*pDDO)->GetVR(0x7fe0, 0x0010);
			if(pVR->Length) noPlane = DePlane((char *)pVR->Data, pVR->Length);
			if(noPlane)
				{
				free(pVR->Data);
				pVR->Data = noPlane;
				(*pDDO)->ChangeVR( 0x0028, 0x0006, (UINT8)0, 'US');// Not any more.
				DePlaned++;
				*Changed = 1;
				}
			}

		// Check if its YBR
		pVR = (*pDDO)->GetVR(0x0028, 0x0004);// Have from above
		if(pVR && pVR->Length == 8 && strncmp( (char *)pVR->Data, "YBR_FULL", 8) == 0)
			{
			pVR = (*pDDO)->GetVR(0x7fe0, 0x0010);
			if(pVR->Length) DeYBRFULL((char *)pVR->Data, pVR->Length);
			(*pDDO)->ChangeVR( 0x0028, 0x0004, "RGB\0", 'CS');
			DePaletted++;
			*Changed = 1;
			}

		return TRUE;
		}

        if(!UseBuiltInDecompressor())
		{
		NewTempFile(name);
		NewTempFile(name2);

		PDU.SaveDICOMDataObject(name, DICOM_CHAPTER_10_EXPLICIT, *pDDO);
		delete *pDDO;
		*pDDO=NULL;

#ifdef WIN32		
		if (!ExecHidden("dcmdjpeg.exe", "-F +ti", name, name2, ""))
#else
		if (!ExecHidden("dcmdjpeg", "-F +ti", name, name2, "DCMDICTPATH=dicom.dic\0"))
#endif
		
			{
			*pDDO = PDU.LoadDICOMDataObject(name);
			unlink(name);
			return FALSE;
			}

		*pDDO = PDU.LoadDICOMDataObject(name2);
		if(!*pDDO)
			{
			OperatorConsole.printf("***[DecompressImage]: Error on load after external decompression, image not decompressed\n");
			*pDDO = PDU.LoadDICOMDataObject(name);	// mvh 20060402: restore original object
			unlink(name);
			//return FALSE;
			return TRUE;
			}

		unlink(name);
		unlink(name2);
	        DecompressJpeg++;
		}
#ifndef NOINTJPEG
	else

#ifdef HAVE_LIBJPEG
	 	{
		if(!DecompressJPEGL(*pDDO))
            		{
			OperatorConsole.printf("***[DecompressImage]: JPEG library decompression error\n");
        	    	return(FALSE);
            		}
	        DecompressJpeg++;
		}
#else
		{
	        if (dcmdjpeg(*pDDO) != DCM_E_OK)
			{
			OperatorConsole.printf("***[DecompressImage]: JPEG decompression error\n");
			return FALSE;
			}
	        DecompressJpeg++;
		}
#endif //HAVE_LIBJPEG #else
#endif

        *Changed = 1;
	return ( TRUE );

	}


// Compress JPEG coded image (lFileCompressMode must be in [0..6], 0=uncompressed)
// CompressJPEGImage() used in one place later in this file, pDDO already decompressed. BCB
BOOL
CompressJPEGImage(DICOMDataObject **pDDO, int lFileCompressMode, int *ActualMode, int qual)
	{
	PDU_Service		PDU;
	char			name[1024], name2[1024];
	VR*			pVR;
	char			opts[80];

        *ActualMode = -1;

	if (lFileCompressMode=='0')
		{
                *ActualMode = '0';
		// return DecompressImage(pDDO, &dum); BCB
		return TRUE;
		}

	PDU.AttachRTC(&VRType);

	NewTempFile(name);
	NewTempFile(name2);

	pVR = (*pDDO)->GetVR(0x0008, 0x0060);
	if (pVR) 
		{
			if (pVR->Length>=2 && (memcmp(pVR->Data, "PR", 2)==0 || 
                                               memcmp(pVR->Data, "SR", 2)==0 ||
                                               memcmp(pVR->Data, "RT", 2)==0))
			{
			SystemDebug.printf("[CompressJPEGImage]: JPEG compression not allowed for PR/SR/RT\n");
	        	return FALSE;
			}
		}

	pVR = (*pDDO)->GetVR(0x0028, 0x0100);
	if (pVR) 
		{
			if (pVR->Length>=2 && pVR->GetUINT16() < 8)
			{
			SystemDebug.printf("[CompressJPEGImage]: JPEG compression skipped for <8 bits data\n");
	        	return FALSE;
			}
		}

	if (((*pDDO)->GetVR(0x7fdf, 0x0010)) != NULL)
		if (!DecompressNKI(*pDDO))
			{
			OperatorConsole.printf("***[CompressJPEGImage]: failed to NKI decompress DICOM object\n");
	        	return FALSE;
			}

#ifdef HAVE_J2K
	if (lFileCompressMode=='K' || lFileCompressMode=='k' || lFileCompressMode=='L' || lFileCompressMode=='l')
		{
		if( lFileCompressMode=='K' || lFileCompressMode=='k') qual = 100;
#ifndef HAVE_BOTH_J2KLIBS
#ifdef HAVE_LIBJASPER
		if(!CompressJPEG2K(*pDDO,qual)) return FALSE;
#endif
#ifdef HAVE_LIBOPENJPEG2
		if(!CompressJPEG2Ko(*pDDO,qual)) return FALSE;
#endif
#else //HAVE_BOTH_J2KLIBS
		if(1)
		{
			if(!CompressJPEG2Ko(*pDDO,qual)) return FALSE;
		}
		else if(!CompressJPEG2K(*pDDO,qual)) return FALSE;
#endif
		*ActualMode = lFileCompressMode;
		CompressJpeg2000++;
        	return ( TRUE );
        	}
#else //HAVE_J2K
	if (lFileCompressMode=='K' || lFileCompressMode=='k' || lFileCompressMode=='L' || lFileCompressMode=='l')
		{
		OperatorConsole.printf("***[CompressJPEGImage]: jpeg2000 not supported\n");
         	return FALSE;
		}
#endif //HAVE_J2K

#ifdef HAVE_LIBCHARLS // JPEG LS stuff
        if (lFileCompressMode=='S' || lFileCompressMode=='s' || lFileCompressMode=='7')
        {
            if( lFileCompressMode=='S' || lFileCompressMode=='s') qual = 100;
            if(!CompressJPEGLS(*pDDO,qual)) return FALSE;
            *ActualMode = lFileCompressMode;
            CompressJpegLS++;
            return ( TRUE );
        }
#else //HAVE_LIBCHARLS
        if (lFileCompressMode=='S' || lFileCompressMode=='s' || lFileCompressMode=='7')
        {
            OperatorConsole.printf("***[CompressJPEGImage]: JPegLS not supported\n");
            return FALSE;
        }
#endif //HAVE_LIBCHARLS

#ifdef HAVE_LIBJPEG
        if(!UseBuiltInDecompressor())
#else
        if(1)
#endif
		{
		pVR = (*pDDO)->GetVR(0x0028, 0x0100);
		if (pVR) 
			{
			BOOL DecompressNon16BitsJpeg;
			UseBuiltInDecompressor(&DecompressNon16BitsJpeg);

			int BitsAllocated = *((UINT8 *)pVR->Data);
			if (BitsAllocated!=16 && !DecompressNon16BitsJpeg)
				{
				OperatorConsole.printf("[CompressJPEGImage]: JPEG compression only supported for 16 bits data\n");
	        		return FALSE;
				}
			}

		pVR = (*pDDO)->GetVR(0x0028, 0x0004);
		if (pVR) 
			{
				if (pVR->Length>=12 && (memcmp(pVR->Data, "YBR_FULL_422", 12)==0))
				{
				OperatorConsole.printf("[CompressJPEGImage]: JPEG compression not allowed for YBR_FULL_422\n");
	        		return FALSE;
				}
			}

		switch(lFileCompressMode)
			{
			case '1': strcpy(opts, "+e1 +un +sr"); break;	// default lossless SV1 "1.2.840.10008.1.2.4.70" = jpeg 14 (33%)
			case '2': strcpy(opts, "+el +un +sr"); break;	// 	   lossless SV6 "1.2.840.10008.1.2.4.57" = jpeg 14 (33%)
			case '3': strcpy(opts, "+eb +un +sr"); break;	// baseline   (8 bits ) "1.2.840.10008.1.2.4.50" = jpeg 1  (15%)
			case '4': strcpy(opts, "+ee +un +sr"); break;	// extended   (12 bits) "1.2.840.10008.1.2.4.51" = jpeg 4  (15%)
			case '5': strcpy(opts, "+es +un +sr"); break;	// spectral sel 	"1.2.840.10008.1.2.4.53" = jpeg 8  (15%)
			case '6': strcpy(opts, "+ep +un +sr"); break;	// progressive		"1.2.840.10008.1.2.4.55" = jpeg 12 (14%)

			default:strcpy(opts, "+e1 +un +sr"); break;	// default = lossless SV1
			}
	 
		pVR = (*pDDO)->GetVR(0x7fe0, 0x0010);
		if (pVR)
		pVR->TypeCode = 'OW';

		PDU.SaveDICOMDataObject(name, DICOM_CHAPTER_10_EXPLICIT, *pDDO);
		delete *pDDO;
		*pDDO = NULL;

#ifdef WIN32		
		if (!ExecHidden("dcmcjpeg.exe", opts, name, name2, ""))
#else
		if (!ExecHidden("dcmcjpeg", opts, name, name2, "DCMDICTPATH=dicom.dic\0"))
#endif
			{
			*pDDO = PDU.LoadDICOMDataObject(name);	// get back original image
			unlink(name);
			return FALSE;
			}

		*pDDO = PDU.LoadDICOMDataObject(name2);
		if(!*pDDO)
			{
			OperatorConsole.printf("***[CompressJPEGImage]: Error on load after: dcmcjpeg %s %s out.dcm)\n", opts, name);
			*pDDO = PDU.LoadDICOMDataObject(name);	// get back original image
			return FALSE;
			}

		unlink(name);
		unlink(name2);
		CompressJpeg++;

	        *ActualMode = lFileCompressMode;
		return ( TRUE );
		}

#ifdef HAVE_LIBJPEG
/* Jpeg standard is lossy = 8 or 12 bits. */
	if(lFileCompressMode > '2')//Lossy
        	{
       		int BitsStored = (*pDDO)->GetBYTE(0x0028, 0x0101);
        	if(BitsStored > 12)
            		{
                	//OperatorConsole.printf(
                    	//"Warn[CompressJPEGImage]: JPEG lossy compression does not support for %d bits data\n",
                    	//BitsAllocated);
            		//return FALSE;//Most likely a c-move, don't know if they can take lossless, so don't compress.

          		OperatorConsole.printf(
                	"Warn[CompressJPEGImage]: JPEG changed to lossless (J1) for %d bits data\n",
                	BitsStored);
            		lFileCompressMode = '1';//Most likely a store, we can take lossless, so change it.
  			}
        	}
    	if(!CompressJPEGL(*pDDO, lFileCompressMode, qual)) return FALSE;
	CompressJpeg++;
#endif //HAVE_LIBJPEG #else
        *ActualMode = lFileCompressMode;
	return ( TRUE );
	}

BOOL recompressFile(char *File, char *Compression, ExtendedPDU_Service *PDU)
	{
//	int 	status = 0;
	BOOL	rc = FALSE;

	if (stricmp(Compression, ""     )==0) return TRUE;
	if (memicmp(Compression, "as", 2)==0) return TRUE;
	if (stricmp(Compression, "is"   )==0) return TRUE;

	if (1)
	{	DICOMDataObject		*pDDO;
		PDU_Service		PDU2;

		PDU2.AttachRTC(&VRType);
		pDDO = PDU2.LoadDICOMDataObject(File);
		if(!pDDO)
			{
			OperatorConsole.printf("***[recompressFile] %s -FAILED: Error on Load\n", File);
			return FALSE;
			}

		if ((rc=recompress(&pDDO, Compression, "", Compression[0]=='n' || Compression[0]=='N', PDU)))
			SaveDICOMDataObject(File, pDDO);
		delete pDDO;
		}

	return rc;
	}
	
static void Strip2(DICOMDataObject *pDDO)
{ 	DICOMObject	DO2;
	VR		*vr;

	while((vr=pDDO->Pop()))
		{
	  	if (vr->Group==2) delete vr;
		else DO2.Push(vr);
		}

	pDDO->Reset();
	while((vr=DO2.Pop())) pDDO->Push(vr);
}

// use Filename to optionally forbid illegal compressions (e.g. nki->.dcm & jpeg->.v2)
BOOL recompress(DICOMDataObject **pDDO, const char *Compression, const char *Filename, BOOL StripGroup2, ExtendedPDU_Service *PDU)
	{
	int 	status = 0;
	BOOL	rc = FALSE, pass, all;
	UNUSED_ARGUMENT(Filename);

	// no strip group 2 for these options!
	if (stricmp(Compression, ""     )==0) return TRUE;
	if (memicmp(Compression, "as", 2)==0) return TRUE;
	if (stricmp(Compression, "is"   )==0) return TRUE;
	if (memicmp(Compression, "v" , 1)==0) return TRUE;

	int t = (int)time(NULL);

	if (memicmp(Compression, "uj",  2)==0) 
		{
		char s[64];
		VR *vr = (*pDDO)->GetVR(0x0002, 0x0010);
		if (vr && vr->Data)
			{
			strncpy(s, (char*)vr->Data, vr->Length); s[vr->Length] = 0;

			// leave JPEG as is
			if ((strcmp(s, "1.2.840.10008.1.2.4.70")==0) ||
#ifdef HAVE_J2K // JPEG 2000 stuff
			    (strcmp(s, "1.2.840.10008.1.2.4.90")==0) ||
			    (strcmp(s, "1.2.840.10008.1.2.4.91")==0) ||
#endif

#ifdef HAVE_LIBCHARLS
			    (strcmp(s, "1.2.840.10008.1.2.4.80")==0) ||
			    (strcmp(s, "1.2.840.10008.1.2.4.81")==0) ||
#endif

#if 0
			    (strcmp(s, "1.2.840.10008.1.2.4.100")==0) ||
			    (strcmp(s, "1.2.840.10008.1.2.4.102")==0) ||
			    (strcmp(s, "1.2.840.10008.1.2.4.103")==0) ||
#endif

			    (strcmp(s, "1.2.840.10008.1.2.4.57")==0) ||
			    (strcmp(s, "1.2.840.10008.1.2.4.50")==0) ||
			    (strcmp(s, "1.2.840.10008.1.2.4.51")==0) ||
			    (strcmp(s, "1.2.840.10008.1.2.4.53")==0) ||
		            (strcmp(s, "1.2.840.10008.1.2.4.55")==0))
			    	{
			    	if (StripGroup2) Strip2(*pDDO);
			    	return TRUE;
			    	}
			}
	
		rc = DecompressImage(pDDO, &status);
		}

	//added uncompressed LittleEndianImplicit, LittleEndianExplicit or BigEndianExplicit. bcb	    
	else if (*Compression == 'u' || *Compression == 'U' )
		{
		char s[64];
		VR *vr = (*pDDO)->GetVR(0x0002, 0x0010);
		if (vr && vr->Data)
			{
			strncpy(s, (char*)vr->Data, vr->Length); s[vr->Length] = 0;
			s[18] = '0';// Used for switch in LittleEndianImplicit.

			// skip recompression to original level, only LittleEndianImplicit is 17 long
			if (strncmp(s, "1.2.840.10008.1.2", 17)==0)
				{
				pass = FALSE;
				all = TRUE;// Stays true for all except default

				switch (s[18])
					{
					case '0': if (Compression[1]=='i'|| Compression[1]=='I') 
							pass = TRUE;
						break;
					case 0  : if (Compression[1]=='i'|| Compression[1]=='I') 
							pass = TRUE;
						break;
					case '1': if (Compression[1]=='l'|| Compression[1]=='L') 
							pass = TRUE;
						break;
					case '2': if (Compression[1]=='b'|| Compression[1]=='B') 
							pass = TRUE;
						break;
					default: all = FALSE;// RLE
						break;
					}
				// Test for "un"
				if (all && (Compression[1]=='n' || Compression[1]=='N')) pass = FALSE;

				if((*pDDO)->GetBYTE(0x0028, 0x0006)) pass=FALSE; // deplaning should happen 20101220

				VR *pVR = (*pDDO)->GetVR(0x0028, 0x0004);		 // depalette should happen 20101220
				if(pVR && pVR->Length == 14 && strncmp( (char *)pVR->Data, "PALETTE COLOR", 13) == 0) pass = FALSE;

				if (pass)
					{
					if (StripGroup2) Strip2(*pDDO);
					return TRUE;
					}
				}
			}
		if((rc = DecompressImage(pDDO, &status)))
			{
	// Tell "DCM to VR" what it is.  May want to set "un" to a specific type
			if(Compression[1]=='i'|| Compression[1] == 'I')
				(*pDDO)->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2\0", 'IU');
			if(Compression[1]=='l'|| Compression[1] == 'L')
				(*pDDO)->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.1\0", 'IU');
			if(Compression[1]=='b'|| Compression[1] == 'B')
				(*pDDO)->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.2\0", 'IU');
			}
		}

	else if (memicmp(Compression, "j",  1)==0) 
		{
		char s[64];
		VR *vr = (*pDDO)->GetVR(0x0002, 0x0010);
		if (vr && vr->Data)
			{
			strncpy(s, (char*)vr->Data, vr->Length); s[vr->Length] = 0;

			// skip recompression to original level
			if(strncmp(s,"1.2.840.10008.1.2.4.",20)==0)//Common to all jpeg
				{
				pass = FALSE;
				all = TRUE;// Stays true for all except default
				switch (atoi(s + 20))
					{
					case 50: if (Compression[1]=='3') pass = TRUE;
						break;
					case 51: if (Compression[1]=='4') pass = TRUE;
						break;
					case 53: if (Compression[1]=='5') pass = TRUE;
						break;
					case 55: if (Compression[1]=='6') pass = TRUE;
						break;
					case 57: if (Compression[1]=='2') pass = TRUE;
						break;
					case 70: if (Compression[1]=='1') pass = TRUE;
						break;
#ifdef HAVE_LIBCHARLS
					case 80: if (Compression[1]=='s' || Compression[1]=='S') pass = TRUE;
						break;
					case 81: if (Compression[1]=='7') pass = TRUE;
						break;
#endif

#ifdef HAVE_J2K // JPEG 2000 stuff
					case 90: if (Compression[1]=='k' || Compression[1]=='K') pass = TRUE; // !!!!!!!!
						break;
					case 91: if (Compression[1]=='l' || Compression[1]=='L') pass = TRUE;
						break;
#endif
					default: all = FALSE;// What is this, unknown jpeg.
						break;
					}
			// Test for "ja"
				if (all && (Compression[1]=='a' || Compression[1]=='A')) pass = TRUE;
				if (pass)
					{
					if (StripGroup2) Strip2(*pDDO);
					return TRUE;
					}
				}
/* The switch is easier to add ja (all)				
			if ((strcmp(s, "1.2.840.10008.1.2.4.70")==0 && Compression[1]=='1') ||
			    (strcmp(s, "1.2.840.10008.1.2.4.57")==0 && Compression[1]=='2') ||
			    (strcmp(s, "1.2.840.10008.1.2.4.50")==0 && Compression[1]=='3') ||
			    (strcmp(s, "1.2.840.10008.1.2.4.51")==0 && Compression[1]=='4') ||
			    (strcmp(s, "1.2.840.10008.1.2.4.53")==0 && Compression[1]=='5') ||
			    (strcmp(s, "1.2.840.10008.1.2.4.55")==0 && Compression[1]=='6')
#ifdef HAVE_J2K // JPEG 2000 stuff
			 || (strcmp(s, "1.2.840.10008.1.2.4.90")==0 && (Compression[1]=='k' || Compression[1]=='K')) ||
			    (strcmp(s, "1.2.840.10008.1.2.4.91")==0 && (Compression[1]=='l' || Compression[1]=='L'))
#endif
#ifdef HAVE_LIBCHARLS // JPEG LS stuff
			 || (strcmp(s, "1.2.840.10008.1.2.4.80")==0 && (Compression[1]=='s' || Compression[1]=='S')) ||
			    (strcmp(s, "1.2.840.10008.1.2.4.81")==0 && (Compression[1]=='7'))
#endif
			   )
			    	{
			    	if (StripGroup2) Strip2(*pDDO);
			    	return TRUE;
			    	}*/
				}
	
		int qual = 0; // default as configured in dicom.ini
		if (Compression[2]) qual = atoi(Compression+2);

		if (!DecompressImage(pDDO, &status)) return FALSE;
		rc = CompressJPEGImage(pDDO, Compression[1], &status, qual);
		if (strchr("3456lL", Compression[1]))
		  (*pDDO)->ChangeVR(0x0028, 0x2110, "01", 'CS'); // 20120624: set lossy compression
		}

	else if (memicmp(Compression, "nj",  2)==0) 
		{
		char s[64];
		VR *vr = (*pDDO)->GetVR(0x0002, 0x0010);
		if (vr && vr->Data)
			{
			strncpy(s, (char*)vr->Data, vr->Length); s[vr->Length] = 0;

			// leave JPEG as is
			if ((strcmp(s, "1.2.840.10008.1.2.4.70")==0) ||
#ifdef HAVE_J2K // JPEG 2000 stuff
			    (strcmp(s, "1.2.840.10008.1.2.4.90")==0) ||
			    (strcmp(s, "1.2.840.10008.1.2.4.91")==0) ||
#endif
#ifdef HAVE_LIBCHARLS
			    (strcmp(s, "1.2.840.10008.1.2.4.80")==0) ||
			    (strcmp(s, "1.2.840.10008.1.2.4.81")==0) ||
#endif
#if 0
			    (strcmp(s, "1.2.840.10008.1.2.4.100")==0) ||
			    (strcmp(s, "1.2.840.10008.1.2.4.102")==0) ||
			    (strcmp(s, "1.2.840.10008.1.2.4.103")==0) ||
#endif			    

			    (strcmp(s, "1.2.840.10008.1.2.4.57")==0) ||
			    (strcmp(s, "1.2.840.10008.1.2.4.50")==0) ||
			    (strcmp(s, "1.2.840.10008.1.2.4.51")==0) ||
			    (strcmp(s, "1.2.840.10008.1.2.4.53")==0) ||
		            (strcmp(s, "1.2.840.10008.1.2.4.55")==0))
			    	{
			    	if (StripGroup2) Strip2(*pDDO);
			    	return TRUE;
			    	}
			}
	
		if (!DecompressImage(pDDO, &status)) return FALSE;
		rc = CompressNKI(*pDDO, MAXNKICOMPRESSION);
		status = rc;
		}

	else if (memicmp(Compression, "n",  1)==0) 
		{
		// skip recompression to original level
		signed char *p;
		VR *vr = (*pDDO)->GetVR(0x7fdf, 0x0010);
		if (vr)
			{
    			p = ((signed char *)(vr->Data));
		    	if (p && get_nki_private_compress_mode(p)==Compression[1]-'0')
			    	{
			    	if (StripGroup2) Strip2(*pDDO);
			    	return TRUE;
			    	}
			}
    
		if (!DecompressImage(pDDO, &status)) return FALSE;

		if (Compression[1]=='0') rc = TRUE;
		else                     
			{
			rc = CompressNKI(*pDDO, Compression[1]-'0');
			status = rc;
			}
		}

	else if (memicmp(Compression, "k",  1)==0) 
		{
		int size;

		switch (Compression[1])
			{ 
			case '1': size = 1024; break;
			case '2': size =  512; break;
			case '4': size =  256; break;
			case '8': size =  128; break;
			case 'A': case 'a': size =  64; break;
			case 'B': case 'b': size =  32; break;
			case 'C': case 'c': size =  16; break;
                	default : size =65536; break;
			}

		if (TestDownsize(*pDDO, NULL, size))
			{
			if (!DecompressImage(pDDO, &status)) return FALSE;

			switch (Compression[1])
				{
				case '1': rc = MaybeDownsize(*pDDO, NULL, 1024); break;
				case '2': rc = MaybeDownsize(*pDDO, NULL, 512); break;
				case '4': rc = MaybeDownsize(*pDDO, NULL, 256); break;
				case '8': rc = MaybeDownsize(*pDDO, NULL, 128); break;
				case 'A': case 'a': rc = MaybeDownsize(*pDDO, NULL, 64); break;
				case 'B': case 'b': rc = MaybeDownsize(*pDDO, NULL, 32); break;
				case 'C': case 'c': rc = MaybeDownsize(*pDDO, NULL, 16); break;
				}
			if (rc==2) status = 1;
			}
		else
			status = 1;
		}

	else if (memicmp(Compression, "s",  1)==0) 
		{
		//CallImportConverterN(*pDDO, 1400 + Compression[1] - '0', NULL, NULL, NULL, NULL, "calling", "called", NULL, NULL, VariableVRs);
		CallImportConverterN(NULL, *pDDO, 1400 + Compression[1] - '0', NULL, NULL, NULL, NULL, PDU, NULL, NULL);
		rc = status = 1;
		}

	if (StripGroup2) Strip2(*pDDO);

        //if (rc && status>0)
        //  OperatorConsole.printf("[recompress]: recompressed with mode = %s (strip=%d)\n", Compression, StripGroup2);

	RecompressTime += (int)time(NULL)-t;

	return rc;
	}

///////////////////////////////////////////////////////////////////////////////////////////////
// PDU with dgate compression styles
///////////////////////////////////////////////////////////////////////////////////////////////

BOOL
ExtendedPDU_Service	::	AddTransferSyntaxs(PresentationContext &PresContext)
	{
	UID		uid;
	TransferSyntax	TrnSyntax;

	// as-is not correctly implemented (hard to do, since each image will require other compression)
	// will now tranmsit over ImplicitLittleEndian - OK for NKI clients

	if (memicmp(RequestedCompressionType, "as", 2)==0 || memicmp(RequestedCompressionType, "is", 2)==0)
		{
		OperatorConsole.printf("Warning: use of as is compression for outgoing associations not fully implemented\n");
		}
	// Added uncompressed LittleEndianExplicit. bcb
	if (memicmp(RequestedCompressionType, "ul", 2)==0)
	    {
	    uid.Set("1.2.840.10008.1.2.1"); TrnSyntax.Set(uid);
	    PresContext.AddTransferSyntax(TrnSyntax);
	    }
	// Added uncompressed BigEndianExplicit. bcb
	if (memicmp(RequestedCompressionType, "ub", 2)==0)
	    {
	    uid.Set("1.2.840.10008.1.2.2"); TrnSyntax.Set(uid);
	    PresContext.AddTransferSyntax(TrnSyntax);
	    }
	// Added uncompressed LittleEndianExplicit or BigEndianExplicit. bcb
	if (memicmp(RequestedCompressionType, "ue", 2)==0)
	    {
	    uid.Set("1.2.840.10008.1.2.1"); TrnSyntax.Set(uid);
	    PresContext.AddTransferSyntax(TrnSyntax);
	    uid.Set("1.2.840.10008.1.2.2"); TrnSyntax.Set(uid);
	    PresContext.AddTransferSyntax(TrnSyntax);
	    }
			
	// only jpeg requires other tranmission syntaxes

	if (RequestedCompressionType[0]=='j' || RequestedCompressionType[0]=='J')
		{
		switch(RequestedCompressionType[1])
			{
/*			case '1': // lossless
				  uid.Set("1.2.840.10008.1.2.4.70"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  break;
*/				 
			// bcb: should not j1 & J2 offer both types of lossless.
			case '1': // lossless
			case '2': // losless sv 6
				  uid.Set("1.2.840.10008.1.2.4.57"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  uid.Set("1.2.840.10008.1.2.4.70"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  break;

			case '3': // lossy baseline (8bit) + extended(12 bit)
			case '4': uid.Set("1.2.840.10008.1.2.4.51"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  uid.Set("1.2.840.10008.1.2.4.50"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  break;

			case '5': // spectral selection
				  uid.Set("1.2.840.10008.1.2.4.53"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  uid.Set("1.2.840.10008.1.2.4.51"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  uid.Set("1.2.840.10008.1.2.4.50"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  break;

			case '6': // progressive
				  uid.Set("1.2.840.10008.1.2.4.55"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  uid.Set("1.2.840.10008.1.2.4.51"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  uid.Set("1.2.840.10008.1.2.4.50"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  break;

#ifdef HAVE_J2K // JPEG 2000 stuff
			case 'K':
			case 'k': // 2K lossless only
				  uid.Set("1.2.840.10008.1.2.4.90"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  break;

			case 'L':
			case 'l': // 2K lossy or lossless
				  uid.Set("1.2.840.10008.1.2.4.91"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  uid.Set("1.2.840.10008.1.2.4.90"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  break;
#endif
#ifdef HAVE_LIBCHARLS // JPEG LS stuff
			case 'S':
			case 's': // LS lossless only
				  uid.Set("1.2.840.10008.1.2.4.80"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  break;
			case '7': // LS lossy or lossless
				  uid.Set("1.2.840.10008.1.2.4.80"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  uid.Set("1.2.840.10008.1.2.4.81"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  break;
#endif

			default:  // lossless
			          // bcb added .57 here
				  uid.Set("1.2.840.10008.1.2.4.57"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  uid.Set("1.2.840.10008.1.2.4.70"); TrnSyntax.Set(uid);
				  PresContext.AddTransferSyntax(TrnSyntax);
				  break;
			}
		}

#if 0
	// WIP support UJ outgoing
	if ((RequestedCompressionType[0]=='u' || RequestedCompressionType[0]=='U') &&
	    (RequestedCompressionType[1]=='j' || RequestedCompressionType[1]=='J'))
		{
		uid.Set("1.2.840.10008.1.2.4.50"); TrnSyntax.Set(uid);
		PresContext.AddTransferSyntax(TrnSyntax);
		uid.Set("1.2.840.10008.1.2.4.51"); TrnSyntax.Set(uid);
		PresContext.AddTransferSyntax(TrnSyntax);
		uid.Set("1.2.840.10008.1.2.4.53"); TrnSyntax.Set(uid);
		PresContext.AddTransferSyntax(TrnSyntax);
		uid.Set("1.2.840.10008.1.2.4.55"); TrnSyntax.Set(uid);
		PresContext.AddTransferSyntax(TrnSyntax);
		uid.Set("1.2.840.10008.1.2.4.57"); TrnSyntax.Set(uid);
		PresContext.AddTransferSyntax(TrnSyntax);
		uid.Set("1.2.840.10008.1.2.4.70"); TrnSyntax.Set(uid);
		PresContext.AddTransferSyntax(TrnSyntax);
		uid.Set("1.2.840.10008.1.2.4.90"); TrnSyntax.Set(uid);
		PresContext.AddTransferSyntax(TrnSyntax);
		uid.Set("1.2.840.10008.1.2.4.91"); TrnSyntax.Set(uid);
		PresContext.AddTransferSyntax(TrnSyntax);
		uid.Set("1.2.840.10008.1.2.4.80"); TrnSyntax.Set(uid);
		PresContext.AddTransferSyntax(TrnSyntax);
		uid.Set("1.2.840.10008.1.2.4.81"); TrnSyntax.Set(uid);
		PresContext.AddTransferSyntax(TrnSyntax);
		uid.Set("1.2.840.10008.1.2.4.100"); TrnSyntax.Set(uid);
		PresContext.AddTransferSyntax(TrnSyntax);
		uid.Set("1.2.840.10008.1.2.4.102"); TrnSyntax.Set(uid);
		PresContext.AddTransferSyntax(TrnSyntax);
		uid.Set("1.2.840.10008.1.2.4.103"); TrnSyntax.Set(uid);
		PresContext.AddTransferSyntax(TrnSyntax);
		}
#endif

        if (strchr(RequestedCompressionType, '!')==NULL)
        { uid.Set("1.2.840.10008.1.2");		// ImplicitLittleEndian
	  TrnSyntax.Set(uid);
	  PresContext.AddTransferSyntax(TrnSyntax);
        }

	return ( TRUE );
	}

BOOL
ExtendedPDU_Service	::	SetRequestedCompressionType(const char *type)
	{
	strcpy(RequestedCompressionType, type);
	return ( TRUE );
	}

char*
ExtendedPDU_Service	::	GetAcceptedCompressionType(UID uid)
	{
	UID TrnSyntaxUID;
	int ContextID = GetPresentationContextID(uid);
	char *p;

	if ( !ContextID )
		{
		strcpy(AcceptedCompressionType, "");
		return ( AcceptedCompressionType );
		}

	GetTransferSyntaxUID(ContextID, TrnSyntaxUID);

	if (RequestedCompressionType[0]!='j' && RequestedCompressionType[0]!='J')
		{
		strcpy(AcceptedCompressionType, RequestedCompressionType);
		return ( AcceptedCompressionType );
		}

	p = (char *)TrnSyntaxUID.GetBuffer(1);

	     if (strcmp(p, "1.2.840.10008.1.2.4.70")==0) strcpy(AcceptedCompressionType, "j1");
	else if (strcmp(p, "1.2.840.10008.1.2.4.57")==0) strcpy(AcceptedCompressionType, "j2");
	else if (strcmp(p, "1.2.840.10008.1.2.4.50")==0) strcpy(AcceptedCompressionType, "j3");
	else if (strcmp(p, "1.2.840.10008.1.2.4.51")==0) strcpy(AcceptedCompressionType, "j4");
	else if (strcmp(p, "1.2.840.10008.1.2.4.53")==0) strcpy(AcceptedCompressionType, "j5");
	else if (strcmp(p, "1.2.840.10008.1.2.4.55")==0) strcpy(AcceptedCompressionType, "j6");
#ifdef HAVE_J2K // JPEG 2000 stuff
	else if (strcmp(p, "1.2.840.10008.1.2.4.90")==0) strcpy(AcceptedCompressionType, "jk");
	else if (strcmp(p, "1.2.840.10008.1.2.4.91")==0) strcpy(AcceptedCompressionType, "jl");
#endif
#ifdef HAVE_LIBCHARLS // JPEG LS stuff
        else if (strcmp(p, "1.2.840.10008.1.2.4.80")==0) strcpy(AcceptedCompressionType, "js");
        else if (strcmp(p, "1.2.840.10008.1.2.4.81")==0) strcpy(AcceptedCompressionType, "j7");
#endif
	// Added uncompressed LittleEndianImplicit, LittleEndianExplicit or BigEndianExplicit. bcb
	else if (strcmp(p, "1.2.840.10008.1.2.1")==0) strcpy(AcceptedCompressionType, "ul");
	else if (strcmp(p, "1.2.840.10008.1.2.2")==0) strcpy(AcceptedCompressionType, "ub");
	else if (strcmp(p, "1.2.840.10008.1.2")==0) strcpy(AcceptedCompressionType, "ui");
	else strcpy(AcceptedCompressionType, "un");

	OperatorConsole.printf("Accepted compression: %s\n", AcceptedCompressionType);


	return ( AcceptedCompressionType );
	}


////////////////////////////////////////////////////////////////////////////////////////////

/* Downsize and convert image to 8 bit monochrome or RGB */

#include "uncompconv.hpp"

static BOOL To8bitMonochromeOrRGB(DICOMDataObject* pDDO, int size, int *Dimx, int *Dimy, unsigned char **out, int RGBout=0, int level=0, int window=0, unsigned int frame=0, float gamma=1)
{ int			i, j, dum;
  VR*			pVR;
  unsigned int		iMaxRowsColumns;
  unsigned short	iRows, iColumns, iNewRows, iNewColumns, iDownsizeFactor;
  unsigned int		iImageSize, iNbBytesPerPixel, iNbPixels, iSkip;
  char*			pcSrc;
  unsigned char*	pcDest;
  short*		psSrc;
//  int*			piSrc;//Anything that points to VR->Data should be the same size on all systems. BCB
  INT32*        piSrc;
  int			r, g, b;
  int                   pixeloffset = 0;
  unsigned char lut[256];
  for (i=0; i<256; i++) lut[i]=i;
  if (gamma!=1)
    for (i=0; i<256; i++) lut[i]=(int)(255.0*pow((float)i/255, gamma)+0.5);

  ExtractFrame(pDDO, frame);

  if (size==0) iMaxRowsColumns = 16384;
  else         iMaxRowsColumns = size;

  /* OK. Now check whether all necessary PixelData-info is present in pDDO */
  iRows = pDDO->GetUINT16(0x0028, 0x0010);
  iColumns = pDDO->GetUINT16(0x0028, 0x0011);
  iNbPixels = iRows * iColumns;

  if (!iNbPixels) 			/* Image must make sense */
    return FALSE;

  pVR = pDDO->GetVR(0x0028, 0x1052);	/* RescaleIntercept */
  if ( pVR) 
  { int intercept = pVR->Getatoi();
    float slope = 1;
    pVR = pDDO->GetVR(0x0028, 0x1053);	/* RescaleSlope */
    if (pVR)
    { char s[64];
      memcpy(s, pVR->Data, pVR->Length);
      s[pVR->Length]=0;
      slope = atof(s);
    }
    if (window==0 && slope>0.999 && slope<1.001)
    { pVR = pDDO->GetVR(0x0008, 0x0060);	/* Modality */
      if (pVR && pVR->Length==2 && memcmp(pVR->Data, "CT", 2)==0)
        pixeloffset = intercept + 1024;
    }
    else if (slope>0.999 && slope<1.001)
    { level -= intercept;
    }
  }
  //pVR = pDDO->GetVR(0x0028, 0x1053);	/* RescaleSlope */
  //if ( pVR) 
  //{ if (window && atof((const char *)pVR->Data)!=0)
  //    window /= atof((const char *)pVR->Data);
  //}
  
  if (iRows >= iColumns)
  { iDownsizeFactor = iRows / iMaxRowsColumns;
    if (iRows % iMaxRowsColumns)
      iDownsizeFactor ++;
  }
  else
  { iDownsizeFactor = iColumns / iMaxRowsColumns;
    if (iColumns % iMaxRowsColumns)
      iDownsizeFactor ++;
  }

  if (iDownsizeFactor<1) iDownsizeFactor=1;

  if (!DecompressImage(&pDDO, &dum))	/* make sure it is not NKI or JPEG compressed */
    return FALSE;

  pVR = pDDO->GetVR(0x7fe0, 0x0010);	/* Pixeldata */
  if (!pVR)
    return FALSE;

  int newLength=0;
  int bitsAllocated = pDDO->GetUINT16(0x0028, 0x0100); // allow < 8 bits data
  if (bitsAllocated<8)
  { if ((newLength = To8BitGray((unsigned char **)&(pVR->Data), pVR->Length, bitsAllocated , TRUE)) == 0)
       return FALSE;
    pVR-> Length = newLength;
  }
  if (bitsAllocated>8 && bitsAllocated<16)
  { if ((newLength = To16BitGray((unsigned char **)&(pVR->Data), pVR->Length, bitsAllocated , TRUE)) == 0)
       return FALSE;
    pVR-> Length = newLength;
  }

  iImageSize = pVR->Length;

  if ((iImageSize < iNbPixels) ||
      (iImageSize % iNbPixels > 1))	/* allow 1 byte padding */
    return FALSE;			/* Image doesn't make sense */

  iNbBytesPerPixel = iImageSize / iNbPixels;
  if (iNbBytesPerPixel > 4)
    return FALSE;			/* Image is too beautiful to convert! */

  /* LETS DO IT !
     The downsized image is stored in the same memory-block as the original one:
     Just be sure to adjust the length-indicator. The image will be (re)compressed anyway
     later on.
  */

  iNewRows    = iRows / iDownsizeFactor;
  if (!iNewRows)
    iNewRows++;
  iNewColumns = iColumns / iDownsizeFactor;
  if (!iNewColumns)
    iNewColumns++;

  if (RGBout & (iNewColumns&3)!=0) 
    iNewColumns &= 0xfffffffc;	// make multiple of 4 (truncate some columns if needed)

  *Dimy = iNewRows;
  *Dimx = iNewColumns;

  *out = (unsigned char *)malloc(iNewRows * iNewColumns * (RGBout?3:1));
  if (*out==NULL) return FALSE;
  pcDest = *out;

  switch (iNbBytesPerPixel + 10 * RGBout)
  { case 1:
      pcSrc  = (char*)pVR->Data;
      for (i=0; i<iNewRows; i++)
      { for (j=0; j<iNewColumns; j++)
	{ *pcDest++ = *pcSrc;
	  pcSrc += iDownsizeFactor;
	}
	pcSrc = (char*)pVR->Data + iDownsizeFactor * iColumns * (i + 1);
      }
      break;

    case 2:
    { int max=0;
      psSrc  = (short*)pVR->Data;

      if (window)
      { for (i=0; i<iNewRows; i++)
        { for (j=0; j<iNewColumns; j++)
          {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
            r = *psSrc + pixeloffset;
#else //Big Endian like Apple power pc
            r = SwitchEndian(*psSrc) + pixeloffset;
#endif
            r = 255 * (r - level + window/2) / window;
            if (r>255) r=255;
            if (r<0)   r=0;
            *pcDest++ = lut[r];
	    psSrc += iDownsizeFactor;
  	  }
	  psSrc = (short*)pVR->Data + iDownsizeFactor * iColumns * (i + 1);
        }
      }
      else
      { for (i=0; i<iNewRows; i++)
        { for (j=0; j<iNewColumns; j++)
  	  { 
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
            r = *psSrc + pixeloffset;
#else //Big Endian like Apple power pc
            r = SwitchEndian(*psSrc) + pixeloffset;
#endif
            if (r > max) max=r;
            if (r>2047) r=2047;
            if (r<0)    r=0;
            r = r/8;
            *pcDest++ = lut[r];
  	    psSrc += iDownsizeFactor;
  	  }
  	  psSrc = (short*)pVR->Data + iDownsizeFactor * iColumns * (i + 1);
        }
  
        // auto scale if too dark or too bright
  
        if (max < 1024 || max > 4095)
        { int factor = (int)((max / 256.0) + 0.5);
          if (factor==0) factor=1;
  
          pcDest = *out;
          psSrc  = (short*)pVR->Data;
          for (i=0; i<iNewRows; i++)
          { for (j=0; j<iNewColumns; j++)
            {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
              r = *psSrc + pixeloffset;
#else //Big Endian like Apple power pc
              r = SwitchEndian(*psSrc) + pixeloffset;
#endif
              r = r/factor;
              if (r>255) r=255;
              if (r<0)   r=0;
              *pcDest++ = lut[r];
  	      psSrc += iDownsizeFactor;
    	    }
    	    psSrc = (short*)pVR->Data + iDownsizeFactor * iColumns * (i + 1);
          }
        }
      }

      break;
    }

    case 3:
      pcSrc  = (char*)pVR->Data;
      iSkip = (iDownsizeFactor - 1) * 3;
      for (i=0; i<iNewRows; i++)
      { for (j=0; j<iNewColumns; j++)
	{ r = lut[*(unsigned char *)pcSrc++];
	  g = lut[*(unsigned char *)pcSrc++];
	  b = lut[*(unsigned char *)pcSrc++];
          *pcDest++ = (r+g+b)/3;
	  pcSrc += iSkip;
	}
	pcSrc = (char*)pVR->Data + iDownsizeFactor * 3 * iColumns * (i + 1);
      }
      break;

    case 4:
      piSrc  = (INT32*)pVR->Data;//Changed from (int*)
      if (window)
      { for (i=0; i<iNewRows; i++)
        { for (j=0; j<iNewColumns; j++)
          {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
            r = *piSrc + pixeloffset;
#else //Big Endian like Apple power pc
            r = SwitchEndian(*piSrc) + pixeloffset;
#endif
            r = 255 * (r - level + window/2) / window;
            if (r>255) r=255;
            if (r<0)   r=0;
            *pcDest++ = lut[r];
	    piSrc += iDownsizeFactor;
	  }
	  piSrc = (INT32*)pVR->Data + iDownsizeFactor * iColumns * (i + 1);// was (int*)
        }
      }
      else
      { int amax=1;
        for (i=0; i<iNewRows; i++)
        { piSrc = (INT32*)pVR->Data + iDownsizeFactor * iColumns * (iNewRows-1-i);//Was (int*)
          for (j=0; j<iNewColumns; j++)
          {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
            if (*piSrc>amax) amax = *piSrc;
#else //Big Endian like Apple power pc
            if (SwitchEndian(*piSrc)>amax) amax = *piSrc;
#endif
	    piSrc += iDownsizeFactor;
	  }
        }
        
	for (i=0; i<iNewRows; i++)
        { for (j=0; j<iNewColumns; j++)
          {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
            *pcDest++ = lut[255*(*piSrc + pixeloffset) / amax];
#else //Big Endian like Apple power pc
            *pcDest++ = lut[255*(SwitchEndian(*piSrc) + pixeloffset) / amax];
#endif
    	    piSrc += iDownsizeFactor;
	  }
	  piSrc = (INT32*)pVR->Data + iDownsizeFactor * iColumns * (i + 1);//Was(int*)
        }
      }
      break;

    case 11:
      for (i=0; i<iNewRows; i++)
      { pcSrc = (char*)pVR->Data + iDownsizeFactor * iColumns * (iNewRows-1-i);
        for (j=0; j<iNewColumns; j++)
	{ *pcDest++ = lut[*(unsigned char *)pcSrc];
	  *pcDest++ = lut[*(unsigned char *)pcSrc];
	  *pcDest++ = lut[*(unsigned char *)pcSrc];
	  pcSrc += iDownsizeFactor;
	}
      }
      break;

    case 12:
    { int max=0;
      if (window)
      { for (i=0; i<iNewRows; i++)
        { psSrc = (short*)pVR->Data + iDownsizeFactor * iColumns * (iNewRows-1-i);
          for (j=0; j<iNewColumns; j++)
            {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
            r = *psSrc + pixeloffset;
#else //Big Endian like Apple power pc
            r = SwitchEndian(*psSrc) + pixeloffset;
#endif
            r = 255 * (r - level + window/2) / window;
            if (r>255) r=255;
            if (r<0)   r=0;
            *pcDest++ = lut[r];
            *pcDest++ = lut[r];
            *pcDest++ = lut[r];
	    psSrc += iDownsizeFactor;
  	  }
        }
      }
      else
      { for (i=0; i<iNewRows; i++)
        { psSrc = (short*)pVR->Data + iDownsizeFactor * iColumns * (iNewRows-1-i);
          for (j=0; j<iNewColumns; j++)
            {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
            r = *psSrc + pixeloffset;
#else //Big Endian like Apple power pc
            r = SwitchEndian(*psSrc) + pixeloffset;
#endif
            if (r > max) max=r;
            if (r>2047) r=2047;
            if (r<0)    r=0;
            r = r/8;
            *pcDest++ = lut[r];
            *pcDest++ = lut[r];
            *pcDest++ = lut[r];
  	    psSrc += iDownsizeFactor;
  	  }
        }
  
        // auto scale if too dark or too bright
  
        if (max < 1024 || max > 4095)
        { int factor = (int)((max / 256.0) + 0.5);
          if (factor==0) factor=1;
  
          pcDest = *out;
          for (i=0; i<iNewRows; i++)
          { psSrc = (short*)pVR->Data + iDownsizeFactor * iColumns * (iNewRows-1-i);
            for (j=0; j<iNewColumns; j++)
              {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
              r = *psSrc + pixeloffset;
#else //Big Endian like Apple power pc
              r = SwitchEndian(*psSrc) + pixeloffset;
#endif
              r = r/factor;
              if (r>255) r=255;
              if (r<0)   r=0;
              *pcDest++ = lut[r];
              *pcDest++ = lut[r];
              *pcDest++ = lut[r];
  	      psSrc += iDownsizeFactor;
    	    }
          }
        }
      }

      break;
    }

    case 13:
      iSkip = (iDownsizeFactor - 1) * 3;
      for (i=0; i<iNewRows; i++)
      { pcSrc = (char*)pVR->Data + iDownsizeFactor * 3 * iColumns * (iNewRows-1-i);
        for (j=0; j<iNewColumns; j++)
	{ *pcDest++ = lut[*(unsigned char *)pcSrc++];
	  *pcDest++ = lut[*(unsigned char *)pcSrc++];
	  *pcDest++ = lut[*(unsigned char *)pcSrc++];
	  pcSrc += iSkip;
	}
      }
      break;

    case 14:
      if (window)
      { for (i=0; i<iNewRows; i++)
        { piSrc = (INT32*)pVR->Data + iDownsizeFactor * iColumns * (iNewRows-1-i);//Was (int*)
          for (j=0; j<iNewColumns; j++)
           {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
            r = *piSrc + pixeloffset;
#else //Big Endian like Apple power pc
            r = SwitchEndian(*piSrc) + pixeloffset;
#endif
            r = 255 * (r - level + window/2) / window;
            if (r>255) r=255;
            if (r<0)   r=0;
            *pcDest++ = lut[r];
  	    *pcDest++ = lut[r];
	    *pcDest++ = lut[r];
	    piSrc += iDownsizeFactor;
	  }
        }
      }
      else
      { int amax=1;
        for (i=0; i<iNewRows; i++)
        { piSrc = (INT32*)pVR->Data + iDownsizeFactor * iColumns * (iNewRows-1-i);//Was (int*)
          for (j=0; j<iNewColumns; j++)
          {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
            if (*piSrc>amax) amax = *piSrc;
#else //Big Endian like Apple power pc
            if (SwitchEndian(*piSrc)>amax) amax = *piSrc;
#endif
	    piSrc += iDownsizeFactor;
	  }
        }

	for (i=0; i<iNewRows; i++)
        { piSrc = (INT32*)pVR->Data + iDownsizeFactor * iColumns * (iNewRows-1-i);//Was (int*)
          for (j=0; j<iNewColumns; j++)
          {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
            *pcDest++ = lut[255*(*piSrc + pixeloffset) / amax];
  	    *pcDest++ = lut[255*(*piSrc + pixeloffset) / amax];
	    *pcDest++ = lut[255*(*piSrc + pixeloffset) / amax];
#else //Big Endian like Apple power pc
            *pcDest++ = lut[255*(SwitchEndian(*piSrc) + pixeloffset) / amax];
  	    *pcDest++ = lut[255*(SwitchEndian(*piSrc) + pixeloffset) / amax];
	    *pcDest++ = lut[255*(SwitchEndian(*piSrc) + pixeloffset) / amax];
#endif
	    piSrc += iDownsizeFactor;
	  }
        }
      }
      break;

    case 21:
      for (i=0; i<iNewRows; i++)
      { pcSrc = (char*)pVR->Data + iDownsizeFactor * iColumns * i;
        for (j=0; j<iNewColumns; j++)
	{ *pcDest++ = lut[*(unsigned char *)pcSrc];
	  *pcDest++ = lut[*(unsigned char *)pcSrc];
	  *pcDest++ = lut[*(unsigned char *)pcSrc];
	  pcSrc += iDownsizeFactor;
	}
      }
      break;

    case 22:
    { int max=0;
      if (window)
      { for (i=0; i<iNewRows; i++)
        { psSrc = (short*)pVR->Data + iDownsizeFactor * iColumns * i;
          for (j=0; j<iNewColumns; j++)
          {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
            r = *psSrc + pixeloffset;
#else //Big Endian like Apple power pc
            r = SwitchEndian(*psSrc) + pixeloffset;
#endif
            r = 255 * (r - level + window/2) / window;
            if (r>255) r=255;
            if (r<0)   r=0;
            *pcDest++ = lut[r];
            *pcDest++ = lut[r];
            *pcDest++ = lut[r];
	    psSrc += iDownsizeFactor;
  	  }
        }
      }
      else
      { for (i=0; i<iNewRows; i++)
        { psSrc = (short*)pVR->Data + iDownsizeFactor * iColumns * i;
          for (j=0; j<iNewColumns; j++)
          {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
            r = *psSrc + pixeloffset;
#else //Big Endian like Apple power pc
            r = SwitchEndian(*psSrc) + pixeloffset;
#endif
            if (r > max) max=r;
            if (r>2047) r=2047;
            if (r<0)    r=0;
            r = r/8;
            *pcDest++ = lut[r];
            *pcDest++ = lut[r];
            *pcDest++ = lut[r];
  	    psSrc += iDownsizeFactor;
  	  }
        }
  
        // auto scale if too dark or too bright
  
        if (max < 1024 || max > 4095)
        { int factor = (int)((max / 256.0) + 0.5);
          if (factor==0) factor=1;
  
          pcDest = *out;
          for (i=0; i<iNewRows; i++)
          { psSrc = (short*)pVR->Data + iDownsizeFactor * iColumns * i;
            for (j=0; j<iNewColumns; j++)
            {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
              r = *psSrc + pixeloffset;
#else //Big Endian like Apple power pc
              r = SwitchEndian(*psSrc) + pixeloffset;
#endif
              r = r/factor;
              if (r>255) r=255;
              if (r<0)   r=0;
              *pcDest++ = lut[r];
              *pcDest++ = lut[r];
              *pcDest++ = lut[r];
  	      psSrc += iDownsizeFactor;
    	    }
          }
        }
      }

      break;
    }

    case 23:
      iSkip = (iDownsizeFactor - 1) * 3;
      for (i=0; i<iNewRows; i++)
      { pcSrc = (char*)pVR->Data + iDownsizeFactor * 3 * iColumns * i;
        for (j=0; j<iNewColumns; j++)
	{ *pcDest++ = lut[*(unsigned char *)pcSrc++];
	  *pcDest++ = lut[*(unsigned char *)pcSrc++];
	  *pcDest++ = lut[*(unsigned char *)pcSrc++];
	  pcSrc += iSkip;
	}
      }
      break;

    case 24:
      if (window)
      { for (i=0; i<iNewRows; i++)
        { piSrc = (INT32*)pVR->Data + iDownsizeFactor * iColumns * i;//Was (int*)
          for (j=0; j<iNewColumns; j++)
          {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
            r = *piSrc + pixeloffset;
#else //Big Endian like Apple power pc
            r = SwitchEndian(*piSrc) + pixeloffset;
#endif
            r = 255 * (r - level + window/2) / window;
            if (r>255) r=255;
            if (r<0)   r=0;
            *pcDest++ = lut[r];
  	    *pcDest++ = lut[r];
	    *pcDest++ = lut[r];
	    piSrc += iDownsizeFactor;
	  }
        }
      }
      else
      { int amax=1;
        for (i=0; i<iNewRows; i++)
        { piSrc = (INT32*)pVR->Data + iDownsizeFactor * iColumns * (iNewRows-1-i);//Was (int*)
          for (j=0; j<iNewColumns; j++)
          {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
            if (*piSrc>amax) amax = *piSrc;
#else //Big Endian like Apple power pc
            if (SwitchEndian(*piSrc)>amax) amax = *piSrc;
#endif
	    piSrc += iDownsizeFactor;
	  }
        }
        
	for (i=0; i<iNewRows; i++)
        { piSrc = (INT32*)pVR->Data + iDownsizeFactor * iColumns * (iNewRows-1-i);//Was (int*)
          for (j=0; j<iNewColumns; j++)
          {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
            *pcDest++ = lut[255*(*piSrc + pixeloffset) / amax];
  	    *pcDest++ = lut[255*(*piSrc + pixeloffset) / amax];
	    *pcDest++ = lut[255*(*piSrc + pixeloffset) / amax];
#else //Big Endian like Apple power pc
            *pcDest++ = lut[255*(SwitchEndian(*piSrc) + pixeloffset) / amax];
  	    *pcDest++ = lut[255*(SwitchEndian(*piSrc) + pixeloffset) / amax];
	    *pcDest++ = lut[255*(SwitchEndian(*piSrc) + pixeloffset) / amax];
#endif
	    piSrc += iDownsizeFactor;
	  }
        }
      }
      break;
  }

  return TRUE;
}

int WriteGIF(FILE *fp, unsigned char *pic, int ptype, int w, int h, 
                       unsigned char *rmap, unsigned char *gmap, unsigned char *bmap,
                       int numcols, int colorstyle, const char *comment);

#ifndef hasColorStyleType
#define hasColorStyleType 1
enum ColorStyleType {csGlobalPalette, csLocalPalette};
#endif

int WriteGIFHeader(FILE *fp, int w, int h, BYTE *rmap, BYTE *gmap, BYTE *bmap,
             int numcols, ColorStyleType colorstyle, char *comment);

int WriteGIFFrame(FILE *fp, BYTE *p, int w, int h,
                  BYTE *rmap, BYTE *gmap, BYTE *bmap,
                  int numcols,
                  int frames, int time, ColorStyleType colorstyle);

BOOL ToGif(DICOMDataObject* pDDO, char *filename, int size, int append, int level, int window, unsigned int frame, float gamma)
{ int dimx, dimy;
  unsigned char *out;
  FILE *f;
  unsigned char lut[256];
  int i;
  int Ratefps;

  for(i=0; i<256; i++) lut[i]=i;

  if (append) f = fopen(filename, "ab");
  else        f = fopen(filename, "wb");
  if (!f)
    return FALSE;

  // create animated GIF here
  if (frame>10000 && GetNumberOfFrames(pDDO)>1)
  { DICOMDataObject *DDO2 = MakeCopy(pDDO);
    To8bitMonochromeOrRGB(DDO2, size, &dimx, &dimy, &out, 0, level, window, 0);
    WriteGIFHeader(f, dimx, dimy, lut, lut, lut, 256, csLocalPalette, "NETSCAPE2.0");
    delete DDO2;
    
    for (i=0; i<GetNumberOfFrames(pDDO); i++)
    { if (i>0)
      { DDO2 = MakeCopy(pDDO);
        To8bitMonochromeOrRGB(DDO2, size, &dimx, &dimy, &out, 0, level, window, i);
        delete DDO2;
      }

      Ratefps = (frame % 100);
      if (Ratefps==0) Ratefps=1;
      if (out) WriteGIFFrame (f, out, dimx, dimy, lut, lut, lut, 256, 1, 100/Ratefps, csLocalPalette);
      free(out);
    }

    fputc(0x3b, f);                    /* Write GIF file terminator */
    fclose(f);
    return TRUE;
  }

  // create static GIF here
  if (To8bitMonochromeOrRGB(pDDO, size, &dimx, &dimy, &out, 0, level, window, frame))
  { WriteGIF(f, out, 0, dimx, dimy, lut, lut, lut, 256, 0, "ConquestDICOMServer");
    free(out);
    fclose(f);
    return TRUE;
  }

  return FALSE;
}

#pragma pack(1)
typedef struct  
{       char	bfType1;		/* Must be 'B' */
        char	bfType2;		/* Must be 'M' */
        int	bfSize;			/* Size of the file */
        short	bfReserved1;		/* Must be 0 */
        short	bfReserved2;		/* Must be 0 */
        int	bfOffBits;		/* Offset of the ImageBits */

	int	biSize;			/* size of all biXX (40) */
	int	biWidth;
	int	biHeight;
	short	biPlanes;
	short	biBitCount;
	int	biCompression;
	int	biSizeImage;
	int	biXPelsPerMeter;
	int	biYPelsPerMeter;
	int	biClrUsed;
	int	biClrImportant;
} BMP_HEADER;	/* 24BITS */
#pragma pack()

BOOL ToBMP(DICOMDataObject* pDDO, char *filename, int size, int append, int level, int window, unsigned int frame, float gamma)
{ int dimx, dimy;
  unsigned char *out, c;
  FILE *f;
  int i, t;
  int iDataSize;
  BMP_HEADER bmp;

  if (To8bitMonochromeOrRGB(pDDO, size, &dimx, &dimy, &out, 1, level, window, frame, gamma))
  { iDataSize = dimx*dimy*3;	// must be multiple of 4: OK because above routine made dimx multiple of 4

    // swap RGB to BGR
    t = dimx * dimy;
    for (i=0; i<t; i++) {c=out[i*3]; out[i*3]=out[i*3+2]; out[i*3+2]=c; }

    memset((void*)&bmp, 0, sizeof(BMP_HEADER));
    bmp.bfType1 = 'B';
    bmp.bfType2 = 'M';
    bmp.bfSize = iDataSize + sizeof(BMP_HEADER);
    bmp.bfOffBits = sizeof(BMP_HEADER);

    bmp.biSize = 40;
    bmp.biWidth = dimx;
    bmp.biHeight = dimy;
    bmp.biPlanes = 1;
    bmp.biBitCount = 24;
    bmp.biSizeImage = iDataSize;
    bmp.biXPelsPerMeter = 0x0ece;
    bmp.biYPelsPerMeter = 0x0ece;

    if (append) f = fopen(filename, "ab");
    else        f = fopen(filename, "wb");
    if (!f)
    { free(out);
      return FALSE;
    }

    fwrite((void*)&bmp, sizeof(BMP_HEADER), 1, f);
    fwrite(out,         dimx*dimy*3,        1, f);
    
    free(out);
    fclose(f);
    return TRUE;
  }

  return FALSE;
}

BOOL BMPStrip(DICOMDataObject** pDDO, char *filename, int size, int append, int level, int window, unsigned int frame, int cols, float gamma)
{ int dimx, dimy;
  unsigned char *out, c;
  FILE *f;
  int i, j, t, len;
  int iDataSize;
  BMP_HEADER bmp;
  
  for (i=0; i<10000; i++)
  { if (pDDO[i]==NULL)
    { len = i;
      break;
    }
  }
  int rows = (len+cols-1)/cols;

  To8bitMonochromeOrRGB(pDDO[0], size, &dimx, &dimy, &out, 1, level, window, frame, gamma);
  
  iDataSize = dimx*dimy*3*len;	// must be multiple of 4: OK because above routine made dimx multiple of 4
  memset((void*)&bmp, 0, sizeof(BMP_HEADER));
  bmp.bfType1 = 'B';
  bmp.bfType2 = 'M';
  bmp.bfSize = iDataSize + sizeof(BMP_HEADER);
  bmp.bfOffBits = sizeof(BMP_HEADER);
  
  bmp.biSize = 40;
  bmp.biWidth = dimx*cols;
  bmp.biHeight = dimy*rows;
  bmp.biPlanes = 1;
  bmp.biBitCount = 24;
  bmp.biSizeImage = iDataSize;
  bmp.biXPelsPerMeter = 0x0ece;
  bmp.biYPelsPerMeter = 0x0ece;
  
  if (append) f = fopen(filename, "ab");
  else        f = fopen(filename, "wb");
  if (!f)
  { free(out);
    return FALSE;
  }

  fwrite((void*)&bmp, sizeof(BMP_HEADER), 1, f);

  for (j=len-1; j>=0; j--)
  { if (j>0) free(out);
    To8bitMonochromeOrRGB(pDDO[j], size, &dimx, &dimy, &out, 1, level, window, frame);
    // swap RGB to BGR
    t = dimx * dimy;
    for (i=0; i<t; i++) {c=out[i*3]; out[i*3]=out[i*3+2]; out[i*3+2]=c; }

    fwrite(out,  dimx*dimy*3, 1, f);
  }
  
  for (j=len; j<rows*cols; j++)
  { memset(out, 0, dimx * dimy * 3);
    fwrite(out,  dimx*dimy*3, 1, f);
  }
    
  free(out);
  fclose(f);
  return TRUE;
}

#ifdef HAVE_LIBJPEG //The jpeg common stuff.
/* This is a replacement for the libjpeg message printer that normally
*  goes to stderr. */
METHODDEF(void)
_joutput_message (j_common_ptr cinfo)
{ 
  char buffer[JMSG_LENGTH_MAX];

  /* Create the message */
  (*cinfo->err->format_message) (cinfo, buffer);

  OperatorConsole.printf("***[JPEG Library]: %s\n", buffer);
}

/* This is a replacement for the libjpeg error handler that normally
*  halts the program. */
METHODDEF(void)
_jerror_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  _jerror_ptr jerr = (_jerror_ptr) cinfo->err;

  /* Always display the message */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(jerr->setjmp_buffer, 1);
}

/* Just for routines we do not need or use. */
METHODDEF(void)
jdoes_nothing (j_decompress_ptr cinfo)
{
/* Nothing */
  UNUSED_ARGUMENT(cinfo);
  return;
}
#else// Use jpeg_encoder.cpp
UINT32 encode_image (UINT8 *input_ptr, UINT8 *output_ptr, UINT32 quality_factor, UINT32 image_format, UINT32 image_width, UINT32 image_height);
#endif

BOOL ToJPG(DICOMDataObject* pDDO, char *filename, int size, int append, int level, int window, unsigned int frame, int quality, float gamma);

BOOL ToJPGStrip(DICOMDataObject** pDDO, char *filename, int size, int append, int level, int window, unsigned int frame, int quality, int cols, float gamma)
{ int 			dimx, dimy;
  unsigned char 	*out[32], c;
  FILE 			*f;
  int 			i, j, Ratefps, len;
#ifdef HAVE_LIBJPEG
  struct                jpeg_compress_struct cinfo;
  unsigned int          rowWidth;
  struct _jerror_mgr     jerr;
  register JSAMPROW     row_pointer;
#endif
  BOOL			result;
  char			*merged_row;

  if (frame>10000 && GetNumberOfFrames(pDDO[0])>1)
  { char file[256], file2[256];
    DICOMDataObject *DDO2 = MakeCopy(pDDO[0]);
    NewTempFile(file);
    for (i=0; i<GetNumberOfFrames(pDDO[0]); i++)
    { sprintf(file2, "%s.%05d", file, i);
      ToJPG(DDO2, file2, size, 0, level, window, i, quality, gamma);
      delete DDO2;
    }
    
    Ratefps = (frame % 100);
    if (Ratefps==0) Ratefps=1;

    // to convert a row of jpeg images to mpeg2 using mjpegtools:
// jpeg2yuv -f 25 -I p -j dgate%03d.jpg | mpeg2enc -f 3 -b 1500 -o mpegfile.m2v
//          rate  progr files                      mpg2  bitrate   file
    sprintf(file2, "jpeg2yuv -f %d -I p -j %s%%05d | mpeg2enc -f 3 -b 1500 -o %s", Ratefps, file, filename);
    system(file2);

    for (i=0; i<GetNumberOfFrames(pDDO[0]); i++)
    { sprintf(file2, "%s.%05d", file, i);
      unlink(file2);
    }
    return TRUE;
  }

  for (i=0; i<10000; i++)
  { if (pDDO[i]==NULL)
    { len = i;
      break;
    }
  }
  int rows = (len+cols-1)/cols;

  i = 0;
  for (j=0; j<cols; j++) 
    out[j]=NULL;

  for (j=0; j<cols; j++) 
  { if (i<len)	  
      result = To8bitMonochromeOrRGB(pDDO[i++], size, &dimx, &dimy, &out[j], 2, level, window, frame, gamma);
    else
    { out[j] = (unsigned char *)malloc(dimx*dimy*3);
      memset(out[j], 0, dimx*dimy*3);
    }
  }

  merged_row = (char *)malloc(dimx * 3 * cols);

  if (result)
  { if (append) f = fopen(filename, "ab");
    else        f = fopen(filename, "wb");
    if (!f)
    { for (i=0; i<cols; i++) free(out[i]);
      free(merged_row);
      return FALSE;
    }
#ifdef HAVE_LIBJPEG

// Init the default handler.
    cinfo.err = jpeg_std_error(&jerr.pub);
// change the error exit so libjpeg can't kill us.
    jerr.pub.error_exit = _jerror_exit;
// Use our methode for outputting messages.
    jerr.pub.output_message = _joutput_message;
    if (setjmp(jerr.setjmp_buffer))
    {
/* If we get here, the JPEG code has signaled an error.
 * We need to clean up the JPEG object and return. */
      jpeg_destroy_compress(&cinfo);
      if(f != NULL)
      { fclose(f);
        if (!append)unlink(filename);
      }
      for (i=0; i<cols; i++) free(out[i]);
      free(merged_row);
      return (FALSE);
    }
//Look for multi-byte version 6c (63) from www.bitsltd.net
    jpeg_CreateCompress(&cinfo, 63, (size_t) sizeof(struct jpeg_compress_struct));
// Color space must be set before a call to jpeg_set_defaults.
    cinfo.in_color_space = JCS_RGB; //To8bitMonochromeOrRGB is set for rgb, gray could be faster.
    jpeg_set_defaults(&cinfo);
// Get all the image size stuff.
    cinfo.image_height =  dimy*rows;
    cinfo.image_width =  dimx*cols;
    cinfo.num_components = 3;
    cinfo.input_components = cinfo.num_components;
    cinfo.data_precision = 8;
    cinfo.data_precision_other = cinfo.data_precision;
    jpeg_set_quality(&cinfo, 95, true);//100 is silly in lossy and the same as 95.
    cinfo.jpeg_color_space = cinfo.in_color_space;
    jpeg_default_colorspace(&cinfo);
// Set where to put it.
    jpeg_stdio_dest(&cinfo, f);
    rowWidth =  cinfo.image_width * cinfo.num_components;
    jpeg_start_compress(&cinfo, TRUE);
    while (cinfo.next_scanline < cinfo.image_height)
    { for (j=0; j<cols; j++)
	memcpy(merged_row + 3 * dimx * j, out[j] + (cinfo.next_scanline%dimy) * 3 * dimx, 3 * dimx);
      row_pointer = (JSAMPROW)merged_row;
      jpeg_write_scanlines(&cinfo, &row_pointer, 1);
      if ((cinfo.next_scanline % dimy)==0)
      { for (j=0; j<cols; j++) 
	{ if (i<len)
	  { free(out[j]);
            To8bitMonochromeOrRGB(pDDO[i++], size, &dimx, &dimy, &out[j], 2, level, window, frame, gamma);
	  }
	  else
	    memset(out[j], 0, dimx*dimy*3);
        }
      }
    }
// If here, finished the compression.
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    for (i=0; i<cols; i++) free(out[i]);
#else // Old way, strip compress not implemented.
    unsigned char *output = (UINT8 *)malloc(dimx * dimy * 3);
    UINT32 len;
    len = encode_image (out[0], output, 256, 14, dimx, dimy);
    fwrite (output, 1, len, f);
    free(output);
    free(out[0]);
#endif
    fclose(f);
    return TRUE;
  }

  return FALSE;
}

BOOL ToJPG(DICOMDataObject* pDDO, char *filename, int size, int append, int level, int window, unsigned int frame, int quality, float gamma)
{ DICOMDataObject *a[2];
  a[0] = pDDO;
  a[1] = NULL;
  return ToJPGStrip(a, filename, size, append, level, window, frame, quality, 1, gamma);
}
