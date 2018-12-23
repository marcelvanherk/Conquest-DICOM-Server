/*
20030606	ljz	Added member-variable 'QueryRetrieveLevel' to StandardQuery
20050401	mvh	Added ModalityWorkListQuery
20051222	mvh	Added pADDO to Query::Write method - answer appended to pADDO if set
20090205	mvh	Added QueryMoveScript
20100816	mvh	Added QueryResultScript
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

// Q/R SOP Class

// Abstract Base Class for Q/R


class	StandardQuery	:
	public	CFindRQ,
	public	CFindRSP
	{
	public:
		char	QueryRetrieveLevel[10];
	public:
		virtual	BOOL	GetUID (UID	&uid) { return (uGetUID(uid)); };
		virtual	BOOL	uGetUID ( UID &) = 0;
		virtual	BOOL	QueryMoveScript (PDU_Service*, 	DICOMCommandObject*, DICOMDataObject	*) = 0;

		virtual	BOOL	QueryResultScript (PDU_Service*, 	DICOMCommandObject*, DICOMDataObject	*) = 0;

		virtual	BOOL	SearchOn (	DICOMDataObject	*,
									Array < DICOMDataObject  *> 	*) = 0;
		virtual	BOOL	CallBack ( 	DICOMCommandObject	*,
									DICOMDataObject * ) = 0;
		BOOL	Read (	PDU_Service	*,
						DICOMCommandObject	*);
		BOOL	Write (	PDU_Service	*,
						DICOMDataObject	*, 	Array < DICOMDataObject  *> 	*);
						
	};

class	StandardRetrieve	:
	public	CMoveRQ,
	public	CMoveRSP
	{
	public:
		virtual	BOOL	GetUID (UID	&uid) { return (uGetUID(uid)); };
		virtual	BOOL	uGetUID ( UID &) = 0;
		virtual	BOOL	QueryMoveScript (PDU_Service*, 	DICOMCommandObject*, DICOMDataObject	*) = 0;
		virtual	BOOL	SearchOn ( DICOMDataObject	*,
								Array < DICOMDataObject *> *) = 0;
		virtual	BOOL	RetrieveOn (	DICOMDataObject *,
										DICOMDataObject **,
										StandardStorage	**) = 0;
		virtual	BOOL	QualifyOn ( BYTE *, BYTE *, BYTE *, BYTE * ) = 0;
		virtual	BOOL	CallBack (	DICOMCommandObject	*,
									DICOMDataObject	* ) = 0;
		BOOL	Read (	PDU_Service *, DICOMCommandObject	* );
		BOOL	Write ( PDU_Service	*, DICOMDataObject	*, BYTE	*);
	};

class	PatientRootQuery	:
			public	StandardQuery
	{
	public:
		BOOL	GetUID ( UID & );
		BOOL	uGetUID ( UID &uid ) { return ( GetUID(uid) ); };

		inline	BOOL	Read (	PDU_Service	*PDU,
								DICOMCommandObject	*DCO )
			{ return ( StandardQuery :: Read ( PDU, DCO ) ); };

		inline	BOOL	Write (	PDU_Service	*PDU,
						DICOMDataObject	*DDO, Array < DICOMDataObject  *> *pADDO)
			{ return ( StandardQuery :: Write ( PDU, DDO, pADDO ) ); };
	};

class	PatientRootRetrieve	:
			public	StandardRetrieve
	{
	public:
		BOOL	GetUID ( UID & );
		BOOL	uGetUID ( UID &uid ) { return ( GetUID(uid) ); };

		inline	BOOL	Read (	PDU_Service	*PDU,
								DICOMCommandObject	*DCO )
			{ return ( StandardRetrieve :: Read ( PDU, DCO ) ); };

		BOOL	Write (	PDU_Service	*PDU,
						DICOMDataObject	*DDO,
						BYTE	*ACRNema)
			{ return ( StandardRetrieve :: Write ( PDU,
						DDO, ACRNema )); };
	};

class	StudyRootQuery	:
			public	StandardQuery
	{
	public:
		BOOL	GetUID ( UID & );
		BOOL	uGetUID ( UID &uid ) { return ( GetUID(uid) ); };

		inline	BOOL	Read (	PDU_Service	*PDU,
								DICOMCommandObject	*DCO )
			{ return ( StandardQuery :: Read ( PDU, DCO ) ); };

		inline	BOOL	Write (	PDU_Service	*PDU,
						DICOMDataObject	*DDO, Array < DICOMDataObject  *> *pADDO)
			{ return ( StandardQuery :: Write ( PDU, DDO, pADDO ) ); };
	};

class	StudyRootRetrieve	:
			public	StandardRetrieve
	{
	public:
		BOOL	GetUID ( UID & );
		BOOL	uGetUID ( UID &uid ) { return ( GetUID(uid) ); };
		
		inline	BOOL	Read (	PDU_Service	*PDU,
								DICOMCommandObject	*DCO )
			{ return ( StandardRetrieve :: Read ( PDU, DCO ) ); };

		BOOL	Write (	PDU_Service	*PDU,
						DICOMDataObject	*DDO,
						BYTE	*ACRNema)
			{ return ( StandardRetrieve :: Write ( PDU,
						DDO, ACRNema )); };
	};

class	PatientStudyOnlyQuery	:
			public	StandardQuery
	{
	public:
		BOOL	GetUID ( UID & );
		BOOL	uGetUID ( UID &uid ) { return ( GetUID(uid) ); };


		inline	BOOL	Read (	PDU_Service	*PDU,
								DICOMCommandObject	*DCO )
			{ return ( StandardQuery :: Read ( PDU, DCO ) ); };

		inline	BOOL	Write (	PDU_Service	*PDU,
						DICOMDataObject	*DDO, Array < DICOMDataObject  *> *pADDO)
			{ return ( StandardQuery :: Write ( PDU, DDO, pADDO ) ); };
	};

class	PatientStudyOnlyRetrieve	:
			public	StandardRetrieve
	{
	public:
		BOOL	GetUID ( UID & );
		BOOL	uGetUID ( UID &uid ) { return ( GetUID(uid) ); };
		
		inline	BOOL	Read (	PDU_Service	*PDU,
								DICOMCommandObject	*DCO )
			{ return ( StandardRetrieve :: Read ( PDU, DCO ) ); };

		BOOL	Write (	PDU_Service	*PDU,
						DICOMDataObject	*DDO,
						BYTE	*ACRNema)
			{ return ( StandardRetrieve :: Write ( PDU,
						DDO, ACRNema )); };
	};

class	ModalityWorkListQuery	:
			public	StandardQuery
	{
	public:
		BOOL	GetUID ( UID & );
		BOOL	uGetUID ( UID &uid ) { return ( GetUID(uid) ); };


		inline	BOOL	Read (	PDU_Service	*PDU,
								DICOMCommandObject	*DCO )
			{ return ( StandardQuery :: Read ( PDU, DCO ) ); };

		inline	BOOL	Write (	PDU_Service	*PDU,
						DICOMDataObject	*DDO, Array < DICOMDataObject  *> *pADDO)
			{ return ( StandardQuery :: Write ( PDU, DDO, pADDO ) ); };
	};
