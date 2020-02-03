/*
19990318    ljz    Added 'DICOMObject::DeleteVR'
20001106    mvh    Used delete [] operator for vr->Data
20010426    ljz    Fixed small leaks in 'DICOMObject::DeleteVR'
20010730    ljz    Added 'DICOMObject::ReplaceVR'. Is able to replace VRs inside
                   sequences.
20010802    ljz    Added 'VR::GetSpecialValueCodeOfVR'
20011121    ljz    Added VR_SPECIAL_RGB_TO_MONO. Used by a conversion-tool to change
                   ultrasound Kretz RGB images to monochrome that VariSeed (brachytherapy)
                   understands.
20011121    ljz    Added VR_SPECIAL_RGB_TO_MONO_PLUS. Same as above, but tries to preserve
                   colored drawings.
20051217    mvh    Added 'VR::ReAlloc'
20051217    mvh    Use malloc/free instead of new char[] and delete []
20051229    mvh    Removed unnecessary reset() in DeleteVR
20071031    mvh    Temp printf memory error messages
20071102    mvh    Send allocation errors to DicomError
20080313    ljz    Removed some warnings
20080910    bcb    Added VR::VR(UINT16 g, UINT16 e, UINT32 l, LE_UINT16	*d) &
                   VR::VR(UINT16 g, UINT16 e, UINT32 l, LE_UINT16 *d, BOOL mFlag)
                   for big endian.
20081016    mvh    Fix for WC: new of 0 gives NULL (check Length in error message)
20090620    jf     Added <stdlib.h>
20091230    mvh    Merged bcb bigendian change
20100110    mvh    And const change
20100309    bcb    Added double parentheses (gcc4.2 Warnings)
20100619    bcb    Fix gcc4 warnings and improve speed and added questions.
20100717    mvh    Merged except keep default ReleaseMemory to TRUE and check data when adjusitn length
20100728    bcb    Removed unused AutoMakeDO, added Get's to VR and DICOMObject.
                   Added ChangeVR's.  Fixed LastVR & LastVRG?
20100815    mvh    Merged; fix VR constructors for failed malloc; added max parameter to GetCstring function
                   Moved defaults from implementation to header files
20100826    mvh    bcb moved another default, size_t
20100901    mvh    bcb changed alloc of US to 2
20110231    mvh    DicomError returns FALSE when handler not installed - to allow some errors to pass
20140512    ea     Updated the implementation of VR* DICOMObject::GetVR()
20140528    lsp    Kept member initialization only in constructors: not GNUC specific
20200203    mvh	   lsp removed DataPointer stuff to support lsp array change
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
#       include <stdlib.h> //DUCKHEAD92

/********************************
 *
 * Error handler
 *
 ********************************/

DeepErrorCall DicomErrorHandler = NULL;

void SetDicomErrorHandler(DeepErrorCall handler)
	{ 
	DicomErrorHandler = handler;
	}

BOOL DicomError(int error, const char *message, int info)
	{ 
	if (DicomErrorHandler)
		{
		DicomErrorHandler(error, message, info);
		return TRUE;
		}
	else
		{
		printf("DICOM ERROR ");
		printf(message, info);
		return FALSE;
		}
	}

/********************************
 *
 * Base VR Type
 *
 ********************************/
VR	::	VR(UINT16 g, UINT16 e, UINT32 l, BOOL	Alloc)
  : Group(g), Element(e), Length(l), Data(NULL), ReleaseMemory(TRUE),
    Valid(FALSE), SQObjectArray(NULL), ReferentialIntegrityDatabase(), TypeCode(0)
{
	BOOL	cLast = FALSE;

	if ( Length & 0x01 )
	{
		++Length;
		cLast = TRUE;
	}
	
	if(Alloc && Length)
	{
		Data = malloc(Length);
		if (!Data) DicomError(DCM_ERROR_MEMORY, "VR: out of memory allocating %d bytes\n", Length);
		if ( Data && cLast ) ((BYTE*)Data)[Length-1] = '\0';
	}
}

VR	::	VR(UINT16 g, UINT16 e, UINT32 l, LE_UINT16	*d)
  : Group(g), Element(e), Length(l), Data(d), ReleaseMemory(TRUE),
    Valid(FALSE), SQObjectArray(NULL), ReferentialIntegrityDatabase(), TypeCode(0)
{
#if NATIVE_ENDIAN == BIG_ENDIAN //Big Endian like Apple power pc
	*d = SwitchEndian((UINT16)*d);
#endif //BigEndian
}

VR	::	VR(UINT16 g, UINT16 e, UINT32 l, LE_UINT16	*d, BOOL	mFlag)
  : Group(g), Element(e), Length(l), Data(d), ReleaseMemory(mFlag),
    Valid(FALSE), SQObjectArray(NULL), ReferentialIntegrityDatabase(), TypeCode(0)
{
#if NATIVE_ENDIAN == BIG_ENDIAN //Big Endian like Apple power pc
	*d = SwitchEndian((UINT16)*d);
#endif //BigEndian
}

