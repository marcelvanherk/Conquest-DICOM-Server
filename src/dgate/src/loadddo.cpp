/*
 * Reads DICOMObject's from disk.  As well as makes Windows BITMAP
 * Structures.
 *
 * Bugs:	Does not handle sequence objects
 *			Does not support the recent change to DICOM Chapter 10
 *			Only works on 32 bit little-endian architecture
 * Notes:
 *
 *			Eventually this routine will be not be needed.  I will
 *	add Disk I/O to the PDU_Service class, which will be a much cleaner,
 *	and much more portable solution.
 * Mark Oskin
 *
 */

/*
19980409 mvh     Added NKI private decompression engine VR 0x7fdf,0x0010
19980410 mvh     Added run length encoding of zero differences
19980410 ljz+mvh Fix 16 bit absolute value decode
19980415 ljz+mvh Fix leak on decompress
19980625 mvh     Added compression in SaveDicomDataObject(C10)
19980703 mvh+ljz Made DObjectSerialize thread save (removed statics)
19990105 ljz     Fix: SaveDICOMDataObjectC10 created VR 0x0002 0x0013 twice
                 Added comment in SaveDICOMDataObjectC10; CleanUp
                 Put the nki-routines also in dgate.hpp
                 Added LoadImplicitLittleEndianFile
19990317 ljz     Decompression removed from LoadImplicitLittleEndianFile; added a
                 VR-sizelimit of that reader
19990827 mvh     NOTE: LoadImplicitLittleEndianFile allocates 100 MB and crashes when passing
                 chapter 10 file (happens when extension is wrongly .v2)
19990830 ljz     Fixed problem above: 'LoadImplicitLittleEndianFile' uses
		 'PDU.LoadDICOMDataObject' in such cases.
19991117 ljz     Added parameter FileCompressMode to in nki_private_compress call
20000629 ljz     Logging of trouble now starts with '***'
20001104 mvh     Renamed _LittleEndianUID to _LittleEndianUID_space
20001105 mvh     Fixed where malloc and new were mixed (vr->data)
20001106 mvh     Use delete [] operation for vr->Data
20001128 ljz     Fix in 'Implicit length' code  in LoadImplicitLittleEndian
20010429 mvh     Use faster and safer decompressor
20020412 ljz     Added class CBufferedIO for faster reading from DVD (stream functions
		 do not seem to be cacheing...)
20020415 mvh     Removed diagnostic time code (did not compile on ms4.2)
                 Made buffer size dependent on iVrSizelimit 
                 (i.e, use smaller for regen, slightly faster)
20030706 mvh	 Attach VRType to PDU's for implicit little endian support
20041130 mvh	 Documented crash on win2000 server in code - remain to be fixed
20050107 mvh     Adapted for linux compile
20051219 mvh     Use ReAlloc instead of new BYTE[] to fill VR data
20070308 bcb     And little/big endian.
20081116 mvh     Adapted for very large objects (use unsigned int for length)
20090211 mvh     Safety checks in LoadImplicitLittleEndian to avoid crash on corrupted V2 file
		 Removed lots of dead code 
20090412 mvh	 Fixed for linux compile (tell does not exist)
20091215 ljz     Fixed LoadImplicitLittleEndian() when iVrSizeLimit is used (e.g. regen)
		 (assumption that the pixeldata would be the last VR is not correct).
20091231 bcb     Changed char* to const char* for gcc4.2 warnings
20100111 mvh     Merged
20100210 bcb     Fixed reading some old V2 files.
20100309 bcb     Changed int to unsigned int, cast m_iNbBytesInBuffer to long (gcc4.2 Warnings)
20100309 bcb     Commented out unused (gcc4.2 Warnings)
20100619 bcb     Fix gcc4 warnings, improve speed and prevent CBufferIO from being copied.
20100717 mvh     Merged
*/


#ifdef WIN32
#include	<io.h>
#else
#include	<unistd.h>
#endif

#include	<fcntl.h>
#include	<sys/stat.h>
#include	<stdio.h>

#ifndef	WHEDGE
#	include	"dgate.hpp"
#else
#	include	"master.h"
#endif

/*-----------------------------------------------------------------------*/

