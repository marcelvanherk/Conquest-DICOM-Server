/*
20020814	ljz	Added some comments and block-out mvh's debug code
20020816	ljz	Fix in 'AbstractPrintManagementMeta::Write'
20130411	bcb	Fix error C4355 (also a leak).
20130429        lsp     Removed unused variables
20140520        mgw     Unix compatibility
20160221	mvh	Imported change 20130411
*/


#	include	"dicom.hpp"
#	include	"printsop.hpp"
#include "errdescr.hpp"
#ifdef unix
#include <unistd.h>	// provides sleep() prototype
#endif

BOOL
IdleProc (BOOL	*Lock)
	{
	// While locked, sleep for 1 second intervals waiting for another
	// thread to (hopefully) unlock the data.
	while ( (*Lock) )
		{
#ifdef	WINDOWS
		Sleep ( 1000 );
#else
		sleep ( 1 );
#endif
		}
	return ( TRUE );
	}

BOOL
BasicFilmSession	::	uGetUID (
	UID		&theUID )
	{
	theUID.Set("1.2.840.10008.5.1.1.1");
	return ( TRUE );
	}

BOOL
BasicFilmBox	::	uGetUID (
	UID		&theUID )
	{
	theUID.Set("1.2.840.10008.5.1.1.2");
	return ( TRUE );
	}

BOOL
BasicGrayscaleImageBox	::	uGetUID (
	UID		&theUID )
	{
	theUID.Set("1.2.840.10008.5.1.1.4");
	return ( TRUE );
	}

BOOL
BasicColorImageBox	::	uGetUID (
	UID		&theUID )
	{
	theUID.Set("1.2.840.10008.5.1.4.1");
	return ( TRUE );
	}

BOOL
ReferencedImageBox	::	uGetUID (
	UID		&theUID )
	{
	theUID.Set("1.2.840.10008.5.1.4.2");
	return ( TRUE );
	}

BOOL
VOILUTBox	::	uGetUID (
	UID		&theUID )
	{
	theUID.Set("1.2.840.10008.5.1.1.22");
	return ( TRUE );
	}

BOOL
BasicAnnotationBox	::	uGetUID (
	UID		&theUID )
	{
	theUID.Set("1.2.840.10008.5.1.1.15");
	return ( TRUE );
	}

BOOL
ImageOverlayBox	::	uGetUID (
	UID		&theUID )
	{
	theUID.Set("1.2.840.10008.5.1.1.24");
	return ( TRUE );
	}

BOOL
BasicPrinter	::	uGetUID (
	UID		&theUID )
	{
	theUID.Set("1.2.840.10008.5.1.1.16");
	return ( TRUE );
	}

BOOL
BasicPrintJob	::	uGetUID (
	UID		&theUID )
	{
	theUID.Set("1.2.840.10008.5.1.1.14");
	return ( TRUE );
	}

#if 0
BOOL
MetaPrinter	::	GetSOPInstanceUID (
	UID	&theUID )
	{
	theUID = itsSOPInstanceUID;
	return ( TRUE );
	}

BOOL
MetaPrinter	::	Get (
	UID	*theUid, DICOMDataObject *theDDO)
	{
	return ( TRUE );
	}

BOOL
MetaPrinter	::	GetStatus (
	UID	*theUid, DICOMDataObject *theDDO)
	{
	return ( TRUE );
	}

BOOL
MetaPrinter	::	SetSOPInstanceUID (
	UID	&theUID )
	{
	itsSOPInstanceUID = theUID;
	return ( TRUE );
	}

BOOL
MetaPrintJob	::	GetSOPInstanceUID (
	UID	&theUID )
	{
	theUID = itsSOPInstanceUID;
	return ( TRUE );
	}

BOOL
MetaPrintJob	::	SetSOPInstanceUID (
	UID	&theUID )
	{
	itsSOPInstanceUID = theUID;
	return ( TRUE );
	}

BOOL
MetaPrintJob	::	Get (
	UID	*theUid, DICOMDataObject *theDDO)
	{
	return ( TRUE );
	}

BOOL
MetaPrintJob	::	GetStatus (
	UID	*theUid, DICOMDataObject *theDDO)
	{
	return ( TRUE );
	}
#endif
		
/********************** Basic Film Session *********************/
BOOL
StandardFilm	::	ReadCreate (
		PDU_Service			*PDU,
		DICOMCommandObject	*DCO,
		DICOMDataObject		*DDO )
		{
		VR		*aVR;
		UID		aMyUID, aUID;
		
		GetUID(aMyUID);
		
		if ( ! PDU )
			return ( FALSE );
			
		if ( ! DCO )
			return ( FALSE );
		
		if ( ! DDO )
			return ( FALSE );
		
		aVR = DCO->GetVR(0x0000, 0x0002 );
		if ( ! aVR )
			return ( FALSE );
		
		if ( ! SetUID(aUID, aVR) )
			return ( FALSE );
		
		if ( aMyUID != aUID )
			return ( FALSE );
	
		aVR = DCO->GetVR(0x0000, 0x1000);
		SetUID(aUID, aVR);	
		if ( NCreateRQ :: Read ( DCO, PDU, DDO ) )
			return ( NCreateRSP :: Write ( PDU, DCO, &aUID, 0, NULL ) );
		
		return ( FALSE );
		}

// StandardFilm::WriteCreate() -- SCU method called to send a
// standard film object create request to SCP
// A StandardFilm is the base class for the BasicFilmSession and BasicFilmBox

BOOL
StandardFilm	::	WriteCreate (
	PDU_Service		*thePDU,
	UID				*theAffectedSOPInstanceUID,
	DICOMDataObject	*theDDO,
	AbstractPrintManagementMeta &theManager )
	{

	ErrorDescription anError(__FILE__, __LINE__, "StandardFilm", "WriteCreate");
		
	
	// if caller supplies pointer to an empty UID,
	// we don't supply one to NCreateRQ but will update
	// caller's supplied UID with value returned in NCreateRSP

	UID *aUID = NULL;

	if(theAffectedSOPInstanceUID && theAffectedSOPInstanceUID->GetSize() > 0)
	{
		aUID = theAffectedSOPInstanceUID;
	}

	// issue the NCreateRQ

	if ( ! NCreateRQ :: Write ( thePDU, theDDO, aUID) )
		{
		anError.SetLine(__LINE__);
		anError.SetErrorType(ErrorDescription::DIMSENWriteError);
		anError.SetOperation(ErrorDescription::NCreateRQWrite);
		anError.SetMessage("NCreateRQ::Write() failed");
		theManager.HandleError(anError);
		return ( FALSE );
		}
	
	BOOL aLookingForResponseFlag = TRUE;

	// look for a response or a possible asynchronous event report

	do
	{

	if ( theDDO )
		theDDO->Reset();	
		
	// attempt to read the response

	DICOMCommandObject		aDCO;

	if ( ! thePDU->Read ( &aDCO ) )
		{
		anError.SetLine(__LINE__);
		anError.SetErrorType(ErrorDescription::PDUReadError);
		anError.SetOperation(ErrorDescription::PDUReadOperation);
		anError.SetMessage("PDU::Read() failed");
		theManager.HandleError(anError);
		return ( FALSE );
		}

	// process the response
	
	if ( ! NCreateRSP :: Read ( &aDCO, thePDU, theDDO ) )
		{
		// not what we expected...
		// have manager's printer object attempt to receive a
		// possible printer status event or print job status event

		if ( ! theManager.ReadEventReport( thePDU, &aDCO ) )
			{
			// wasn't an event report either -- give up
			anError.SetLine(__LINE__);
			anError.SetErrorType(ErrorDescription::UnexpectedDIMSENResponse);
			anError.SetOperation(ErrorDescription::NCreateRSPRead);
			anError.SetResponseCommand(&aDCO);
			anError.SetResponseDataset(theDDO);
			anError.SetMessage("NCreateRSP::Read() received unexpected response");
			theManager.HandleError(anError);
			return ( FALSE );
			}
		}
		else
		{
		// found a response
		UINT16 aStatus = aDCO.GetUINT16(0x0000, 0x0900);

		if (aStatus != 0)
			{
			// failure or dubious warning from SCP
			anError.SetLine(__LINE__);
			anError.SetOperation(ErrorDescription::NCreateRSPRead);
			anError.SetDICOMStatus(aStatus);
			anError.SetResponseCommand(&aDCO);
			anError.SetResponseDataset(theDDO);
			switch(aStatus)
				{
				default:
					anError.SetErrorType(
						ErrorDescription::UnexpectedDIMSENStatus);
					anError.SetMessage("Unknown Status Returned");
				break;
				case 0xb600:
					anError.SetErrorType(ErrorDescription::DIMSENWarningStatus);
					anError.SetMessage("Memory Allocation Not Supported");
				break;
				case 0x0107:
					anError.SetErrorType(ErrorDescription::DIMSENWarningStatus);
					anError.SetMessage(
						"Attribute(s) Not Recognized in Context of this SOP Class");
				break;
				case 0x0106:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Invalid Attribute Value or Memory Requested Unavailable");
				break;
				case 0x0110:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Cannot decode DIMSE Attribute");
				break;
				case 0x0111:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Specified SOP Instance UID already in use");
				break;
				case 0x0117:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Invalid SOP Instance UID syntax");
				break;
				case 0x0120:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Mandatory Attribute Missing");
				break;
				case 0x0121:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Mandatory Attribute was given, but had no value");
				break;
				case 0x0213:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Resource Limitation: Can't Create Session or Film");
				break;
				}

			// see if the user wants to continue anyway

			if(!theManager.HandleError(anError))
				{
				return FALSE;
				}
			}

		// update caller's UID from value in response received

		if(!aUID && theAffectedSOPInstanceUID)
			{

			// get Affected SOP Instance UID from DCO

			VR *aVR = aDCO.GetVR(0x0000, 0x1000);
			if(aVR)
				{
        		SetUID(*theAffectedSOPInstanceUID, aVR);
				}
			}
		aLookingForResponseFlag = FALSE;	// found the response
		}
	} while ( aLookingForResponseFlag );

	return ( TRUE );
	}
	
BOOL
StandardFilm	::	ReadSet (
		PDU_Service			*PDU,
		DICOMCommandObject	*DCO,
		DICOMDataObject		*DDO )
		{
		VR		*aVR;
		UID		aMyUID, aUID;
		
		GetUID(aMyUID);
		
		if ( ! PDU )
			return ( FALSE );
			
		if ( ! DCO )
			return ( FALSE );
		
		if ( ! DDO )
			return ( FALSE );
		DDO->Reset();	
		aVR = DCO->GetVR(0x0000, 0x0002 );
		if ( ! aVR )
			return ( FALSE );
		
		if ( ! SetUID(aUID, aVR) )
			return ( FALSE );
		
		if ( aMyUID != aUID )
			return ( FALSE );
	
		aVR = DCO->GetVR(0x0000, 0x1001);
		SetUID(aUID, aVR);	
		if ( NSetRQ :: Read ( DCO, PDU, DDO ) )
			return ( NSetRSP :: Write ( PDU, DCO, &aUID, 0, NULL ) );
		
		return ( FALSE );
		}

// StandardFilm::WriteSet() -- SCU method called to
// send a set request for a standard film object to SCP
// to "set" an ImageBox in a FilmBox, for example

BOOL
StandardFilm	::	WriteSet (
	PDU_Service		*thePDU,
	UID				*theAffectedSOPInstanceUID,
	DICOMDataObject	*theDDO,
	AbstractPrintManagementMeta &theManager )
	{
	ErrorDescription anError(__FILE__, __LINE__, "StandardFilm", "WriteSet");

	// issue the NSetRQ

	if ( ! NSetRQ :: Write ( thePDU, theDDO, theAffectedSOPInstanceUID) )
		{
		anError.SetLine(__LINE__);
		anError.SetErrorType(ErrorDescription::DIMSENWriteError);
		anError.SetMessage("NSetRQ::Write() failed");
		anError.SetOperation(ErrorDescription::NSetRQWrite);
		theManager.HandleError(anError);
		return ( FALSE );
		}

	BOOL aLookingForResponseFlag = TRUE;

	// look for a response or a possible asynchronous event report

	do
	{

	if ( theDDO )
		theDDO->Reset();	
		
	// attempt to read the response

	DICOMCommandObject		aDCO;

	if ( ! thePDU->Read ( &aDCO ) )
		{
		anError.SetLine(__LINE__);
		anError.SetErrorType(ErrorDescription::PDUReadError);
		anError.SetOperation(ErrorDescription::PDUReadOperation);
		anError.SetMessage("PDU::Read() failed");
		theManager.HandleError(anError);
		return ( FALSE );
		}
	// process the response
	
	if ( ! NSetRSP :: Read ( &aDCO, thePDU, theDDO ) )
		{
		// not what we expected...
		// have manager's printer object attempt to receive a
		// possible printer status event or print job status event

		if ( ! theManager.ReadEventReport( thePDU, &aDCO ) )
			{
			// wasn't an event report either -- give up
			anError.SetLine(__LINE__);
			anError.SetErrorType(ErrorDescription::UnexpectedDIMSENResponse);
			anError.SetOperation(ErrorDescription::NSetRSPRead);
			anError.SetResponseCommand(&aDCO);
			anError.SetResponseDataset(theDDO);
			anError.SetMessage("NSetRSP::Read() received unexpected response");
			theManager.HandleError(anError);
			return ( FALSE );
			}
		}
		else
		{
		// found a response
		UINT16 aStatus = aDCO.GetUINT16(0x0000, 0x0900);

		if (aStatus != 0)
			{
			// failure or dubious warning from SCP
			anError.SetLine(__LINE__);
			anError.SetOperation(ErrorDescription::NSetRSPRead);
			anError.SetDICOMStatus(aStatus);
			anError.SetResponseCommand(&aDCO);
			anError.SetResponseDataset(theDDO);
			switch(aStatus)
				{
				default:
					anError.SetErrorType(
						ErrorDescription::UnexpectedDIMSENStatus);
					anError.SetMessage("Unknown Status Returned");
				break;
				case 0xb600:
					anError.SetErrorType(ErrorDescription::DIMSENWarningStatus);
					anError.SetMessage("Memory Allocation Not Supported");
				break;
				case 0x0107:
					anError.SetErrorType(ErrorDescription::DIMSENWarningStatus);
					anError.SetMessage(
						"Attribute(s) Not Recognized in Context of this SOP Class");
				break;
				case 0x0106:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Invalid Attribute Value or Memory Requested Unavailable");
				break;
				case 0x0110:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Cannot decode DIMSE Attribute");
				break;
				case 0x0112:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Specified SOP Instance does not exist");
				break;
				case 0x0117:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Invalid SOP Instance UID syntax");
				break;
				case 0x0120:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Mandatory Attribute Missing");
				break;
				case 0x0121:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Mandatory Attribute was given, but had no value");
				break;
				}

			// see if the user wants to continue anyway

			if(!theManager.HandleError(anError))
				{
				return FALSE;
				}
			}


		aLookingForResponseFlag = FALSE;	// found the response
		}
	} while ( aLookingForResponseFlag );
	return ( TRUE );
	}
	