VR	::	VR(UINT16 g, UINT16 e, UINT32 l, void	*d)
  : Group(g), Element(e), Length(l), Data(d), ReleaseMemory(TRUE),
    Valid(FALSE), SQObjectArray(NULL), ReferentialIntegrityDatabase(), TypeCode(0)
{
}

VR	::	VR(UINT16 g, UINT16 e, UINT32 l, void	*d, BOOL	mFlag)
  : Group(g), Element(e), Length(l), Data(d), ReleaseMemory(mFlag),
    Valid(FALSE), SQObjectArray(NULL), ReferentialIntegrityDatabase(), TypeCode(0)
{
}

VR	::	~VR()
{
	Array<DICOMObject	*>	*ArrayDO;
/*
	printf("~VR()\n\tGroup = %4.4x Element = %4.4x Data = %x Length = %d\n",
		Group, Element, Data, Length);
	printf("\tReleasememory = %d, SQObjectArray = %x, RID = %x Valid = %d\n",
		ReleaseMemory, SQObjectArray, ReferentialIntegrityDatabase, Valid);
	fflush(stdout);
*/
	if(ReleaseMemory)
		if(Data)
//			delete [] Data;
			free(Data);
	if(SQObjectArray)
	{
		ArrayDO = (Array<DICOMObject *> *) SQObjectArray;
		while(ArrayDO->GetSize())
		{
			delete ArrayDO->Get(0);
			ArrayDO->RemoveAt(0);
		}
		delete ArrayDO;
		SQObjectArray = NULL;
	}		
}

UINT	VR	::	operator	>	(VR	&vr)
{
	if(Group > vr.Group)
		return ( 1 );
	if(Group == vr.Group)
	{
		if(Element > vr.Element)
			return ( 1 );
	}
	return ( 0 );
}

UINT	VR	::	operator	<	(VR	&vr)
{
	if(Group < vr.Group)
		return ( 1 );
	if(Group == vr.Group)
	{
		if(Element < vr.Element)
			return ( 1 );
	}
	return ( 0 );
}

UINT	VR	::	operator	==	(VR	&vr)
{
	if(Group == vr.Group)
	if(Element == vr.Element)
			return ( 1 );
	return ( 0 );
}

/*****************************************************
 *
 * VR Group P-Queues
 *
 *****************************************************/

BOOL	VRGroupPQueue	::	Push(VR	*vr)
{
//	printf("[VRGroupPQueue : Push(%x)]\n", vr);
	if(vr->Group!=Group)
		return ( FALSE );
	Length += vr->Length + sizeof(UINT16) + sizeof(UINT16) + sizeof(UINT32);
	if(vr->Length&0x01)
		++Length;	// Transfer Syntax Encodeder will +1 here
	PQueueOfPtr<VR *> :: Push ( vr );
	return ( TRUE );
}

VR	*	VRGroupPQueue	::	Pop()
{
	VR	*tv;

//	printf("[VRGroupPQueue : Pop()]\n");
	if ( !PQueueOfPtr<VR *> :: GetSize())
		return ( (VR *) NULL );
	tv = PQueueOfPtr<VR *> :: Pop ();
	if(!tv)
		return ( tv );
	Length -= (tv->Length + sizeof(UINT16) + sizeof(UINT16) + sizeof(UINT32));
	if(tv->Length&0x01)
		--Length;	// See Push
	return ( tv );
}

VR	*	VRGroupPQueue	::	GroupLength()
{
	LE_UINT32	*puint32;

	Element0 = new VR ( Group, 0x0000, 4, TRUE);
	if(!Element0)
		return ( NULL );
	puint32 = (LE_UINT32 *) Element0->Data;
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
	(*puint32) = (LE_UINT32) Length;// + sizeof(UINT16) + sizeof(UINT16) + sizeof(UINT32) + sizeof(UINT32);
#else //Big Endian like Apple power pc
	(*puint32) = (LE_UINT32)SwitchEndian((UINT32) Length);// + sizeof(UINT16) + sizeof(UINT16) + sizeof(UINT32) + sizeof(UINT32);
#endif
	Element0->TypeCode = 'UL';
	return ( Element0 );
}

UINT	VRGroupPQueue	::	operator	>	(VRGroupPQueue	&vrgroup)
{
	if(Group > vrgroup.Group)
		return ( 1 );
	return ( 0 );
}

UINT	VRGroupPQueue	::	operator	<	(VRGroupPQueue	&vrgroup)
{
	if(Group < vrgroup.Group)
		return ( 1 );
	return ( 0 );
}

UINT	VRGroupPQueue	::	operator	==	(VRGroupPQueue	&vrgroup)
{
	if(Group == vrgroup.Group)
		return ( 1 );
	return ( 0 );
}

/*****************************************************
 *
 * DICOM Objects
 *
 *****************************************************/

