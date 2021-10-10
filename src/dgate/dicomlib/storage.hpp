// mvh 20030922 Added PDU_Service to CheckObject call
// mvh 20050108 Fixed for linux compile
// mvh 20070314	Allow send of 0000,1030 (MoveOriginatorAE) and 0000,1031 (MoveOriginatorMessageID) in StandardStorage : write
// mvh 20200121	Added DCO to CheckObject
// mvh 20210425 Allow own DCOR to be passed on StandardStorage::Write

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

// Storage classes

// Note there are many "ungood" things about the class hierachys in
// this file.  Notable things should be protected rather than public,
// there should be no inline "wrapper"'s..  Both of these are done
// to be compatible with gcc.

// Base Storage class

class	StandardStorage	:
	public	CStoreRQ,
	public	CStoreRSP
	{
	public:
		BOOL	GetUID ( UID &uid) { return (uGetUID(uid)); };
		virtual	BOOL	uGetUID ( UID &uid ) = 0;
		virtual	UINT16	CheckObject(DICOMCommandObject *, DICOMDataObject *, PDU_Service *) { return ( 0 ); };
		BOOL	Read (	PDU_Service *,
						DICOMCommandObject *, 
						DICOMDataObject * );
		BOOL	Write (	PDU_Service	*, DICOMDataObject	*, VR *MoveMessageID = NULL, unsigned char *CallingAE = NULL );
		BOOL	Write (	PDU_Service	*, DICOMDataObject	*, VR *MoveMessageID, unsigned char *CallingAE, DICOMCommandObject *);
	};

class	UnknownStorage	:
	public	CStoreRQ,
	public	CStoreRSP
	{
	public:
		BOOL	GetUID(UID &);
		virtual	UINT16	CheckObject(DICOMCommandObject *, DICOMDataObject *, PDU_Service *) { return ( 0 ); };

		BOOL	Read (	PDU_Service	*,
					DICOMCommandObject	*,
					DICOMDataObject	* );
	};

class	CRStorage	:
	public	StandardStorage
	{
	public:
		BOOL	GetUID(UID &);
		BOOL	uGetUID(UID &uid) { return ( GetUID(uid) ); };
		inline	BOOL Read ( PDU_Service *PDU, DICOMCommandObject *DCO,
					DICOMDataObject *DDO )
			{ return ( StandardStorage :: Read ( PDU, DCO, DDO ) ); };
		inline	BOOL Write ( PDU_Service *PDU, DICOMDataObject *DDO)
			{ return ( StandardStorage :: Write ( PDU, DDO ) ); };
	};

class	CTStorage	:
	public	StandardStorage
	{
	public:
		BOOL	GetUID(UID &);
		BOOL	uGetUID(UID &uid) { return ( GetUID(uid) ); };
		inline	BOOL Read ( PDU_Service *PDU, DICOMCommandObject *DCO,
					DICOMDataObject *DDO )
			{ return ( StandardStorage :: Read ( PDU, DCO, DDO ) ); };
		inline	BOOL Write ( PDU_Service *PDU, DICOMDataObject *DDO)
			{ return ( StandardStorage :: Write ( PDU, DDO ) ); };
	};

class	USMultiframeStorage	:
	public	StandardStorage
	{
	public:
		BOOL	GetUID(UID &);
		BOOL	uGetUID(UID &uid) { return ( GetUID(uid) ); };
		inline	BOOL Read ( PDU_Service *PDU, DICOMCommandObject *DCO,
					DICOMDataObject *DDO )
			{ return ( StandardStorage :: Read ( PDU, DCO, DDO ) ); };
		inline	BOOL Write ( PDU_Service *PDU, DICOMDataObject *DDO)
			{ return ( StandardStorage :: Write ( PDU, DDO ) ); };
	};

class	MRStorage	:
	public	StandardStorage
	{
	public:
		BOOL	GetUID(UID &);
		BOOL	uGetUID(UID &uid) { return ( GetUID(uid) ); };
		inline	BOOL Read ( PDU_Service *PDU, DICOMCommandObject *DCO,
					DICOMDataObject *DDO )
			{ return ( StandardStorage :: Read ( PDU, DCO, DDO ) ); };
		inline	BOOL Write ( PDU_Service *PDU, DICOMDataObject *DDO)
			{ return ( StandardStorage :: Write ( PDU, DDO ) ); };
	};

class	NMStorage	:
	public	StandardStorage
	{
	public:
		BOOL	GetUID(UID &);
		BOOL	uGetUID(UID &uid) { return ( GetUID(uid) ); };
		inline	BOOL Read ( PDU_Service *PDU, DICOMCommandObject *DCO,
					DICOMDataObject *DDO )
			{ return ( StandardStorage :: Read ( PDU, DCO, DDO ) ); };
		inline	BOOL Write ( PDU_Service *PDU, DICOMDataObject *DDO)
			{ return ( StandardStorage :: Write ( PDU, DDO ) ); };
	};

