// mvh 20140910 added hasTrn flag for Aria issue

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
 *	A-ASSOCIATE-AC Class.  
 *
 * Base Classes:
 *
 *	PresentationContextAccept
 *
 *
 *
 *********************************************************************/
		
class	PresentationContextAccept
	{
	private:
		BYTE							ItemType;	// 0x21
		BYTE							Reserved1;
		UINT16							Length;
	public:
		BYTE							PresentationContextID;
	private:
		BYTE							Reserved2;
	public:
		BYTE							Result;
		BYTE		hasTrn;
	private:
		BYTE							Reserved4;
	public:
//		AbstractSyntax				AbsSyntax;
		TransferSyntax				TrnSyntax;
					PresentationContextAccept();
					PresentationContextAccept(/*AbstractSyntax &, */TransferSyntax &);
					~PresentationContextAccept();
//		void		SetAbstractSyntax(AbstractSyntax	&);
		void		SetTransferSyntax(TransferSyntax	&);
		void		SetResult(BYTE	Res) { Result = Res; };
		BYTE		GetResult()	{ return ( Result ); };
		BOOL		Write(Buffer &);
		BOOL		Read(Buffer &);
		BOOL		ReadDynamic(Buffer	&);
		UINT32	Size();
		
	};

class	AAssociateAC
	{
	private:
		BYTE		ItemType;		// 0x02
		BYTE		Reserved1;
		UINT32	Length;
		UINT16	ProtocolVersion;	// 0x01
		UINT16	Reserved2;
	public:
		BYTE		CalledApTitle[17];	// 16 bytes transfered
		BYTE		CallingApTitle[17];	// 16 bytes transfered
		BYTE		Reserved3[32];
		ApplicationContext					AppContext;
		Array<PresentationContextAccept>	PresContextAccepts;
		UserInformation						UserInfo;
	public:
					AAssociateAC();
					AAssociateAC(BYTE	*, BYTE	*);
		virtual			~AAssociateAC();
		void		SetCalledApTitle(BYTE	*);
		void		SetCallingApTitle(BYTE	*);
		void		SetApplicationContext(ApplicationContext &);
		void		AddPresentationContextAccept(PresentationContextAccept &);
		void		SetUserInformation(UserInformation &);
		BOOL		Write(Buffer &);
		BOOL		Read(Buffer &);
		BOOL		ReadDynamic(Buffer	&);
		UINT32		Size();
	};			


