/* mvh 20001106: Use memcpy instead of ByteCopy */
/* mvh 20071031: Temp printf allocation errors */
/* mvh 20071102: Send allocation errors to DicomError */
/* mvh 20081016: Default buffer size from 1024 to 10240 */
/* bcb 20100619: Fix gcc4 warnings and improve speed. */
/* mvh 20100717: Merged */
/* mvh 20130416: Set DEFAULT_BREAK_SIZE for UNIX to 8192 to fix not understood 
                 ubuntu12.04 problem with zeros tranmitted after 1st 64k 
Spectra-0009: Wed, 5 Feb 2014 10:57:52 +0000: Fix cppcheck bug #1
Spectra-0019:	Thu, 6 Mar 2014 17:02:42 -0300: Fix mismatched new/delete in buffer.cxx
20140528    lsp  Kept member initialization only in constructors: not GNUC specific
20160221    mvh  Clear data if buffer read fails; avoids infinite loop if connection is broken
20180316    mvh  Implemented buffer size changes proposed by lsp
20190318    mvh  DEFAULT_BREAK_SIZE back to 8192 for linux
20201224    mvh  Added error handling
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

/************************************************************
 * Buffer Class
 *
 ************************************************************/


#	include	"dicom.hpp"

#ifdef UNIX
#	define	DEFAULT_BREAK_SIZE	8192
#else
#	define	DEFAULT_BREAK_SIZE	262144  // was 32600
#endif

BufferSpace	::	BufferSpace(UINT	Alloc)
  : BufferSize(0),
    Index(0),
    isTemp(FALSE),
    Data(NULL)
	{
//printf("buffer allocating %d bytes\n", Alloc);
		if ( Alloc )
		{
			Data = new BYTE [ Alloc ];
			if (!Data) DicomError(DCM_ERROR_MEMORY, "Bufferspace: out of memory allocating %d bytes\n", Alloc);
			BufferSize = Alloc;
		}
	}

BufferSpace	::	BufferSpace()
  : BufferSize(0),
    Index(0),
    isTemp(FALSE),
    Data(NULL) {}

BufferSpace	::	~BufferSpace()
	{
	if ( !isTemp )
		if ( Data )
			delete [] Data;
	}

Buffer	::	Buffer()
  : BreakSize(DEFAULT_BREAK_SIZE),
    InEndian(NATIVE_ENDIAN),
    OutEndian(NATIVE_ENDIAN),
    InSize(0),
    OutSize(0),
    Incoming(),
    Outgoing() {}

Buffer	::	~Buffer()
	{
	
	while ( Incoming.GetSize() )
		{
//		if ( Incoming[0].Data )
//			delete Incoming[0].Data;
//		Incoming[0].Data = NULL;
		
		delete Incoming.Get(0);
		Incoming.RemoveAt( 0 );
		}
	while ( Outgoing.GetSize() )
		{
//		if ( Outgoing[0].Data )
//			delete Outgoing[0].Data;
//		Outgoing[0].Data = NULL;
		
		delete Outgoing.Get(0);
		Outgoing.RemoveAt ( 0 );
		}
	Incoming.ClearType = 0;
	Outgoing.ClearType = 0;
//	Incoming.dt.Data = NULL;
//	Outgoing.dt.Data = NULL;
	InEndian = NATIVE_ENDIAN;
	}

BOOL	Buffer	::	SetIncomingEndian(UINT	Endian)
	{
	InEndian = Endian;
	return ( TRUE );
	}

BOOL	Buffer	::	SetOutgoingEndian(UINT	Endian)
	{
	OutEndian = Endian;
	return ( TRUE );
	}

BOOL	Buffer	::	SetBreakSize(UINT	ToSize)
	{
	BreakSize = ToSize;
	return ( TRUE );
	}

Buffer	&	Buffer	::	operator	>>	(BYTE	&x)
	{
	BYTE	s[2];

	Read(s, 1);
	x = s[0];
//	fprintf(stderr, "%2.2x ", x);
	return (*this);
	}

