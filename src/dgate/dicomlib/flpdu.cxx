/*
20050130	mvh	Allocate 1 item longer lists to allow configuration from GUI
20050213	mvh	Fix to allow windows created file as input under linux
20060618	mvh	Added definition of _SH_DENYNO
20090802	mvh	Added DCM_ERROR_DEBUG statement to debug failed connect
20100309	bcb	Commented out unused variable (gcc4.2 Warnings)
20100619	bcb	Fix gcc4 warnings and improve speed
20100717	mvh	Merged
Spectra 0014 - Wed, 12 Feb 2014 15:48:27 -0200: Patch mismatches new/delete in flpdu.cxx
20140528        lsp     Kept member initialization only in constructor and removed
                        InitializeFrom(): not GNUC specific
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

#	include	"dicom.hpp"
#ifdef	MAC
extern	"C"	int	stricmp(char	*, char *);
#endif
#ifndef	stricmp
#ifndef	WINDOWS
#ifndef	MAC
#	define	stricmp	strcasecmp
#endif
#endif
#endif
#ifdef	WINDOWS
#	include	<stdlib.h>
#endif

#ifndef _SH_DENYNO 
#define _SH_DENYNO SH_DENYNO
#endif

static	BOOL
__is_whitespace(char	ch)
{
	switch ( ch )
	{
		case	' ':
		case	'\t':
		case	'\n':
			return ( TRUE );
	}
	return ( FALSE );
}

#	define	is_whitespace(xxx)	__is_whitespace(xxx)

CheckedPDU_Service	::	CheckedPDU_Service ( char	*filename )
  : SOPUIDListCount(0),
    TransferUIDListCount(0),
    ApplicationUIDListCount(0),
    RemoteAEListCount(0),
    LocalAEListCount(0),
    SOPUIDList(NULL),
    SOPUIDListNames(NULL),
    TransferUIDList(NULL),
    TransferUIDListNames(NULL),
    ApplicationUIDList(NULL),
    ApplicationUIDListNames(NULL),
    RemoteAEList(NULL),
    RemoteAEListNames(NULL),
    LocalAEList(NULL),
    LocalAEListNames(NULL)
    { //Initialize it here, no need for an extra call
	FILE		*fp;
	UINT		Index, ArgIndex;
	UINT	SOPUIDListIndex = 0,
	TransferUIDListIndex = 0,
	ApplicationUIDListIndex = 0,
	RemoteAEListIndex = 0,
	LocalAEListIndex = 0;
	char		s[256];
	char		Arg1[64];
	char		Arg2[64];
	char		Arg3[64];
	//	char		Arg4[64];
		
	if ( ! filename )
		return;
	
#ifndef	WINDOWS
	fp = fopen ( filename, "r" );
#else
	fp = _fsopen ( filename, "r", _SH_DENYWR );
#endif
	if ( ! fp )
		return;
	
	fgets ( s, 255, fp );
	while ( ! feof ( fp ) )
	{
		if ( s [ 0 ] == '#' )
		{
			fgets( s, 255, fp );
			continue;
		}
		Index = 0;
		ArgIndex = 0;
		while ( is_whitespace(s[Index]) )
		{
			if ( ! s[Index] )
				break;
			++Index;
		}
		if ( ! s[Index] )
		{
			fgets ( s, 255, fp );
			continue;
		}
		while ( !is_whitespace(s[Index]) )
		{
			Arg1[ArgIndex] = s[Index];
			if(!s[Index])
				break;
			++Index;
			++ArgIndex;
		}
		Arg1[ArgIndex] = '\0';
		if ( !s [Index] )
		{
			// garbled line
			fgets ( s, 255, fp );
			continue;
		}
		ArgIndex = 0;
		while ( is_whitespace(s[Index]) )
		{
			if ( ! s[Index] )
				break;
			++Index;
		}
		if ( ! s[Index] )
		{
			fgets ( s, 255, fp );
			continue;
		}
		while ( !is_whitespace(s[Index]) )
		{
			Arg2[ArgIndex] = s[Index];
			if(!s[Index])
				break;
			++Index;
			++ArgIndex;
		}
		Arg2[ArgIndex] = '\0';
		if ( !s [Index] )
		{
			fgets ( s, 255, fp );
			continue;
		}
		ArgIndex = 0;
		while ( is_whitespace(s[Index]) )
		{
			if ( ! s[Index] )
				break;
			++Index;
		}
		if ( ! s[Index] )
		{
			fgets ( s, 255, fp );
			continue;
		}
		while ( !is_whitespace(s[Index]) && s[Index]!=10 && s[Index]!=13)
		{
			Arg3[ArgIndex] = s[Index];
			if(!s[Index])
				break;
			++Index;
			++ArgIndex;
		}
		Arg3[ArgIndex] = '\0';
		
		if(!stricmp(Arg3, "RemoteAE"))
			++RemoteAEListCount;
		if(!stricmp(Arg3, "LocalAE"))
			++LocalAEListCount;
		if(!stricmp(Arg3, "sop"))
			++SOPUIDListCount;
		if(!stricmp(Arg3, "abstract"))
			++SOPUIDListCount;
		if(!stricmp(Arg3, "transfer"))
			++TransferUIDListCount;
		if(!stricmp(Arg3, "Application"))
			++ApplicationUIDListCount;
		
		// should detect garbled / unrecognized lines
		
		fgets ( s, 255, fp );
	}
	//fclose ( fp );
	
	if ( SOPUIDListCount || TRUE )
	{
		SOPUIDList = new char *[SOPUIDListCount+1];
		SOPUIDListNames = new char *[SOPUIDListCount+1];
	}
	if ( TransferUIDListCount || TRUE )
	{
		TransferUIDList = new char *[TransferUIDListCount+1];
		TransferUIDListNames = new char *[TransferUIDListCount+1];
	}
	if ( ApplicationUIDListCount || TRUE)
	{
		ApplicationUIDList = new char *[ApplicationUIDListCount+1];
		ApplicationUIDListNames = new char *[ApplicationUIDListCount+1];
	}
	if ( RemoteAEListCount || TRUE)
	{
		RemoteAEList = new char *[RemoteAEListCount+1];
		RemoteAEListNames = new char *[RemoteAEListCount+1];
	}
	if ( LocalAEListCount || TRUE)
	{
		LocalAEList = new char *[LocalAEListCount+1];
		LocalAEListNames = new char *[LocalAEListCount+1];
	}
	
	Index = 0;
	while ( Index < SOPUIDListCount )
	{
		SOPUIDList [ Index ] = NULL;
		SOPUIDListNames [ Index ] = NULL;
		++Index;
	}
	Index = 0;
	while ( Index < TransferUIDListCount )
	{
		TransferUIDList [ Index ] = NULL;
		TransferUIDListNames [ Index ] = NULL;
		++Index;
	}
	Index = 0;
	while ( Index < ApplicationUIDListCount )
	{
		ApplicationUIDList [ Index ] = NULL;
		ApplicationUIDListNames [ Index ] = NULL;
		++Index;
	}
	Index = 0;
	while ( Index < RemoteAEListCount )
	{
		RemoteAEList [ Index ] = NULL;
		RemoteAEListNames [ Index ] = NULL;
		++Index;
	}
	Index = 0;
	while ( Index < LocalAEListCount )
	{
		LocalAEList [ Index ] = NULL;
		LocalAEListNames [ Index] = NULL;
		++Index;
	}
	
	fseek(fp, 0, SEEK_SET);
	//	fp = fopen ( filename, "r" );
	//	if ( ! fp )
	//		return ( FALSE );
	
	fgets ( s, 255, fp );
	while ( ! feof ( fp ) )
	{
		if ( s [ 0 ] == '#' )
		{
			fgets( s, 255, fp );
			continue;
		}
		Index = 0;
		ArgIndex = 0;
		while ( is_whitespace(s[Index]) )
		{
			if ( ! s[Index] )
				break;
			++Index;
		}
		if ( ! s[Index] )
		{
			fgets ( s, 255, fp );
			continue;
		}
		while ( !is_whitespace(s[Index]) )
		{
			Arg1[ArgIndex] = s[Index];
			if(!s[Index])
				break;
			++Index;
			++ArgIndex;
		}
		Arg1[ArgIndex] = '\0';
		if ( !s [Index] )
		{
			// garbled line
			fgets ( s, 255, fp );
			continue;
		}
		ArgIndex = 0;
		while ( is_whitespace(s[Index]) )
		{
			if ( ! s[Index] )
				break;
			++Index;
		}
		if ( ! s[Index] )
		{
			fgets ( s, 255, fp );
			continue;
		}
		while ( !is_whitespace(s[Index]) )
		{
			Arg2[ArgIndex] = s[Index];
			if(!s[Index])
				break;
			++Index;
			++ArgIndex;
		}
		Arg2[ArgIndex] = '\0';
		if ( !s [Index] )
		{
			fgets ( s, 255, fp );
			continue;
		}
		ArgIndex = 0;
		while ( is_whitespace(s[Index]) )
		{
			if ( ! s[Index] )
				break;
			++Index;
		}
		if ( ! s[Index] )
		{
			fgets ( s, 255, fp );
			continue;
		}
		while ( !is_whitespace(s[Index]) && s[Index]!=10 && s[Index]!=13)
		{
			Arg3[ArgIndex] = s[Index];
			if(!s[Index])
				break;
			++Index;
			++ArgIndex;
		}
		Arg3[ArgIndex] = '\0';
		if(!stricmp(Arg3, "RemoteAE"))
		{
			RemoteAEList [ RemoteAEListIndex ] = 
			new char [ strlen ( Arg2 ) + 1 ];
			RemoteAEListNames [ RemoteAEListIndex ] =
			new char [ strlen ( Arg1 ) + 1 ];
			strcpy ( RemoteAEList [ RemoteAEListIndex ], Arg2 );
			strcpy ( RemoteAEListNames [ RemoteAEListIndex ], Arg1 );
			++RemoteAEListIndex;
		}
		if(!stricmp(Arg3, "LocalAE"))
		{
			LocalAEList [ LocalAEListIndex ] = 
			new char [ strlen ( Arg2 ) + 1 ];
			LocalAEListNames [ LocalAEListIndex ] =
			new char [ strlen ( Arg1 ) + 1 ];
			strcpy ( LocalAEList [ LocalAEListIndex ], Arg2 );
			strcpy ( LocalAEListNames [ LocalAEListIndex ], Arg1 );
			++LocalAEListIndex;
		}
		if(!stricmp(Arg3, "sop"))
		{
			SOPUIDList [ SOPUIDListIndex ] = 
			new char [ strlen ( Arg2 ) + 1 ];
			SOPUIDListNames [ SOPUIDListIndex ] =
			new char [ strlen ( Arg1 ) + 1 ];
			strcpy ( SOPUIDList [ SOPUIDListIndex ], Arg2 );
			strcpy ( SOPUIDListNames [ SOPUIDListIndex ], Arg1 );
			++SOPUIDListIndex;
		}
		if(!stricmp(Arg3, "abstract"))
		{
			SOPUIDList [ SOPUIDListIndex ] = 
			new char [ strlen ( Arg2 ) + 1 ];
			SOPUIDListNames [ SOPUIDListIndex ] =
			new char [ strlen ( Arg1 ) + 1 ];
			strcpy ( SOPUIDList [ SOPUIDListIndex ], Arg2 );
			strcpy ( SOPUIDListNames [ SOPUIDListIndex ], Arg1 );
			++SOPUIDListIndex;
		}
		if(!stricmp(Arg3, "transfer"))
		{
			TransferUIDList [ TransferUIDListIndex ] = 
			new char [ strlen ( Arg2 ) + 1 ];
			TransferUIDListNames [ TransferUIDListIndex ] =
			new char [ strlen ( Arg1 ) + 1 ];
			strcpy ( TransferUIDList [ TransferUIDListIndex ], Arg2 );
			strcpy ( TransferUIDListNames [ TransferUIDListIndex ], Arg1 );
			++TransferUIDListIndex;
		}
		if(!stricmp(Arg3, "Application"))
		{
			ApplicationUIDList [ ApplicationUIDListIndex ] = 
			new char [ strlen ( Arg2 ) + 1 ];
			ApplicationUIDListNames [ ApplicationUIDListIndex ] =
			new char [ strlen ( Arg1 ) + 1 ];
			strcpy ( ApplicationUIDList [ ApplicationUIDListIndex ], Arg2 );
			strcpy ( ApplicationUIDListNames [ ApplicationUIDListIndex ], Arg1 );
			++ApplicationUIDListIndex;
		}
		
		// should detect garbled / unrecognized lines
		
		fgets ( s, 255, fp );
	}
	fclose ( fp );
	
	return;	
}

CheckedPDU_Service	::	~CheckedPDU_Service ()
	{
	ReleaseMemory ( );
	}

BOOL
CheckedPDU_Service	::	ReleaseMemory ()
	{
	UINT		Index;

	if ( SOPUIDList )
		{
		Index = 0;
		while ( Index < SOPUIDListCount )
			{
			delete [] SOPUIDList [ Index ] ;
			delete [] SOPUIDListNames [ Index ] ;
			++Index;
			}
		delete [] SOPUIDList;
		delete [] SOPUIDListNames;
		SOPUIDList = NULL;
		SOPUIDListNames = NULL;
		SOPUIDListCount = 0;
		}

	if ( TransferUIDList )
		{
		Index = 0;
		while ( Index < TransferUIDListCount )
			{
			delete [] TransferUIDList [ Index ] ;
			delete [] TransferUIDListNames [ Index ] ;
			++Index;
			}
		delete [] TransferUIDList;
		delete [] TransferUIDListNames;
		TransferUIDList = NULL;
		TransferUIDListNames = NULL;
		TransferUIDListCount = 0;
		}

	if ( ApplicationUIDList )
		{
		Index = 0;
		while ( Index < ApplicationUIDListCount )
			{
			delete [] ApplicationUIDList [ Index ] ;
			delete [] ApplicationUIDListNames [ Index ] ;
			++Index;
			}
		delete [] ApplicationUIDList;
		delete [] ApplicationUIDListNames;
		ApplicationUIDList = NULL;
		ApplicationUIDListNames = NULL;
		ApplicationUIDListCount = 0;
		}
	if ( RemoteAEList )
		{
		Index = 0;
		while ( Index < RemoteAEListCount )
			{
			delete [] RemoteAEList [ Index ] ;
			delete [] RemoteAEListNames [ Index ] ;
			++Index;
			}
		delete [] RemoteAEList;
		delete [] RemoteAEListNames;
		RemoteAEList = NULL;
		RemoteAEListNames = NULL;
		RemoteAEListCount = 0;
		}
	if ( LocalAEList )
		{
		Index = 0;
		while ( Index < LocalAEListCount )
			{
			delete [] LocalAEList [ Index ] ;
			delete [] LocalAEListNames [ Index ] ;
			++Index;
			}
		delete [] LocalAEList;
		delete [] LocalAEListNames;
		LocalAEList = NULL;
		LocalAEListNames = NULL;
		LocalAEListCount = 0;
		}
	return ( TRUE );
	}

BOOL
CheckedPDU_Service	::	CanYouHandleTransferSyntax (
	TransferSyntax	&TrnSyntax)
	{
	UINT		Index;	

	if ( ! TransferUIDListCount )
		return ( TRUE );

	Index = 0;
	while ( Index < TransferUIDListCount )
		{

char test[1000];
sprintf(test, "Testing transfer: '%s' against list #%d = '%s'", (char*) TrnSyntax.TransferSyntaxName.GetBuffer(1), Index, 
(char *) UID ( TransferUIDList [ Index ]).GetBuffer(1));
DicomError(DCM_ERROR_DEBUG, test, 0);

		if ( UID ( TransferUIDList [ Index ] ) ==
			TrnSyntax.TransferSyntaxName)
			return ( TRUE );
		++Index;
		}
	return ( FALSE );
	}

BOOL
CheckedPDU_Service	::	ShouldIAcceptRemoteApTitle (
	BYTE	*ApTitle)
	{
	UINT	Index;
	char	s[64];

	if ( ! RemoteAEListCount )
		return ( TRUE );

	memset((void*)s, 0, 32);
	memcpy((void*)s, (void*)ApTitle, 16);
	Index = 0;
	while(s[Index])
		{
		if(__is_whitespace(s[Index]))
			{
			s[Index] = '\0';
			break;
			}
		++Index;
		}
	Index = 0;
	while ( Index < RemoteAEListCount )
		{
		if ( !strcmp(RemoteAEList [ Index ], s))
			return ( TRUE );
		++Index;
		}
	return ( FALSE );
	}

	
BOOL
CheckedPDU_Service	::	ShouldIAcceptLocalApTitle (
	BYTE	*ApTitle)
	{
	UINT	Index;
	char	s[64];

	if ( ! LocalAEListCount )
		return ( TRUE );

	memset((void*)s, 0, 32);
	memcpy((void*)s, (void*)ApTitle, 16);
	Index = 0;
	while(s[Index])
		{
		if(__is_whitespace(s[Index]))
			{
			s[Index] = '\0';
			break;
			}
		++Index;
		}
	Index = 0;
	while ( Index < LocalAEListCount )
		{
		if ( !strcmp(LocalAEList [ Index ], s))
			return ( TRUE );
		++Index;
		}
	return ( FALSE );
	}

BOOL
CheckedPDU_Service	::	ShouldIAcceptApplicationContext (
	ApplicationContext	&AppContext)
	{
	UINT	Index;

	if ( ! ApplicationUIDListCount )
		return ( TRUE );

	Index = 0;
	while ( Index < ApplicationUIDListCount )
		{
		if ( UID ( ApplicationUIDList [ Index ] ) ==
			AppContext.ApplicationContextName)
			return ( TRUE );
		++Index;
		}
	return ( FALSE );
	}

BOOL
CheckedPDU_Service	::	ShouldIAcceptAbstractSyntax (
	AbstractSyntax	&AbsSyntax)
	{
	UINT	Index;

	if ( ! SOPUIDListCount )
		return ( TRUE );

	Index = 0;
	while ( Index < SOPUIDListCount )
		{
		if ( UID ( SOPUIDList [ Index ] ) ==
			AbsSyntax.AbstractSyntaxName)
			return ( TRUE );
		++Index;
		}
	return ( FALSE );
	}

