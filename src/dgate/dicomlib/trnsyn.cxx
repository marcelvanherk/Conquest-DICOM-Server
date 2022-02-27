/*
19980409   ljz   Fixed possible transition between presence and absence
                 of TypeCodes in 'Dynamic_ParseRawVRIntoDCM'
19990904   ljz   Treat type UN same as OB,OW and SQ when reading Explicit files.
20001106   mvh   Replaced == operator by strcmp
20010905   ljz   Several fixes: UCDMC should NOW be able to read ExplicitBigEndian files
20011220   ljz   Fixes in Sequence and Item delimiters
20030122   ljz   Fixed small leak in Dynamic_ParseRawVRIntoDCM
20030205   ljz   Support new datatype 'UT' (unlimited text). Thanx to William Peterson
20030424   ljz   Changedprototype of ParseDCMIntoRawVR
		 Fixed huge bug in sending of ExplicitLittleEndian sequences
20030627   mvh   Fix for MSC 4.2
20030725   ljz   Keep GroupLength for group 0x00 (Radworks 6.0 was choking in this)
20040406   ljz   Make TypeCode 'UN' if not known, and conversion to Explicit is wanted
20041102   ljz   Added some entries in TransferSyntaxAlias table
20050112   ljz   Fixed huge bug in Explicit_ParseDCMIntoRawVR, causing data-loss. Thanx to Goran Peros.
20051217   mvh   Use new VR::ReAlloc method to replace data in VR
20051218   mvh   Use memcpy to enter UID (extra 0 will be available: end of string)
20051219   mvh   Use ReAlloc instead of new BYTE[] to fill VR data
20051229   mvh   Moved Push in ParseRawVRIntoDCM so that not done when VR exists
20070308   bcb   Added big endian stuff
20071031   mvh   Removed some commented out code
20071108   ljz   In Dynamic_ParseDCMIntoRawVR(), let group 0x08 behave the same as all other groups
                 greater than 0x02.
20071118   mvh   NOTE: previous change is not in release 1.4.13
20071128   mvh   Add TypeCodes if needed using AttachedRTC->RunTimeClass
20080913   mvh   Fix in bigendian input: interpret group 0 as littleendian
20090211   mvh   Added many checks on corrupt dicom data during parsing; removed blocked out code
20090212   mvh   Fixed a problem in the check code if some (but not all) group lenghts are missing
20090215   mvh	 Improved formatting of lenght error message
20090620   jf    Added <stdlib.h>
20090824   mvh   Resume parsing after invalid element length if length was not too big
20100102   mvh   Merged bcb changes for bigendian
20100111   mvh   bcb moved setting 0002,0010 to work for all transfers in ParseRawVRIntoDCM: fix for osiris
		 Added JASPER stuff; const
20100118   mvh   Always allow jp2k 
20100119   bcb   Put HAVE_LIBJASPER back in (unfortunately that creates a dependency between dicomlib and dgate)
20100119   bcb   Changed Dynamic_ParseRawVRIntoDCM and ParseImagePixelDataRawToDCM to read NovaRad files and remove the errors.
20100121   mvh   Merged but put into NOVARAD_FIX define; not bcb alsp prints which parser reports errors
20100125   mvh   Linux warnings.
20100224   bcb   Changed HAVE_LIBJASPER to HAVE_J2K for OpenJPEG and fixed more warnings (-Wall)
20100309   bcb   Added double parentheses (gcc4.2 Warnings)
20100309   bcb   Changed int to UINT, commented out unused variables (gcc4.2 Warnings)
20100419   bcb   Fixed Dynamic_ParseRawVRIntoDCM big endian
20100619   bcb   Made VRBuffer local lVRBuffer.
20100707   mvh   Merged
20100718   mvh   Looked at and rejected the date change: you can use importconverters for this job
20100722   mvh   iorgmode change breaks the reader for me!
20100724   bcb   Put back date change with DATE_FIX inclosing it.
20101001   mvh   Allow CurrentGroupLength==0 as warning
20103020   mvh   Rollback of dicom decoder to 1.4.16beta4 version
20110231   mvh   allow some parsing errors when DicomError handler not installed
20105001   mvh   This is the rolled back dicom decoder to 1.4.16beta4 version; with dummy FALSE passed to Implicit_ParseRawVRIntoDCM
20105001   mvh   It fixes a small leak that fragments memory
20110502   mvh   merged IgnoreDicomError change (20110231)
20111020   mvh   Enabled NOVARAD_ODD_FIX for rubo demo images; added warning
20120723   mvh   bcb fixed datefix (extra ; after if)
20130514   bcb   Fixed Big Endian parse, fixed warnings with strnlen32u()
20130523   mvh   Merged Big Endian parse fix not strnlen32u()
20130619   lsp   Fixed problem in GetTransferSyntaxUID() indicated by Klocwork
20130711   lsp   Merged
20131207   bcb   Fixed Jpeg_LS
20140125   mvh   Set TypeCode to SQ if SQObjectArray defined and TypeCode was UN
20140319   mvh   Allow MPEG encoding
20140608   mvh 	 Blocked MPEG for now
20140611 lsp+mgw I16 macro to replace multicharacter literals
20140617 mgw+lsp Reverted I16(): considered a major change by mvh
20141111   mvh   Added type 'OF' with 32 bits length
20141230   mvh   Add proper byte swapping to read and write bigendian; 
                 set transfer syntax to littleendian after read bigendian and swap
20150629   mvh	 If encoding short dicom tag with length > 64k make it 'UN'
20151207   bcb   put DATE_FIX where it belonges.
20160221   mvh	 Imported bcb changes 20131207 20151207
20160229   mvh   Removed #ifdefs for CharLS and J2k
20170827   mvh   Do not insert TransferSyntax into sequences, use UI instead of IU
20181124   mvh   Back to IU is in preamble
20201223   mvh   check length of VR < lVRBuffer.GetIncomingSize() to avoid crash on corrupted files
20210110   mvh   Lenght error conditional, returns possibly truncated object if NoDicomCheck=1
20210113   mvh   Added new types UR, UC, OD and OV with 32 bits VR length; added swapping of OD and OV
20210118   mvh   WIP: Added code to read implicitly coded sequences in explicit content
20210118   mvh   Every length exceeding warning phrased differently for easy of finding
*/

