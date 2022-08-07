/*
20030219	ljz	Removed MessageID and Priority from the 'write' functions
			of CStoreRSP, CFindRSP and CMoveRSP
20030606	ljz	Added parameter 'QueryRetrieveLevel' to CFindRSP::Write
20030811	ljz	Do not send DDO in CMoveRSP::Write (DicomEye and OFFIS do not
			like that).
20050129	mvh	Added Write with extra vr to CEchoRSP
20051222	mvh	Do not add query level if already exists
20070308	bcb 	Made changes for big endian
20070314	mvh	Allow send of 0000,1030 (MoveOriginatorAE) and 0000,1031 (MoveOriginatorMessageID) 
			in CStoreRQ
20070330	mvh	Merged bcb's Darwin and Endian stuff
20080203	mvh	Added experimental ConfigPadAEWithZeros
20080910	bcb	Removed big endian changes from here and moved to deivr
20100309	bcb	Added double parentheses (gcc4.2 Warnings)
20100619	bcb	Added UNUSED_ARGUMENT.
20100717	mvh	Merged
20120624	mvh	Delete transfer syntax in data object of outgoing images
20120628	mvh	Undo above change that only affects 'as': would probably break 'as' transfer of jpeg
20140608	mvh	Added BYTE PCid to CStoreRSP::Write
20140611	mvh	This change is under review: pcid does not seem to get the right value
20180315	mvh	Started on C-GET
20220805        mvh     Pass through 0x9999,0x0404 from DCO to DCOR in CECHORSP (set when payload odd length)
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

#ifndef UNUSED_ARGUMENT
#define UNUSED_ARGUMENT(x) (void)x
#endif

#	include	"dicom.hpp"

/************************* C-Echo *******************************/

BOOL	CEchoRQ :: Read ( DICOMCommandObject *DCO )
	{
	if (!DCO)
		return ( FALSE );

	if (DCO->GetUINT16(0x0000, 0x0100)!=0x0030)
		return ( FALSE );	// not a C-Echo-RQ

	return ( TRUE );
	}

BOOL	CEchoRQ :: Write ( PDU_Service *PDU )
	{
	DICOMCommandObject	DCO;
	VR					*vr;
	UINT16				Temp;
	LE_UINT16			leCommand, leDSType, leMessageID;
	UID					uid;

	if (!PDU)
		return ( FALSE );

	if (!GetUID(uid))
		return ( FALSE );
	vr = new VR (0x0000, 0x0002, uid.GetSize(),
			(void*)uid.GetBuffer(1), FALSE );
	DCO.Push ( vr );
	
	leCommand = 0x0030;
	vr = new VR (0x0000, 0x0100, sizeof(UINT16), &leCommand,
			FALSE);
	DCO.Push ( vr );

	leDSType = 0x0101;
	vr = new VR (0x0000, 0x0800, sizeof(UINT16), &leDSType,
			FALSE);
	DCO.Push ( vr );

	Temp = uniq16odd();
	leMessageID = Temp;
	vr = new VR (0x0000, 0x0110, sizeof(UINT16), &leMessageID,
			FALSE);
	DCO.Push ( vr );

	if (!PDU->Write(&DCO, uid))
		return ( FALSE );

	return ( TRUE );
	}

BOOL	CEchoRSP :: Read ( DICOMCommandObject *DCO )
	{
	if ( ! DCO )
		return ( FALSE );

	if (DCO->GetUINT16(0x0000, 0x0100)!=0x8030)
		return ( FALSE );

	return ( TRUE );
	}

