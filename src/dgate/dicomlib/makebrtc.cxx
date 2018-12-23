/*
19990415	mvh	Note: requires changelog by ljz
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

/*****
 *
 * NOTE: Binary RTC files are *not* compatible across machines, and compilers.
 *
 * Your MUST recompile a text based RTC into a binary RTC if you switch
 * machines or compilers.  If you require machine / compiler indepdence,
 * then stick to text based RTCs.
 *
 *****/




#	include	"dicom.hpp"


int MkBinaryRtc(char* pszTextIn, char* pszBinaryOut, BOOL bCarryDescriptions)
	{

	RTC	vRTC(bCarryDescriptions);
	if(!vRTC.AttachRTC(pszTextIn))
		return(1);

	FILE	*oFile = fopen(pszBinaryOut, "wb");
	if(!oFile)
		return(2);


	__Binary_RTC_Header	bRTCHeader;

	strcpy(bRTCHeader.Prefix, "#!BinRTC");
	bRTCHeader.Prefix[8] = 13;
	bRTCHeader.Prefix[9] = 10;

	memset((void*)bRTCHeader.Padding, 0, __Binary_RTC_Header_PaddingSize);

	UINT32	RunningOffset = sizeof ( __Binary_RTC_Header );

	bRTCHeader.ArraySize = vRTC.TypeCodes->ArraySize;
	bRTCHeader.AbsKeyTableOffset = RunningOffset;
	bRTCHeader.AbsKeyTableSize = bRTCHeader.ArraySize * sizeof ( UINT32 );
	RunningOffset += bRTCHeader.AbsKeyTableSize;
	bRTCHeader.AbsDataTableOffset = RunningOffset;
	bRTCHeader.AbsDataTableSize = bRTCHeader.ArraySize * sizeof (RTCElement);
	RunningOffset += bRTCHeader.AbsDataTableSize;

	char	*StringTable;

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
	fwrite((char*)vRTC.TypeCodes->DataTable, 1, bRTCHeader.AbsDataTableSize, oFile);
	if ( bCarryDescriptions )
		{
		fwrite((char*)StringTable, 1, bRTCHeader.AbsStringTableSize, oFile);
		}
	fclose ( oFile );


	if ( StringTable )
		delete StringTable;

	return(0);
	}
