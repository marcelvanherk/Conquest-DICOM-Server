/*
19980322    mvh    blocked one remaining printf statement "comparing ..."
19990202    ljz    Added PDU memberfunction 'IsAbstractSyntaxAccepted'
19990415    mvh    Merged both versions
20000322    ljz    Fix in search for PresentationContext when identical
		   AbstractSyntaxes occur (e.g. GECT-scanner).
		   Do not break out of the inner loop in
		   PDU_Service::InterogateAAssociateAC.
20000322  ljz+mvh  Fix of PresentationContextID in PDV: in routines
		   PDU_Service::Write(DICOMObject*, UID*), set
		   PresentationContextID to TempByte, not to TempByte1.
20000911    ljz    Removed two warnings
20020316    mvh    Added GetLocalAddress and GetRemoteAddress
20030306    ljz    Fix: Huge bug in 'InterogateAAssociateRQ'
20030424    ljz    Blocked out obsolete PDU.Write(&DDO)
		   Some work on offering other TransferSyntaxes than
		   ImplicitLittleEndian when running in SCU mode
		   (Note: when running in SCP mode, 'dgatesop.lst' is used)
		   Fix: CommandObjects are always ImplicitLittleEndian
20030627    mvh    Fix for MSC4.2; re-enabled obsolete write calls for test.cxx
20030710    ljz    Adjusted test.cxx, and disabled obsolete calls again
20080816    mvh	   ValidPresContexts is now a global: how many incoming connection can handle 
20090802    mvh    Added DCM_ERROR_DEBUG statements to debug failed connect
20100303    bcb    Commented out unused variables (gcc4.2 Warnings)
20100619    bcb    Fix gcc4 warnings, improve speed.
20100717    mvh    Merged
20100726    mvh    bcb documented the mess in function AddAbstractSyntaxAlias
20100728    bcb    May have fixed AddAbstractSyntaxAlias, but I have no way to test
20100815    mvh    Merged
20100914    bcb    Cleaned up AddAbstractSyntaxAlias problems (double release)
20100918    mvh    Merged
20110604    mvh    Count and show #candidates for "No valid presentation contexts found"
20110605    mvh    Added more DCM_ERROR_ASSOCIATION info that will show as proper server errors
20120820    bcb    Fixed a case where k-pacs sent a leading empty Transfer Syntax Name.
20130619    lsp    Return NULL from DetachRTC() rather than pointer to deleted object
20130807    mvh    Testing result of all :: Read operations fixes hanging thread problem if you ctrl-c a c-store script
20131016    mvh    Merged
20131106    mvh    Removed return FALSE on Failed read in Multiplex(int Socketfd) 
		   fixes multiplex connection terminated on some clients.
20140528    lsp    Kept member initialization only in constructors: not GNUC specific
20140607    mvh    Added PDU_Service::Write(DICOMCommandObject	*DCMObject, UID	&uid, BYTE PCid)
20140610    mvh    Merged
20140611    mvh    Fixed accidentally blocked if() statement
20141009    mvh    Added many debug messages to identify sources of failure
20160221    mvh	   Clear ItemType to avoid endloss loop after connection lost
20160827    mvh	   Released with 32768 PDU max size
20190109    mvh    Pass incoming MaxSubLength to PDataTF (print for now)
20190118    mvh    Take out print statement
20190318    mvh    Deal with zero returned MaxSubLength; use 8192 pdusize for Linux
20190319    mvh    Image corruption only stops at 4096 pdusize for Linux
20230608    mvh    Copy AAssociateRQ SCPSCURoles to AAssociateAC
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


PDU_Service	::	PDU_Service()
:ProposedAbstractSyntaxs(),
AcceptedPresentationContexts(),
SizeCap(0),
AttachedRTC(NULL),
DestructAttachedRTC(FALSE),
UsedTransferSyntaxUID(),
ValidPresContexts(0),
Link(),
#ifdef UNIX
pdusize(4096)
#else
pdusize(32768)
#endif
	{
	// PDU Level services are always big endian architecture
	Buffer :: SetIncomingEndian(BIG_ENDIAN);
	Buffer :: SetOutgoingEndian(BIG_ENDIAN);
	}

PDU_Service	::	~PDU_Service()
	{
	if(Link.Connected)
		Close();
	ClearAbstractSyntaxs();
	while ( AcceptedPresentationContexts.GetSize())
		AcceptedPresentationContexts.RemoveAt ( 0 );

	if ( AttachedRTC )
		if ( DestructAttachedRTC )
			delete AttachedRTC;
	}

BOOL
PDU_Service	::	AttachRTC (
	RTC		*vRTC,
	BOOL	DestructFlag)
	{
	DetachRTC();
	AttachedRTC = vRTC;
	DestructAttachedRTC = DestructFlag;
	return ( TRUE );
	}

RTC	*
PDU_Service	::	DetachRTC()
	{
	RTC	*vRTC;

	vRTC = AttachedRTC;

	if ( AttachedRTC )
		if ( DestructAttachedRTC )
			delete AttachedRTC;

	AttachedRTC = NULL;
	return ( NULL );
	}

BOOL	PDU_Service	::	Read(DICOMObject	*DCMObject)
	{
	BYTE		ItemType;

	if (!Link.Connected)
		{
		DicomError(DCM_ERROR_DEBUG, "Link.Connected false in PDU:Read", 0);
		return FALSE;
		}
	PDataTF :: MsgStatus = 0;
	if((PDataTF :: Length)!=0)	// for bad GCC v2.6.0
		{
		if (!PDataTF :: ReadDynamic( *this )) 
		{
		DicomError(DCM_ERROR_DEBUG, "ReadDynamic in PDU:Read failed", 0);
		return FALSE;
		}

		if (PDataTF :: MsgStatus > 0)
			{
			SizeCap = 0;
			if(!ParseRawVRIntoDCM(PDataTF :: PresentationContextID,
				PDataTF::VRBuffer, DCMObject))
				{
				AAbortRQ :: Write(*this);
				Link.Close();
				DicomError(DCM_ERROR_DEBUG, "ParseRawVRIntoDCM in PDU:Read failed", 0);
				return ( FALSE );
				}
			//DumpDDO((DICOMDataObject*)DCMObject);
			return(TRUE);
			}
		}
	
	while ( TRUE )
		{
		(*this) >> ItemType;
		switch ( ItemType )
			{
			case	0x01:	// A-ASSOCIATE-RQ Should not get this here
				if (!AAssociateRQ :: Read(*this)) 
					{
					DicomError(DCM_ERROR_DEBUG, "AAssociateRQ :: Read in unexpected state 1 in PDU:Read failed", 0);
					return FALSE;
					}
				AAbortRQ :: Write ( *this );
				Link.Close();
				DicomError(DCM_ERROR_DEBUG, "State 1 in PDU:Read unexpected", 0);
				return ( FALSE );
			case	0x02:	// A-ASSOCIATE-AC Should not get this here
				if (!AAssociateAC :: Read(*this)) 
					{
					DicomError(DCM_ERROR_DEBUG, "AAssociateAC :: Read un unexpected state 2 in PDU:Read failed", 0);
					return FALSE;
					}
				AAbortRQ :: Write ( *this );
				Link.Close();
				DicomError(DCM_ERROR_DEBUG, "State 2 in PDU:Read unexpected", 0);
				return ( FALSE );
			case	0x03:	// A-ASSOCIATE-RJ Should not get this here
				AAbortRQ :: Write ( *this );
				Link.Close();
				DicomError(DCM_ERROR_DEBUG, "State 3 in PDU:Read unexpected", 0);
				return ( FALSE );
			case	0x04:	// P-DATA-TF
//				fprintf(stderr, "start: P-DATA-TF Read\n");
				if (!PDataTF :: ReadDynamic( *this )) 
					{
					DicomError(DCM_ERROR_DEBUG, "ReadDynamic in PDU:Read failed", 0);
					return FALSE;
					}
//				fprintf(stderr, "end: P-DATA-TF Read : %d, Buffer Size %d\n",
//					PDataTF :: MsgStatus, Buffer :: GetSize());

				if (PDataTF :: MsgStatus > 0)
					{
					SizeCap = 0;
					if(!ParseRawVRIntoDCM(PDataTF :: PresentationContextID,
						PDataTF::VRBuffer, DCMObject))
						{
						AAbortRQ :: Write(*this);
						Link.Close();
						DicomError(DCM_ERROR_DEBUG, "ParseRawVRIntoDCM in PDU:Read failed", 0);
						return ( FALSE );
						}
			//DumpDDO((DICOMDataObject*)DCMObject);
					return(TRUE);
					}
				break;
			case	0x05:	// A-RELEASE-RQ
				if (!AReleaseRQ :: ReadDynamic(*this)) 
					{
					DicomError(DCM_ERROR_DEBUG, "AReleaseRQ::ReadDynamic in PDU:Read failed", 0);
					return FALSE;
					}
				// also drop
				AReleaseRP :: Write(*this);
				return ( FALSE );
			case	0x06:	// A-RELEASE-RP
				Link.Close();
				return ( FALSE );
			case	0x07:	// A-ABORT-RQ
				Link.Close();
				return ( FALSE );
			default:
				// Protocol Error
				AAbortRQ :: Write ( *this );
				Link.Close();
				DicomError(DCM_ERROR_DEBUG, "Protocol error %d in PDU:Read", ItemType);
				return ( FALSE );
			}
		}
	return ( FALSE );
	}

// These routines are being de-emphazised
#if THESE_ARE_OBSOLETE
BOOL	PDU_Service	::	Write(DICOMCommandObject	*DCMObject)
	{
	BYTE	TempByte;

	TempByte = ParseDCMIntoRawVR(DCMObject, PDataTF :: VRBuffer);
	if ( ! TempByte )
		{
		DicomError(DCM_ERROR_DEBUG, "ParseDCMIntoRawVR in PDU:Write(DICOMCommandObject) failed");
		//printf("Failure to find TempByte\n");
		return ( FALSE );
		}
	PDataTF :: PresentationContextID = TempByte;
	PDataTF :: MsgHeader = 0x01;
	return(PDataTF :: Write ( *this ));
	}

BOOL	PDU_Service	::	Write(DICOMDataObject	*DCMObject)
	{
	BYTE	TempByte;

	TempByte = ParseDCMIntoRawVR(DCMObject, PDataTF :: VRBuffer);
	if ( ! TempByte )
		{
		DicomError(DCM_ERROR_DEBUG, "ParseDCMIntoRawVR in PDU:Write(DICOMDataObject) failed");
		return ( FALSE );
		}
	PDataTF :: PresentationContextID = TempByte;
	PDataTF :: MsgHeader = 0x00;

	return(PDataTF :: Write ( *this ));
	}
#endif
// These routines are the recommended method of sending a DICOM Object


BOOL	PDU_Service	::	Write(DICOMCommandObject	*DCMObject, UID	&uid)
	{
	BYTE	TempByte, TempByte1;
	//DumpDDO((DICOMDataObject*)DCMObject);
	
	TempByte1 = GetPresentationContextID(uid);
//	printf("PDU_Service :: Write(DCMObject:%x, %s) PID = %d\n",
//		DCMObject, uid.GetBuffer(1), TempByte1);
	if(!TempByte1)
		{
		DicomError(DCM_ERROR_DEBUG, "GetPresentationContextID in PDU:Write(DICOMCommandObject, UID) failed", 0);
		return ( FALSE );
		}
	TempByte = ParseDCMIntoRawVR(DCMObject, PDataTF :: VRBuffer, uid, FALSE);
//	printf("PDU_Service :: Write(DCMObject:%x, %s) RetParseDCMIntoRawVR = %d\n",
//		DCMObject, uid.GetBuffer(1), TempByte);
	if ( ! TempByte )
		{
		DicomError(DCM_ERROR_DEBUG, "ParseDCMIntoRawVR in PDU:Write(DICOMCommandObject, UID) failed", 0);
		return ( FALSE );
		}

	PDataTF :: PresentationContextID = TempByte;
	PDataTF :: MsgHeader = 0x01;
	return(PDataTF :: Write ( *this ));
	}

BOOL	PDU_Service	::	Write(DICOMCommandObject	*DCMObject, UID	&uid, BYTE	PCid)
	{
	BYTE	TempByte, TempByte1;
	//DumpDDO((DICOMDataObject*)DCMObject);
	
	TempByte1 = GetPresentationContextID(uid);
//	printf("PDU_Service :: Write(DCMObject:%x, %s) PID = %d\n",
//		DCMObject, uid.GetBuffer(1), TempByte1);
	if(!TempByte1)
		{
		DicomError(DCM_ERROR_DEBUG, "GetPresentationContextID in PDU:Write(DICOMCommandObject, UID, BYTE) failed", 0);
		return ( FALSE );
		}
	TempByte = ParseDCMIntoRawVR(DCMObject, PDataTF :: VRBuffer, uid, FALSE);
//	printf("PDU_Service :: Write(DCMObject:%x, %s) RetParseDCMIntoRawVR = %d\n",
//		DCMObject, uid.GetBuffer(1), TempByte);
	if ( ! TempByte )
		{
		DicomError(DCM_ERROR_DEBUG, "ParseDCMIntoRawVR in PDU:Write(DICOMCommandObject, UID, BYTE) failed", 0);
		return ( FALSE );
		}

	if (PCid)
	  PDataTF :: PresentationContextID = PCid;
	else
	  PDataTF :: PresentationContextID = TempByte;
	PDataTF :: MsgHeader = 0x01;
	return(PDataTF :: Write ( *this ));
	}

BOOL	PDU_Service	::	Write(DICOMDataObject	*DCMObject, UID	&uid)
	{
	BYTE	TempByte, TempByte1;

	//DumpDDO((DICOMDataObject*)DCMObject);
	TempByte1 = GetPresentationContextID(uid);
	if(!TempByte1)
		{
		DicomError(DCM_ERROR_DEBUG, "GetPresentationContextID in PDU:Write(DICOMDataObject, UID) failed", 0);
		return ( FALSE );
		}

	TempByte = ParseDCMIntoRawVR(DCMObject, PDataTF :: VRBuffer, uid, TRUE);
	if ( ! TempByte )
		{
		DicomError(DCM_ERROR_DEBUG, "ParseDCMIntoRawVR in PDU:Write(DICOMDataObject, UID) failed", 0);
		return ( FALSE );
		}
	PDataTF :: PresentationContextID = TempByte;
	PDataTF :: MsgHeader = 0x00;

	return(PDataTF :: Write ( *this ));
	}


BOOL	PDU_Service	::	InterogateAAssociateRQ()
	{
	UINT	Index, IndexTrn, ValidP, temp;
	TransferSyntax				TrnSyntax;
	PresentationContext			PresContext;
	PresentationContextAccept	PresContextAccept;
	UserInformation				UserInfo;
	MaximumSubLength			MaxSubLength;
	Array<PresentationContext>	PCArray ( FALSE );


	if(!ShouldIAcceptRemoteApTitle(AAssociateRQ :: CallingApTitle))
		{
		AAssociateRJ :: Reason = 3;	// Calling party not rec.
		AAssociateRJ :: Source = 1;	// PDU
		AAssociateRJ :: Result = 1;	// Permanent
		AAssociateRJ :: Write ( *this );
		DicomError(DCM_ERROR_ASSOCIATION, "Calling AE title not accepted", 0);
		return ( FALSE );
		}
	if(!ShouldIAcceptLocalApTitle(AAssociateRQ :: CalledApTitle))
		{
		AAssociateRJ :: Reason = 7;	// Called Party Not Rec.
		AAssociateRJ :: Source = 1;	// PDU
		AAssociateRJ :: Result = 1;	// Permanent
		AAssociateRJ :: Write ( *this );
		DicomError(DCM_ERROR_ASSOCIATION, "Called AE title not accepted", 0);
		return ( FALSE );
		}
	if(!ShouldIAcceptApplicationContext(AAssociateRQ :: AppContext))
		{
		AAssociateRJ :: Reason = 2;	// AppContext Not-Supported
		AAssociateRJ :: Source = 1;	// PDU
		AAssociateRJ :: Result = 1;	// Permanent
		AAssociateRJ :: Write ( *this );
		DicomError(DCM_ERROR_ASSOCIATION, "AppContext Not-Supported", 0);
		return ( FALSE );
		}
	// Transfer the information over to the A-ASSOCIATE-AC Class
	AAssociateAC :: SetCalledApTitle(AAssociateRQ :: CalledApTitle);
	AAssociateAC :: SetCallingApTitle(AAssociateRQ :: CallingApTitle);
	AAssociateAC :: AppContext = AAssociateRQ :: AppContext;
	AAssociateAC :: UserInfo = AAssociateRQ :: UserInfo;

	// Ok, now, we need to find an acceptable presentation context
	Index = 0;
	ValidPresContexts = ValidP = temp = 0;
	PCArray = (AAssociateRQ :: PresContexts);

	while ( AcceptedPresentationContexts.GetSize())
		AcceptedPresentationContexts.RemoveAt ( 0 );
	/* Check all Presentation Contexts proposed by the other side, e.g.:
		CTStorage			1.2.840.10008.5.1.4.1.1.2	sop
		USMultiframeStorage		1.2.840.10008.5.1.4.1.1.3.1	sop
		MRStorage			1.2.840.10008.5.1.4.1.1.4	sop
		USStorage			1.2.840.10008.5.1.4.1.1.6.1	sop
		SCStorage			1.2.840.10008.5.1.4.1.1.7	sop
	*/
	while ( Index < PCArray.GetSize())
		{
		PresContext = PCArray.Get ( Index );
		PresContext.TrnSyntax.ClearType = FALSE;
		IndexTrn = 0;
		PresContextAccept.PresentationContextID =
			PresContext.PresentationContextID;
		/* Fix: default to (Transfer Syntaxes Not Supported) */
		PresContextAccept.Result = 4;
		/* For each Presentation Context, multiple TransferTyntaxes may be proposed, e.g:
			LittleEndianImplicit		1.2.840.10008.1.2
			LittleEndianExplicit		1.2.840.10008.1.2.1
			BigEndianExplicit		1.2.840.10008.1.2.2
			JPEGLossless			1.2.840.10008.1.2.4.70
			RLELossless			1.2.840.10008.1.2.5
			JPEG2000			1.2.840.10008.1.2.4.91
		*/
		temp += PresContext.TrnSyntax.GetSize();
		while ( IndexTrn < PresContext.TrnSyntax.GetSize())
			{
			TrnSyntax = PresContext.TrnSyntax.Get(IndexTrn);
			/* Following routine could better be named 'CanI...' */
			//bcb Found a case of k-pacs sending a leading empty TransferSyntaxName, the second one was valid. 17 is the smallest.
			if(TrnSyntax.TransferSyntaxName.GetSize() > 16 && CanYouHandleTransferSyntax(TrnSyntax))
				{
				PresContextAccept.TrnSyntax.TransferSyntaxName =
				TrnSyntax.TransferSyntaxName;
				/* OK: Accept the Transfer Syntax */
				PresContextAccept.Result = 0;
				AcceptedPresentationContexts.Add ( PresContextAccept );
				++ValidPresContexts; ++ValidP;
				break;
				}
			++IndexTrn;
			}
		/* Do we support the offered Presentation Context? */
		if(!ShouldIAcceptAbstractSyntax(PresContext.AbsSyntax))
		{
			PresContextAccept.Result = 3;
			ValidPresContexts--;
		}
		AAssociateAC :: AddPresentationContextAccept(PresContextAccept);
		++Index;
		}
	if(!ValidP)
		{
		DicomError(DCM_ERROR_ASSOCIATION, "No valid presentation contexts/transfer syntax found in %d candidates", temp);
		DicomError(DCM_ERROR_ASSOCIATION, "In %d presentation contexts", PCArray.GetSize());
		DicomError(DCM_ERROR_ASSOCIATION, "#Possible transfer syntaxes: %d", ((CheckedPDU_Service*)this)->TransferUIDListCount);
		AAssociateAC :: Write ( *this );
		return ( FALSE );
		}
	MaxSubLength.Set(pdusize);	// we can do all DICOM can handle
	GetImplementationClass(UserInfo.ImpClass);
	GetImplementationVersion(UserInfo.ImpVersion);
	UserInfo.SetMax(MaxSubLength);
        for (int i=0; i<AAssociateRQ :: UserInfo.SCPSCURole.GetSize(); i++)
          UserInfo.AddSCPSCURole(AAssociateRQ :: UserInfo.SCPSCURole.Get(i));

	AAssociateAC :: SetUserInformation ( UserInfo );
	AAssociateAC :: Write ( *this );

	return ( TRUE );	// yes, the communication should work out
	}


