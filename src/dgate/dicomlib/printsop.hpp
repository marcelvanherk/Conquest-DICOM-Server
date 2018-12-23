/*
 * Printer SOP Classes
 *
 * Theory:
 *
 *		These are not simple classes like the Storage or Query/Retrieve
 * classes implemented in storage and qrsop.  See dicom.doc for a full
 * description of use/theory, but briefly:
 *
 *
 *
 *
 */


#ifndef	_PRINTSOP_H_
#	define	_PRINTSOP_H_
#include "errdescr.hpp"

class AbstractPrintManagementMeta;	// forward declaration
class ErrorDescription;				// forward reference -- see errdescr.hpp

// Conceptually, BasicFilmSession and BasicFilmBox are not inherited from
// a common base class.  However, the logic is so similar, that we cut down
// coding errors/time/size by making them inherit from this common
// Abstract Film class.

class	StandardFilm	:
	public	NCreateRQ,
	public	NCreateRSP,
	public	NSetRQ,
	public	NSetRSP,
	public	NDeleteRQ,
	public	NDeleteRSP,
	public	NActionRQ,
	public	NActionRSP
	{
	public:
		virtual		BOOL	uGetUID ( UID & ) = 0;
		BOOL		GetUID ( UID &uid ) { return ( uGetUID ( uid ) ); };
		//
		// ReadCreate() -- SCP uses to read a Create (N-CREATE) request
		// 	- create a basic film session SOP instance
		//  - create a basic film box SOP instance
		//    - also creates image box SOP instances (depending upon meta type)
		//    - also creates (if supported) annotation box SOP instances
		//  - create a VOI LUT box SOP instance
		//  - create a  image overlay box SOP instance
		BOOL		ReadCreate (
						PDU_Service *,
						DICOMCommandObject *,
						DICOMDataObject * );
		//
		// WriteCreate() -- SCU uses to send a Create request to SCP
		// (see ReadCreate() above for more details)
		//
		BOOL		WriteCreate (
						PDU_Service *,
						UID	*,
						DICOMDataObject *,
						AbstractPrintManagementMeta &theManager);

		//
		// ReadSet() -- SCP uses to read a Set (N-SET) request
		//
		//  - to update attributes of most recently created film session SOP
		//  - to update attributes of most recently created film box SOP
		//  - to update image box SOP of most recently created film box SOP
		//  - to update annotation box SOP of most recently created film box SOP
		//  - to update a VOI LUT SOP instance
		//  - to update a image overlay box SOP instance
		BOOL		ReadSet (
						PDU_Service *,
						DICOMCommandObject *,
						DICOMDataObject * );
		//
		// WriteSet() -- SCU uses to send Set request to SCP
		// (see ReadSet() above for more details)
		//
		BOOL		WriteSet (
						PDU_Service *,
						UID	*,
						DICOMDataObject *,
						AbstractPrintManagementMeta &theManager );
		//
		// ReadDelete() -- SCP uses to read a Delete (N-DELETE) request
		//
		//  - to delete film session SOP and all it's subordinate components
		//  - to delete the most recently created film box SOP
		//  - to delete a VOI LUT box SOP
		//  - to delete a image overlay box SOP
		BOOL		ReadDelete (
						PDU_Service *,
						DICOMCommandObject *);
		//
		// WriteDelete() -- SCU uses to send a Delete request to SCP
		// (see ReadDelete() above for more details)
		//
		BOOL		WriteDelete (
						PDU_Service *,
						UID *,
						AbstractPrintManagementMeta &theManager);
		//
		// ReadPrint() -- SCP uses to read a Print (N-ACTION) request
		//
		//  - to cause printing of an entire film session
		//  - to cause printing of a single film box within a film session
		//
		BOOL		ReadPrint (
						PDU_Service *,
						DICOMCommandObject *,
						DICOMDataObject * );
		//
		// WritePrint() -- SCU uses to send a Print request to SCP
		// (see ReadPrint() above for more details)
		//
		BOOL		WritePrint (
						PDU_Service *,
						UID *,
						AbstractPrintManagementMeta &theManager );
		//
		// Read() -- SCP uses to read any supported SOP from the connection
		//
		BOOL		Read (
						PDU_Service *,
						DICOMCommandObject	*,
						DICOMDataObject		*);
	};

