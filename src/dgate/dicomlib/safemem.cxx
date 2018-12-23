/*
20010720	ljz     Changed 'printf' to 'fprintf(stderr,...)'
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

#	include	"dicom.hpp"

#ifdef	DEBUG_VERSION

SafeMemory	SF;	// For the init / de-init
#	include	<stddef.h>
#	include	<stdlib.h>


typedef	struct	_MemAction
	{
	int		AllocOrDelete;
	int		LineNumber;
	char	*FileName;
	int		MemSize;
	void	*vptr;
	struct	_MemAction	*Related;
	struct	_MemAction	*Prev, *Next;
	}	MemAction;

static	char	TempFileName[1024];
static	int		TempLineNumber;
MemAction	*MemorySegments;

MemAction	*CreateMemAction (
	char	*FileName,
	int		LineNumber,
	int		Action,
	int		Size,
	void	*vptr)
	{
	MemAction	*MA;

	MA = (MemAction*)malloc(sizeof(MemAction));
	memset((void*)MA, 0, sizeof(MemAction));
	MA->FileName = (char*)malloc(strlen(FileName)+1);
	strcpy(MA->FileName, FileName);
	MA->AllocOrDelete = Action;
	MA->vptr = vptr;
	MA->MemSize = Size;
	MA->LineNumber = LineNumber;
	return ( MA );
	}

BOOL	FreeMemAction (
	MemAction	*MA )
	{
	/* very destructive */
	if ( ! MA )
		return ( FALSE );
	FreeMemAction(MA->Prev);
	FreeMemAction(MA->Next);
	FreeMemAction(MA->Related);
	if ( MA->FileName )
		free (MA->FileName);
	free( MA );
	return ( TRUE );
	}

BOOL	InitMemoryWatch()
	{
	UINT	Index;

	MemorySegments = NULL;

	return ( TRUE );
	}

static
BOOL
__s_isascii(	char	ch	)
	{
	if ( (ch>='A')&&(ch<='Z'))
		return ( TRUE );
	if ( (ch>='a')&&(ch<='z'))
		return ( TRUE );
	if ( (ch>='0')&&(ch<='9'))
		return ( TRUE );
	return ( FALSE );
	}

BOOL
PrintUnfreed(void	*data, UINT	Size)
	{
	UINT32	*p32;
	UINT16	*p16;
	UINT	Index;
	char	*pc;

	switch ( Size )
		{
		case	2:	p16 = (UINT16*) data;
					fprintf(stderr, "%4.4 / %d ",*p16,*p16);
					break;
		case	4:	p32 = (UINT32*) data;
					fprintf(stderr, "%8.8x / %d ",*p32,*p32);
					break;
		}
	Index = 0;
	pc = (char*)data;
	while ( Index < Size )
		{
		if ( Index > 50 )
			break;
		if(__s_isascii(*pc))
			fprintf(stderr, "%2.2x(%c),", *pc, *pc);
		else
			fprintf(stderr, "%2.2x(.),", *pc);
		++pc;
		++Index;
		}
	fprintf(stderr, "\n");
	return ( TRUE );
	}

BOOL	CloseMemoryWatch()
	{
	UINT	Index;
	UINT	Count;
	MemAction	*MA;

	Index = 0; Count = 0;

	MA = MemorySegments;

	while ( MA )
		{
		fprintf (stderr, "%s/%d Memory : %8.8x Size : %8.8x : %d Not Freed\n",
				MA->FileName,
				MA->LineNumber,
				MA->vptr,
				MA->MemSize);
		fflush ( stderr );
		PrintUnfreed(MA->vptr, MA->MemSize);
		MA = MA->Next;
		++Count;
		}
	FreeMemAction(MemorySegments);	
	return ( Count );	
	}

BOOL
RegisterMemory (size_t	Size, void	*vptr)
	{
	UINT	Index;
	MemAction	*MA;

	MA = CreateMemAction(TempFileName, TempLineNumber, 1, Size, vptr);
	MA->Next = MemorySegments;
	if (MemorySegments)
		MemorySegments->Prev = MA;
	MemorySegments = MA;
	return ( FALSE );
	}

BOOL
UnRegisterMemory ( void *vptr )
	{
	UINT	Index;
	MemAction	*MA;

	MA = MemorySegments;

	while ( MA )
		{
		if ( MA->vptr == vptr )
				{
				if ( MemorySegments == MA )
					MemorySegments = MA->Next;
				if ( MA->Prev )
					MA->Prev->Next = MA->Next;
				if ( MA->Next )
					MA->Next->Prev = MA->Prev; 
				MA->Next = MA->Prev = NULL;
				FreeMemAction(MA);
				return ( TRUE );
				}
		MA = MA->Next;
		}
	return ( FALSE );
	}
#undef	new
#undef	delete

void	* operator	new(size_t	x)
	{
	void	*vptr;

	if ( ! x )
		{
		fprintf(stderr, "Attempt to allocate zero length block in %s/%d\n",
			TempFileName, TempLineNumber);
		return ( NULL );
		}

	vptr = malloc(x);
	RegisterMemory (x, vptr);
	return ( vptr );
	}

void	 operator	delete	( void	*vptr )
	{
	if ( ! vptr )
		{
		fprintf(stderr, "Attempt to delete NULL pointer %s/%d\n",
			TempFileName, TempLineNumber);
		return;
		}
	if (!UnRegisterMemory(vptr))
		{
		fprintf(stderr, "Possible Duplicate Delete in File: %s Line %d vptr=%x\n",
			TempFileName, TempLineNumber, vptr);
		}
	free(vptr);
	}

int	snew(
	char	*file,
	int		line)
	{
	strcpy(TempFileName, file);
	TempLineNumber = line;
	return(0);	// make the else part go..
	}

int	 sdelete	(
	char	*file,
	int		line )
	{
	strcpy(TempFileName, file);
	TempLineNumber = line;
	return(0);	// make the else part go..
	}


#endif

