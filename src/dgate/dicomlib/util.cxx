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

#	include	"dicom.hpp"

BOOL	ByteCopy(BYTE	*s1, BYTE	*s2, UINT	Count)
	{
	memcpy((void *) s1, (void *) s2, Count);
	return ( TRUE );
	}

BOOL	ZeroMem(BYTE	*mem, UINT	Count)
	{
	memset((void *) mem, 0, Count);
	return ( TRUE );
	}

BOOL
SpaceMem(
	BYTE	*mem,
	UINT	Count )
	{
	memset((void *) mem, ' ', Count);
	return ( TRUE );
	}
		
UINT	ByteStrLength(BYTE	*s)
	{
	return ( strlen((char *) s) );
	}
/*
UINT16	NativeEndian(UINT16	uint16, UINT	Endian)
	{
	UINT16	ruint16;

	if(Endian==NATIVE_ENDIAN)
		return(uint16);
	
	ruint16 = 0;
	ruint16 = (uint16 << 8)&0xff00;
	ruint16 = ruint16 | ((tuint16 >> 8) & 0x00ff);
	return(ruint16);
	}
*/