class	BasicFilmSession	:
	public		StandardFilm
	{
	public:
		BOOL		uGetUID ( UID &uid );
		BOOL		Read (
						PDU_Service *PDU,
						DICOMCommandObject	*DCO,
						DICOMDataObject		*DDO)
				{ return ( StandardFilm :: Read ( PDU, DCO, DDO ) ); };
	};

class	BasicFilmBox	:
	public		StandardFilm
	{
	public:
		BOOL		uGetUID ( UID &uid );
		BOOL		Read (
						PDU_Service *PDU,
						DICOMCommandObject	*DCO,
						DICOMDataObject		*DDO)
				{ return ( StandardFilm :: Read ( PDU, DCO, DDO ) ); };
	};

//
// StandardBox -- base class for BasicGrayscaleImageBox,
//                BasicColorImageBox, ReferencedImageBox, and
//                BasicAnnotationBox
//

class	StandardBox	:
	public	NSetRQ,
	public	NSetRSP
	{
	public:
		BOOL		GetUID ( UID &uid ) { return (uGetUID(uid)); };
		virtual		BOOL	uGetUID ( UID &uid ) = 0;
		BOOL		Read (
						PDU_Service *,
						DICOMCommandObject *,
						DICOMDataObject * );
		BOOL		Write (
						PDU_Service *,
						UID *,
						DICOMDataObject *,
						AbstractPrintManagementMeta &theManager);
	};
	
//
// StandardAdvancedBox - base class for
//                       VOILUTBox and ImageOverlayBox

class	StandardAdvancedBox	:
	public	NSetRQ,
	public	NSetRSP,
	public	NCreateRQ,
	public	NCreateRSP,
	public	NDeleteRQ,
	public	NDeleteRSP
	{
	public:
		BOOL		GetUID ( UID &uid ) { return (uGetUID(uid)); };
		virtual		BOOL	uGetUID ( UID &uid ) = 0;
		BOOL		ReadSet (
						PDU_Service *,
						DICOMCommandObject *,
						DICOMDataObject * );
		BOOL		WriteSet (
						PDU_Service *,
						UID *,
						DICOMDataObject *,
						AbstractPrintManagementMeta &theManager );
		BOOL		ReadCreate (
						PDU_Service *,
						DICOMCommandObject *,
						DICOMDataObject * );
		BOOL		WriteCreate (
						PDU_Service *,
						UID	*,
						DICOMDataObject *,
						AbstractPrintManagementMeta &theManager);
		BOOL		ReadDelete (
						PDU_Service *,
						DICOMCommandObject *);
		BOOL		WriteDelete (
						PDU_Service *,
						UID *,
						AbstractPrintManagementMeta &theManager);
	};
	
//
// class BasicGrayscaleImageBox -- BasicGrayscaleImageBox SOP class
// heritage: StandardBox -> BasicGrayscaleImageBox
//

class	BasicGrayscaleImageBox	:
	public	StandardBox
	{
	public:
		BOOL	uGetUID ( UID &uid );
		BOOL	Read (
					PDU_Service 		*PDU,
					DICOMCommandObject	*DCO,
					DICOMDataObject		*DDO )
			{ return ( StandardBox :: Read ( PDU, DCO, DDO ) ); };
		BOOL	Write (
					PDU_Service 		*PDU,
					UID *uid,
					DICOMDataObject		*DDO,
					AbstractPrintManagementMeta &theManager)
			{ return ( StandardBox :: Write ( PDU, uid, DDO, theManager ) ); };
	};

//
// class BasicColorImageBox -- BasicColorImageBox SOP class
// heritage: StandardBox -> BasicColorImageBox
//

class	BasicColorImageBox	:
	public	StandardBox
	{
	public:
		BOOL	uGetUID ( UID &uid );
		BOOL	Read (
					PDU_Service 		*PDU,
					DICOMCommandObject	*DCO,
					DICOMDataObject		*DDO )
			{ return ( StandardBox :: Read ( PDU, DCO, DDO ) ); };
		BOOL	Write (
					PDU_Service 		*PDU,
					UID *uid,
					DICOMDataObject		*DDO,
					AbstractPrintManagementMeta &theManager)
			{ return ( StandardBox :: Write ( PDU, uid, DDO, theManager ) ); };
	};