DICOMObject	::	DICOMObject()
  : CurrentGroup(), LastVRG(NULL), LastVR(NULL), LastGroup(0), LastElement(0),
    Length(0), Packaged(FALSE), VRGroupPQueues(),SQObjectArray(NULL),
    FreeSQObjectSize(0), FixedSize(0), UseFixedSize(FALSE)
{
	VRGroupPQueues.ClearType = FALSE;
}
	
DICOMObject	::	~DICOMObject()
{
	VR	*VRPtr;

	if ( !Packaged)
		if ( !Package() )
			return;

	if ( Packaged )
	{
		while (( VRPtr = this->Pop()))
			delete VRPtr;
		return;
	}
	while ( VRGroupPQueues.GetSize() )
		VRGroupPQueues.RemoveAt(0);
}

//Only used in ParseRaw and unused Morph functions, LastVR & LastVRG should be valid at this point.
BOOL	DICOMObject	::	Push(DICOMObject	*SQDO)
{
	Array<DICOMObject *>	*ArrayPtr;

	if ( Packaged )
		return ( FALSE );

	if ( ! LastVR || !LastVRG )//bcb Both used, check both.
	{
		return ( FALSE );	// not-supported yet
		if (!SQObjectArray)
		{
			SQObjectArray = new Array<DICOMObject *>;
			if ( !SQObjectArray )
				return ( FALSE );
		}
		SQObjectArray->Add ( SQDO );
		FreeSQObjectSize += SQDO->Length;
		return ( TRUE );
	}
	if (!LastVR->SQObjectArray)
	{
		ArrayPtr = new Array<DICOMObject *>;
		if (!ArrayPtr)
			return ( FALSE );
		LastVR->SQObjectArray = (void *) ArrayPtr;
		// "new sequence"
		LastVRG->Length += (2 * (sizeof(UINT16) + sizeof(UINT16) +
						sizeof(UINT32) ));
		Length += (2 * (sizeof(UINT16) + sizeof(UINT16) +
						sizeof(UINT32) ));
	}
	ArrayPtr = (Array<DICOMObject *> *) LastVR->SQObjectArray;
	ArrayPtr->Add ( SQDO );	
	LastVRG->Length += SQDO->Length + 
			(2 * (sizeof(UINT16) + sizeof(UINT16) + sizeof(UINT32)));
	Length += SQDO->Length + 
			(2 * (sizeof(UINT16) + sizeof(UINT16) + sizeof(UINT32)));
	return ( TRUE );
}

BOOL	DICOMObject	::	Push(VR	*vr)
{
	UINT			Index;
 	VRGroupPQueue		*VRGroupPQueuePtr;

//	printf("[Push]\n");
	if ( Packaged )
		return ( FALSE );	// already Pop() from this object

	if(vr->Length == 0xffffffff)
	{
//		fprintf(stdout, "Attemping to push -1 length\n");
		vr->Length = 0;
	}

	Index = 0; 
	if(vr->Element == 0x0000)
	{
//		fprintf(stderr, "Attemping to push GLC: %d\n",
//			(*((UINT32 *) vr->Data)));
		delete vr;
		return ( TRUE );	// ignore Group Length Codes (auto calculated)
	}
	LastVR = vr;

	if(LastVRG)
		if(LastVRG->Group == vr->Group)	// Faster, no need to search again
		{
			Length += vr->Length + sizeof(UINT16) + sizeof(UINT16) +
						sizeof(UINT32);
			if(vr->Length&0x01)
				++Length;
			return ( LastVRG->Push ( vr ));
		}
	while ( Index < VRGroupPQueues.GetSize())
	{
		VRGroupPQueuePtr = VRGroupPQueues.Get(Index);
		if(!VRGroupPQueuePtr)
			return ( FALSE );
		if(VRGroupPQueuePtr->Group == vr->Group)
			{
			LastVRG = VRGroupPQueuePtr;	// If we do the same group again, faster.
			Length += vr->Length + sizeof(UINT16) + sizeof(UINT16) +
						sizeof(UINT32);
			if(vr->Length&0x01)
				++Length;
//			printf("[VRGroupPQueuePtr->Group : %d]\n", VRGroupPQueuePtr->Group);
			return ( VRGroupPQueuePtr->Push ( vr ));
		}
		++Index;
	}

	VRGroupPQueuePtr = new VRGroupPQueue(vr->Group);
	if(!VRGroupPQueuePtr)
		return ( FALSE );

	Length += sizeof(UINT16) + sizeof(UINT16) + sizeof(UINT32) +
			sizeof(UINT32);
	Length += vr->Length + sizeof(UINT16) + sizeof(UINT16) +
			sizeof(UINT32);
	if(vr->Length&0x01)
		++Length;
	LastVRG = VRGroupPQueuePtr;	
//	printf("[VRGroupPQueues.Push]\n");
	VRGroupPQueues.Add(VRGroupPQueuePtr);
	return ( VRGroupPQueuePtr->Push(vr) );
}