BOOL	PDU_Service	::	InterogateAAssociateAC()
	{
	UINT	Index;//,ValidPresContexts, IndexTrn;
//	BOOL	Good;
//	TransferSyntax				TrnSyntax;
//	PresentationContext			PresContext;
	PresentationContextAccept	PresContextAccept;
	Array<PresentationContextAccept>	PCArray ( FALSE );

	Index = 0;
//	ValidPresContexts = 0;
	PCArray = (AAssociateAC :: PresContextAccepts);

	//printf("InterogateAAssociateAC : PCArray.GetSize() = %d\n",
	//	PCArray.GetSize());
	while ( AcceptedPresentationContexts.GetSize())
		AcceptedPresentationContexts.RemoveAt ( 0 );

	while ( Index < PCArray.GetSize())
		{
		PresContextAccept = PCArray.Get ( Index );
		if(!PresContextAccept.Result)
			{
			//printf("Remote Accepting Abstract Class\n");
			AcceptedPresentationContexts.Add ( PresContextAccept );
			}
//		else
//			printf("Remote Rejecting Abstract Class [%d]\n",
//				PresContextAccept.Result);

		++Index;
		}
	if(!AcceptedPresentationContexts.GetSize())
		{
		DicomError(DCM_ERROR_ASSOCIATION, "No accepted presentation contexts/transfer syntax found in %d candidates", PCArray.GetSize());
		return ( FALSE );	// no, this association should be terminated
		}
	return ( TRUE );	// yes, the communication should work out
	}