BOOL
StandardFilm	::	ReadDelete (
		PDU_Service			*PDU,
		DICOMCommandObject	*DCO)
		{
		VR		*aVR;
		UID		aMyUID, aUID;
		
		GetUID(aMyUID);
		
		if ( ! PDU )
			return ( FALSE );
			
		if ( ! DCO )
			return ( FALSE );
		
		aVR = DCO->GetVR(0x0000, 0x0002 );
		if ( ! aVR )
			return ( FALSE );
		
		if ( ! SetUID(aUID, aVR) )
			return ( FALSE );
		
		if ( aMyUID != aUID )
			return ( FALSE );
	
		aVR = DCO->GetVR(0x0000, 0x1001);
		SetUID(aUID, aVR);	
		if ( NDeleteRQ :: Read ( DCO, PDU ) )
			return ( NDeleteRSP :: Write ( PDU, DCO, &aUID, 0 ) );
		
		return ( FALSE );
		}

// StandardFilm::WriteDelete() -- SCU method called to send
// a delete standard film object request to SCP

BOOL
StandardFilm	::	WriteDelete (
	PDU_Service		*thePDU,
	UID				*theAffectedSOPInstanceUID,
	AbstractPrintManagementMeta &theManager)
	{

	if ( ! NDeleteRQ :: Write ( thePDU, theAffectedSOPInstanceUID) )
		return ( FALSE );
	
	BOOL aLookingForResponseFlag = TRUE;

	// look for a response or a possible asynchronous event report

	do
	{

	// attempt to read the response

	DICOMCommandObject		aDCO;

	if ( ! thePDU->Read ( &aDCO ) )
		return ( FALSE );

	// process the response
	
	if ( ! NDeleteRSP :: Read ( &aDCO, thePDU ) )
		{
		// not what we expected...
		// have manager's printer object attempt to receive a
		// possible printer status event or print job status event

		if ( ! theManager.ReadEventReport( thePDU, &aDCO ) )
			{
			// wasn't an event report either -- give up
			return ( FALSE );
			}
		}
		else
		{
		// found a response
		// ignore status -- who cares about double delete, for example
		aLookingForResponseFlag = FALSE;	// found the response
		}
	} while ( aLookingForResponseFlag );
	
	return ( TRUE );
	}
	
BOOL
StandardFilm	::	ReadPrint (
		PDU_Service			*PDU,
		DICOMCommandObject	*DCO,
		DICOMDataObject		*DDO )
		{
		VR		*aVR;
		UID		aMyUID, aUID;
		
		GetUID(aMyUID);
		
		if ( ! PDU )
			return ( FALSE );
			
		if ( ! DCO )
			return ( FALSE );
		
		if ( ! DDO )
			return ( FALSE );
		
		aVR = DCO->GetVR(0x0000, 0x0002 );
		if ( ! aVR )
			return ( FALSE );
		
		if ( ! SetUID(aUID, aVR) )
			return ( FALSE );
		
		if ( aMyUID != aUID )
			return ( FALSE );
	
		aVR = DCO->GetVR(0x0000, 0x1001);
		SetUID(aUID, aVR);	
		if ( NActionRQ :: Read ( DCO, PDU, DDO ) )
			return ( NActionRSP :: Write ( PDU, DCO, &aUID, 1, 0, NULL ) );
		
		return ( FALSE );
		}

// StandardFilm::WritePrint() -- SCU method called to send
// a print standard film object action request to SCP

BOOL
StandardFilm	::	WritePrint (
	PDU_Service		*thePDU,
	UID				*theAffectedSOPInstanceUID,
	AbstractPrintManagementMeta &theManager )
	{

	ErrorDescription anError(__FILE__, __LINE__, "StandardFilm", "WritePrint");

	// NActionRQ sends NO dataset (at least the way we use it here)

	if ( ! NActionRQ :: Write ( thePDU, NULL, theAffectedSOPInstanceUID, 1) )
		{
		anError.SetLine(__LINE__);
		anError.SetErrorType(ErrorDescription::DIMSENWriteError);
		anError.SetOperation(ErrorDescription::NActionRQWrite);
		anError.SetMessage("NActionRQ::Write() failed");
		theManager.HandleError(anError);
		return ( FALSE );
		}
	BOOL aLookingForResponseFlag = TRUE;

	do
	{

	// attempt to read the response

	DICOMCommandObject		aDCO;

	if ( ! thePDU->Read ( &aDCO ) )
		{
		anError.SetLine(__LINE__);
		anError.SetErrorType(ErrorDescription::PDUReadError);
		anError.SetOperation(ErrorDescription::PDUReadOperation);
		anError.SetMessage("PDU::Read() failed");
		theManager.HandleError(anError);
		return ( FALSE );
		}
	// the response can potentially contain a print job reference
	// so instantiate an AbstractPrintJob object to contain it

	AbstractPrintJob *aPrintJob = new AbstractPrintJob(theManager);

	// process the response
	
	if ( ! NActionRSP :: Read ( &aDCO, thePDU, aPrintJob ) )
		{
		// not what we expected...
		// have manager's printer object attempt to receive a
		// possible printer status event or print job status event

		if ( ! theManager.ReadEventReport( thePDU, &aDCO ) )
			{
			anError.SetLine(__LINE__);
			// wasn't an event report either -- give up
			delete aPrintJob;
			// wasn't an event report either -- give up
			anError.SetErrorType(ErrorDescription::UnexpectedDIMSENResponse);
			anError.SetOperation(ErrorDescription::NActionRSPRead);
			anError.SetResponseCommand(&aDCO);
			anError.SetResponseDataset(aPrintJob);
			anError.SetMessage("NActionRSP::Read() received unexpected response");
			theManager.HandleError(anError);
			return FALSE;
			}
		}
		else
		{
		// found a response
		// if the SCP supports Print Job SOP, a print job has been created
		// and the response dataset contains information on the print job
		// attempt to setup the AbstractPrintJob from the received dataset

		UINT16 aStatus = aDCO.GetUINT16(0x0000, 0x0900);

		if (aStatus != 0)
			{
			// failure or empty page warning from SCP
			anError.SetLine(__LINE__);
			anError.SetOperation(ErrorDescription::NActionRSPRead);
			anError.SetDICOMStatus(aStatus);
			anError.SetResponseCommand(&aDCO);
			anError.SetResponseDataset(aPrintJob);
			switch(aStatus)
				{
				default:
					anError.SetErrorType(
						ErrorDescription::UnexpectedDIMSENStatus);
					anError.SetMessage("Unknown Status Returned");
				break;
				case 0xb600:
					anError.SetErrorType(ErrorDescription::DIMSENWarningStatus);
					anError.SetMessage("Memory Allocation Not Supported");
				break;
				case 0xb601:
					anError.SetErrorType(ErrorDescription::DIMSENWarningStatus);
					anError.SetMessage(
						"Film Session Printing (Collation) Not Supported");
				break;
				case 0xb602:
				case 0xb603:
					anError.SetErrorType(ErrorDescription::DIMSENWarningStatus);
					anError.SetMessage(
						"Film Session Contains No Images (Empty Page)");
				break;
				case 0x0106:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage("Invalid Attribute");
				break;
				case 0x0213:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Requested Resource Temporarily Unavailable");
				break;
				case 0xc600:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Film Session Contains No Film Instances");
				break;
				case 0xc601:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage("Print Queue Full");
				break;
				case 0xc603:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage("Specified Magnification Produces Image Size Larger Than Image Box Size");
				break;
				case 0xc604:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage("Multiple Images Assigned to Image Position (Image Collision)");
				break;
				}

			// see if the user wants to continue anyway

			if(!theManager.HandleError(anError))
				{
				return FALSE;
				}
			}

		if ( ! aPrintJob->ProtoGen() )
			{
			// received dataset contained no print job reference
			// so get rid of AbstractPrintJob
			delete aPrintJob;
			}
		else
			{
			// add the new print job to manager's list of print jobs
			theManager.Add(aPrintJob);
			}

		aLookingForResponseFlag = FALSE;	// found the response
		}
	} while ( aLookingForResponseFlag );
	
	return ( TRUE );
	}

BOOL
StandardFilm	::	Read (
	PDU_Service			*PDU,
	DICOMCommandObject	*DCO,
	DICOMDataObject		*DDO )
	{
	if ( ReadCreate ( PDU, DCO, DDO ) )
		return ( TRUE );
	if ( ReadSet ( PDU, DCO, DDO ) )
		return ( TRUE );
	if ( ReadDelete ( PDU, DCO ) )
		return ( TRUE );
	if ( ReadPrint ( PDU, DCO, DDO ) )
		return ( TRUE );
	
	return ( FALSE );
	}
	
/******************** StandardBox *********************/
//
// StandardBox for boxes that are created automatically
// by creation of a higher level object
// thus, StandardBox does not represent a BasicFilmBox
//

BOOL
StandardBox	::	Read (
		PDU_Service			*PDU,
		DICOMCommandObject	*DCO,
		DICOMDataObject		*DDO )
		{
		VR		*aVR;
		UID		aMyUID, aUID;
		
		GetUID(aMyUID);
		
		if ( ! PDU )
			return ( FALSE );
			
		if ( ! DCO )
			return ( FALSE );
		
		if ( ! DDO )
			return ( FALSE );
	
		DDO->Reset();	
		aVR = DCO->GetVR(0x0000, 0x0002 );
		if ( ! aVR )
			return ( FALSE );
		
		if ( ! SetUID(aUID, aVR) )
			return ( FALSE );
		
		if ( aMyUID != aUID )
			return ( FALSE );
	
		aVR = DCO->GetVR(0x0000, 0x1001);
		SetUID(aUID, aVR);	
		if ( NSetRQ :: Read ( DCO, PDU, DDO ) )
			return ( NSetRSP :: Write ( PDU, DCO, &aUID, 0, NULL ) );
		
		return ( FALSE );
		}

// StandardBox::Write() -- SCU method called to send
// a set standard box object request to SCP
// sends  BasicGrayscaleImageBox,
//        BasicColorImageBox, ReferencedImageBox, and
//        BasicAnnotationBox to print SCP

BOOL
StandardBox	::	Write (
	PDU_Service		*thePDU,
	UID				*theAffectedSOPInstanceUID,
	DICOMDataObject	*theDDO,
	AbstractPrintManagementMeta &theManager )
	{
	ErrorDescription anError(__FILE__, __LINE__, "StandardBox", "Write");

	if ( ! NSetRQ :: Write ( thePDU, theDDO, theAffectedSOPInstanceUID) )
		{
		anError.SetLine(__LINE__);
		anError.SetErrorType(ErrorDescription::DIMSENWriteError);
		anError.SetOperation(ErrorDescription::NSetRQWrite);
		anError.SetMessage("NSetRQ::Write() failed");
		theManager.HandleError(anError);
		return ( FALSE );
		}
	
	BOOL aLookingForResponseFlag = TRUE;

	do
	{

	if ( theDDO )
		theDDO->Reset ();

	// attempt to read the response

	DICOMCommandObject		aDCO;

	if ( ! thePDU->Read ( &aDCO ) )

		{
		anError.SetLine(__LINE__);
		anError.SetErrorType(ErrorDescription::PDUReadError);
		anError.SetOperation(ErrorDescription::PDUReadOperation);
		anError.SetMessage("PDU::Read() failed");
		theManager.HandleError(anError);
		return ( FALSE );
		}

	// process the response
	
	if ( ! NSetRSP :: Read ( &aDCO, thePDU, theDDO ) )
		{
		// not what we expected...
		// have manager's printer object attempt to receive a
		// possible printer status event or print job status event

		if ( ! theManager.ReadEventReport( thePDU, &aDCO ) )
			{
			// wasn't an event report either -- give up
			anError.SetLine(__LINE__);
			anError.SetErrorType(ErrorDescription::UnexpectedDIMSENResponse);
			anError.SetOperation(ErrorDescription::NSetRSPRead);
			anError.SetMessage("NSetRSP::Read received unexpected response");
			anError.SetResponseCommand(&aDCO);
			anError.SetResponseDataset(theDDO);
			theManager.HandleError(anError);
			return ( FALSE );
			}
		}
		else
		{
		UINT16 aStatus = aDCO.GetUINT16(0x0000, 0x0900);

		if (aStatus != 0)
			{
			// failure or dubious warning from SCP
			anError.SetLine(__LINE__);
			anError.SetOperation(ErrorDescription::NSetRSPRead);
			anError.SetDICOMStatus(aStatus);
			anError.SetResponseCommand(&aDCO);
			anError.SetResponseDataset(theDDO);
			switch(aStatus)
				{
				default:
					anError.SetErrorType(
						ErrorDescription::UnexpectedDIMSENStatus);
					anError.SetMessage("Unknown Status Returned");
				break;
				case 0x0106:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Invalid Attribute or Memory Requested Unavailable");
				break;
				case 0x0213:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Requested Memory Allocation Temporarily Unavailable");
				break;
				case 0xc605:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"Insufficient memory in printer to store image");
				break;
				case 0xc606:
					anError.SetErrorType(ErrorDescription::DIMSENErrorStatus);
					anError.SetMessage(
						"More than one VOI LUT Box contained in the image");
				break;
				}

			// see if the user wants to continue anyway

			if(!theManager.HandleError(anError))
				{
				return FALSE;
				}
			}

		// found a response
		aLookingForResponseFlag = FALSE;	// found the response
		}
	} while ( aLookingForResponseFlag );
	
	return ( TRUE );
	}

/****************** Standard Advanced Box ***************/