/*************************************************
	BufferSpace	BS;

	if (InSize <= 0)
		{
		ReadBlock();
		}
	if (InSize <= 0)
		{
		x = 0;
		return ( * this );
		}
	BS = Incoming.Get ( 0 );
	if ( ! BS.Data )
		{
		InSize -= BS.Size;
		Incoming.RemoveAt ( 0 );
		x = 0;
		return ( * this );
		}
	x = BS.Data[BS.Index];
	++BS.Index;
	--InSize;
	if(BS.Index == BS.Size)
		Incoming.RemoveAt ( 0 );
	else
		Incoming.Get ( 0 ) = BS;
	BS.isTemp = TRUE;
	return ( * this );
	}
***************************************************/

Buffer	&	Buffer	::	operator	>>	(UINT16	&x)
	{
	BYTE	b1, b2;
	BYTE	*op;

	(*this) >> b1;
	(*this) >> b2;

	if(InEndian == NATIVE_ENDIAN)
		{
		op = (BYTE*) &x;
		(*op) = b1;++op;
		(*op) = b2;	
		}
	else
		{
		op = (BYTE*) &x;
		(*op) = b2;++op;
		(*op) = b1;	
		}

	return ( * this );
	}
/*
static	void	Dump(void	*vp, int	size)
	{
	unsigned	char	*cvp;

	cvp = (unsigned char *) vp;
	while(size)
		{
		fprintf(stderr, "%2.2x ", *cvp);
		++cvp;
		--size;
		}
	}
*/

Buffer	&	Buffer	::	operator	>>	(UINT32	&x)
	{
	UINT16	w1, w2;
	BYTE	*op, *ip;

	(*this) >> w1;
	(*this) >> w2;

	if(InEndian == NATIVE_ENDIAN)
		{
		op = (BYTE *) &x;
		ip = (BYTE *) &w1;
		(*op) = (*ip);++op;++ip;
		(*op) = (*ip);++op;++ip;
		ip = (BYTE *) &w2;
		(*op) = (*ip);++op;++ip;
		(*op) = (*ip);
		}
	else
		{
		op = (BYTE *) &x;
		ip = (BYTE *) &w2;
		(*op) = (*ip);++op;++ip;
		(*op) = (*ip);++op;++ip;
		ip = (BYTE *) &w1;
		(*op) = (*ip);++op;++ip;
		(*op) = (*ip);
		}
	
	return ( * this );
	}

Buffer	&	Buffer	::	operator	<<	(BYTE	&x)
	{
	BYTE	s[2];
	
//	fprintf(stderr, "Buffer :: << Byte\n");fflush(stderr);
	s[0] = x;
	Write(s, 1);
//	fprintf(stderr, "returning\n");fflush(stderr);
	return (*this);
	}

/************************************************
	BufferSpace	BS;

	if ( ! Outgoing.GetSize() )
		{
		BufferSpace	ABS( BreakSize );
		Outgoing.Add ( ABS );
		ABS.Data = NULL;
		}
	BS = Outgoing.Get ( Outgoing.GetSize() - 1 );
	if(BS.Index == BS.Size)
		{
		BufferSpace	ABS( BreakSize );
		Outgoing.Add ( ABS );
		ABS.Data = NULL;
		BS = Outgoing.Get ( Outgoing.GetSize() - 1);
		}
	BS.Data[BS.Index] = x;
	++BS.Index;
	Outgoing.Get ( Outgoing.GetSize() - 1 ) = BS;
	BS.isTemp = TRUE;
	++OutSize;
	return (*this );
	}
*************************************************/	
Buffer	&	Buffer	::	operator	<<	(UINT16	&x)
	{
	BYTE	b1, b2;
	BYTE	*ip;

	ip = (BYTE*)&x;
	b1 = (*ip);++ip;
	b2 = (*ip);
	if(OutEndian == NATIVE_ENDIAN)
		{
		(*this) << b1;
		(*this) << b2;
		}
	else
		{
		(*this) << b2;
		(*this) << b1;
		}
	return ( * this );
	}

Buffer	&	Buffer	::	operator	<<	(UINT32	&x)
	{
	UINT16	w1, w2;
	UINT16	*ip;

	ip = (UINT16*)&x;
	w1 = (*ip);++ip;
	w2 = (*ip);
	if(OutEndian == NATIVE_ENDIAN)
		{
		(*this) << w1;
		(*this) << w2;
		}
	else
		{
		(*this) << w2;
		(*this) << w1;
		}
	return ( * this );
	}

