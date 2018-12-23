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

/* 20070330	mvh	Merged bcb stuff */
/* 20070726	bcb	Removed some duplicates from endian.hpp */
/* 20130508     lsp     Removed unused XE_UINTX and include "endian.cpd"

/***************************************************************************
 *
 * University of California, Davis
 * UCDMC DICOM Network Transport Libraries
 * Version 0.1 Beta
 *
 * Technical Contact: mhoskin@ucdavis.edu
 *
 ***************************************************************************/

#	include	"dicom.hpp"

UINT16	SwitchEndian(UINT16	x)
	{
	return ( ((x >> 8) & 0x00ff) | ((x << 8) & 0xff00) );
	}

UINT32	SwitchEndian(UINT32	x)
	{
	return ( ((UINT32) SwitchEndian ((UINT16) (x >> 16))) |
		((UINT32) (SwitchEndian ((UINT16) (x & 0x0000ffff)) << 16) ));
	}

INT16	SwitchEndian(INT16	x)
	{
	return ((INT16) SwitchEndian((UINT16) x) );
	}

INT32	SwitchEndian(INT32	x)
	{
	return ((INT32) SwitchEndian((UINT32) x) );
	}

#   ifndef BSD  // done in endian.hpp

#	if	NATIVE_ENDIAN	==	LITTLE_ENDIAN

#	define	LE_UINT16	UINT16
#	define	LE_UINT32	UINT32
#	define	LE_INT16	INT16
#	define	LE_INT32	INT32

//#	define	XE_UINTX	BE_UINT16
#	define	UINTX		UINT16
//#	include	"endian.cpd"
//#undef	XE_UINTX
#undef	UINTX
//#	define	XE_UINTX	BE_UINT32
#	define	UINTX		UINT32
//#	include	"endian.cpd"
//#undef	XE_UINTX
#undef	UINTX
//#	define	XE_UINTX	BE_INT16
#	define	UINTX		INT16
//#	include	"endian.cpd"
//#undef	XE_UINTX
#undef	UINTX
//#	define	XE_UINTX	BE_INT32
#	define	UINTX		INT32
//#	include	"endian.cpd"

//#undef	XE_UINTX
#undef	UINTX

#else
#	define	BE_UINT16	UINT16
#	define	BE_UINT32	UINT32
#	define	BE_INT16	INT16
#	define	BE_INT32	INT32

//#	define	XE_UINTX	LE_UINT16
#	define	UINTX		UINT16
//#	include	"endian.cpd"
//#undef	XE_UINTX
#undef	UINTX
//#	define	XE_UINTX	LE_UINT32
#	define	UINTX		UINT32
//#	include	"endian.cpd"
//#undef	XE_UINTX
#undef	UINTX
//#	define	XE_UINTX	LE_INT16
#	define	UINTX		INT16
//#	include	"endian.cpd"
//#undef	XE_UINTX
#undef	UINTX
//#	define	XE_UINTX	LE_INT32
#	define	UINTX		INT32
//#	include	"endian.cpd"

//#undef	XE_UINTX
#undef	UINTX

#endif
#endif //BSD