BOOL
StandardAdvancedBox	::	ReadCreate (
		PDU_Service			*thePDU,
		DICOMCommandObject	*theDCO,
		DICOMDataObject		*theDDO )
		{
		VR		*aVR;
		UID		aMyUID, aUID;
		
		GetUID(aMyUID);
		
		if ( ! thePDU )
			return ( FALSE );
			
		if ( ! theDCO )
			return ( FALSE );
		
		if ( ! theDDO )
			return ( FALSE );
		
		aVR = theDCO->GetVR(0x0000, 0x0002 );
		if ( ! aVR )
			return ( FALSE );
		
		if ( ! SetUID(aUID, aVR) )
			return ( FALSE );
		
		if ( aMyUID != aUID )
			return ( FALSE );
	
		aVR = theDCO->GetVR(0x0000, 0x1000);
		SetUID(aUID, aVR);	
		if ( NCreateRQ :: Read ( theDCO, thePDU, theDDO ) )
			return ( NCreateRSP :: Write ( thePDU, theDCO, &aUID, 0, NULL ) );
		
		return ( FALSE );
		}

// StandardAdvancedBox::WriteCreate() -- SCU method called to
// send an advanced box to SCP
// sends VOILUTBox and ImageOverlayBox to print SCP

BOOL
StandardAdvancedBox	::	WriteCreate (
	PDU_Service		*thePDU,
	UID				*theAffectedSOPInstanceUID,
	DICOMDataObject	*theDDO,
	AbstractPrintManagementMeta &theManager )
	{

	if(!thePDU)
		{
		return FALSE;
		}

	// if caller supplies pointer to an empty UID,
	// we don't supply one to NCreateRQ but will update
	// caller's supplied UID with value returned in NCreateRSP

	UID *aUID = NULL;
	if(theAffectedSOPInstanceUID && theAffectedSOPInstanceUID->GetSize() > 0)
	{
		aUID = theAffectedSOPInstanceUID;
	}

	// issue the NCreateRQ

	if ( ! NCreateRQ :: Write ( thePDU, theDDO, aUID) )
		return ( FALSE );

	// look for a response or a possible asynchronous event report

	BOOL aLookingForResponseFlag = TRUE;

	do
	{

	if ( theDDO )
		theDDO->Reset();	
		
	// attempt to read the response

	DICOMCommandObject aDCO;

	if ( ! thePDU->Read ( &aDCO ) )
		return ( FALSE );

	// process the response
	
	if ( ! NCreateRSP :: Read ( &aDCO, thePDU, theDDO ) )
		{
		// not what we expected...
		// have manager's printer object attempt to receive a
		// possible printer status event or print job status event

		if ( ! theManager.ReadEventReport( thePDU, &aDCO ) )
			{
			// wasn't an event report either -- give up
			return ( FALSE );
			}
		}
		else
		{
		// found a response

		UINT16 aStatus = aDCO.GetUINT16(0x0000, 0x0900);

		if (aStatus != 0)
			{
			// failure or dubious warning from SCP
			return FALSE;
			}

		if(!aUID && theAffectedSOPInstanceUID)
			{

			// get Affected SOP Instance UID from DCO

			VR *aVR = aDCO.GetVR(0x0000, 0x1000);
			if(aVR)
				{
        		SetUID(*theAffectedSOPInstanceUID, aVR);
				}
			}
		aLookingForResponseFlag = FALSE;	// found the response
		}
	} while ( aLookingForResponseFlag );

	return ( TRUE );
	}
	
BOOL
StandardAdvancedBox	::	ReadSet (
		PDU_Service			*thePDU,
		DICOMCommandObject	*theDCO,
		DICOMDataObject		*theDDO )
		{
		VR		*aVR;
		UID		aMyUID, aUID;
		
		GetUID(aMyUID);
		
		if ( ! thePDU )
			return ( FALSE );
			
		if ( ! theDCO )
			return ( FALSE );
		
		if ( ! theDDO )
			return ( FALSE );
		theDDO->Reset();	
		
		aVR = theDCO->GetVR(0x0000, 0x0002 );
		if ( ! aVR )
			return ( FALSE );
		
		if ( ! SetUID(aUID, aVR) )
			return ( FALSE );
		
		if ( aMyUID != aUID )
			return ( FALSE );
	
		aVR = theDCO->GetVR(0x0000, 0x1001);
		SetUID(aUID, aVR);	
		if ( NSetRQ :: Read ( theDCO, thePDU, theDDO ) )
			return ( NSetRSP :: Write ( thePDU, theDCO, &aUID, 0, NULL ) );
		
		return ( FALSE );
		}

// StandardAdvancedBox::WriteSet() -- SCU method called to send
// a set standard advanced box object request to SCP
// sends update of VOILUTBox and ImageOverlayBox to print SCP

BOOL
StandardAdvancedBox	::	WriteSet (
	PDU_Service		*thePDU,
	UID				*theAffectedSOPInstanceUID,
	DICOMDataObject	*theDDO,
	AbstractPrintManagementMeta &theManager )
	{
	if ( ! NSetRQ :: Write ( thePDU, theDDO, theAffectedSOPInstanceUID) )
		return ( FALSE );
	
	// look for a response or a possible asynchronous event report

	BOOL aLookingForResponseFlag = TRUE;

	do
	{

	if ( theDDO )
		theDDO->Reset();	
		
	// attempt to read the response

	DICOMCommandObject aDCO;

	if ( ! thePDU->Read ( &aDCO ) )
		return ( FALSE );

	// process the response
	
	if ( ! NSetRSP :: Read ( &aDCO, thePDU, theDDO ) )
		{
		// not what we expected...
		// have manager's printer object attempt to receive a
		// possible printer status event or print job status event

		if ( ! theManager.ReadEventReport( thePDU, &aDCO ) )
			{
			// wasn't an event report either -- give up
			return ( FALSE );
			}
		}
		else
		{
		// found a response
		UINT16 aStatus = aDCO.GetUINT16(0x0000, 0x0900);

		if (aStatus != 0)
			{
			// failure or dubious warning from SCP
			return FALSE;
			}

		aLookingForResponseFlag = FALSE;	// found the response
		}
	} while ( aLookingForResponseFlag );
	
	return ( TRUE );
	}
	
BOOL
StandardAdvancedBox	::	ReadDelete (
		PDU_Service			*thePDU,
		DICOMCommandObject	*theDCO)
		{
		VR		*aVR;
		UID		aMyUID, aUID;
		
		GetUID(aMyUID);
		
		if ( ! thePDU )
			return ( FALSE );
			
		if ( ! theDCO )
			return ( FALSE );
		
		aVR = theDCO->GetVR(0x0000, 0x0002 );
		if ( ! aVR )
			return ( FALSE );
		
		if ( ! SetUID(aUID, aVR) )
			return ( FALSE );
		
		if ( aMyUID != aUID )
			return ( FALSE );
	
		aVR = theDCO->GetVR(0x0000, 0x1001);
		SetUID(aUID, aVR);	
		if ( NDeleteRQ :: Read ( theDCO, thePDU ) )
			return ( NDeleteRSP :: Write ( thePDU, theDCO, &aUID, 0 ) );
		
		return ( FALSE );
		}

// StandardAdvancedBox::WriteDelete() -- SCU method called to send
// a delete standard advanced box object request to SCP
// sends request to delete VOILUTBox and ImageOverlayBox instance to print SCP

BOOL
StandardAdvancedBox	::	WriteDelete (
	PDU_Service		*thePDU,
	UID				*theAffectedSOPInstanceUID,
	AbstractPrintManagementMeta &theManager)
	{
	if ( ! NDeleteRQ :: Write ( thePDU, theAffectedSOPInstanceUID) )
		return ( FALSE );
	
	BOOL aLookingForResponseFlag = TRUE;

	// look for a response or a possible asynchronous event report

	do
	{

	// attempt to read the response

	DICOMCommandObject		aDCO;

	if ( ! thePDU->Read ( &aDCO ) )
		return ( FALSE );

	// process the response
	
	if ( ! NDeleteRSP :: Read ( &aDCO, thePDU ) )
		{
		// not what we expected...
		// have manager's printer object attempt to receive a
		// possible printer status event or print job status event

		if ( ! theManager.ReadEventReport( thePDU, &aDCO ) )
			{
			// wasn't an event report either -- give up
			return ( FALSE );
			}
		}
		else
		{
		// don't check status -- assume error due to deleting something
		// twice -- doesn't really matter 
		//UINT16 aStatus = aDCO.GetUINT16(0x0000, 0x0900);

		//if (aStatus != 0)
			//{
			// failure or dubious warning from SCP
			//return FALSE;
			//}

		// found a response
		aLookingForResponseFlag = FALSE;	// found the response
		}
	} while ( aLookingForResponseFlag );
	
	return ( TRUE );
	}
	

/******************* StandardStatus ********************/

BOOL
StandardStatus	::	ReadGet (
	PDU_Service		*thePDU,
	DICOMCommandObject	*theDCO )
	{
	DICOMDataObject		aDDO;
	VR		*aVR;
	UID		aUID;
	UID		aMyUID;
		
	GetUID(aMyUID);
		
	if ( ! thePDU )
		return ( FALSE );
			
	if ( ! theDCO )
		return ( FALSE );
		
	aVR = theDCO->GetVR(0x0000, 0x0002 );
	if ( ! aVR )
		return ( FALSE );
		
	if ( ! SetUID(aUID, aVR) )
		return ( FALSE );
		
	if ( aMyUID != aUID )
		return ( FALSE );
	
	aVR = theDCO->GetVR(0x0000, 0x1001);
	SetUID(aUID, aVR);
#if 0
	GetSOPInstanceUID(aMyUID);
	if(aUID != aMyUID )
		{
		// SOP Instance UID's don't match up
		if(NGetRQ :: Read ( theDCO, thePDU, &aDDO ) )
			return ( NGetRSP :: Write ( thePDU, theDCO, &aUID, 0xc001, NULL ) );
		return ( FALSE );
		}
#endif
	if ( NGetRQ :: Read ( theDCO, thePDU, &aDDO ) )
		{
#if 0
		if(!Get(&aUID, &aDDO))
			return ( NGetRSP :: Write ( thePDU, theDCO, &aUID, 0x0fff, NULL ) );
#endif
		return ( NGetRSP :: Write ( thePDU, theDCO, &aUID, 0, &aDDO ) );
		}
		
	return ( FALSE );
	}	

// StandardStatus::WriteGet() -- SCU method called to send
// a get standard status object request to SCP

BOOL
StandardStatus	::	WriteGet (
	PDU_Service	*thePDU,
	UID			*theRequestedSOPInstanceUID,
	DICOMDataObject	*theDDO,
	AbstractPrintManagementMeta &theManager )
	{
	if ( ! NGetRQ :: Write ( thePDU, theDDO, theRequestedSOPInstanceUID) )
		return ( FALSE );
	
	BOOL aLookingForResponseFlag = TRUE;

	// look for a response or a possible asynchronous event report

	do
	{

	if ( theDDO )
		theDDO->Reset();	
		
	// attempt to read the response

	DICOMCommandObject aDCO;

	if ( ! thePDU->Read ( &aDCO ) )
		return ( FALSE );

	// process the response
	
	if ( ! NGetRSP :: Read ( &aDCO, thePDU, theDDO ) )
		{
		// not what we expected...
		// have manager's printer object attempt to receive a
		// possible printer status event or print job status event

		if ( ! theManager.ReadEventReport( thePDU, &aDCO ) )
			{
			// wasn't an event report either -- give up
			return ( FALSE );
			}
		}
		else
		{
		// found a response
		// ignore status which could be checked here...
		aLookingForResponseFlag = FALSE;	// found the response
		}
	} while ( aLookingForResponseFlag );

	return ( TRUE );
	}
		
	
// StandardStatus::ReadEventReport() -- SCU method called to receive
// a standard status sent by the SCP

BOOL
StandardStatus	::	ReadEventReport (
	PDU_Service	*thePDU,
	DICOMCommandObject	*theDCO,
	DICOMDataObject		*theDDO,
	UID         *theExpectedSOPInstanceUID)
	{
	VR		*aVR;
	UID		aMyUID, aUID;
		
	if ( ! thePDU )
		return ( FALSE );
			
	if ( ! theDCO )
		return ( FALSE );
		
	// check for wrong SOP class

	aVR = theDCO->GetVR(0x0000, 0x0002 );
	if ( ! aVR )
		return ( FALSE );
		
	if ( ! SetUID(aUID, aVR) )
		return ( FALSE );
		
	GetUID(aMyUID);

	if ( aMyUID != aUID )
		return ( FALSE );	// not our SOP class
	
	// get the affected SOP instance UID for the response

	aVR = theDCO->GetVR(0x0000, 0x1000);
	if ( !SetUID(aUID, aVR) )
		return ( FALSE );

	// check SOP instance UID if specified (NULL => accept any)

	if ( theExpectedSOPInstanceUID && *theExpectedSOPInstanceUID != aUID )
		{
		return FALSE;	// not specified instance
		}

	if ( theDDO )
		{
		theDDO->Reset();
		}

	// attempt to read the event report request

	if ( NEventReportRQ :: Read ( theDCO, thePDU, theDDO ) )
		{
		UINT16 anEventTypeID = theDCO->GetUINT16(0x0000, 0x1002);

		// return the response expected

		return ( NEventReportRSP :: Write ( thePDU, theDCO, &aUID, 0,
						anEventTypeID, NULL ) );
		}
	return ( FALSE );
	}	

// StandardStatus::WriteEventReport() -- SCP method

BOOL
StandardStatus	::	WriteEventReport (
	PDU_Service		*thePDU,
	UID				*theAffectedSOPInstanceUID,
	UINT16			theEventID,
	DICOMDataObject	*theDDO )
	{
	if ( ! NEventReportRQ :: Write ( thePDU, theDDO, theAffectedSOPInstanceUID,
		theEventID) )
		return ( FALSE );
	
	DICOMCommandObject		aDCO;
	
	if ( theDDO )
		theDDO->Reset ();
		
	if ( ! thePDU->Read ( &aDCO ) )
		return ( FALSE );

	if ( ! NEventReportRSP :: Read ( &aDCO, thePDU, theDDO ) )
		return ( FALSE );
	
	return ( TRUE );
	}
		
// Sync's (adds) an SOP Instance/Class to a VR SQObjectArray

