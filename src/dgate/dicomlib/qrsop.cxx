/*
20020429	mvh	Fixed reporting of number of complete slices (now starts with 1)
			return cancel status if C_STORE failed
20030606	ljz	Added parameter 'QueryRetrieveLevel' to CFindRSP::Write
20050401	mvh	Added ModalityWorkListQuery
20051222	mvh	Added pADDO to Query::Write method - answer appended to pADDO if set
20080820	mvh	Solaris fix (do not use variable name SS)
20090205	mvh     Added QueryMoveScript callbacks
20100122	mvh     Added support for C-CANCEL: poll the socket for any data and cancel if so
20100309	bcb     Added double parentheses (gcc4.2 Warnings)
20100619	bcb     Made Port local (lPort).
20100707	mvh     Merged
20100816	mvh     Added QueryResultScript
20140414        ea      Added std::vector to store database query results 
                        because indexing in linked-list takes too much time.
20140527        mvh     Replaced std::vector by straight malloc to fix compile issue on some systems
20151115	mvh	Added <malloc.h>
20160827	mvh	respond to destroy in QueryResultScript
20160830	mvh	fixed that, reversed the test
20170904        mvh     Limit reported image count to 65535 in move response
20171018        mvh     Fix linux compile, can't use min()
20200203	mvh	lsp removed DataPointer stuff to support lsp array change
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

#include <malloc.h>
#include "dicom.hpp"

BOOL	StandardQuery	::	Read (
	PDU_Service			*PDU,
	DICOMCommandObject	*DCO)
	{
	UID	MyUID, uid;
	VR	*vr;
	DICOMDataObject	DDO;
	Array < DICOMDataObject *>	ADDO;
	UINT	Index;
	BOOL	Cancelled;

	GetUID(MyUID);

	if( ! PDU )
		return ( FALSE );

	if( ! DCO )
		return ( FALSE );

	vr = DCO->GetVR(0x0000, 0x0002);
	SetUID(uid, vr);
	if (!(MyUID == uid))
		return ( FALSE );

	if (! CFindRQ :: Read (DCO, PDU, &DDO) )
		return ( FALSE ); // my SOP, but wrong command

	if (! QueryMoveScript (PDU, DCO, &DDO))
		{
		CFindRSP :: Write (PDU, DCO, 0xc011, NULL, QueryRetrieveLevel);
		return ( TRUE );
		}

	if (! SearchOn (&DDO, &ADDO) )
		{
		CFindRSP :: Write (PDU, DCO, 0xc001, NULL, QueryRetrieveLevel);
		return ( TRUE );
		}
	
	Index = 0;
	Cancelled = FALSE;
	
	DICOMDataObject **LocalDDOArray = (DICOMDataObject**) malloc(ADDO.GetSize()*sizeof(DICOMDataObject**));
        // std::vector<DICOMDataObject *> LocalDDOArray(ADDO.GetSize());

        //DataLink<DICOMDataObject *> *DataPointer = ADDO.first;
        
        while( Index < ADDO.GetSize())
        { LocalDDOArray[Index] = ADDO.Get(Index); //DataPointer->Data;
          //DataPointer = DataPointer->next;
          ++Index;
        }

        Index=0;

	while ( Index < ADDO.GetSize() )
		{
                DICOMDataObject *localDDO = LocalDDOArray[Index];

		if (!Cancelled)
			{
			if (QueryResultScript (PDU, DCO, localDDO))
				CFindRSP :: Write (PDU, DCO, localDDO, QueryRetrieveLevel);
			}
		else
			{
			if (PDU->Link.Poll()) 
			      {
			      DicomError(DCM_ERROR_DEBUG, "Query received cancel request", 0);
			      Cancelled = TRUE;
			      }
			}
		delete localDDO;
		++Index;
		}
	CFindRSP :: Write ( PDU, DCO, NULL, QueryRetrieveLevel);
	free(LocalDDOArray);
	
        return ( TRUE );
	}

BOOL	StandardQuery	::	Write (
	PDU_Service		*PDU,
	DICOMDataObject	*DDO,
	Array < DICOMDataObject  *> *pADDO)
	{
	DICOMCommandObject	*DCO;
	DICOMDataObject		*RDDO;

	if ( ! PDU )
		return ( FALSE );

	if ( ! CFindRQ :: Write ( PDU, DDO ) )
		return ( FALSE );

	CallBack ( NULL, DDO );

	DCO = new DICOMCommandObject;

	while ( PDU->Read ( DCO ) )
		{
		RDDO = new DICOMDataObject;

		if (! (CFindRSP :: Read ( DCO, PDU, RDDO) ) )
			{
			return ( FALSE );
			}
		if ( DCO->GetUINT16(0x0000, 0x0800) == 0x0101)
			{
			CallBack ( DCO, NULL );
			delete RDDO;
			delete DCO;
			return ( TRUE );
			}
		CallBack ( DCO, RDDO );
		if (pADDO) pADDO->Add(RDDO);
		else       delete RDDO;
		delete DCO;
		DCO = new DICOMCommandObject;
		}

	delete DCO;
	return ( FALSE );
	}

BOOL	StandardRetrieve	::	Read (
	PDU_Service			*PDU,
	DICOMCommandObject	*DCO)
	{
	UID	MyUID, uid, iUID, AppUID ("1.2.840.10008.3.1.1.1");
	VR	*vr;
	DICOMDataObject	DDO;
	Array < DICOMDataObject *>	ADDO;
	UINT		Index;
	BYTE		IP [ 64 ], lPort [ 64 ], ACRNema [ 17 ], MyACR[17];
	StandardStorage	*SStorage;
	DICOMDataObject	*iDDO;
	PDU_Service	NewPDU;
	UINT16		Failed;
	UINT16		status=0;

	GetUID(MyUID);

	if( ! PDU )
		return ( FALSE );

	if( ! DCO )
		return ( FALSE );

	vr = DCO->GetVR(0x0000, 0x0002);
	SetUID(uid, vr);
	if (!(MyUID == uid))
		return ( FALSE );

	if (! CMoveRQ :: Read (DCO, PDU, &DDO) )
		return ( FALSE ); // my SOP, but wrong command


	vr = DCO->GetVR(0x0000, 0x0600);
	if(!vr)
		{
		CMoveRSP :: Write ( PDU, DCO, 0xc001 , 0, 0, 0, 0, NULL );
		return ( TRUE );
		}

	memset((void*)ACRNema, 0, 17);
	if(vr->Length > 16)
		vr->Length = 16;
	memcpy((void*)ACRNema, vr->Data, (int) vr->Length);
	if(!vr->Length)
		{
		CMoveRSP :: Write ( PDU, DCO, 0xc002 , 0, 0, 0, 0, NULL );
		return ( TRUE );
		}

	if(ACRNema[vr->Length-1]==' ')
		ACRNema[vr->Length-1] = '\0';

	if(!QualifyOn(ACRNema, MyACR, IP, lPort))
		{
		CMoveRSP :: Write ( PDU, DCO, 0xc003 , 0, 0, 0, 0, NULL );
		return ( TRUE );
		}

	if (! QueryMoveScript (PDU, DCO, &DDO))
		{
		CMoveRSP :: Write (PDU, DCO, 0xc013, 0, 0, 0, 0, NULL );
		return ( TRUE );
		}

	if (! SearchOn (&DDO, &ADDO) )
		{
		CMoveRSP :: Write ( PDU, DCO, 0xc004, 0, 0, 0, 0, NULL );
		return ( TRUE );
		}

	NewPDU.SetApplicationContext ( AppUID );
	NewPDU.SetLocalAddress ( MyACR );
	NewPDU.SetRemoteAddress ( ACRNema );
	
	// Add all the Abstract Syntaxs we need

	Index = 0;
	while ( Index < ADDO.GetSize() )
		{
		vr = ADDO.Get ( Index ) -> GetVR(0x0008, 0x0016);
		if(!vr)
			{
			delete ADDO.Get ( Index );
			ADDO.RemoveAt ( Index );
			}
		else
			{
			SetUID ( iUID, vr );
			NewPDU.AddAbstractSyntax ( iUID );
			++Index;
			}
		}

	if (!NewPDU.Connect (IP, lPort))
		{
		CMoveRSP :: Write ( PDU, DCO, 0xc005 , 0, 0, 0, 0, NULL );
		return ( TRUE );
		}


	Index = 0;
	Failed = 0;
	while ( Index < ADDO.GetSize() )
		{
		vr = ADDO.Get ( Index ) -> GetVR(0x0008, 0x0016);
		SetUID ( iUID, vr );
		if ( !NewPDU.GetPresentationContextID(iUID) )
			{
			++Failed;
			// Remote end did not accept this UID
			}
		else
			{
			if ( !RetrieveOn (ADDO.Get(Index), &iDDO, &SStorage))
				++Failed;
			else
				{
				if(!SStorage->Write(&NewPDU, iDDO))
					{
					//++Failed;
					// Remote end should accept this image.  if it did
					// not, then just bail out.  Probably means the
					// TCP/IP link has been dropped.
					Failed += (ADDO.GetSize() - Index);
					status = 0xfe00;					// mvh 20020429: return cancel status
					break;
					}

				delete iDDO;
				}
			}
			
		int remaining = ADDO.GetSize() - Index - 1;
		if (remaining>65535) remaining = 65535;
		int done  = Index + 1;
		if (done>65535) done = 65535;
		
		CMoveRSP :: Write ( PDU,
							DCO,
							0xff00, (UINT16) remaining,
							(UINT16) done, Failed, 0,		// mvh 20020429: added +1
							ADDO.Get ( Index ));
		delete ADDO.Get ( Index );
		++Index;
		}

	CMoveRSP :: Write ( PDU, DCO, status, 0, (UINT16) Index, Failed, 0, NULL );		// mvh 20020429: replaced 0 by status

	// Incase we broke out from above..
	while ( Index < ADDO.GetSize () )
		{
		delete ADDO.Get(Index);
		++Index;
		}
	
	return ( TRUE );
	}

BOOL	StandardRetrieve	::	Write (
	PDU_Service		*PDU,
	DICOMDataObject	*DDO,
	BYTE			*ACRNema )
	{
	DICOMCommandObject	*DCO;
	DICOMDataObject		*RDDO;

	if ( ! PDU )
		return ( FALSE );

	if ( ! CMoveRQ :: Write ( PDU, DDO, ACRNema ) )
		return ( FALSE );

	CallBack ( NULL, DDO );

	DCO = new DICOMCommandObject;

	while ( PDU->Read ( DCO ) )
		{
		RDDO = new DICOMDataObject;

		if (! (CMoveRSP :: Read ( DCO, PDU, RDDO) ) )
			{
			return ( FALSE );
			}
		if ( DCO->GetUINT16(0x0000, 0x0800) == 0x0101)
			{
			CallBack ( DCO, NULL );
			delete RDDO;
			if ( DCO->GetUINT16(0x0000, 0x0900) != 0x0000)
				{
				VR *vr;
				while ((vr = DCO->Pop()))
					{
					//DumpVR(vr);
					delete vr;
					}
				delete DCO;
				return ( FALSE );
				}
			delete DCO;
			return ( TRUE );
			}
		CallBack ( DCO, RDDO );
		delete RDDO;
		delete DCO;
		DCO = new DICOMCommandObject;
		}

	delete DCO;
	return ( FALSE );
	}

BOOL	PatientRootQuery	::	GetUID ( UID &uid )
	{
	uid.Set ( "1.2.840.10008.5.1.4.1.2.1.1" );
	return ( TRUE );
	}

BOOL	PatientRootRetrieve	::	GetUID ( UID &uid )
	{
	uid.Set ( "1.2.840.10008.5.1.4.1.2.1.2" );
	return ( TRUE );
	}

BOOL	StudyRootQuery	::	GetUID ( UID &uid )
	{
	uid.Set ( "1.2.840.10008.5.1.4.1.2.2.1" );
	return ( TRUE );
	}

BOOL	StudyRootRetrieve	::	GetUID ( UID &uid )
	{
	uid.Set ( "1.2.840.10008.5.1.4.1.2.2.2" );
	return ( TRUE );
	}

BOOL	PatientStudyOnlyQuery	::	GetUID ( UID &uid )
	{
	uid.Set ( "1.2.840.10008.5.1.4.1.2.3.1" );
	return ( TRUE );
	}

BOOL	PatientStudyOnlyRetrieve	::	GetUID ( UID &uid )
	{
	uid.Set ( "1.2.840.10008.5.1.4.1.2.3.2" );
	return ( TRUE );
	}

BOOL	ModalityWorkListQuery	::	GetUID ( UID &uid )
	{
	uid.Set ( "1.2.840.10008.5.1.4.31" );
	return ( TRUE );
	}
