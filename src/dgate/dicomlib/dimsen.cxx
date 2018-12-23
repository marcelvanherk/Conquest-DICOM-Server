/* 19990415 	ljz	Removed static on TranslateUIDToVR */
/* 20020822	mvh	Use instead of DIMSEN.CPP */
/* 20071118	mvh	Removed faulty overloaded 'NGetRQ::Write' */
/* 20080905	bcb	Removed (void*) cast for deivr change */
/* 20100309	bcb	Added double parentheses (gcc4.2 Warnings) */
/* 20100619	bcb	Added UNUSED(x) */
/* 20100717	mvh	Merged */

#	include	"dicom.hpp"

#ifndef UNUSED_ARGUMENT
#define UNUSED_ARGUMENT(x) (void)x
#endif

static	VR		*CopyVR ( VR * );
static	VR		*BuildATVR ( DICOMObject *, UINT16, UINT16 );
static	BOOL	DecomposeATVR ( VR *, DICOMObject * );

BOOL
NEventReportRQ	:: Read (
	DICOMCommandObject	*DCO,
	PDU_Service			*PDU,
	DICOMDataObject		*DDO )
	{
	if ( ! DCO )
		return ( FALSE );
	
	if ( DCO->GetUINT16(0x0000, 0x0100) != 0x0100 )
		return ( FALSE );

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

	return ( TRUE );
	}

BOOL
NEventReportRQ	::	Write (
	PDU_Service		*PDU,
	DICOMDataObject	*DDO,
	UID				*AffectedSOPInstanceUID,
	UINT16			EventID)
	{
	UID				SOPClassUID;
	LE_UINT16		Command, MessageID, DataSetType, EventTypeID;
	VR				*vr;
	DICOMCommandObject	DCO;

	if ( ! PDU )
		return ( FALSE );

	Command = 0x0100;
	MessageID = uniq16odd();
	if ( DDO )
		DataSetType = 0x0102;
	else
		DataSetType = 0x0101;

	EventTypeID = EventID;
	if ( ! GetUID ( SOPClassUID ) )
		{
		return ( FALSE );
		}

	vr = TranslateUIDToVR ( &SOPClassUID, 0x0000, 0x0002 );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0100, sizeof(UINT16), &Command, (BOOL) FALSE );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0110, sizeof(UINT16), &MessageID, (BOOL) FALSE );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0800, sizeof(UINT16), &DataSetType, (BOOL) FALSE );
	DCO.Push(vr);

	if ( AffectedSOPInstanceUID )
		{
		vr = TranslateUIDToVR ( AffectedSOPInstanceUID, 0x0000, 0x1000 );
		DCO.Push ( vr );
		}

	vr = new VR ( 0x0000, 0x1002, sizeof(UINT16), &EventTypeID, (BOOL) FALSE );
	DCO.Push ( vr );

	if ( ! PDU->Write ( &DCO, SOPClassUID ) )
		return ( FALSE );

	if ( DDO )
		{
		if ( ! PDU->Write ( DDO, SOPClassUID ) )
			return ( FALSE );
		}
	return ( TRUE );
	}


BOOL
NEventReportRQ	::	Write (
	PDU_Service		*PDU,
	UID				*AffectedSOPInstanceUID,
	UINT16			EventID )
	{
	return ( Write ( PDU, NULL, AffectedSOPInstanceUID, EventID) );
	}


BOOL
NEventReportRSP	::	Read (
	DICOMCommandObject	*DCO,
	PDU_Service			*PDU,
	DICOMDataObject		*DDO)
	{
	if ( ! DCO )
		return ( FALSE );
	
	if ( DCO->GetUINT16(0x0000, 0x0100) != 0x8100 )
		return ( FALSE );

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

	return ( TRUE );
	}


