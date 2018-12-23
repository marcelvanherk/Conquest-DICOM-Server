/*
19980331	ljz	Added prototype for writing binary RTCs
20091029	ljz	Adjustments for 64-bits
20100619	bcb	Added no-copy to the RTC class.
20100717	bcb	Merged
20130429        lsp     Made no-copy generic (not only for __GNUC__)
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



// Run-Time-Classing of VR's

BOOL	Pack(BYTE, BYTE, UINT16 &);
BOOL	Pack(UINT16, BYTE &, BYTE &);


int MkBinaryRtc(char* pszTextIn, char* pszBinaryOut, BOOL bCarryDescriptions);


#	define	MAKEUINT32(xxx, yyy)	((UINT32) (((UINT32) xxx << 16) | yyy))

#pragma pack(1)
typedef	struct	_RTCElement
	{
	UINT16	Group;
	UINT16	Element;
	UINT16	TypeCode;
	char	*Description;
	}	RTCElement;

typedef	struct	_RTCElementBIN		// Used for reading and writing binary libraries
	{
	UINT16	Group;
	UINT16	Element;
	UINT16	TypeCode;
	char	cDummy[6];
	}	RTCElementBIN;
#pragma pack()

	
class	RTC
	{
	public:
		FixedArray < UINT32, RTCElement >		*TypeCodes;
		BOOL		CarryDescriptions;
	public:
					RTC (BOOL	CD = TRUE, char *filename = NULL);
					~RTC ();
		BOOL		AttachRTC ( char *filename );
		BOOL		AttachBinaryRTC ( char *filename );
		BOOL		AttachResourceRTC (char* pDict);
		BOOL		DetachRTC ();
		BOOL		RunTimeClass ( DICOMObject * );
		BOOL		RunTimeClass ( VR * );
		UINT16		RunTimeClass ( UINT16, UINT16, char * =NULL);
		BOOL		GetGroupElement(RTCElement* pEntry);
	private:// This will prevent it from being copied (it has a pointer)
		RTC(const RTC&);
		const	RTC & operator = (const RTC&);
	};