BOOL	CEchoRSP :: Write ( PDU_Service *PDU, DICOMCommandObject *DCO,
	UINT16	ErrorCode, VR *vrextra )
	{
	DICOMCommandObject	DCOR;
	LE_UINT16	leCommand, leError;
	VR			*vr;
	UID			uid;

	if ( ! PDU )
		return ( FALSE );
	if ( ! DCO )
		return ( FALSE );
	
	if (!GetUID(uid))
		return ( FALSE );

	while (( vr = DCO->Pop() ))
		{
		switch (vr->Element)
			{
			case	0x0002:
			case	0x0800:
				DCOR.Push ( vr );
				break;
			case	0x0110:
				vr->Element = 0x0120;
				DCOR.Push ( vr );
				break;
			case	0x0404: // odd length in payload from server command
				DCOR.Push ( vr );
				break;
			default:
				delete vr;
			}
		}
	leCommand = 0x8030;
	vr = new VR (0x0000, 0x0100, sizeof(UINT16), &leCommand, FALSE );
	DCOR.Push ( vr );

	leError = ErrorCode;
	vr = new VR (0x0000, 0x0900, sizeof(UINT16), &leError, FALSE );
	DCOR.Push ( vr );

	if (vrextra)
		DCOR.Push ( vrextra );
 
	if (!PDU->Write(&DCOR, uid))
		return ( FALSE );

	return ( TRUE );
	}

BOOL	CEchoRSP :: Write ( PDU_Service *PDU, DICOMCommandObject *DCO,
	UINT16	ErrorCode )
	{
	return ( Write ( PDU, DCO, ErrorCode, NULL ) );	// Write no vrextra
	}

BOOL	CEchoRSP :: Write (PDU_Service *PDU, DICOMCommandObject *DCO)
	{
	return ( Write ( PDU, DCO, 0x0000, NULL ) );	// Write Success, no extra
	}


/************************** C-Store ***************************/

BOOL	CStoreRQ :: Read (
	DICOMCommandObject	*DCO,
	PDU_Service			*PDU,
	DICOMDataObject *DDO )
	{
	if ( ! DCO )
		return ( FALSE );
	
	if ( DCO->GetUINT16(0x0000, 0x0100) != 0x0001 )
		return ( FALSE ); // not C-Store

	if ( DCO->GetUINT16(0x0000, 0x0800) != 0x0101 )
		{
		if ( PDU )
			{
			if ( DDO )
				{
				return ( PDU->Read( DDO ) );
				}
			}
		return ( TRUE );
		}
	// C-Store request, but no data section.. -error
	return ( FALSE );
	}

BOOL	CStoreRQ	::	Read ( DICOMCommandObject	*DCO )
	{
	return ( Read ( DCO, NULL, NULL ) );
	}

BOOL	CStoreRQ	::	Write (
	PDU_Service	*PDU,
	DICOMDataObject *DDO,
	VR *MoveMessageID, unsigned char *CallingAE)
	{
	DICOMCommandObject	DCO;
	VR					*vr;
	UINT16				Temp;
	LE_UINT16			leCommand, leDSType, leMessageID, lePriority;
	UID					uid;

	if (!PDU)
		return ( FALSE );

	if (!GetUID(uid))
		return ( FALSE );
	vr = new VR (0x0000, 0x0002, uid.GetSize(),
			(void*)uid.GetBuffer(1), FALSE );
	DCO.Push ( vr );
	
	leCommand = 0x0001;
	vr = new VR (0x0000, 0x0100, sizeof(UINT16), &leCommand,
			FALSE);
	DCO.Push ( vr );

	leDSType = 0x0102;
	vr = new VR (0x0000, 0x0800, sizeof(UINT16), &leDSType,
			FALSE);
	DCO.Push ( vr );

	Temp = uniq16odd();
	leMessageID = Temp;

	vr = new VR (0x0000, 0x0110, sizeof(UINT16), &leMessageID,
			FALSE);
	DCO.Push ( vr );

	lePriority = 0;
	vr = new VR (0x0000, 0x0700, sizeof(UINT16), &lePriority,
			FALSE);
	DCO.Push ( vr );

	vr = DDO->GetVR(0x0008, 0x0018);	// Instance
	if(vr)
		{
		VR	*vr2;

		vr2 = new VR(0x0000, 0x1000, vr->Length, (BOOL)TRUE);
		memcpy(vr2->Data, vr->Data, vr->Length);
		DCO.Push(vr2);
		}

	if (CallingAE)
		{
		VR	*vr2;
		int     len = strlen((char *)CallingAE);
		if (len&1) len++;
		vr2 = new VR(0x0000, 0x1030, len, (BOOL)TRUE);
		memcpy(vr2->Data, CallingAE, len);
		if (((char *)vr2->Data)[len-1]==0) ((char *)vr2->Data)[len-1]=' ';
		DCO.Push(vr2);
		}

	if (MoveMessageID)
		{
		VR	*vr2;
		vr2 = new VR(0x0000, 0x1031, MoveMessageID->Length, (BOOL)TRUE);
		memcpy(vr2->Data, MoveMessageID->Data, MoveMessageID->Length);
		DCO.Push(vr2);
		}

	if (!PDU->Write(&DCO, uid))
		return ( FALSE );
	if ( DDO )
		{
//		vr = DDO->GetVR(0x0002, 0x0010);	// delete tr syntax: mvh 20120624
//		if (vr) DDO->DeleteVR(vr);
	    	return ( PDU->Write ( DDO, uid ) );
		}
	return ( TRUE );
	}