/*
bcb: Bruce Barton http://www.suelix.com/index.php/contact
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
#	include	<stdlib.h> //DUCKHEAD92


// A special thanks to Terry Rosenbaum at MSU for the countless bug fixes
// in the sequence stuff.


// These two routines ParseRawVRIntoDCM and ParseDCMIntoRawVR represent the support
// side of the Transfer Syntax negiotian.  If you plan on supporting anything beyond
// base level DICOM Little Endian, then you need to override these routines, and 
// provide the appropiate parseing mechanisms.

// This should only be called if the transfer syntax is the default DICOM transfer
// syntax. 1.2.840.10008.1.2

// Note that this routine is recursive in nature.

typedef	struct	_TransferSyntaxAlias
	{
	const char	*TransferSyntaxUID;
	unsigned int 	Alias;
	}	TransferSyntaxAlias;

// Transfer Syntax Aliases.  Currently, we don't try and figure out how
// to decode the JPEG compressed transfer syntaxes.  We can read/write them
// assuming that someother process can encode/decode the JPEG sections.
//
// The order these are placed in the table is very important.  The ones
// at the top are tryed first.  Ie, the syntax encoder will default to
// sending the top most transfer synax.  In this case, that is Implicit
// Little Endian.
TransferSyntaxAlias	TransferSyntaxAliases[] =
	{
		{ "1.2.840.10008.1.2", TSA_IMPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.1", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.2", TSA_EXPLICIT_BIG_ENDIAN },
		{ "1.2.840.10008.1.2.5", TSA_EXPLICIT_LITTLE_ENDIAN },		// RLE lossless
		{ "1.2.840.10008.1.2.4.50", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.51", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.52", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.53", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.54", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.55", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.56", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.57", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.58", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.59", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.60", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.61", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.62", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.63", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.64", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.65", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.66", TSA_EXPLICIT_LITTLE_ENDIAN },
		{ "1.2.840.10008.1.2.4.70", TSA_EXPLICIT_LITTLE_ENDIAN },	// JPEG lossless
		{ "1.2.840.10008.1.2.4.80", TSA_EXPLICIT_LITTLE_ENDIAN },	// JPEGLS Lossless
		{ "1.2.840.10008.1.2.4.81", TSA_EXPLICIT_LITTLE_ENDIAN },	// JPEGLS Lossy
		{ "1.2.840.10008.1.2.4.90", TSA_EXPLICIT_LITTLE_ENDIAN },	// JPEG2000 LosslessOnly
		{ "1.2.840.10008.1.2.4.91", TSA_EXPLICIT_LITTLE_ENDIAN },	// JPEG2000
#if 0
		{ "1.2.840.10008.1.2.4.100", TSA_EXPLICIT_LITTLE_ENDIAN },	// MPEG2 Main Profile Main Level   
		{ "1.2.840.10008.1.2.4.102", TSA_EXPLICIT_LITTLE_ENDIAN },	// MPEG-4 AVC/H.264 High Profile / Level 4.1   
		{ "1.2.840.10008.1.2.4.103", TSA_EXPLICIT_LITTLE_ENDIAN },	// MPEG-4 AVC/H.264 BD-compatible High Profile / Level 4.1 
#endif

		{ NULL, 0 }
	};

#define NOVARAD_ODD_FIX

void SwapBigEndianData(VR* pVR);

// Decapsulate image pixel data into an encapsulated format.
BOOL
PDU_Service	::	ParseImagePixelDataRawToDCM(
	LinkedBuffer	&lVRBuffer,
	DICOMObject	*DCMObject)
	{
#ifdef NOVARAD_ODD_FIX
	BOOL	novaRadOdd;//Used for bad NovaRad DCM files.
#endif

	while ( lVRBuffer.GetIncomingSize() )
		{
		VR	*vr	= new VR;
#ifdef NOVARAD_ODD_FIX
		novaRadOdd = FALSE;
#endif

		lVRBuffer >> vr->Group;
		lVRBuffer >> vr->Element;
		lVRBuffer >> vr->Length;

		if (vr->Length>lVRBuffer.GetIncomingSize())
			{
			if (DicomError(DCM_ERROR_PARSE, "Length exceeds remaining file size: %08x\n", vr->Length))
			  return FALSE;
			return ( TRUE );
			}
	
		if ( (vr->Group == 0xfffe) &&
			 (vr->Element == 0xe0dd))
			{
			delete vr;
			return ( TRUE );	// the end
			}

		if ( (vr->Group == 0xfffe) &&
			 (vr->Element == 0xe000))	// hopefully...
			{
			DICOMDataObject	*DDO = new DICOMDataObject;
#ifdef NOVARAD_ODD_FIX
			if(vr->Length & 1) 
				{
				novaRadOdd = TRUE;//Bad bad NovaRad!
				DicomError(DCM_ERROR_DEBUG, "Odd VR length: %08x; attempted to fix\n", vr->Length);
				DicomError(DCM_ERROR_DEBUG, "ParseImagePixelDataRawToDCM In image data: %08x\n", (vr->Group<<16)+vr->Element);
				}
#endif
			vr->ReAlloc(vr->Length);
#ifdef NOVARAD_ODD_FIX
			if(novaRadOdd)(vr->Length)--;// Read an odd length.
#endif
			lVRBuffer.Read((BYTE*)vr->Data, vr->Length);

#ifdef NOVARAD_ODD_FIX
			if(novaRadOdd)
			{
				((char *)(vr->Data))[vr->Length] = 0;//Fix the end.
				(vr->Length)++;//Fix the length.
			}
#endif

			if (lVRBuffer.GetIncomingEndian()==BIG_ENDIAN)
				SwapBigEndianData(vr);

			DDO->Push(vr);
			DCMObject->Push(DDO);
			continue;
			}

		// protocol error? or maybe something encoded before an image start..
		// whatever it is.. we dont' care.. do our best..
		vr->ReAlloc(vr->Length);
		lVRBuffer.Read((BYTE*)vr->Data, vr->Length);
		if (lVRBuffer.GetIncomingEndian()==BIG_ENDIAN)
			SwapBigEndianData(vr);
		delete vr;		
		}	

	DicomError(DCM_ERROR_PARSE, "Mis-encoded pixel data during load of DCM file\n", 0);
	return ( FALSE );	// mis-encoded
	}

// Encapsulate image pixel data into an encapsulated format.
BOOL
PDU_Service	::	ParseImagePixelDataDCMToRaw(
	Array < DICOMDataObject *>	*ADDO,
	LinkedBuffer			&lVRBuffer)
	{
	VR		*vr;
	UINT	Index = 0;
	DICOMDataObject	*DDO;
	
	while ( Index < ADDO->GetSize() )
		{
		DDO = ADDO->Get(Index);
		++Index;
		while((vr = DDO->Pop()))
			{
			if ((vr->Group == 0xfffe) &&
				(vr->Element == 0xe000))
				break;
			delete vr;
			}
		if(!vr)
			{
			// messed up encapsulated
			continue;
			}
		lVRBuffer << vr->Group;
		lVRBuffer << vr->Element;
		lVRBuffer << vr->Length;
		if (lVRBuffer.GetIncomingEndian()==BIG_ENDIAN)
			SwapBigEndianData(vr);
		if(vr->Length)
			lVRBuffer.Write((BYTE*)vr->Data, vr->Length);
		if (lVRBuffer.GetIncomingEndian()==BIG_ENDIAN)
			SwapBigEndianData(vr);
		delete vr;
		}

	VR	vrs;

	vrs.Group = 0xfffe;
	vrs.Element = 0xe0dd;
	vrs.Length = 0;

	lVRBuffer << vrs.Group;
	lVRBuffer << vrs.Element;
	lVRBuffer << vrs.Length;

	return ( TRUE );	
	}


BOOL	PDU_Service	::	ParseRawVRIntoDCM(BYTE	PCID, LinkedBuffer	&lVRBuffer, DICOMObject	*DCMObject)
	{
	UID	uid;
	UINT	Index;
	VR	*pVR;

	if(!GetTransferSyntaxUID(PCID, uid))
		{
		DicomError(DCM_ERROR_PARSE, "no UID associated with PCID %d during load of DCM file in ParseRawVRIntoDCM\n", PCID);
		return ( FALSE );	// Error no UID associated with PCID
		}

	Index = 0;
	while ( TransferSyntaxAliases[Index].TransferSyntaxUID )
		{
		if(strcmp((char *)(uid.GetBuffer(0)), TransferSyntaxAliases[Index].TransferSyntaxUID)==0)
			{
			/* LJ: Store the used TransferSyntax for later use (in database
			   and) when we want to write a Chapter10 file (bcb moved here).
			*/
			pVR = DCMObject->GetVR(0x0002, 0x0010);
			if (pVR)
				{
				pVR->ReAlloc(strlen((char*)uid.GetBuffer(0)));
				memcpy((char*)pVR->Data, (char*)uid.GetBuffer(0), pVR->Length);
				}
				else
				{
				pVR = new VR(0x0002, 0x0010, strlen((char*)uid.GetBuffer(0)),
				strdup((char*)uid.GetBuffer(0)), TRUE); 
				DCMObject->Push(pVR);
				}

			switch (TransferSyntaxAliases[Index].Alias)
				{
				case	TSA_IMPLICIT_LITTLE_ENDIAN:
					if(!ImplicitLittleEndian_ParseRawVRIntoDCM(
						lVRBuffer,DCMObject))
						return ( FALSE );
					break;
				case	TSA_EXPLICIT_LITTLE_ENDIAN:
					if(!ExplicitLittleEndian_ParseRawVRIntoDCM(
						lVRBuffer,DCMObject))
						return ( FALSE );
					break;
				case	TSA_EXPLICIT_BIG_ENDIAN:
					if(!ExplicitBigEndian_ParseRawVRIntoDCM(
						lVRBuffer,DCMObject))
						return ( FALSE );
					break;

				default:
					DicomError(DCM_ERROR_PARSE, "Internal error during load of DCM file in ParseRawVRIntoDCM\n", 0);
					return ( FALSE );	// internal error
				}
			return ( TRUE );
			}
		++Index;
		}
	return ( FALSE );	// Cannot Decode
	}

// Routine looks in the accepted transfer syntax table for a transfer
// syntax to use.
BYTE	PDU_Service	::	ParseDCMIntoRawVR(DICOMObject	*DCMObject, LinkedBuffer	&lVRBuffer)
	{
//	VR		*vr;
	UID		uid;
	BYTE	PCID;
	UINT					Index;

	if ( ! DCMObject )
		return ( FALSE );

	Index = 0;
	while (TransferSyntaxAliases[Index].TransferSyntaxUID)
		{
		uid.Set(TransferSyntaxAliases[Index].TransferSyntaxUID);
		if((PCID=GetAcceptedPCIDForTransferSyntax(uid)))
			{
			switch (TransferSyntaxAliases[Index].Alias)
				{
				case	TSA_IMPLICIT_LITTLE_ENDIAN:
					if(!ImplicitLittleEndian_ParseDCMIntoRawVR(
						DCMObject,lVRBuffer))
						return ( 0 );
					break;
				case	TSA_EXPLICIT_LITTLE_ENDIAN:
					if(!ExplicitLittleEndian_ParseDCMIntoRawVR(
						DCMObject,lVRBuffer))
						return ( 0 );
					break;
				case	TSA_EXPLICIT_BIG_ENDIAN:
					if(!ExplicitBigEndian_ParseDCMIntoRawVR(
						DCMObject,lVRBuffer))
						return ( 0 );
					break;
				default:
					return ( 0 );	// internal error
				}
			return ( PCID );
			}
		++Index;
		}
	return ( 0 );
	}

BYTE	PDU_Service	::	ParseDCMIntoRawVR(DICOMObject	*DCMObject, LinkedBuffer	&lVRBuffer, UID	&AbsUID, BOOL bIsDataObject)
	{
//	VR		*vr;
	UID		uid;
	BYTE	PCID;
	UINT					Index;

	if ( ! DCMObject )
		return ( FALSE );

	Index = 0;
	while (TransferSyntaxAliases[Index].TransferSyntaxUID)
		{
		uid.Set(TransferSyntaxAliases[Index].TransferSyntaxUID);
		if((PCID=GetPresentationContextID(AbsUID, uid)))
			{
			if (!bIsDataObject)
				{	// CommandObject always ImplicitLittleEndian
				if(!ImplicitLittleEndian_ParseDCMIntoRawVR(
					DCMObject,lVRBuffer))
				return ( 0 );	// internal error
				}
			else
				switch (TransferSyntaxAliases[Index].Alias)
				{
				case	TSA_IMPLICIT_LITTLE_ENDIAN:
					if(!ImplicitLittleEndian_ParseDCMIntoRawVR(
						DCMObject,lVRBuffer))
						return ( 0 );
					break;
				case	TSA_EXPLICIT_LITTLE_ENDIAN:
					if(!ExplicitLittleEndian_ParseDCMIntoRawVR(
						DCMObject,lVRBuffer))
						return ( 0 );
					break;
				case	TSA_EXPLICIT_BIG_ENDIAN:
					if(!ExplicitBigEndian_ParseDCMIntoRawVR(
						DCMObject,lVRBuffer))
						return ( 0 );
					break;
				default:
					return ( 0 );	// internal error
				}
			return ( PCID );
			}
		++Index;
		}
	return ( 0 );
	}