BOOL
SyncReferenceSequence (
	VR		*theSQE,
	UID		*theSOPClassUID,
	UID		*theSOPInstanceUID)
	{
	if ( ! theSQE )
		return ( FALSE );
	
	if ( ! theSQE->SQObjectArray )
		{
		Array < DICOMDataObject * > *SQE = new Array < DICOMDataObject * >;
		DICOMDataObject 	*DDO = new DICOMDataObject;
		
		DDO->Push(TranslateUIDToVR(theSOPClassUID, 0x0008, 0x1150));
		DDO->Push(TranslateUIDToVR(theSOPInstanceUID, 0x0008, 0x1155));
		SQE->Add(DDO);
		
		theSQE->SQObjectArray = (void*) SQE;
		return ( TRUE );
		}
		
	UINT	Index = 0;
	UID		aCmpUID;
	DICOMDataObject	*DDO;
	Array < DICOMDataObject	* >	*SQArray = (Array < DICOMDataObject * > *)
											theSQE->SQObjectArray;
	
	while ( Index < SQArray->GetSize() )
		{
		DDO = SQArray->Get(Index);

		SetUID(aCmpUID, DDO->GetVR(0x0008, 0x1155));
		if ( aCmpUID == (*theSOPInstanceUID) )
			return ( TRUE );
		
		++Index;
		}
	
	DICOMDataObject 	*DDDO = new DICOMDataObject;
		
	DDDO->Push(TranslateUIDToVR(theSOPClassUID, 0x0008, 0x1150));
	DDDO->Push(TranslateUIDToVR(theSOPInstanceUID, 0x0008, 0x1155));
	SQArray->Add(DDDO);
	return ( TRUE )	;	
	}

BOOL
SyncReferenceSequence (
	DICOMDataObject	*DDO,
	UINT16			Group,
	UINT16			Element,
	UID		*SOPClassUID,
	UID		*SOPInstanceUID)
	{
	VR		*aVR;
	if(aVR = DDO->GetVR(Group, Element))
		return(SyncReferenceSequence (aVR, SOPClassUID, SOPInstanceUID) );
	else
		aVR = new VR ( Group, Element, 0, (BOOL) FALSE);
	DDO->Push(aVR);
	return ( SyncReferenceSequence (aVR, SOPClassUID, SOPInstanceUID) );
	}
	
BOOL
ResyncEntireReferenceSequence (
	VR		*theSQE,							// Sequence List to resynch
	Array < DICOMDataObject * >	*ADDO )		// list of SOPClass/Instance UID's
	{

	if(!theSQE)
		return ( FALSE );

	if(theSQE->SQObjectArray)
		{
		Array < DICOMDataObject	* >	*SQArray = (Array < DICOMDataObject * > *)
											theSQE->SQObjectArray;
		// empty the original reference list
		// deleting its contents

		while(SQArray->GetSize())
			{
			delete SQArray->Get(0);
			SQArray->RemoveAt(0);
			}
		delete SQArray;
		theSQE->SQObjectArray = NULL;
		}
	if ( ADDO )
		{
		UINT	Index = 0;
		
		while ( Index < ADDO->GetSize() )
			{
			UID		aSOPInstanceUID, aSOPClassUID;
			
			SetUID(aSOPInstanceUID, ADDO->Get(Index)->GetVR(0x0000, 0x0018));
			SetUID(aSOPClassUID, ADDO->Get(Index)->GetVR(0x0000, 0x0016));
			SyncReferenceSequence ( theSQE, &aSOPClassUID, &aSOPInstanceUID );
			++Index;
			}
		}
	return ( TRUE );	
	}
	
/************** Abstract Printer ************/

AbstractPrinter	::	AbstractPrinter(AbstractPrintManagementMeta &theManager) :
	itsLock(FALSE),
	// NOTE: standard says a printer has a well-known SOP instance 
	itsSOPInstanceUID(new UID ( "1.2.840.10008.5.1.1.17" ) ),
	itsSOPClassUID(new UID),
	itsManager(&theManager)
	{
	// use the manager's printer object to set the printer SOP class
	itsManager->itsPrinter.GetUID(*itsSOPClassUID);
	}

AbstractPrinter	::	~AbstractPrinter()
	{
	delete itsSOPClassUID;
	delete itsSOPInstanceUID;
	}

BOOL
AbstractPrinter	::	GetSOPInstanceUID ( UID	&theUID )
	{
	theUID = *itsSOPInstanceUID;
	return ( TRUE );
	}

BOOL
AbstractPrinter	::	SetSOPInstanceUID ( UID	&theUID )
	{
	delete itsSOPInstanceUID;
	if ( ( itsSOPInstanceUID = new UID ) == NULL )
		return ( FALSE );
	*itsSOPInstanceUID = theUID;
	return ( TRUE );
	}

/************** Abstract Print Job ************/

AbstractPrintJob	::	AbstractPrintJob(
	AbstractPrintManagementMeta &theManager) :
	itsLock(FALSE),
	itsSOPInstanceUID(new UID),	
	itsSOPClassUID(new UID),
	itsManager(&theManager)
	{
	// use the manager's print job object to set the print job SOP class
	itsManager->itsPrintJob.GetUID(*itsSOPClassUID);
	}

AbstractPrintJob	::	~AbstractPrintJob()
	{
	DeleteAll();
	}

BOOL
AbstractPrintJob	::	DeleteAll()
	{
	IdleProc (&itsLock);
	delete itsSOPClassUID;
	delete itsSOPInstanceUID;
	return ( TRUE );
	}

BOOL
AbstractPrintJob	::	GetSOPInstanceUID ( UID	&theUID )
	{
	theUID = *itsSOPInstanceUID;
	return ( TRUE );
	}

BOOL
AbstractPrintJob	::	SetSOPInstanceUID ( UID	&theUID )
	{
	delete itsSOPInstanceUID;
	if ( ( itsSOPInstanceUID = new UID ) == NULL )
		return ( FALSE );
	*itsSOPInstanceUID = theUID;
	return ( TRUE );
	}

// AbstractPrintJob::ProtoGen() -- 
// rebuild our abstract representation from the DICOMObject
// used when a new print job has been instantiated
// -- that happens as a result of N-ACTION-PRINT returning
// a dataset containing a print job instantiation

BOOL
AbstractPrintJob	::	ProtoGen()
	{
	// try to get a Referenced Print Job Sequence from our DDO

	VR	*aVR = GetVR(0x2100, 0x0500);

	if(aVR && aVR->SQObjectArray)
		{

		// get the sequence array from the VR

		Array < DICOMDataObject	* >	*aSQArray;
		aSQArray = (Array < DICOMDataObject * > *) aVR->SQObjectArray;
	
		UINT anIndex = 0;
		while( anIndex < aSQArray->GetSize() )
			{
			UID aUID;
			UID aReceivedUID;

			// retrieve the SOP Class UID from the sequence item

			aVR = aSQArray->Get(anIndex)->GetVR(0x0008, 0x1150);

			if ( aVR )
				{
				SetUID(aReceivedUID, aVR);
				itsManager->itsPrintJob.GetUID(aUID);

				// check to see if SOP class is ours
				if ( aReceivedUID == aUID )
					{

					// set our SOP class UID accordingly

					*itsSOPClassUID = aUID;
					
					// retrieve the SOP Instance UID from the sequence item

					aVR = aSQArray->Get(anIndex)->GetVR(0x0008, 0x1155);

					// set our SOP instance

					if ( aVR )
						{
						SetUID(aReceivedUID, aVR);
						SetSOPInstanceUID(aReceivedUID);

						return ( TRUE );
						}
					}
				}
			++anIndex;
			}		// end of loop looking for SOP Class/Instance info
		}		// end of if there is a referenced print job sequence
	return ( FALSE );
	}

/************** Abstract Overlay Box ***********/

AbstractOverlayBox	::	AbstractOverlayBox (AbstractPrintManagementMeta &theManager) :
	itsSOPInstanceUID(new UID),
	itsSOPClassUID(new UID),
	itsManager(&theManager),
	itsLock(FALSE)
	{
	}

AbstractOverlayBox	::	~AbstractOverlayBox ()
	{
	DeleteAll ();
	}

BOOL
AbstractOverlayBox	::	DeleteAll ()
	{
	IdleProc (&itsLock);
	
	delete itsSOPInstanceUID;
	delete itsSOPClassUID;
	itsSOPInstanceUID = NULL;
	itsSOPClassUID = NULL;
	return ( TRUE );
	}
	
//
// AbstractOverlayBox::ProtoGen() --
// rebuild our abstract representation from the DICOMObject

BOOL
AbstractOverlayBox	::	ProtoGen ()
	{
	DeleteAll ();

	if(!itsSOPInstanceUID)
		itsSOPInstanceUID = new UID;
	if(!itsSOPClassUID)
		itsSOPClassUID = new UID;

	// set the new overlay box's SOP Class/Instance

	UID aUID;
	itsManager->itsImageOverlayBox.GetUID(*itsSOPClassUID);
	itsManager->GetNewSOPInstanceUID(*itsSOPClassUID, *itsSOPInstanceUID);

	// check for referenced image box sequence
	// M/M -- error if missing or bad reference
	// code to do that goes here...
	
	// check for referenced Overlay sequence
	// U/M -- if present, error if bad reference or unsupported

	// remaining values are U/U could check validity of those present

	return ( TRUE );
	}
	
/************** Abstract LUT Box ***********/

AbstractLUTBox	::	AbstractLUTBox (AbstractPrintManagementMeta &theManager) :
	itsSOPInstanceUID(new UID),
	itsSOPClassUID(new UID),
	itsManager(&theManager),
	itsLock(FALSE)
	{
	}

AbstractLUTBox	::	~AbstractLUTBox ()
	{
	DeleteAll ();
	}

BOOL
AbstractLUTBox	::	DeleteAll ()
	{
	IdleProc (&itsLock);
	
	delete itsSOPInstanceUID;
	delete itsSOPClassUID;
	
	itsSOPInstanceUID = NULL;
	itsSOPClassUID = NULL;
	return ( TRUE );
	}
	
// AbstractLUTBox::ProtoGen() --
// rebuild our abstract representation from the DICOMObject

BOOL
AbstractLUTBox	::	ProtoGen ()
	{
	DeleteAll ();
	if(!itsSOPInstanceUID)
		itsSOPInstanceUID = new UID;
	if(!itsSOPClassUID)
		itsSOPClassUID = new UID;
		
	// set the new VOI LUT box's SOP Class/Instance

	// we don't know what kind of box or even if there really should be one...
	// GetLUTBox() has been overriden to return a pointer to
	// a LUT box of the correct type for the MetaSOP class being used
	// or NULL if the MetaSOP class doesn't support VOI LUT

	StandardAdvancedBox *aLUTBox = itsManager->GetLUTBox();

	if ( aLUTBox )
		{
		UID aUID;
		aLUTBox->GetUID(*itsSOPClassUID);
		itsManager->GetNewSOPInstanceUID(*itsSOPClassUID, *itsSOPInstanceUID);
		}
	else
		{
		// the real class represented doesn't support VOI LUT box
		// so its an error
		// handle that here
		return FALSE;
		}

	// check for presence of VOI LUT Box Seqeunce and validate if present

	// check for and validate other optional attributes

	return ( TRUE );
	}
	
/************** Abstract Annotation Box ***********/

AbstractAnnotationBox	::	AbstractAnnotationBox (AbstractPrintManagementMeta &theManager) :
	itsSOPInstanceUID(new UID),
	itsSOPClassUID(new UID),
	itsManager(&theManager),
	itsLock(FALSE)
	{
	}

AbstractAnnotationBox	::	~AbstractAnnotationBox ()
	{
	DeleteAll ();
	}

BOOL
AbstractAnnotationBox	::	DeleteAll ()
	{
	IdleProc (&itsLock);
	
	delete itsSOPInstanceUID;
	delete itsSOPClassUID;
	
	itsSOPInstanceUID = NULL;
	itsSOPClassUID = NULL;
	return ( TRUE );
	}
	
// AbstractAnnotationBox::ProtoGen() --
// rebuild our abstract representation from the DICOMObject

BOOL
AbstractAnnotationBox	::	ProtoGen ()
	{
	DeleteAll ();
	if(!itsSOPInstanceUID)
		itsSOPInstanceUID = new UID;
	if(!itsSOPClassUID)
		itsSOPClassUID = new UID;
		
	// set the new annotation box's SOP Class/Instance

	UID aUID;
	itsManager->itsBasicAnnotationBox.GetUID(*itsSOPClassUID);
	itsManager->GetNewSOPInstanceUID(*itsSOPClassUID, *itsSOPInstanceUID);

	// Annotation boxes are never N-CREATED explicitly -- only implicitly
	// created and N-SET

	return ( TRUE );
	}
	
/************** Abstract Image Box ****************/	

AbstractImageBox	::	AbstractImageBox (AbstractPrintManagementMeta &theManager) :
	itsSOPInstanceUID(new UID),
	itsSOPClassUID(new UID),
	itsLock(FALSE),
	itsManager(&theManager),
	itsAbstractOverlayBoxs(NULL),
	itsAbstractLUTBoxs(NULL)
	{
	}	

AbstractImageBox	::	~AbstractImageBox ()
	{
	IdleProc (&itsLock);
	DeleteAll ();
	}

AbstractOverlayBox*
AbstractImageBox	::	Add(AbstractOverlayBox *theOverlayBox)
{
	if(!itsAbstractOverlayBoxs)
		itsAbstractOverlayBoxs = new Array < AbstractOverlayBox * >;
	return itsAbstractOverlayBoxs->Add(theOverlayBox);
}

AbstractLUTBox*
AbstractImageBox	::	Add(AbstractLUTBox *theLUTBox)
{
	if(!itsAbstractLUTBoxs)
		itsAbstractLUTBoxs = new Array < AbstractLUTBox * >;
	return itsAbstractLUTBoxs->Add(theLUTBox);
}