// Checks the Known Class list for the UID and further verifies that
// it's a big/little Endian.  If it is, then it sets the Transfer type
// in the TransferSyntax for further reference
BOOL	PDU_Service	::	CanYouHandleTransferSyntax(TransferSyntax &TrnSyntax)
	{
	/*
	KnownClass	KC;

	if(KnownUID(TrnSyntax.TransferSyntaxName, KC))
		return ( FALSE );
	
	if(KC.Type != "transfer")
		return ( FALSE );

	if(KC.Comment != "LittleEndian")
		{
		if(KC.Command != "BigEndian")
			return ( FALSE );
		else
			TrnSyntax.SetType ( BIG_ENDIAN );
		}
	else
			TrnSyntax.SetType ( LITTLE_ENDIAN );
	*/
	if(!strcmp((char*) TrnSyntax.TransferSyntaxName.GetBuffer(1), "1.2.840.10008.1.2"))
		{
		TrnSyntax.SetType ( LITTLE_ENDIAN );
		return ( TRUE );
		}
	return ( FALSE );
	}

BOOL	PDU_Service	::	Listen(BYTE	*port)
	{
	PDataTF :: Length = 0;
	
	if (Link.Connected)
		{
		DicomError(DCM_ERROR_DEBUG, "Link not connected in PDU:Listen", 0);
		Link.Close();
		return ( FALSE );
		}


	if(!Link.Listen((char *) port))
		{
		DicomError(DCM_ERROR_DEBUG, "Socket listen failed in PDU:Listen", 0);
		return ( FALSE );
		}

	if(!Link.Accept())
		{
		DicomError(DCM_ERROR_DEBUG, "Socket accept failed in PDU:Listen", 0);
		return ( FALSE );
		}

//	fprintf(stderr, "Waiting for AAssociateRQ\n");
	if (!AAssociateRQ :: Read( *this )) 
		{
		DicomError(DCM_ERROR_DEBUG, "AAssociateRQ :: Read failed in PDU:Listen", 0);
		return FALSE;
		}
//	fprintf(stderr, "Interogating AAssociateRQ\n");
	if(!InterogateAAssociateRQ())
		{
//		fprintf(stderr, "failed to connect\n");
		DicomError(DCM_ERROR_DEBUG, "InterogateAAssociateRQ failed in PDU:Listen", 0);
		Link.Close();
		return ( FALSE );
		}
//	fprintf(stderr, "connected, but not disconnecting (missing P-DATA service)\n");
//	AAbortRQ :: Write ( *this );

//	return ( FALSE );
	return ( TRUE );
	}