BOOL
NEventReportRSP	::	Write (
	PDU_Service			*PDU,
	DICOMCommandObject	*DCO,
	UID					*AffectedSOPInstanceUID,
	UINT16				Status,
	UINT16				EventID,
	DICOMDataObject		*DDO)
	{
	UID				SOPClassUID;
	LE_UINT16		Command, MessageID, DataSetType, EventTypeID, StatusField;
	VR				*vr;
	DICOMCommandObject	rDCO;

	if ( ! PDU )
		return ( FALSE );
	
	if ( ! DCO )
		return ( FALSE );

	Command = 0x8100;
	MessageID = DCO->GetUINT16 ( 0x0000, 0x0110 );

	if ( DDO )
		DataSetType = 0x0102;
	else
		DataSetType = 0x0101;

	EventTypeID = EventID;
	StatusField = Status;

	if ( ! GetUID ( SOPClassUID ) )
		{
		return ( FALSE );
		}

	vr = TranslateUIDToVR ( &SOPClassUID, 0x0000, 0x0002 );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0100, sizeof(UINT16), &Command, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0120, sizeof(UINT16), &MessageID, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0800, sizeof(UINT16), &DataSetType, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0900, sizeof(UINT16), &StatusField, (BOOL) FALSE );
	rDCO.Push(vr);

	if ( AffectedSOPInstanceUID )
		{
		vr = TranslateUIDToVR ( AffectedSOPInstanceUID, 0x0000, 0x1000 );
		rDCO.Push ( vr );
		}
	else
		{
		// Check to see if we can snag it from the command object
		vr = DCO->GetVR(0x0000, 0x1000);
		if ( vr )
			{
			// Copy it
			vr = CopyVR ( vr );
			rDCO.Push( vr );
			}
		}

	vr = new VR ( 0x0000, 0x1002, sizeof(UINT16), &EventTypeID, (BOOL) FALSE );
	rDCO.Push ( vr );

	if ( ! PDU->Write ( &rDCO, SOPClassUID ) )
		return ( FALSE );

	if ( DDO )
		{
		if ( ! PDU->Write ( DDO, SOPClassUID ) )
			return ( FALSE );
		}
	return ( TRUE );
	}

BOOL
NEventReportRSP	::	Write (
	PDU_Service			*PDU,
	DICOMCommandObject	*DCO,
	UID					*AffectedSOPInstanceUID,
	UINT16				EventID,
	DICOMDataObject		*DDO)
	{
	return ( Write ( PDU, DCO, AffectedSOPInstanceUID, 0x0000, EventID, DDO ) );
	}

/*********************** N-GET-RQ ****************************/

BOOL
NGetRQ	::	Read (
	DICOMCommandObject	*DCO,
	PDU_Service			*PDU,
	DICOMDataObject		*DDO)
	{
	UNUSED_ARGUMENT(DDO);
	UNUSED_ARGUMENT(PDU);
	if ( ! DCO )
		return ( FALSE );
	
	if ( DCO->GetUINT16(0x0000, 0x0100) != 0x0110 )
		return ( FALSE );

	DecomposeATVR(DCO->GetVR(0x0000, 0x1005), DDO);

	return ( TRUE );
	}

BOOL
NGetRQ	::	Write (
	PDU_Service	*PDU,
	DICOMDataObject *DDO,
	UID			*RequestedSOPInstanceUID)
	{
	UID				SOPClassUID;
	LE_UINT16		Command, MessageID, DataSetType;
	VR				*vr;
	DICOMCommandObject	DCO;

	if ( ! PDU )
		return ( FALSE );

	Command = 0x0110;
	MessageID = uniq16odd();
	DataSetType = 0x0101;

	if ( ! GetUID ( SOPClassUID ) )
		{
		return ( FALSE );
		}

	vr = TranslateUIDToVR ( &SOPClassUID, 0x0000, 0x0003 );	// Note Affected SOP Class UID
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0100, sizeof(UINT16), &Command, (BOOL) FALSE );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0110, sizeof(UINT16), &MessageID, (BOOL) FALSE );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0800, sizeof(UINT16), &DataSetType, (BOOL) FALSE );
	DCO.Push(vr);

	if ( RequestedSOPInstanceUID )
		{
		vr = TranslateUIDToVR ( RequestedSOPInstanceUID, 0x0000, 0x1001 );
		DCO.Push ( vr );
		}

	vr = BuildATVR(DDO, 0x0000, 0x1005);
	DCO.Push(vr);

	if ( ! PDU->Write ( &DCO, SOPClassUID ) )
		return ( FALSE );

	return ( TRUE );
	}