//
// class ReferencedImageBox -- ReferencedImageBox SOP class
// heritage: StandardBox -> ReferencedImageBox
//

class	ReferencedImageBox	:
	public	StandardBox
	{
	public:
		BOOL	uGetUID ( UID &uid );
		BOOL	Read (
					PDU_Service 		*PDU,
					DICOMCommandObject	*DCO,
					DICOMDataObject		*DDO )
			{ return ( StandardBox :: Read ( PDU, DCO, DDO ) ); };
		BOOL	Write (
					PDU_Service 		*PDU,
					UID *uid,
					DICOMDataObject		*DDO,
					AbstractPrintManagementMeta &theManager)
			{ return ( StandardBox :: Write ( PDU, uid, DDO, theManager ) ); };
	};

//
// class VOILUTBox -- implement VOI LUT box SOP
// heritage: StandardAdvancedBox -> VOILUTBox
//

class	VOILUTBox	:
	public	StandardAdvancedBox
	{
	public:
		BOOL	uGetUID ( UID &uid );
	};

//
// class BasicAnnotationBox -- implement Basic Annotation Box SOP
// heritage: StandardBox -> BasicAnnotationBox
//

class	BasicAnnotationBox	:
	public	StandardBox
	{
	public:
		BOOL	uGetUID ( UID &uid );
		BOOL	Read (
					PDU_Service 		*PDU,
					DICOMCommandObject	*DCO,
					DICOMDataObject		*DDO )
			{ return ( StandardBox :: Read ( PDU, DCO, DDO ) ); };
		BOOL	Write (
					PDU_Service 		*PDU,
					UID *uid,
					DICOMDataObject		*DDO,
					AbstractPrintManagementMeta &theManager)
			{ return ( StandardBox :: Write ( PDU, uid, DDO, theManager ) ); };
	};

//
// class ImageOverlayBox -- implement image overlay box SOP
// heritage: StandardAdvancedBox -> ImageOverlayBox
//

class	ImageOverlayBox	:
	public	StandardAdvancedBox
	{
	public:
		BOOL	uGetUID ( UID &uid );
	};

//
// class StandardStatus -- base class for status reporting
//

class	StandardStatus	:
	public	NGetRQ,
	public	NGetRSP,
	public	NEventReportRQ,
	public	NEventReportRSP
	{
	public:
		BOOL		GetUID ( UID &uid ) { return ( uGetUID ( uid ) ); };
		virtual		BOOL	uGetUID ( UID &uid ) = 0;
		//virtual		BOOL	GetSOPInstanceUID ( UID &uid ) = 0;
		//virtual		BOOL	Get ( UID *, DICOMDataObject * ) = 0;
		//virtual		BOOL	GetStatus ( UID *, DICOMDataObject * ) = 0;

		BOOL		ReadGet (
								PDU_Service *,
								DICOMCommandObject *);
		BOOL		ReadEventReport (
								PDU_Service *,
								DICOMCommandObject *,
								DICOMDataObject *,
								UID *theExpectedSOPInstanceUID );
		BOOL		WriteGet (
								PDU_Service *,
								UID	*SOPInstanceUID,
								DICOMDataObject *,
								AbstractPrintManagementMeta &theManager);

		BOOL		WriteEventReport (
								PDU_Service *,
								UID	*SOPInstanceUID,
								UINT16		EventID,
								DICOMDataObject * );
	};

//
// class BasicPrinter --
// heritage: StandardStatus -> BasicPrinter
//
class	BasicPrinter	:
	public		StandardStatus
	{
	public:
		virtual	BOOL		uGetUID ( UID & );
	};

#if 0
//
// class MetaPrinter --
// Used internally by xMeta classes
// heritage: Printer -> MetaPrinter
//
class	MetaPrinter	:
	public		Printer
	{
	public:
		UID		itsSOPInstanceUID;

		MetaPrinter ();
		virtual	BOOL	GetSOPInstanceUID ( UID & );
		virtual	BOOL	SetSOPInstanceUID ( UID & );
		virtual		BOOL	Get ( UID *, DICOMDataObject * );
		virtual		BOOL	GetStatus ( UID *, DICOMDataObject * );
	};
#endif

