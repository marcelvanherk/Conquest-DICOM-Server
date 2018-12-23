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
 * PDU Service Classes: 
 *	A-ASSOCIATE-RJ Class.  
 *	A-RELEASE-RQ Class.
 * 	A-RELEASE-RP Class.
 *	A-ABORT-RQ Class.
 *
 * Base Classes:
 *
 *
 *
 *
 *********************************************************************/
		

class	AAssociateRJ
	{
	private:
		BYTE		ItemType;		// 0x03
		BYTE		Reserved1;
		UINT32		Length;
		BYTE		Reserved2;
	public:
		BYTE		Result;
		BYTE		Source;
		BYTE		Reason;

					AAssociateRJ();
					AAssociateRJ(BYTE, BYTE, BYTE);
		virtual		~AAssociateRJ();
		BOOL		Write(Buffer &);
		BOOL		Read(Buffer &);
		BOOL		ReadDynamic(Buffer	&);
		UINT32		Size();
	};			


class	AReleaseRQ
	{
	private:
		BYTE		ItemType;		// 0x04
		BYTE		Reserved1;
		UINT32		Length;
		UINT32		Reserved2;
	public:
					AReleaseRQ();
		virtual		~AReleaseRQ();
		BOOL		Write(Buffer &);
		BOOL		Read(Buffer &);
		BOOL		ReadDynamic(Buffer	&);
		UINT32		Size();
	};			

class	AReleaseRP
	{
	private:
		BYTE		ItemType;		// 0x06
		BYTE		Reserved1;
		UINT32		Length;
		UINT32		Reserved2;
	public:
					AReleaseRP();
		virtual		~AReleaseRP();
		BOOL		Write(Buffer &);
		BOOL		Read(Buffer &);
		BOOL		ReadDynamic(Buffer	&);
		UINT32		Size();
	};			

class	AAbortRQ
	{
	private:
		BYTE		ItemType;		// 0x07
		BYTE		Reserved1;
		UINT32		Length;
		BYTE		Reserved2;
		BYTE		Reserved3;
	public:
		BYTE		Source;
		BYTE		Reason;

					AAbortRQ();
					AAbortRQ(BYTE, BYTE);
		virtual		~AAbortRQ();
		BOOL		Write(Buffer &);
		BOOL		Read(Buffer &);
		BOOL		ReadDynamic(Buffer	&);
		UINT32		Size();
	};			