VR	*	DICOMObject	::	Pop()
{
	VR	*VRPtr;

	//return ( (VR *) NULL );

	if(!Packaged)
		if(!Package())
			return ( (VR *) NULL );
	if(!Packaged)
		return ( (VR *) NULL );

	if ( ! CurrentGroup )
	{
//		printf("[Pop : CurrentGroup = NULL]\n");
		if ( !PQueueOfPtr<VRGroupPQueue *>::GetSize())
		{
			Packaged = FALSE;
			LastVRG = NULL;
			LastVR = NULL;
			return ( (VR *) NULL );
		}
		CurrentGroup = PQueueOfPtr<VRGroupPQueue *>::Pop();
		if ( ! CurrentGroup )
			return ( (VR *) NULL );
		Length -= ( sizeof(UINT16) + sizeof(UINT16) + sizeof(UINT32) +
				sizeof(UINT32) );
	
		return ( CurrentGroup->GroupLength() );
	}
	if ( ! ( VRPtr = CurrentGroup->Pop() ) )
	{
		delete CurrentGroup;
		CurrentGroup = NULL;
		return ( this->Pop() );
	}
	Length -= (VRPtr->Length + sizeof(UINT16) + sizeof(UINT16) +
			sizeof(UINT32) );
	if(VRPtr->Length&0x01)
		--Length;	// See Push
	return ( VRPtr );
}

// this needs to be replaced with a more efficient algorithm in the
// future, but it will do for now.
VR *DICOMObject ::GetVR(UINT16 g, UINT16 e)
{
  VRGroupPQueue *            VRGroupPQueuePtr;
  //DataLink<VRGroupPQueue *> *VRGroupPQueueDataLink;

  if (!VRGroupPQueues.GetSize())
    return (NULL);
  //if (VRGroupPQueues.first)
  //  VRGroupPQueueDataLink = VRGroupPQueues.first;
  //else
  //  return (NULL);

  // while (VRGroupPQueueDataLink)
  for (int i=0; i<VRGroupPQueues.GetSize(); i++)
  { VRGroupPQueuePtr = VRGroupPQueues.Get(i); // VRGroupPQueueDataLink->Data;
    if (!VRGroupPQueuePtr)
      return (NULL);
    if (VRGroupPQueuePtr->Group == g)
    { //DataLink<VR *> *VRLink;
      if (VRGroupPQueuePtr->GetSize()<=0)
        return (NULL);
      //if (VRGroupPQueuePtr->first)
      //  VRLink = VRGroupPQueuePtr->first;
      //else
      //  return (NULL);

      //while (VRLink)
      for (int l=0; l<VRGroupPQueuePtr->GetSize(); l++)
      { //VR *vr = VRLink->Data;
        VR *vr = VRGroupPQueuePtr->Get(l);
        if (!vr)
          return (NULL);
        if (vr->Element == e)
          return (vr);

        //VRLink = VRLink->next;
      }
      return (NULL);
    }
    //VRGroupPQueueDataLink = VRGroupPQueueDataLink->next;
  }
  return (NULL);
}

/* bcb replaced by byte order independent versions
LE_UINT16	DICOMObject	::	GetUINT16(UINT16	g, UINT16	e)
{
	VR		*vr;
	LE_UINT16	Val;

	vr = this->GetVR ( g, e );
	if(!vr)
		return(0);
		
	if ( vr->Data )
		if(vr->Length == sizeof(UINT16))
		{
			memcpy(&Val, vr->Data, 2);
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
			return ( Val );
#else //Big Endian like Apple power pc
			return (SwitchEndian((UINT16) Val ));
#endif //Big Endian
		}
	return ( 0 );
}*/

// Will return the first byte from LE data, independent of vr->Length
UINT8 DICOMObject	::	GetBYTE(UINT16	g, UINT16	e)
{
	VR		*vr;
	UINT8		value = 0;
	
	vr = this->GetVR ( g, e );
	if(!vr || !vr->Length) return(0);
	
	if(vr->Data) value = ((unsigned char *)vr->Data)[0];
	return(value);
}

// Will return a short from LE data, independent of vr->Length
UINT16 DICOMObject	::	GetUINT16(UINT16	g, UINT16	e)
{
	VR		*vr;
	UINT16	value = 0;
	
	vr = this->GetVR ( g, e );
	if(!vr || !vr->Length) return(0);
	
	if(vr->Data)
	{
		if (vr->Length > 1) value = ((unsigned char *)vr->Data)[1] << 8;
		value += ((unsigned char *)vr->Data)[0];
	}
	return(value);
}

// Will return a 32 bit integer from LE data, independent of vr->Length
UINT DICOMObject	::	GetUINT(UINT16	g, UINT16	e)
{
	VR		*vr;
	INT	cnt;
	UINT	value = 0;
	
	vr = this->GetVR ( g, e );
	if(!vr || !vr->Length) return(0);

	cnt = vr->Length - 1;
	
	if(vr->Data)
	{
		if (cnt > 3) cnt = 3;
		for(;cnt >= 0;--cnt)
		{
			value <<= 8;
			value += ((unsigned char *)vr->Data)[cnt];
		}
	}
	return(value);
}