BOOL	Buffer	::	Flush()
	{
	BufferSpace	*BS;

	
	//fprintf(stderr, "Buffer :: Flush() %d buffers\n", Outgoing.GetSize());fflush(stderr);
	while ( Outgoing.GetSize() )
		{
		BS = Outgoing.Get ( 0 );
		if(BS->Index)
			if (!SendBinary(BS->Data, BS->Index)) return FALSE;
		
		delete Outgoing.Get(0);
		Outgoing.RemoveAt ( 0 );
		}
	OutSize = 0;
//	fprintf(stderr, "returning TRUE\n");fflush(stderr);
	return ( TRUE );
	}

BOOL	Buffer	::	Flush(UINT	Bytes)
	{
	BufferSpace	*BS;

	//fprintf(stderr, "FLUSH: OutSize=%d, Bytes=%d\n", OutSize,Bytes);

	if(Bytes > OutSize)
		return(Flush());

	
	OutSize -= Bytes;

	while(Bytes)
		{
		if(!Outgoing.GetSize())
			{
			//fprintf(stderr, "Attempting to flush more bytes than input, OutSize=%d Bytes=%d\n", OutSize,Bytes);
			OutSize = 0;
			return ( FALSE );
			}

		BS = Outgoing.Get( 0 );
		if(BS->Index > Bytes)
			{
			if (!SendBinary(BS->Data, Bytes)) return FALSE;
			memcpy(BS->Data, &BS->Data[Bytes], BS->Index - Bytes);
			BS->Index = BS->Index - Bytes;
			Bytes = 0;
			return ( TRUE );
			}
		else
			{
			if(BS->Index)
				{
				if (!SendBinary(BS->Data, BS->Index)) return FALSE;
				Bytes -= BS->Index;
				delete Outgoing.Get(0);
				Outgoing.RemoveAt ( 0 );
				}
			else
				{
				delete Outgoing.Get(0);
				Outgoing.RemoveAt ( 0 );
				}
			}
		}
	
	return ( TRUE );
	}

				
BOOL	Buffer	::	Kill(UINT	Bytes)
	{
	BOOL	Ret;
	BYTE	*Data = new BYTE[Bytes+1];
        if (!Data) DicomError(DCM_ERROR_MEMORY, "Bufferspace: out of memory allocating %d bytes\n", Bytes+1);

	Ret = Read(Data, Bytes);
	delete [] Data;
	return ( Ret );
	}
	
/*
	BufferSpace	*BS;
	InSize -= Bytes;
	while ( Incoming.GetSize() )
		{
		BS = Incoming.Get ( 0 );
		if ( (BS->Index + Bytes) > BS->BufferSize)
			Bytes -= (BS->BufferSize - BS->Index);
		else
			{
			// NOTE, Used to be other way around...
			BS->Index += Bytes;
			Bytes = 0;
			return ( TRUE );
			}
		delete Incoming.Get(0);
		Incoming.RemoveAt ( 0 );
		}
	
	return ( FALSE );	// not got that many bytes yet, but go negative
	}
*/

BOOL	Buffer	::	Fill(UINT	Bytes)
	{
	UINT	ECount;

	SetBreakSize(Bytes);
	
	ECount = InSize + Bytes;
	while(InSize < ECount)
		{
		//fprintf(stderr, "InSize: %d\n", InSize);
		//printf("Fill %d, %d\n", ECount, Bytes);
		SetBreakSize(ECount - InSize);

		if(!ReadBlock())
			return ( FALSE );
		//fprintf(stderr, "InSize: %d\n", InSize);
		}
	SetBreakSize(DEFAULT_BREAK_SIZE);
	return ( TRUE );
	}

		
