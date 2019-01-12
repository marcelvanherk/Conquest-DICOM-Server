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
//20190109   mvh    Made MaxSubLen configurable from outside

/***************************************************************************
 *
 * University of California, Davis
 * UCDMC DICOM Network Transport Libraries
 * Version 0.1 Beta
 *
 * Technical Contact: mhoskin@ucdavis.edu
 *
 ***************************************************************************/

/****************************************************************************
 *
 * Abstract Base Class for P-DATA Service
 *
 *
 ****************************************************************************/

/*20100619    bcb Added no-copy to the classes and init list to LinkedBuffer.
  20100717    mvh Merged
  20140528    lsp Private copy constructors and member initialization are not GNUC specific
*/

class	LinkedBuffer	:
	public	Buffer
	{
		Buffer	*LinkedTo;
	public:
			LinkedBuffer():LinkedTo(NULL) {};
			~LinkedBuffer()	{ LinkedTo = NULL; };
		BOOL	Fill(Buffer	&Link, UINT	Count);
		BOOL	Flush(Buffer &Link, UINT	Count);
		UINT	GetOutgoingSize();
		UINT	GetIncomingSize();
		virtual	INT		ReadBinary(BYTE	*Data, UINT	Count);
		virtual	BOOL	SendBinary(BYTE *Data, UINT Count);
	private:// This will prevent it from being copied (it has a pointer)
		LinkedBuffer(const LinkedBuffer&);
		const	LinkedBuffer & operator = (const LinkedBuffer&);
	};

class	MemoryBuffer	:
	public	LinkedBuffer
	{
	private:
		BYTE		*Data;
		UINT		Length;
		UINT		Index;
		BOOL		DestructFlag;
	public:
		INT		ReadBinary(BYTE	*Data, UINT	Count);
		BOOL	SendBinary(BYTE *Data, UINT Count);
				MemoryBuffer ( BYTE *Data, UINT Length, BOOL Destruct,
								UINT Endian);
				~MemoryBuffer ();
	private:// This will prevent it from being copied (it has a pointer)
		MemoryBuffer(const MemoryBuffer&);
		const	MemoryBuffer & operator = (const MemoryBuffer&);
	};


class	PDV
	{
	public:
		UINT32	Length;
		BYTE	PresentationContextID;
		BYTE	MsgHeader;
	};

class	PDataTF
	{
	private:
		BYTE			ItemType;	// 0x04
		BYTE			Reserved1;
	public:
		UINT32			Length;
	public:
		LinkedBuffer	VRBuffer;

	public:
		INT		MsgStatus;
		UINT	Endian;
		UINT	MaxSubLen;
		PDV		pdv;
		BYTE	PresentationContextID;
		BYTE	MsgHeader;

		BOOL	ReadDynamic(Buffer	&Link);
		BOOL	Write(Buffer	&Link);

				PDataTF();
		virtual		~PDataTF();
	};

