
/*
19980330    ljz    Added an RTC memberfunction for loading the dicom
                   library from memory (e.g. a resource).
19980331    ljz    Added function for writing binary RTCs (function was
                   originally implemeted in 'makebrtc.cxx'
19980414    ljz    Added RTC::GetGroupElement memberfunction, to retrieve the
                   Group and Element numbers using a library-keyword.
20040930    ljz    Also return TypeCode in RTC.GetGroupElement()
20071128    mvh    Do not remove existing TypeCode if new one is unknown 
20090620    jf     Always include stdlib.h
20091028    ljz    pack(4) for __Binary_RTC_Header
20091029    ljz    More adjustments for 64-bits (reading/writing binary RTC)
20100309    bcb    Changed int to unsigned int, commented out unused variables (gcc4.2 Warnings)
20100717    mvh    Merged
20130503    lsp    Make sure that padding bytes are set to zero
20130508    lsp    Use delete [] StringTable rather than delete StringTable as indicated by Klocwork
20130619    lsp    Added fclose(fp) before return, added checks to AttachBinaryRTC(), 
                   again as indicated by Klocwork
Spectra-0016 Thu, 6 Mar 2014 15:36:06 -0300: Fix mismatched new/delete in rtc.cxx                   
20140528    lsp    Kept member initialization only in constructors and non GNUC specific code
                   in GetGroupElement()
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

/**********************************************************************
 *
 * Run-Time-Class support
 *
 **********************************************************************/
#	include	"dicom.hpp"

#include <stdlib.h> //DUCKHEAD92

BOOL
Pack(
	BYTE	c1,
	BYTE	c2,
	UINT16	&u1)
	{

	if('OB'==20290)
		u1 = (((UINT16)c1)<<8)+((UINT16)c2);
	else
		u1 = (((UINT16)c2)<<8)+((UINT16)c1);

	return ( TRUE );
	}

BOOL
Pack(
	UINT16	u1,
	BYTE	&c1,
	BYTE	&c2)
	{
	if('OB'==20290)
		{
		c1 = (u1 >> 8);
		c2 = (u1 & 0xff);
		}
	else
		{
		c2 = (u1 >> 8);
		c1 = (u1 & 0xff);
		}
	return ( TRUE );
	}
RTC	::	RTC	(
	BOOL	CD,
	char	*filename)
  : TypeCodes(NULL), CarryDescriptions(CD)
	{
	AttachRTC(filename);
	}

RTC	::	~RTC ()
	{
	DetachRTC ();
	}

BOOL
RTC	::	DetachRTC ()
	{
	UINT	Index;

	if ( TypeCodes )
		{
		Index = 0;
		while ( Index < TypeCodes->GetSize() )
			{
			if (TypeCodes->Get(Index).Description)
				{
				/*printf("Deleting Description: %4.4x, %4.4%, %x\n",
					TypeCodes->Get(Index).Group,
					TypeCodes->Get(Index).Element,
					TypeCodes->Get(Index).Description);fflush(stdout);*/
				delete [] TypeCodes->Get(Index).Description;
				}
			++Index;
			}
		delete TypeCodes;
		}
	TypeCodes = NULL;
	return ( TRUE );
	}

static	BOOL
__rtc_isHexDigit(char	ch)
	{
	if((ch>='0')&&(ch<='9'))
		return ( TRUE );
	if((ch>='a')&&(ch<='f'))
		return ( TRUE );
	if((ch>='A')&&(ch<='F'))
		return ( TRUE );
	return ( FALSE );
	}
	
static	BOOL
__rtc_iswhitespace(char	ch)
	{
	switch ( ch )
		{
		case	'\'':
		case	',':
		case	' ':
		case	'\"':
			return ( TRUE );
		}
	return ( FALSE );
	}

static	UINT16
__rtc_HexDigit(char	ch)
	{
	if((ch>='a')&&(ch<='f'))
		return(ch-'a'+10);
	if((ch>='A')&&(ch<='F'))
		return(ch-'A'+10);
	return(ch-'0');
	}

