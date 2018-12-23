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

#	ifdef	DEBUG_VERSION
#	define	MAX_SEGMENTS	10000

void	*operator new(size_t);
void	operator delete(void *);
int		snew(char*, int);
int		sdelete(char*, int);
void	MemCatch(char*, int);
#	define	new		(snew(__FILE__, __LINE__)) ? NULL : new
#	define	delete	(sdelete(__FILE__, __LINE__)) ? 0 : delete

BOOL	InitMemoryWatch();
BOOL	CloseMemoryWatch();
#else
#	define	InitMemoryWatch()
#	define	CloseMemoryWatch()
#endif

class	SafeMemory
	{
	public:
		SafeMemory ()	{ InitMemoryWatch(); };
		~SafeMemory ()	{ CloseMemoryWatch(); };
	};