//BOOL
//NGetRQ	::	Write (
//	PDU_Service	*PDU,
//	UID			*RequestedSOPInstanceUID)
//	{
//	return ( Write ( PDU, RequestedSOPInstanceUID) );
//	}

BOOL
NGetRSP		::	Read (
	DICOMCommandObject *DCO,
	PDU_Service *PDU,
	DICOMDataObject *DDO)
	{
	if ( ! DCO )
		return ( FALSE );
	
	if ( DCO->GetUINT16(0x0000, 0x0100) != 0x8110 )
		return ( FALSE );

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

	return ( TRUE );
	}

BOOL
NGetRSP		::	Write (
	PDU_Service *PDU,
	DICOMCommandObject *DCO,
	UID		*AffectedSOPInstanceUID,
	UINT16	Status,
	DICOMDataObject	*DDO)
	{
	UID				SOPClassUID;
	LE_UINT16		Command, MessageID, DataSetType, StatusField;
	VR				*vr;
	DICOMCommandObject	rDCO;

	if ( ! PDU )
		return ( FALSE );
	
	if ( ! DCO )
		return ( FALSE );

	Command = 0x8110;
	MessageID = DCO->GetUINT16 ( 0x0000, 0x0110 );

	if ( DDO )
		DataSetType = 0x0102;
	else
		DataSetType = 0x0101;

	StatusField = Status;

	if ( ! GetUID ( SOPClassUID ) )
		{
		return ( FALSE );
		}

	vr = TranslateUIDToVR ( &SOPClassUID, 0x0000, 0x0002 );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0100, sizeof(UINT16), &Command, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0120, sizeof(UINT16), &MessageID, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0800, sizeof(UINT16), &DataSetType, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0900, sizeof(UINT16), &StatusField, (BOOL) FALSE );
	rDCO.Push(vr);

	if ( AffectedSOPInstanceUID )
		{
		vr = TranslateUIDToVR ( AffectedSOPInstanceUID, 0x0000, 0x1000 );
		rDCO.Push ( vr );
		}
	else
		{
		// Check to see if we can snag it from the command object
		vr = DCO->GetVR(0x0000, 0x1000);
		if ( vr )
			{
			// Copy it
			vr = CopyVR ( vr );
			rDCO.Push( vr );
			}
		}


	if ( ! PDU->Write ( &rDCO, SOPClassUID ) )
		return ( FALSE );

	if ( DDO )
		{
		if ( ! PDU->Write ( DDO, SOPClassUID ) )
			return ( FALSE );
		}
	return ( TRUE );
	}

/**************************** N-SET-RQ *********************************/

BOOL
NSetRQ		::	Read (
	DICOMCommandObject *DCO,
	PDU_Service *PDU,
	DICOMDataObject *DDO)
	{
   	if ( ! DCO )
		return ( FALSE );
	
	if ( DCO->GetUINT16(0x0000, 0x0100) != 0x0120 )
		return ( FALSE );

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

	return ( TRUE );
	}

BOOL
NSetRQ		::	Write (
	PDU_Service *PDU,
	DICOMDataObject *DDO,
	UID		*RequestedSOPInstanceUID )
	{
	UID				SOPClassUID;
	LE_UINT16		Command, MessageID, DataSetType;
	VR				*vr;
	DICOMCommandObject	DCO;

	if ( ! PDU )
		return ( FALSE );

	Command = 0x0120;
	MessageID = uniq16odd();

//	if ( DDO )
		DataSetType = 0x0102;
//	else
//		DataSetType = 0x0101;

	if ( ! GetUID ( SOPClassUID ) )
		{
		return ( FALSE );
		}

	vr = TranslateUIDToVR ( &SOPClassUID, 0x0000, 0x0003 );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0100, sizeof(UINT16), &Command, (BOOL) FALSE );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0110, sizeof(UINT16), &MessageID, (BOOL) FALSE );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0800, sizeof(UINT16), &DataSetType, (BOOL) FALSE );
	DCO.Push(vr);

	if ( RequestedSOPInstanceUID )
		{
		vr = TranslateUIDToVR ( RequestedSOPInstanceUID, 0x0000, 0x1001 );
		DCO.Push ( vr );
		}

	if ( ! PDU->Write ( &DCO, SOPClassUID ) )
		return ( FALSE );

	if ( DDO )
		{
		if ( ! PDU->Write ( DDO, SOPClassUID ) )
			return ( FALSE );
		}
	return ( TRUE );
	}