static	UINT16
ex_ascii_to_uint16(char	*string)
	{
	if(strlen(string)<3)
		return((UINT16)atoi(string));
	if((string[1]=='x')||(string[1]=='X'))
		{
		UINT16		Val = 0;
		char		*s = &string[2];

		while ( *s )
			{
			Val = (Val << 4) + __rtc_HexDigit(*s);
			++s;
			}
		return ( Val );
		}
	return ((UINT16)atoi(string));
	}

BOOL
RTC	::	AttachRTC(
	char	*filename)
	{
	FILE		*fp;
	UINT		Index, DIndex;
	Array < RTCElement >	TArray;
	RTCElement	VRT;

	char		s[1024];
	char		s1[1024];

	if ( ! filename )
		return ( FALSE );
		
	fp = fopen ( filename, "r" );
	if ( ! fp )
		return ( FALSE );

	fgets ( s, 1024 - sizeof( INT ), fp );
	if(!strncmp(s, "#!BinRTC", 8))
		{
		fclose(fp);
		return(AttachBinaryRTC(filename));
		}
	while ( ! feof ( fp ) )
		{
		if ( s[0] != '{' )
			{
			if ( s[0] != '(' )
				{
				fgets( s, 1024 - sizeof(INT), fp);
				continue;
				}
			}
		if(s[0] == '{')
			{
			// NIH / Dejarnette Data dictionary format
			Index = 1;
			while ( __rtc_iswhitespace(s[Index]) )
				{
				++Index;
				if ( ! s[ Index ] )
					{
					fgets (s, 1024 - sizeof(INT), fp );
					continue;
					}
				}
			DIndex = 0;
			while ( !__rtc_iswhitespace(s[Index]) )
				{
				s1[DIndex] = s[Index];
				++DIndex;++Index;
				if ( !s[Index] )
					{
					fgets (s, 1024 - sizeof(INT), fp );
					continue;
					}
				}
			s1[DIndex] = '\0';
			VRT.Group = ex_ascii_to_uint16(s1);

			while ( __rtc_iswhitespace(s[Index]) )
				{
				++Index;
				if ( ! s[ Index ] )
					{
					fgets (s, 1024 - sizeof(INT), fp );
					continue;
					}
				}
			DIndex = 0;
			while ( !__rtc_iswhitespace(s[Index]) )
				{
				s1[DIndex] = s[Index];
				++DIndex;++Index;
				if ( !s[Index] )
					{
					fgets (s, 1024 - sizeof(INT), fp );
					continue;
					}
				}
			s1[DIndex] = '\0';
			VRT.Element = ex_ascii_to_uint16(s1);
	
			while ( __rtc_iswhitespace(s[Index]) )
				{
				++Index;
				if ( ! s[ Index ] )
					{
					fgets (s, 1024 - sizeof(INT), fp );
					continue;
					}
				}
			DIndex = 0;
			while ( !__rtc_iswhitespace(s[Index]) )
				{
				s1[DIndex] = s[Index];
				++DIndex;++Index;
				if ( !s[Index] )
					{
					fgets (s, 1024 - sizeof(INT), fp );
					continue;
					}
				}
			s1[DIndex] = '\0';
			Pack((BYTE)s1[0],(BYTE)s1[1],VRT.TypeCode);

			while ( __rtc_iswhitespace(s[Index]) )
				{
				++Index;
				if ( ! s[ Index ] )
					{
					fgets (s, 1024 - sizeof(INT), fp );
					continue;
					}
				}
			DIndex = 0;
			while ( s[Index]!='\"' )
				{
				if(s[Index]=='\\')
					++Index;
				s1[DIndex] = s[Index];
				++DIndex;++Index;
				if ( !s[Index] )
					{
					fgets (s, 1024 - sizeof(INT), fp );
					continue;
					}
				}
			s1[DIndex] = '\0';
			if ( CarryDescriptions )
				{
				VRT.Description = new char [ Index + 1];
				strcpy(VRT.Description,  s1);
				}
			else
				VRT.Description = NULL;

			TArray.Add(VRT);
			fgets(s,1024 - sizeof(INT), fp);
			}
		else
			{
			// David Clunie style data dictionary
			VRT.Group = 0;
			VRT.Element = 0;
			VRT.TypeCode = 0;
			VRT.Description = NULL;
	
			unsigned	char	Digit;
			char	*cPtr = &s[1];

			Digit = __rtc_HexDigit(*cPtr);++cPtr;
			VRT.Group = (VRT.Group << 4) + Digit;
			Digit = __rtc_HexDigit(*cPtr);++cPtr;
			VRT.Group = (VRT.Group << 4) + Digit;
			Digit = __rtc_HexDigit(*cPtr);++cPtr;
			VRT.Group = (VRT.Group << 4) + Digit;
			Digit = __rtc_HexDigit(*cPtr);++cPtr;
			VRT.Group = (VRT.Group << 4) + Digit;
			
			while(!__rtc_isHexDigit((*cPtr)))
				++cPtr;
			
			Digit = __rtc_HexDigit(*cPtr);++cPtr;
			VRT.Element = (VRT.Element << 4) + Digit;
			Digit = __rtc_HexDigit(*cPtr);++cPtr;
			VRT.Element = (VRT.Element << 4) + Digit;
			Digit = __rtc_HexDigit(*cPtr);++cPtr;
			VRT.Element = (VRT.Element << 4) + Digit;
			Digit = __rtc_HexDigit(*cPtr);++cPtr;
			VRT.Element = (VRT.Element << 4) + Digit;

		
			// Now scan for 'VR=" and Keyword=" strings

			char	*vType = strstr(cPtr, "VR=\"");
			if(!vType)
				{
				fgets(s,1024 - sizeof(INT), fp);
				continue;	// if there's no type code.. don't bother
				}

			vType += 4;
			Pack((BYTE)vType[0],(BYTE)vType[1],VRT.TypeCode);

			if ( CarryDescriptions )
				{
				char	*Desc = strstr(vType, "Keyword=\"");

				if(Desc)
					{
//					char	s1[1024];// Already declared above. bcb

					strcpy(s1, &Desc[9]);
					char	*vTemp = strchr(s1, '\"');
					if(vTemp)
						{
						(*vTemp) = '\0';
						VRT.Description = new char[strlen(Desc)+1];
						strcpy(VRT.Description,  s1);
						}

					}
				}
			TArray.Add(VRT);
			fgets(s,1024 - sizeof(INT), fp);
			}
		}
	fclose(fp);
	if ( ! TArray.GetSize() )
		return ( FALSE );
	
	DetachRTC();
	TypeCodes = new FixedArray < UINT32, RTCElement > (TArray.GetSize(), TRUE);
	if ( ! TypeCodes )
		return ( FALSE );
	UINT32	GCode;
	while ( TArray.GetSize() )
		{
		VRT = TArray.Get(0);
		TArray.RemoveAt(0);
		GCode = MAKEUINT32(VRT.Group, VRT.Element);
		TypeCodes->Add(GCode, VRT);
		}
	TypeCodes->Sort();
	return ( TRUE );
	}