BOOL	CStoreRSP	::	Read ( DICOMCommandObject	*DCO )
	{
	if ( ! DCO )
		return ( FALSE );

	if (DCO->GetUINT16(0x0000, 0x0100) != 0x8001)
		return ( FALSE ); // not a C-Store-RSP

	return ( TRUE );
	}

BOOL	CStoreRSP	::	Write (
	PDU_Service			*PDU,
	DICOMCommandObject 	*DCO,
	UINT16				ErrorCode,
	BYTE				PCid)	
	{
	VR			*vr;
	LE_UINT16	leCommand, leMessageID, leRQMessageID, leDSType,
		leErrorCode, lePriority;
	DICOMCommandObject	DCOR;
	UID					uid;


	if ( ! PDU )
		return ( FALSE );

	if ( ! DCO )
		return ( FALSE );
	
	if (!GetUID(uid))
		{
		// Try and grab the uid from the DCO
		VR		*vru = DCO->GetVR(0x0000, 0x0002);
		if ( ! vru )
			return ( FALSE );
		SetUID(uid, vru);
		}

	lePriority = 0;
	if(DCO->GetVR(0x0000, 0x0700))
		lePriority = DCO->GetUINT16( 0x0000, 0x0700 );
	leErrorCode = ErrorCode;
	leRQMessageID = DCO->GetUINT16 ( 0x0000, 0x0110 );
	leMessageID = leRQMessageID;
	leDSType = 0x0101;
	leCommand = 0x8001;

	while (( vr = DCO->Pop() ))
		{
		switch ( vr->Element )
			{
			case	0x0002:
				DCOR.Push ( vr );
				break;
			case	0x0200:
				vr->Element = 0x0300;
				DCOR.Push ( vr );
				break;
			case	0x0300:
				vr->Element = 0x0200;
				DCOR.Push ( vr );
				break;
			case	0x1000:
				DCOR.Push ( vr );
				break;
			default:
				delete vr;
			}
		}	
	vr = new VR (0x0000, 0x0100, sizeof(UINT16), &leCommand, FALSE);
		DCOR.Push ( vr );
	/* The MessageID in a RSP is now retired */
//	vr = new VR (0x0000, 0x0110, sizeof(UINT16), &leMessageID, FALSE);
//		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x0120, sizeof(UINT16), &leRQMessageID, FALSE);
		DCOR.Push ( vr );
	/* The priority is not used either */
//	vr = new VR (0x0000, 0x0700, sizeof(UINT16), &lePriority, FALSE);
//		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x0800, sizeof(UINT16), &leDSType, FALSE);
		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x0900, sizeof(UINT16), &leErrorCode, FALSE);
		DCOR.Push ( vr );

	PDU->Write ( &DCOR, uid, PCid );
	return ( TRUE );
	}

BOOL	CStoreRSP :: Write ( PDU_Service *PDU, DICOMCommandObject *DCO)
	{
	return ( Write ( PDU, DCO, 0x0000, 0 ) );
	}

/************************** C-Find ***************************/