BOOL
AbstractImageBox		::	DeleteAll ()
	{
	IdleProc (&itsLock);
	
	delete itsSOPInstanceUID;
	delete itsSOPClassUID;
	
	if ( itsAbstractOverlayBoxs )
		{
		UINT	Index = 0;
		
		while ( Index < itsAbstractOverlayBoxs->GetSize() )
			{
			delete itsAbstractOverlayBoxs->Get(Index);
			++Index;
			}
		
		delete itsAbstractOverlayBoxs;
		}
	if ( itsAbstractLUTBoxs )
		{
		UINT	Index = 0;
		
		while ( Index < itsAbstractLUTBoxs->GetSize() )
			{
			delete itsAbstractLUTBoxs->Get(Index);
			++Index;
			}
		
		delete itsAbstractLUTBoxs;	
		}
	itsSOPInstanceUID = NULL;
	itsSOPClassUID = NULL;
	itsAbstractOverlayBoxs = NULL;
	itsAbstractLUTBoxs = NULL;
	return ( TRUE );
	}
	
// AbstractImageBox::ProtoGen() --
// rebuild our abstract representation from the DICOMObject

BOOL
AbstractImageBox	::	ProtoGen ()
	{
	UINT	Index = 0;	

	// clear all storage
	
	DeleteAll ();

	// recreate any missing objects

	if(!itsSOPInstanceUID)
		itsSOPInstanceUID = new UID;
	if(!itsSOPClassUID)
		itsSOPClassUID = new UID;
	if(!itsAbstractOverlayBoxs)
		itsAbstractOverlayBoxs = new Array < AbstractOverlayBox * >;
	if(!itsAbstractLUTBoxs)
		itsAbstractLUTBoxs = new Array < AbstractLUTBox * >;

	// set the new image box's SOP Class/Instance
	// we don't know what kind of box
	// GetImageBox() has been overriden to return a pointer to
	// a Image box of the correct type for the MetaSOP class being used

	StandardBox *aImageBox = itsManager->GetImageBox();

	if ( aImageBox )
		{
		UID aUID;
		aImageBox->GetUID(*itsSOPClassUID);
		itsManager->GetNewSOPInstanceUID(*itsSOPClassUID, *itsSOPInstanceUID);
		}
	else
		{
		// this shouldn't happen -- implementation error
		// handle that here
		return FALSE;
		}


	// Image boxes are never N-CREATED explicitly -- only implicitly
	// created and N-SET
	
	return ( TRUE );
	}
	

/************** Abstract Film Box *****************/

AbstractFilmBox		::	AbstractFilmBox (AbstractPrintManagementMeta &theManager) :
	itsSOPInstanceUID(new UID),
	itsSOPClassUID(new UID),
	itsLock(FALSE),
	itsManager(&theManager),
	itsAbstractImageBoxs(NULL),
	itsAbstractAnnotationBoxs(NULL),
	itsCreated(FALSE)
	{
	}	

AbstractFilmBox		::	~AbstractFilmBox ()
	{
	DeleteAll ();
	}

AbstractImageBox*
AbstractFilmBox	::	Add(AbstractImageBox *theImageBox)
{
	if(!itsAbstractImageBoxs)
		itsAbstractImageBoxs = new Array < AbstractImageBox * >;
	return itsAbstractImageBoxs->Add(theImageBox);
}

AbstractAnnotationBox*
AbstractFilmBox	::	Add(AbstractAnnotationBox *theAnnotationBox)
{
	if(!itsAbstractAnnotationBoxs)
		itsAbstractAnnotationBoxs = new Array < AbstractAnnotationBox * >;
	return itsAbstractAnnotationBoxs->Add(theAnnotationBox);
}

BOOL
AbstractFilmBox		::	DeleteAll ()
	{
	IdleProc (&itsLock);
	
	delete itsSOPInstanceUID;
	delete itsSOPClassUID;
	
	if ( itsAbstractImageBoxs )
		{
		UINT	Index = 0;
		
		while ( Index < itsAbstractImageBoxs->GetSize() )
			{
			delete itsAbstractImageBoxs->Get(Index);
			++Index;
			}
		
		delete itsAbstractImageBoxs;
		}
	if ( itsAbstractAnnotationBoxs )
		{
		UINT	Index = 0;
		
		while ( Index < itsAbstractAnnotationBoxs->GetSize() )
			{
			delete itsAbstractAnnotationBoxs->Get(Index);
			++Index;
			}
		
		delete itsAbstractAnnotationBoxs;	
		}
	itsSOPInstanceUID = NULL;
	itsSOPClassUID = NULL;
	itsAbstractImageBoxs = NULL;
	itsAbstractAnnotationBoxs = NULL;
	return ( TRUE );
	}
	
// AbstractFilmBox ::  ProtoGen ()
// rebuild SCP's abstract representation from the DICOMObject

BOOL
AbstractFilmBox	::	ProtoGen ()
	{
	UINT	Index = 0;	
	
	DeleteAll ();

	if(!itsSOPInstanceUID)
		itsSOPInstanceUID = new UID;
	if(!itsSOPClassUID)
		itsSOPClassUID = new UID;
	if(!itsAbstractImageBoxs)
		itsAbstractImageBoxs = new Array < AbstractImageBox * >;
	if(!itsAbstractAnnotationBoxs)
		itsAbstractAnnotationBoxs = new Array < AbstractAnnotationBox * >;

	// set the new film box's SOP Class/Instance

	UID aUID;
	itsManager->itsBasicFilmBox.GetUID(*itsSOPClassUID);
	itsManager->GetNewSOPInstanceUID(*itsSOPClassUID, *itsSOPInstanceUID);

	// create the abstract image boxes implied by the Image Display Format

	// code to do that goes here...

	// create the Abstract Annotation Boxes implied by the
	// Annotation Display Format ID

	// code to do that goes here...

	return ( TRUE );
	}
	
//
// SetImageBoxInstanceUIDS() -- called to update the Image Box
// object's internally stored SOPClassUID and SOPInstanceUID
// from the values returned as a result of creating a film box
// this MUST be called prior to doing any SET operations on the
// image box

BOOL
AbstractFilmBox::SetImageBoxInstanceUIDS()
	{

	if(!itsAbstractImageBoxs)
		{
		return TRUE;	// nothing to do...
		}

	// get the Referenced Image Box Sequence attribute from our DDO
	// that lists the SOPClass/SOPInstance UID pair for each contained
	// image box created by the server for this film box

	VR	*aVR = GetVR(0x2010, 0x0510);

#if 1
/*
   MVH: here something is wrong when printing to OFFIS: aVR->SQObjectArray is NULL
   LJZ: Probably this happened because 'PrintGrayScaleImages' didn't have a proper
        RTC attached...

   LJZ: Also aVR may become NULL.
        It happens when OFFIS complains 'cannot create Basic Film Box: unsupported
        image display format: xxxxxx'
        Maybe there are other situations too...
        Anyway, we must fail.
*/

//printf("test %d\n", aVR->SQObjectArray);

//FILE *f = fopen("c:\\test.dat", "wb");
//fwrite(aVR->Data, aVR->Length, 1, f);
//fclose(f);
#endif

	if(aVR && aVR->SQObjectArray)
		{

		// get the sequence array from the VR

		Array < DICOMDataObject	* >	*aSQArray;
		aSQArray = (Array < DICOMDataObject * > *) aVR->SQObjectArray;
	
		if(aSQArray)
			{
			UINT anIndex = 0;

			while(anIndex < aSQArray->GetSize() &&
				anIndex < itsAbstractImageBoxs->GetSize())
				{

				// retrieve a pointer to the next image box
				// NOTE: user may add NULL box where desired
				// as a "placeholder" if it is to be unused.
				// NULL boxes will not be "SET"

				AbstractImageBox *aImageBox = itsAbstractImageBoxs->Get(anIndex);
				if(aImageBox)
					{

					// update the image box's internal SOPInstanceUID
					SetUID((*aImageBox->itsSOPInstanceUID),
						aSQArray->Get(anIndex)->GetVR(0x0008, 0x1155));

					// update the image box's internal SOPClassUID
					SetUID((*aImageBox->itsSOPClassUID),
						aSQArray->Get(anIndex)->GetVR(0x0008, 0x1150));
					}
				++anIndex;
				}
			}
		}
	else
		{
		return FALSE; // no boxes referenced
		}
	return TRUE;
	}
	
//
// SetAnnotationBoxInstanceUIDS() -- called to update the Annotation Box
// object's internally stored SOPClassUID and SOPInstanceUID
// from the values returned as a result of creating a film box
// this MUST be called prior to doing any SET operations on the
// image box

BOOL
AbstractFilmBox::SetAnnotationBoxInstanceUIDS()
	{

	if(!itsAbstractAnnotationBoxs)
		{
		return TRUE;	// nothing to do...
		}

	// get the Referenced Annotation Box Sequence attribute from our DDO
	// that lists the SOPClass/SOPInstance UID pair for each contained
	// image box created by the server for this film box

	VR	*aVR = GetVR(0x2010, 0x0520);

	if(aVR && aVR->SQObjectArray)
		{

		// get the sequence array from the VR

		Array < DICOMDataObject	* >	*aSQArray;
		aSQArray = (Array < DICOMDataObject * > *) aVR->SQObjectArray;
	
		if(aSQArray)
			{
			UINT anIndex = 0;

			while(anIndex < aSQArray->GetSize() &&
				anIndex < itsAbstractAnnotationBoxs->GetSize())
				{

				// retrieve a pointer to the next image box
				// NOTE: user may add NULL box where desired
				// as a "placeholder" if it is to be unused.
				// NULL boxes will not be "SET"

				AbstractAnnotationBox *aAnnotationBox = itsAbstractAnnotationBoxs->Get(anIndex);
				if(aAnnotationBox)
					{

					// update the image box's internal SOPInstanceUID
					SetUID((*aAnnotationBox->itsSOPInstanceUID),
						aSQArray->Get(anIndex)->GetVR(0x0008, 0x1155));

					// update the image box's internal SOPClassUID
					SetUID((*aAnnotationBox->itsSOPClassUID),
						aSQArray->Get(anIndex)->GetVR(0x0008, 0x1150));
					}
				++anIndex;
				}
			}
		}
	else
		{
		return FALSE; // no boxes referenced
		}
	return TRUE;
	}
	
/***************** Abstract Film Session ******************/

AbstractFilmSession	::	AbstractFilmSession () :
	itsAbstractFilmBoxs(NULL),
	itsAbstractPrintJobs(NULL),
	itsLock(FALSE),
	itsSOPInstanceUID(new UID),
	itsSOPClassUID(new UID),
	itsCreated(FALSE)
	{
	}

AbstractFilmSession	::	~AbstractFilmSession ()
	{
	DeleteAll ();
	}
	
AbstractFilmBox*
AbstractFilmSession	::	Add(AbstractFilmBox *theFilmBox)
{
	if(!itsAbstractFilmBoxs)
		itsAbstractFilmBoxs = new Array < AbstractFilmBox * >;
	return itsAbstractFilmBoxs->Add(theFilmBox);
}

AbstractPrintJob*
AbstractFilmSession	::	Add(AbstractPrintJob *thePrintJob)
{
	if(!itsAbstractPrintJobs)
		itsAbstractPrintJobs = new Array < AbstractPrintJob * >;
	return itsAbstractPrintJobs->Add(thePrintJob);
}

BOOL
AbstractFilmSession	::	DeleteAll ()
	{
	IdleProc (&itsLock);
	
	if ( itsAbstractFilmBoxs )
		{
		UINT	anIndex = 0;
		
		while ( anIndex < itsAbstractFilmBoxs->GetSize() )
			{
			delete itsAbstractFilmBoxs->Get(anIndex);
			++anIndex;
			}
		
		delete itsAbstractFilmBoxs;
		}
	
	if ( itsAbstractPrintJobs )
		{
		UINT	anIndex = 0;
		
		while ( anIndex < itsAbstractPrintJobs->GetSize() )
			{
			delete itsAbstractPrintJobs->Get(anIndex);
			++anIndex;
			}
		
		delete itsAbstractPrintJobs;
	}
	delete itsSOPInstanceUID;
	delete itsSOPClassUID;

	itsSOPInstanceUID = NULL;
	itsSOPClassUID = NULL;
	itsAbstractFilmBoxs = NULL;
	itsAbstractPrintJobs = NULL;

	return ( TRUE );
	}

BOOL
AbstractFilmSession	::	GetNewSOPInstanceUID( UID &theSOPClassUID, UID& theUID )
	{
	// for specified SOP Class, generate a new instance ID and return it

	theUID.Set("You.Need.To.Override.This.For.An.SCP");
	return FALSE;
	}
	
BOOL
AbstractFilmSession	::	ProtoGen ()
	{
	UINT	Index = 0;	
	
	DeleteAll ();

	if(!itsSOPInstanceUID)
		itsSOPInstanceUID = new UID;
	if(!itsSOPClassUID)
		itsSOPClassUID = new UID;
	if(!itsAbstractFilmBoxs)
		itsAbstractFilmBoxs = new Array < AbstractFilmBox * >;
	
	// set the new film session's SOP Class/Instance

	UID aUID;

	itsBasicFilmSession.GetUID(*itsSOPClassUID);
	GetNewSOPInstanceUID(*itsSOPClassUID, *itsSOPInstanceUID);

	// reserve memory specified in Memory Allocation VR
	// to store the film session
	// hierarchy and "working copy" if requested by SCU and supported by SCP

	// code to do that goes here...

	return ( TRUE );
	}

BOOL
AbstractFilmSession	::	ResyncAbstractFilmBoxs ()
	{	
	VR		*aVR = GetVR ( 0x2000, 0x0500 );
	Array < DICOMDataObject	* >	ADDO;
	DICOMDataObject				*DDO;
	UINT						Index;
		
	if ( ! aVR )
		return ( FALSE );


	Index = 0;
	while ( Index < itsAbstractFilmBoxs->GetSize() )
		{
		DDO = new DICOMDataObject;

		// Relink back up
		SyncReferenceSequence(
			itsAbstractFilmBoxs->Get(Index),
			0x2010,
			0x0500,
			itsSOPClassUID,
			itsSOPInstanceUID);
/*
******************* NEED Copy() **********************
		DDO->Push ( Copy ( NULL,
			itsAbstractFilmBoxs->Get(Index)->GetVR(0x0008, 0x0016) ) );
		DDO->Push ( Copy ( NULL,
			itsAbstractFilmBoxs->Get(Index)->GetVR(0x0008, 0x0018) ) );
*/
		ADDO.Add ( DDO );
		++Index;
		}
	ResyncEntireReferenceSequence(aVR, &ADDO);
	while(ADDO.GetSize() )
		{
		delete ADDO.Get(0);
		ADDO.RemoveAt(0);
		}
	return ( TRUE );
	}
	
