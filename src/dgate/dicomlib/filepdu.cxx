/*
20000420	ljz	Fix in MakeChapter10: type of AffectedSOPClassUID and
			RequestedSOPClassUID
20030205	ljz	Fixed FileMetaInformationVersion for big-endian machines
20030424	ljz	Put the correct TransferSyntaxUID in chapter10 files
20041108	mvh	Fix strdup memory access problem (found using $c drop file test mode)
20051229	mvh	Fixed LEAK of 68 bytes when 0002,0010 not defined
20060618	mvh	Added definition of _SH_DENYNO
20090211	mvh	Check result of Dynamic_ParseRawVRIntoDCM
20091231	bcb	Changed char* to const char* for gcc4.2 warnings
20100111	mvh	Merged
20100309	bcb	Added double parentheses (gcc4.2 Warnings)
20100309	bcb	Cast multichar as UINT16, commented out unused variables (gcc4.2 Warnings)
20100706	bcb	Fixed TransferSyntax in SaveDICOMDataObject(ACRNEMA_VR_DUMP) for testcompress.
20100726	mvh	Merged
20100823	mvh	Merged small comment fix
20130508        lsp     Fixed problem in SaveDicomDataObject() indicated by Klocwork
20220824    	mvh	Added LoadDICOMDataObjectTrunc (WIP)
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
#	include	<stdlib.h>

#ifndef _SH_DENYNO 
#define _SH_DENYNO SH_DENYNO
#endif

// Disk File I/O for PDU_Service Class

BOOL
FileBuffer	::	OpenForRead ( char	*filename )
	{
#ifndef	WINDOWS
	fp = fopen(filename, "rb");
#else
	// Open Shared mode for Windows.  Unix will allow sharing by default
	fp = _fsopen(filename, "rb", _SH_DENYWR);
#endif
	if( fp )
		return ( TRUE );
	return ( FALSE );
	}

BOOL
FileBuffer	::	OpenForWrite ( char	*filename )
	{
	fp = fopen(filename, "wb");
	if ( fp )
		return ( TRUE );
	return ( FALSE );
	}

BOOL
FileBuffer	::	Close ()
	{
	fclose(fp);
	return ( TRUE );
	}

BOOL
FileBuffer	::	SendBinary ( BYTE	*Data, UINT	Length )
	{
	fwrite((char*)Data, 1, Length, fp);
	return ( TRUE );
	}

INT
FileBuffer	::	ReadBinary ( BYTE	*Data, UINT	Length )
	{
	INT		Ret;

	//fprintf(stderr, "ReadBinary(Data, %d) : ", Length);
	if(feof(fp))
		{
		//fprintf(stderr, " -1\n");
		return ( -1 );
		}
	Ret = (INT)fread((char*)Data, 1, Length, fp);
	//fprintf(stderr, " %d\n", Ret);
	return ( Ret );
	}

DICOMDataObject	*
PDU_Service	::	LoadDICOMDataObject	(
	char	*filename )
	{
	FileBuffer		IOBuffer;
	FILE			*fp;
	char			s[256];
	DICOMDataObject	*DDO;
	UINT			Mode;
	UINT			CheckOffset;

	fp = fopen(filename, "rb");
	if(!fp)
		return ( NULL );
	fseek(fp, 128, SEEK_SET);
	fread(s, 1, 4, fp);
	s[4] = '\0';
	if(strcmp(s, "DICM"))
		{
		CheckOffset = 0;
		fseek(fp, 0, SEEK_SET);
		}
	else
		CheckOffset = 128+4;

	fread(s,1,6,fp);
	if(s[5]>10)
		Mode = TSA_EXPLICIT_LITTLE_ENDIAN;
	else
		Mode = TSA_IMPLICIT_LITTLE_ENDIAN;
	fseek(fp, CheckOffset, SEEK_SET);
		
	IOBuffer.fp = fp;

	while(IOBuffer.Buffer :: Fill(50000))
		;	// still reading from disk

	DDO = new DICOMDataObject;

	if (!Dynamic_ParseRawVRIntoDCM(IOBuffer, DDO, Mode))
		{
		delete DDO;
		DDO = NULL;
		}

	IOBuffer.Close();
	return ( DDO );
	}

DICOMDataObject	*
PDU_Service	::	LoadDICOMDataObjectTrunc	(
	char	*filename, 	UINT	trunc )
	{
	FileBuffer		IOBuffer;
	FILE			*fp;
	char			s[256];
	DICOMDataObject		*DDO;
	UINT			Mode;
	UINT			CheckOffset;
	fp = fopen(filename, "rb");
	if(!fp)	return ( NULL );
	fseek(fp, 128, SEEK_SET);
	fread(s, 1, 4, fp);
	s[4] = '\0';
	if(strcmp(s, "DICM"))
		{
		CheckOffset = 0;
		fseek(fp, 0, SEEK_SET);
		}
	else
		CheckOffset = 128+4;
	fread(s,1,6,fp);
	if(s[5]>10) Mode = TSA_EXPLICIT_LITTLE_ENDIAN;
	else Mode = TSA_IMPLICIT_LITTLE_ENDIAN;
	fseek(fp, CheckOffset, SEEK_SET);
	IOBuffer.fp = fp;
	IOBuffer.SetBreakSize(trunc);
	IOBuffer.Buffer :: Fill(trunc);
	DDO = new DICOMDataObject;
	Dynamic_ParseRawVRIntoDCM(IOBuffer, DDO, Mode);
	IOBuffer.Close();
	return ( DDO );
	}
	
#	define	_LittleEndianUID	"1.2.840.10008.1.2"
//#	define	_ImplementationUID	"none yet"
//#	define	_ImplementationVersion	"0.1AlphaUCDMC "
//#	define	_SourceApplicationAE	"none            "

BOOL
PDU_Service	::	MakeChapter10 (DICOMDataObject	*DDOPtr,const char* pszTransferSyntaxUID)
	{
	VR		*vr, *vr1;
//	UINT16	tuint16;
//	VR	*TempVR;
//	char	s[140];
	char	_ImplementationUID[128];
	char	_ImplementationVersion[128];
	char	_SourceApplicationAE[32];
	ImplementationVersion	IV;
	ImplementationClass		IC;
	DICOMDataObject			DDOTemp;

	GetImplementationClass(IC);
	GetImplementationVersion(IV);
	strcpy(_ImplementationUID, (char*)IC.ImplementationName.GetBuffer(1));
	strcpy(_ImplementationVersion, (char*)IV.Version.GetBuffer(1));
	strcpy(_SourceApplicationAE, (char*)AAssociateRQ::CallingApTitle);
	if (_SourceApplicationAE[0] == ' ')
		strcpy(_SourceApplicationAE, "UCDMC_TOOLKIT ");

	// Strip away the existing Part 10 header..
	while((vr = DDOPtr->Pop()))
		{
		if(vr->Group!=0x0002)
			DDOTemp.Push(vr);
		else
			delete vr;
		}
	while((vr = DDOTemp.Pop()))
		DDOPtr->Push(vr);

	vr = DDOPtr->GetVR(0x0002, 0x0001);	// always null
	if(!vr)
		{
		// This does not contain the C10 Header yet, so we need to
		// constuct it
		vr = new VR(0x0002, 0x0001, 0x0002, TRUE);
		/* The FileMetaInformationVersion */