//
// class BasicPrintJob -- implement Print Job SOP
// heritage: StandardStatus -> BasicPrintJob
//
class	BasicPrintJob	:
	public	StandardStatus
	{
	public:
		virtual	BOOL		uGetUID ( UID & );
	};

#if 0
// class MetaPrintJob
// Used Internally by xMeta classes
// heritage: PrintJob -> MetaPrintJob
//
class	MetaPrintJob	:
	public	PrintJob
	{
	public:
		UID		itsSOPInstanceUID;

		MetaPrintJob();
		virtual	BOOL	GetSOPInstanceUID ( UID & );
		virtual	BOOL	SetSOPInstanceUID ( UID & );
		virtual		BOOL	Get ( UID *, DICOMDataObject * );
		virtual		BOOL	GetStatus ( UID *, DICOMDataObject * );
	};
#endif


/**************** Data Classes *************************************
 *
 * The purpose of data classes is to generate a semi-structured IOD
 * description.  They are more tuned to how a print job is constructed
 * and linked together than to how any particular IOD is defined.  This
 * ambiguitity is done intentionally (i.e., why they are not CCOC?) because
 * print SOP support is considered a "core" library function, and CCOC
 * is a seperate extended library built atop the core library.
 *
 *******************************************************************/

class AbstractPrinter :
	public DICOMDataObject
{
	public:
		BOOL		itsLock;
		UID			*itsSOPInstanceUID;
		UID			*itsSOPClassUID;
		AbstractPrintManagementMeta *itsManager;

		virtual	BOOL	GetSOPInstanceUID ( UID & );
		virtual	BOOL	SetSOPInstanceUID ( UID & );
		AbstractPrinter (AbstractPrintManagementMeta &theManager);
		virtual	~AbstractPrinter ();
	};
	
class AbstractPrintJob :
	public DICOMDataObject
{
	public:
		BOOL		itsLock;
		UID			*itsSOPInstanceUID;
		UID			*itsSOPClassUID;
		AbstractPrintManagementMeta *itsManager;

		virtual	BOOL	GetSOPInstanceUID ( UID & );
		virtual	BOOL	SetSOPInstanceUID ( UID & );

		BOOL		DeleteAll ();
		BOOL		ProtoGen ();

		AbstractPrintJob (AbstractPrintManagementMeta &theManager);
		virtual	~AbstractPrintJob ();
	};

//
// class AbstractAnnotationBox -- represent an annotation box DICOM object
// heritage: DICOMDataObject -> AbstractAnnotationBox
//

class	AbstractAnnotationBox	:
	public	DICOMDataObject
	{
	public:
		BOOL		itsLock;
		UID			*itsSOPInstanceUID;
		UID			*itsSOPClassUID;
		AbstractPrintManagementMeta *itsManager;

	public:
		BOOL		DeleteAll ();
		BOOL		ProtoGen ();
		AbstractAnnotationBox (AbstractPrintManagementMeta &theManager);
		virtual	~AbstractAnnotationBox ();
	};

//
// class AbstractOverlayBox -- represent an overlay box as a DICOM object
// heritage: DICOMDataObject -> AbstractOverlayBox
//
class	AbstractOverlayBox	:
	public	DICOMDataObject
	{
	public:
		BOOL		itsLock;
		UID			*itsSOPInstanceUID;
		UID			*itsSOPClassUID;
		AbstractPrintManagementMeta *itsManager;
	public:
		BOOL		DeleteAll ();
		BOOL		ProtoGen ();
		virtual	~AbstractOverlayBox ();
		AbstractOverlayBox (AbstractPrintManagementMeta &theManager);
	};
	
//
// class AbstractLUTBox -- represent an LUT box as a DICOM object
// heritage: DICOMDataObject -> AbstractLUTBox
//
class	AbstractLUTBox	:
	public	DICOMDataObject
	{
	public:
		BOOL		itsLock;
		UID			*itsSOPInstanceUID;
		UID			*itsSOPClassUID;
		AbstractPrintManagementMeta *itsManager;
	public:
		BOOL		DeleteAll ();
		BOOL		ProtoGen ();
		virtual	~AbstractLUTBox ();
		AbstractLUTBox (AbstractPrintManagementMeta &theManager);
	};
		