int	PDU_Service	::	Multiplex(BYTE	*port)
	{
	int	TempSocket;
	
	PDataTF :: Length = 0;

	if(!Link.Listen((char*)port))
		{
		DicomError(DCM_ERROR_DEBUG, "link.listen in PDU:Multiplex failed", 0);
		return(-1);
		}

	TempSocket = Link.ListenSocketfd;
	Link.ListenSocketfd = 0;
	Link.Listened = FALSE;
	return (Listen(TempSocket));
	}

int	PDU_Service	::	Listen(int	sock)
	{
	int	TempSocket;

	if(Link.Connected)
		{
		DicomError(DCM_ERROR_DEBUG, "link not connected in PDU:Listen", 0);
		Link.Close();
		return ( FALSE );
		}
	Link.ListenSocketfd = sock;
	Link.Listened =  TRUE;

	while (!Link.Connected)
		{	
		if(!Link.Accept())
			{
			DicomError(DCM_ERROR_DEBUG, "link.accept failed in PDU:Listen", 0);
			return ( FALSE );
			}
	
		if (!AAssociateRQ :: Read( *this )) 
			{
			DicomError(DCM_ERROR_DEBUG, "AAssociateRQ :: Read failed in PDU:Listen", 0);
			return FALSE;
			}
		if(!InterogateAAssociateRQ())
			{
			TempSocket = Link.ListenSocketfd;
			Link.Listened = FALSE;
			Link.Close();
			Link.Listened = TRUE;
			Link.ListenSocketfd = TempSocket;
			}
		}
	TempSocket = Link.ListenSocketfd;
	Link.ListenSocketfd = 0;
	Link.Listened = 0;
	return ( TempSocket );	
	}