BOOL	PDU_Service	::	ImplicitLittleEndian_ParseRawVRIntoDCM(
	LinkedBuffer	&lVRBuffer,
	DICOMObject	*DCMObject)
	{
	lVRBuffer.SetIncomingEndian(LITTLE_ENDIAN);
	return ( Implicit_ParseRawVRIntoDCM(lVRBuffer, DCMObject, FALSE) );
	}

BOOL	PDU_Service	::	ImplicitLittleEndian_ParseDCMIntoRawVR (
	DICOMObject	*DCMObject,
	LinkedBuffer	&lVRBuffer)
	{
	lVRBuffer.SetOutgoingEndian ( LITTLE_ENDIAN );
	return ( Implicit_ParseDCMIntoRawVR(DCMObject, lVRBuffer) );
	}

BOOL	PDU_Service	::	Implicit_ParseRawVRIntoDCM(LinkedBuffer	&lVRBuffer, DICOMObject	*DCMObject, BOOL dum)
	{
	VR	*vr = NULL;
//	UINT32	tuint32, tuint32_2;
//	DICOMObject	*EBO;		// Embedded sequence Object

	unsigned int	CurrentGroup = 0;
	unsigned int	CurrentElement = 0;
	unsigned int	CurrentGroupLength = 0xffffffff;
	
	if ( ! DCMObject )
		return ( FALSE );	// passed a NULL object pointer

	while ( lVRBuffer.GetIncomingSize())
		{
		vr = new VR;
		if ( ! vr )
			return ( FALSE );	// memory error
		
		lVRBuffer >> vr->Group;
		lVRBuffer >> vr->Element;

		if (vr->Group < CurrentGroup)
			{
			if (DicomError(DCM_ERROR_PARSE, "(Imp) Encountered an invalid group order during load of DCM file (after %08x)\n", (CurrentGroup<<16)+CurrentElement))
				return FALSE;
			}

		if (vr->Group != CurrentGroup)
			{
			CurrentGroup = vr->Group;
			CurrentElement = 0;
			CurrentGroupLength = 0xffffffff;
			}

		if (vr->Element < CurrentElement)
			{
			if (DicomError(DCM_ERROR_PARSE, "(Imp) Encountered an invalid element order during load of DCM file (in %08x)\n", (CurrentGroup<<16)+CurrentElement))
				return FALSE;
			}

		CurrentElement = vr->Element;

		if ( vr->Group == 0xfffe )
			{
			// Ok, this is a deliminator type of item.
			// handled special.  Always implicit
			lVRBuffer >> vr->Length;

			if ( (vr->Element == 0xe0dd)||
				 (vr->Element == 0xe00d) )	// implicit terminmator
				{
				delete vr;
				return ( TRUE );
				}

			if ( vr->Length == 0xffffffff)
				{
				// Implicit length.. go until deliminator
				vr->Length = 0;delete vr;
				
				DICOMDataObject *DDO = new DICOMDataObject;
				if ( ! Implicit_ParseRawVRIntoDCM ( lVRBuffer, DDO, FALSE ) )
					{
					delete DDO;
					return ( FALSE );
					}
				DCMObject->Push(DDO);
				continue;
				}
			else if (vr->Length > CurrentGroupLength || (vr->Length&1))
				{
				BOOL b=DicomError(DCM_ERROR_PARSE, "Implicit Parse encountered an invalid element length during load of DCM file (in %08x)\n", (CurrentGroup<<16)+CurrentElement);
				DicomError(DCM_ERROR_PARSE, "Length = %d\n", vr->Length);
				if (vr->Length > CurrentGroupLength) 
					DicomError(DCM_ERROR_PARSE, "Group Length = %u\n", CurrentGroupLength);
				if (b && vr->Length > 0xffff && CurrentGroupLength!=0) return FALSE; // pass if not dangerous
				DicomError(DCM_ERROR_PARSE, "Continuing parsing", 0);
				}

			if (vr->Length>lVRBuffer.GetIncomingSize())
				{
				if (DicomError(DCM_ERROR_PARSE, "Length exceeding remaining file size: %08x\n", vr->Length))
					return FALSE;
				return ( TRUE );
				}

			// Ok, an explicit length.. sequence item begin?
			if ( vr->Element == 0xe000 )
				{
				vr->ReAlloc(vr->Length);
				if ( !vr->Data )
					return ( FALSE );	// memory error
				lVRBuffer.Read ( (BYTE *) vr->Data, vr->Length);

				MemoryBuffer 	MB( (BYTE*)vr->Data, vr->Length,
							TRUE, lVRBuffer.GetIncomingEndian() );
				vr->Length = 0;
				vr->Data = NULL;
				delete vr;

				DICOMDataObject *DDO = new DICOMDataObject;
				if ( ! Implicit_ParseRawVRIntoDCM ( MB, DDO, FALSE ) )
					{
					delete DDO;
					return ( FALSE );
					}
				DCMObject->Push(DDO);
				continue;
				}

			// Not quite sure what to say here...
			DicomError(DCM_ERROR_PARSE, "Sequence parse error during load of DCM file (after %08x)\n", (CurrentGroup<<16)+CurrentElement);
			return ( FALSE );
			}

		lVRBuffer >> vr->Length;

		// Type this element
		if ( AttachedRTC )
			AttachedRTC->RunTimeClass(vr);


		if ( vr->Length == 0xffffffff )
			{
			// variable length item [sequence, etc]
			vr->Length = 0;
			DCMObject->Push(vr);
			if ( (vr->Group == 0x7fe0) &&
				 (vr->Element == 0x0010))
				{
				// Image Pixel data.. encapsulated.. ugly..

				if(!ParseImagePixelDataRawToDCM(
					lVRBuffer,
					DCMObject))
					return(FALSE);
				continue;
				}
			if(!Implicit_ParseRawVRIntoDCM(lVRBuffer, DCMObject, FALSE))
				return(FALSE);
			continue;
			}

		if ( vr->TypeCode == 'SQ' )
			{
			// explicit length sequence
			// in this case.. pull the data into a memory buffer
			// and parse that.
			vr->ReAlloc(vr->Length);
			if ( !vr->Data )
				return ( FALSE );	// memory error

			lVRBuffer.Read ( (BYTE *) vr->Data, vr->Length);

			MemoryBuffer 	MB( (BYTE*)vr->Data, vr->Length,
								TRUE, lVRBuffer.GetIncomingEndian() );
			vr->Length = 0;
			vr->Data = NULL;
			DCMObject->Push(vr);

			if ( ! Implicit_ParseRawVRIntoDCM ( MB, DCMObject, FALSE ) )
				{
				return ( FALSE );
				}
			continue;			
			}

		// Explicit Length ( normal)
		if (vr->Length > lVRBuffer.GetIncomingSize())
			{
			if (DicomError(DCM_ERROR_PARSE, "Length exceeds remainder file size: %08x\n", vr->Length))
			  return FALSE;
			return ( TRUE );
			}

		if ( vr->Length )
			{
			if (vr->Length>CurrentGroupLength || (vr->Length&1))
				{
				BOOL b=DicomError(DCM_ERROR_PARSE, "Implicit_Parse encountered an invalid element length during load of DCM file (in %08x)\n", (CurrentGroup<<16)+CurrentElement);
				DicomError(DCM_ERROR_PARSE, "Length = %d\n", vr->Length);
				if (vr->Length > CurrentGroupLength) 
					DicomError(DCM_ERROR_PARSE, "Group Length = %u\n", CurrentGroupLength);
				if (b && vr->Length > 0xffff && CurrentGroupLength!=0) return FALSE; // pass if not dangerous
				DicomError(DCM_ERROR_PARSE, "Continuing parsing", 0);
				}

			vr->ReAlloc(vr->Length);

			if ( !vr->Data )
				return ( FALSE );	// memory error

			lVRBuffer.Read ( (BYTE *) vr->Data, vr->Length);

			if (vr->Element==0)
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
				CurrentGroupLength = *(unsigned int *)vr->Data;
#else //Big Endian like Apple power pc
				CurrentGroupLength = SwitchEndian(*(unsigned int *)vr->Data);
#endif
#ifdef DATE_FIX
// Time to look  a dates and fix it here if needed.  Some systems allow invalid birthdates
// like 00010101 (the DICOM box).  Other systems (OsiriX) will delete the leading 0's,
// but keep the vr length at 8.
// Ex: vr->TypeCode = DA, vr->Length = 8, vr->Data = 10101. == Image not stored!
// To fix this, find dates with the length of 8 and make sure the first digit is not 0.
// I'm not interested in fixing the date and prefer an obviously invalid date,
// but I don't want the year to excede the current year.  Hope for 10010101.
			if ( vr->Length == 8 )	//Dates (DA) are 8 long.
				{
				if ( vr->TypeCode == 'DA' ) // It's a date
					{
					if(*(char *)vr->Data == '0') *(char *)vr->Data = '1';
					}
				}
#endif
			}
		else
			vr->Data = NULL;

		DCMObject->Push(vr);
		}
				
	return ( TRUE );
	}