BOOL
NSetRQ		::	Write (
	PDU_Service *PDU,
	UID		*RequestedSOPInstanceUID )
	{
	return ( Write ( PDU, NULL, RequestedSOPInstanceUID ) );
	}

BOOL
NSetRSP		::	Read (
	DICOMCommandObject *DCO,
	PDU_Service *PDU,
	DICOMDataObject *DDO)
	{
   	if ( ! DCO )
		return ( FALSE );
	
	if ( DCO->GetUINT16(0x0000, 0x0100) != 0x8120 )
		return ( FALSE );

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

	return ( TRUE );
	}

BOOL
NSetRSP		::	Write (
	PDU_Service *PDU,
	DICOMCommandObject *DCO,
	UID		*AffectedSOPInstanceUID,
	UINT16	Status,
	DICOMDataObject	*DDO)
	{
	UID				SOPClassUID;
	LE_UINT16		Command, MessageID, DataSetType, StatusField;
	VR				*vr;
	DICOMCommandObject	rDCO;

	if ( ! PDU )
		return ( FALSE );
	
	if ( ! DCO )
		return ( FALSE );

	Command = 0x8120;
	MessageID = DCO->GetUINT16 ( 0x0000, 0x0110 );

	if ( DDO )
		DataSetType = 0x0102;
	else
		DataSetType = 0x0101;

	StatusField = Status;

	if ( ! GetUID ( SOPClassUID ) )
		{
		return ( FALSE );
		}

	vr = TranslateUIDToVR ( &SOPClassUID, 0x0000, 0x0002 );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0100, sizeof(UINT16), &Command, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0120, sizeof(UINT16), &MessageID, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0800, sizeof(UINT16), &DataSetType, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0900, sizeof(UINT16), &StatusField, (BOOL) FALSE );
	rDCO.Push(vr);

	if ( AffectedSOPInstanceUID )
		{
		vr = TranslateUIDToVR ( AffectedSOPInstanceUID, 0x0000, 0x1000 );
		rDCO.Push ( vr );
		}
	else
		{
		// Check to see if we can snag it from the command object
		vr = DCO->GetVR(0x0000, 0x1000);
		if ( vr )
			{
			// Copy it
			vr = CopyVR ( vr );
			rDCO.Push( vr );
			}
		}


	if ( ! PDU->Write ( &rDCO, SOPClassUID ) )
		return ( FALSE );

	if ( DDO )
		{
		if ( ! PDU->Write ( DDO, SOPClassUID ) )
			return ( FALSE );
		}
	return ( TRUE );
	}

/***************************** N-ACTION *******************************/

BOOL
NActionRQ	::	Read (
	DICOMCommandObject *DCO,
	PDU_Service *PDU,
	DICOMDataObject *DDO)
	{
   	if ( ! DCO )
		return ( FALSE );
	
	if ( DCO->GetUINT16(0x0000, 0x0100) != 0x0130 )
		return ( FALSE );

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

	return ( TRUE );
	}