class CBufferedIO
{
  private:
    char*	m_pBuf;
    int		m_iBufSize;
    int		m_iCurPos;
    int		m_iNbBytesInBuffer;
    int		m_iNbBlocksRead;
  public:
    CBufferedIO(int iBufSize = 0x4000);
    ~CBufferedIO();
    int  read (int handle, void *buffer, unsigned int count);
    long lseek(int handle, long offset, int origin);
#ifdef __GNUC__
private:// This will prevent it from being copied (it has a pointer)
	CBufferedIO(const CBufferedIO&);
	const	CBufferedIO & operator = (const CBufferedIO&);
#endif
};

CBufferedIO::CBufferedIO(int iBufSize)
#ifdef __GNUC__ // Faster with member initialization.
:m_pBuf(NULL),
m_iBufSize(iBufSize),
m_iCurPos(0),
m_iNbBytesInBuffer(0),
m_iNbBlocksRead(0) {}
#else
{
  m_pBuf             = NULL;
  m_iBufSize         = iBufSize;
  m_iCurPos          = 0;
  m_iNbBytesInBuffer = 0;
  m_iNbBlocksRead    = 0;
};
#endif

CBufferedIO::~CBufferedIO()
{
  if (m_pBuf)
    free(m_pBuf);
}

int CBufferedIO::read (int handle, void *buffer, unsigned int count)
{ int		iNbBytesCopied = 0;

  if (!m_pBuf)
  { if (m_iBufSize <= 0)
      return -1;
    m_pBuf = (char*)malloc(m_iBufSize);
    if (!m_pBuf)
      return -1;
  }

  while ((signed int)count - iNbBytesCopied > m_iNbBytesInBuffer - m_iCurPos)
  { memcpy(buffer, m_pBuf + m_iCurPos, m_iNbBytesInBuffer - m_iCurPos);
    buffer = (char*)buffer + m_iNbBytesInBuffer - m_iCurPos;
    iNbBytesCopied += m_iNbBytesInBuffer - m_iCurPos;
    m_iCurPos = 0;
    m_iNbBytesInBuffer = ::read(handle, m_pBuf, m_iBufSize);
    m_iNbBlocksRead ++;
    if (m_iNbBytesInBuffer <= 0)
      return iNbBytesCopied;
  }

/* crash probably when read failed of file after:

2004-11-30 09:46:48 [RT8769103] Sending file : y:\dv0_0001\20001314\1.2.840.113619.2.22.287.1.369.2.20000619.221935_0002_000014_96263112199.V2

 0001:0001fb70       ?read@CBufferedIO@@QAEHHPAXI@Z 00420b70 f loadddo.obj
 0001:0001fc40       ?lseek@CBufferedIO@@QAEJHJH@Z 00420c40 f loadddo.obj


function: <nosymbols>
        00420c0d 5b               pop     ebx
        00420c0e c20c00           ret     0xc
        00420c11 8b44241c         mov     eax,[esp+0x1c]         ss:0b6c89cf=????????
        00420c15 8b7500           mov     esi,[ebp+0x0]          ss:02b85cb6=????????
        00420c18 8bd0             mov     edx,eax
        00420c1a 037508           add     esi,[ebp+0x8]          ss:02b85cb6=????????
        00420c1d 2bd3             sub     edx,ebx
        00420c1f 8b7c2418         mov     edi,[esp+0x18]         ss:0b6c89cf=????????
        00420c23 8bca             mov     ecx,edx
        00420c25 c1e902           shr     ecx,0x2
FAULT ->00420c28 f3a5            rep  movsd ds:023c2000=???????? es:1f838480=00000000
        00420c2a 8bca             mov     ecx,edx
        00420c2c 83e103           and     ecx,0x3
        00420c2f f3a4             rep     movsb         ds:023c2000=?? es:1f838480=00
        00420c31 8bc8             mov     ecx,eax
        00420c33 2bcb             sub     ecx,ebx
        00420c35 014d08           add     [ebp+0x8],ecx          ss:02b85cb6=????????
        00420c38 5d               pop     ebp
        00420c39 5f               pop     edi
        00420c3a 5e               pop     esi
        00420c3b 5b               pop     ebx
        00420c3c c20c00           ret     0xc

*/

  memcpy(buffer, m_pBuf + m_iCurPos, count - iNbBytesCopied);
  m_iCurPos += count - iNbBytesCopied;
  return count;
}