BOOL	PDU_Service	::	Implicit_ParseDCMIntoRawVR (
	DICOMObject	*DCMObject,
	LinkedBuffer	&lVRBuffer)
	{
	VR		*vr;
//	Array<DICOMObject *>	*ArrayPtr;
	UINT					Index;
	UINT					A1Flag;
	UINT32					tuint32;
	
	while ((vr = DCMObject->Pop()))
		{
		if ((!vr->Element) && (vr->Group != 0x00))
		{
			delete vr;
			continue;
		}
		lVRBuffer << vr->Group;
		lVRBuffer << vr->Element;
		if(vr->Length&0x01)
			{
			A1Flag = 1;
			++vr->Length;
			}
		else
			A1Flag = 0;
		if(!vr->SQObjectArray)
			{
			lVRBuffer << vr->Length;
			if(vr->Length)
				lVRBuffer.Write ( (BYTE *) vr->Data, vr->Length-A1Flag );
			if(A1Flag)
				{
				lVRBuffer.Write((BYTE*) "\0", 1);
				--vr->Length;
				A1Flag = 0;
				}
			}
		else
			{
			tuint32 = 0xffffffff;
			lVRBuffer << tuint32;
			if(A1Flag)
				{
				// (intentionally commented out)lVRBuffer.Write((BYTE*) "\0", 1);
				--vr->Length;
				A1Flag = 0;
				}
			}
		if(vr->SQObjectArray)
			{
			Array < DICOMDataObject	*>	*ArrayPtr =
				(Array<DICOMDataObject *> *) vr->SQObjectArray;
			Index = 0;
			if ( (vr->Group == 0x7fe0 )&&
				 (vr->Element == 0x0010 ))
				{
				ParseImagePixelDataDCMToRaw(ArrayPtr, lVRBuffer);
				delete vr;
				continue;
				}
			while ( Index < ArrayPtr->GetSize() )
				{
				vr->Group = 0xfffe;
				vr->Element = 0xe000;
				vr->Length = 0xffffffff;
				lVRBuffer << vr->Group;
				lVRBuffer << vr->Element;
				lVRBuffer << vr->Length;
				Implicit_ParseDCMIntoRawVR(
					ArrayPtr->Get(Index), lVRBuffer);
				vr->Group = 0xfffe;
				vr->Element = 0xe00d;
				vr->Length = 0x00000000;
				lVRBuffer << vr->Group;
				lVRBuffer << vr->Element;
				lVRBuffer << vr->Length;
				++Index;
				}
			vr->Group = 0xfffe;
			vr->Element = 0xe0dd;
			vr->Length = 0x00000000;
			lVRBuffer << vr->Group;
			lVRBuffer << vr->Element;
			lVRBuffer << vr->Length;
			}
			
		delete vr;
		}
	return ( TRUE );
	}


BOOL	PDU_Service	::	ExplicitLittleEndian_ParseRawVRIntoDCM(
	LinkedBuffer	&lVRBuffer,
	DICOMObject	*DCMObject)
	{
	lVRBuffer.SetIncomingEndian(LITTLE_ENDIAN);
	return ( Explicit_ParseRawVRIntoDCM(lVRBuffer, DCMObject) );
	}

BOOL	PDU_Service	::	ExplicitLittleEndian_ParseDCMIntoRawVR (
	DICOMObject	*DCMObject,
	LinkedBuffer	&lVRBuffer)
	{
	lVRBuffer.SetOutgoingEndian ( LITTLE_ENDIAN );
	return ( Explicit_ParseDCMIntoRawVR(DCMObject, lVRBuffer) );
	}

BOOL	PDU_Service	::	ExplicitBigEndian_ParseRawVRIntoDCM(
	LinkedBuffer	&lVRBuffer,
	DICOMObject	*DCMObject)
	{
	lVRBuffer.SetIncomingEndian(BIG_ENDIAN);
	return ( Explicit_ParseRawVRIntoDCM(lVRBuffer, DCMObject) );
	}

BOOL	PDU_Service	::	ExplicitBigEndian_ParseDCMIntoRawVR (
	DICOMObject	*DCMObject,
	LinkedBuffer	&lVRBuffer)
	{
	lVRBuffer.SetOutgoingEndian ( BIG_ENDIAN );
	return ( Explicit_ParseDCMIntoRawVR(DCMObject, lVRBuffer) );
	}