UINT16
RTC	::	RunTimeClass (
	UINT16	Group,
	UINT16	Element,
	char	*Desc)
	{
	INT			Index;
	UINT32		GCode;
	RTCElement	VRT;

	if ( Desc )
		(*Desc) = '\0';

	if ( ! TypeCodes )
		return ( 0 );

	GCode = MAKEUINT32(Group, Element);
	Index = TypeCodes->IndexOf(GCode);
	if ( Index < 0)
		return ( 0 );

	VRT = TypeCodes->Get(Index);
	if(Desc)
		if(VRT.Description)
			strcpy(Desc, VRT.Description);

	return (VRT.TypeCode);
	}

BOOL
RTC	::	RunTimeClass(VR	*vr)
	{
	UINT16	TypeCode = RunTimeClass(vr->Group, vr->Element, NULL);

	// do not remove existing TypeCode if new one is unknown 
	if ( TypeCode && !vr->TypeCode )
		vr->TypeCode = TypeCode;

	if ( TypeCode )
		return ( TRUE );
	return ( FALSE );
	}

BOOL
RTC	::	RunTimeClass(DICOMObject	*DO)
	{
	DICOMObject	TempObject;
	VR			*vr;

	while (( vr = DO->Pop() ))
		{
		RunTimeClass(vr);
		TempObject.Push(vr);
		}
	while (( vr = TempObject.Pop() ))
		{
		DO->Push(vr);
		}
	return ( TRUE );
	}

