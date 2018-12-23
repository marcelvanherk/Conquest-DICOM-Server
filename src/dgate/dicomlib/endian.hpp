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

/*
20090215	mvh	Started an update history
*/

/***************************************************************************
 *
 * University of California, Davis
 * UCDMC DICOM Network Transport Libraries
 * Version 0.1 Beta
 *
 * Technical Contact: mhoskin@ucdavis.edu
 *
 ***************************************************************************/

// Architectually Neutral Endian types.
//
// Note these are not done with C++ templates.  This is a design choice.
UINT16	SwitchEndian(UINT16	x);
UINT32	SwitchEndian(UINT32	x);
INT16	SwitchEndian(INT16	x);
INT32	SwitchEndian(INT32	x);

#   ifdef BSD  // Swap durring file reads and writes
#	define	LE_UINT16	UINT16
#	define	LE_UINT32	UINT32
#	define	LE_INT16	INT16
#	define	LE_INT32	INT32
#	else //Do the original stuff
#	if	NATIVE_ENDIAN	==	LITTLE_ENDIAN

#	define	LE_UINT16	UINT16
#	define	LE_UINT32	UINT32
#	define	LE_INT16	INT16
#	define	LE_INT32	INT32

#	define	XE_UINTX	BE_UINT16
#	define	UINTX		UINT16
#	include	"endian.hpd"
#undef	XE_UINTX
#undef	UINTX
#	define	XE_UINTX	BE_UINT32
#	define	UINTX		UINT32
#	include	"endian.hpd"
#undef	XE_UINTX
#undef	UINTX
#	define	XE_UINTX	BE_INT16
#	define	UINTX		INT16
#	include	"endian.hpd"
#undef	XE_UINTX
#undef	UINTX
#	define	XE_UINTX	BE_INT32
#	define	UINTX		INT32
#	include	"endian.hpd"

#undef	XE_UINTX
#undef	UINTX

#else
#	define	BE_UINT16	UINT16
#	define	BE_UINT32	UINT32
#	define	BE_INT16	INT16
#	define	BE_INT32	INT32

#	define	XE_UINTX	LE_UINT16
#	define	UINTX		UINT16
#	include	"endian.hpd"
#undef	XE_UINTX
#undef	UINTX
#	define	XE_UINTX	LE_UINT32
#	define	UINTX		UINT32
#	include	"endian.hpd"
#undef	XE_UINTX
#undef	UINTX
#	define	XE_UINTX	LE_INT16
#	define	UINTX		INT16
#	include	"endian.hpd"
#undef	XE_UINTX
#undef	UINTX
#	define	XE_UINTX	LE_INT32
#	define	UINTX		INT32
#	include	"endian.hpd"

#undef	XE_UINTX
#undef	UINTX

#endif
#endif //BSD

