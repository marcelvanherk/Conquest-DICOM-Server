/* 
mvh 20071102: Define SetDicomErrorHandler etc 
mvh 20080203: Added experimental ConfigPadAEWithZeros
mvh 20090211: Added DCM_ERROR_PARSE
mvh 20090802: Added DCM_ERROR_DEBUG
bcb 20091231: Changed char* to const char* for gcc4.2 warnings
mvh 20100111: Merged
mvh 20110231: DicomError returns FALSE when handler not installed - to allow some errors to pass
mvh 20110605: Added DCM_ERROR_ASSOCIATION
*/

//#	define	DEBUG_VERSION

/****************************************************************************
          Copyright (C) 1995, University of California, Davis

          THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND THE UNIVERSITY
          OF CALIFORNIA DOES NOT MAKE ANY WARRANTY ABOUT THE SOFTWARE, ITS
          PERFORMANCE, ITS MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR
          USE, FREEDOM FROM ANY COMPUTER DISEASES OR ITS CONFORMITY TO ANY
          SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF
          THE SOFTWARE IS WITH THE USER.

          Copyright of the software and supporting documentation is
          owned by the University of California, and free access
          is hereby granted as a license to use this software, copy this
          software and prepare derivative works based upon this software.
          However, any distribution of this software source code or
          supporting documentation or derivative works (source code and
          supporting documentation) must include this copyright notice.
****************************************************************************/

/***************************************************************************
 *
 * University of California, Davis
 * UCDMC DICOM Network Transport Libraries
 * Version 0.1 Beta
 *
 * Technical Contact: mhoskin@ucdavis.edu
 *
 ***************************************************************************/

/************************************************************************
 *
 * Standard Include for all source files.
 *
 ***********************************************************************/
#ifndef	_DICOM_H_
#	define	_DICOM_H_
#	include	"base.hpp"		// C Header Files/Base Data Templates
#	include	"endian.hpp"	// Endian Nuetral types
#	include	"constant.h"	// Related constants
#	include	"version.h"		// Version Related Information
#	include	"socket.hpp"	// Socket Transport Layer
#	include	"deivr.hpp"		// DICOMObject/DICOMCMDObject/DICOMDATAObject
#	include	"rtc.hpp"		// Run-Time-Class support of VR
#	include	"aarq.hpp"		// A-ASSOCIATE-RQ PDU
#	include	"aaac.hpp"		// A-ASSOCIATE-AC PDU
#	include	"aarj.hpp"		// A-ASSOCIATE-RJ/A-RELEASE-RQ/A-RELEASE-RP/A-ABORT-RQ
#	include	"pdata.hpp"		// P-DATA-TF
#	include	"pdu.hpp"		// PDU Services

#	include	"dimsec.hpp"	// DIMSE-C Services CEchoRQ/RSP, CStoreRQ/RSP
							// CFindRQ/RSP, CMoveRQ/RSP
#	include	"dimsen.hpp"	// DIMSE-N Services
#	include	"verify.hpp"	// Verification SOP Class
#	include	"storage.hpp"	// CT Storage, MR Storage, etc.
#	include	"qrsop.hpp"		// Abstract Q/R Classes
#	include	"flpdu.hpp"		// Checked PDU_Service Class

#define DCM_ERROR_MEMORY 1
#define DCM_ERROR_PARSE 2
#define DCM_ERROR_DEBUG 3
#define DCM_ERROR_ASSOCIATION 4

typedef void (*DeepErrorCall)(int error,const char *message, int info);
void SetDicomErrorHandler(DeepErrorCall handler);
BOOL DicomError(int error,const char *message, int info);

extern BOOL ConfigPadAEWithZeros;

#endif

