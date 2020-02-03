/*
20050125	ljz	Added 'ReplaceAt()' to the 'Array' class
20100619	bcb	Fix gcc4 warnings and improve speed.
20130429        lsp     Corrected (unused) printf format string
20130812        mvh     Removed non-thread safe caching of last value
			Does not seem to affect speed, throughput remains 
			at 320 MB/s receive with 3 senders and receivers 
			in same dgate on 8-core machine
20131016        mvh	Merged
20190919        lsp     Replaced linked list by plain array for performance reasons
20190926        lsp     Fixed issue with RemoveAt() followed by Add()
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

#include <assert.h>
#include <memory>

#define CHUNK_INCREMENT 64

/*
template	<class	DATATYPE>
	Array<DATATYPE>	::	Array ()
	{
	ArraySize = 0;
	first = NULL;
	last = NULL;
	ClearType = 1;
	}
*/

template	<class	DATATYPE>
	Array<DATATYPE>	::	Array (UINT	CT)
#ifdef __GNUC__ //Faster with member initialization.
:first(NULL),
first_dyn(NULL),
ArraySize(0),
ArraySizeAllocated(0),
ClearType(CT) {}
#else
	{
	ArraySize = 0;
        ArraySizeAllocated = 0;
	first = NULL;
        first_dyn = NULL;
	ClearType = CT;
	}
#endif

template	<class	DATATYPE>
	Array<DATATYPE>	::	~Array()
	{
	if(ClearType == 1)
		{
		while(ArraySize)
			RemoveAt(0);
                first = NULL;
                first_dyn = NULL;                        
		}
	}

template	<class	DATATYPE>
DATATYPE	&	Array<DATATYPE>	::	Add(DATATYPE	&Value)
	{
        if (ArraySize+1>ArraySizeAllocated)
        { DATATYPE **first_realloc = (DATATYPE**)malloc(sizeof(DATATYPE*)*(ArraySize+CHUNK_INCREMENT));
          if (!first_realloc)
            throw std::bad_alloc();

          memcpy(first_realloc, first_dyn, ArraySize*sizeof(DATATYPE*));
          free(first);
          first = first_realloc;
          first_dyn = first;
          ArraySizeAllocated = ArraySize+CHUNK_INCREMENT;
        }
        DATATYPE *dtptr = new DATATYPE;
        *dtptr = Value;
        first_dyn[ArraySize] = dtptr;


	++ArraySize;
	return ( Value );
	}

template	<class	DATATYPE>
DATATYPE	&	Array<DATATYPE>	::	Get(unsigned int	Index)
	{
        assert(Index<ArraySize);
        return *(first_dyn[Index]);
	}
#if 0
template	<class	DATATYPE>
DATATYPE	&	Array<DATATYPE>	::	ReplaceAt(DATATYPE	&Value, unsigned int	Index)
	{
	DataLink<DATATYPE>	*dl;
	unsigned	int		rIndex = Index;
	
	if ( Index >= ArraySize )
		{
		//fprintf(stderr, "Returning NULL Data\n");
		//return ( NullData );
		dl = NULL;
		return ( (DATATYPE &) *dl );	// Invoke a seg fault
		}

	/*if ( LastAccess )
		{
		if ((LastAccessNumber + 1) == Index )
			{
			LastAccess = LastAccess->next;
			++LastAccessNumber;
			LastAccess->Data = Value;
			return ( LastAccess->Data );
			}
		}
	*/

	// locate requested element by following pointer chain
	// decide which is faster -- scan from head or scan from tail

	if(Index < ArraySize / 2)
		{

		// requested element closer to head -- scan forward

		dl = first;
		++Index;
		while(--Index > 0)
			{
			dl = dl->next;
			}
		}
		else
		{

		// requested element closer to tail -- scan backwards

		dl = last;
		Index = (ArraySize - Index);
		while(--Index > 0)
			{
			dl = dl->prev;
			}
		}

	//LastAccess = dl;
	//LastAccessNumber = rIndex;
	dl->Data = Value;
	return ( dl->Data );
	}
#endif

template	<class	DATATYPE>
BOOL	Array<DATATYPE>	::	RemoveAt(unsigned int Index)
	{
        delete first_dyn[Index]; // call dtor (otherwise leaks DATATYPE)
        if (Index==0)
        { first_dyn++;
          --ArraySizeAllocated;
        }
        else if (ArraySize-Index>1)
        { memmove(first_dyn+Index, first_dyn+(Index+1), (ArraySize-Index-1)*sizeof(DATATYPE**));
        }

	--ArraySize;
        if (ArraySize<=0)
        { free(first);
          first = NULL;
          first_dyn = NULL;
          ArraySizeAllocated = 0;
        }        
	return ( TRUE );
	}

/****************************************************************
 * Test Functions
 *
 *
 ****************************************************************/

#	ifdef	BUILD_TEST_UNITS

int
testfunc()
	{
	unsigned	int	Index;
	Array<int>	AInt;

	Index = 0;
	while(Index < 10)
		{
		AInt.Add(Index * 2);
		++Index;
		}
	Index = 0;
	while(Index < 10)
		{
		printf("Val: %d : %d\n", Index, AInt.RemoveAt(0));
		++Index;
		}
	return ( 0 );
	}

int
main()
	{
	testfunc();
	fprintf(stderr, "Display Should be like 'Val: x: 2*x' with x[0..9]\n");
	}

#	endif