BOOL
AbstractFilmBox	::	ResyncAbstractImageBoxs ()
	{	

	// get the Referenced Image Box Sequence from our DDO
	// this is the list of SOPClassUID's and SOPInstanceUID's for
	// each image box created by the SCP for this film box

	VR		*aVR = GetVR ( 0x2010, 0x0510 );

	Array < DICOMDataObject	* >	ADDO;
	DICOMDataObject				*DDO;
	UINT						Index;
		
	if ( ! aVR )
		return ( FALSE );


	Index = 0;

	// build a list of the SOPClass/SOPINstance UIDs
	// for each AbstractImageBox belonging to this film box

	while ( Index < itsAbstractImageBoxs->GetSize() )
		{
		DDO = new DICOMDataObject;
		
/*
******************* NEED Copy() **********************
		DDO->Push ( Copy ( NULL,
			itsAbstractImageBoxs->Get(Index)->GetVR(0x0008, 0x0016) ) );
		DDO->Push ( Copy ( NULL,
			itsAbstractImageBoxs->Get(Index)->GetVR(0x0008, 0x0018) ) );
*/
		ADDO.Add ( DDO );
		++Index;
		}

	// make the film box'es Referenced Image Box Sequence match the
	// list of Image boxes we just built

	ResyncEntireReferenceSequence(aVR, &ADDO);

	// empty the list we built

	while(ADDO.GetSize() )
		{
		delete ADDO.Get(0);
		ADDO.RemoveAt(0);
		}
	return ( TRUE );
	}
	
BOOL
AbstractFilmBox	::	ResyncAbstractAnnotationBoxs ()
	{	
	VR		*aVR = GetVR ( 0x2010, 0x0520 );
	Array < DICOMDataObject	* >	ADDO;
	DICOMDataObject				*DDO;
	UINT						Index;
		
	if ( ! aVR )
		return ( FALSE );


	Index = 0;
	while ( Index < itsAbstractAnnotationBoxs->GetSize() )
		{
		DDO = new DICOMDataObject;
		
/*
******************* NEED Copy() **********************
		DDO->Push ( Copy ( NULL,
			itsAbstractAnnotationBoxs->Get(Index)->GetVR(0x0008, 0x0016) ) );
		DDO->Push ( Copy ( NULL,
			itsAbstractAnnotationBoxs->Get(Index)->GetVR(0x0008, 0x0018) ) );
*/
		ADDO.Add ( DDO );
		++Index;
		}
	ResyncEntireReferenceSequence(aVR, &ADDO);
	while(ADDO.GetSize() )
		{
		delete ADDO.Get(0);
		ADDO.RemoveAt(0);
		}
	return ( TRUE );
	}
	
BOOL
AbstractImageBox	::	ResyncAbstractOverlayBoxs ()
	{	
	VR		*aVR = GetVR ( 0x2020, 0x0130 );
	Array < DICOMDataObject	* >	ADDO;
	DICOMDataObject				*DDO;
	UINT						Index;
		
	if ( ! aVR )
		return ( FALSE );


	Index = 0;
	while ( Index < itsAbstractOverlayBoxs->GetSize() )
		{
		DDO = new DICOMDataObject;
		
/*
******************* NEED Copy() **********************
		DDO->Push ( Copy ( NULL,
			itsAbstractOverlayBoxs->Get(Index)->GetVR(0x0008, 0x0016) ) );
		DDO->Push ( Copy ( NULL,
			itsAbstractOverlayBoxs->Get(Index)->GetVR(0x0008, 0x0018) ) );
*/
		ADDO.Add ( DDO );
		++Index;
		}
	ResyncEntireReferenceSequence(aVR, &ADDO);
	while(ADDO.GetSize() )
		{
		delete ADDO.Get(0);
		ADDO.RemoveAt(0);
		}
	return ( TRUE );
	}
	
BOOL
AbstractImageBox	::	ResyncAbstractLUTBoxs ()
	{	
	VR		*aVR = GetVR ( 0x2020, 0x0140 );
	Array < DICOMDataObject	* >	ADDO;
	DICOMDataObject				*DDO;
	UINT						Index;
		
	if ( ! aVR )
		return ( FALSE );


	Index = 0;
	while ( Index < itsAbstractLUTBoxs->GetSize() )
		{
		DDO = new DICOMDataObject;
/*
******************* NEED Copy() **********************
		DDO->Push ( Copy ( NULL,
			itsAbstractLUTBoxs->Get(Index)->GetVR(0x0008, 0x0016) ) );
		DDO->Push ( Copy ( NULL,
			itsAbstractLUTBoxs->Get(Index)->GetVR(0x0008, 0x0018) ) );
*/
		ADDO.Add ( DDO );
		++Index;
		}
	ResyncEntireReferenceSequence(aVR, &ADDO);
	while(ADDO.GetSize() )
		{
		delete ADDO.Get(0);
		ADDO.RemoveAt(0);
		}
	return ( TRUE );
	}

AbstractPrintManagementMeta	::	AbstractPrintManagementMeta() :
	itsAbstractPrinter(NULL), //(new AbstractPrinter(*this)),//bcb later, not here!
	itsPrinterEventUserArg(NULL),
	itsPrintJobEventUserArg(NULL)
	{
		itsAbstractPrinter = new AbstractPrinter(*this);//Would be a second time here! Leak!
	}

AbstractPrintManagementMeta ::  ~AbstractPrintManagementMeta()
	{
	delete itsAbstractPrinter;
	}

void*
AbstractPrintManagementMeta	::	SetPrinterEventUserArg ( void *theArg )
	{
	void *aTemp = itsPrinterEventUserArg;
	itsPrinterEventUserArg = theArg;
	return aTemp;
	}

void*
AbstractPrintManagementMeta	::	SetPrintJobEventUserArg ( void *theArg )
	{
	void *aTemp = itsPrintJobEventUserArg;
	itsPrintJobEventUserArg = theArg;
	return aTemp;
	}

BOOL
AbstractPrintManagementMeta	::	ReadBasicFilmSession (
	PDU_Service			*thePDU,
	DICOMCommandObject	*theDCO,
	UINT16				theCommand,
	UID					*theSOPInstanceUID )
	{
	DICOMDataObject		DDO;
	VR					*aVR;
	UINT16				ActionTypeID;
			
	switch ( theCommand )
		{
		case	N_CREATE_RQ:

			if ( theSOPInstanceUID->GetSize() )
				{
				if ((*(this->itsSOPInstanceUID)) == (*theSOPInstanceUID))
					{
					itsBasicFilmSession.NCreateRSP :: Write (
						thePDU, theDCO, theSOPInstanceUID, 0x0213, NULL );
					}
				// They want another instance created.  Technically,
				// not DICOM compatible.. but perhaps at a future date
				// DICOM will change and allow such things...
				return ( FALSE );
				}
			if ( ! itsBasicFilmSession.ReadCreate (
					thePDU,
					theDCO,
					this ) )
					{
					// Could not N-Create this film session
					return ( FALSE );
					}
			ProtoGen();
			return ( TRUE );

		case	N_SET_RQ:

			// Make sure they mean us..

			if ((*(this->itsSOPInstanceUID)) != (*theSOPInstanceUID))
				return ( FALSE );
			
			// Ok, read in a DDO, then update ourselves..

			if ( ! itsBasicFilmSession.ReadSet (
						thePDU,
						theDCO,
						&DDO	) )
				{
				return ( FALSE );
				}

			// pop attributes from the DDO of this object
			// and push them onto the just-read DDO object if the attribute
			// is not already in the just-read DDO
			// (thus building an updated copy of this object's DDO in the
			// just-read DDO)

			while ( aVR = this->Pop () )
				{
				if ( ! DDO.GetVR(aVR->Group, aVR->Element) )
					DDO.Push(aVR);
				else
					delete aVR;
				}

			// now, the DDO of this object is empty so it has been
			// reset. We can now push the updated list of the attributes back
			// onto the DDO of this object

			while ( aVR = DDO.Pop () )
				this->Push(aVR);

			// and update the SOPInstanceUID and SOPClassUID of this object
			// from the updated DDO
				
			ProtoGen();
			
			return ( TRUE );

		case	N_DELETE_RQ:

			// Make sure they mean us..

			if ((*(this->itsSOPInstanceUID)) != (*theSOPInstanceUID))
				return ( FALSE );

			if ( ! itsBasicFilmSession.ReadDelete (
						thePDU,
						theDCO))
				{
				return ( FALSE );
				}

			DeleteAll();
			Reset ();
			ProtoGen ();
			return ( TRUE );

		case	N_ACTION_RQ:
			// Make sure they mean us..
			if ((*(this->itsSOPInstanceUID)) != (*theSOPInstanceUID))
				return ( FALSE );
			
			ActionTypeID = theDCO->GetUINT16(0x0000, 0x1008);
			if ( ActionTypeID != 1 )
				{
				// Protocol Error	
				if ( theDCO->GetUINT16(0x0000, 0x0800) == 0x0101)
					thePDU->Read ( &DDO );
					
				itsBasicFilmSession.NActionRSP::Write (
					thePDU, theDCO, theSOPInstanceUID, 1, 0x0fff, NULL );
				return ( FALSE );
				}
				
			// There can only be one action...
									
			if ( ! itsBasicFilmSession.ReadPrint (
						thePDU,
						theDCO,
						&DDO))
				{
				return ( FALSE );
				}
			return ( ActionPrint ( this, &DDO ) );
		default:
		break;
		}
	return ( FALSE );		
	}

BOOL
AbstractPrintManagementMeta	::	ReadBasicFilmBox (
	PDU_Service			*thePDU,
	DICOMCommandObject	*theDCO,
	UINT16				theCommand,
	UID					*theSOPInstanceUID )
	{
	DICOMDataObject		aDDO;
	UINT16				ActionTypeID;
	
	switch ( theCommand )
		{

		case	N_CREATE_RQ:	// Append a new film box
			{
			AbstractFilmBox		*AFB = new AbstractFilmBox(*this);
				
			SetUID ( (*AFB->itsSOPInstanceUID), theDCO->GetVR(0x0000, 0x1000));
			SetUID ( (*AFB->itsSOPClassUID), theDCO->GetVR(0x0000, 0x00002));
		
			// Make sure they are not N-CREATE-RQ'ing something that
			// should really be N-SET-RQ.  This check might want to
			// be disabled on production servers inorder to allow for
			// poorly written clients.
			
			UINT	Index = 0;
			while ( Index < itsAbstractFilmBoxs->GetSize() )
				{
				if ((*(itsAbstractFilmBoxs->Get(Index)->itsSOPInstanceUID)) ==
						(*AFB->itsSOPInstanceUID) )
					{
					DICOMDataObject	aTempDDO;
					delete AFB;
					// Protocol Error
					if(theDCO->GetUINT16(0x0000, 0x0800)!=0x0101)
						thePDU->Read(&aTempDDO);		// throw away the data
					itsBasicFilmBox.NCreateRSP :: Write (
						thePDU, theDCO, theSOPInstanceUID, 0x0213, NULL );
					return ( FALSE );
					}
				++Index;
				}
				
			// Ok, can N-CREATE it..
			if ( ! itsBasicFilmBox.ReadCreate (
					thePDU,
					theDCO,
					AFB ))
					{
					// Could not N-Create this film box
					delete AFB;
					return ( FALSE );
					}
			SyncReferenceSequence (
				GetVR(0x2000, 0x0500),
				AFB->itsSOPClassUID,
				AFB->itsSOPInstanceUID);
			AFB->ProtoGen ();
			itsAbstractFilmBoxs->Add ( AFB );
			return ( TRUE );
			}

		case	N_SET_RQ:
			{
			// Locate the AFB which they are trying to set
			AbstractFilmBox		*AFB = NULL;
				
		
			// Make sure they are not N-SET-RQ'ing something that exists!
			
			UINT	Index = 0;
			while ( Index < itsAbstractFilmBoxs->GetSize() )
				{
				if ((*(itsAbstractFilmBoxs->Get(Index)->itsSOPInstanceUID)) ==
						(*AFB->itsSOPInstanceUID) )
					{
					AFB = itsAbstractFilmBoxs->Get(Index);
					break;
					}
				++Index;
				}
			
			// Here, they are trying to N-SET-RQ an Instance which hasn't
			// been N-CREATE-RQ'ed either in the Film Session IOD or
			// appended to it.  In this case we return a protocol error.
			// Similar note as to above.  If you want a more flexiable
			// server which allows for some less than DICOM compatible
			// clients then you might want to internally perform the
			// actions of the N-CREATE-RQ message and generate this instance.
			if ( ! AFB )
				{
				DICOMDataObject	aTempDDO;
				delete AFB;
				// Protocol Error
				if(theDCO->GetUINT16(0x0000, 0x0800)!=0x0101)
					thePDU->Read(&aTempDDO);		// throw away the data
				itsBasicFilmBox.NCreateRSP :: Write (
					thePDU, theDCO, theSOPInstanceUID, 0x0214, NULL );
				return ( FALSE );
				}
				
			if ( ! itsBasicFilmBox.ReadSet (
					thePDU,
					theDCO,
					AFB ))
					{
					// Could not N-Create this film box
					return ( FALSE );
					}
			AFB->ProtoGen ();
			return ( TRUE );
			}

		case	N_DELETE_RQ:
			{
			// Locate the AFB which they are trying to set
			AbstractFilmBox		*AFB = NULL;
		
			// Make sure they are not N-DELETE-RQ'ing something that exists!
			
			UINT	Index = 0;
			while ( Index < itsAbstractFilmBoxs->GetSize() )
				{
				if ((*(itsAbstractFilmBoxs->Get(Index)->itsSOPInstanceUID)) ==
						(*AFB->itsSOPInstanceUID) )
					{
					AFB = itsAbstractFilmBoxs->Get(Index);
					break;
					}
				++Index;
				}
			
			if ( ! AFB )
				{
				DICOMDataObject	aTempDDO;
				delete AFB;
				// Protocol Error
				if(theDCO->GetUINT16(0x0000, 0x0800)!=0x0101)
					thePDU->Read(&aTempDDO);		// throw away the data
				itsBasicFilmBox.NCreateRSP :: Write (
					thePDU, theDCO, theSOPInstanceUID, 0x0214, NULL );
				return ( FALSE );
				}
				
			if ( ! itsBasicFilmBox.ReadDelete (
					thePDU,
					theDCO))
				{
				return ( FALSE );
				}
			// Ok, ugly.. we need to delete this AFB from existence..
			// Poor algoritham for doing this.. but it's not something
			// one should be doing anyways..
			VR		*aVR = GetVR(0x2000, 0x0500);
			if ( ! aVR )
				return ( FALSE );	
			aVR->Reset();
			aVR->Group = 0x2000;
			aVR->Element = 0x0500;
			aVR->TypeCode = 'SQ';
			delete AFB;
			itsAbstractFilmBoxs->RemoveAt(Index);
			Index = 0;
			while ( Index < itsAbstractFilmBoxs->GetSize() )
				{
				AFB = itsAbstractFilmBoxs->Get(Index);
				SyncReferenceSequence ( aVR, AFB->itsSOPClassUID, AFB->itsSOPInstanceUID );
				++Index;
				}
			return ( TRUE );
			}

		case	N_ACTION_RQ:
			
			{
			ActionTypeID = theDCO->GetUINT16(0x0000, 0x1008);
			if ( ActionTypeID != 1 )
				{
				// Protocol Error	
				if ( theDCO->GetUINT16(0x0000, 0x0800) == 0x0101)
					thePDU->Read ( &aDDO );
					
				itsBasicFilmBox.NActionRSP::Write (
					thePDU, theDCO, theSOPInstanceUID, 1, 0x0fff, NULL );
				return ( FALSE );
				}

			AbstractFilmBox		*anAFB = NULL;
		
			// Make sure they are not N-ACTION-RQ'ing something that exists!
			
			UINT	Index = 0;
			while ( Index < itsAbstractFilmBoxs->GetSize() )
				{
				if ((*(itsAbstractFilmBoxs->Get(Index)->itsSOPInstanceUID)) ==
						(*anAFB->itsSOPInstanceUID) )
					{
					anAFB = itsAbstractFilmBoxs->Get(Index);
					break;
					}
				++Index;
				}
			
			if ( ! anAFB )
				{
				DICOMDataObject	aTempDDO;
				delete anAFB;
				// Protocol Error
				if(theDCO->GetUINT16(0x0000, 0x0800)!=0x0101)
					thePDU->Read(&aTempDDO);		// throw away the data
				itsBasicFilmBox.NCreateRSP :: Write (
					thePDU, theDCO, theSOPInstanceUID, 0x0214, NULL );
				return ( FALSE );
				}
				
			// There can only be one action...
									
			if ( ! itsBasicFilmBox.ReadPrint (
						thePDU,
						theDCO,
						&aDDO))
				{
				return ( FALSE );
				}
				
			// invoke user-written print method to print the AFB

			return ( ActionPrint ( anAFB, &aDDO ) );
			}

		default:
		break;
		}
	return ( FALSE );		
	}