//		tuint16 = 0x0100;
//		memcpy(vr->Data, (void*)&tuint16, 2);
		/* First byte should be 0, second should be 1
		   Thanx to William Peterson [wfpeterson@kinetixresources.com]
		*/
		((char*)vr->Data)[0] = 0;
		((char*)vr->Data)[1] = 1;
		vr->TypeCode = 'OB';
		DDOPtr->Push(vr);
		vr = DDOPtr->GetVR(0x0008, 0x0016);
		if(vr)
			{
			vr1 = new VR(0x0002, 0x0002, vr->Length, TRUE);
			memcpy(vr1->Data, vr->Data, vr->Length);
			/* Fix: vr changed to vr1.
			   Thanx to Gunter Zeilinger & Michael Hofer [M_Hofer@gmx.net]
			*/
			vr1->TypeCode = 'UI';
			DDOPtr->Push(vr1);
			}
		vr = DDOPtr->GetVR(0x0008, 0x0018);
		if(vr)
			{
			vr1 = new VR(0x0002, 0x0003, vr->Length, TRUE);
			memcpy(vr1->Data, vr->Data, vr->Length);
			/* Fix: vr changed to vr1.
			   Thanx to Gunter Zeilinger & Michael Hofer [M_Hofer@gmx.net]
			*/
			vr1->TypeCode = 'UI';
			DDOPtr->Push(vr1);
			}
//		vr = new VR(0x0002, 0x0010, strlen(_LittleEndianUID), TRUE);
		vr = new VR(0x0002, 0x0010, strlen(pszTransferSyntaxUID), TRUE);
		memset(vr->Data, 0, vr->Length);
