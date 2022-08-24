/*
19990202    ljz    Added PDU memberfunction 'IsAbstractSyntaxAccepted'
20020316    mvh    Added GetLocalAddress and GetRemoteAddress
20030424    ljz    Changedprototype of ParseDCMIntoRawVR
20030627    mvh    Fix for MSC4.2
20030905    mvh    Removed spurious ;
20080816    mvh	   ValidPresContexts is now public: how many incoming connection can handle 
20080819    mvh	   CanYouHandleTransferSyntax is now public; use BOOL not bool
20091231    bcb    Changed char* to const char* for gcc4.2 warnings
20100111    mvh    Merged
20100619    bcb    Fix gcc4 warnings and improve speed,
                   and prevented copying of the PDU_Service class with pointers.
20100723    mvh    Merged
20101120    mvh    bcb added parameter 
20110320    mvh    Rollback of dicom decoder (parameter) to 1.4.16beta4 version
20110326    mvh    Put it back again
20130619    lsp    Made no-copy generic (not only for __GNUC__)
20140528    lsp    Private copy constructor and member initialization are not GNUC specific
20140607    mvh    Added PDU_Service::Write(DICOMCommandObject	*DCMObject, UID	&uid, BYTE PCid)
20140610    mvh    Merged
20160827    mvh    PDU size configurable 
20220824    mvh    Added LoadDICOMDataObjectTrunc
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

/***********************************************************************
 *
 *
 ***********************************************************************/
#define	DEFAULT_ACCEPTANCE	TRUE


// These are Read/Write file types
#	define	ACRNEMA_VR_DUMP				1
#	define	DICOM_CHAPTER_10_IMPLICIT	2
#	define	DICOM_CHAPTER_10_EXPLICIT	3

// These are "Transfer Syntax Alias" types.
enum
	{
	TSA_IMPLICIT_LITTLE_ENDIAN = 100,
	TSA_EXPLICIT_LITTLE_ENDIAN,
	TSA_EXPLICIT_BIG_ENDIAN
	};

class	FileBuffer	:
	public	LinkedBuffer
	{
	public:
		FILE	*fp;
		BOOL	OpenForRead ( char	*filename );
		BOOL	OpenForWrite ( char *filename );
		BOOL	Close ();
		INT		ReadBinary(BYTE *Data, UINT Count);
		BOOL	SendBinary(BYTE *Data, UINT Count);
		FileBuffer():fp(NULL) {};
	private:// This will prevent it from being copied (it has pointers)
		FileBuffer(const FileBuffer&);
		const	FileBuffer & operator = (const FileBuffer&);
	};