BOOL	PDU_Service	::	Explicit_ParseRawVRIntoDCM(LinkedBuffer	&lVRBuffer, DICOMObject	*DCMObject)
	{
	VR	*vr;
//	UINT32	tuint32, tuint32_2;
	UINT16	Length16;
//	DICOMObject	*EBO;		// Embedded sequence Object
	BYTE		s1[2];
	BOOL        swapEndian = false;
    
	if(BIG_ENDIAN == lVRBuffer.GetIncomingEndian())//bcb Big Endian fix
		{
		lVRBuffer.SetIncomingEndian(LITTLE_ENDIAN);//Everything starts as little.
		swapEndian = true;
		}

	unsigned int	CurrentGroup = 0;
	unsigned int	CurrentElement = 0;
	unsigned int	CurrentGroupLength = 0xffffffff;

	if ( ! DCMObject )
		return ( FALSE );	// passed a NULL object pointer

	while ( lVRBuffer.GetIncomingSize())
		{
		vr = new VR;
		if ( ! vr )
			return ( FALSE );	// memory error

		lVRBuffer >> vr->Group;
		if (swapEndian && vr->Group > 0x02)//after 0x02 change back
			{
			vr->Group = SwitchEndian(vr->Group);
			lVRBuffer.SetIncomingEndian(BIG_ENDIAN);
			swapEndian = false;
			}		
		lVRBuffer >> vr->Element;

                //DicomError(DCM_ERROR_PARSE, "process: %08x\n", 0x10000*vr->Group+vr->Element);

		if (vr->Group < CurrentGroup)
			{
			if (DicomError(DCM_ERROR_PARSE, "(Exp) Encountered an invalid group order during load of DCM file (after %08x)\n", (CurrentGroup<<16)+CurrentElement))
				return FALSE;
			}

		if (vr->Group != CurrentGroup)
			{
			CurrentGroup = vr->Group;
			CurrentElement = 0;
			CurrentGroupLength = 0xffffffff;
			}

		if (vr->Element < CurrentElement)
			{
			if (DicomError(DCM_ERROR_PARSE, "(Exp) Encountered an invalid element order during load of DCM file (in %08x)\n", (CurrentGroup<<16)+CurrentElement))
				return FALSE;
			}

		CurrentElement = vr->Element;

		if (vr->Group == 0x0000)
			{
			if (lVRBuffer.GetIncomingEndian()==BIG_ENDIAN)
			{ lVRBuffer.SetIncomingEndian(LITTLE_ENDIAN);
			  lVRBuffer >> vr->Length;
			  lVRBuffer.SetIncomingEndian(BIG_ENDIAN);
			  vr->Element = ((vr->Element&0xff) << 8) + ((vr->Element&0xff00) >> 8);
			}
			else
			  lVRBuffer >> vr->Length;

			// Explicit Length ( normal)

			if (vr->Length)
				{

				if (vr->Length > CurrentGroupLength || (vr->Length&1))
					{
					BOOL b=DicomError(DCM_ERROR_PARSE, "Explicit Parse encountered an invalid element length during load of DCM file (in %08x)\n", (CurrentGroup<<16)+CurrentElement);
					DicomError(DCM_ERROR_PARSE, "Length = %d\n", vr->Length);
					if (vr->Length > CurrentGroupLength) 
						DicomError(DCM_ERROR_PARSE, "Group Length = %u\n", CurrentGroupLength);
 					if (b && vr->Length > 0xffff && CurrentGroupLength!=0) return FALSE; // pass if not dangerous
					DicomError(DCM_ERROR_PARSE, "Continuing parsing", 0);
					}			

				vr->ReAlloc(vr->Length);
				if ( !vr->Data )
					return ( FALSE );	// memory error
				lVRBuffer.Read ( (BYTE *) vr->Data, vr->Length);
				if (lVRBuffer.GetIncomingEndian()==BIG_ENDIAN)
					SwapBigEndianData(vr);
				}
			else
				vr->Data = NULL;
			DCMObject->Push(vr);
			continue;
			}

		if ( vr->Group == 0xfffe )
			{
			// Ok, this is a deliminator type of item.
			// handled special.  Always implicit
			lVRBuffer >> vr->Length;

			if ( (vr->Element == 0xe0dd)||
				 (vr->Element == 0xe00d) )	// implicit terminator
				{
				delete vr;
				return ( TRUE );
				}

			if ( vr->Length == 0xffffffff)
				{
				// Implicit length.. go until deliminator
				vr->Length = 0;delete vr;
				
				DICOMDataObject *DDO = new DICOMDataObject;
				if ( ! Explicit_ParseRawVRIntoDCM ( lVRBuffer, DDO ) )
					{
					delete DDO;
					return ( FALSE );
					}
				DCMObject->Push(DDO);
				continue;
				}
			else if (vr->Length > CurrentGroupLength || (vr->Length&1))
				{
				BOOL b=DicomError(DCM_ERROR_PARSE, "Implicit_Parse encountered an invalid element length during load of DCM file (in %08x)\n", (CurrentGroup<<16)+CurrentElement);
				DicomError(DCM_ERROR_PARSE, "Length = %d\n", vr->Length);
				if (vr->Length > CurrentGroupLength) 
					DicomError(DCM_ERROR_PARSE, "Group Length = %u\n", CurrentGroupLength);
				if (b && vr->Length > 0xffff && CurrentGroupLength!=0) return FALSE; // pass if not dangerous
				DicomError(DCM_ERROR_PARSE, "Continuing parsing", 0);
				}			

			if (vr->Length>lVRBuffer.GetIncomingSize())
				{
				if (DicomError(DCM_ERROR_PARSE, "Length exceeding remainder file size: %08x\n", vr->Length))
				  return FALSE;
				return ( TRUE );
				}

			// Ok, an explicit length.. sequence item begin?
			if ( vr->Element == 0xe000 )
				{

				vr->ReAlloc(vr->Length);
				if ( !vr->Data )
					return ( FALSE );	// memory error
				lVRBuffer.Read ( (BYTE *) vr->Data, vr->Length);

				MemoryBuffer 	MB( (BYTE*)vr->Data, vr->Length,
								TRUE, lVRBuffer.GetIncomingEndian() );
				vr->Length = 0;
				vr->Data = NULL;
				delete vr;

				DICOMDataObject *DDO = new DICOMDataObject;
				if ( ! Explicit_ParseRawVRIntoDCM ( MB, DDO ) )
					{
					delete DDO;
					return ( FALSE );
					}
				DCMObject->Push(DDO);
				continue;
				}

			// Not quite sure what to say here...
			DicomError(DCM_ERROR_PARSE, "Sequence parse error during load of DCM file (after %08x)\n", (CurrentGroup<<16)+CurrentElement);
			return ( FALSE );
			}

		lVRBuffer >> s1[0];lVRBuffer>>s1[1];
		Pack(s1[0],s1[1],vr->TypeCode);

                // invalid typecode, may be unexpected implicit coding
		if (s1[0]<'A' || s1[0]>'Z' || s1[1]<'A' || s1[1]>'Z') 
                        {
                                UINT16 h; lVRBuffer >> h;
                                vr->Length = ((UINT32)h<<16) + ((UINT32)s1[1]<<8) + ((UINT32)s1[0]);
                                
                                vr->TypeCode = 0;
                                if (AttachedRTC) 
                                        AttachedRTC->RunTimeClass(vr);
                                if (vr->TypeCode == 0)
                                        vr->TypeCode = 'UN';
                                //if(	(vr->TypeCode=='OB')||
                                //        (vr->TypeCode=='OW')||
                                //        (vr->TypeCode=='UN')||
                                //        (vr->TypeCode=='UT')||
                                //        (vr->TypeCode=='OF')||
                                //        (vr->TypeCode=='OD')||
                                //        (vr->TypeCode=='OV')||
                                //        (vr->TypeCode=='UR')||
                                //        (vr->TypeCode=='UC')||
                                //        (vr->TypeCode=='SQ'))
                                //        {
                                //        lVRBuffer >> vr->Length;	// 32 bit Length
                                //        }
                                //else
                                //        {
                                //        vr->Length = (UINT32) Length16;
                                //        }
                        
                        }
                // END invalid typecode, may be unexpected implicit coding

		else if(	(vr->TypeCode=='OB')||
			(vr->TypeCode=='OW')||
			(vr->TypeCode=='UN')||
			(vr->TypeCode=='UT')||
			(vr->TypeCode=='OF')||
			(vr->TypeCode=='OD')||
			(vr->TypeCode=='OV')||
			(vr->TypeCode=='UR')||
			(vr->TypeCode=='UC')||
			(vr->TypeCode=='SQ'))
			{
			lVRBuffer >> Length16;	// Toss away 16 bits
			lVRBuffer >> vr->Length;	// 32 bit Length
			}
                else
			{
			lVRBuffer >> Length16;
			vr->Length = (UINT32) Length16;
			}


		if ( vr->Length == 0xffffffff )
			{
			// variable length item [sequence, etc]
			vr->Length = 0;
			DCMObject->Push(vr);
			if ( (vr->Group == 0x7fe0) &&
				 (vr->Element == 0x0010))
				{
				// Image Pixel data.. encapsulated.. ugly..

				if(!ParseImagePixelDataRawToDCM(
					lVRBuffer,
					DCMObject))
					return(FALSE);
				continue;
				}
			// Recurse
			if(!Explicit_ParseRawVRIntoDCM(lVRBuffer, DCMObject))
				return(FALSE);
			continue;
			}

		if ( vr->TypeCode == 'SQ' )
			{
			// explicit length sequence
			// in this case.. pull the data into a memory buffer
			// and parse that.
			vr->ReAlloc(vr->Length);
			if ( !vr->Data )
				return ( FALSE );	// memory error
			lVRBuffer.Read ( (BYTE *) vr->Data, vr->Length);

			MemoryBuffer 	MB( (BYTE*)vr->Data, vr->Length,
								TRUE, lVRBuffer.GetIncomingEndian() );
			vr->Length = 0;
			vr->Data = NULL;
			DCMObject->Push(vr);

			if ( ! Explicit_ParseRawVRIntoDCM ( MB, DCMObject ) )
				{
				return ( FALSE );
				}
			continue;			
			}

		if (vr->Length>lVRBuffer.GetIncomingSize())
			{
			//DicomError(DCM_ERROR_PARSE, "In: %08x\n", 0x10000*vr->Group+vr->Element);
			if (DicomError(DCM_ERROR_PARSE, "Item Length exceeds remaining file size: %08x\n", vr->Length))
			  return FALSE;
			return ( TRUE );
			}

		// Explicit Length ( normal)
		if (vr->Length)
			{
			if (vr->Length > CurrentGroupLength || (vr->Length&1))
				{
				BOOL b=DicomError(DCM_ERROR_PARSE, "Explicit_ParseRaw encountered an invalid element length during load of DCM file (in %08x)\n", (CurrentGroup<<16)+CurrentElement);
				DicomError(DCM_ERROR_PARSE, "Length = %d\n", vr->Length);
				if (vr->Length > CurrentGroupLength) 
					DicomError(DCM_ERROR_PARSE, "Group Length = %u\n", CurrentGroupLength);
				if (b && vr->Length > 0xffff && CurrentGroupLength!=0) return FALSE; // pass if not dangerous
				DicomError(DCM_ERROR_PARSE, "Continuing parsing", 0);
				}

			vr->ReAlloc(vr->Length);
			if ( !vr->Data )
				return ( FALSE );	// memory error

			lVRBuffer.Read ( (BYTE *) vr->Data, vr->Length);

#ifdef DATE_FIX
// Time to look  a dates and fix it here if needed.  Some systems allow invalid birthdates
// like 00010101 (the DICOM box).  Other systems (OsiriX) will delete the leading 0's,
// but keep the vr length at 8.
// Ex: vr->TypeCode = DA, vr->Length = 8, vr->Data = 10101. == Image not stored!
// To fix this, find dates with the length of 8 and make sure the first digit is not 0.
// I'm not interested in fixing the date and prefer an obviously invalid date,
// but I don't want the year to excede the current year.  Hope for 10010101.
                            if ( vr->Length == 8 )	//Dates (DA) are 8 long.
                            {
                                if ( vr->TypeCode == 'DA' ) // It's a date
                                {
                                    if(*(char *)vr->Data == '0') *(char *)vr->Data = '1';
                                }
                            }
#endif

			if (vr->Element==0)
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
				CurrentGroupLength = *(unsigned int *)vr->Data;
#else //Big Endian like Apple power pc
				CurrentGroupLength = SwitchEndian( *(unsigned int *)vr->Data);
#endif

			if (lVRBuffer.GetIncomingEndian()==BIG_ENDIAN)
				SwapBigEndianData(vr);
			}
		else
			vr->Data = NULL;

		DCMObject->Push(vr);
		}

	if (lVRBuffer.GetIncomingEndian()==BIG_ENDIAN && DCMObject->GetVR(0x0002, 0x0010))
		DCMObject->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.1\0", 'IU');

	return ( TRUE );
	}


BOOL	PDU_Service	::	Explicit_ParseDCMIntoRawVR (
	DICOMObject	*DCMObject,
	LinkedBuffer	&lVRBuffer)
	{
	VR		*vr;
//	Array<DICOMObject *>	*ArrayPtr;
	UINT					Index;
	UINT					A1Flag;
	UINT16					Length16;
 	BYTE					s1[2];
	UINT32					tuint32;

	while ((vr = DCMObject->Pop()))
		{
		if((!vr->Element) && (vr->Group != 0x0000))
			{
					// LJ maybe send group-length of group 0x0000
			delete vr;	// 4/16/96 - no longer sender group length under
					// explicit.  It wasn't correct anyway.
			continue;
			}
		lVRBuffer << vr->Group;
		lVRBuffer << vr->Element;

	
		if (vr->TypeCode == 0 && AttachedRTC) 
			AttachedRTC->RunTimeClass(vr);

		if (vr->TypeCode == 0)
			vr->TypeCode = 'UN';
			
		if ((vr->Length >= 0x10000) && 
		      !((vr->TypeCode=='OB')||
			(vr->TypeCode=='OW')||
			(vr->TypeCode=='UN')||
			(vr->TypeCode=='UT')||
			(vr->TypeCode=='OF')||
			(vr->TypeCode=='OD')||
			(vr->TypeCode=='OV')||
			(vr->TypeCode=='UR')||
			(vr->TypeCode=='UC')||
			(vr->TypeCode=='SQ')))
			vr->TypeCode = 'UN';

		if (vr->SQObjectArray && vr->TypeCode == 'UN')
			vr->TypeCode = 'SQ';

		Pack(vr->TypeCode, s1[0], s1[1]);
		lVRBuffer << s1[0];lVRBuffer<<s1[1];
	
		if(vr->Length&0x01)
			{
			A1Flag = 1;
			++vr->Length;
			}
		else
			A1Flag = 0;
		if(	(vr->TypeCode=='OB')||
			(vr->TypeCode=='OW')||
			(vr->TypeCode=='UN')||
			(vr->TypeCode=='UT')||
			(vr->TypeCode=='OF')||
			(vr->TypeCode=='OD')||
			(vr->TypeCode=='OV')||
			(vr->TypeCode=='UR')||
			(vr->TypeCode=='UC')||
			(vr->TypeCode=='SQ'))
			{
			Length16 = 0;
			lVRBuffer << Length16;	// Toss in 16 bits
			if(vr->SQObjectArray)
				{
				if(vr->SQObjectArray)
					tuint32 = 0xffffffff;
				else
					tuint32 = 0;
				lVRBuffer << tuint32;
				}
			else
				lVRBuffer << vr->Length;	// 32 bit Length
			}
		else
			{
			Length16 = (UINT16) vr->Length;
			lVRBuffer << Length16;
			}
		if(vr->TypeCode!='SQ')
			{		 
			if (lVRBuffer.GetOutgoingEndian()==BIG_ENDIAN)
				SwapBigEndianData(vr);
			if(vr->Length)
				lVRBuffer.Write ( (BYTE *) vr->Data, vr->Length-A1Flag );
			if (lVRBuffer.GetOutgoingEndian()==BIG_ENDIAN)
				SwapBigEndianData(vr);
			}
		if(A1Flag)
			{
			if(vr->TypeCode!='SQ')
				lVRBuffer.Write((BYTE*) "\0", 1);
			--vr->Length;
			A1Flag = 0;
			}

		if(vr->SQObjectArray)
			{
			Array<DICOMDataObject*>	*ArrayPtr =
				(Array<DICOMDataObject *> *) vr->SQObjectArray;
			Index = 0;
			if ( (vr->Group == 0x7fe0 )&&
				 (vr->Element == 0x0010 ))
				{
				ParseImagePixelDataDCMToRaw(ArrayPtr, lVRBuffer);
				delete vr;
				continue;
				}
			while ( Index < ArrayPtr->GetSize() )
				{
				vr->Group = 0xfffe;
				vr->Element = 0xe000;
				vr->Length = 0xffffffff;
				lVRBuffer << vr->Group;
				lVRBuffer << vr->Element;
				lVRBuffer << vr->Length;

				Explicit_ParseDCMIntoRawVR(
					ArrayPtr->Get(Index), lVRBuffer);
				vr->Group = 0xfffe;	// (0xfffe,0xe00d) is a Item Delimiter
				vr->Element = 0xe00d;
				vr->Length = 0;		
				lVRBuffer << vr->Group;
				lVRBuffer << vr->Element;
				lVRBuffer << vr->Length;
				++Index;
				}
			vr->Group = 0xfffe;	// (0xfffe,0xe0dd) is a Sequence Delimiter
			vr->Element = 0xe0dd;
			vr->Length = 0;		
			lVRBuffer << vr->Group;
			lVRBuffer << vr->Element;
			lVRBuffer << vr->Length;

			}
			
		delete vr;
		}
	return ( TRUE );
	}