// Will return a 64 bit integer from LE data, independent of vr->Length
unsigned long long DICOMObject	::	GetULongLong(UINT16	g, UINT16	e)
{
	VR		*vr;
	unsigned long long	value = 0;
	INT			cnt;
	
	vr = this->GetVR ( g, e );
	if(!vr || !vr->Length) return(0);

	cnt = vr->Length - 1;
	
	if(vr->Data)
	{
		if (cnt > 7) cnt = 7;
		for(;cnt >= 0;--cnt)
		{
			value <<= 8;
			value += ((unsigned char *)vr->Data)[cnt];
		}
	}
	return(value);
}

// Will return a 32 bit integer from Ascii, with integer vr->Length the first 10 chars (max for 32 bits anyway).
INT DICOMObject	::	Getatoi(UINT16	g, UINT16	e)
{
	VR		*vr;
	INT	cnt, value = 0;
	char	s[11];
	
	vr = this->GetVR ( g, e );
	if(!vr || !vr->Length) return(0);

	cnt = vr->Length;
	if(cnt > 10) cnt = 10;
	
	if(vr->Data)
	{
		strncpy(s, (const char*)vr->Data, cnt);
		s[cnt] = 0;
		value = atoi(s);
	}
	return(value);
}


// This will allocate this string, do not forget to free it.
char * DICOMObject	::	GetCString(UINT16 g, UINT16 e, size_t max)
{
	VR	*vr;
	size_t	cnt;
	char	*s;
	
	vr = this->GetVR ( g, e );
	if(!vr || !vr->Length || !vr->Data) return(NULL);

	cnt = vr->Length - 1;
	if(((char*)vr->Data)[cnt] != 0 || ((char*)vr->Data)[cnt] != ' ') cnt++;

	if(cnt > max) cnt = max;// Long enough
    
	if(!(s = (char*)malloc(cnt + 1))) return(NULL);// Maybe return -1?
	strncpy(s, (const char*)vr->Data, cnt);
	s[cnt] = 0;// End the string

	return(s);
}

//Fixed LastVR & LastVRG, still worry about CurrentGroup.
BOOL	DICOMObject::DeleteVR(VR* pVR)
{
  VR			*vr; 
  UINT			Index,Index2;
  VRGroupPQueue		*VRGroupPQueuePtr;

  if ( Packaged )
    return ( FALSE );	// already Pop() from this object

  /* Search the pVR */
  Index = 0;
  while ( Index < VRGroupPQueues.GetSize())
  { VRGroupPQueuePtr = VRGroupPQueues.Get(Index);
    if(!VRGroupPQueuePtr)
      return FALSE;

    if(VRGroupPQueuePtr->Group == pVR->Group)
    { Index2 = 0;
      while ( Index2 < VRGroupPQueuePtr->GetSize() )
      { vr = VRGroupPQueuePtr->Get(Index2);
        if(!vr)
        return FALSE;

        if(vr->Element == pVR->Element)
        { /* OK, the pVR has been found.
          Remove from Array of VrPointers */
          VRGroupPQueuePtr->RemoveAt(Index2);

          VRGroupPQueuePtr->Length -= (pVR->Length + sizeof(UINT16) + sizeof(UINT16) +
          				sizeof(UINT32) );
          if (VRGroupPQueuePtr->Length & 0x01)
            VRGroupPQueuePtr->Length--;

          Length -= (pVR->Length + sizeof(UINT16) + sizeof(UINT16) +
          		sizeof(UINT32) );
          if (Length & 0x01)
            Length--;

          // Maybe remove from Array of GroupPointers.
          if (!VRGroupPQueuePtr->Length)
          { VRGroupPQueues.RemoveAt(Index);
            delete VRGroupPQueuePtr;		// LJ: leak
            Length -= (sizeof(UINT16) + sizeof(UINT16) + sizeof(UINT32));
            LastVRG = NULL;			// bcb: Fix invalid pointer
          }

          // Get rid of the data.
          delete vr;				// LJ: leak
          LastVR = NULL;			// bcb: Crash fix.
          return TRUE;
        }
        ++Index2;
      }
      return FALSE;
    }
    ++Index;
  }
  return FALSE;
}

