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

// This file is used on UNIX systems that have a brain damaged
// Linker (which is just about all of them).  It creates a much
// smaller library/executable file size.
// Useful primarilly after you have implemented any changes and verified
// They work.  (ie, I use this file to produce the distributed binaries
// of the library; I do not use it while I'm adding various pieces..)
#	include	"dicom.hpp"
#	include	"buffer.cxx"
#	include	"socket.cxx"
#	include	"deivr.cxx"
#	include	"aarq.cxx"
#	include	"aaac.cxx"
#	include	"aarj.cxx"
#	include	"pdata.cxx"
#	include	"pdu.cxx"
#	include	"trnsyn.cxx"
#	include	"endian.cxx"
#	include	"safemem.cxx"
#	include	"uniq.cxx"
#	include	"util.cxx"
#	include	"dimsec.cxx"
#	include	"dimsen.cxx"
#	include	"verify.cxx"
#	include	"storage.cxx"
#	include	"qrsop.cxx"
#	include	"flpdu.cxx"
#	include	"filepdu.cxx"
#	include	"rtc.cxx"