BOOL	CFindRQ :: Read (
	DICOMCommandObject	*DCO,
	PDU_Service			*PDU,
	DICOMDataObject *DDO )
	{
	if ( ! DCO )
		return ( FALSE );
	
	if ( DCO->GetUINT16(0x0000, 0x0100) != 0x0020 )
		return ( FALSE ); // not C-Find

	if ( DCO->GetUINT16(0x0000, 0x0800) != 0x0101 )
		{
		if ( PDU )
			{
			if ( DDO )
				{
				return ( PDU->Read( DDO ) );
				}
			}
		return ( TRUE );
		}
	// C-Find request, but no data section.. -error
	return ( FALSE );
	}

BOOL	CFindRQ	::	Read ( DICOMCommandObject	*DCO )
	{
	return ( Read ( DCO, NULL, NULL ) );
	}

BOOL	CFindRQ	::	Write (
	PDU_Service	*PDU,
	DICOMDataObject *DDO)
	{
	DICOMCommandObject	DCO;
	VR					*vr;
	UINT16				Temp;
	LE_UINT16			leCommand, leDSType, leMessageID, lePriority;
	UID					uid;

	if (!PDU)
		return ( FALSE );

	if (!GetUID(uid))
		return ( FALSE );
	vr = new VR (0x0000, 0x0002, uid.GetSize(),
			(void*)uid.GetBuffer(1), FALSE );
	DCO.Push ( vr );
	
	leCommand = 0x0020;
	vr = new VR (0x0000, 0x0100, sizeof(UINT16), &leCommand,
			FALSE);
	DCO.Push ( vr );

	leDSType = 0x0102;
	vr = new VR (0x0000, 0x0800, sizeof(UINT16), &leDSType,
			FALSE);
	DCO.Push ( vr );

	Temp = uniq16odd();
	leMessageID = Temp;
	
	vr = new VR (0x0000, 0x0110, sizeof(UINT16), &leMessageID,
			FALSE);
	DCO.Push ( vr );

	lePriority = 0;
	vr = new VR (0x0000, 0x0700, sizeof(UINT16), &lePriority,
			FALSE);
	DCO.Push ( vr );

	if (!PDU->Write(&DCO, uid))
		return ( FALSE );
	if ( DDO )
		return ( PDU->Write ( DDO, uid ) );
	return ( TRUE );
	}

BOOL	CFindRSP	::	Read (
	DICOMCommandObject	*DCO,
	PDU_Service		*PDU,
	DICOMDataObject	*DDO )
	{
	if ( ! DCO )
		return ( FALSE );

	if (DCO->GetUINT16(0x0000, 0x0100) != 0x8020)
		return ( FALSE ); // not a C-Find-RSP

	if (DDO)
		{
		if (DCO->GetUINT16(0x0000, 0x0800) != 0x0101)
			{
			return ( PDU->Read ( DDO ) );
			}
		}	
	return ( TRUE );
	}

BOOL	CFindRSP	::	Read ( DICOMCommandObject	*DCO )
	{
	return ( Read ( DCO, NULL, NULL ) );
	}