BOOL DICOMObject::ReplaceVR(VR* pVR)
{
  VR*			vr;
  UINT			Index, Index1, Index2;
  VRGroupPQueue*	VRGroupPQueuePtr;
  Array<DICOMDataObject*>*	pADDO;

  if ( Packaged )
    return ( FALSE );	// already Pop() from this object
  /* Search the pVR */
  Index = 0;
  while ( Index < VRGroupPQueues.GetSize())
  { VRGroupPQueuePtr = VRGroupPQueues.Get(Index);
    if(!VRGroupPQueuePtr)
      return FALSE;
    Index1 = 0;
    while ( Index1< VRGroupPQueuePtr->GetSize() )
    { vr = VRGroupPQueuePtr->Get(Index1);
      if(!vr)
	    return FALSE;
      if (vr->SetIf(pVR))
      { /* OK, the pVR has been found */
      }
      else if (vr->SQObjectArray)
      { /* Loop over all DICOMDataObjects in this sequence */
	    pADDO = (Array<DICOMDataObject*>*)vr->SQObjectArray;
	    Index2 = 0;
	    while (Index2< pADDO->GetSize())
	    { pADDO->Get(Index2)->ReplaceVR(pVR);
	      ++Index2;
	    }
      }
      ++Index1;
    }
    ++Index;
  }
  return FALSE;
}
// The ChangeVRs are cleaner than the ReplaceVR and do not require delete vr,
// they return a 0 if failed,
// a 1 if replaced,
// a 2 if created.
// For a string, it must end in a 0 ( standard c string ). 
INT     DICOMObject	::	ChangeVR(UINT16 g, UINT16 e, const char * str, UINT16 type, BOOL space)
{
    VR      *vr;
    UINT    slen;
    INT     rc = 1;
    
    slen = strlen(str);// Remove the end 
    vr = this->GetVR( g, e );
    if(!vr)
    {
        vr = new VR( g, e, slen, TRUE);
        if(!vr)return(0);
        rc = 2;
        vr->TypeCode = type;
        this->Push(vr);
    }
    else
    {
        if(!vr->ReAlloc(slen))return(0);
    }
    memcpy(vr->Data, str, slen);
    if(slen & 1)
    {
        if(space) ((char *)vr->Data)[slen] = ' ';
        else ((char *)vr->Data)[slen] = 0;
    }
    return(rc);
}

INT     DICOMObject	::	ChangeVR(UINT16 g, UINT16 e, UINT8 val, UINT16 type = 0)
{
    VR      *vr;
    INT     rc = 1;
    
    vr = this->GetVR( g, e );
    if(!vr)
    {
        vr = new VR( g, e, 2, TRUE);
        if(!vr)return(0);
        rc = 2;
        vr->TypeCode = type;
        this->Push(vr);
    }
    else
    {
        if(vr->Length != 2 || !vr->ReAlloc(2))return(0);
    }

    *(UINT8 *)vr->Data = val;
    ((UINT8 *)vr->Data)[1] = 0;
    return(rc);
}

INT     DICOMObject	::	ChangeVR(UINT16 g, UINT16 e, UINT16 val, UINT16 type = 0)
{
    VR      *vr;
    INT     rc = 1;
    
    vr = this->GetVR( g, e );
    if(!vr)
    {
        vr = new VR( g, e, 2, TRUE);
        if(!vr)return(0);
        rc = 2;
        vr->TypeCode = type;
        this->Push(vr);
    }
    else
    {
        if(vr->Length != 2 || !vr->ReAlloc(2))return(0);
    }
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
	*(UINT16 *)vr->Data = val;
#else //Big Endian like Apple power pc
	*(UINT16 *)vr->Data = SwitchEndian(val);
#endif
    return(rc);
}

INT     DICOMObject	::	ChangeVR(UINT16 g, UINT16 e, UINT val, UINT16 type = 0)
{
    VR      *vr;
    INT     rc = 1;
    
    vr = this->GetVR( g, e );
    if(!vr)
    {
        vr = new VR( g, e, 4, TRUE);
        if(!vr)return(0);
        vr->TypeCode = type;
        rc = 2;
        this->Push(vr);
    }
    else
    {
        if(vr->Length != 4 || !vr->ReAlloc(4))return(0);
    }
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
	*(UINT *)vr->Data = val;
#else //Big Endian like Apple power pc
	*(UINT *)vr->Data = SwitchEndian(val);
#endif
    return(rc);
}


BOOL	DICOMObject	::	Reset()
{
	VR	*vr;

	while (( vr = Pop() ))
		delete vr;

	LastVRG = NULL;
	LastVR = NULL;

	return ( TRUE );
}


BOOL	DICOMObject	::	Package()
{
	UINT	Index;
	
	CurrentGroup = NULL;
	Index = 0;
//	printf("[start: Package()]\n");
	while ( Index < VRGroupPQueues.GetSize())
	{
		PQueueOfPtr<VRGroupPQueue *> :: Push (VRGroupPQueues.Get(Index) );
		++Index;
	}
	while ( VRGroupPQueues.GetSize() )
		VRGroupPQueues.RemoveAt(0);
		
//	printf("[Packaged : %d Groups]\n", Index);
	Packaged = TRUE;
	if ( Index )
		return ( TRUE );
	Packaged = FALSE;
	return ( FALSE );
}