BOOL
RTC ::  GetGroupElement(RTCElement* pEntry)
{
	int			iArraySize;
	int			i;
	RTCElement	Entry;
	
	iArraySize = TypeCodes->GetSize();
	for (i=0; i< iArraySize; i++)
	{
		Entry = TypeCodes->Get(i);
		if (Entry.Description)
			if (strcmp(Entry.Description, pEntry->Description) == 0)
			{
				pEntry->Group    = Entry.Group;
				pEntry->Element  = Entry.Element;
				pEntry->TypeCode = Entry.TypeCode;
				return TRUE;
			}
	}
	return FALSE;
}


/*************** Binary RTC Load **********************/

#	define	__Binary_RTC_Header_PaddingSize	(1024 - (10 + 7*4))
#pragma pack(4)
typedef	struct	s__Binary_RTC_Header
	{
	char	Prefix[10];		// #!BinRTC<cr><lf> + typically 2 bytes padding
	UINT32	ArraySize;
	UINT32	AbsKeyTableOffset;
	UINT32	AbsKeyTableSize;
	UINT32	AbsDataTableOffset;
	UINT32	AbsDataTableSize;
	UINT32	AbsStringTableOffset;
	UINT32	AbsStringTableSize;
	char	Padding[__Binary_RTC_Header_PaddingSize];
	}	__Binary_RTC_Header;

#	define	SetFilePos(fffppp, xxx)	fseek(fffppp, xxx, 0)
#pragma pack()