BOOL	PDU_Service	::	Multiplex(int	Socketfd)
	{
	if(Link.Connected)
		{
		DicomError(DCM_ERROR_DEBUG, "link is not connected", 0);
		Link.Close();
		}
	Link.Connected = TRUE;
	Link.Socketfd = Socketfd;
//	if (!AAssociateRQ :: Read( *this )) return FALSE;
	if (!AAssociateRQ :: Read( *this ))
		DicomError(DCM_ERROR_DEBUG, "Invalid message in PDU:Multiplex", 0);

	if(!InterogateAAssociateRQ())
		{
		DicomError(DCM_ERROR_DEBUG, "no transfer syntax could be negotiated", 0);
		Link.Close();
		return(FALSE);
		}
	return (TRUE);
	}	

BOOL	PDU_Service	::	Close()
	{
	PDataTF :: Length = 0;
	if ( Link.Connected )
		{
		AReleaseRQ :: Write ( *this );
		if (!AReleaseRP :: Read ( *this )) 
			{
			DicomError(DCM_ERROR_DEBUG, "AReleaseRP :: Read failed in PDU:Close", 0);
			return FALSE;
			}
		}
	Link.Close();
	return ( TRUE );
	}

// Support routine to get Presentation Context ID from UID

BYTE	PDU_Service	::	GetAcceptedPCIDForTransferSyntax(UID	&uid)
	{
	UINT	Index;
	PresentationContextAccept	PCA;

	Index = 0;
	while (Index < AcceptedPresentationContexts.GetSize() )
		{
		PCA = AcceptedPresentationContexts.Get ( Index );
/*		fprintf(stderr, "Comparing: %s with %s [%d:%d]\n",
			PCA.TrnSyntax.TransferSyntaxName.GetBuffer(1),
			uid.GetBuffer(1),
			PCA.TrnSyntax.TransferSyntaxName.GetSize(),
			uid.GetSize());
*/
			if(PCA.TrnSyntax.TransferSyntaxName == uid)
			return ( PCA.PresentationContextID );	
		++Index;
		}
	return ( 0 );
	}