BOOL	CFindRSP	::	Write (
	PDU_Service			*PDU,
	DICOMCommandObject 	*DCO,
	UINT16				ErrorCode,
	DICOMDataObject		*DDO,	
	char* QueryRetrieveLevel)
	{
	VR			*vr, *vr2;
	LE_UINT16	leCommand, leMessageID, leRQMessageID, leDSType,
		leErrorCode, lePriority;
	DICOMCommandObject	DCOR;
	UID					uid;

	if ( ! PDU )
		return ( FALSE );

	if ( ! DCO )
		return ( FALSE );

	if (!GetUID(uid))
		return ( FALSE );

	lePriority = 0;
	if(DCO->GetVR(0x0000, 0x0700))
		lePriority = DCO->GetUINT16( 0x0000, 0x0700 );
	leErrorCode = ErrorCode;
	leRQMessageID = DCO->GetUINT16 ( 0x0000, 0x0110 );
	leMessageID = leRQMessageID;
	if(DDO)
		leDSType = 0x0102;
	else
		leDSType = 0x0101;
	leCommand = 0x8020;

	// Rather than destory the original command object, just make
	// copies of the elements we need.

	if((vr = DCO->GetVR ( 0x0000, 0x0002 ) ))
		{
		vr2 = new VR ( 0x0000, 0x0002, vr->Length, TRUE );
		memcpy(vr2->Data, vr->Data, (int)vr->Length);
		DCOR.Push ( vr2 );
		}
	if((vr = DCO->GetVR ( 0x0000, 0x0300 ) ))
		{
		vr2 = new VR ( 0x0000, 0x0300, vr->Length, TRUE );
		memcpy(vr2->Data, vr->Data, (int)vr->Length);
		DCOR.Push ( vr2 );
		}
	if((vr = DCO->GetVR ( 0x0000, 0x0200 ) ))
		{
		vr2 = new VR ( 0x0000, 0x0200, vr->Length, TRUE );
		memcpy(vr2->Data, vr->Data, (int)vr->Length);
		DCOR.Push ( vr2 );
		}
	vr = new VR (0x0000, 0x0100, sizeof(UINT16), &leCommand, FALSE);
		DCOR.Push ( vr );
	/* The MessageID in a RSP is now retired */
//	vr = new VR (0x0000, 0x0110, sizeof(UINT16), &leMessageID, FALSE);
//		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x0120, sizeof(UINT16), &leRQMessageID, FALSE);
		DCOR.Push ( vr );
	/* The priority is not used either */
//	vr = new VR (0x0000, 0x0700, sizeof(UINT16), &lePriority, FALSE);
//		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x0800, sizeof(UINT16), &leDSType, FALSE);
		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x0900, sizeof(UINT16), &leErrorCode, FALSE);
		DCOR.Push ( vr );

	PDU->Write ( &DCOR, uid );
	if(DDO)
		{
		if (!DDO->GetVR(0x0008, 0x0052))
			{
			// Also send the required Quey/Retrieve level
			int	iLength;
			iLength = strlen(QueryRetrieveLevel);
			if (iLength & 0x01)
				iLength++;
			vr2 = new VR(0x0008, 0x0052, iLength, TRUE );
			memcpy(vr2->Data, QueryRetrieveLevel, iLength);
			DDO->Push(vr2);
			}
		PDU->Write ( DDO, uid );
		}
	return ( TRUE );
	}

BOOL	CFindRSP :: Write (
	PDU_Service *PDU,
	DICOMCommandObject *DCO,
	DICOMDataObject	*DDO,
	char* QueryRetrieveLevel)
	{
	if ( DDO )
		return ( Write ( PDU, DCO, 0xff00, DDO, QueryRetrieveLevel) );
	
	return ( Write ( PDU, DCO, 0x0000, NULL, QueryRetrieveLevel) );
	}

/************************** C-Move ***************************/

BOOL	CMoveRQ :: Read (
	DICOMCommandObject	*DCO,
	PDU_Service			*PDU,
	DICOMDataObject *DDO )
	{
	if ( ! DCO )
		return ( FALSE );
	
	if ( DCO->GetUINT16(0x0000, 0x0100) != 0x0021 )
		return ( FALSE ); // not C-Move

	if ( DCO->GetUINT16(0x0000, 0x0800) != 0x0101 )
		{
		if ( PDU )
			{
			if ( DDO )
				{
				return ( PDU->Read( DDO ) );
				}
			}
		return ( TRUE );
		}
	// C-Move request, but no data section.. -error
	return ( FALSE );
	}

BOOL	CMoveRQ	::	Read ( DICOMCommandObject	*DCO )
	{
	return ( Read ( DCO, NULL, NULL ) );
	}