BOOL
NActionRQ	::	Write (
	PDU_Service *PDU,
	DICOMDataObject *DDO,
	UID		*RequestedSOPInstanceUID,
	UINT16	ActionTypeID)
	{
	UID				SOPClassUID;
	LE_UINT16		Command, MessageID, DataSetType, ActionID;
	VR				*vr;
	DICOMCommandObject	DCO;

	if ( ! PDU )
		return ( FALSE );

	Command = 0x0130;
	MessageID = uniq16odd();
	if ( DDO )
		DataSetType = 0x0102;
	else
		DataSetType = 0x0101;

	ActionID = ActionTypeID;
	if ( ! GetUID ( SOPClassUID ) )
		{
		return ( FALSE );
		}

	vr = TranslateUIDToVR ( &SOPClassUID, 0x0000, 0x0003 );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0100, sizeof(UINT16), &Command, (BOOL) FALSE );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0110, sizeof(UINT16), &MessageID, (BOOL) FALSE );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0800, sizeof(UINT16), &DataSetType, (BOOL) FALSE );
	DCO.Push(vr);

	if ( RequestedSOPInstanceUID )
		{
		vr = TranslateUIDToVR ( RequestedSOPInstanceUID, 0x0000, 0x1001 );
		DCO.Push ( vr );
		}

	vr = new VR ( 0x0000, 0x1008, sizeof(UINT16), &ActionID, (BOOL) FALSE );
	DCO.Push ( vr );

	if ( ! PDU->Write ( &DCO, SOPClassUID ) )
		return ( FALSE );

	if ( DDO )
		{
		if ( ! PDU->Write ( DDO, SOPClassUID ) )
			return ( FALSE );
		}
	return ( TRUE );
	}

BOOL
NActionRQ	::	Write (
	PDU_Service *PDU,
	UID		*RequestedSOPInstanceUID,
	UINT16	ActionTypeID)
	{

	return ( Write ( PDU, NULL, RequestedSOPInstanceUID, ActionTypeID ) );
	}


BOOL
NActionRSP	::	Read (
	DICOMCommandObject *DCO,
	PDU_Service *PDU,
	DICOMDataObject *DDO)
	{
   	if ( ! DCO )
		return ( FALSE );
	
	if ( DCO->GetUINT16(0x0000, 0x0100) != 0x8130 )
		return ( FALSE );

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

	return ( TRUE );
	}

BOOL
NActionRSP	::	Write (
	PDU_Service *PDU,
	DICOMCommandObject *DCO,
	UID		*AffectedSOPInstanceUID,
	UINT16	ActionTypeID,
	UINT16	Status,
	DICOMDataObject	*DDO)
	{
	UID				SOPClassUID;
	LE_UINT16		Command, MessageID, DataSetType, ActionID, StatusField;
	VR				*vr;
	DICOMCommandObject	rDCO;

	if ( ! PDU )
		return ( FALSE );
	
	if ( ! DCO )
		return ( FALSE );

	Command = 0x8130;
	MessageID = DCO->GetUINT16 ( 0x0000, 0x0110 );

	if ( DDO )
		DataSetType = 0x0102;
	else
		DataSetType = 0x0101;

	ActionID = ActionTypeID;
	StatusField = Status;

	if ( ! GetUID ( SOPClassUID ) )
		{
		return ( FALSE );
		}

	vr = TranslateUIDToVR ( &SOPClassUID, 0x0000, 0x0002 );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0100, sizeof(UINT16), &Command, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0120, sizeof(UINT16), &MessageID, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0800, sizeof(UINT16), &DataSetType, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0900, sizeof(UINT16), &StatusField, (BOOL) FALSE );
	rDCO.Push(vr);

	if ( AffectedSOPInstanceUID )
		{
		vr = TranslateUIDToVR ( AffectedSOPInstanceUID, 0x0000, 0x1000 );
		rDCO.Push ( vr );
		}
	else
		{
		// Check to see if we can snag it from the command object
		vr = DCO->GetVR(0x0000, 0x1000);
		if ( vr )
			{
			// Copy it
			vr = CopyVR ( vr );
			rDCO.Push( vr );
			}
		}

	vr = new VR ( 0x0000, 0x1008, sizeof(UINT16), &ActionID, (BOOL) FALSE );
	rDCO.Push ( vr );

	if ( ! PDU->Write ( &rDCO, SOPClassUID ) )
		return ( FALSE );

	if ( DDO )
		{
		if ( ! PDU->Write ( DDO, SOPClassUID ) )
			return ( FALSE );
		}
	return ( TRUE );
	}

/************************* N-CREATE **********************************/
BOOL
NCreateRQ	::	Read (
	DICOMCommandObject *DCO,
	PDU_Service *PDU,
	DICOMDataObject *DDO)
	{
   	if ( ! DCO )
		return ( FALSE );
	
	if ( DCO->GetUINT16(0x0000, 0x0100) != 0x0140 )
		return ( FALSE );

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

	return ( TRUE );
	}

