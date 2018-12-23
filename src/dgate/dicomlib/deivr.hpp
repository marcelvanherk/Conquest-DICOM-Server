/*
19990318    ljz    Added 'DICOMObject::DeleteVR'
20010730    ljz    Added 'DICOMObject::ReplaceVR'
20010802    ljz    Added 'VR::GetSpecialValueCodeOfVR'
20051217    mvh	   Added 'VR::ReAlloc'
20080910    bcb    Added VR::VR(UINT16 g, UINT16 e, UINT32 l, LE_UINT16	*d) &
		   VR::VR(UINT16 g, UINT16 e, UINT32 l, LE_UINT16 *d, BOOL mFlag)
                   for big endian.
20100619    bcb    Fix gcc4 warnings and improve speed,
                   and prevented coping of clases with pointers.
20100717    mvh    Merged
20100728    bcb    Removed unused AutoMakeDO, added Get's to VR and DICOMObject.
                   Added ChangeVR's.
20100815    mvh    Merged; added max to GetCstring
                   Moved defaults from implementation to header files
20100823    mvh    bcb moved default parameter here
20140528    lsp    Member initialization and private copy constructors are not GNUC specific
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


class	DICOMObject;

class	VR
	{
	private:
		int		GetSpecialValueCodeOfVR(VR* pVR);

	public:
		UINT16	Group;
		UINT16	Element;
		UINT32	Length;
		void		*Data;
		BOOL		ReleaseMemory;
		BOOL		Valid;
		// void pointers for historical reasons.  SQObjectArray
		// is problem best moved to an Array < DICOMObject * > * in
		// the future once the various system compilers work around
		// the linking problems...  ReferentialIntegrityDatabase should
		// remain void because it is not defined unless the (optional)
		// IOD library is linked in.
		void		*SQObjectArray;
		void		*ReferentialIntegrityDatabase;
		UINT16		TypeCode;
		VR() : Group(0), Element(0), Length(0), Data(NULL),
			ReleaseMemory(TRUE), Valid(FALSE), SQObjectArray(NULL),
			ReferentialIntegrityDatabase(NULL),TypeCode(0) {};
		VR(UINT16 g, UINT16 e, UINT32 l, BOOL	Alloc);
		VR(UINT16 g, UINT16 e, UINT32 l, LE_UINT16	*d);
		VR(UINT16 g, UINT16 e, UINT32 l, LE_UINT16	*d, BOOL	mFlag);
		VR(UINT16 g, UINT16 e, UINT32 l, void	*d);
		VR(UINT16 g, UINT16 e, UINT32 l, void	*d, BOOL);
		~VR();
		UINT		operator	>	(VR	&vr);
		UINT		operator	<	(VR	&vr);
		UINT		operator	==	(VR	&vr);
		BOOL		ReAlloc (UINT32);
		UINT16		GetUINT16();
		UINT		GetUINT();
		unsigned long long	GetULongLong();
		INT		Getatoi();

		// To support SetIf/Morph

		BOOL		SetIf(VR	*);
//		BOOL		Morph(DICOMObject	*);
		BOOL		Reset ();
	private:// This will prevent it from being copied (it has pointers)
		VR(const VR&);
		VR & operator = (const VR&);
	};

class	VRGroupPQueue	:
	public	PQueueOfPtr<VR	*>
	{
	public:
		UINT16	Group;
		UINT32	Length;
		VR			*Element0;
	public:
		BOOL	Push(VR	*vr);
		VR		*Pop();
		VR		*GroupLength();
		UINT		operator	>	(VRGroupPQueue	&vrgroup);
		UINT		operator	<	(VRGroupPQueue	&vrgroup);
		UINT		operator	==	(VRGroupPQueue	&vrgroup);
		VRGroupPQueue(UINT16	G):Group(G),Length(0),Element0(NULL) {};
	private:// This will prevent it from being copied (it has a pointer)
		VRGroupPQueue(const VRGroupPQueue&);
		VRGroupPQueue & operator = (const VRGroupPQueue&);
	};

class	DICOMObject	:
	public	PQueueOfPtr<VRGroupPQueue *>
	{
		VRGroupPQueue			*CurrentGroup, *LastVRG;
		VR				*LastVR;

		UINT16				LastGroup;
		UINT16				LastElement;

	public:
		UINT32				Length;
		BOOL				Packaged;
		Array<VRGroupPQueue *>		VRGroupPQueues;
		Array<DICOMObject *>		*SQObjectArray;
		UINT32				FreeSQObjectSize;
//		DICOMObject			*AutoMakeDO; //bcb Always NULL.
		UINT32				FixedSize;
		BOOL				UseFixedSize;
	private:
		BOOL				Package();
                // This will prevent it from being copied (it has pointers)
		DICOMObject(const DICOMObject&);
		DICOMObject & operator = (const DICOMObject&);
	public:
		virtual		BOOL		Push(VR	*vr);
		virtual		BOOL		Push(DICOMObject *);	// Sequence Embedding
		virtual		VR		*Pop();
		UINT8				GetBYTE(UINT16, UINT16);
		UINT16				GetUINT16(UINT16, UINT16);
		UINT				GetUINT(UINT16, UINT16);
		unsigned long long		GetULongLong(UINT16, UINT16);
		INT				Getatoi(UINT16, UINT16);
		char 				*GetCString(UINT16, UINT16, size_t max = 1023);
		VR				*GetVR(UINT16, UINT16);
		BOOL				DeleteVR(VR* pVR);
		BOOL				ReplaceVR(VR* pNewVR);
		INT				ChangeVR(UINT16, UINT16, const char *, UINT16 type = 0, BOOL space = FALSE);
		INT				ChangeVR(UINT16, UINT16, UINT8, UINT16);
		INT				ChangeVR(UINT16, UINT16, UINT16, UINT16);
		INT				ChangeVR(UINT16, UINT16, UINT, UINT16);
		virtual	BOOL			Reset();
		DICOMObject();
		~DICOMObject();
	};

class	DICOMCommandObject	:
	public	DICOMObject
	{
	};

class	DICOMDataObject	:
	public	DICOMObject
	{
	};