BOOL	CMoveRQ	::	Write (
	PDU_Service	*PDU,
	DICOMDataObject *DDO,
	BYTE		*Where)
	{
	DICOMCommandObject	DCO;
	VR					*vr;
	UINT16				Temp;
	LE_UINT16			leCommand, leDSType, leMessageID, lePriority;
	BYTE				WhereACRNema [ 17 ];
	UID					uid;

	if (!PDU)
		return ( FALSE );

	if (!Where )
		return ( FALSE );

	if (!GetUID(uid))
		return ( FALSE );
	vr = new VR (0x0000, 0x0002, uid.GetSize(),
			(void*)uid.GetBuffer(1), FALSE );
	DCO.Push ( vr );
	
	leCommand = 0x0021;
	vr = new VR (0x0000, 0x0100, sizeof(UINT16), &leCommand,
			FALSE);
	DCO.Push ( vr );

	leDSType = 0x0102;
	vr = new VR (0x0000, 0x0800, sizeof(UINT16), &leDSType,
			FALSE);
	DCO.Push ( vr );

	Temp = uniq16odd();
	leMessageID = Temp;
	
	vr = new VR (0x0000, 0x0110, sizeof(UINT16), &leMessageID,
			FALSE);
	DCO.Push ( vr );

	lePriority = 0;
	vr = new VR (0x0000, 0x0700, sizeof(UINT16), &lePriority,
			FALSE);
	DCO.Push ( vr );

	strcpy((char*) WhereACRNema, "                ");
	if (ConfigPadAEWithZeros) memset((char*) WhereACRNema, 0, 16);
	memcpy(	(void*) &WhereACRNema[0],
			(void*) Where,
			strlen((char*) Where) );
	vr = new VR (0x0000, 0x0600, 16, (void*) &WhereACRNema[0],
			FALSE);
	DCO.Push ( vr );

	if (!PDU->Write(&DCO, uid))
		return ( FALSE );
	if ( DDO )
		return ( PDU->Write ( DDO, uid ) );
	return ( TRUE );
	}

BOOL	CMoveRSP	::	Read (
	DICOMCommandObject	*DCO,
	PDU_Service			*PDU,
	DICOMDataObject		*DDO )
	{
	if ( ! DCO )
		return ( FALSE );

	if (DCO->GetUINT16(0x0000, 0x0100) != 0x8021)
		return ( FALSE ); // not a C-Move-RSP

	if (DDO)
		{
		if (DCO->GetUINT16(0x0000, 0x0800) != 0x0101)
			{
			if ( PDU )
				return ( PDU->Read ( DDO ) );
			}
		}	
	return ( TRUE );
	}

BOOL	CMoveRSP	::	Read ( DICOMCommandObject	*DCO )
	{
	return ( Read ( DCO, NULL, NULL ) );
	}

BOOL	CMoveRSP	::	Write (
	PDU_Service			*PDU,
	DICOMCommandObject 	*DCO,
	UINT16				ErrorCode,
	UINT16				NumberRemaining,
	UINT16				NumberComplete,
	UINT16				NumberFailed,
	UINT16				NumberWarning,
	DICOMDataObject		*DDO)	
	{
	VR			*vr, *vr2;
	LE_UINT16	leCommand, leMessageID, leRQMessageID, leDSType,
		leErrorCode, lePriority;
	LE_UINT16	leNumberRemaining, leNumberComplete,
				leNumberFailed, leNumberWarning;
	DICOMCommandObject	DCOR;
	UID					uid;

	if ( ! PDU )
		return ( FALSE );

	if ( ! DCO )
		return ( FALSE );

	if (!GetUID(uid))
		return ( FALSE );

	lePriority = 0;
	if(DCO->GetVR(0x0000, 0x0700))
		lePriority = DCO->GetUINT16( 0x0000, 0x0700 );
	leNumberRemaining = NumberRemaining;
	leNumberComplete = NumberComplete;
	leNumberFailed = NumberFailed;
	leNumberWarning = NumberWarning;
	leErrorCode = ErrorCode;
	leRQMessageID = DCO->GetUINT16 ( 0x0000, 0x0110 );
	leMessageID = leRQMessageID;
	UNUSED_ARGUMENT(DDO);
//	if(DDO)					// ljz 20030811: do not send DDO
//		leDSType = 0x0102;
//	else
	leDSType = 0x0101;
	leCommand = 0x8021;
	
	if((vr = DCO->GetVR ( 0x0000, 0x0002 ) ))
		{
		vr2 = new VR ( 0x0000, 0x0002, vr->Length, TRUE );
		memcpy(vr2->Data, vr->Data, (int)vr->Length);
		DCOR.Push ( vr2 );
		}
	if((vr = DCO->GetVR ( 0x0000, 0x0300 ) ))
		{
		vr2 = new VR ( 0x0000, 0x0300, vr->Length, TRUE );
		memcpy(vr2->Data, vr->Data, (int)vr->Length);
		DCOR.Push ( vr2 );
		}
	if((vr = DCO->GetVR ( 0x0000, 0x0200 ) ))
		{
		vr2 = new VR ( 0x0000, 0x0200, vr->Length, TRUE );
		memcpy(vr2->Data, vr->Data, (int)vr->Length);
		DCOR.Push ( vr2 );
		}
	
	vr = new VR (0x0000, 0x0100, sizeof(UINT16), &leCommand, FALSE);
		DCOR.Push ( vr );
	/* The MessageID in a RSP is now retired */
//	vr = new VR (0x0000, 0x0110, sizeof(UINT16), &leMessageID, FALSE);
//		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x0120, sizeof(UINT16), &leRQMessageID, FALSE);
		DCOR.Push ( vr );
	/* The priority is not used either */
//	vr = new VR (0x0000, 0x0700, sizeof(UINT16), &lePriority, FALSE);
//		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x0800, sizeof(UINT16), &leDSType, FALSE);
		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x0900, sizeof(UINT16), &leErrorCode, FALSE);
		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x1020, sizeof(UINT16), &leNumberRemaining,
		FALSE);
		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x1021, sizeof(UINT16), &leNumberComplete,
		FALSE);
		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x1022, sizeof(UINT16), &leNumberFailed,
		FALSE);
		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x1023, sizeof(UINT16), &leNumberWarning,
		FALSE);
		DCOR.Push ( vr );

	PDU->Write ( &DCOR, uid );