//		memcpy(vr->Data, (void*)_LittleEndianUID, strlen(_LittleEndianUID));
		memcpy(vr->Data, (void*)pszTransferSyntaxUID, strlen(pszTransferSyntaxUID));
		vr->TypeCode = 'UI';
		DDOPtr->Push(vr);
		vr = new VR(0x0002, 0x0012, strlen(_ImplementationUID), TRUE);
		memset(vr->Data, 0, vr->Length);
		memcpy(vr->Data, (void*)_ImplementationUID, strlen(_ImplementationUID));
		vr->TypeCode = 'UI';
		DDOPtr->Push(vr);
		vr = new VR(0x0002, 0x0013, strlen(_ImplementationVersion), TRUE);
		memset(vr->Data, 0, vr->Length);
		memcpy(vr->Data, (void*)_ImplementationVersion, strlen(_ImplementationVersion));
		vr->TypeCode = 'SH';
		DDOPtr->Push(vr);
		/* Following VR is type 3 (not obligatory), and the lib does not know
		   what AE has received the image. The code above where AAssociateRQ is
		   used to retrieve the AE does not work: this PDU is not the same as
		   the PDU that has receiced the image...
		*/
/*
		vr = new VR(0x0002, 0x0016, strlen(_SourceApplicationAE), TRUE);
		memset(vr->Data, 0, vr->Length);
		memcpy(vr->Data, (void*)_SourceApplicationAE, strlen(_SourceApplicationAE));
		vr->TypeCode = 'AE';
		DDOPtr->Push(vr);
*/
		}
	return ( TRUE );
	}


BOOL
PDU_Service	::	SaveDICOMDataObject (
	char	*filename,
	UINT	Format,
	DICOMDataObject	*DDO )
	{
	FileBuffer		IOBuffer;
	FILE			*fp;
	char			s[264];
	char			TransferSyntaxUID[256];
	UID			uid;
	VR*			pVR;

	switch ( Format )
		{
		case	ACRNEMA_VR_DUMP:
			fp = fopen(filename, "wb");
			if(!fp)
				return ( FALSE );
// Fix the TransferSyntax if there and wrong. bcb 20100706
// This makes test compression work and does no harm on a correct file!
			pVR = DDO->GetVR(0x0002, 0x0010);
			if (pVR && pVR->Length && pVR->Length != 18)// Implicit always 18.
			{
				pVR->ReAlloc(18);
				memcpy((char*)pVR->Data,"1.2.840.10008.1.2\0", 18);
			}
			ImplicitLittleEndian_ParseDCMIntoRawVR(DDO, IOBuffer);
			IOBuffer.fp = fp;
			IOBuffer.Buffer::Flush();
			IOBuffer.Close();
			return ( TRUE );
		case	DICOM_CHAPTER_10_IMPLICIT:
			fp = fopen(filename, "wb");
			if(!fp)
				return ( FALSE );

			MakeChapter10(DDO);
			ImplicitLittleEndian_ParseDCMIntoRawVR(DDO, IOBuffer);
			memset((void*)s, 0, 256);
			strcpy(&s[128], "DICM");
			fwrite(s, 1, 128 + 4, fp);
			IOBuffer.fp = fp;
			IOBuffer.Buffer::Flush();
			IOBuffer.Close();
			return ( TRUE );
		case	DICOM_CHAPTER_10_EXPLICIT:

			fp = fopen(filename, "wb");
			if(!fp)
				return ( FALSE );

			/* See what TransferSyntax was used */
			pVR = DDO->GetVR(0x0002, 0x0010);
			if (pVR && pVR->Data && (((char*)pVR->Data)[0] != 0))
			{ memset(TransferSyntaxUID, 0, 256);
                          memcpy(TransferSyntaxUID, (char*)pVR->Data, pVR->Length);
			}
			else
			{ /* When undefined, use implicit little endian */
			  char *p=GetTransferSyntaxUID(TSA_IMPLICIT_LITTLE_ENDIAN);
                          if (p)
                          { strcpy(TransferSyntaxUID, p);
			    free(p);	// mvh 20051230: LEAK
                          }
                          else
                          { fclose(fp);
                            return FALSE;
                          }
			}
			MakeChapter10(DDO, TransferSyntaxUID);

			Dynamic_ParseDCMIntoRawVR(DDO, IOBuffer, TSA_EXPLICIT_LITTLE_ENDIAN);

			memset((void*)s, 0, 256);
			strcpy(&s[128], "DICM");
			fwrite(s, 1, 128 + 4, fp);
			IOBuffer.fp = fp;
			IOBuffer.Buffer::Flush();
			IOBuffer.Close();
			return ( TRUE );

		}
	return ( FALSE );
	}