#define VR_SPECIAL_NONE			0x00
#define VR_SPECIAL_FIX_TMS		0x01
#define VR_SPECIAL_RGB_TO_MONO		0x02
#define VR_SPECIAL_RGB_TO_MONO_PLUS	0x03
#define VR_SPECIAL_UNKNOWN		0xFF

int VR::GetSpecialValueCodeOfVR(VR* pVR)
{  char*	pcData;

  /* This private function is called by SetIf.
     It analyzes the data of the VR. If it contains a special code, a string between
     '[[' and ']]', it will return the code; otherwise zero.
  */
  if ((pVR->Length < 4) || (!pVR->Data))
    return VR_SPECIAL_NONE;
  pcData = (char*)pVR->Data;
  if ((pcData[0] != '[') || (pcData[1] != '['))
    return VR_SPECIAL_NONE;

  if ((pVR->Length == 12) && (strncmp(pcData, "[[FIX_TMS]]", 11) == 0))
    return VR_SPECIAL_FIX_TMS;
  if ((pVR->Length == 20) && (strncmp(pcData, "[[RGB_TO_MONO_PLUS]]", 20) == 0))
    return VR_SPECIAL_RGB_TO_MONO_PLUS;
  if ((pVR->Length == 16) && (strncmp(pcData, "[[RGB_TO_MONO]]", 15) == 0))
    return VR_SPECIAL_RGB_TO_MONO;

  return VR_SPECIAL_UNKNOWN;
}


BOOL VR::SetIf(VR	*vr)
{ Array<DICOMObject*>	*ArrayDO;
  int			i, iSecialCodeOfVR;
  BOOL			rc = FALSE;
  char*			pCopy;
  unsigned char*	pRGB;
  unsigned char*	pMono;
  int			iNewLength;
  int			iNewValue, iPad = 0;

  if (!vr)
    return rc;
  if ((vr->Group==Group) && (vr->Element==Element))
  {
    iSecialCodeOfVR = GetSpecialValueCodeOfVR(vr);
    switch (iSecialCodeOfVR)
    { case VR_SPECIAL_NONE:
      /* Just replace the VR */
      if(ReleaseMemory)
//        delete [] Data;
	  free(Data);
      if(SQObjectArray)
      { ArrayDO = (Array<DICOMObject *> *) SQObjectArray;
	while(ArrayDO->GetSize())
	{ delete ArrayDO->Get(0);
	  ArrayDO->RemoveAt(0);
	}
	delete ArrayDO;
	SQObjectArray = NULL;
      }
      Length = vr->Length;
      if (vr->Length)
      { Data = malloc(vr->Length);
        if (!Data) DicomError(DCM_ERROR_MEMORY, "VR:SetIf out of memory allocating %d bytes\n", Length);
	memcpy(Data, vr->Data, vr->Length);
	ReleaseMemory = TRUE;
      }
      else
	ReleaseMemory = FALSE;
      rc = TRUE;
      break;

      case VR_SPECIAL_FIX_TMS:
      /* Change PatientName like 'LastName^FirstName^MiddleName' into
	 'LastName,FirstName,MiddleName'.
      */
      if (Length > 0)
      { pCopy = (char*)malloc(Length+2);
        if (!pCopy) DicomError(DCM_ERROR_MEMORY, "VR:SetIf out of memory allocating %d bytes\n", Length+2);
	memcpy(pCopy, (char*)Data, Length);
	pCopy[Length] = 0;
	/* Maybe padded with a space */
	if ((pCopy[Length-1] == ' ') || (pCopy[Length-1] == '^'))
	  pCopy[Length-1] = 0;
	if ((Length > 1) && (pCopy[Length-2] == '^'))
	  pCopy[Length-2] = 0;
	for (i=0; i<(int)(strlen(pCopy)); i++)
	  if (pCopy[i] == '^')
	    pCopy[i] = ',';
	/* May pad it again */
	if (strlen(pCopy) & 0x01)
	  strcat(pCopy, " ");
	strncpy((char*)Data, pCopy, strlen(pCopy));
	Length = strlen(pCopy);
	free(pCopy);
      }
      rc = TRUE;
      break;

      case VR_SPECIAL_RGB_TO_MONO:
      /* Convert triplets of bytes to just bytes (the mean of the RGB channels) */
      if (Length >= 3)
      { iNewLength = Length / 3;
	if (iNewLength & 0x01)
	  iPad = 1;
        pMono = (unsigned char*)malloc(iNewLength + iPad);
        if (!pMono) DicomError(DCM_ERROR_MEMORY, "VR:SetIf out of memory allocating %d bytes\n", iNewLength + iPad);
	pRGB = (unsigned char*)Data;
	for (i=0; i<iNewLength; i++)
	{ iNewValue = (pRGB[i * 3] + pRGB[i * 3 + 1] + pRGB[i * 3 + 2]) / 3; 
	  pMono[i] = (unsigned char)iNewValue;
	}
	if (iPad)
	  pMono[iNewLength] = 0;
	Length = iNewLength + iPad;
	free(Data);
	Data = pMono;
      }
      rc = TRUE;
      break;

      case VR_SPECIAL_RGB_TO_MONO_PLUS:
      /* Convert triplets of bytes to just bytes only when the value of those three
	 are equal (a B&W image). However, 'real' color (contours e.g.) are converted
	 to very white (255).
      */
      if (Length >= 3)
      { iNewLength = Length / 3;
	if (iNewLength & 0x01)
	  iPad = 1;
        pMono = (unsigned char*)malloc(iNewLength + iPad);
        if (!pMono) DicomError(DCM_ERROR_MEMORY, "VR:SetIf out of memory allocating %d bytes\n", iNewLength + iPad);
	pRGB = (unsigned char*)Data;
	for (i=0; i<iNewLength; i++)
	{ if ((pRGB[i * 3] == pRGB[i * 3 + 1]) && (pRGB[i * 3] == pRGB[i * 3 + 2])) 
	    pMono[i] = pRGB[i * 3];
	  else
	    pMono[i] = 255;
	}
	if (iPad)
	  pMono[iNewLength] = 0;
	Length = iNewLength + iPad;
	free(Data);
	Data = pMono;
      }
      rc = TRUE;
      break;
    }
  }
  return rc;
}