//	if(DDO)					// ljz 20030811: do not send DDO
//		PDU->Write ( DDO, uid );
	return ( TRUE );
	}


/************************** C-Get ***************************/

BOOL	CGetRQ :: Read (
	DICOMCommandObject	*DCO,
	PDU_Service			*PDU,
	DICOMDataObject *DDO )
	{
	if ( ! DCO )
		return ( FALSE );
	
	if ( DCO->GetUINT16(0x0000, 0x0100) != 0x0010 )
		return ( FALSE ); // not C-GET

	if ( DCO->GetUINT16(0x0000, 0x0800) != 0x0101 )
		{
		if ( PDU )
			{
			if ( DDO )
				{
				return ( PDU->Read( DDO ) );
				}
			}
		return ( TRUE );
		}
	// C-Get request, but no data section.. -error
	return ( FALSE );
	}

BOOL	CGetRQ	::	Read ( DICOMCommandObject	*DCO )
	{
	return ( Read ( DCO, NULL, NULL ) );
	}

BOOL	CGetRQ	::	Write (
	PDU_Service	*PDU,
	DICOMDataObject *DDO)
	{
	DICOMCommandObject	DCO;
	VR					*vr;
	UINT16				Temp;
	LE_UINT16			leCommand, leDSType, leMessageID, lePriority;
	UID					uid;

	if (!PDU)
		return ( FALSE );

	if (!GetUID(uid))
		return ( FALSE );
	vr = new VR (0x0000, 0x0002, uid.GetSize(),
			(void*)uid.GetBuffer(1), FALSE );
	DCO.Push ( vr );
	
	leCommand = 0x0010;
	vr = new VR (0x0000, 0x0100, sizeof(UINT16), &leCommand,
			FALSE);
	DCO.Push ( vr );

	leDSType = 0x0102;
	vr = new VR (0x0000, 0x0800, sizeof(UINT16), &leDSType,
			FALSE);
	DCO.Push ( vr );

	Temp = uniq16odd();
	leMessageID = Temp;
	
	vr = new VR (0x0000, 0x0110, sizeof(UINT16), &leMessageID,
			FALSE);
	DCO.Push ( vr );

	lePriority = 0;
	vr = new VR (0x0000, 0x0700, sizeof(UINT16), &lePriority,
			FALSE);
	DCO.Push ( vr );

	if (!PDU->Write(&DCO, uid))
		return ( FALSE );
	if ( DDO )
		return ( PDU->Write ( DDO, uid ) );
	return ( TRUE );
	}