UINT
static
EndianOf(
	UINT	Alias)
	{
	if((Alias==TSA_EXPLICIT_LITTLE_ENDIAN)||
		(Alias==TSA_IMPLICIT_LITTLE_ENDIAN))
		return ( LITTLE_ENDIAN );
	return ( BIG_ENDIAN );
	}

void SwapBigEndianData(VR* pVR)
{ char		cSave;
  int		i, iNbValues;
  char*		pcData;

  switch (pVR->TypeCode)
  { case 'FL':	// float
    case 'OF':	// float array
    case 'SL':	// signed long
    case 'UL':	// unsigned long
    pcData = (char*)pVR->Data;
    iNbValues = pVR->Length / 4;
    for (i=0; i<iNbValues; i++)
    { cSave = pcData[0];
      pcData[0] = pcData[3];
      pcData[3] = cSave;
      cSave = pcData[1];
      pcData[1] = pcData[2];
      pcData[2] = cSave;
      pcData += 4;
    }
    break;

    case 'OW':	// int16 array
    case 'SS':	// signed short
    case 'US':	// unsigned short
    pcData = (char*)pVR->Data;
    iNbValues = pVR->Length / 2;
    for (i=0; i<iNbValues; i++)
    { cSave = pcData[0];
      pcData[0] = pcData[1];
      pcData[1] = cSave;
      pcData += 2;
    }
    break;

    case 'FD':	// double
    case 'OD':	// double array
    case 'OV':	// very long array
    pcData = (char*)pVR->Data;
    iNbValues = pVR->Length / 8;
    for (i=0; i<iNbValues; i++)
    { cSave = pcData[0];
      pcData[0] = pcData[7];
      pcData[7] = cSave;
      cSave = pcData[1];
      pcData[1] = pcData[6];
      pcData[6] = cSave;
      cSave = pcData[1];
      pcData[2] = pcData[5];
      pcData[5] = cSave;
      cSave = pcData[1];
      pcData[3] = pcData[4];
      pcData[4] = cSave;
      pcData += 8;
    }
    break;
  }
}