class	PDU_Service	:
	public	Buffer,			// Transport Buffer
	public	AAssociateRQ,	// Associate Request
	public	AAssociateAC,	// Associate Accept
	public	AAssociateRJ,	// Associate Reject
	public	AReleaseRQ,		// Associate Release
	public	AReleaseRP,		// Associate Release Response
	public	AAbortRQ,		// Associate Abort Request
	public	PDataTF			// P-DATA Service
	{
	protected:
		Array<AbstractSyntax>			ProposedAbstractSyntaxs;
		Array<PresentationContextAccept>	AcceptedPresentationContexts;
		UINT32					SizeCap;
		RTC					*AttachedRTC;
		BOOL					DestructAttachedRTC;
		UID					UsedTransferSyntaxUID;
	protected:
		BOOL			InterogateAAssociateRQ();
		BOOL			InterogateAAssociateAC();
		virtual	BOOL	ShouldIAcceptRemoteApTitle(BYTE	*)
			{ return ( DEFAULT_ACCEPTANCE ); };
		virtual	BOOL	ShouldIAcceptLocalApTitle(BYTE *)
			{ return ( DEFAULT_ACCEPTANCE ); };
		virtual	BOOL	ShouldIAcceptApplicationContext(ApplicationContext &)
			{ return ( DEFAULT_ACCEPTANCE ); };
		virtual	BOOL	ShouldIAcceptPresentationContext(
							PresentationContext &,
							PresentationContextAccept &)
			{ return ( DEFAULT_ACCEPTANCE ); };
		virtual	BOOL	ShouldIAcceptAbstractSyntax(AbstractSyntax &)
			{ return ( DEFAULT_ACCEPTANCE ); };
		virtual	BOOL	AddTransferSyntaxs(PresentationContext &);
		virtual	BOOL	GetImplementationClass(ImplementationClass &);
		virtual	BOOL	GetImplementationVersion(ImplementationVersion &);

		// Entry Point Functions
		virtual	BOOL	ParseRawVRIntoDCM(BYTE, LinkedBuffer &, DICOMObject *);
		virtual	BYTE	ParseDCMIntoRawVR(DICOMObject *, LinkedBuffer &);
//		virtual	BYTE	ParseDCMIntoRawVR(DICOMObject *, LinkedBuffer &, UID &);
		virtual	BYTE	ParseDCMIntoRawVR(DICOMObject *, LinkedBuffer &, UID &, BOOL bIsDataObject = TRUE);

		// Implicit Encoders
		virtual	BOOL	Implicit_ParseRawVRIntoDCM(
							LinkedBuffer &, DICOMObject *, BOOL bZeroTaken);
//							LinkedBuffer &, DICOMObject *, BOOL bZeroTaken = FALSE);
		virtual	BOOL	Implicit_ParseDCMIntoRawVR
							(DICOMObject *, LinkedBuffer &);
		virtual	BOOL	ImplicitLittleEndian_ParseRawVRIntoDCM(
							LinkedBuffer &, DICOMObject *);
		virtual	BOOL	ImplicitLittleEndian_ParseDCMIntoRawVR
							(DICOMObject *, LinkedBuffer &);

		// Explicit Decoders
		virtual	BOOL	Explicit_ParseRawVRIntoDCM(
							LinkedBuffer &, DICOMObject *);
		virtual	BOOL	Explicit_ParseDCMIntoRawVR
							(DICOMObject *, LinkedBuffer &);
		virtual	BOOL	ExplicitLittleEndian_ParseRawVRIntoDCM(
							LinkedBuffer &, DICOMObject *);
		virtual	BOOL	ExplicitLittleEndian_ParseDCMIntoRawVR
							(DICOMObject *, LinkedBuffer &);
		virtual	BOOL	ExplicitBigEndian_ParseRawVRIntoDCM(
							LinkedBuffer &, DICOMObject *);
		virtual	BOOL	ExplicitBigEndian_ParseDCMIntoRawVR
							(DICOMObject *, LinkedBuffer &);

		// Used to Read DICOM Files (all types)
		virtual	BOOL	Dynamic_ParseRawVRIntoDCM
							(LinkedBuffer &, DICOMObject *, UINT StartMode);

		// Used to write Chapter 10 Explcit (new) files
		virtual	BOOL	Dynamic_ParseDCMIntoRawVR
							(DICOMObject *, LinkedBuffer &, UINT StartMode);

		// Image Pixel Data encapsulation routines

		virtual	BOOL	ParseImagePixelDataDCMToRaw
							(Array<DICOMDataObject*>	*, LinkedBuffer &);
		virtual	BOOL	ParseImagePixelDataRawToDCM
							(LinkedBuffer	&, DICOMObject	*);

		// Support Routines for Transfer Syntax Encoding/Decoding
	public:	// The following routine is usefull to check for a valid
			// Accepted Presentation Context UID
		UINT	ValidPresContexts;
		virtual	BOOL	CanYouHandleTransferSyntax(TransferSyntax &);
		BYTE	GetAcceptedPCIDForTransferSyntax(UID &TrnUID);
		BOOL	GetTransferSyntaxUID(BYTE, UID &TrnUID);
		char*	GetTransferSyntaxUID(UINT);
		BYTE	GetPresentationContextID(UID &AbsUID);
		BYTE	GetPresentationContextID(UID &AbsUID, UID &TrnUID);
		BOOL 	IsAbstractSyntaxAccepted(UID &uid);
		void	GetUsedTransferSyntax(UID &uid){ uid = UsedTransferSyntaxUID;};

	public:
		Socket	Link;
		int	pdusize;

		// RTC Support (optional, helps transfer syntax engines)
				BOOL	AttachRTC(RTC *, BOOL	Destruct = FALSE);
				RTC		*DetachRTC();
		// Wrap data over from the Associate/PData Classes over to the buffer
				BOOL	Read(BYTE	*Data, UINT	Count)
			{	return ( Buffer :: Read ( Data, Count ) ); };
				BOOL	Write(BYTE	*Data, UINT	Count)
			{	return ( Buffer :: Write ( Data, Count ) ); };
				BOOL	Kill(UINT	Count)
			{	return ( Buffer :: Kill ( Count ) ); };
				BOOL	Flush()
			{	return ( Buffer :: Flush () ); };
		// Wrap data over from the buffer over to the socket
				INT	ReadBinary(BYTE *Data, UINT Count)
			{	return ( Link.ReadBinary(Data, Count) ); };
				BOOL	SendBinary(BYTE *Data, UINT Count)
			{	return ( Link.SendBinary(Data, Count) ); };

		virtual	BOOL	MakeChapter10(DICOMDataObject *,const char* pszTransferSyntaxUID = "1.2.840.10008.1.2");
		DICOMDataObject	*LoadDICOMDataObject ( char	* );
		DICOMDataObject	*LoadDICOMDataObjectTrunc ( char	*,  UINT );
		BOOL	SaveDICOMDataObject ( char	*, UINT, DICOMDataObject	* );
		BOOL	ClearAbstractSyntaxs();
		virtual	BOOL	AddAbstractSyntax(UID &);
		virtual	BOOL	AddAbstractSyntaxAlias(UID &Source, UID &Alias);
		virtual	BOOL	SetApplicationContext(UID &);
		virtual	BOOL	SetLocalAddress(BYTE *);
		virtual	BOOL	SetRemoteAddress(BYTE *);
		virtual	BOOL	GetLocalAddress(BYTE *);
		virtual	BOOL	GetRemoteAddress(BYTE *);
		BOOL	Connect(BYTE *, BYTE *);
		
		BOOL	Close();

		BOOL	Listen(BYTE *);

		BOOL	Read(DICOMObject	*);
		BOOL	Write(DICOMCommandObject	*);
		BOOL	Write(DICOMDataObject	*);
		BOOL	Write(DICOMCommandObject	*, UID	&);
		BOOL	Write(DICOMCommandObject	*, UID	&, BYTE);
		BOOL	Write(DICOMDataObject	*, UID	&);
		BOOL	Multiplex(int);

		int		Multiplex(BYTE *);
		int		Listen(int);
		BOOL	SetTimeOut(int TimeOut)
			{ return ( Link.SetTimeOut(TimeOut) ); };
				PDU_Service();
		virtual	~PDU_Service();
	private:// This will prevent it from being copied (it has pointers)
		PDU_Service(const PDU_Service&);
		const	PDU_Service & operator = (const PDU_Service&);
	};