BOOL
NCreateRQ	::	Write (
	PDU_Service *PDU,
	DICOMDataObject *DDO,
	UID		*AffectedSOPInstanceUID )
	{
	UID				SOPClassUID;
	LE_UINT16		Command, MessageID, DataSetType;
	VR				*vr;
	DICOMCommandObject	DCO;

	if ( ! PDU )
		return ( FALSE );

	Command = 0x0140;
	MessageID = uniq16odd();
//	if ( DDO )
		DataSetType = 0x0102;
//	else
//		DataSetType = 0x0101;

	if ( ! GetUID ( SOPClassUID ) )
		{
		return ( FALSE );
		}

	vr = TranslateUIDToVR ( &SOPClassUID, 0x0000, 0x0002 );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0100, sizeof(UINT16), &Command, (BOOL) FALSE );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0110, sizeof(UINT16), &MessageID, (BOOL) FALSE );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0800, sizeof(UINT16), &DataSetType, (BOOL) FALSE );
	DCO.Push(vr);

	if ( AffectedSOPInstanceUID )
		{
		vr = TranslateUIDToVR ( AffectedSOPInstanceUID, 0x0000, 0x1000 );
		DCO.Push ( vr );
		}


	if ( ! PDU->Write ( &DCO, SOPClassUID ) )
		return ( FALSE );

	if ( DDO )
		{
		if ( ! PDU->Write ( DDO, SOPClassUID ) )
			return ( FALSE );
		}
	return ( TRUE );
	}

BOOL
NCreateRQ	::	Write (
	PDU_Service *PDU,
	UID		*AffectedSOPInstanceUID )
	{

	return ( Write ( PDU, NULL, AffectedSOPInstanceUID ) );
	}


BOOL
NCreateRSP	::	Read (
	DICOMCommandObject *DCO,
	PDU_Service *PDU,
	DICOMDataObject *DDO )
	{
   	if ( ! DCO )
		return ( FALSE );
	
	if ( DCO->GetUINT16(0x0000, 0x0100) != 0x8140 )
		return ( FALSE );

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

	return ( TRUE );
	}

BOOL
NCreateRSP	::	Write (
	PDU_Service *PDU,
	DICOMCommandObject *DCO,
	UID		*AffectedSOPInstanceUID,
	UINT16	Status,
	DICOMDataObject	*DDO)
	{
	UID				SOPClassUID;
	LE_UINT16		Command, MessageID, DataSetType, StatusField;
	VR				*vr;
	DICOMCommandObject	rDCO;

	if ( ! PDU )
		return ( FALSE );
	
	if ( ! DCO )
		return ( FALSE );

	Command = 0x8140;
	MessageID = DCO->GetUINT16 ( 0x0000, 0x0110 );

	if ( DDO )
		DataSetType = 0x0102;
	else
		DataSetType = 0x0101;

	StatusField = Status;

	if ( ! GetUID ( SOPClassUID ) )
		{
		return ( FALSE );
		}

	vr = TranslateUIDToVR ( &SOPClassUID, 0x0000, 0x0002 );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0100, sizeof(UINT16), &Command, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0120, sizeof(UINT16), &MessageID, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0800, sizeof(UINT16), &DataSetType, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0900, sizeof(UINT16), &StatusField, (BOOL) FALSE );
	rDCO.Push(vr);

	if ( AffectedSOPInstanceUID )
		{
		vr = TranslateUIDToVR ( AffectedSOPInstanceUID, 0x0000, 0x1000 );
		rDCO.Push ( vr );
		}
	else
		{
		// Check to see if we can snag it from the command object
		vr = DCO->GetVR(0x0000, 0x1000);
		if ( vr )
			{
			// Copy it
			vr = CopyVR ( vr );
			rDCO.Push( vr );
			}
		}

	if ( ! PDU->Write ( &rDCO, SOPClassUID ) )
		return ( FALSE );

	if ( DDO )
		{
		if ( ! PDU->Write ( DDO, SOPClassUID ) )
			return ( FALSE );
		}
	return ( TRUE );
	}