BYTE	PDU_Service	::	GetPresentationContextID(UID	&uid)
	{
	UINT	Index;
	Array <PresentationContext>	PCArray ( FALSE );
	PresentationContext		PresContext;

	PCArray = (AAssociateRQ :: PresContexts);

	Index = 0;
	while ( Index < PCArray.GetSize())
		{
		PresContext = PCArray.Get ( Index );
		PresContext.TrnSyntax.ClearType = FALSE;
//		printf("PDU_Service::GetPresentationContextID, Compare \"%s\"\n",
//			PresContext.AbsSyntax.AbstractSyntaxName.GetBuffer(1));
		if(PresContext.AbsSyntax.AbstractSyntaxName == uid)
			return ( PresContext.PresentationContextID );
		++Index;
		}

	return ( 0 );
	}

// Get the PCID for a given AbsUID and TrnUID
BYTE	PDU_Service	::	GetPresentationContextID(UID	&AbsUID, UID	&TrnUID)
	{
	UINT	Index;
	Array <PresentationContext>	PCArray ( FALSE );
	PresentationContext			PresContext;

	PCArray = (AAssociateRQ :: PresContexts);

	Index = 0;
//	printf("GetPresentationContextID, Searching for: %s/%s\n", AbsUID.GetBuffer(1),
//		TrnUID.GetBuffer(1));
	
	while ( Index < PCArray.GetSize())
		{
		PresContext = PCArray.Get ( Index );
		PresContext.TrnSyntax.ClearType = FALSE;
		if(PresContext.AbsSyntax.AbstractSyntaxName == AbsUID)
			{
//			printf("GetPresentationContextID :: Found %s in AAssociateRQ :: PresContexts\n",
//				AbsUID.GetBuffer(1));
			UINT	Index2;
			PresentationContextAccept	PCA;
			Index2 = 0;
			while (Index2 < AcceptedPresentationContexts.GetSize() )
				{
				PCA = AcceptedPresentationContexts.Get ( Index2 );
//				printf("PCA.TrnSyntax.TransferSyntaxName = %s\n",
//					PCA.TrnSyntax.TransferSyntaxName.GetBuffer(1));
				if((PCA.TrnSyntax.TransferSyntaxName == TrnUID) &&
					(PCA.PresentationContextID == PresContext.PresentationContextID))
					{
//					printf("Returning Good!");
					return ( PCA.PresentationContextID );
					}
				++Index2;
				}
			}
		++Index;
		}

//	printf("GetPresentationContextID :: Error, cannot find in Accepted PC List\n");
	return ( 0 );
	}

BOOL	PDU_Service	::	GetTransferSyntaxUID(BYTE	PCID, UID	&uid)
	{
	UINT	Index;
	PresentationContextAccept	PCA;

	Index = 0;
	while (Index < AcceptedPresentationContexts.GetSize() )
		{
		PCA = AcceptedPresentationContexts.Get ( Index );
		if(PCA.PresentationContextID == PCID)
			{
			uid = PCA.TrnSyntax.TransferSyntaxName;
			return ( TRUE );
			}
		++Index;
		}
	return ( FALSE );
	}

