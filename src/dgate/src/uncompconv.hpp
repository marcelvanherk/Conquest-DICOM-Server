/****************************************************************************
 
 THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND IMAGE RETERIVAL SERVICES
 DOES NOT MAKE ANY WARRANTY ABOUT THE SOFTWARE, ITS
 PERFORMANCE, ITS MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR
 USE, FREEDOM FROM ANY COMPUTER DISEASES OR ITS CONFORMITY TO ANY
 SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF
 THE SOFTWARE IS WITH THE USER.
 
 Copyright of the software and supporting documentation is
 owned by the Image Reterival Services, and free access
 is hereby granted as a license to use this software, copy this
 software and prepare derivative works based upon this software.
 However, any distribution of this software source code or
 supporting documentation or derivative works (source code and
 supporting documentation) must include this copyright notice.
 ****************************************************************************/

/***************************************************************************
 *
 * Image Reterival Services
 * Version 0.1 Beta
 *
 ***************************************************************************/

/*
 bcb 20140513: Created this file.*/
#ifndef uncompconv_hpp
#define uncompconv_hpp

#include "dicom.hpp"

//May never be used, but it is possible to have 6 bits allocated gray.
//This will map smaller pixel allocations into 8 bits filling the extra bits with 0s.
UINT32 To8BitGray(unsigned char** dataH,  UINT32 length, UINT16 dataWidth, BOOL freeBuf = FALSE);
//May never be used, but it is possible to have 6 bits allocated gray.
//This will map smaller pixel allocations into 16 bits filling the extra bits with 0s.
UINT32 To16BitGray(unsigned char** dataH,  UINT32 length, UINT16 dataWidth, BOOL freeBuf = FALSE);
//Swap 8 bit high and low words.
void SwapEndian(unsigned char* dataPtr, UINT32 length);
//Expands the size of *Data and fills in the extra CrCbs.
//Returns new buffer length or zero if malloc() fails.
//freeBuf will free the original buffer before reassigning *dataH.
UINT32 YBR_FULL_422_to_FULL(unsigned char** dataIn,  UINT32 length, BOOL freeBuf = FALSE);
//Converts the Partial values of Y, Cr, and Cb to the full values.
void YBR_PARTIAL_422_to_FULL_422(unsigned char* data, int length);
//Expands the size of *Data and fills in the extra CrCbs after
//converting the Partial values of Y, Cr, and Cb to the full values.
//Returns new buffer length or zero if malloc() fails
//freeBuf will free the original buffer before reassigning *dataH.
UINT32 YBR_PARTIAL_420_to_FULL(unsigned char** dataH, UINT32 height,
                               UINT32 width, BOOL freeBuf = FALSE);
//Converts the R G B to Y, Cr, and Cb.
void RGB_ToYBRFULL(char* data, int length);
//Some local math routines.
#ifndef _WIN32 // Statics
static unsigned char FULL_from_PARTIAL_Y(unsigned char data);
static unsigned char FULL_from_PARTIAL_Cx(unsigned char data);
#else
unsigned char FULL_from_PARTIAL_Y(unsigned char data);
unsigned char FULL_from_PARTIAL_Cx(unsigned char data);
#endif

#endif