BOOL	Buffer	::	Read(BYTE	*Data, UINT	Bytes)
	{
	BufferSpace	*BS;

	//SetBreakSize(Bytes);
	while(InSize < Bytes)
		if(!ReadBlock())
			{
			memset(Data, 0, Bytes);
			return ( FALSE );
			}

	while ( Bytes )
		{
		BS = Incoming.Get ( 0 );
		if(BS->Index == BS->BufferSize)
			{
			delete BS;
			Incoming.RemoveAt( 0 );
			}
		else
			{
			if((BS->Index + Bytes) <= BS->BufferSize)
				{
				memcpy(Data, &BS->Data[BS->Index], Bytes);
				BS->Index += Bytes;
				InSize -= Bytes;
				return ( TRUE );
				}
			else
				{
				memcpy(Data, &BS->Data[BS->Index], BS->BufferSize - BS->Index);
				Bytes -= (BS->BufferSize - BS->Index);
				Data += (BS->BufferSize - BS->Index);
				InSize -= (BS->BufferSize - BS->Index);
				delete BS;
				Incoming.RemoveAt ( 0 );
				}
			}
		}
 	
	return ( FALSE );	// should not get here
	}

BOOL	Buffer	::	Write(BYTE	*Data, UINT	Bytes)
	{
	BufferSpace	*BS;
	UINT		Count;

	
	if ( !Outgoing.GetSize() )
		{
		BufferSpace	*ABS	=	new BufferSpace(BreakSize);
		Outgoing.Add ( ABS );
		}
	while(Bytes)
		{
		BS = Outgoing.Get ( Outgoing.GetSize() - 1);
		Count = BS->BufferSize - BS->Index;
		if ( Count )
			{
			if (Count <= Bytes )
				{
				memcpy(&BS->Data[BS->Index], Data, Count);
				Bytes -= Count;
				Data += Count;
				BS->Index += Count;
				
				
				OutSize += Count;
				BufferSpace	*ABS	=	new BufferSpace(BreakSize);
				Outgoing.Add ( ABS );
				
				}
			else
				{
				memcpy(&BS->Data[BS->Index], Data, Bytes);
				BS->Index += Bytes;
				OutSize += Bytes;
				Bytes = 0;
				return ( TRUE );
				}
			}
		else
			{
			//BufferSpace	ABS(BreakSize);
			BufferSpace	*ABS	=	new BufferSpace(DEFAULT_BREAK_SIZE);
			Outgoing.Add( ABS );
			
			}
		}
	return ( TRUE );
	}

BOOL	Buffer	::	ReadBlock()
	{
	int		Length, ILength;
	BYTE	s[102400];	// was 10240

	if(BreakSize<102400)
		ILength = BreakSize;
	else
		ILength = 102400;
	
	//printf("ReadBlock(%d)\n", Length);
	
	while(!(Length = ReadBinary((BYTE*)s, ILength)))
		;
	
	//fprintf(stderr, "Block Size = %d\n", ABS.Size);
	if(Length == (-1))
		{
		//fprintf(stderr, "Error from ReadBinary()\n");
		//printf("ReadBinary:-1\n");
		return (FALSE);	// Error from ReadBinary()
		}
	
	BufferSpace	*ABS	=	new BufferSpace(Length);
	//fprintf(stdout, "Length=%d\n", Length);
	memcpy((void*)ABS->Data, (void*)s, Length);

	InSize += ABS->BufferSize;
	Incoming.Add(ABS);
	
	return ( TRUE );
	}

BOOL	Buffer	::	Poll()
	{
	/*
	BufferSpace	ABS(BreakSize);

	ABS.BufferSize = ReadBinary(ABS.Data, BreakSize);
	if(!ABS.BufferSize)
		return ( TRUE );	// no data available
	if(ABS.BufferSize == (-1))
		return (FALSE);	// Error from ReadBinary()
	InSize += ABS.BufferSize;
	Incoming.Add(ABS);
	ABS.Data = NULL;
	*/
	return ( TRUE );
	}
/*
class	TestBuffer	:	public	Buffer
	{

	BOOL	SendBinary(BYTE	*Data, UINT	Count)
		{
		UINT	Index;
		fprintf(stderr, "Sending: %d Bytes\n", Count);
		Index = 0;
		while ( Index < Count )
			{
			fprintf(stderr, "%2.2x ", Data[Index]);
			++Index;
			if(!(Index%16))
				fprintf(stderr, "\n");
			}
		fprintf(stderr, "\n");
		fflush(stderr);
		return ( TRUE );
		}
		
	INT	ReadBinary(BYTE	*Data, UINT	Count)
		{
		fgets((char *) Data, Count, stdin);
		return ( strlen((char*) Data));
		}
	};

*/