BOOL	PDU_Service	::	Dynamic_ParseRawVRIntoDCM(
	LinkedBuffer	&lVRBuffer,
	DICOMObject	*DCMObject,
	UINT		Mode
	)
	{
	VR		*vr;
	VR		*vrTRN;	//0x0002,0x0010
//	UINT32		tuint32, tuint32_2;
	UINT16		Length16;
//	DICOMObject	*EBO;		// Embedded sequence Object
	UINT		NewMode;
	BYTE		s1[2];
	char		s[128];
	int		iOrgMode;	// LJ + MVH: Added to save the original mode
	unsigned int	CurrentGroup = 0;
	unsigned int	CurrentElement = 0;
	unsigned int	CurrentGroupLength = 0xffffffff;
		
// Init for warnings.
	s1[0] = s1[1] = 0;
	
	iOrgMode = Mode;		// LJ + MVH

	if ( ! DCMObject )
		return ( FALSE );	// passed a NULL object pointer

	if((Mode==TSA_EXPLICIT_LITTLE_ENDIAN)||
		(Mode==TSA_IMPLICIT_LITTLE_ENDIAN))
		lVRBuffer.SetIncomingEndian(LITTLE_ENDIAN);
	else
		lVRBuffer.SetIncomingEndian(BIG_ENDIAN);
		
	vrTRN = NULL;

	while ( lVRBuffer.GetIncomingSize())
		{
		vr = new VR;
		if ( ! vr )
			{
			return FALSE;
			}
		
		lVRBuffer >> vr->Group;

		if (vr->Group < CurrentGroup)
			{
			if (DicomError(DCM_ERROR_PARSE, "(Dyn) Encountered an invalid group order during load of DCM file (after %08x)\n", (CurrentGroup<<16)+CurrentElement))
				return FALSE;
			}

		if(vr->Group > 0x0002)
			{
			// Key on the vrTRN
			UINT		Index,sLen;
			if(vrTRN)
				{
				memset((void*)s, 0,128);
				memcpy((void*)s,vrTRN->Data, vrTRN->Length%64);
				sLen = strlen(s);
				if(sLen)
					{
					if(s[sLen-1]==' ')
						s[sLen-1]='\0';
					}
				Index = 0;
				while(TransferSyntaxAliases[Index].TransferSyntaxUID)
					{
					if(!strcmp(TransferSyntaxAliases[Index].TransferSyntaxUID,s))
						{
// This bcb change breaks reads on many of my images mvh!
//                        			iOrgMode = 0; //No need to check again, we were told what it is. bcb 20100419
   
						NewMode = TransferSyntaxAliases[Index].Alias;
						if(NewMode==Mode)
							break;
						if(EndianOf(NewMode)!=EndianOf(Mode))
							{
							// Darn
							vr->Group = SwitchEndian(vr->Group);
							lVRBuffer.SetIncomingEndian(EndianOf(NewMode));
							}
						Mode = NewMode;		// LJ + MVH: Maybe disollow this later...
						break;
						}
					++Index;
					}
				if(!TransferSyntaxAliases[Index].TransferSyntaxUID)
					{
					DicomError(DCM_ERROR_PARSE, "Unknown transfer syntax in %08x\n", (CurrentGroup<<16)+CurrentElement);
					return FALSE;
					}
				vrTRN = NULL;
				}
			}

		if (vr->Group != CurrentGroup)//Bcb must be done after "Big Endian" check
			{
			CurrentGroup = vr->Group;
			CurrentElement = 0;
			CurrentGroupLength = 0xffffffff;
			}

		lVRBuffer >> vr->Element;

		if (vr->Element < CurrentElement)
			{

#ifdef NOVARAD_FIX			
			if(CurrentGroup == 0x0002 && CurrentElement == 0x0016 && vr->Element == 0x0013)
				{// Looks like NovaRad
				lVRBuffer >> s1[0];
				lVRBuffer >> s1[1];
				if(s1[0] == 'S' && s1[1] == 'H')
					{// Looking more like NovaRad
					lVRBuffer >> Length16;
					if(Length16 > 7 && Length16 < 0x0010)
						{//Getting closer
						vr->ReAlloc(Length16);
						if ( !vr->Data )
							return ( FALSE );	// memory error
						lVRBuffer.Read ( (BYTE *) vr->Data, Length16);
						if(memcmp(vr->Data, "NovaRad", 7) == 0)
							{//Sneaky NovaRad, bad bad NovaRad!
							delete vr;// Dump the bad vr.
							continue;// Lets keep going.
							}
						}
					}
				}
#endif

			if (DicomError(DCM_ERROR_PARSE, "(Dyn) Encountered an invalid element order during load of DCM file (in %08x)\n", (CurrentGroup<<16)+CurrentElement))
				return FALSE;
			}

		CurrentElement = vr->Element;

		// Regardless of mode.. 0xfffe are encoded implicit
		if ( vr->Group == 0xfffe )
			{
			// Ok, this is a deliminator type of item.
			// handled special.  Always implicit

			lVRBuffer >> vr->Length;

			if ( (vr->Element == 0xe0dd)||
				 (vr->Element == 0xe00d) )	// implicit terminmator
				{
				delete vr;
				return ( TRUE );
				}

			if ( vr->Length == 0xffffffff)
				{
				// Implicit length.. go until deliminator
				vr->Length = 0;delete vr;
				
				DICOMDataObject *DDO = new DICOMDataObject;
				if((Mode==TSA_EXPLICIT_BIG_ENDIAN)||
					(Mode==TSA_EXPLICIT_LITTLE_ENDIAN))
					{
					if ( ! Explicit_ParseRawVRIntoDCM ( lVRBuffer, DDO ) )
						{
						delete DDO;
						return ( FALSE );
						}
					}
				else
					{
					if ( ! Implicit_ParseRawVRIntoDCM ( lVRBuffer, DDO, FALSE ) )
						{
						delete DDO;
						return ( FALSE );
						}
					}	
				DCMObject->Push(DDO);
				continue;
				}
			else if (vr->Length > CurrentGroupLength || (vr->Length&1))
				{
				BOOL b=DicomError(DCM_ERROR_PARSE, "Dynamic Parse encountered an invalid element length during load of DCM file (in %08x)\n", (CurrentGroup<<16)+CurrentElement);
				DicomError(DCM_ERROR_PARSE, "Length = %d\n", vr->Length);
				if (vr->Length > CurrentGroupLength) 
					DicomError(DCM_ERROR_PARSE, "Group Length = %u\n", CurrentGroupLength);
				if (b && vr->Length > 0xffff && CurrentGroupLength!=0) return FALSE; // pass if not dangerous
				DicomError(DCM_ERROR_PARSE, "Continuing parsing", 0);
				}			

			if (vr->Length>lVRBuffer.GetIncomingSize())
				{
				if (DicomError(DCM_ERROR_PARSE, "Item Length exceeding remaining file size: %08x\n", vr->Length))
				  return FALSE;
				return ( TRUE );
				}

			// Ok, an explicit length.. sequence item begin?
			if ( vr->Element == 0xe000 )
				{
				vr->ReAlloc(vr->Length);
				if ( !vr->Data )
					{
					delete vr;
					return ( FALSE );	// memory error
					}
				lVRBuffer.Read ( (BYTE *) vr->Data, vr->Length);

				MemoryBuffer 	MB( (BYTE*)vr->Data, vr->Length,
								TRUE, lVRBuffer.GetIncomingEndian() );
				vr->Length = 0;
				//delete vr->Data;
				vr->Data = NULL;
				delete vr;
				DICOMDataObject *DDO = new DICOMDataObject;
				if((Mode==TSA_EXPLICIT_BIG_ENDIAN)||
					(Mode==TSA_EXPLICIT_LITTLE_ENDIAN))
					{
					if ( ! Explicit_ParseRawVRIntoDCM ( MB, DDO ) )
						{
						delete DDO;
						return ( FALSE );
						}
					}
				else
					{
					if ( ! Implicit_ParseRawVRIntoDCM ( MB, DDO, FALSE ) )
						{
						delete DDO;
						return ( FALSE );
						}
					}
				DCMObject->Push(DDO);
				continue;
				}

			// Not quite sure what to say here...
			delete vr;
			DicomError(DCM_ERROR_PARSE, "Sequence parse error during load of DCM file (after %08x)\n", (CurrentGroup<<16)+CurrentElement);
			return ( FALSE );
			}


		if (iOrgMode && (vr->Group > 0x0002))
		{
		/* LJ + MVH: iOrgMode will be set to one when leaving this 'if', and set
		   to zero in the following if.
		   So, this if is entered only when processing this first GroupElement NOT
		   belonging to the metaheader...
		   This code check whether TypeCodes are present. In the Davis software,
		   the presence of TypeCodes is called 'explicit' !
		   The same check is done in 'filepdu.cxx': PDU_Service::LoadDICOMDataObject()
		   In the Davis software, IMPLICIT_BIG_ENDIAN is not known.
		 */
			lVRBuffer >> s1[0];
			lVRBuffer >> s1[1];
			if (s1[1] > 10)		// Explicit
			{
				if (Mode == TSA_IMPLICIT_LITTLE_ENDIAN)
					Mode = TSA_EXPLICIT_LITTLE_ENDIAN;
			}
			else
			{
				if (Mode == TSA_EXPLICIT_LITTLE_ENDIAN)
					Mode = TSA_IMPLICIT_LITTLE_ENDIAN;
			}
			iOrgMode = 1;
		}
		
		if((Mode==TSA_EXPLICIT_LITTLE_ENDIAN)||
			(Mode==TSA_EXPLICIT_BIG_ENDIAN))
		{
			if (iOrgMode != 1)
			{
				lVRBuffer >> s1[0];
				lVRBuffer >> s1[1];
			}
			else
				iOrgMode = 0;		// LJ + MVH: allready read two bytes
			Pack(s1[0], s1[1], vr->TypeCode);


			if(	(vr->TypeCode=='OB')||
				(vr->TypeCode=='OW')||
				(vr->TypeCode=='UN')||
				(vr->TypeCode=='UT')||
				(vr->TypeCode=='OF')||
				(vr->TypeCode=='OD')||
				(vr->TypeCode=='OV')||
				(vr->TypeCode=='UR')||
				(vr->TypeCode=='UC')||
				(vr->TypeCode=='SQ'))
				{
				lVRBuffer >> Length16;	// Toss away 16 bits
				lVRBuffer >> vr->Length;	// 32 bit Length
				}
			else
				{
				lVRBuffer >> Length16;
				vr->Length = (UINT32) Length16;
				}
		}
		else // TSA_IMPLICIT_LITTLE_ENDIAN
			{
			if (iOrgMode != 1)
				lVRBuffer >> vr->Length;
			else
	                	{	// LJ + MVH: already read two bytes
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
	                	*((char*)(&vr->Length) + 0) = s1[0];
        	        	*((char*)(&vr->Length) + 1) = s1[1];
	                	lVRBuffer >> ((char*)(&vr->Length))[2];
                		lVRBuffer >> ((char*)(&vr->Length))[3];
#else //Big Endian like Apple power pc
                		*((char*)(&vr->Length) + 3) = s1[0];
                		*((char*)(&vr->Length) + 2) = s1[1];
                		lVRBuffer >> ((char*)(&vr->Length))[1];
                		lVRBuffer >> ((char*)(&vr->Length))[0];
#endif
				iOrgMode = 0;
                		}
			if ( AttachedRTC )
				AttachedRTC->RunTimeClass ( vr );
			}
			
		if(vr->Group == 0x0002)
			{
			if(vr->Element==0x0010)
				vrTRN = vr;
			}


		// Ok, at this point it's not a 0xfffe element, and it's
		// and the group, element, length (and possibly typecode)
		// has been decoded.
		// must now 


		if ( vr->Length == 0xffffffff )
			{
			// variable length item [sequence, etc]
			vr->Length = 0;
			DCMObject->Push(vr);
			if ( (vr->Group == 0x7fe0) &&
				 (vr->Element == 0x0010))
				{
				// Image Pixel data.. encapsulated.. ugly..

				if(!ParseImagePixelDataRawToDCM(
					lVRBuffer,
					DCMObject))
					return(FALSE);
				continue;
				}
			// Recurse
			if((Mode==TSA_EXPLICIT_BIG_ENDIAN)||
				(Mode==TSA_EXPLICIT_LITTLE_ENDIAN))
				{
				if ( ! Explicit_ParseRawVRIntoDCM ( lVRBuffer, DCMObject ) )
					return(FALSE);
				}
			else
				{
				if ( ! Implicit_ParseRawVRIntoDCM ( lVRBuffer, DCMObject, FALSE ) )
					{
					return(FALSE);
					}
				}

			continue;
			}

		else if (vr->Length > CurrentGroupLength || (vr->Length&1))
			{
			BOOL b=DicomError(DCM_ERROR_PARSE, "Dynamic_Parse encountered an invalid element length during load of DCM file (in %08x)\n", (CurrentGroup<<16)+CurrentElement);
			DicomError(DCM_ERROR_PARSE, "Length = %d\n", vr->Length);
			if (vr->Length > CurrentGroupLength) 
				DicomError(DCM_ERROR_PARSE, "Group Length = %u\n", CurrentGroupLength);
			if (b && vr->Length > 0xffff && CurrentGroupLength!=0) return FALSE; // pass if not dangerous
			DicomError(DCM_ERROR_PARSE, "Continuing parsing", 0);
			}

		if (vr->Length>lVRBuffer.GetIncomingSize())
			{
			if (DicomError(DCM_ERROR_PARSE, "Item Length exceeds remainder file size: %08x\n", vr->Length))
			  return FALSE;
			return ( TRUE );
			}

		if ( vr->TypeCode == 'SQ' )
			{
			// explicit length sequence
			// in this case.. pull the data into a memory buffer
			// and parse that.
			vr->ReAlloc(vr->Length);
			if ( !vr->Data )
				return ( FALSE );	// memory error
			lVRBuffer.Read ( (BYTE *) vr->Data, vr->Length);

			MemoryBuffer 	MB( (BYTE*)vr->Data, vr->Length,
								TRUE, lVRBuffer.GetIncomingEndian() );
			vr->Length = 0;
			vr->Data = NULL;
			DCMObject->Push(vr);

			if((Mode==TSA_EXPLICIT_BIG_ENDIAN)||
				(Mode==TSA_EXPLICIT_LITTLE_ENDIAN))
				{
				if ( ! Explicit_ParseRawVRIntoDCM ( MB, DCMObject ) )
					{
					return ( FALSE );
					}
				}
			else
				{
				if ( ! Implicit_ParseRawVRIntoDCM ( MB, DCMObject, FALSE ) )
					{
					return ( FALSE );
					}
				}
			continue;			
			}

		// Explicit Length ( normal)
		if ( vr->Length )
			{
			vr->ReAlloc(vr->Length);
			if ( !vr->Data )
				return ( FALSE );	// memory error
			lVRBuffer.Read ( (BYTE *) vr->Data, vr->Length);
			if (Mode==TSA_EXPLICIT_BIG_ENDIAN)
				SwapBigEndianData(vr);

			if (vr->Element==0)
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
				CurrentGroupLength = *(unsigned int *)vr->Data;
#else //Big Endian like Apple power pc
				CurrentGroupLength = SwitchEndian(*(unsigned int *)vr->Data);
#endif
#ifdef DATE_FIX
// Time to look  a dates and fix it here if needed.  Some systems allow invalid birthdates
// like 00010101 (the DICOM box).  Other systems (OsiriX) will delete the leading 0's,
// but keep the vr length at 8.
// Ex: vr->TypeCode = DA, vr->Length = 8, vr->Data = 10101. == Image not stored!
// To fix this, find dates with the length of 8 and make sure the first digit is not 0.
// I'm not interested in fixing the date and prefer an obviously invalid date,
// but I don't want the year to excede the current year.  Hope for 10010101.
			if ( vr->Length == 8 )	//Dates (DA) are 8 long.
				{
				if ( vr->TypeCode == 'DA' ) // It's a date
					{
					if(*(char *)vr->Data == '0') *(char *)vr->Data = '1';
					}
				}
#endif
			}
		else
			vr->Data = NULL;

		DCMObject->Push(vr);
		}

	if (Mode==TSA_EXPLICIT_BIG_ENDIAN && DCMObject->GetVR(0x0002, 0x0010))
		DCMObject->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.1\0", 'IU');

	return ( TRUE );
	}