BOOL
RTC	::	AttachBinaryRTC (
	char	*filename )
	{
	FILE	*fp;
	__Binary_RTC_Header	bRTCHeader;
	void*		pBuffer;
	RTCElementBIN*	pRTCElementBIN;
	unsigned int	i;
        size_t FileSize;

	fp = fopen(filename, "rb");
	if ( ! fp )
		return ( FALSE );

        fseek(fp, 0, SEEK_END);
        FileSize = ftell(fp);
        SetFilePos(fp, 0);

	fread ((char*)&bRTCHeader, 1, 1024, fp);

        // Make sure that file is properly formatted
        if (bRTCHeader.ArraySize>FileSize || bRTCHeader.AbsDataTableSize>FileSize ||
          bRTCHeader.AbsStringTableSize>FileSize || bRTCHeader.AbsKeyTableSize>FileSize)
        { fclose(fp);
          return FALSE;
        }

	TypeCodes = new FixedArray < UINT32, RTCElement > (bRTCHeader.ArraySize,TRUE);
	if ( ! TypeCodes )
		{
		fclose(fp);
		return ( FALSE );
		}

	TypeCodes->Top = bRTCHeader.ArraySize;
	TypeCodes->ArraySize = bRTCHeader.ArraySize;

	SetFilePos(fp, bRTCHeader.AbsKeyTableOffset);
	fread((char*)TypeCodes->KeyTable, 1, bRTCHeader.AbsKeyTableSize, fp);

	SetFilePos(fp, bRTCHeader.AbsDataTableOffset);
	pBuffer = malloc(bRTCHeader.AbsDataTableSize);
	if (!pBuffer)
        { 
          fclose ( fp );
          return (FALSE);
        }
	fread(pBuffer, 1, bRTCHeader.AbsDataTableSize, fp);
	pRTCElementBIN = (RTCElementBIN*)pBuffer;
	for (i=0; i<bRTCHeader.ArraySize; i++)
		{
		TypeCodes->DataTable[i].Group       = pRTCElementBIN->Group;
		TypeCodes->DataTable[i].Element     = pRTCElementBIN->Element;
		TypeCodes->DataTable[i].TypeCode    = pRTCElementBIN->TypeCode;
		TypeCodes->DataTable[i].Description = NULL;
		pRTCElementBIN ++;
		}
	free(pBuffer);

	if ( CarryDescriptions )
		{
		// If the database was not built with descriptions, then we
		// can't possibly load them... don't consider it an error though.
		// a warning.. maybe sometime in the future..
		if(bRTCHeader.AbsStringTableOffset)
			{
			char	*StringTable = new char[bRTCHeader.AbsStringTableSize];
			if(!StringTable)
				{
                                fclose ( fp );
				return ( FALSE );	// memory error
				}
			SetFilePos(fp, bRTCHeader.AbsStringTableOffset);
			fread((char*)StringTable, 1, bRTCHeader.AbsStringTableSize, fp);
			UINT	Index = 0;
			char	*TempS = StringTable;
			while ( Index < bRTCHeader.ArraySize )
				{
				if((*TempS))
					{
					TypeCodes->DataTable[Index].Description = new
						char [ strlen(TempS) + 1 ];
					strcpy(TypeCodes->DataTable[Index].Description, TempS);
					}
				TempS += (strlen(TempS) + 1);
				++Index;
				}
			delete [] StringTable;
			
			}
		}

	fclose ( fp );
	return ( TRUE );
	}

BOOL
RTC	::	AttachResourceRTC (
	char* pDict )
	{
	__Binary_RTC_Header	bRTCHeader;
	RTCElementBIN*	pRTCElementBIN;
	unsigned int	i;

	memcpy((void*)&bRTCHeader, pDict, 1024);

	TypeCodes = new FixedArray < UINT32, RTCElement > (bRTCHeader.ArraySize, TRUE);
	if ( ! TypeCodes )
		return ( FALSE );

	TypeCodes->Top = bRTCHeader.ArraySize;
	TypeCodes->ArraySize = bRTCHeader.ArraySize;

	memcpy((char*)TypeCodes->KeyTable, pDict + bRTCHeader.AbsKeyTableOffset, 
		bRTCHeader.AbsKeyTableSize);

	pRTCElementBIN = (RTCElementBIN*)(pDict + bRTCHeader.AbsDataTableOffset);
	for (i=0; i<bRTCHeader.ArraySize; i++)
		{
		TypeCodes->DataTable[i].Group       = pRTCElementBIN->Group;
		TypeCodes->DataTable[i].Element     = pRTCElementBIN->Element;
		TypeCodes->DataTable[i].TypeCode    = pRTCElementBIN->TypeCode;
		TypeCodes->DataTable[i].Description = NULL;
		pRTCElementBIN ++;
		}
	if ( CarryDescriptions )
		{
		// If the database was not built with descriptions, then we
		// can't possibly load them... don't consider it an error though.
		// a warning.. maybe sometime in the future..
		if(bRTCHeader.AbsStringTableOffset)
			{
			char	*StringTable = new char[bRTCHeader.AbsStringTableSize];
			if(!StringTable)
				{
				return ( FALSE );	// memory error
				}
			memcpy((char*)StringTable, pDict + bRTCHeader.AbsStringTableOffset, 
				bRTCHeader.AbsStringTableSize);
			UINT	Index = 0;
			char	*TempS = StringTable;
			while ( Index < bRTCHeader.ArraySize )
				{
				if((*TempS))
					{
					TypeCodes->DataTable[Index].Description = new
						char [ strlen(TempS) + 1 ];
					strcpy(TypeCodes->DataTable[Index].Description, TempS);
					}
				TempS += (strlen(TempS) + 1);
				++Index;
				}
			delete [] StringTable;
			
			}
		}

	return ( TRUE );
	}