//
// class AbstractImageBox -- represent an image box as a DICOM object
// heritage: DICOMDataObject -> AbstractImageBox
//
class	AbstractImageBox	:
	public	DICOMDataObject
	{
	public:
		Array < AbstractOverlayBox * >		*itsAbstractOverlayBoxs;
		Array < AbstractLUTBox * >			*itsAbstractLUTBoxs;
		BOOL		itsLock;
		UID			*itsSOPInstanceUID;
		UID			*itsSOPClassUID;
		AbstractPrintManagementMeta *itsManager;

		BOOL		ResyncAbstractOverlayBoxs();
		BOOL		ResyncAbstractLUTBoxs();
	public:
		BOOL		DeleteAll ();
		BOOL		ProtoGen ();

		AbstractImageBox(AbstractPrintManagementMeta &theManager);
		virtual	~AbstractImageBox ();

		// add an AbstractOverlayBox to the image box

		AbstractOverlayBox*	Add(AbstractOverlayBox *theOverlayBox);

		// add an AbstractLUTBox to the image box

		AbstractLUTBox*	Add(AbstractLUTBox *theLUTBox);
	};
	
//
// class AbstractFilmBox -- represent a film box as a DICOM object
// heritage: DICOMDataObject -> AbstractFilmBox
//
class	AbstractFilmBox	:
	public	DICOMDataObject
	{
	public:
		Array < AbstractImageBox * >		*itsAbstractImageBoxs;
		Array < AbstractAnnotationBox * >	*itsAbstractAnnotationBoxs;
		BOOL		itsLock;
		UID			*itsSOPInstanceUID;
		UID			*itsSOPClassUID;
		AbstractPrintManagementMeta *itsManager;
		void		SetCreated(BOOL theState)
						{
						itsCreated = theState;
						}

		BOOL		IsCreated()
						{
						return itsCreated;
						}

		BOOL		ResyncAbstractImageBoxs();
		BOOL		ResyncAbstractAnnotationBoxs();
	protected:
		BOOL		itsCreated;
	public:
		BOOL		DeleteAll ();
		BOOL		ProtoGen ();

		AbstractFilmBox (AbstractPrintManagementMeta &theManager);
		virtual	~AbstractFilmBox ();

		BOOL	SetImageBoxInstanceUIDS();
		BOOL	SetAnnotationBoxInstanceUIDS();

		// add an AbstractImageBox to the film box

		AbstractImageBox*	Add(AbstractImageBox *theImageBox);

		// add an AbstractAnnotationBox to the film box

		AbstractAnnotationBox*	Add(AbstractAnnotationBox *theAnnotationBox);
	};
	
//
// class AbstractFilmSession -- represent a film session as a DICOM object
// heritage: DICOMDataObject -> AbstractFilmSession
//

class	AbstractFilmSession	:
	public	DICOMDataObject
	{
	public:

		// contains an array of Film Boxes (each represents a sheet of film)

		Array < AbstractFilmBox * >	*itsAbstractFilmBoxs;

		// contains an array of AbstractPrintJob (one per print job)

		Array < AbstractPrintJob * >	*itsAbstractPrintJobs;

		BOOL		itsLock;
		UID			*itsSOPInstanceUID;
		UID			*itsSOPClassUID;
		BasicFilmSession	itsBasicFilmSession;
		BasicPrinter		itsPrinter;
		BasicPrintJob		itsPrintJob;

		BOOL		ResyncAbstractFilmBoxs();
		virtual BOOL GetNewSOPInstanceUID(UID &theSOPClassUID, UID &theUID);
		void		SetCreated(BOOL theState)
						{
						itsCreated = theState;
						}

		BOOL		IsCreated()
						{
						return itsCreated;
						}

	protected:

		BOOL		itsCreated;	// has session been created on SCP yet ?

	public:

		AbstractFilmSession ();
		virtual	~AbstractFilmSession ();

		BOOL		DeleteAll ();
		BOOL		ProtoGen ();

		// add an AbstractFilmBox to the film session

		AbstractFilmBox*	Add(AbstractFilmBox *theFilmBox);

		// add an Abstract print job to film session's list of print jobs

		AbstractPrintJob*	Add(AbstractPrintJob *thePrintJob);

		// Override these to provide virtual memory space to any of the
		// Image Box, Overlay Box, or Annotation Box objects.

		virtual	BOOL	ActionStore ( DICOMDataObject * )
							{ return ( TRUE ); };
		virtual	BOOL	ActionRetreive ( DICOMDataObject * )
							{ return ( TRUE ); };

	};