/* Just not used
BOOL
VR	::	Morph (
	DICOMObject	*DDO)
	{
	Array<DICOMObject	*>	*ArrayDO;

	if (!Length)
		if(!Valid)
			return ( FALSE );

	VR	*vr = new VR(Group, Element, 0, (BOOL) FALSE);
	vr->Length = Length;
	vr->Data = Data;
	vr->ReleaseMemory = ReleaseMemory;
	Length = 0;
	Data = NULL;
	ReleaseMemory = FALSE;
	DDO->Push(vr);

	if(SQObjectArray)
		{
		ArrayDO = (Array<DICOMObject *> *) SQObjectArray;
		while(ArrayDO->GetSize())
			{
			DDO->Push(ArrayDO->Get(0));
			ArrayDO->RemoveAt(0);
			}
		delete ArrayDO;
		SQObjectArray = NULL;
		}
	return ( TRUE );
	}*/

BOOL
VR	::	Reset ()
	{
	Array<DICOMObject	*>	*ArrayDO;

	if(ReleaseMemory)
		if(Data)
//			delete [] Data;
			free(Data);
	if(SQObjectArray)
		{
		ArrayDO = (Array<DICOMObject *> *) SQObjectArray;
		while(ArrayDO->GetSize())
			{
			delete ArrayDO->Get(0);
			ArrayDO->RemoveAt(0);
			}
		delete ArrayDO;
		SQObjectArray = NULL;
		}
	ReleaseMemory = FALSE;
	Data = NULL;
	Length = 0;
	return ( TRUE );
	}


BOOL
VR	::	ReAlloc (UINT32 N)
	{
        Reset();

	if (N&1) N++;
	ReleaseMemory = TRUE;
	Data = malloc(N);
        if (!Data) DicomError(DCM_ERROR_MEMORY, "VR:ReAlloc out of memory allocating %d bytes\n", N);
	Length = N;
	return ( Data!=NULL );
	}

// Will return an unsigned short from LE data, independent of vr->Length
UINT16
VR	::	GetUINT16()
{
	UINT16	value;
	
	value = 0;
	
	if(Data)
	{
		if (Length > 1)value = ((unsigned char *)Data)[1] << 8;
		value += ((unsigned char *)Data)[0];
	}
	return(value);
}

// Will return a 32 bit interger from LE data, independent of vr->Length
UINT
VR	::	GetUINT()
{
	INT	cnt;
	UINT	value = 0;
	
	cnt = Length - 1;
	
	if(Data)
	{
		if (cnt > 3) cnt = 3;
		for(;cnt >= 0;--cnt)
		{
			value <<= 8;
			value += ((unsigned char *)Data)[cnt];
		}
	}
	return(value);
}

// Will return an unsigned 64 bit interger from LE data, independent of vr->Length
unsigned long long
VR	::	GetULongLong()
{
	unsigned long long	value;
	INT			cnt;
	
	cnt = Length - 1;
	value = 0;
	
	if(Data)
	{
		if (cnt > 7) cnt = 7;
		for(;cnt >= 0;--cnt)
		{
			value <<= 8;
			value += ((unsigned char *)Data)[cnt];
		}
	}
	return(value);
}

// Will return an 32 bit interger from Ascii, with interger vr->Length or shorter.
INT
VR	::	Getatoi()
{
	INT		cnt, value = 0;
	char		s[11];
	
	cnt = Length;
    if(cnt > 10) cnt = 10;
	
	if(cnt && Data)
	{
		strncpy(s, (const char*)Data,cnt);
		s[cnt] = 0;
		value = atoi(s);
	}
	return(value);
}