BOOL	PDU_Service	::	Dynamic_ParseDCMIntoRawVR(
	DICOMObject	*DCMObject,
	LinkedBuffer &lVRBuffer,
	UINT	Mode)
	{

	VR		*vr,*vrTRN;
//	Array<DICOMObject *>	*ArrayPtr;
	UINT					Index;
	UINT					A1Flag;
	UINT16					Length16;
	UINT					NewMode;
 	BYTE					s1[2];
	char					s[128];
	UINT32					tuint32;


	if((Mode==TSA_EXPLICIT_LITTLE_ENDIAN)||
		(Mode==TSA_IMPLICIT_LITTLE_ENDIAN))
		lVRBuffer.SetIncomingEndian(LITTLE_ENDIAN);
	else
		lVRBuffer.SetIncomingEndian(BIG_ENDIAN);
		
#if NATIVE_ENDIAN == BIG_ENDIAN //Big Endian like Apple power pc
	lVRBuffer.SetOutgoingEndian(LITTLE_ENDIAN);
#endif

	vrTRN = NULL;
	while ((vr = DCMObject->Pop()))
		{
		if(vrTRN && vr->Group>0x0002)
			{
			// Key on the vrTRN
			UINT		Index2,sLen;
			memset((void*)s, 0,128);
			memcpy((void*)s,vrTRN->Data, vrTRN->Length%64);
			sLen = strlen(s);
			if(sLen)
				{
				if(s[sLen-1]==' ')
					s[sLen-1]='\0';
				}
			Index2 = 0;
			while(TransferSyntaxAliases[Index2].TransferSyntaxUID)
				{
				if(!strcmp(TransferSyntaxAliases[Index2].TransferSyntaxUID,s))
					{
					NewMode = TransferSyntaxAliases[Index2].Alias;
					if(NewMode==Mode)
						break;
					lVRBuffer.SetIncomingEndian(EndianOf(NewMode));
					Mode = NewMode;
					break;
					}
				++Index2;
				}
			if(!TransferSyntaxAliases[Index2].TransferSyntaxUID)
				return ( FALSE );	// woa, cannot encode remainder of data set
			delete vrTRN;
			vrTRN = NULL;
			}

		if((Mode==TSA_EXPLICIT_BIG_ENDIAN)||
			(Mode==TSA_EXPLICIT_LITTLE_ENDIAN))
			if(!vr->Element)
				{
				if  (vr->Group == 0x0002)
					{
						/* LJ: Keep grouplength of Chapter10, but fix the value so that
						   the 'DICM' tag is included
						*/
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
						*((int*)vr->Data) = *((int*)vr->Data) + 4;//strlen("DICM");
#else //Big Endian like Apple power pc
						*((UINT32*)vr->Data) = SwitchEndian(
		                                SwitchEndian(*((UINT32*)vr->Data)) + 4);
#endif
					}
				else
					{
						delete vr;	// 4-16-96 No longer send group length in
							// explicit mode.  --wasn't correct anyway.
						continue;
					}
				}

		lVRBuffer << vr->Group;
		lVRBuffer << vr->Element;


		if(vr->Length&0x01)
			{
			A1Flag = 1;
			++vr->Length;
			}
		else
			A1Flag = 0;


		if((Mode==TSA_EXPLICIT_BIG_ENDIAN)||
			(Mode==TSA_EXPLICIT_LITTLE_ENDIAN))
			{
			if (vr->TypeCode == 0 && AttachedRTC) 
				AttachedRTC->RunTimeClass(vr);

			if (vr->TypeCode == 0)
				vr->TypeCode = 'UN';
			
			if ((vr->Length >= 0x10000) && 
			      !((vr->TypeCode=='OB')||
				(vr->TypeCode=='OW')||
				(vr->TypeCode=='UN')||
				(vr->TypeCode=='UT')||
				(vr->TypeCode=='OF')||
				(vr->TypeCode=='OD')||
				(vr->TypeCode=='OV')||
				(vr->TypeCode=='UR')||
				(vr->TypeCode=='UC')||
				(vr->TypeCode=='SQ')))
				vr->TypeCode = 'UN';

			if (vr->SQObjectArray && vr->TypeCode == 'UN')
				vr->TypeCode = 'SQ';

			Pack(vr->TypeCode, s1[0], s1[1]);
			lVRBuffer << s1[0];lVRBuffer<<s1[1];

			if(	(vr->TypeCode=='OB')||
				(vr->TypeCode=='OW')||
				(vr->TypeCode=='UN')||
				(vr->TypeCode=='UT')||
				(vr->TypeCode=='OF')||
				(vr->TypeCode=='OD')||
				(vr->TypeCode=='OV')||
				(vr->TypeCode=='UR')||
				(vr->TypeCode=='UC')||
				(vr->TypeCode=='SQ'))
				{
				Length16 = 0;
				lVRBuffer << Length16;	// Toss in 16 bits
				if(vr->SQObjectArray)
					{
					tuint32 = 0xffffffff;
					lVRBuffer << tuint32;
					}
				else
					lVRBuffer << vr->Length;	// 32 bit Length
				}
			else
				{
				Length16 = (UINT16) vr->Length;
				lVRBuffer << Length16;
				}
			}
		else
			{
			if(vr->SQObjectArray)
				{
				tuint32 = 0xffffffff;
				lVRBuffer << tuint32;
				}
			else
				lVRBuffer << vr->Length;
			}
	

		if (lVRBuffer.GetOutgoingEndian()==BIG_ENDIAN)
			SwapBigEndianData(vr);

		if(vr->Length)
			lVRBuffer.Write ( (BYTE *) vr->Data, vr->Length-A1Flag );

		if (lVRBuffer.GetOutgoingEndian()==BIG_ENDIAN)
			SwapBigEndianData(vr);
		
		if(A1Flag)
			{
			lVRBuffer.Write((BYTE*) "\0", 1);
			--vr->Length;
			A1Flag = 0;
			}

		if(vr->SQObjectArray)
			{
			Array<DICOMDataObject*>	*ArrayPtr =
				(Array<DICOMDataObject *> *) vr->SQObjectArray;

			if ( (vr->Group == 0x7fe0 )&&
				 (vr->Element == 0x0010 ))
				{
				ParseImagePixelDataDCMToRaw(ArrayPtr, lVRBuffer);
				delete vr;
				continue;
				}

			Index = 0;
			while ( Index < ArrayPtr->GetSize() )
				{
				vr->Group = 0xfffe;
				vr->Element = 0xe000;
				vr->Length = 0xffffffff;
				lVRBuffer << vr->Group;
				lVRBuffer << vr->Element;
				lVRBuffer << vr->Length;

				if((Mode==TSA_EXPLICIT_LITTLE_ENDIAN)||
					(Mode==TSA_EXPLICIT_BIG_ENDIAN))
					{
					Explicit_ParseDCMIntoRawVR(
						ArrayPtr->Get(Index), lVRBuffer);
					}
				else
					{
					Implicit_ParseDCMIntoRawVR(
						ArrayPtr->Get(Index), lVRBuffer);
					}
				vr->Group = 0xfffe;
				vr->Element = 0xe00d;
				vr->Length = 0;
				lVRBuffer << vr->Group;
				lVRBuffer << vr->Element;
				lVRBuffer << vr->Length;
				++Index;
				}
			vr->Group = 0xfffe;
			vr->Element = 0xe0dd;
			vr->Length = 0;
			lVRBuffer << vr->Group;
			lVRBuffer << vr->Element;
			lVRBuffer << vr->Length;
			}

		if((vr->Group == 0x0002)&&
			(vr->Element==0x0010))
				vrTRN = vr;
		else			
			delete vr;
		}
	if(vrTRN)
		delete	vrTRN;

	return ( TRUE );
	}


	
char* PDU_Service :: GetTransferSyntaxUID (UINT TransferSyntaxMode)
{
	
	UINT			Index;
	char			*TransferSyntax;

	TransferSyntax = NULL;// Init for warning.
	Index = 0;
	while(TransferSyntaxAliases[Index].TransferSyntaxUID)
	{
		if(TransferSyntaxMode == TransferSyntaxAliases[Index].Alias)
		{
			TransferSyntax = (char *)malloc(64+1);
                        if (TransferSyntax)
                        { memset(TransferSyntax, 0, (64+1));
			  strcpy(TransferSyntax, TransferSyntaxAliases[Index].TransferSyntaxUID);
                        }
			break;
		}
		++Index;
	}
	return (TransferSyntax);
}