// Note Inheritence!! When you create a print job as an SCU (you're a client
// printing to a printer), you want to declare a xPrintManagementClass
// (your own actually, since you will have to stub out some virtual
// functions).  You should never be writing code like:
// new AbstractFilmSession.  (you shouldn't be new'ing any AbstractX
// or StandardX class).
//
// You will however "fill in" the structure defined by AbstractFilmSession
// since that is how the Write () call functions.  It goes through and
// makes sure everything is linked correctly (actually it relinks
// everything and assumes you didn't.. so don't worry about linking).
// It then N-CREATE's and N-SET's everything.  At that point you should
// call Print.  Don't worry about calling Delete since once the
// association is droped, the printer should automatically delete.
//
// Final note, DICOM allows only one instance of a printer class to be
// used per association.  This printer class (both server/client) will
// function along side other instances within the *same* association
// without any problems.  You will have to provide upper layer code to
// enable this break from DICOM anyways, .. you have been warned.


// class AbstractPrintManagementMeta -- base class for printer meta classes
// heritage: AbstractFilmSession -> AbstractPrintManagementMeta

class	AbstractPrintManagementMeta	:
	public		AbstractFilmSession
	{
	public:
		AbstractPrinter			*itsAbstractPrinter;
		BasicFilmBox			itsBasicFilmBox;
		BasicAnnotationBox		itsBasicAnnotationBox;
		ImageOverlayBox			itsImageOverlayBox;
					
		AbstractPrintManagementMeta();
		virtual ~AbstractPrintManagementMeta();

		// Write() -- SCU method
		// Cause creation of currently defined print session
		// called by PrintSession() or PrintFilm() as needed -- probably not
		// needed by SCU user in most cases

		BOOL			Write ( PDU_Service * thePDU );

		// PrintSession() -- SCU method
		// Cause the session to be printed
		// Handles creation of session on SCP if needed
		// Can be called to do first-time print on session,
		// or to reprint the session
		// If caller supplies a pointer-to-UID, a print job
		// SOP Instance UID for the print job created by this
		// action will be returned. If Print Job is not supported
		// by the association, the pointed-to UID will be set to ""

		BOOL			PrintSession ( PDU_Service * thePDU,
							UID *thePrintJobInstanceUID = NULL );

		// PrintFilm() -- SCU method
		// Print the specified film box -- default ( index == 0xffff)
		// is to print the most recently created film box
		// I hope that 32K film boxes is enough per session....
		// Handles creation of session on SCP as needed
		// If caller supplies a pointer-to-UID, a print job
		// SOP Instance UID for the print job created by this
		// action will be returned. If Print Job is not supported
		// by the association, the pointed-to UID will be set to ""

		BOOL			PrintFilm ( PDU_Service * thePDU,
							UID *thePrintJobInstanceUID = NULL,
							UINT16 theFilmBoxIndex = 0xffff );

		// Delete() -- SCU method
		// Delete the session

		BOOL			Delete ( PDU_Service * );

		// DeleteFilm() -- SCU method
		// Delete the most-recently-created film box
		// (actually, we attempt to delete the last film box
		// in the current session -- the printer may allow 'popping'
		// that way, even though the standard doesn't require it)

		BOOL			DeleteFilm ( PDU_Service * );

		virtual	void	HandlePrinterEvent ( DICOMDataObject *theEvent,
			UID& theUID,				// of affected SOP instance
			UINT16 theEventTypeID,
			void *theUserArg ) = 0;

		virtual void *SetPrinterEventUserArg ( void *theArg );

		// HandlePrintJobEvent() -- SCU method
		// HandlePrintJobEvent () -- like HandlePrinterEvent(), but used
		// when N-EVENT-REPORT reporting status of a print job is received.

		virtual	void	HandlePrintJobEvent ( DICOMDataObject *theEvent,
			UID& theUID,				// of affected SOP instance
			UINT16 theEventTypeID,
			void *theUserArg ) = 0;

		virtual void *SetPrintJobEventUserArg ( void *theArg );

		// HandleError() -- SCU method
		// HandleError () -- called when an error occurs during
		// transmission of a print job to the print server
		// argument theError contains a reference to an
		// ErrorDescription which describes the error
		// if the Handler returns TRUE, and the problem was a warning
		// response, the warning will be ignored and processing will
		// continue. In all other cases, the HandleError() method should
		// return FALSE. This allows user to override warning status
		// responses from the SCP.

		virtual BOOL HandleError(const ErrorDescription& theError) const = 0;

		// GetPrinter() -- SCU method
		// GetPrinter () -- used by SCU to retrieve status and other
		// information from the printer. Corresponds to the N-GET operation
		// for the printer SOP class. The DICOMDataObject must contain
		// a list of attributes which SCU is requesting from the printer.
		// Upon TRUE return, the DICOMDataObject will contain the
		// list of attributes actually received from the printer

		virtual	BOOL	GetPrinter ( PDU_Service *thePDU,
			DICOMDataObject *theDDO );

		// GetPrintJob() -- SCU method
		// GetPrintJob () -- used by SCU to retrieve status and other
		// information about a print job. Corresponds to the N-GET operation
		// for the Print Job SOP class. The DICOMDataObject must contain
		// a list of attributes which SCU is requesting from the print job.
		// Upon TRUE return, the DICOMDataObject will contain the
		// list of attributes actually received from the printer

		virtual	BOOL	GetPrintJob (PDU_Service *thePDU,
			UID &thePrintJobInstance,
			DICOMDataObject *theDDO);

		// Methods below here are typically NOT of interest to the
		// average SCU user...

		// Read() -- SCP method

		BOOL			Read ( PDU_Service *, DICOMCommandObject * );

		// GetImageBox() and GetLUTBox() -- return a pointer
		// to a StandardBox or a StandardAdvancedBox object
		// which is actually a pointer to a derived box
		// object of the proper class for boxes in the
		// specific subclassed object type
		// (e.g., if you instantiate a BasicGrayscalePrintManagementMeta
		// object and call its GetImageBox() method, that method
		// should return a pointer to a BasicGrayscaleImageBox object)
		// if the instantiated object does not support VOILUT boxes,
		// GetLUTBox() will return a NULL pointer
		// these must be supplied by a derived class
		// these methods are supplied by BasicGrayscalePrintManagementMeta,
		// BasicColorPrintManagementMeta, etc. and user of those classes
		// need not worry about these

		virtual StandardBox *GetImageBox() = 0;
		virtual StandardAdvancedBox *GetLUTBox() = 0;
	
		// AbstractPrintManagementMeta::ReadEventReport() --
		// while an SCU is looking for any type of input,
		// an asynchronous event report may come in instead
		// of what's expected. This method is called to receive and
		// handle those event reports. Algorithm is to look for
		// a printer status event or a print job status event
		// and dispatch to the appropriate virtual function written by the
		// user (HandlePrinterEvent() or HandlePrintJobEvent())
		// this function is used internally

		BOOL ReadEventReport (PDU_Service *thePDU, DICOMCommandObject *theDCO);

		// HandlePrinterEvent() -- SCU method
		// HandlePrinterEvent() is called when an asynchronous
		// printer status report (N-EVENT-REPORT) is received
		// from the printer. You must override this to handle the event.
		// The status report is contained in the DICOMDataObject argument.

	protected:

		// Used internally.  Might want to override these (calling the
		// base class if you want some sort of extended functionality).

		// ReadBasicFilmSession() -- SCP method

		virtual	BOOL	ReadBasicFilmSession (
								PDU_Service *,
								DICOMCommandObject *,
								UINT16,
								UID	*);

		// ReadBasicFilmBox() -- SCP method

		virtual	BOOL	ReadBasicFilmBox (
								PDU_Service *,
								DICOMCommandObject *,
								UINT16,
								UID	*);
		
		// ActionPrint() -- SCP method

		virtual	BOOL	ActionPrint ( AbstractFilmSession *, DICOMDataObject * );
		virtual	BOOL	AcceptSOPClass ( UID & );

		// You must provide functions for the following:

		// ActionPrint() -- SCU method (provide a stub returning FALSE)
		// ActionPrint() -- SCP method
		// ActionPrint() -- called to print a film box
		// actually does the printing to the output device

		virtual	BOOL	ActionPrint ( AbstractFilmBox *, DICOMDataObject * ) = 0;

		// ReadBox() -- SCP method
		// ReadBox() -- called to read a box specific to the actual
		// subclassed object type
		// this method is supplied by BasicGrayscalePrintManagementMeta,
		// BasicColorPrintManagementMeta, etc. and user of those classes
		// need not worry about these

		virtual	BOOL	ReadBox ( PDU_Service *, DICOMCommandObject *, DICOMDataObject * ) = 0;

		// WriteBox() -- called to write a box specific to the actual
		// subclassed object type
		// e.g. BasicGrayscalePrintManagementMeta object would write a
		// BasicGrayscaleImageBox and ReferencedGrayscalePrintManagementMeta
		// would decide, based upon the DDO, whether to write a
		// ReferencedImageBox or a VOILUTBox and then do that
		// this method is supplied by BasicGrayscalePrintManagementMeta,
		// BasicColorPrintManagementMeta, etc. and user of those classes
		// need not worry about these

		virtual	BOOL	WriteBox ( PDU_Service *, UID *, DICOMDataObject * ) = 0;
		// args to be supplied to user event callbacks
		// use SetPrinterEventUserArg() and SetPrintJobEventUserArg() to
		// manipulate these

		void *itsPrinterEventUserArg;
		void *itsPrintJobEventUserArg;
	};				