BOOL	PDU_Service	::	IsAbstractSyntaxAccepted(UID &uid)
{
	BYTE	id;
	UINT	Index;
	PresentationContextAccept	PCA;


	id = GetPresentationContextID(uid);
	if (!id)
		return FALSE;

	Index = 0;
	while (Index < AcceptedPresentationContexts.GetSize() )
		{
		PCA = AcceptedPresentationContexts.Get ( Index );
/*		fprintf(stderr, "Comparing: %s with %s [%d:%d]\n",
			PCA.TrnSyntax.TransferSyntaxName.GetBuffer(1),
			uid.GetBuffer(1),
			PCA.TrnSyntax.TransferSyntaxName.GetSize(),
			uid.GetSize());
*/
		if(PCA.PresentationContextID == id)
			return TRUE;
		++Index;
		}
	return FALSE;
}

BOOL	PDU_Service	::	ClearAbstractSyntaxs ()
	{
	while (ProposedAbstractSyntaxs.GetSize())
		ProposedAbstractSyntaxs.RemoveAt ( 0 );
	return ( TRUE );
	}

BOOL	PDU_Service	::	AddAbstractSyntax(UID &uid)
	{
	UINT			Index;
	AbstractSyntax	AbsSyntax;

	Index = 0;
	while ( Index < ProposedAbstractSyntaxs.GetSize() )
		{
		if ( uid ==
			ProposedAbstractSyntaxs.Get ( Index ).AbstractSyntaxName)
			return ( TRUE );	// already in array
		++Index;
		}
	AbsSyntax.Set(uid);
	ProposedAbstractSyntaxs.Add ( AbsSyntax );
	return ( TRUE );
	}

// Used only in StorageApp ::	PrinterSupport
BOOL	PDU_Service	::	AddAbstractSyntaxAlias (
	UID		&UIDSource,
	UID		&UIDAlias)
	{
	UINT				Index;
	Array <PresentationContext>	PCArray ( FALSE );
	PresentationContext		PresContext;

	// First check to see if it's already aliased
	if ( GetPresentationContextID ( UIDAlias ) )
		return ( TRUE );

	PCArray = (AAssociateRQ :: PresContexts);

	Index = 0;
	while ( Index < PCArray.GetSize())
		{
		PresContext = PCArray.Get ( Index );
		PresContext.TrnSyntax.ClearType = FALSE;// Original does not clear the array.

		if(PresContext.AbsSyntax.AbstractSyntaxName == UIDSource)
			{
			Array<TransferSyntax> vTrnSyntax = PresContext.TrnSyntax; //Copies just pointers, see array.thh
			vTrnSyntax.ClearType = FALSE; // Pointers still belong to AAssociateRQ :: PresContexts

//			PresentationContext	vPresContext = PresContext;// Does not copy the array, just pointers.
//			vPresContext.TrnSyntax.ClearType = FALSE;// The copy does not clear the array, leak?

			PresContext.TrnSyntax.Clear();// The original loses pointers to the array.
			UINT	SubIndex = 0;

			while (SubIndex < vTrnSyntax.GetSize() ) //bcb replaced loop 20100728
				{
				PresContext.TrnSyntax.Add(vTrnSyntax.Get(SubIndex));// Copies the values.
				++SubIndex;
				}

/*			while (SubIndex < vPresContext.TrnSyntax.GetSize() )
				{
				PresContext.TrnSyntax.Add(vPresContext.TrnSyntax.Get(SubIndex));// Copies the values.
				++SubIndex;
				}
*/

			PresContext.TrnSyntax.ClearType = TRUE;// The ability to clear the new array.
			PresContext.AbsSyntax.AbstractSyntaxName = UIDAlias;
			AAssociateRQ :: AddPresentationContext(PresContext);// Sets ClearType.
			PresContext.TrnSyntax.ClearType = FALSE;// Take it back, pointers still belong to AAssociateRQ :: PresContexts

            		// Now we clear the array for real when the destructor vTrnSyntax is called. bcb added 20100728
			return ( TRUE );// Here calls destructor for vPresContext, but the old array survives?
			}
		++Index;
		}

	// The source was not proposed

	return ( FALSE );
	}

BOOL	PDU_Service	::	SetApplicationContext(UID &uid)
	{
	AAssociateRQ :: SetApplicationContext(uid);
	return ( TRUE );
	}

BOOL	PDU_Service	::	SetLocalAddress(BYTE *address)
	{
	AAssociateRQ :: SetCallingApTitle(address);
	return ( TRUE );
	}

BOOL	PDU_Service	::	SetRemoteAddress(BYTE *address)
	{
	AAssociateRQ :: SetCalledApTitle(address);
	return ( TRUE );
	}

BOOL	PDU_Service	::	GetLocalAddress(BYTE *address)
	{
	memset(address, 0, 17); memcpy(address, AAssociateAC :: CallingApTitle, 16);
	return ( TRUE );
	}

BOOL	PDU_Service	::	GetRemoteAddress(BYTE *address)
	{
	memset(address, 0, 17); memcpy(address, AAssociateAC :: CalledApTitle, 16);
	return ( TRUE );
	}