class	USStorage	:
	public	StandardStorage
	{
	public:
		BOOL	GetUID(UID &);
		BOOL	uGetUID(UID &uid) { return ( GetUID(uid) ); };
		inline	BOOL Read ( PDU_Service *PDU, DICOMCommandObject *DCO,
					DICOMDataObject *DDO )
			{ return ( StandardStorage :: Read ( PDU, DCO, DDO ) ); };
		inline	BOOL Write ( PDU_Service *PDU, DICOMDataObject *DDO)
			{ return ( StandardStorage :: Write ( PDU, DDO ) ); };
	};

class	SCStorage	:
	public	StandardStorage
	{
	public:
		BOOL	GetUID(UID &);
		BOOL	uGetUID(UID &uid) { return ( GetUID(uid) ); };
		inline	BOOL Read ( PDU_Service *PDU, DICOMCommandObject *DCO,
					DICOMDataObject *DDO )
			{ return ( StandardStorage :: Read ( PDU, DCO, DDO ) ); };
		inline	BOOL Write ( PDU_Service *PDU, DICOMDataObject *DDO)
			{ return ( StandardStorage :: Write ( PDU, DDO ) ); };
	};

class	StandaloneOverlayStorage	:
	public	StandardStorage
	{
	public:
		BOOL	GetUID(UID &);
		BOOL	uGetUID(UID &uid) { return ( GetUID(uid) ); };
		inline	BOOL Read ( PDU_Service *PDU, DICOMCommandObject *DCO,
					DICOMDataObject *DDO )
			{ return ( StandardStorage :: Read ( PDU, DCO, DDO ) ); };
		inline	BOOL Write ( PDU_Service *PDU, DICOMDataObject *DDO)
			{ return ( StandardStorage :: Write ( PDU, DDO ) ); };
	};

class	StandaloneCurveStorage	:
	public	StandardStorage
	{
	public:
		BOOL	GetUID(UID &);
		BOOL	uGetUID(UID &uid) { return ( GetUID(uid) ); };
		inline	BOOL Read ( PDU_Service *PDU, DICOMCommandObject *DCO,
					DICOMDataObject *DDO )
			{ return ( StandardStorage :: Read ( PDU, DCO, DDO ) ); };
		inline	BOOL Write ( PDU_Service *PDU, DICOMDataObject *DDO)
			{ return ( StandardStorage :: Write ( PDU, DDO ) ); };
	};

class	StandaloneModalityLUTStorage	:
	public	StandardStorage
	{
	public:
		BOOL	GetUID(UID &);
		BOOL	uGetUID(UID &uid) { return ( GetUID(uid) ); };
		inline	BOOL Read ( PDU_Service *PDU, DICOMCommandObject *DCO,
					DICOMDataObject *DDO )
			{ return ( StandardStorage :: Read ( PDU, DCO, DDO ) ); };
		inline	BOOL Write ( PDU_Service *PDU, DICOMDataObject *DDO)
			{ return ( StandardStorage :: Write ( PDU, DDO ) ); };
	};

class	StandaloneVOILUTStorage	:
	public	StandardStorage
	{
	public:
		BOOL	GetUID(UID &);
		BOOL	uGetUID(UID &uid) { return ( GetUID(uid) ); };
		inline	BOOL Read ( PDU_Service *PDU, DICOMCommandObject *DCO,
					DICOMDataObject *DDO )
			{ return ( StandardStorage :: Read ( PDU, DCO, DDO ) ); };
		inline	BOOL Write ( PDU_Service *PDU, DICOMDataObject *DDO)
			{ return ( StandardStorage :: Write ( PDU, DDO ) ); };
	};


class	GEMRStorage	:
	public	StandardStorage
	{
	public:
		BOOL	GetUID(UID &);
		BOOL	uGetUID(UID &uid) { return ( GetUID(uid) ); };
		inline	BOOL Read ( PDU_Service *PDU, DICOMCommandObject *DCO,
					DICOMDataObject *DDO )
			{ return ( StandardStorage :: Read ( PDU, DCO, DDO ) ); };
		inline	BOOL Write ( PDU_Service *PDU, DICOMDataObject *DDO)
			{ return ( StandardStorage :: Write ( PDU, DDO ) ); };
	};


class	GECTStorage	:
	public	StandardStorage
	{
	public:
		BOOL	GetUID(UID &);
		BOOL	uGetUID(UID &uid) { return ( GetUID(uid) ); };
		inline	BOOL Read ( PDU_Service *PDU, DICOMCommandObject *DCO,
					DICOMDataObject *DDO )
			{ return ( StandardStorage :: Read ( PDU, DCO, DDO ) ); };
		inline	BOOL Write ( PDU_Service *PDU, DICOMDataObject *DDO)
			{ return ( StandardStorage :: Write ( PDU, DDO ) ); };
	};


