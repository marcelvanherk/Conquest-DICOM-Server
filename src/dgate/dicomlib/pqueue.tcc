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
/*
  20100309    bcb commented out unused variables (gcc4.2 Warnings)
  20100707    mvh Merged
*/

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
 *
 * Template P-Queue Class
 *
 *
 *
 ***********************************************************************/

template	<class	DATATYPE>
DATATYPE	&	PQueue<DATATYPE>	::	Push(DATATYPE	&Value)
	{
	unsigned	int	Index;//, Base;
	DATATYPE	tdt;

	Array<DATATYPE> :: Add(Value);

	Index = Array<DATATYPE>::GetSize();
	--Index;
	while(Index)
		{
		if(Array<DATATYPE>::Get(Index) < Array<DATATYPE>::Get((Index-1)>>1))
			{
			tdt = Array<DATATYPE> :: Get(Index);
			Array<DATATYPE>::Get(Index) = Array<DATATYPE>::Get((Index-1)>>1);
			Array<DATATYPE>::Get((Index-1)>>1) = tdt;
			Index = (Index-1) >> 1;
			}
		else
			break;
		}
	return ( Value );
	}

template	<class	DATATYPE>
DATATYPE	&	PQueue<DATATYPE>	::	Pop()
	{
	DATATYPE	tdt;
//	DATATYPE	tdt2;
//	unsigned	int	Index;
	unsigned	int	sorted;
	unsigned	int	Pick;
	unsigned	int	Child1;
	unsigned	int	Child2;
	unsigned	int	Hole;

	if(!Array<DATATYPE>::GetSize())
		return ( dt );	// error

	dt = Array<DATATYPE> :: Get ( 0 );

	if(Array<DATATYPE> :: GetSize() == 1)
		{
		Array<DATATYPE> :: RemoveAt ( 0 );
		return ( dt ) ;
		}
	tdt = Array<DATATYPE> :: Get ( Array<DATATYPE> :: GetSize() - 1);
	Array<DATATYPE> :: RemoveAt ( Array<DATATYPE> :: GetSize() - 1);
	Hole = 0;
	sorted = 0;
	while(!sorted)
		{
		Child1 = 2 * Hole + 1;
		Child2 = 2 * Hole + 2;
		if(Child2 >= Array<DATATYPE> :: GetSize())
			{
			if ( Child1 >= Array<DATATYPE> :: GetSize())
				{
				Array<DATATYPE> :: Get(Hole) = tdt;
				return ( dt );
				}
			if(Array<DATATYPE> :: Get ( Child1 ) < tdt)
				{
				Array<DATATYPE> :: Get(Hole) = Array<DATATYPE> :: Get(Child1);
				Array<DATATYPE> :: Get(Child1) = tdt;
				return(dt);
				}
			Array<DATATYPE> :: Get(Hole) = tdt;
			return ( dt );
			}
		Pick = Child1;
		if ( Array<DATATYPE> :: Get ( Child1 ) > Array<DATATYPE> :: Get(Child2))
			Pick = Child2;
		if ( Array<DATATYPE> :: Get ( Pick ) < tdt)
			{
			Array<DATATYPE> :: Get ( Hole) = Array<DATATYPE> :: Get( Pick );
			Hole = Pick;
			}
		else
			{
			Array<DATATYPE> :: Get(Hole) = tdt;
			return ( dt );
			}
		}
	return ( dt );
	}
template	<class	DATATYPE>
DATATYPE	&	PQueueOfPtr<DATATYPE>	::	Push(DATATYPE	&Value)
	{
	unsigned	int	Index;//, Base;
	DATATYPE	tdt;

	Array<DATATYPE> :: Add(Value);

	Index = Array<DATATYPE>::GetSize();
	--Index;
	while(Index)
		{
		if((*Array<DATATYPE>::Get(Index)) < (*Array<DATATYPE>::Get((Index-1)>>1)))
			{
			tdt = Array<DATATYPE> :: Get(Index);
			Array<DATATYPE>::Get(Index) = Array<DATATYPE>::Get((Index-1)>>1);
			Array<DATATYPE>::Get((Index-1)>>1) = tdt;
			Index = (Index-1) >> 1;
			}
		else
			break;
		}
	return ( Value );
	}

template	<class	DATATYPE>
DATATYPE	&	PQueueOfPtr<DATATYPE>	::	Pop()
	{
	DATATYPE	tdt;
//	DATATYPE	tdt2;
//	unsigned	int	Index;
	unsigned	int	sorted;
	unsigned	int	Pick;
	unsigned	int	Child1;
	unsigned	int	Child2;
	unsigned	int	Hole;

	if(!Array<DATATYPE>::GetSize())
		return ( dt );	// error

	dt = Array<DATATYPE> :: Get ( 0 );

	if(Array<DATATYPE> :: GetSize() == 1)
		{
		Array<DATATYPE> :: RemoveAt ( 0 );
		return ( dt ) ;
		}
	tdt = Array<DATATYPE> :: Get ( Array<DATATYPE> :: GetSize() - 1);
	Array<DATATYPE> :: RemoveAt ( Array<DATATYPE> :: GetSize() - 1);
	Hole = 0;
	sorted = 0;
	while(!sorted)
		{
		Child1 = 2 * Hole + 1;
		Child2 = 2 * Hole + 2;
		if(Child2 >= Array<DATATYPE> :: GetSize())
			{
			if ( Child1 >= Array<DATATYPE> :: GetSize())
				{
				Array<DATATYPE> :: Get(Hole) = tdt;
				return ( dt );
				}
			if((*Array<DATATYPE> :: Get ( Child1 )) < (*tdt))
				{
				Array<DATATYPE> :: Get(Hole) = Array<DATATYPE> :: Get(Child1);
				Array<DATATYPE> :: Get(Child1) = tdt;
				return(dt);
				}
			Array<DATATYPE> :: Get(Hole) = tdt;
			return ( dt );
			}
		Pick = Child1;
		if ( (*Array<DATATYPE> :: Get ( Child1 )) > (*Array<DATATYPE> :: Get(Child2)))
			Pick = Child2;
		if ( (*Array<DATATYPE> :: Get ( Pick )) < (*tdt))
			{
			Array<DATATYPE> :: Get ( Hole) = Array<DATATYPE> :: Get( Pick );
			Hole = Pick;
			}
		else
			{
			Array<DATATYPE> :: Get(Hole) = tdt;
			return ( dt );
			}
		}
	return ( dt );
	}

#ifdef	BUILD_TEST_UNITS

int
testfunc()
	{
	int		tempint, tempint2, index;
	PQueue<int>	Pq;

	fprintf(stderr, "[adding randum numbers]\n");fflush(stderr);
	index = 0;
	while(index < 1000)
		{
		tempint = rand();
		//fprintf(stderr, "Adding: %d, ", index);fflush(stderr);
		Pq.Push(tempint);
		++index;
		}
	fprintf(stderr, "[testing Queue Size: %d]\n", Pq.GetSize());fflush(stderr);
	index = 0;
	tempint2 = 0;
	while(Pq.GetSize())
		{
		tempint = Pq.Pop();
		//fprintf(stderr, "%d : %d Pop : %d\n", index+1, Pq.GetSize(), tempint);
		if(tempint < tempint2)
			{
			fprintf(stderr, "[---failed---]\n");fflush(stderr);return(0);
			}
		tempint2 = tempint;
		++index;
		}
	fprintf(stderr, "[success]\n");
	return ( 1 );	
	}

main()
	{
	testfunc();
	return(0);
	}

#endif