// class BasicGrayscalePrintManagementMeta -- represent 
// Basic Grayscale Print Management Meta SOP class
// heritage: DICOMDataObject -> AbstractFilmSession ->
// AbstractPrintManagementMeta -> BasicGrayscalePrintManagementMeta

class	BasicGrayscalePrintManagementMeta	:
	public	AbstractPrintManagementMeta
	{
	private:
		BOOL			ReadBox ( PDU_Service *, DICOMCommandObject *, DICOMDataObject * );
		BOOL			WriteBox ( PDU_Service *, UID *, DICOMDataObject * );

		StandardBox *GetImageBox()
			{
				return (StandardBox *)&itsBasicGrayscaleImageBox;
			};
		StandardAdvancedBox *GetLUTBox()
			{
				return (StandardAdvancedBox *)NULL;
			};

	public:
		BasicGrayscaleImageBox	itsBasicGrayscaleImageBox;
	};

class	BasicColorPrintManagementMeta	:
	public	AbstractPrintManagementMeta
	{
	private:
		BOOL			ReadBox ( PDU_Service *, DICOMCommandObject *, DICOMDataObject * );
		BOOL			WriteBox ( PDU_Service *, UID *, DICOMDataObject * );

		StandardBox *GetImageBox()
			{
				return (StandardBox *)&itsBasicColorImageBox;
			};
		StandardAdvancedBox *GetLUTBox()
			{
				return (StandardAdvancedBox *)NULL;
			};
	public:
		BasicColorImageBox		itsBasicColorImageBox;
	};

