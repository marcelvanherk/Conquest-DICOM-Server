//
//  jpegconv.hpp
//
/*
20120710 bcb Created.
20131111 bcb Changed from #include<> to #include"" for Xcode
20131207 bcb Added JPeg_LS
20151203 bcb Moved some dependent defines from total.cpp to here.
20160203 bcb Fix VS 2015 Libjpeg bool redefine.
*/

#ifndef jpegconv_hpp
#define jpegconv_hpp
#ifdef _WIN32
#define boolean bool
#endif

#include "imagedata.hpp"

// dependent defines
#ifdef BCB_FIX // Became tired of retyping on different systems.
#define FUJI_FIX // Allows decompressing Fuji AC3 jpeg compressed,not encapsulated images.
#endif
#ifdef HAVE_LIBJPEG
#undef NOINTJPEG
// Used to make Libjpeg use int as UINT.
#ifndef UINT_MAX
#define UINT_MAX 0xffffffff
#endif
#endif

#ifdef HAVE_LIBOPENJPEG2
#define HAVE_LIBOPENJPEG
#endif


#ifdef HAVE_LIBOPENJPEG
#define HAVE_J2K
#endif

#ifdef HAVE_LIBJASPER
#ifdef HAVE_LIBOPENJPEG
#define  HAVE_BOTH_J2KLIBS
#else // Only one lib
#define HAVE_J2K
#endif
#endif

#ifdef HAVE_LIBJPEG
#ifdef _WIN32
#ifndef __RPCNDR_H__
#define __RPCNDR_H__ // don't redifine boolean
#include "jpeglib.h"
#include "jerror.h"
#include "setjmp.h"
#undef  __RPCNDR_H__
#else // !__RPCNDR_H__
#include "jpeglib.h"
#include "jerror.h"
#include "setjmp.h"
#endif // !__RPCNDR_H__
#else //UNIX
#include "jpeglib.h"
#include "jerror.h"
#include "setjmp.h"
#endif //_WIN32
#endif //HAVE_LIBJPEG

#ifdef HAVE_LIBJPEG
METHODDEF(void) joutput_message (j_common_ptr cinfo);
METHODDEF(void) jerror_exit (j_common_ptr cinfo);
METHODDEF(void) jdoes_nothing_comp (j_compress_ptr cinfo);
METHODDEF(void) jdoes_nothing_decomp (j_decompress_ptr cinfo);
METHODDEF(boolean) jreturns_true_comp (j_compress_ptr cinfo);
#else// Use jpeg_encoder.cpp
UINT32 encode_image (UINT8 *input_ptr, UINT8 *output_ptr, UINT32 quality_factor, UINT32 image_format, UINT32 image_width, UINT32 image_height);
#endif
BOOL ToJPG(DICOMDataObject* pDDO, char *filename, int size, int append, int level, int window, unsigned int frame);
#ifdef HAVE_LIBJPEG
BOOL CompressJPEGL(DICOMDataObject* pDDO, int comp = '1', int jpegQuality = 95 );
METHODDEF(boolean) jfill_input_buffer (j_decompress_ptr cinfo);
METHODDEF(void) jskip_input_data (j_decompress_ptr cinfo, long num_bytes);
BOOL DecompressJPEGL(DICOMDataObject* pDDO);
#endif //End HAVE_LIBJEPG
#ifdef HAVE_J2K
BOOL HaveNoJ2kStream(ImageData *imageDataPtr);
#endif
#ifdef HAVE_LIBJASPER
BOOL CompressJPEG2K(DICOMDataObject* pDDO, int j2kQuality);
BOOL DecompressJPEG2K(DICOMDataObject* pDDO);
#endif //End LIBJASPER
#ifdef HAVE_LIBOPENJPEG
void error_callback(const char *msg, void *client_data);
void warning_callback(const char *msg, void *client_data);
void info_callback(const char *msg, void *client_data);
BOOL CompressJPEG2Ko(DICOMDataObject* pDDO, int j2kQuality);
BOOL DecompressJPEG2Ko(DICOMDataObject* pDDO);
#endif //End LIBOPENJPEG
#ifdef HAVE_LIBCHARLS// JPEG LS stuff
BOOL CompressJPEGLS(DICOMDataObject* pDDO, int jpegQuality = 0);
BOOL DecompressJPEGLS(DICOMDataObject* pDDO);
#endif
#endif