BOOL	PDU_Service	::	AddTransferSyntaxs(PresentationContext &PresContext)
	{
	UID		uid;
	TransferSyntax	TrnSyntax;
//	char		szAbstractSyntax[100];

/*
	uid = PresContext.AbsSyntax.AbstractSyntaxName;
	strncpy(szAbstractSyntax, (char*)uid.GetBuffer(0), uid.GetSize());
	szAbstractSyntax[uid.GetSize()] = 0;
	if ((strcmp(szAbstractSyntax, "1.2.840.10008.1.1") != 0) &&	// c-echo
	    (strstr(szAbstractSyntax, ".5.1.4.1.2.") == 0))		// c-find
	{
		uid.Set("1.2.840.10008.1.2.4.70");	// LJ LosslessJPEG
		TrnSyntax.Set(uid);
		PresContext.AddTransferSyntax(TrnSyntax);
	}
*/
	uid.Set("1.2.840.10008.1.2");		// LJ ImplicitLittleEndian
	TrnSyntax.Set(uid);
	PresContext.AddTransferSyntax(TrnSyntax);
//	uid.Set("1.2.840.10008.1.2.1");		// LJ ExplicitLittleEndian
//	TrnSyntax.Set(uid);
//	PresContext.AddTransferSyntax(TrnSyntax);
	return ( TRUE );
	}

BOOL	PDU_Service	::	GetImplementationClass(ImplementationClass &ImpClass)
	{
	ImpClass.Set((BYTE*) IMPLEMENTATION_CLASS_STRING);
	return(TRUE);
	}

BOOL	PDU_Service	::	GetImplementationVersion(ImplementationVersion &ImpVersion)
	{
	ImpVersion.Set((BYTE*) IMPLEMENTATION_VERSION_STRING);
	return ( TRUE );
	}

BOOL	PDU_Service	::	Connect ( BYTE *ip, BYTE *port )
	{
	UINT					Index;
	PresentationContext		PresContext;
	UserInformation			UserInfo;
	MaximumSubLength		MaxSubLength;
	BYTE					ItemType;

	PDataTF :: Length = 0;

	if (!ProposedAbstractSyntaxs.GetSize () )
		{
		DicomError(DCM_ERROR_DEBUG, "connect failed due to no proposed syntaxes (caller error)", 0);
		return ( FALSE );		// must proposed atleast one SOP class
		}

	AAssociateRQ :: ClearPresentationContexts();
	Index = 0;

	//AddTransferSyntaxs(PresContext);
	while ( Index < ProposedAbstractSyntaxs.GetSize())
		{
		PresContext.TrnSyntax.Clear();
		PresContext.SetAbstractSyntax ( ProposedAbstractSyntaxs.Get ( Index ) );
		AddTransferSyntaxs(PresContext);
		PresContext.PresentationContextID = uniq8odd();
		AAssociateRQ :: AddPresentationContext ( PresContext );
		++Index;
		}

	PresContext.TrnSyntax.ClearType = FALSE;
	MaxSubLength.Set(pdusize);	// we can do all DICOM can handle
	GetImplementationClass(UserInfo.ImpClass);
	GetImplementationVersion(UserInfo.ImpVersion);
	UserInfo.SetMax(MaxSubLength);

	AAssociateRQ :: SetUserInformation ( UserInfo );

	// ok, every thing proposed, now try and contact the remote end
	if ( ! Link.Open ( (char*) ip, (char*)port ) )
		{
		DicomError(DCM_ERROR_DEBUG, "connect failed on socket level (called not running)", 0);
		return ( FALSE );
		}

	AAssociateRQ :: Size();
	AAssociateRQ :: Write ( *this );

 	(*this) >> ItemType;
//	fprintf(stderr, "Got Response Type: %d\n", ItemType);
	switch ( ItemType )
		{
		case	0x02:
			if (!AAssociateAC :: ReadDynamic ( * this )) 
				{
				DicomError(DCM_ERROR_DEBUG, "AAssociateAC :: ReadDynamic failed in PDU:Connect", 0);
				return FALSE;
				}
			if(!InterogateAAssociateAC())
				{
				DicomError(DCM_ERROR_DEBUG, "InterogateAAssociateAC failed in PDU:Connect", 0);
				//printf("Failed to InterogateAAssociateAC\n");
				Link.Close();
				return ( FALSE );
				}
			// printf("MaxSubLength=%d\n", AAssociateAC :: UserInfo.GetMax());
			PDataTF::MaxSubLen = AAssociateAC :: UserInfo.GetMax();
			if (PDataTF::MaxSubLen==0) PDataTF::MaxSubLen = pdusize;
			return ( TRUE );	// this is what we want to happen
		case	0x03:
			if (!AAssociateRJ :: ReadDynamic ( * this ))
				{
				DicomError(DCM_ERROR_DEBUG, "AAssociateRJ :: ReadDynamic failed in PDU:Connect", 0);
				return FALSE;
				}
			DicomError(DCM_ERROR_DEBUG, "PDU:Connect failed due to reject", 0);
			Link.Close();
			return ( FALSE );
		default:
			// corrupt transmission
			Link.Close();
			DicomError(DCM_ERROR_DEBUG, "PDU:Connect failed due to corrupt transmission %d", ItemType);
			return ( FALSE );
		}
	
	DicomError(DCM_ERROR_DEBUG, "connect failed due to incomplete response", 0);
	return ( FALSE );
	}