/************************ N-DELETE **************************/

BOOL
NDeleteRQ	::	Read (
	DICOMCommandObject *DCO,
	PDU_Service *PDU)
	{
	UNUSED_ARGUMENT(PDU);
   	if ( ! DCO )
		return ( FALSE );
	
	if ( DCO->GetUINT16(0x0000, 0x0100) != 0x0150 )
		return ( FALSE );

	if ( DCO->GetUINT16(0x0000, 0x0800) != 0x0101 )
		{
		return ( TRUE );
		}

	return ( TRUE );
	}

BOOL
NDeleteRQ	::	Write (
	PDU_Service *PDU,
	UID		*RequestedSOPInstanceUID )
	{
	UID				SOPClassUID;
	LE_UINT16		Command, MessageID, DataSetType;
	VR				*vr;
	DICOMCommandObject	DCO;

	if ( ! PDU )
		return ( FALSE );

	Command = 0x0150;
	MessageID = uniq16odd();
	DataSetType = 0x0101;

	if ( ! GetUID ( SOPClassUID ) )
		{
		return ( FALSE );
		}

	vr = TranslateUIDToVR ( &SOPClassUID, 0x0000, 0x0003 );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0100, sizeof(UINT16), &Command, (BOOL) FALSE );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0110, sizeof(UINT16), &MessageID, (BOOL) FALSE );
	DCO.Push(vr);
	vr = new VR ( 0x0000, 0x0800, sizeof(UINT16), &DataSetType, (BOOL) FALSE );
	DCO.Push(vr);

	if ( RequestedSOPInstanceUID )
		{
		vr = TranslateUIDToVR ( RequestedSOPInstanceUID, 0x0000, 0x1001 );
		DCO.Push ( vr );
		}


	if ( ! PDU->Write ( &DCO, SOPClassUID ) )
		return ( FALSE );

	return ( TRUE );
	}



BOOL
NDeleteRSP	::	Read (
	DICOMCommandObject *DCO,
	PDU_Service *PDU)
	{
	UNUSED_ARGUMENT(PDU);
   	if ( ! DCO )
		return ( FALSE );
	
	if ( DCO->GetUINT16(0x0000, 0x0100) != 0x8150 )
		return ( FALSE );

	if ( DCO->GetUINT16(0x0000, 0x0800) != 0x0101 )
		{
		return ( TRUE );
		}

	return ( TRUE );
	}

BOOL
NDeleteRSP	::	Write (
	PDU_Service *PDU,
	DICOMCommandObject *DCO,
	UID		*AffectedSOPInstanceUID,
	UINT16	Status)
	{
	UID				SOPClassUID;
	LE_UINT16		Command, MessageID, DataSetType, StatusField;
	VR				*vr;
	DICOMCommandObject	rDCO;

	if ( ! PDU )
		return ( FALSE );
	
	if ( ! DCO )
		return ( FALSE );

	Command = 0x8150;
	MessageID = DCO->GetUINT16 ( 0x0000, 0x0110 );

	DataSetType = 0x0101;


	StatusField = Status;

	if ( ! GetUID ( SOPClassUID ) )
		{
		return ( FALSE );
		}

	vr = TranslateUIDToVR ( &SOPClassUID, 0x0000, 0x0002 );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0100, sizeof(UINT16), &Command, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0120, sizeof(UINT16), &MessageID, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0800, sizeof(UINT16), &DataSetType, (BOOL) FALSE );
	rDCO.Push(vr);
	vr = new VR ( 0x0000, 0x0900, sizeof(UINT16), &StatusField, (BOOL) FALSE );
	rDCO.Push(vr);

	if ( AffectedSOPInstanceUID )
		{
		vr = TranslateUIDToVR ( AffectedSOPInstanceUID, 0x0000, 0x1000 );
		rDCO.Push ( vr );
		}
	else
		{
		// Check to see if we can snag it from the command object
		vr = DCO->GetVR(0x0000, 0x1000);
		if ( vr )
			{
			// Copy it
			vr = CopyVR ( vr );
			rDCO.Push( vr );
			}
		}


	if ( ! PDU->Write ( &rDCO, SOPClassUID ) )
		return ( FALSE );

	return ( TRUE );
	}