BOOL
AbstractPrintManagementMeta	::	Read (
	PDU_Service			*thePDU,
	DICOMCommandObject	*theDCO )
	{
	// First of all, what is this command?
	LE_UINT16	Command;
	UID			aSOPInstanceUID;
	UID			aSOPClass;
	UID			aCmpUID;
	VR			*aVR;
	
	Command = theDCO->GetUINT16 ( 0x0000, 0x0100 );
	if(aVR=theDCO->GetVR(0x0000, 0x1000))
		SetUID(aSOPInstanceUID, aVR);
	else
	if(aVR=theDCO->GetVR(0x0000, 0x1001))
		SetUID(aSOPInstanceUID, aVR);
	
	SetUID(aSOPClass, theDCO->GetVR(0x0000, 0x0002));
	if ( ! AcceptSOPClass ( aSOPClass ) )
		{
		// This SOP class is not us.  We should allow some other object
		// to handle it.  Note that it might be a print request, but it's
		// sent to the wrong SOP class (color instead of gray scale? etc).
		return ( FALSE );
		}
	itsBasicFilmSession.GetUID(aCmpUID);
	if ( aSOPClass == aCmpUID )
		{
		return (
			ReadBasicFilmSession(thePDU, theDCO, (UINT16) Command, &aSOPInstanceUID )
			);
		}
	
	itsBasicFilmBox.GetUID ( aCmpUID );
	if ( aSOPClass == aCmpUID )
		{
		return (
			ReadBasicFilmBox(thePDU, theDCO, (UINT16) Command, &aSOPInstanceUID )
			);
		}
	
	// Must be an image box, annotation box, or overlay box of some kind
	// Which means this command better be an N-SET-RQ
	// If it's not, then the command is not ment for us.
	
	if ( ! ((Command == N_SET_RQ)||(Command == N_CREATE_RQ)||
		(Command == N_DELETE_RQ)) )
		return ( FALSE );

	// This is a bit of a mess.. truth is we really don't care if it's
	// image box, an overlay, or an annotation,.. we still must N-SET-RQ
	// it all the same.  Hence, we just progress through to locate the
	// thing where interested in.. then ReadBox it.  It's the instanced
	// class further up the chain to make sure they want it...
	
	UINT	FilmBoxIndex = 0,
			ImageBoxIndex = 0,
			Index = 0;
	AbstractFilmBox			*AFB;
	AbstractImageBox		*AIB;
	AbstractAnnotationBox	*AAB;
	AbstractOverlayBox		*AOB;
	AbstractLUTBox			*ALB;
	DICOMDataObject			loDDO;
	VR						*loVR = NULL;
	UID						LinkedSOPInstanceUID;
		
	if ( Command == N_CREATE_RQ )
		{
		// These are handled seperatly.  Where N_CREAT_RQ either an
		// Overlay object, or an LUT module
		// In such a case we need to read the DDO
	
		if(!ReadBox(thePDU, theDCO, &loDDO))
			{
			if(!itsImageOverlayBox.ReadCreate(thePDU, theDCO, &loDDO))
				return ( FALSE );
			loVR = GetVR(0x2040, 0x0500);
			}
		else
			loVR = GetVR(0x0008, 0x1140);
		
		if(loVR)
			{
			Array < DICOMDataObject * >	*SQArray = 
				(Array < DICOMDataObject * > *)loVR->SQObjectArray;
			
			if(SQArray)
				SetUID(LinkedSOPInstanceUID, SQArray->Get(0)->GetVR(0x0008, 0x0018) );
				
			}
		}
	
	while ( FilmBoxIndex < itsAbstractFilmBoxs->GetSize() )
		{
		AFB = itsAbstractFilmBoxs->Get(Index);
		ImageBoxIndex = 0;
		while ( ImageBoxIndex < AFB->itsAbstractImageBoxs->GetSize() )
			{
			AIB = AFB->itsAbstractImageBoxs->Get(ImageBoxIndex);
			
			// Is this an N_CREATE_RQ?
			if(loVR)
				{
				if(loVR->Group == 0x2040)
					{
					// It's an overlay	
					if (LinkedSOPInstanceUID == (*AIB->itsSOPInstanceUID) )
						{
						// Good, now, move the loDDO into a new AOB
						AOB = new AbstractOverlayBox(*this);
						
						while(aVR = loDDO.Pop () )
							AOB->Push(aVR);
						AOB->ProtoGen ();
						AIB->itsAbstractOverlayBoxs->Add ( AOB );
						AIB->ResyncAbstractOverlayBoxs();
						return ( TRUE );
						}
					}
				else
					{
					// It's a LUT module
					if (LinkedSOPInstanceUID == (*AIB->itsSOPInstanceUID) )
						{
						ALB = new AbstractLUTBox(*this);
						
						while ( aVR = loDDO.Pop () )
							ALB->Push(aVR);
						
						ALB->ProtoGen ();
						AIB->itsAbstractLUTBoxs->Add ( ALB );
						AIB->ResyncAbstractLUTBoxs ();
						return ( TRUE );
						}	
					}
				}
			// Is it the image box?
			if ((*AIB->itsSOPInstanceUID) == aSOPInstanceUID)
				{
				if(ReadBox(thePDU, theDCO, AIB))
					{
					AIB->ProtoGen ();
					ActionStore ( AIB );
					return ( TRUE );
					}
				return ( FALSE );
				}
			// Is it an overlay box belonging to this image box?
			Index = 0;
			while ( Index < AIB->itsAbstractOverlayBoxs->GetSize() )
				{
				AOB = AIB->itsAbstractOverlayBoxs->Get(Index);
				if((*AOB->itsSOPInstanceUID) == aSOPInstanceUID)
					{
					if(Command == N_SET_RQ)
						{
						if(!itsImageOverlayBox.ReadSet(thePDU, theDCO, AOB))
							{
							return ( FALSE );
							}
						}
					else
					if ( Command == N_DELETE_RQ )
						{
						if(itsImageOverlayBox.ReadDelete(thePDU, theDCO))
							{
							delete AOB;
							AIB->itsAbstractOverlayBoxs->RemoveAt(Index);
							AIB->ResyncAbstractOverlayBoxs ();
							return ( TRUE );
							}
						return ( FALSE );
						}
					else
						{
						// Protocol error: N_CREATE_RQ an instance already
						// made
						itsImageOverlayBox.NDeleteRSP :: Write (
							thePDU, theDCO, itsSOPInstanceUID, 0x0fff );
						// Yeh we handled it, but it's really an error.
						// The remote side knows it, this TRUE is so that
						// the UL code will reset and ackwoledge that
						// we handled it, not that it was handled right...
						return ( TRUE );
						}
					return ( TRUE );
					}
				++Index;
				}
			// Is it a LUT box belonging to this image box?
			Index = 0;
			while ( Index < AIB->itsAbstractLUTBoxs->GetSize() )
				{
				ALB = AIB->itsAbstractLUTBoxs->Get(Index);
				
				if((*ALB->itsSOPInstanceUID) == aSOPInstanceUID)
					{
					if(Command == N_SET_RQ)
						{
						ActionRetreive ( ALB );	// Encase were overrighting it
						if(ReadBox(thePDU, theDCO, ALB))
							{
							ActionStore ( ALB );
							return ( TRUE );
							}
						}
					else
					if ( Command == N_DELETE_RQ )
						{
						ActionRetreive ( ALB );
						if(ReadBox(thePDU, theDCO, ALB))
							{
							delete ALB;
							AIB->itsAbstractLUTBoxs->RemoveAt(Index);
							AIB->ResyncAbstractLUTBoxs ();
							return ( TRUE );
							}
						return ( FALSE );
						}
					else
						{
						// Protocol error: N_CREATE_RQ an instance already
						// made
						return ( TRUE );
						}
					return ( FALSE );
					}
				++Index;
				}
			++ImageBoxIndex;							
			}
		// Is it an annotation box belonging to this film box?
		Index = 0;
		while ( Index < AFB->itsAbstractAnnotationBoxs->GetSize() )
			{
			AAB = AFB->itsAbstractAnnotationBoxs->Get(Index);
			if((*AAB->itsSOPInstanceUID) == aSOPInstanceUID)
				{
				if(!itsBasicAnnotationBox.Read(thePDU, theDCO, AAB))
					{
					return ( FALSE );
					}
				ActionStore ( AAB );
				return ( TRUE );
				}
			++Index;
			}
		++FilmBoxIndex;
		}
	return ( FALSE );
	}

//
// AbstractPrintManagementMeta::Write()
//
// cause creation of the entire print session on the SCP,
// but don't issue the print session action
// can be called multiple times and will create only
// the parts of a session hierarchy that have not yet
// been created
//
// e.g. can be used in the non-collated printing style
// (print a single film box at a time rather than entire session)
// by repeatedly deleting the most-recently created film box
// adding a new film box, and calling this Write() routine after
// each addition of a film box
//

