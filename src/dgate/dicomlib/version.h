/*
200107017	ljz	Changed ImplementationClassUID to a valid UID: the Agfa dicom
			validation tool protested against the original 'UID'.
			Changed ImplementationVersion from "0.1B/WIN32" to "1.01/WIN32"
20020416        mvh     changed IMPLEMENTATION_VERSION_STRING to 1.3.11/WIN32
20021018        mvh     changed IMPLEMENTATION_VERSION_STRING to 1.4.0/WIN32
20030424        ljz     changed IMPLEMENTATION_VERSION_STRING to 2.0.1/WIN32
20030709        mvh     changed IMPLEMENTATION_VERSION_STRING to 1.4.2/WIN32
20030815        mvh     changed IMPLEMENTATION_VERSION_STRING to 1.4.3/WIN32
20040402        mvh     changed IMPLEMENTATION_VERSION_STRING to 1.4.4/WIN32
20040530        mvh     changed IMPLEMENTATION_VERSION_STRING to 1.4.5/WIN32
20040713        mvh     changed IMPLEMENTATION_VERSION_STRING to 1.4.6/WIN32
20041101        mvh     changed IMPLEMENTATION_VERSION_STRING to 1.4.7/WIN32
20050901        mvh     changed IMPLEMENTATION_VERSION_STRING to 1.4.9/WIN32
20060103        mvh     changed IMPLEMENTATION_VERSION_STRING to 1.4.10/WIN32
20060311        mvh     changed IMPLEMENTATION_VERSION_STRING to 1.4.11/WIN32
20060708        mvh     changed IMPLEMENTATION_VERSION_STRING to 1.4.12/WIN32
20070307        mvh     changed IMPLEMENTATION_VERSION_STRING to 1.4.13/WIN32
20080831        mvh     changed IMPLEMENTATION_VERSION_STRING to 1.4.14/WIN32
20100713     bcb+mvh    changed IMPLEMENTATION_VERSION_STRING to 1.4.16/WIN32; added osx
20160221        mvh     changed IMPLEMENTATION_VERSION_STRING to 1.4.19/WIN32
20200311	mvh	changed IMPLEMENTATION_VERSION_STRING to 1.5.0/WIN32
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

/* Version related information
 *
 * This file contains the version number/Class that will be embedded not
 * only in the executable, but in the default PDU Service transfer Class.
 *
 */
//#	define	IMPLEMENTATION_CLASS_STRING		"UCDMC/gcc2.5.8/DECMIPS/MHO"
#	define	IMPLEMENTATION_CLASS_STRING		"1.2.826.0.1.3680043.2.135.1066.101"
#ifdef	WINDOWS
//#	define	IMPLEMENTATION_VERSION_STRING	"0.1B/WIN32"
#	define	IMPLEMENTATION_VERSION_STRING	"1.5.0/WIN32"
#else
#ifdef	ULTRIXMIPS
#	define	IMPLEMENTATION_VERSION_STRING	"1.5.0/UMIPS"
#else
#ifdef	SOLARIS
#	define	IMPLEMENTATION_VERSION_STRING	"1.5.0/SL5"
#else
#ifdef	MAC
#	define	IMPLEMENTATION_VERSION_STRING	"1.5.0/MAC"
#else
#ifdef	SUNOSSPARC
#	define	IMPLEMENTATION_VERSION_STRING	"1.5.0/SUNOS"
#else
#ifdef	DARWIN
#	define	IMPLEMENTATION_VERSION_STRING	"1.5.0/OSX"
#else
#	define	IMPLEMENTATION_VERSION_STRING	"1.5.0/OTHER"
#endif
#endif
#endif
#endif
#endif
#endif