class	ReferencedGrayscalePrintManagementMeta	:
	public	AbstractPrintManagementMeta
	{
	private:
		BOOL			ReadBox ( PDU_Service *, DICOMCommandObject *, DICOMDataObject * );
		BOOL			WriteBox ( PDU_Service *, UID *, DICOMDataObject * );

		StandardBox *GetImageBox()
			{
				return (StandardBox *)&itsReferencedImageBox;
			};
		StandardAdvancedBox *GetLUTBox()
			{
				return (StandardAdvancedBox *)&itsVOILUTBox;
			};
	public:
		ReferencedImageBox		itsReferencedImageBox;
		VOILUTBox				itsVOILUTBox;
	};

class	ReferencedColorPrintManagementMeta	:
	public	AbstractPrintManagementMeta
	{
	private:
		BOOL			ReadBox ( PDU_Service *, DICOMCommandObject *, DICOMDataObject * );
		BOOL			WriteBox ( PDU_Service *, UID *, DICOMDataObject * );
		StandardBox *GetImageBox()
			{
				return (StandardBox *)&itsReferencedImageBox;
			};
		StandardAdvancedBox *GetLUTBox()
			{
				return (StandardAdvancedBox *)NULL;
			};
	public:
		ReferencedImageBox		itsReferencedImageBox;
	};
		
#endif