// Convert UID structures to a VR structure
//
// Notes: very lenienant about encoding.  Makes a good effort to encode
// correctly VR UI type even if UID isn't perfect

VR		*
TranslateUIDToVR (
	UID *Uid,
	UINT16	Group,
	UINT16	Element )
	{
	UINT32		Length;
	VR			*vr;
	char		*s;

	if ( ! Uid )
		{
		return ( new VR (Group, Element, 0, (BOOL) FALSE ) );
		}
	Length = Uid->GetSize ();

	if(Length & 0x01)
		++Length;

	vr = new VR ( Group, Element, Length, (BOOL) TRUE );

	if ( ! vr )
		return ( NULL );

	memset ( (void*)vr->Data, 0, Length);
	memcpy ( (void*)vr->Data, (void*)Uid->GetBuffer(1), Uid->GetSize() );

	Length = Uid->GetSize () ;

	s = (char*)vr->Data;
	while ( Length )
		{
		if(s[Length-1] == ' ' )
			s[Length-1] = '\0';
		else
			break;
		--Length;
		}
	if ( Length & 0x01 )
		++Length;

	vr->Length = Length;
	return ( vr );
	}

static	VR		*
CopyVR (
	VR *vr )
	{
	VR		*nvr;

	if ( ! vr )
		return ( NULL );

	if(vr->Length)
		{
		nvr = new VR ( vr->Group, vr->Element, vr->Length, (BOOL) TRUE );
		memcpy ( (void*)nvr->Data, (void*)vr->Data, vr->Length);
		}
	else
		nvr = new VR ( vr->Group, vr->Element, 0, (BOOL) FALSE );

	return ( nvr );
	}


static	VR		*
BuildATVR (
	DICOMObject *DO,
	UINT16		Group,
	UINT16 		Element)
	{
	VR		*vr, *tvr;
	DICOMObject	TDO;
	UINT		Count;
	LE_UINT16	dGroup, dElement;
	BYTE		*Data;

	if ( ! DO )
		return ( new VR ( Group, Element, 0, (BOOL) FALSE ) );

	Count = 0;


	while (( tvr = DO->Pop () ))
		{
		if ( tvr->Element != 0x0000 )
			++Count;
		TDO.Push(tvr);
		}
	
	vr = new VR ( Group, Element, (UINT32) Count * (sizeof(UINT16) * 2), (BOOL) TRUE );

	Data = (BYTE*)vr->Data;

	Count = 0;
	while (( tvr = TDO.Pop () ))
		{
		if ( tvr->Element == 0x0000 )
			{
			delete tvr;
			continue;
			}

		dGroup = tvr->Group;
		dElement = tvr->Element;

		memcpy ( (void*) &Data[Count], (void*) &dGroup, sizeof(UINT16) );
		Count += sizeof(UINT16);
		memcpy ( (void*) &Data[Count], (void*) &dElement, sizeof(UINT16) );
		Count += sizeof(UINT16);
		DO->Push(tvr);
		}

	return ( vr );
	}


static	BOOL
DecomposeATVR (
	VR 			*vr,
	DICOMObject *DO )
	{
	VR			*tvr;
	DICOMObject	TDO;
	UINT		Count;
	LE_UINT16	dGroup, dElement;
	BYTE		*Data;

	if ( ! vr )
		return ( FALSE );

	if ( ! vr->Data )
		return ( FALSE );

	if ( ! vr->Length )
		return ( FALSE );

	Count = 0;
	Data = (BYTE*)vr->Data;

	while ( Count < vr->Length )
		{
		memcpy ( (void*) &dGroup, (void*) &Data[Count], sizeof(UINT16) );
		Count += sizeof(UINT16);
		memcpy ( (void*) &dElement, (void*) &Data[Count], sizeof(UINT16) );
		Count += sizeof(UINT16);
		tvr = new VR ( dGroup, dElement, 0, (BOOL) FALSE );
		DO->Push(tvr);
		}

	return ( TRUE );
	}