long CBufferedIO::lseek(int handle, long offset, int origin)
{ long	rc;

  /* Just handle the special case for the 'DICM' test */
  if ((origin == SEEK_SET) &&
      (offset < (long)m_iNbBytesInBuffer) &&
      (m_iNbBlocksRead == 1))
  { m_iCurPos = offset;
    rc = offset;
  }
  else
  { /* Make sure that above special case does not re-appear */
    m_iNbBlocksRead = 999;
    if (origin == SEEK_CUR)
      /* Adjust the file-position to where the caller thinks it is */
      offset = offset - (m_iNbBytesInBuffer - m_iCurPos);
    rc = (long)::lseek(handle, offset, origin);
    m_iCurPos          = 0;
    m_iNbBytesInBuffer = 0;
  }
  return rc;
}

/*-----------------------------------------------------------------------*/

extern void MyDcmError(int error, const char *message, int count);


static BOOL
LoadImplicitLittleEndian(DICOMDataObject* pDDO, CBufferedIO* pBufferedIO,
	int handle, unsigned int iVrSizeLimit, unsigned int uFileSize)
{
	DICOMDataObject*	pNewDDO;
	VR*			pVR;
	char			Buf[2 + 2 + 4];
	unsigned int		CurrentGroup = 0;
	unsigned int		CurrentElement = 0;

	SetDicomErrorHandler(NULL);

	while (pBufferedIO->read(handle, Buf, sizeof(Buf)) == sizeof(Buf))
	{
		/* Group, Element and Size could be read */
		pVR = new VR;
		if (!pVR)
			{
			OperatorConsole.printf("*** failed to allocate memory during load of V2 file (previous %04x:%04x)\n", CurrentGroup, CurrentElement);
			SetDicomErrorHandler(MyDcmError);
			return FALSE;
			}

#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
		pVR->Group   = *((unsigned short*) Buf);
		pVR->Element = *((unsigned short*)(Buf + 2));
		pVR->Length  = *((unsigned int*)  (Buf + 2 + 2));
#else //Big Endian like Apple power pc
		pVR->Group   = SwitchEndian( *((UINT16*) Buf));
		pVR->Element = SwitchEndian( *((UINT16*)(Buf + 2)));
		pVR->Length  = SwitchEndian( *((UINT32*)  (Buf + 2 + 2)));
#endif //Big Endian

		if (pVR->Group < CurrentGroup)
			{
			OperatorConsole.printf("*** Encountered an invalid group order during load of V2 file (current %04x:%04x, previous %04x:%04x)\n", pVR->Group, pVR->Element, CurrentGroup, CurrentElement);
			SetDicomErrorHandler(MyDcmError);
			return FALSE;
			}

		if (pVR->Group != CurrentGroup)
			{
			CurrentGroup = pVR->Group;
			CurrentElement = 0;
			}

		if (pVR->Element < CurrentElement)
			{
			OperatorConsole.printf("*** Encountered an invalid element order during load of V2 file (current %04x:%04x, previous %04x:%04x)\n", pVR->Group, pVR->Element, CurrentGroup, CurrentElement);
			SetDicomErrorHandler(MyDcmError);
			return FALSE;
			}

		CurrentElement = pVR->Element;

		if (pVR->Group == 0xfffe)
		{
			/* A deliminator */
			if ((pVR->Element == 0xe0dd) || (pVR->Element == 0xe00d))
			{
				delete pVR;
				SetDicomErrorHandler(MyDcmError);
				return TRUE;
			}
			if (pVR->Length == 0xffffffff)
			{
				/* Implicit length... Go until deliminator */
				pVR->Length = 0;
				delete pVR;
				pNewDDO = new DICOMDataObject;
				if (LoadImplicitLittleEndian(pNewDDO, pBufferedIO, handle, iVrSizeLimit, uFileSize))
				{
					pDDO->Push(pNewDDO);
					continue;
				}
				else
				{
					delete pNewDDO;
					SetDicomErrorHandler(MyDcmError);
					return FALSE;
				}
			}
			if (pVR->Element == 0xe000)
			{
                /* BCB- Found a v2 file with a length != 0 and pixel data here (see LJZ below) */
                if(pVR->Length == 0)
                {
					/* Sequence begin ? */
					//    				pVR->Length = 0;
					delete pVR;
					pNewDDO = new DICOMDataObject;
					if (LoadImplicitLittleEndian(pNewDDO, pBufferedIO, handle, iVrSizeLimit, uFileSize))
					{
						pDDO->Push(pNewDDO);
						continue;
					}
					else
					{
						delete pNewDDO;
						SetDicomErrorHandler(MyDcmError);
						return FALSE;
					}
				}
			}
		}
		if (pVR->Length == 0xffffffff)
		{
			pVR->Length = 0;
			pDDO->Push(pVR);
			if (!LoadImplicitLittleEndian(pDDO, pBufferedIO, handle, iVrSizeLimit, uFileSize))
				{
				SetDicomErrorHandler(MyDcmError);
				return FALSE;
				}
			continue;
		}
		else if (pVR->Length>uFileSize || (pVR->Length&1))
		{
			OperatorConsole.printf("*** Encountered an invalid item length (%u) during load of V2 file in %04x:%04x\n", 
				pVR->Length, CurrentGroup, CurrentElement);
			SetDicomErrorHandler(MyDcmError);
			return FALSE;
		}

		/* Check whether the current VR has to be read.
		   NKI DicomNodes can restrict what has to be read
		   Following code assumes that reading is finished when pixeldata
		   are encountered. (Maybe a problem here!!!)
		   LJZ: indeed, there are examples of pixeldata inside a sequence, when the 'real'
			pixeldata (amongst other VRs) still have to come...
		*/
		if (pVR->Length > iVrSizeLimit)
		{
			if (((pVR->Group == 0x7fdf) || (pVR->Group == 0x7fe0)) &&
				(pVR->Element == 0x0010))
			{
				/* Ready !? */
/*
				pVR->Length = 0;
				delete pVR;
				SetDicomErrorHandler(MyDcmError);
				return TRUE;
*/
				pBufferedIO->lseek(handle, pVR->Length, SEEK_CUR);
				pVR->Length = 0;
				delete pVR;
				continue;
			}
			else
			{	/* Read it, throw it away and continue */
				pVR->ReAlloc(pVR->Length);
				if (!pVR->Data)
					{
					OperatorConsole.printf("*** Element too large during load of V2 file in %04x:%04x\n",
						CurrentGroup, CurrentElement);
					SetDicomErrorHandler(MyDcmError);
					return FALSE;
					}
				pBufferedIO->read(handle, pVR->Data, pVR->Length);
				delete pVR;
				continue;
			}
		}
		if (pVR->Length)
		{
			pVR->ReAlloc(pVR->Length);
			if (!pVR->Data)
				{
				OperatorConsole.printf("*** Element too large during load of V2 file in %04x:%04x\n",
					CurrentGroup, CurrentElement);
				SetDicomErrorHandler(MyDcmError);
				return FALSE;
				}
			pBufferedIO->read(handle, pVR->Data, pVR->Length);
		}
		else
			pVR->Data = NULL;
		pDDO->Push(pVR);
	}

	SetDicomErrorHandler(MyDcmError);
	return TRUE;
}