BOOL
AbstractPrintManagementMeta	::	Write( PDU_Service *thePDU )
	{
	if( !IsCreated() )
		{
		// ensure that the BasicFilmSession's SOPClassUID is set

		itsBasicFilmSession.GetUID(*itsSOPClassUID);

		// create the session -- this has the side effect of updating
		// our SOPInstanceUID with the value returned from the SCP 

		if(!itsBasicFilmSession.WriteCreate(thePDU, itsSOPInstanceUID, this,
			*this ))
			{
			return FALSE;
			}

		// make note of the fact that session has been created on SCP

		SetCreated(TRUE);
		}

	// create each film box in this session

	UINT aFilmBoxIndex = 0;
		
	while(aFilmBoxIndex < itsAbstractFilmBoxs->GetSize())
		{

		// get the next AbstractFilmBox

		AbstractFilmBox *aFilmBox = itsAbstractFilmBoxs->Get(aFilmBoxIndex);

		if ( !aFilmBox->IsCreated() )
			{
			// ensure that the film box'es SOPClassUID is set corectly

			itsBasicFilmBox.GetUID(*(aFilmBox->itsSOPClassUID));

			// set the Referenced Film Session attribute of the film box
			// to the SOPClassUID and SOPInstanceUID of the parent film session

			// add the Referenced Film Session sequence attribute

			VR *aVR = new VR(0x2010, 0x0500, 0, FALSE);

			aFilmBox->Push(aVR);

			// create  DDO to hold items in the sequence

			DICOMDataObject *aDDO = new DICOMDataObject;

			// add the referenced film session (this object)'s SOPClassUID
			// to the sequence

			aDDO->Push(TranslateUIDToVR(itsSOPClassUID, 0x0008, 0x1150));

			// add the referenced film session (this object)'s SOPInstanceUID
			// to the sequence

			aDDO->Push(TranslateUIDToVR(itsSOPInstanceUID, 0x0008, 0x1155));

			// add the sequence DDO to the film box'es most recently
			// pushed VR (the Referenced Film Session attribute)

			aFilmBox->Push(aDDO);

			// create the film box which has the side effect of
			// setting the film box'es SOPInstanceUID from the
			// affected SOPInstanceUID returned in the response message

			if(!itsBasicFilmBox.WriteCreate(thePDU, aFilmBox->itsSOPInstanceUID,
				(DICOMDataObject *)aFilmBox, *this ) )
				{
				break;
				}

			// make note of film box created on SCP

			aFilmBox->SetCreated(TRUE);

			// each film box when created implicitly creates a number of
			// subordinate image boxes (number depends upon film box format)
			// the SOPClassUID and SOPInstanceUID of each subordinate image
			// box is returned from the SCP as the Referenced Image Box sequence
			// in the response to the create call
			// each AbstractImageBox subordinate to the just-created film box
			// does not yet have its SOPClassUID and SOPInstanceUID set
			// since those were heretofore unknown
			// now we must set each image box's SOPClassUID and SOPInstanceUID to
			// the value returned from the server when the film box was created

			if ( aFilmBox->itsAbstractImageBoxs &&
				aFilmBox->SetImageBoxInstanceUIDS() )
				{

				// set each image box for this film box

				UINT aImageBoxIndex = 0;

				while(aImageBoxIndex < aFilmBox->itsAbstractImageBoxs->GetSize())
					{

					// get a pointer to next image box
					// NOTE: pointer may be NULL indicating a "placeholder"
					// which will not be sent to the SCP

					AbstractImageBox *aImageBox =
						aFilmBox->itsAbstractImageBoxs->Get(aImageBoxIndex);

					if(aImageBox && !WriteBox(thePDU,
						aImageBox->itsSOPInstanceUID,
						(DICOMDataObject *)aImageBox))
						{
						break;
						}

					++aImageBoxIndex;

					}

				if(aImageBoxIndex < aFilmBox->itsAbstractImageBoxs->GetSize())
					{
					// WriteBox() failed in loop above
					break;
					}
				}		// end of if there are image boxes

			// each film box when created implicitly creates a number of
			// subordinate annotation boxes
			// (number depends upon Annotation Display Format ID of film box)
			// the SOPClassUID and SOPInstanceUID of each subordinate annotation
			// box is returned from the SCP as the Referenced Annotation
			// Box sequence in the response to the create call
			// each AbstractAnnotationBox subordinate to the
			// just-created film box
			// does not yet have its SOPClassUID and SOPInstanceUID set
			// since those were heretofore unknown
			// now we must set each annotation box's SOPClassUID and
			// SOPInstanceUID to
			// the value returned from the server when the film box was created

			if ( aFilmBox->itsAbstractAnnotationBoxs &&
				aFilmBox->SetAnnotationBoxInstanceUIDS() )
				{

				// set each annotation box for this film box

				UINT aAnnotationBoxIndex = 0;

				while(aAnnotationBoxIndex <
					aFilmBox->itsAbstractAnnotationBoxs->GetSize())
					{

					// get a pointer to next image box
					// NOTE: pointer may be NULL indicating a "placeholder"
					// which will not be sent to the SCP

					AbstractAnnotationBox *aAnnotationBox =
						aFilmBox->itsAbstractAnnotationBoxs->Get(
							aAnnotationBoxIndex);

					if(aAnnotationBox && !itsBasicAnnotationBox.Write(thePDU,
						aAnnotationBox->itsSOPInstanceUID,
						(DICOMDataObject *)aAnnotationBox, *this))
						{
						break;
						}

					++aAnnotationBoxIndex;

					}

				if(aAnnotationBoxIndex <
					aFilmBox->itsAbstractAnnotationBoxs->GetSize())
					{
					// Write() failed in loop above
					break;
					}
				}		// end of if there are annotation boxes

			}		// end of if film box not yet created

		++aFilmBoxIndex;
		}

	if(aFilmBoxIndex < itsAbstractFilmBoxs->GetSize())
		{
		// something failed in loop above
		return FALSE;
		}

	return TRUE;
	}


//
// AbstractPrintManagementMeta::PrintSession()
//
// ensure that the entire abstract print session hierarchy
// has been created on the SCP and
// then issue the command to print the entire session
//

BOOL
AbstractPrintManagementMeta	::	PrintSession( PDU_Service *thePDU,
	UID *thePrintJobInstanceUID )
	{

	// its an error to try to print an empty session...

	if ( ! itsAbstractFilmBoxs || itsAbstractFilmBoxs->GetSize() == 0 )
		{
		return FALSE;
		}

	// first, ensure that the entire abstract session hierarchy
	// has been created on the SCP -- Write() does that for us

	if ( !Write( thePDU ) )
		{
		return FALSE;
		}

	// issue the print session command
	// the data set returned will contain the Referenced Print Job Sequence
	// if SCP supports Print Job

	if ( itsBasicFilmSession.WritePrint(thePDU, itsSOPInstanceUID, *this ) )
		{
		// success => get the UID of the most recently created print job
		// and return to caller

		unsigned int anIndex;

		if ( thePrintJobInstanceUID )
			{

			AbstractPrintJob *aPrintJob = NULL;

			if ( itsAbstractPrintJobs &&
				(anIndex = itsAbstractPrintJobs->GetSize()) > 0)
				{
				--anIndex;	// index most recently added item
				aPrintJob = itsAbstractPrintJobs->Get(anIndex);

				if ( aPrintJob )
					{
					aPrintJob->GetSOPInstanceUID ( *thePrintJobInstanceUID );
					}
				}

			// set print job instance UID to "" if there was no print job

			if ( aPrintJob == NULL )
				{
				thePrintJobInstanceUID->Set("");
				}
			}

		return TRUE;	// success

		}

	return FALSE;	// utter failure...

	}

//
// AbstractPrintManagementMeta::PrintFilm()
//
// ensure that the entire abstract print session hierarchy
// has been created on the SCP and
// then issue the command to print the most-recently added Film Box
//

BOOL
AbstractPrintManagementMeta	::	PrintFilm( PDU_Service *thePDU,
	UID *thePrintJobInstanceUID,
	UINT16 theFilmBoxIndex)
	{

	// its an error to try to print nothing...or something that isn't there

	if ( ! itsAbstractFilmBoxs || itsAbstractFilmBoxs->GetSize() == 0 )
		{
		return FALSE;
		}

	// handle default case -- print most recent

	if ( theFilmBoxIndex == 0xffff )
		{
		theFilmBoxIndex = itsAbstractFilmBoxs->GetSize() - 1;
		}

	// check for out in the weeds...

	if ( theFilmBoxIndex >= itsAbstractFilmBoxs->GetSize() )
		{
		return FALSE;
		}

	// first, ensure that the entire abstract session hierarchy
	// has been created on the SCP -- Write() does that for us

	if ( !Write( thePDU ) )
		{
		return FALSE;
		}

	// get the most-recently-created film box

	AbstractFilmBox *aFilmBox = itsAbstractFilmBoxs->Get( theFilmBoxIndex );

	if ( !aFilmBox )
		{
		return FALSE;
		}

	// issue the print film command
	// the data set returned will contain the Referenced Print Job Sequence
	// if SCP supports Print Job

	if ( itsBasicFilmBox.WritePrint(thePDU, aFilmBox->itsSOPInstanceUID,
		*this ) )
		{
		// success => get the UID of the most recently created print job
		// and return to caller

		unsigned int anIndex;

		if ( thePrintJobInstanceUID )
			{

			AbstractPrintJob *aPrintJob = NULL;

			if ( itsAbstractPrintJobs &&
				(anIndex = itsAbstractPrintJobs->GetSize()) > 0)
				{
				--anIndex;	// index most recently added item
				aPrintJob = itsAbstractPrintJobs->Get(anIndex);

				if ( aPrintJob )
					{
					aPrintJob->GetSOPInstanceUID ( *thePrintJobInstanceUID );
					}
				}

			// set print job instance UID to "" if there was no print job

			if ( aPrintJob == NULL )
				{
				thePrintJobInstanceUID->Set("");
				}
			}

		return TRUE;	// success
		}

	return FALSE;	// failed...

	}

// AbstractPrintManagementMeta::Delete() -- delete the entire session

BOOL
AbstractPrintManagementMeta :: Delete ( PDU_Service *thePDU )
	{
	// ask the SCP to delete the film session

	if ( !itsBasicFilmSession.WriteDelete(thePDU, itsSOPInstanceUID, *this ) )
		{
		return FALSE;
		}

	// should we do delete on "this" ? NO -- "this" may be a pointer to
	// local storage as opposed to new allocated memory
	// leave that up to the caller we're outa here...

	return TRUE;
	}

// AbstractPrintManagementMeta::DeleteFilm() -- delete the last film created
// (actually, delete the last film box in our list of film boxes --
// SCP may support "popping" them so we'll let you try)

BOOL
AbstractPrintManagementMeta :: DeleteFilm ( PDU_Service *thePDU )
	{

	if ( ! itsAbstractFilmBoxs || itsAbstractFilmBoxs->GetSize() == 0 )
		{
		return TRUE;	// already deleted...
		}

	unsigned int anIndex = itsAbstractFilmBoxs->GetSize() - 1;
	AbstractFilmBox *aFilmBox = itsAbstractFilmBoxs->Get( anIndex );

	// ask the SCP to delete the film box

	if ( !itsBasicFilmBox.WriteDelete(thePDU, aFilmBox->itsSOPInstanceUID,
			*this ) )
		{
		return FALSE;
		}

	// remove the film box from the hierarchy too...

	delete aFilmBox;
	return itsAbstractFilmBoxs->RemoveAt( anIndex );
	}

BOOL
AbstractPrintManagementMeta	::	ActionPrint (
	AbstractFilmSession	*theAFS,
	DICOMDataObject		*theDDO )
	{
	// Call the ActionPrint function for each Film Box
	if ( ! theAFS )
		return ( FALSE );
	
	UINT	Index = 0;
		
	while ( Index < theAFS->itsAbstractFilmBoxs->GetSize() )
		{
/*
******************* NEED Copy() **********************
		ActionPrint (
			theAFS->itsAbstractFilmBoxs->Get(Index),
			(DICOMDataObject *) Copy(new DICOMDataObject, theDDO) );
*/
		++Index;
		}
	return ( TRUE );
	}

BOOL
AbstractPrintManagementMeta	::	AcceptSOPClass (
	UID		&theUID )
	{
	return ( TRUE );		// Accept any.  That is, any of the overloaded
							// Image/Referenced Image box's.. any overlay,
							// and any annotation.
							
							// You should of course override this (to disable
							// overlays, for instance)
	}
	
// AbstractPrintManagementMeta::ReadEventReport() -- SCU method
// called to check an incomming command object for the possibility
// that it contains either a printer status event report or a
// print job event report and handle the event report if found

BOOL
AbstractPrintManagementMeta	::	ReadEventReport (
		PDU_Service *thePDU,
		DICOMCommandObject *theDCO)
	{

	// we've been called upon to handle a possible
	// N-EVENT-REPORT request -- it may be a printer status
	// event or a print job status event
	// first, have our printer object attempt to recieve a
	// printer status event

	if ( ! itsPrinter.ReadEventReport( thePDU, theDCO,
		(DICOMDataObject *)itsAbstractPrinter,
		itsAbstractPrinter->itsSOPInstanceUID ) )
		{

		// not a printer status event -- have each outstanding
		// print job attempt to recieve a print job status event

		// LJZ: Testing with AGFA ADVT revealed that 'itsAbstractPrintJobs'
		//      might be NULL, and caused a crash
		if (!itsAbstractPrintJobs)
			return (FALSE);

		unsigned int anIndex;
		AbstractPrintJob *aPrintJob;

		for ( anIndex = 0; anIndex < itsAbstractPrintJobs->GetSize();
			++anIndex )
			{
			aPrintJob = itsAbstractPrintJobs->Get(anIndex);
			if( aPrintJob )
				{
				if ( itsPrintJob.ReadEventReport( thePDU, theDCO,
					(DICOMDataObject *)aPrintJob,
					aPrintJob->itsSOPInstanceUID ) )
					{
					// print job received an event -- call virtual
					// function defined by user to handle event

					UINT16 anEventTypeID = theDCO->GetUINT16(0x0000, 0x1002);
					VR *aVR = theDCO->GetVR(0x0000, 0x1000);
					UID aUID;
					SetUID(aUID, aVR);	

					HandlePrintJobEvent (
						(DICOMDataObject *)aPrintJob,
						aUID,
						anEventTypeID,
						itsPrintJobEventUserArg);

					break;
					}
				}
			}
			if ( anIndex >= itsAbstractPrintJobs->GetSize() )
				{
				// either we have no print jobs defined,
				// or it is a print job status event, but
				// the SOP Instance UID didn't match any we know of
				// attempt to receive a print job event for any
				// SOP Instance and discard it (NULL SOP instance
				// UID pointer => accept any)

				DICOMDataObject aDDO;

				if ( itsPrintJob.ReadEventReport( thePDU, theDCO,
					&aDDO, NULL ) )
					{
					// print job received the event which we discard
					return ( TRUE );
					}
				}
			else
				{
				// we broke search loop before end => received an event report
				return ( TRUE );
				}
			return ( FALSE );
		}
	else
		{
		// our printer received an event report -- call virtual
		// function defined by user to handle event

		UINT16 anEventTypeID = theDCO->GetUINT16(0x0000, 0x1002);
		VR *aVR = theDCO->GetVR(0x0000, 0x1000);
		UID aUID;
		SetUID(aUID, aVR);	

		HandlePrinterEvent ( (DICOMDataObject *)itsAbstractPrinter,
			aUID,
			anEventTypeID,
			itsPrinterEventUserArg);

		}
	return ( TRUE );
	}

BOOL
AbstractPrintManagementMeta	::	GetPrinter (
		PDU_Service *thePDU,
		DICOMDataObject *theDDO)
	{

	if(thePDU == NULL || theDDO == NULL)
		{
		return FALSE;
		}

	UID aUID;
	itsAbstractPrinter->GetSOPInstanceUID(aUID);

	return itsPrinter.WriteGet(thePDU, &aUID, theDDO, *this);
	}
		
BOOL
AbstractPrintManagementMeta	::	GetPrintJob (
		PDU_Service *thePDU,
		UID &thePrintJobInstance,
		DICOMDataObject *theDDO)
	{

	if(thePDU == NULL || theDDO == NULL)
		{
		return FALSE;
		}

	return itsPrintJob.WriteGet(thePDU, &thePrintJobInstance, theDDO, *this);
	}
		
BOOL
BasicGrayscalePrintManagementMeta::ReadBox(
	PDU_Service *thePDU,
	DICOMCommandObject *theDCO,
	DICOMDataObject *theDDO)
	{
	return itsBasicGrayscaleImageBox.Read(thePDU, theDCO, theDDO);
	}

BOOL
BasicGrayscalePrintManagementMeta::WriteBox(
	PDU_Service *thePDU,
	UID *theUID,
	DICOMDataObject *theDDO )
	{
	return itsBasicGrayscaleImageBox.Write(thePDU, theUID, theDDO, *this);
	}