BOOL	CGetRSP	::	Read (
	DICOMCommandObject	*DCO,
	PDU_Service			*PDU,
	DICOMDataObject		*DDO )
	{
	if ( ! DCO )
		return ( FALSE );

	if (DCO->GetUINT16(0x0000, 0x0100) != 0x8010)
		return ( FALSE ); // not a C-Get-RSP

	if (DDO)
		{
		if (DCO->GetUINT16(0x0000, 0x0800) != 0x0101)
			{
			if ( PDU )
				return ( PDU->Read ( DDO ) );
			}
		}	
	return ( TRUE );
	}

BOOL	CGetRSP	::	Read ( DICOMCommandObject	*DCO )
	{
	return ( Read ( DCO, NULL, NULL ) );
	}

BOOL	CGetRSP	::	Write (
	PDU_Service			*PDU,
	DICOMCommandObject 	*DCO,
	UINT16				ErrorCode,
	UINT16				NumberRemaining,
	UINT16				NumberComplete,
	UINT16				NumberFailed,
	UINT16				NumberWarning)	
	{
	VR			*vr, *vr2;
	LE_UINT16	leCommand, leMessageID, leRQMessageID, leDSType,
		leErrorCode, lePriority;
	LE_UINT16	leNumberRemaining, leNumberComplete,
				leNumberFailed, leNumberWarning;
	DICOMCommandObject	DCOR;
	UID					uid;

	if ( ! PDU )
		return ( FALSE );

	if ( ! DCO )
		return ( FALSE );

	if (!GetUID(uid))
		return ( FALSE );

	lePriority = 0;
	if(DCO->GetVR(0x0000, 0x0700))
		lePriority = DCO->GetUINT16( 0x0000, 0x0700 );
	leNumberRemaining = NumberRemaining;
	leNumberComplete = NumberComplete;
	leNumberFailed = NumberFailed;
	leNumberWarning = NumberWarning;
	leErrorCode = ErrorCode;
	leRQMessageID = DCO->GetUINT16 ( 0x0000, 0x0110 );
	leMessageID = leRQMessageID;
	leDSType = 0x0101;
	leCommand = 0x8010;
	
	if((vr = DCO->GetVR ( 0x0000, 0x0002 ) ))
		{
		vr2 = new VR ( 0x0000, 0x0002, vr->Length, TRUE );
		memcpy(vr2->Data, vr->Data, (int)vr->Length);
		DCOR.Push ( vr2 );
		}
	if((vr = DCO->GetVR ( 0x0000, 0x0300 ) ))
		{
		vr2 = new VR ( 0x0000, 0x0300, vr->Length, TRUE );
		memcpy(vr2->Data, vr->Data, (int)vr->Length);
		DCOR.Push ( vr2 );
		}
	if((vr = DCO->GetVR ( 0x0000, 0x0200 ) ))
		{
		vr2 = new VR ( 0x0000, 0x0200, vr->Length, TRUE );
		memcpy(vr2->Data, vr->Data, (int)vr->Length);
		DCOR.Push ( vr2 );
		}
	
	vr = new VR (0x0000, 0x0100, sizeof(UINT16), &leCommand, FALSE);
		DCOR.Push ( vr );
	/* The MessageID in a RSP is now retired */
//	vr = new VR (0x0000, 0x0110, sizeof(UINT16), &leMessageID, FALSE);
//		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x0120, sizeof(UINT16), &leRQMessageID, FALSE);
		DCOR.Push ( vr );
	/* The priority is not used either */
//	vr = new VR (0x0000, 0x0700, sizeof(UINT16), &lePriority, FALSE);
//		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x0800, sizeof(UINT16), &leDSType, FALSE);
		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x0900, sizeof(UINT16), &leErrorCode, FALSE);
		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x1020, sizeof(UINT16), &leNumberRemaining,
		FALSE);
		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x1021, sizeof(UINT16), &leNumberComplete,
		FALSE);
		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x1022, sizeof(UINT16), &leNumberFailed,
		FALSE);
		DCOR.Push ( vr );
	vr = new VR (0x0000, 0x1023, sizeof(UINT16), &leNumberWarning,
		FALSE);
		DCOR.Push ( vr );

	PDU->Write ( &DCOR, uid );
	return ( TRUE );
	}