int MkBinaryRtc(char* pszTextIn, char* pszBinaryOut, BOOL bCarryDescriptions)
	{
	unsigned int	i;
	RTC	vRTC(bCarryDescriptions);
	if(!vRTC.AttachRTC(pszTextIn))
		return(1);

	FILE	*oFile = fopen(pszBinaryOut, "wb");
	if(!oFile)
		return(2);


	__Binary_RTC_Header	bRTCHeader;

        memset(&bRTCHeader, 0, sizeof(bRTCHeader));
	strcpy(bRTCHeader.Prefix, "#!BinRTC");
	bRTCHeader.Prefix[8] = 13;
	bRTCHeader.Prefix[9] = 10;

	UINT32	RunningOffset = sizeof ( __Binary_RTC_Header );

	bRTCHeader.ArraySize = vRTC.TypeCodes->ArraySize;
	bRTCHeader.AbsKeyTableOffset = RunningOffset;
	bRTCHeader.AbsKeyTableSize = bRTCHeader.ArraySize * sizeof ( UINT32 );
	RunningOffset += bRTCHeader.AbsKeyTableSize;
	bRTCHeader.AbsDataTableOffset = RunningOffset;
	bRTCHeader.AbsDataTableSize = bRTCHeader.ArraySize * sizeof (RTCElementBIN);
	RunningOffset += bRTCHeader.AbsDataTableSize;

	char	*StringTable;
		
	StringTable = NULL;// GCC warning

	if ( bCarryDescriptions )
		{
		UINT		Index;
		UINT		TSize;

		// Ok, descriptions are still ugly to handle.  I recommend in production
		// systems not using them unless you have to.. but for those that do..

		Index = 0;
		TSize = 0;
		while ( Index < vRTC.TypeCodes->ArraySize )
			{
			if(!vRTC.TypeCodes->DataTable[Index].Description)
				++TSize;
			else
				TSize += (strlen(vRTC.TypeCodes->DataTable[Index].Description)
							+ 1);
			++Index;
			}
		StringTable = new char[TSize];

		Index = 0;
		char	*TempS = StringTable;
		while ( Index < vRTC.TypeCodes->ArraySize )
			{
			if(!vRTC.TypeCodes->DataTable[Index].Description)
				{
				(*TempS) = '\0';
				}
			else
				strcpy(TempS, vRTC.TypeCodes->DataTable[Index].Description);
			TempS += (strlen(TempS) + 1);
			delete vRTC.TypeCodes->DataTable[Index].Description;
			vRTC.TypeCodes->DataTable[Index].Description = NULL;
			++Index;
			}	
		bRTCHeader.AbsStringTableOffset = RunningOffset;
		bRTCHeader.AbsStringTableSize = TSize;
		}
	else
		{
		bRTCHeader.AbsStringTableOffset = 0;
		bRTCHeader.AbsStringTableSize = 0;
		}

	fwrite((char*)&bRTCHeader, 1, sizeof ( __Binary_RTC_Header), oFile );
	fwrite((char*)vRTC.TypeCodes->KeyTable, 1, bRTCHeader.AbsKeyTableSize, oFile);
	for (i=0; i<bRTCHeader.ArraySize; i++)
		fwrite(&(vRTC.TypeCodes->DataTable[i]), 1, sizeof(RTCElementBIN), oFile);
//	fwrite((char*)vRTC.TypeCodes->DataTable, 1, bRTCHeader.AbsDataTableSize, oFile);
	if ( bCarryDescriptions )
		{
		fwrite((char*)StringTable, 1, bRTCHeader.AbsStringTableSize, oFile);
		}
	fclose ( oFile );


	if ( StringTable )
		delete [] StringTable;

	return(0);
	}
