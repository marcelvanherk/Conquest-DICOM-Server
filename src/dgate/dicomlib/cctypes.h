/*
????????	mvh	Fixed to allow compile with BC
20010705	ljz	Fixed to allow compile with MSVC 6 (see note below)
20040111        mvh     Fix for __GNUC__
20040117        mvh     Fix for linux
20070308	bcb 	Added for DARWIN apple endian defs
20070902	mvh	Changed INT32 and UINT32 from long to int for ms7/ms8 compile
20100706	bcb	Made INT32 match libjpeg if HAVE_LIBJPEG.
20100717	mvh	Merged; fixed for WIN32 64 bits
20140611      lsp+mgw   Fix for INT32 under unix
*/

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

/*
 * Copyright (c) 1995 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and that due credit is given
 * to the University of California at Davis. The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific prior written permission. This software
 * is provided ``as is'' without express or implied warranty.
 *
 */

/* CC Types */


/*
   NOTE: MSVC 5 did not have UINT32 defined. Version 6 however defines it
   in 'BASETSD.H', but wrong!!!!!:

//
// The following types are guaranteed to be signed and 32 bits wide.
//

typedef int LONG32, *PLONG32;
typedef int INT32, *PINT32;

//
// The following types are guaranteed to be unsigned and 32 bits wide.
//

typedef unsigned int ULONG32, *PULONG32;
typedef unsigned int DWORD32, *PDWORD32;
typedef unsigned int UINT32, *PUINT32;


    The int type will in future be 64 bits or more
*/

#ifndef CCTYPES_H
#define CCTYPES_H

#ifdef	SOLARIS
#	define	SYSTEM_V
#endif

#ifndef	WINDOWS
typedef unsigned	int	BOOL;
#endif
typedef unsigned	int		UINT;
typedef unsigned	short	UINT16;
#ifndef __BORLANDC__
#if _MSC_VER != 1200
#if !defined(__GNUC__) || !defined(WIN32)
typedef unsigned	int	UINT32;
#endif
#endif
#endif

typedef unsigned	char	UINT8;
#ifndef	WINDOWS
typedef unsigned	char	BYTE;
#endif

typedef	signed		char	INT8;
typedef	signed		short	INT16;
#ifndef WIN32

#ifndef XMD_H			/* X11/xmd.h correctly defines INT32 */
#if UINT_MAX == 0xffffffff
typedef int INT32;             /* Most likely */
#elif UINT_MAX == 0xffffffffffffffff
typedef __int32 INT32;         /* Some 64 bit systems */
#else //UINT_MAX == 0xffffffff
typedef long INT32;            /* Default */
#endif //UINT_MAX == 0xffffffff
#endif //XMD_H

#endif //WIN32

#ifndef	WINDOWS
typedef	signed		int		INT;
#endif

#ifndef	TRUE
#define	TRUE	((UINT) 1)
#endif
#ifndef	FALSE
#define	FALSE	((UINT) 0)
#endif


#ifdef	LITTLE_ENDIAN
#undef	LITTLE_ENDIAN
#endif
#ifdef	BIG_ENDIAN
#undef	BIG_ENDIAN
#endif

#	define	LITTLE_ENDIAN	1
#	define	BIG_ENDIAN		2
// Some Mac OS X universal defines
// Apple has both big and little endian systems
#ifndef NATIVE_ENDIAN
#ifdef DARWIN
#if __BIG_ENDIAN__
#	define	NATIVE_ENDIAN  BIG_ENDIAN
#endif
#if __LITTLE_ENDIAN__
#	define	NATIVE_ENDIAN  LITTLE_ENDIAN
#endif
#endif
#endif
#ifndef	NATIVE_ENDIAN
#	define	NATIVE_ENDIAN	LITTLE_ENDIAN
#endif
        
#endif
