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

/* DICOM Definations */

#	define	IsThisaCmdMsg(ccc)			(ccc&0x01)
#	define	IsThisaDataMsg(ccc)			(!(ccc&0x01))
#	define	IsThisTheLastFragment(ccc)	(ccc&0x02)

#	define	PDU_ASSOCIATE_RQ			0x01
#	define	PDU_ASSOCIATE_AC			0x02
#	define	PDU_ASSOCIATE_RJ			0x03
#	define	PDU_PDATA					0x04
#	define	PDU_RELEASE_RQ				0x05
#	define	PDU_RELEASE_RP				0x06
#	define	PDU_ABORT_RQ				0x07

#	define	PDU_APPLICATION_CONTEXT		0x10
#	define	PDU_PRESENTATION_CONTEXT	0x20
#	define	PDU_PRESENTATION_CONTEXT_ACCEPT	0x21
#	define	PDU_ABSTRACT_SYNTAX			0x30
#	define	PDU_TRANSFER_SYNTAX			0x40
#	define	PDU_USER_INFORMATION		0x50

#	define	NO_REASON						0x01
#	define	APPLICATION_CONTEXT_NAME_BAD	0x02
#	define	CALLING_APPICATION_BAD			0x03
#	define	CALLED_APPLICATION_BAD			0x07
#	define	PROTOCOL_VERSION_BAD			0x02
#	define	TO_MUCH_NETWORK_TRAFFIC			0x01
#	define	TIME_LIMIT						0x02

#	define	DICOM_ULS_USER					0x01
#	define	DICOM_ULS_PROVIDER_ACSE			0x02
#	define	DICOM_ULS_PROVIDER_PRESENTATION	0x03
