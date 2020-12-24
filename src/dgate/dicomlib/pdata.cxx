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

//20100619   bcb    Fix gcc4 warnings, improve speed and made locals.
//20100717   mvh    Merged (note: locals means changes parameter names)
//20130807   mvh    Detect invalid Length returned to fix hanging thread issue
//		    Fixed >0 check on UINT32
//20140528   lsp    Kept member initialization only in constructors: not GNUC specific
//20160827   mvh    Added DARWIN change by bcb as described here:
//		    The speed problem in OSX dropped the files transfer from 110 seconds 
//		    to 1 second on my Apple.  I found this when trying to send the file that 
//		    crashed jpeg 2000 and jpegls that you sent me.  Just a one nanosecond delay 
//		    fixed a race. By the way, this test was done in non threaded mode. 
//		    So it is a race in the operating system. 
//20180316   mvh    Implemented buffer size change proposed by lsp
//20181123   mvh    Changed that to 16k, restored compatibility with carestream viewer
//20190109   mvh    Made MaxSubLen configurable from outside
//20190117   mvh    Default MaxSubLen to 4096 for if not set
//20190126   mvh    Reduced blocksize by 6; now DCMGETSCU etc work
//20201224   mvh    Added error handling

#	include	"dicom.hpp"


INT		MemoryBuffer	::	ReadBinary ( BYTE * lData, UINT Count )
	{
	INT	RetVal;

	if ( Index == Length )
			return ( -1 );	// end
	if((Length - Index)<Count )
		{
		memcpy((void*)lData, (void*)&this->Data[Index], Length - Index);
		RetVal = (INT) Length - Index;
		Index = Length;
		return ( RetVal );
		}
	memcpy((void*)lData, (void*)&this->Data[Index], Count);
	Index += Count;
	return ( Count );
	}

BOOL	MemoryBuffer	::	SendBinary ( BYTE *, UINT )
	{
	return ( FALSE );
	}

MemoryBuffer	::	MemoryBuffer (
	BYTE *lData,
	UINT lLength,
	BOOL	Destruct,
	UINT	lInEndian )
  : Data(lData),
    Length(lLength),
    Index(0),
    DestructFlag(Destruct)
	{
	SetIncomingEndian(lInEndian);
	
	this->Index = lLength;
	BufferSpace	*aBS = new BufferSpace;
	aBS->BufferSize = lLength;
	aBS->isTemp = TRUE;
	aBS->Data = lData;
	InSize += lLength;
	Incoming.Add(aBS);
	}

MemoryBuffer	::	~MemoryBuffer ()
	{
	if(DestructFlag)
		if(Data)
			delete Data;
	}

BOOL	LinkedBuffer	::	Fill(Buffer	&Link, UINT	Count)
	{
	LinkedTo = &Link;
	return(Buffer :: Fill(Count));
	}

INT		LinkedBuffer	::	ReadBinary(BYTE	*Data, UINT	Count)
	{
	if(LinkedTo->Read(Data, Count))
		return(Count);
	return(-1);
	}

BOOL	LinkedBuffer	::	SendBinary(BYTE	*Data, UINT	Count)
	{
	LinkedTo->Write(Data, Count);
	return (LinkedTo->Flush());
	}

UINT	LinkedBuffer	::	GetOutgoingSize()
	{
	return(OutSize);
	}


UINT	LinkedBuffer	::	GetIncomingSize()
	{
	return(InSize);
	}

BOOL	LinkedBuffer	::	Flush(Buffer	&Link, UINT	Count)
	{
	LinkedTo = &Link;
	return(Buffer :: Flush(Count));
	}

BOOL	PDataTF	::	ReadDynamic(Buffer	&Link)
	{
	UINT32		Count;

	if(!Length)
		{	
		Link >> Reserved1;
		Link >> Length;
		}
		
	//fprintf(stderr, "Reading P-DATA-TF: %d Length\n", Length);
	Count = Length;
	MsgStatus = 0;	// continue
	
	while ( (INT32)Count > 0)  // (INT32) 20130807
		{
		Link >> pdv.Length;
		if (pdv.Length > Count) return FALSE; // 20130807
		

		Link >> pdv.PresentationContextID;
		Link >> pdv.MsgHeader;
		//fprintf(stderr, "Reading PDV: %d Length, %d ID, %x Header\n",
		//	pdv.Length, pdv.PresentationContextID, pdv.MsgHeader);
		VRBuffer.Fill(Link, pdv.Length - 2);
		//fprintf(stderr, "Finished reading PDV\n");
		Count = Count - pdv.Length - sizeof(UINT32);
		Length = Length - pdv.Length - sizeof(UINT32);
		//fprintf(stderr, "Count = %d\n", Count);
		if(IsThisTheLastFragment(pdv.MsgHeader))
			{
			MsgStatus = 1;
			PresentationContextID = pdv.PresentationContextID;
			return ( TRUE );
			}				
		}
	//fprintf(stderr, "done reading PDV's\n");
	if(IsThisTheLastFragment(pdv.MsgHeader))
		MsgStatus = 1;

	PresentationContextID = pdv.PresentationContextID;

	return ( TRUE );
	}


BOOL	PDataTF	::	Write(Buffer	&Link)
	{
	// Generate P-DATA-TF Messages.
	UINT	BlockSize, SentSize, TotalSize, TLength;

//#define SHOW_TIME 1

#ifdef SHOW_TIME
	clock_t starttime;
	starttime = clock();
#endif

#ifdef DARWIN
	struct timespec myTime;
	myTime.tv_sec = 0;
	myTime.tv_nsec = 1;
#endif

	TotalSize = VRBuffer.GetOutgoingSize();
	BlockSize = MaxSubLen-6; //16384; // was 4096;
	SentSize = 0;
	TLength = Length;
	while(SentSize < TotalSize)
		{
		if((TotalSize - SentSize) < BlockSize)
			BlockSize = TotalSize - SentSize;
		if((BlockSize + SentSize) == TotalSize)
	//	if(BlockSize)
			MsgHeader = MsgHeader | 0x02;
		else
			MsgHeader = MsgHeader & 0x01;
		
		pdv.PresentationContextID = PresentationContextID;
		pdv.MsgHeader = MsgHeader;
		pdv.Length = BlockSize + 2;
		Length = pdv.Length + sizeof(UINT32);
		ItemType = 0x04;
		Reserved1 = 0;
		Link << ItemType;
		Link << Reserved1;
		Link << Length;
		Link << pdv.Length;
		Link << pdv.PresentationContextID;
		Link << MsgHeader;
		if(BlockSize)
			VRBuffer.Flush(Link, BlockSize);
		SentSize += BlockSize;
		Link.Flush();
#ifdef DARWIN
		nanosleep(&myTime, nullptr);
#endif		
		}
#ifdef SHOW_TIME
	printf("PDataTF::Write done at %d\n",(int)((clock() - starttime)/(CLOCKS_PER_SEC / 1000)));
#endif
	Length = TLength;
	return ( TRUE );		
	}

PDataTF	::	PDataTF()
  : ItemType(0x04),
    Reserved1(0),
    Length(0),
    VRBuffer(),
    MsgStatus(0),
    Endian(NATIVE_ENDIAN),
    pdv(),
    MaxSubLen(4096),
    PresentationContextID(0),
    MsgHeader(0) {}

PDataTF	::	~PDataTF()
	{

	}