DICOMDataObject*
LoadImplicitLittleEndianFile(char* filename, unsigned int iVrSizeLimit)
{
	DICOMDataObject*	pDDO;
	int			handle;
	char			Buf[128 + 4];
	PDU_Service		PDU;
	unsigned int		uFileSize;//,iLastTime, iCurTime;
	CBufferedIO*		pBufferedIO;

	PDU.AttachRTC(&VRType);

#ifdef WIN32	
	handle = sopen( filename, O_RDONLY | O_BINARY, SH_DENYNO, S_IREAD);
#else
	handle = open( filename, O_RDONLY);
#endif

	if (handle == -1)
	{
		return NULL;
	}

	uFileSize = (long)lseek(handle, 0, SEEK_END);
  	// uFileSize = tell(handle); /* ask position */
	lseek(handle, 0, SEEK_SET);

	pBufferedIO = new CBufferedIO(iVrSizeLimit < 0x1000 ? 0x1000 : 0x8000);
	/* Find out whether marker 'DICM' exists */
	if (pBufferedIO->read(handle, Buf, 128 + 4) != 128 + 4)
	{
		delete pBufferedIO;
		close(handle);
		return NULL;
	}
	else
	{
		if (strncmp(Buf + 128, "DICM", 4) != 0)
		{
			pBufferedIO->lseek(handle, 0, SEEK_SET);
			/* We are at beginning of the VRs */
			pDDO = new DICOMDataObject;
			if (!LoadImplicitLittleEndian(pDDO, pBufferedIO, handle, iVrSizeLimit, uFileSize))
			{
				delete pDDO;
				return NULL;
			}
			delete pBufferedIO;
			close(handle);
			return pDDO;
		}
		else
		{
			/* The function 'LoadImplicitLittleEndianFile' is not intended for
			   DICM files, however don't be fuzzy about it... */
			delete pBufferedIO;
			close(handle);
			pDDO = PDU.LoadDICOMDataObject(filename);
			return pDDO;
		}
	}
}

