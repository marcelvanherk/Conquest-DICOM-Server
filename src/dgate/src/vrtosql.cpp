/*
20000629	ljz	Logging of trouble now starts with '***'
20010330	ljz	Added a few debug printf's
20010420	ljz	Fixed memory leaks in all query levels
20020519        mvh     Clear BindField result strings before reading (NULL does not read)
20021028	mvh	Restructured queries to give lowest level of de-normalized databases
                        Fixed sorting on PatientName in denormalized study query
20021030	mvh	Reversed this again apart from study level because SQL server becomes very slow
20030113        mvh     Added PatientQuerySortOrder etc overrides for sorting
20030114        mvh     Added in querycolumns (even if akreday present); 
                        tested for SQL server, requires testing for other datasources
20030122      ljz+mvh   Remove unused entries from Tables string in queries
20040930        mvh     Started adapt such that query strings etc and not limited in length
			For now: query string max 8192 (here and in odbci.cpp); sql statement max 16384 (in odbci.hpp)
			Enough for query with one multiple values UID of about 100 images (list truncated if too long)
			Added safestrcat protection of SearchString
20041003        mvh     Truncated debug prints, malloc strings at image level
                        Analysed string lenghts; limited 'records =' debug log
20041013	mvh	Used MAXQUERYLENGTH
20050107	mvh	Removed UNIX flags: solve difference in database interface
20050206	mvh	Image query can send filename and device in 0x9999,0x800/0x0801
20050401	mvh	Added QueryOnModalityWorkList; - todo - put selected items into sequence 0040,0100
20050404	mvh	Take query for Scheduled Procedure Step from sequence, put results into sequence
20050414	mvh	Made sequence unfolding for worklist more generic - accepts any number of and nested N=1 sequences
			This is correct behavior for query input, but limited for query results
20050417	mvh	Removed unused variable
20050831	mvh	Fixes in worklist code: is ok for all OFFIS tests except 1 (empty seq not returned) and 
			10 (undef non-empty seq takes one entry from higher level) - good enough for initial release?
20050901	mvh	Fix for test 10: allow undefined sequence in query
20050907	mvh	Merged change by Hans-Peter Hellemann: fix missing max() macro
20051229	mvh	Debug log show records of all queries. DumpVR only left in worklistquery and shows sequence layout
20051230	mvh	Removed 2 forgotten dumpvr's
20060103	mvh	Added debug info from testing phase inside nested sequences for modality query
20060211	mvh	Added empty required modality sequences 8,1110 and 8,1120 when not there
20060224	mvh	Fixed modality worklist query: empty result sequences were not handled correctly (Thanks Tony Tong)
20060311	mvh	Worklist change was tested OK with Agfa (thanks Frank Grogan), cleaned debug log a bit
20060607	mvh	Fix crash when coding empty sequence as last item in worklist query (thanks Francois Piette)
20061219	mvh	Small fix in layout debug log
20071118   	mvh    	Adapted for 64 bits (use SQLLEN for BindField)
20080817	mvh	Fixed bug found by larc: worklist sql syntax error (fields start with ,) when 1st item is sequence
		mvh	Fixed bug found by Alberto Smulders: sometimes sequence level in response would not come down
			The problem was that a higher levels the reported sequence level was inconsistent with the 1st level
			Changed CodeSequence stop criterium and level coding for deeper levels; now works with varies sequence combination
			Added WorkListReturnsISO_IR_100 flag (default 1)
20080818    	mvh 	DbaseIII check now uses DB flags, not PATHSEP in datasource name
20080901	mvh	Implemented 'Number of Patient Related Studies' (0x0020, 0x1200) etc
20080902	mvh	Fixed that option for VirtualServers; added EnableComputedFields 
20080905	bcb	Added void* cast for deivr change
20081016	mvh	Fixed for WC compile
20081121	mvh	Fixed ISO_IR 100
20090930        mvh     Fixed ImageQuerySortOrder: crashed on C-MOVE, because sortcolumn was inserted before the filename
20091005        mvh     Fixed ComputeField: return VR was used during cleanup
20091231        bcb     Changed char* to const char* for gcc4.2 warnings
20100111	mvh	Merged
20100309	bcb	Added double parentheses (gcc4.2 Warnings)
20100309	bcb	Changed int to unsigned int, commented out unused variables (gcc4.2 Warnings)
20100706	bcb	Init Level
20100717	mvh	Merged
20100822	mvh	Delete 9999,0802 virtualservermask control VR from searches and pass it to ComputeField
20100823	mvh	Fixed compile for ms8amd64
20100901	mvh	Rephrased "Failed on VR Search...."
20101003	mvh	Delete 9999,0900 script control from queries
20101120	mvh	Delete 0002,0010 transfer syntax from queries
20101127	mvh	Added CountOnly mode to accelerate e.g., NumberOfStudyRelatedInstances queries
20110105	mvh	Pass database to MakeSafeString to allow db_type dependent processing; use LIKE only when needed
20110105	mvh	Moved local routines here: MakeSafeString, DICOM2SQLQuery, BuildSearchString, BuildColumnString
20110603	mvh	Fully init vr used for MakeSafeString
20110605	mvh	Allow ' *' for query
20120214	mvh	Allow WorkListReturnsISO_IR_100 to be any value (e.g., 192)
20120422	mvh	Fix search in DT_MSTR with embedded _ to use  =, was a LIKE that failed for MySQL
20120624	mvh	Fix DT_MSTR for DoubleBackSlashToDB (mysql and pgsql): requires 4 backslashses (!)
20120701	mvh	Fix in BuildSearchString for UseEscapeStringConstants for DT_MSTR and DT_DATE
20131013	mvh	DT_MSTR warning for DBASEIII reports now to console
20131017        ea      Put the changes of 20120701 back because dgate fails for PatientID with '_'
20171122	mvh	Added DT_ISTR (case insensitive query string)
20190321	mvh	Added optional conversion from ISO_IR 100 to UTF8 in MakeSafeString
20190322	mvh	Fixed that
20191019        mvh     Removed sorting for CountOnly query (SQL fails on postgres)
20200125	mvh	Accept 9999,0c000 ConquestImageQueryOrder (use field name, is truncated to 10)
20200314	mvh	Implemented QueriesReturnISO_IR flag
20220815        mvh     Implemented sorting for all queries except Modality, properly sort on Number fields
20220817        mvh     Private tags for query limit (9999,0c01) and offset (9999,0c02), report sorting
20220820        mvh     Overhaul of sorting code
20230620        mvh     Fix sorting code for PostGres; use CAST and pass expression to columnstring through OrderExp
*/

#ifndef	WHEDGE
#	include	"dgate.hpp"
#else
#	include	"master.h"
#endif

extern char	PatientQuerySortOrder[];
extern char	StudyQuerySortOrder[];
extern char	SeriesQuerySortOrder[];
extern char	ImageQuerySortOrder[];
extern int 	WorkListReturnsISO_IR_100;
extern int	QueriesReturnISO_IR;
extern int 	EnableComputedFields;
extern int	DoubleBackSlashToDB;
extern int	UseEscapeStringConstants;

void safestrcat(char *result, const char *tocat, int maxlen)
{ int len = strlen(result), len2 = strlen(tocat);
  if (len+len2 < maxlen)
    strcpy(result+len, tocat);
}

// from dgate.cpp
#ifndef VirtualQuery
int VirtualQuery(DICOMDataObject *DDO, const char *Level, int N, Array < DICOMDataObject  *> *pADDO, char *ae);
#endif

void RemoveQueryDuplicates(const char *Level, Array < DICOMDataObject * > *ADDO);

extern int	UTF8ToDB;

BOOL
MakeSafeString (
	VR	*vr,
	char	*string,
	Database *db )
	{
	unsigned int Length;
	unsigned char	*sout;
	unsigned char	*sin;
	char	*s;
	UINT	Index;
	BOOL	AddEscape = FALSE;
	BOOL	UseLike = FALSE;

	s = SetString(vr, NULL, 0);
	Length = strlen(s);
	sin = (unsigned char*)s;
	sout = (unsigned char *)string;

	// convert ** query to * query (efilm problem)
	if (Length==2 && s[0]=='*' && s[1]=='*') Length--;
	
	// convert ' *' query to '*' query (some other pacs problem)
	if (Length==2 && s[0]==' ' && s[1]=='*') { Length--; s[0]='*'; }

	if (strchr(s, '*') || strchr(s, '?')) 	// 20110105: force use of 'LIKE' (pattern matching), else use exact matching
		{
		(*sout++) = '?';		// is processed by BuildSearchString defined and only used below
		UseLike = TRUE;
		}

	if (UseEscapeStringConstants)		// typically for postgres
		(*sout++) = 'E';

	(*sout++) = '\'';

	if (vr)
		if(vr->Data)
		{
		Index = 0;
		while(Index < Length)
			{
		  	if (UTF8ToDB && (*sin&128)) 
				{ 
				*sout++=0xc2+(*sin>0xbf);
				*sout++=(*sin&0x3f)+0x80;
				}

			else switch (*sin)
				{
				case	'*':
					(*sout) = '%';++sout;
					break;

				case	'?':
					(*sout) = '_';++sout;
					break;

				// original code
//				case	'%':
//					(*sout) = '\\';++sout;
//					(*sout) = '%';++sout;
//					break;
//				case	'[':
//					(*sout) = '\\';++sout;
//					(*sout) = '[';++sout;
//					break;
//				case	'_':
//					(*sout) = '_';++sout;
//					break;
//				case	'\\':			// not OK: mysql, sqlite, sqlserver
//					if (DoubleBackSlashToDB)// OK: dbase (set for mysql and postgres)`
//						{
//						if ((Index > 0) && (sin[-1] != '\\'))
//							{
//							(*sout) = '\\';++sout;
//							(*sout) = '\\';++sout;
//							}
//						}
//					else
//						{
//						(*sout) = (*sin);
//						++sout;
//						}
//					break;
				// end original code

				// redone these special characters: mvh 20110105
				case	'%':
					if (db->db_type==DT_ODBC && UseLike)	// sql server
						{
						(*sout) = '[';++sout;
						(*sout) = '%';++sout;
						(*sout) = ']';++sout;
						}
					else if (db->db_type==DT_SQLITE && UseLike)
						{
						(*sout) = '\\';++sout;
						(*sout) = '%';++sout;
						AddEscape = TRUE;
						}
					else if (db->db_type==DT_POSTGRES && UseLike)
						{
						(*sout) = '\\';++sout;
						(*sout) = '\\';++sout;
						(*sout) = '%';++sout;
						AddEscape = TRUE;
						}
					else if (UseLike)
						{
						(*sout) = '\\';++sout;	// ok: mysql, dbase
						(*sout) = '%';++sout;
						}
					else
						{
						(*sout) = '%';++sout;
						}
					break;

				case	'[':
					if (db->db_type==DT_ODBC && UseLike)	// sql server
						{
						(*sout) = '[';++sout;
						(*sout) = '[';++sout;
						(*sout) = ']';++sout;
						}
					else if (db->db_type==DT_SQLITE && UseLike)
						{
						(*sout) = '\\';++sout;
						(*sout) = '[';++sout;
						AddEscape = TRUE;
						}
					else if (db->db_type==DT_POSTGRES && UseLike)
						{
						(*sout) = '\\';++sout;
						(*sout) = '\\';++sout;
						(*sout) = '[';++sout;
						AddEscape = TRUE;
						}
					else if (UseLike)
						{
						(*sout) = '\\';++sout;	// ok: mysql, dbase
						(*sout) = '[';++sout;
						}
					else
						{
						(*sout) = '[';++sout;
						}
					break;

				case	'_':
					if (db->db_type==DT_ODBC && UseLike)	// sql server
						{
						(*sout) = '[';++sout;
						(*sout) = '_';++sout;
						(*sout) = ']';++sout;
						}
					else if (db->db_type==DT_SQLITE && UseLike)
						{
						(*sout) = '\\';++sout;
						(*sout) = '_';++sout;
						AddEscape = TRUE;
						}
					else if (db->db_type==DT_POSTGRES && UseLike)
						{
						(*sout) = '\\';++sout;
						(*sout) = '\\';++sout;
						(*sout) = '_';++sout;
						AddEscape = TRUE;
						}
					else if (UseLike)
						{
						(*sout) = '\\';++sout; // ok: mysql, dbase
						(*sout) = '_';++sout;
						}
					else
						{
						(*sout) = '_';++sout;
						}
					break;

				case	'\\':
					if (db->db_type==DT_ODBC && UseLike)	// sql server
						{
						(*sout) = '[';++sout;
						(*sout) = '\\';++sout;
						(*sout) = ']';++sout;
						}
					else if (db->db_type==DT_SQLITE && UseLike)
						{
						(*sout) = '\\';++sout;
						(*sout) = '\\';++sout;
						AddEscape = TRUE;
						}
					else if (db->db_type==DT_POSTGRES && UseLike)
						{
						(*sout) = '\\';++sout;
						(*sout) = '\\';++sout;
						(*sout) = '\\';++sout;
						(*sout) = '\\';++sout;
						AddEscape = TRUE;
						}
					else if (db->db_type==DT_POSTGRES && !UseLike)
						{
						(*sout) = '\\';++sout;
						(*sout) = '\\';++sout;
						}
					else if (UseLike)
						{
						(*sout) = '\\';++sout;	// ok: 
						(*sout) = '\\';++sout;	// not ok: mysql
						}
					else if (DoubleBackSlashToDB)
						{
							if ((Index > 0) && (sin[-1] != '\\'))
							{
							(*sout) = '\\';++sout;	// ok: 
							(*sout) = '\\';++sout;	// not ok: 
							}
						}
					else
						{
						(*sout) = '\\';++sout;
						}
					break;
				// end redone these special characters: mvh 20110105

				case	'\'':
					(*sout) = '\'';++sout;
					(*sout) = '\'';++sout;
					break;

				case	'\"':
					(*sout) = '\"';++sout;
					break;

				case	0:	break;

				default:
					(*sout) = (*sin);
					++sout;
				}
			++sin;
			++Index;
			}
		}

        /* new code removes all trailing spaces (no check on begin: sout always start with ') */
        sout--;
        while (*sout == ' ') sout--;
        sout++;

	(*sout) = '\'';++sout;
	(*sout) = '\0';++sout;
	
	// redone these special characters: mvh 20110105
	//20120701: seems this is not needed and highly complicates further processing
	if (AddEscape && db->db_type==DT_POSTGRES) strcpy((char *)sout-1, " ESCAPE E'\\\\'");
	else if (AddEscape) strcpy((char *)sout-1, " ESCAPE '\\'");
	// end redone these special characters: mvh 20110105

	delete s;
	return ( TRUE );
	}

BOOL	DICOM2SQLQuery (
	char	*s,
	Database *db )
	{
	VR		vr;
	char	*s1;

	if(*s)
		{
		vr.Data = (void*)s;
		vr.Length = strlen(s);
		vr.Group = 0;
		vr.Element = 0;
		s1 = (char *)malloc(vr.Length*3 + 20);	// must allow MakeSafeString in-place (!) ?E'[_]' ESCAPE '\'
		MakeSafeString(&vr, s1, db);
		strcpy(s, s1);
		free(s1);
		vr.Data = NULL;
		vr.Length = 0;
		}

	return ( TRUE );
	}

BOOL
BuildSearchString(Database *DB, DBENTRY	*DBE, char	*TableName, VR	*vr, char	*Search,
	char	*TempString, 	int maxlen)
	{
	char	*s, *t, *search;
	char	ch;
	char	TempString1[64];
	char	TempString2[64];
	UINT	Index = DBEIndex(DBE, vr);
	int	len;
	char	escape[16];
	
	if (UseEscapeStringConstants) 
		{
		escape[0]='E';
		escape[1]=0;
		}
	else 
		escape[0]=0;
		
	search = Search;
	if (*search=='?') search++;
	if (*search=='E') search++;

	if(vr->Length)
		{
		if(DBE[Index].DICOMType==DT_DATE)
			{
			if((s=strchr(search, '-')))
				{
				// Date Range
				if((*(s+1))=='\'')
					{
					(*(s+1)) = '\0';
					(*s) = '\'';

					sprintf(TempString, "%s.%s >= %s%s", TableName, DBE[Index].SQLColumn, 
					escape, search);
					}
				else if((*(s-1))=='\'')
					{
					(*s) = '\'';
					sprintf(TempString, "%s.%s <= %s%s", TableName, DBE[Index].SQLColumn, 
					escape, s);
					}
				else
					{
					// Bummer format is 'date-date'
					ch = (*(s+1));
					(*(s+1)) = '\0';
					(*s) = '\'';

					sprintf(TempString1, "%s.%s >= %s%s", TableName, DBE[Index].SQLColumn, 
					escape, search);

					(*(s+1)) = ch;
					sprintf(TempString2, "%s.%s <= %s%s", TableName, DBE[Index].SQLColumn, 
					escape, s);

					sprintf(TempString, "%s and %s", TempString1, TempString2);
					}
				}
			else
				{
				if (Search[0]=='?')
					sprintf(TempString, "%s.%s LIKE %s%s", TableName, DBE[Index].SQLColumn, 
					escape, search);
				else
					sprintf(TempString, "%s.%s = %s%s", TableName, DBE[Index].SQLColumn, 
					escape, search);
				}
			}

		// Multiple UID matching
		else if(DBE[Index].DICOMType==DT_UI)
			{
			BOOL dbiii = (DB->db_type == DT_DBASEIII);

			s = strchr(search, '\\');

			if (s && !dbiii)
				{
				// Multiple UID matching sql server style with syntax: field in ('a','b','c')
				// requires 3 characters per UID, e.g., 8192-512 fits about 110 UIDs
				sprintf(TempString, "%s.%s in (", TableName, DBE[Index].SQLColumn);

				t = search + 1;		// skip '

				while (s)		// before each \ is an UID
					{
					*s = 0;
					if (s[1] == '\\')	// also process double backslash correctly
						{
						s++;
						*s = 0;
						}

					len = strlen(TempString);
					if (len<maxlen)
  					  sprintf(TempString + len, "'%s',", t);

					t = s + 1;
					s = strchr(t, '\\');
					}

				// the last UID ends with a '
				sprintf(TempString + strlen(TempString), "'%s)", t);
				}

			else if (s && dbiii)
				{
				// Multiple UID matching built-in dbaseIII style with | syntax: field in 'a|b|c'
				// requires 1 character per UID, e.g., 8192-512 fit max about 115 UIDs
				sprintf(TempString, "%s.%s in '", TableName, DBE[Index].SQLColumn);

				t = search + 1;		// skip '

				while (s)		// before each \ is an UID
					{
					*s = 0;
					if (s[1] == '\\')	// also process double backslash correctly
						{
						s++;
						*s = 0;
						}

					len = strlen(TempString);
					if (len<maxlen)
					  sprintf(TempString + len, "%s|", t);

					t = s + 1;
					s = strchr(t, '\\');
					}

				// the last UID ends with a '
				sprintf(TempString + strlen(TempString), "%s", t);
				}

			else
				{
				if (Search[0]=='?')
					sprintf(TempString, "%s.%s LIKE %s%s", TableName, DBE[Index].SQLColumn,
						escape, search);
				else
					sprintf(TempString, "%s.%s = %s%s", TableName, DBE[Index].SQLColumn,
						escape, search);
				}
			}
		// DT_MSTR: PET matches 'PET', 'PET\CT', 'MR\PET\CT', and 'CT\PET'
		// not implemented yet for DBASEIII, also wildcards give original query
		else if((DBE[Index].DICOMType==DT_MSTR) && (DB->db_type != DT_DBASEIII))
			{
			if (Search[0]=='?')
				sprintf(TempString, "%s.%s LIKE %s%s", TableName, DBE[Index].SQLColumn,
					escape, search);
			else
				{
				char *p = search + strlen(search) - 1;
				const char *d;

				// mysql and postgres require  four backslashes
				if (DoubleBackSlashToDB) d = "\\\\\\\\"; else d = "\\";

				sprintf(TempString, "(%s.%s = %s%s or ", TableName, DBE[Index].SQLColumn, 
				escape, search);

				*p = 0;
				sprintf(TempString + strlen(TempString), "%s.%s LIKE %s'%s%s%%' or ", TableName, DBE[Index].SQLColumn, 
				escape, search+1, d);
				sprintf(TempString + strlen(TempString), "%s.%s LIKE %s'%%%s%s%s%%' or ", TableName, DBE[Index].SQLColumn, 
				escape, d, search+1, d);
				sprintf(TempString + strlen(TempString), "%s.%s LIKE %s'%%%s%s')", TableName, DBE[Index].SQLColumn,
				escape, d, search+1);
				*p = '\'';
				}
			}
		else
			{
			if(DBE[Index].DICOMType==DT_ISTR)
				{
				strupr(search);
				if (Search[0]=='?')
					sprintf(TempString, "UPPER(%s.%s) LIKE %s%s", TableName, DBE[Index].SQLColumn,
						escape, search);
				else
					{
					sprintf(TempString, "UPPER(%s.%s) = %s%s", TableName, DBE[Index].SQLColumn,
						escape, search);

					if (DBE[Index].DICOMType==DT_MSTR)
						OperatorConsole.printf("warning: query for this multi-valued item (%s) will not test individual values\n", DBE[Index].SQLColumn);
					}
				}
				else
				{
				if (Search[0]=='?')
					sprintf(TempString, "%s.%s LIKE %s%s", TableName, DBE[Index].SQLColumn,
						escape, search);
				else
					{
					sprintf(TempString, "%s.%s = %s%s", TableName, DBE[Index].SQLColumn,
						escape, search);

					if (DBE[Index].DICOMType==DT_MSTR)
						OperatorConsole.printf("warning: query for this multi-valued item (%s) will not test individual values\n", DBE[Index].SQLColumn);
					}
				}

			}

		return (TRUE);
		}
	return ( FALSE );
	}

BOOL
BuildColumnString(DBENTRY	*DBE, char	*TableName, VR	*vr,	char	*TempString)
	{
	UINT	Index = DBEIndex(DBE, vr);

	strcpy(TempString, TableName);
	strcat(TempString, ".");
	strcat(TempString, DBE[Index].SQLColumn);
	return ( TRUE );
	}

UINT32
SQLRealSize(char	*Str, SDWORD	Max)
	{
	UNUSED_ARGUMENT(Str);
#if 0
	UINT32	Index = 0;
	UINT32	RSize = 0;
#endif

	if(Max<0) return 0;
	return Max;
#if 0
	while (Index < Max)				// mvh 20051123
		{
		if(Str[Index]== 0 ) break;
		if(Str[Index]!=' ') RSize = Index+1;
		++Index;
		}
	return ( RSize );
#endif
	}


// compute items like 'Number of Patient Related Studies' and generate VR with the result
// allowcount should only be set TRUE for databases that allows SELECT COUNT(1) statements
static VR *ComputeField(DICOMDataObject *DDO, int group, int element, UINT16 mask=0xffff, BOOL allowcount=FALSE)
	{
	Array < DICOMDataObject * > ADDO;
	char count[16];
	const char *Level;
	VR *vr, *vr2;
	int save = DebugLevel, count1, count2, sources=0;
	unsigned int i;
	BOOL CountOnly = FALSE;
	
	// level for VirtualQueries to other servers
	Level = NULL;
	if (element==0x1200) Level = "STUDY";
	if (element==0x1202) Level = "SERIES";
	if (element==0x1206) Level = "SERIES";
	if (element==0x1204) Level = "IMAGE";
	if (element==0x1208) Level = "IMAGE";
	if (element==0x1209) Level = "IMAGE";
        
	// make that a UNIQUE query will result the correct count
	if (element==0x1200 && DDO->GetVR(0x0020, 0x000d)==NULL) DDO->Push(new VR(0x0020, 0x000d, 0, FALSE));
	if (element==0x1202 && DDO->GetVR(0x0020, 0x000e)==NULL) DDO->Push(new VR(0x0020, 0x000e, 0, FALSE));
	if (element==0x1206 && DDO->GetVR(0x0020, 0x000e)==NULL) DDO->Push(new VR(0x0020, 0x000e, 0, FALSE));
	if (element==0x1204 && DDO->GetVR(0x0008, 0x0018)==NULL) DDO->Push(new VR(0x0008, 0x0018, 0, FALSE));
	if (element==0x1208 && DDO->GetVR(0x0008, 0x0018)==NULL) DDO->Push(new VR(0x0008, 0x0018, 0, FALSE));
	if (element==0x1209 && DDO->GetVR(0x0008, 0x0018)==NULL) DDO->Push(new VR(0x0008, 0x0018, 0, FALSE));

	// query other virtualfor servers
	for (i=0; i<10; i++)
		if (mask & (1<<i))
			sources += VirtualQuery(DDO, Level, i, &ADDO, NULL)!=0;
	count1 = ADDO.GetSize();
	
	if (allowcount && count1==0)
		{
		CountOnly = TRUE;	
		DDO->Push(new VR(0x9999, 0x9999, 0, FALSE)); // no need to locate duplicates: force CountOnly
		sources = 0;
		}

	// query this server
	DebugLevel=0;
	if (element==0x1200) QueryOnStudy  (DDO, &ADDO); // Number of Patient Related Studies
	if (element==0x1202) QueryOnSeries (DDO, &ADDO); // Number of Patient Related Series
	if (element==0x1206) QueryOnSeries (DDO, &ADDO); // Number of Study Related Series
	if (element==0x1204) QueryOnImage  (DDO, &ADDO); // Number of Patient Related Instances
	if (element==0x1208) QueryOnImage  (DDO, &ADDO); // Number of Study Related Instances
	if (element==0x1209) QueryOnImage  (DDO, &ADDO); // Number of Series Related Instances
	DebugLevel=save;

	// count number of servers accessed
	count2 = ADDO.GetSize()-count1;
	sources += count2!=0;

	if (sources>1)	// more than one source; data can be duplicated
		RemoveQueryDuplicates(Level, &ADDO);

	// create result VR
	sprintf(count, "%d", ADDO.GetSize());
	if (strlen(count)&1) strcat(count, " ");
	vr = new VR( group, element, strlen(count), TRUE);
	memcpy(vr->Data, count, strlen(count));

	if (!CountOnly)
        	{
        	// free arrays
		for (i=0; i<ADDO.GetSize(); i++)
			while((vr2=ADDO.Get(i)->Pop())) delete vr2;

		while (ADDO.GetSize())
			{
			delete ADDO.Get(0);
			ADDO.RemoveAt(0);
			}
		}
	else
		while (ADDO.GetSize())
			ADDO.RemoveAt(0);
	
	return vr;
	}

BOOL	QueryOnPatient (
	DICOMDataObject *DDO,
	Array < DICOMDataObject *> *ADDO)
	{
	UINT				Index, CIndex, CCIndex;
	UINT32				VRLength;
	DICOMDataObject			*RDDO;
	Database			DB;
	char				*SQLResultString;
	Array < char * >		SQLResult, SQLResultPatient;
	Array < VR * >			EMaskPatient, EMask;
	Array < SQLLEN *>		SQLResultLength;
	Array < DBENTRY * >		DBQPatient;
	Array < DBENTRY * >		DBQMaster;
	DBENTRY				*TempDBEPtr;
	char				*DBR;
	SQLLEN				*DBL;
	VR				*vr;
	VR				VRPatientName;
	DBENTRY				*DBEntryPatientName;
	char				SortOrder[128];
	char				Order[128];
	char				OrderCalc[128];
	char				OrderExp[128];
	char				*Sorting;
	BOOL				DoSort;
	BOOL				SendAE = FALSE;
	char				TempString [ 8192 ];
	char				SearchString [ 8192 ];
	char				ColumnString [ 4096 ];
	char				Tables [ 256 ];
        char				*Sort=NULL;
      	DICOMDataObject 		*qStudies = NULL;
      	DICOMDataObject 		*qSeries = NULL;
      	DICOMDataObject 		*qSops = NULL;
	UINT16				mask = 0xffff;
	BOOL				CountOnly = FALSE;

	SystemDebug.printf("Query On Patient\n");

	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		{
		DB.PrintLastError();
		return ( FALSE );	// failed open
		}

	// First.  Check that all the asked-for elements are actually in
	// the Patient/Study database.  If they are not, well, then we
	// return FALSE.
	DoSort = FALSE;
	Order[0] = OrderCalc[0] = OrderExp[0] = 0;
	while ((vr = DDO->Pop()))
		{
		if(vr->Element == 0x0000)
			{
			delete	vr;
			continue;	// discard length codes
			}
		if(vr->Group == 0x0002)
			if(vr->Element == 0x0010)
				{
				delete	vr;
				continue;	// discard transfer syntax
				}
		if(vr->Group == 0x0008)
			if(vr->Element == 0x0052)
				{
				delete	vr;
				continue;	// discard model level
				}

		if(vr->Group == 0x0008)
			if(vr->Element == 0x0054)
				{
				SendAE = TRUE;
				delete vr;
				continue;	// discard it (but send it)
				}

		if(vr->Group == 0x0020 && EnableComputedFields)
			if(vr->Element == 0x1200)
				{
				qStudies = new DICOMDataObject;
				delete	vr;
				continue;	// discard 'Number of' items (but send them)
				}

		if(vr->Group == 0x0020 && EnableComputedFields)
			if(vr->Element == 0x1202)
				{
				qSeries = new DICOMDataObject;
				delete	vr;
				continue;	// discard 'Number of' items (but send them)
				}

		if(vr->Group == 0x0020 && EnableComputedFields)
			if(vr->Element == 0x1204)
				{
				qSops = new DICOMDataObject;
				delete	vr;
				continue;	// discard 'Number of' items (but send them)
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0802)
				{
				mask = vr->GetUINT16();
				delete vr;
				continue;	// discard it
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0900)
				{
				mask = vr->GetUINT16();
				delete vr;
				continue;	// discard it
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0c00)
				{
				memcpy(Order, vr->Data, vr->Length);
				Order[vr->Length]=0;
				if (strstr(Order, "Number")) 
                                { Order[10]=0;
                                  if (DB.db_type!=DT_POSTGRES) sprintf(OrderCalc, "(%s+0)", Order);
				  else sprintf(OrderCalc, "CAST(%s AS integer)", Order);
                                }
                                else
				{ Order[10]=0;
			          strcpy(OrderCalc, Order);
				}
			        strcpy(OrderExp, OrderCalc);
				delete vr;
				continue;	// discard it
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0c01)
				{
                                int len = strlen(OrderCalc);
                                strcat(OrderCalc, " LIMIT ");
				memcpy(OrderCalc+len+7, vr->Data, vr->Length);
				OrderCalc[vr->Length+len+7]=0;
                                }
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0c02)
				{
                                int len = strlen(OrderCalc);
                                strcat(OrderCalc, " OFFSET ");
				memcpy(OrderCalc+len+8, vr->Data, vr->Length);
				OrderCalc[vr->Length+len+8]=0;
                                }
		if(vr->Group == 0x9999)
			if(vr->Element == 0x9999)
				{
				CountOnly = TRUE;
				delete vr;
				continue;	// discard it
				}

		if(!VerifyIsInDBE(vr, PatientDB, TempDBEPtr))
			{
			SystemDebug.printf("Queried item %4.4x %4.4x is not in the database\n",
				vr->Group, vr->Element);
			delete	vr;
			continue;

			while(EMaskPatient.GetSize())
				{
				delete EMaskPatient.Get(0);
				EMaskPatient.RemoveAt(0);
				}
			while(SQLResultPatient.GetSize())
				{
				delete SQLResultPatient.Get(0);
				SQLResultPatient.RemoveAt(0);
				}

			return ( FALSE );
			}
		else
			{
			if(vr->Group == 0x0010)
				if(vr->Element == 0x0010)
					DoSort = TRUE;
			SQLResultString = SetString(vr, NULL, 0);
			DICOM2SQLQuery(SQLResultString, &DB);
			SQLResultPatient.Add ( SQLResultString );
			EMaskPatient.Add ( vr );
			DBQPatient.Add ( TempDBEPtr );	
			}
		}

	// Prepare the query string.
	// from EMasks, and SQLResults

	SearchString[0] = '\0';
	ColumnString[0] = '\0';

	Index = 0;CIndex = 0;CCIndex = 0;
	while ( Index < SQLResultPatient.GetSize() )
		{
		SQLResultString = SQLResultPatient.Get(Index);
		if(BuildSearchString(&DB, PatientDB, PatientTableName, EMaskPatient.Get(Index),
			SQLResultString, TempString, sizeof(TempString)-512))
			{
			if(CIndex++)
				safestrcat(SearchString, " and ", sizeof(SearchString));
			safestrcat(SearchString, TempString, sizeof(SearchString));
			}
		BuildColumnString(PatientDB, PatientTableName, EMaskPatient.Get(Index), TempString);
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, TempString);
		EMask.Add(EMaskPatient.Get(Index));
		DBQMaster.Add(DBQPatient.Get(Index));
		++Index;++CCIndex;
		}

        sprintf(Tables, "%s",
	  	PatientTableName);
        
	if (CountOnly)
	  sprintf(ColumnString, "COUNT(1)");

	while(SQLResultPatient.GetSize())
		{
		delete SQLResultPatient.Get(0);
		SQLResultPatient.RemoveAt(0);
		}

	SortOrder[0]=0;
	VRPatientName.Group = 0x0010;
	VRPatientName.Element = 0x0010;
	DBEntryPatientName = FindDBE(&VRPatientName);
	if(DBEntryPatientName && DoSort)
		{
		sprintf(SortOrder, "%s.%s",
			PatientTableName,
			DBEntryPatientName->SQLColumn);
		}

	if (Order[0])
		{
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, OrderExp);
		}
	else if (PatientQuerySortOrder[0]) 
		{
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, PatientQuerySortOrder);
		}

	if (Order[0]) Sort = OrderCalc;
        else if (StudyQuerySortOrder[0]) Sort = StudyQuerySortOrder;
        else if (SortOrder[0])           Sort = SortOrder;
	if (CountOnly) Sort = NULL;

        sprintf(Tables, "%s",
	  	PatientTableName);
        
	if (CountOnly)
	  sprintf(ColumnString, "COUNT(1)");

	while(SQLResultPatient.GetSize())
		{
		delete SQLResultPatient.Get(0);
		SQLResultPatient.RemoveAt(0);
		}

/*        if (Order[0])
		{
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, OrderExp);
		}
	else if (PatientQuerySortOrder[0]) 
		{
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, PatientQuerySortOrder);
		}
*/

	SystemDebug.printf("Issue Query on Columns: %s\n", ColumnString);
	SystemDebug.printf("Values: %.1000s\n", SearchString);
	SystemDebug.printf("Tables: %.1000s\n", Tables);
	if (Sort) SystemDebug.printf("Sort: %.1000s\n", Sort);

	if(strlen(SearchString))
		{
		if (!DB.QueryDistinct ( Tables, ColumnString, SearchString, Sort) )
			{
			DB.PrintLastError();
			return ( FALSE );	// failed query
			}
		}
	else
		if (!DB.QueryDistinct ( Tables, ColumnString, NULL, Sort))
			{
			DB.PrintLastError();
			return ( FALSE );	// failed query
			}

	if (!CountOnly)
		{
		Index = 0;
		while ( Index < CCIndex )
			{
			DBR = new char[255];
			DBR[0] = 0;			// in case a field is NULL it does not read
			SQLResult.Add(DBR);
			DBL = new SQLLEN;
			SQLResultLength.Add(DBL);
			if(!DB.BindField (Index+1, SQL_C_CHAR,
				SQLResult.Get(Index), 255,
				SQLResultLength.Get(Index)))
				{
				SystemDebug.printf("Column Number : %d\n", Index+1);
				DB.PrintLastError();
				while(SQLResult.GetSize())
					{
					delete SQLResult.Get(0);
					SQLResult.RemoveAt(0);
					}
				while(SQLResultLength.GetSize())
					{
					delete SQLResultLength.Get(0);
					SQLResultLength.RemoveAt(0);
					}
				return ( FALSE );	// failed to bind column
				}
			++Index;
			}
		while (DB.NextRecord())
			{
			RDDO = new DICOMDataObject;
			Index = 0;
			while ( Index < CCIndex )
				{
				VRLength = SQLRealSize(SQLResult.Get(Index),
							*SQLResultLength.Get(Index));
				vr = ConstructVRFromSQL (
					DBQMaster.Get(Index),
					EMask.Get(Index)->Group,
					EMask.Get(Index)->Element,
					VRLength,
					SQLResult.Get(Index));

				if (qStudies) 
					qStudies->Push(ConstructVRFromSQL (DBQMaster.Get(Index), EMask.Get(Index)->Group, EMask.Get(Index)->Element, VRLength, SQLResult.Get(Index)));
				if (qSeries) 
					qSeries->Push(ConstructVRFromSQL (DBQMaster.Get(Index), EMask.Get(Index)->Group, EMask.Get(Index)->Element, VRLength, SQLResult.Get(Index)));
				if (qSops) 
					qSops->Push(ConstructVRFromSQL (DBQMaster.Get(Index), EMask.Get(Index)->Group, EMask.Get(Index)->Element, VRLength, SQLResult.Get(Index)));

				RDDO->Push(vr);
				++Index;
				}

			if (QueriesReturnISO_IR)
				{
				vr = RDDO->GetVR(0x0008, 0x0005);
				if(!vr)
					{
					VR *vrNew = new VR(0x0008, 0x0005, 10, TRUE);
					sprintf((char*)vrNew->Data, "ISO_IR %d", QueriesReturnISO_IR);
					RDDO->Push(vrNew);
					}
				}

			if(SendAE)
				RDDO->Push(ConstructAE());

			if (qStudies)
				{
				RDDO->Push(ComputeField(qStudies, 0x0020, 0x1200, mask, DB.db_type!=DT_DBASEIII));
				qStudies->Reset();
				}

			if (qSeries)
				{
				RDDO->Push(ComputeField(qSeries, 0x0020, 0x1202, mask, DB.db_type!=DT_DBASEIII));
				qSeries->Reset();
				}

			if (qSops)
				{
				RDDO->Push(ComputeField(qSops, 0x0020, 0x1204, mask, DB.db_type!=DT_DBASEIII));
				qSops->Reset();
				}

			ADDO->Add(RDDO);
			}
		}
	else
		{
		char res[256];
		SQLLEN len;
		DB.BindField (1, SQL_C_CHAR, res, 255, &len);
		DB.NextRecord();
		DICOMDataObject *rd=NULL;
		for (int i=0; i<atoi(res); i++) ADDO->Add(rd);
		}
		
	SystemDebug.printf("Records = %d\n", ADDO->GetSize());

	while(SQLResult.GetSize())
		{
		delete SQLResult.Get(0);
		SQLResult.RemoveAt(0);
		}
	while(SQLResultLength.GetSize())
		{
		delete SQLResultLength.Get(0);
		SQLResultLength.RemoveAt(0);
		}

	/* LJ: Leak!! */
	while(EMaskPatient.GetSize())
		{
		delete EMaskPatient.Get(0);
		EMaskPatient.RemoveAt(0);
		}

	if (qStudies) delete qStudies;
	if (qSeries) delete qSeries;
	if (qSops) delete qSops;

	return ( TRUE );
	}

BOOL	QueryOnStudy (
	DICOMDataObject *DDO,
	Array < DICOMDataObject *> *ADDO)
	{
	UINT				Index, CIndex, CCIndex;
	UINT32				VRLength;
	DICOMDataObject			*RDDO;
	Database			DB;
	char				*SQLResultString;
	Array < char * >		SQLResult, SQLResultStudy, SQLResultPatient;
	Array < DBENTRY * >		DBQPatient, DBQStudy;
	Array < DBENTRY * >		DBQMaster;
	Array < VR * >			EMaskPatient, EMaskStudy, EMask;
	Array < SQLLEN *>		SQLResultLength;
	DBENTRY				*TempDBEPtr;
	char				*DBR;
	SQLLEN				*DBL;
	VR				*vr;
	BOOL				SendAE = FALSE;
	char				TempString [ 8192 ];
	char				SearchString [ 8192 ];
	char				ColumnString [ 4096 ];
	char				Tables [ 256 ];
	VR				VRPatientName;
	DBENTRY				*DBEntryPatientName;
	char				SortOrder[128];
	char				Order[128];
	char				OrderCalc[128];
	char				OrderExp[128];
	char				*Sorting;
	BOOL				DoSort;
        char				*Sort=NULL;
      	DICOMDataObject 		*qSeries = NULL;
      	DICOMDataObject 		*qSops = NULL;
	UINT16				mask = 0xffff;
	BOOL				CountOnly = FALSE;

	SystemDebug.printf("Query On Study\n");

	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		{
		DB.PrintLastError();
		return ( FALSE );	// failed open
		}

	// First.  Check that all the asked-for elements are actually in
	// the Patient/Study database.  If they are not, well, then we
	// return FALSE.

	DoSort = FALSE;
	Order[0] = OrderCalc[0] = OrderExp[0] = 0;
	while ((vr = DDO->Pop()))
		{
		if(vr->Element == 0x0000)
			{
			delete	vr;
			continue;	// discard length codes
			}

		if(vr->Group == 0x0002)
			if(vr->Element == 0x0010)
				{
				delete	vr;
				continue;	// discard transfer syntax
				}
		if(vr->Group == 0x0008)
			if(vr->Element == 0x0052)
				{
				delete	vr;
				continue;	// discard model level
				}

		if(vr->Group == 0x0008)
			if(vr->Element == 0x0054)
				{
				SendAE = TRUE;
				delete vr;
				continue;	// discard it (but send it)
				}

		if(vr->Group == 0x0020 && EnableComputedFields)
			if(vr->Element == 0x1206)
				{
				qSeries = new DICOMDataObject;
				delete	vr;
				continue;	// discard 'Number of' items (but send them)
				}

		if(vr->Group == 0x0020 && EnableComputedFields)
			if(vr->Element == 0x1208)
				{
				qSops = new DICOMDataObject;
				delete	vr;
				continue;	// discard 'Number of' items (but send them)
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0802)
				{
				mask = vr->GetUINT16();
				delete vr;
				continue;	// discard it
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0900)
				{
				mask = vr->GetUINT16();
				delete vr;
				continue;	// discard it
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0c00)
				{
				memcpy(Order, vr->Data, vr->Length);
				Order[vr->Length]=0;
				if (strstr(Order, "Number")) 
                                { Order[10]=0;
                                  if (DB.db_type!=DT_POSTGRES) sprintf(OrderCalc, "(%s+0)", Order);
				  else sprintf(OrderCalc, "CAST(%s AS integer)", Order);
                                }
                                else
				{ Order[10]=0;
			          strcpy(OrderCalc, Order);
				}
			        strcpy(OrderExp, OrderCalc);
				delete vr;
				continue;	// discard it
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0c01)
				{
                                int len = strlen(OrderCalc);
                                strcat(OrderCalc, " LIMIT ");
				memcpy(OrderCalc+len+7, vr->Data, vr->Length);
				OrderCalc[vr->Length+len+7]=0;
                                }
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0c02)
				{
                                int len = strlen(OrderCalc);
                                strcat(OrderCalc, " OFFSET ");
				memcpy(OrderCalc+len+8, vr->Data, vr->Length);
				OrderCalc[vr->Length+len+8]=0;
                                }
		if(vr->Group == 0x9999)
			if(vr->Element == 0x9999)
				{
				CountOnly = TRUE;
				delete vr;
				continue;	// discard it
				}

		if(vr->Group == 0x0010)
			if(vr->Element == 0x0010)
				DoSort = TRUE;

		if(VerifyIsInDBE(vr, StudyDB, TempDBEPtr))
			{
			SQLResultString = SetString(vr, NULL, 0);
			DICOM2SQLQuery(SQLResultString, &DB);
			SQLResultStudy.Add ( SQLResultString );
			EMaskStudy.Add ( vr );
			DBQStudy.Add ( TempDBEPtr );
			}
		else if(VerifyIsInDBE(vr, PatientDB, TempDBEPtr))
			{
			SQLResultString = SetString(vr, NULL, 0);
			DICOM2SQLQuery(SQLResultString, &DB);
			SQLResultPatient.Add ( SQLResultString );
			EMaskPatient.Add ( vr );
			DBQPatient.Add ( TempDBEPtr );
			}
		else
			{
			SystemDebug.printf("Queried item %4.4x %4.4x is not in the database\n",
				vr->Group, vr->Element);
			delete	vr;
			continue;
			}
		}

	// Prepare the query string.
	// from EMasks, and SQLResults

	SearchString[0] = '\0';
	ColumnString[0] = '\0';

	Index = 0;CIndex = 0;CCIndex = 0;
	while ( Index < SQLResultPatient.GetSize() )
		{
		SQLResultString = SQLResultPatient.Get(Index);
		if(BuildSearchString(&DB, PatientDB, PatientTableName, EMaskPatient.Get(Index),
			SQLResultString, TempString, sizeof(TempString)-512))
			{
			if(CIndex++)
				safestrcat(SearchString, " and ", sizeof(SearchString));
			safestrcat(SearchString, TempString, sizeof(SearchString));
			}
		BuildColumnString(PatientDB, PatientTableName, EMaskPatient.Get(Index), TempString);
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, TempString);
		EMask.Add(EMaskPatient.Get(Index));
		DBQMaster.Add(DBQPatient.Get(Index));
		++Index;++CCIndex;
		}
	Index = 0;
	while ( Index < SQLResultStudy.GetSize() )
		{
		SQLResultString = SQLResultStudy.Get(Index);
		if(BuildSearchString(&DB, StudyDB, StudyTableName, EMaskStudy.Get(Index),
			SQLResultString, TempString, sizeof(TempString)-512))
			{
			if(CIndex++)
				safestrcat(SearchString, " and ", sizeof(SearchString));
			safestrcat(SearchString, TempString, sizeof(SearchString));
			}
		BuildColumnString(StudyDB, StudyTableName, EMaskStudy.Get(Index), TempString);
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, TempString);
		EMask.Add(EMaskStudy.Get(Index));
		DBQMaster.Add(DBQStudy.Get(Index));
		++Index;++CCIndex;
		}

	// Join only required when patient items asked (i.e., not for denormalized databases)
	if (SQLResultPatient.GetSize())
		{
		if(CIndex++)
			safestrcat(SearchString, " and ", sizeof(SearchString));
		// Join Study into Patient
		sprintf(TempString, "%s.%s = %s.%s",
			StudyTableName, StudyDB [ LastDBE(StudyDB) ].SQLColumn,
			PatientTableName, StudyDB [ LastDBE(StudyDB) ].SQLColumn);
		safestrcat(SearchString, TempString, sizeof(SearchString));
		}

	VRPatientName.Group = 0x0010;
	VRPatientName.Element = 0x0010;
	DBEntryPatientName = FindDBE(&VRPatientName);
	SortOrder[0]=0;
	if(DBEntryPatientName && DoSort)
		{
		if (DBEIndex(StudyDB, &VRPatientName))
			sprintf(SortOrder, "%s.%s",
				StudyTableName,
				DBEntryPatientName->SQLColumn);
		else
			sprintf(SortOrder, "%s.%s",
				PatientTableName,
				DBEntryPatientName->SQLColumn);
		}

	if (Order[0])
		{
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, OrderExp);
		}
	else if (StudyQuerySortOrder[0]) 
		{
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, StudyQuerySortOrder);
		}

	if (SQLResultPatient.GetSize())
	  sprintf(Tables, "%s, %s",
	  	StudyTableName, PatientTableName);
	else
	  sprintf(Tables, "%s",
	  	StudyTableName);

	if (CountOnly)
	  sprintf(ColumnString, "COUNT(1)");
        
	while(SQLResultStudy.GetSize())
		{
		delete SQLResultStudy.Get(0);
		SQLResultStudy.RemoveAt(0);
		}
	while(SQLResultPatient.GetSize())
		{
		delete SQLResultPatient.Get(0);
		SQLResultPatient.RemoveAt(0);
		}

	// Issue Query
        if (Order[0]) Sort = OrderCalc;
        else if (StudyQuerySortOrder[0]) Sort = StudyQuerySortOrder;
        else if (SortOrder[0])           Sort = SortOrder;
	if (CountOnly) Sort = NULL;

	SystemDebug.printf("Issue Query on Columns: %s\n", ColumnString);
	SystemDebug.printf("Values: %.1000s\n", SearchString);
	SystemDebug.printf("Tables: %.1000s\n", Tables);
	if (Sort) SystemDebug.printf("Sort: %.1000s\n", Sort);

	if(strlen(SearchString))
		{
		if (!DB.QueryDistinct ( Tables, ColumnString, SearchString, Sort) )
			{
			DB.PrintLastError();
			return ( FALSE );	// failed query
			}
		}
	else
		if (!DB.QueryDistinct ( Tables, ColumnString, NULL, Sort))
			{
			DB.PrintLastError();
			return ( FALSE );	// failed query
			}

	if (!CountOnly)
		{
		Index = 0;
		while ( Index < CCIndex )
			{
			DBR = new char[255];
			DBR[0] = 0;			// in case a field is NULL it does not read
			SQLResult.Add(DBR);
			DBL = new SQLLEN;
			SQLResultLength.Add(DBL);
			if(!DB.BindField (Index+1, SQL_C_CHAR,
				SQLResult.Get(Index), 255,
				SQLResultLength.Get(Index)))
				{
				SystemDebug.printf("Column Number : %d\n", Index+1);
				DB.PrintLastError();
				while(SQLResult.GetSize())
					{
					delete SQLResult.Get(0);
					SQLResult.RemoveAt(0);
					}
				while(SQLResultLength.GetSize())
					{
					delete SQLResultLength.Get(0);
					SQLResultLength.RemoveAt(0);
					}
				return ( FALSE );	// failed to bind column
				}
			++Index;
			}
		while (DB.NextRecord())
			{
			RDDO = new DICOMDataObject;
			Index = 0;
			while ( Index < CCIndex )
				{
				VRLength = SQLRealSize(SQLResult.Get(Index),
							*SQLResultLength.Get(Index));
				vr = ConstructVRFromSQL (
					DBQMaster.Get(Index),
					EMask.Get(Index)->Group,
					EMask.Get(Index)->Element,
					VRLength,
					SQLResult.Get(Index));
				RDDO->Push(vr);

				if (qSeries) 
					qSeries->Push(ConstructVRFromSQL (DBQMaster.Get(Index), EMask.Get(Index)->Group, EMask.Get(Index)->Element, VRLength, SQLResult.Get(Index)));
				if (qSops) 
					qSops->Push(ConstructVRFromSQL (DBQMaster.Get(Index), EMask.Get(Index)->Group, EMask.Get(Index)->Element, VRLength, SQLResult.Get(Index)));
			
				++Index;
				}

			if (QueriesReturnISO_IR)
				{
				vr = RDDO->GetVR(0x0008, 0x0005);
				if(!vr)
					{
					VR *vrNew = new VR(0x0008, 0x0005, 10, TRUE);
					sprintf((char*)vrNew->Data, "ISO_IR %d", QueriesReturnISO_IR);
					RDDO->Push(vrNew);
					}
				}

			if(SendAE)
				RDDO->Push(ConstructAE());

			if (qSeries)
				{
				RDDO->Push(ComputeField(qSeries, 0x0020, 0x1206, mask, DB.db_type!=DT_DBASEIII));
				qSeries->Reset();
				}

			if (qSops)
				{
				RDDO->Push(ComputeField(qSops, 0x0020, 0x1208, mask, DB.db_type!=DT_DBASEIII));
				qSops->Reset();
				}

			ADDO->Add(RDDO);
			}
		}
	else
		{
		char res[256];
		SQLLEN len;
		DB.BindField (1, SQL_C_CHAR, res, 255, &len);
		DB.NextRecord();
		DICOMDataObject *rd=NULL;
		for (int i=0; i<atoi(res); i++) ADDO->Add(rd);
		}

	SystemDebug.printf("Records = %d\n", ADDO->GetSize());

	while(SQLResult.GetSize())
		{
		delete SQLResult.Get(0);
		SQLResult.RemoveAt(0);
		}
	while(SQLResultLength.GetSize())
		{
		delete SQLResultLength.Get(0);
		SQLResultLength.RemoveAt(0);
		}

	/* LJ: Leak!! */
	while(EMaskStudy.GetSize())
		{
		delete EMaskStudy.Get(0);
		EMaskStudy.RemoveAt(0);
		}
	while(EMaskPatient.GetSize())
		{
		delete EMaskPatient.Get(0);
		EMaskPatient.RemoveAt(0);
		}

	if (qSeries) delete qSeries;
	if (qSops) delete qSops;

	return ( TRUE );
	}

BOOL	QueryOnSeries (
	DICOMDataObject *DDO,
	Array < DICOMDataObject *> *ADDO)
	{
	UINT				Index, CIndex, CCIndex;
	UINT32				VRLength;
	DICOMDataObject			*RDDO;
	Database			DB;
	char				*SQLResultString;
	Array < char * >		SQLResult, SQLResultStudy, SQLResultPatient,
					SQLResultSeries;
	Array < DBENTRY * >		DBQPatient, DBQStudy, DBQSeries;
	Array < DBENTRY * >		DBQMaster;
	Array < VR * >			EMaskPatient, EMaskStudy, EMaskSeries, EMask;
	Array < SQLLEN *>		SQLResultLength;
	DBENTRY				*TempDBEPtr;
	char				*DBR;
	SQLLEN				*DBL;
	VR				*vr;
	BOOL				SendAE = FALSE;
	char				Order[128];
	char				OrderCalc[128];
	char				OrderExp[128];
	char				TempString [ 8192 ];
	char				SearchString [ 8192 ];
	char				ColumnString [ 4096 ];
	char				Tables [ 256 ];
        char				*Sort=NULL;
      	DICOMDataObject 		*qSops = NULL;
	UINT16				mask = 0xffff;
	BOOL				CountOnly = FALSE;

	SystemDebug.printf("Query On Series\n");

	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		{
		DB.PrintLastError();
		return ( FALSE );	// failed open
		}

	// First.  Check that all the asked-for elements are actually in
	// the Patient/Study database.  If they are not, well, then we
	// return FALSE.

	Order[0] = OrderCalc[0] = OrderExp[0] = 0;
	while ((vr = DDO->Pop()))
		{
		if(vr->Element == 0x0000)
			{
			delete	vr;
			continue;	// discard length codes
			}

		if(vr->Group == 0x0002)
			if(vr->Element == 0x0010)
				{
				delete	vr;
				continue;	// discard transfer syntax
				}
		if(vr->Group == 0x0008)
			if(vr->Element == 0x0052)
				{
				delete	vr;
				continue;	// discard model level
				}

		if(vr->Group == 0x0008)
			if(vr->Element == 0x0054)
				{
				SendAE = TRUE;
				delete vr;
				continue;	// discard it (but send it)
				}

		if(vr->Group == 0x0020 && EnableComputedFields)
			if(vr->Element == 0x1209)
				{
				qSops = new DICOMDataObject;
				delete	vr;
				continue;	// discard 'Number of' items (but send them)
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0802)
				{
				mask = vr->GetUINT16();
				delete vr;
				continue;	// discard it
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0900)
				{
				mask = vr->GetUINT16();
				delete vr;
				continue;	// discard it
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0c00)
				{
				memcpy(Order, vr->Data, vr->Length);
				Order[vr->Length]=0;
				if (strstr(Order, "Number")) 
                                { Order[10]=0;
                                  if (DB.db_type!=DT_POSTGRES) sprintf(OrderCalc, "(%s+0)", Order);
				  else sprintf(OrderCalc, "CAST(%s AS integer)", Order);
                                }
                                else
				{ Order[10]=0;
			          strcpy(OrderCalc, Order);
				}
			        strcpy(OrderExp, OrderCalc);
				delete vr;
				continue;	// discard it
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0c01)
				{
                                int len = strlen(OrderCalc);
                                strcat(OrderCalc, " LIMIT ");
				memcpy(OrderCalc+len+7, vr->Data, vr->Length);
				OrderCalc[vr->Length+len+7]=0;
                                }
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0c02)
				{
                                int len = strlen(OrderCalc);
                                strcat(OrderCalc, " OFFSET ");
				memcpy(OrderCalc+len+8, vr->Data, vr->Length);
				OrderCalc[vr->Length+len+8]=0;
                                }
		if(vr->Group == 0x9999)
			if(vr->Element == 0x9999)
				{
				CountOnly = TRUE;
				delete vr;
				continue;	// discard it
				}

		if(VerifyIsInDBE(vr, StudyDB, TempDBEPtr))
			{
			SQLResultString = SetString(vr, NULL, 0);
			DICOM2SQLQuery(SQLResultString, &DB);
			SQLResultStudy.Add ( SQLResultString );
			EMaskStudy.Add ( vr );
			DBQStudy.Add ( TempDBEPtr );
			}
		else if(VerifyIsInDBE(vr, PatientDB, TempDBEPtr))
			{
			SQLResultString = SetString(vr, NULL, 0);
			DICOM2SQLQuery(SQLResultString, &DB);
			SQLResultPatient.Add ( SQLResultString );
			EMaskPatient.Add ( vr );
			DBQPatient.Add ( TempDBEPtr );
			}
		else if(VerifyIsInDBE(vr, SeriesDB, TempDBEPtr))
			{
			SQLResultString = SetString(vr, NULL, 0);
			DICOM2SQLQuery(SQLResultString, &DB);
			SQLResultSeries.Add ( SQLResultString );
			EMaskSeries.Add ( vr );
			DBQSeries.Add ( TempDBEPtr );
			}
		else
			{
			SystemDebug.printf("Queried item %4.4x %4.4x is not in the database\n",
				vr->Group, vr->Element);
			delete	vr;
			continue;
			}
		}

	// Prepare the query string.
	// from EMasks, and SQLResults

	SearchString[0] = '\0';
	ColumnString[0] = '\0';

	Index = 0;CIndex = 0;CCIndex = 0;
	while ( Index < SQLResultSeries.GetSize() )
		{
		SQLResultString = SQLResultSeries.Get(Index);
		if(BuildSearchString(&DB, SeriesDB, SeriesTableName, EMaskSeries.Get(Index),
			SQLResultString, TempString, sizeof(TempString)-512))
			{
			if(CIndex++)
				safestrcat(SearchString, " and ", sizeof(SearchString));
			safestrcat(SearchString, TempString, sizeof(SearchString));
			}
		BuildColumnString(SeriesDB, SeriesTableName, EMaskSeries.Get(Index), TempString);
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, TempString);
		EMask.Add(EMaskSeries.Get(Index));
		DBQMaster.Add(DBQSeries.Get(Index));
		++Index;++CCIndex;
		}
	Index = 0;
	while ( Index < SQLResultPatient.GetSize() )
		{
		SQLResultString = SQLResultPatient.Get(Index);
		if(BuildSearchString(&DB, PatientDB, PatientTableName, EMaskPatient.Get(Index),
			SQLResultString, TempString, sizeof(TempString)-512))
			{
			if(CIndex++)
				safestrcat(SearchString, " and ", sizeof(SearchString));
			safestrcat(SearchString, TempString, sizeof(SearchString));
			}
		BuildColumnString(PatientDB, PatientTableName, EMaskPatient.Get(Index), TempString);
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, TempString);
		EMask.Add(EMaskPatient.Get(Index));
		DBQMaster.Add(DBQPatient.Get(Index));
		++Index;++CCIndex;
		}
	Index = 0;
	while ( Index < SQLResultStudy.GetSize() )
		{
		SQLResultString = SQLResultStudy.Get(Index);
		if(BuildSearchString(&DB, StudyDB, StudyTableName, EMaskStudy.Get(Index),
			SQLResultString, TempString, sizeof(TempString)-512))
			{
			if(CIndex++)
				safestrcat(SearchString, " and ", sizeof(SearchString));
			safestrcat(SearchString, TempString, sizeof(SearchString));
			}
		BuildColumnString(StudyDB, StudyTableName, EMaskStudy.Get(Index), TempString);
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, TempString);
		EMask.Add(EMaskStudy.Get(Index));
		DBQMaster.Add(DBQStudy.Get(Index));
		++Index;++CCIndex;
		}

	if (SQLResultPatient.GetSize())
		{
		if(CIndex++)
			safestrcat(SearchString, " and ", sizeof(SearchString));
		// Join Study into Patient
		sprintf(TempString, "%s.%s = %s.%s",
			StudyTableName, StudyDB [ LastDBE(StudyDB) ].SQLColumn,
			PatientTableName, StudyDB [ LastDBE(StudyDB) ].SQLColumn);
		safestrcat(SearchString, TempString, sizeof(SearchString));
		}

	if (SQLResultStudy.GetSize() || SQLResultPatient.GetSize())
		{
		if(CIndex++)
			safestrcat(SearchString, " and ", sizeof(SearchString));
		// Join Series into Study
		sprintf(TempString, "%s.%s = %s.%s",
			SeriesTableName, SeriesDB [ LastDBE(SeriesDB) ].SQLColumn,
			StudyTableName, SeriesDB [ LastDBE(SeriesDB) ].SQLColumn);
		safestrcat(SearchString, TempString, sizeof(SearchString));
		}

        if (Order[0])
		{
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, OrderExp);
		}
	else if (SeriesQuerySortOrder[0]) 
		{
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, SeriesQuerySortOrder);
		}

	if (SQLResultPatient.GetSize())
	  sprintf(Tables, "%s, %s, %s",
	  	SeriesTableName, StudyTableName, PatientTableName);
	else if (SQLResultStudy.GetSize())
	  sprintf(Tables, "%s, %s",
	  	SeriesTableName, StudyTableName);
	else
	  sprintf(Tables, "%s",
	  	SeriesTableName);
        
	if (CountOnly)
	  sprintf(ColumnString, "COUNT(1)");

	while(SQLResultSeries.GetSize())
		{
		delete SQLResultSeries.Get(0);
		SQLResultSeries.RemoveAt(0);
		}
	while(SQLResultStudy.GetSize())
		{
		delete SQLResultStudy.Get(0);
		SQLResultStudy.RemoveAt(0);
		}
	while(SQLResultPatient.GetSize())
		{
		delete SQLResultPatient.Get(0);
		SQLResultPatient.RemoveAt(0);
		}

	// Issue Query

        if (Order[0]) Sort = OrderCalc;
        else if (StudyQuerySortOrder[0]) Sort = SeriesQuerySortOrder;
	if (CountOnly) Sort = NULL;

	SystemDebug.printf("Issue Query on Columns: %s\n", ColumnString);
	SystemDebug.printf("Values: %.1000s\n", SearchString);
	SystemDebug.printf("Tables: %.1000s\n", Tables);
	if (Sort) SystemDebug.printf("Sort: %.1000s\n", Sort);

	if(strlen(SearchString))
		{
		if (!DB.QueryDistinct ( Tables, ColumnString, SearchString, Sort) )
			{
			DB.PrintLastError();
			return ( FALSE );	// failed query
			}
		}
	else
		if (!DB.QueryDistinct ( Tables, ColumnString, NULL, Sort))
			{
			DB.PrintLastError();
			return ( FALSE );	// failed query
			}

	if (!CountOnly)
		{
		Index = 0;
		while ( Index < CCIndex )
			{
			DBR = new char[255];
			DBR[0] = 0;			// in case a field is NULL it does not read
			SQLResult.Add(DBR);
			DBL = new SQLLEN;
			SQLResultLength.Add(DBL);
			if(!DB.BindField (Index+1, SQL_C_CHAR,
				SQLResult.Get(Index), 255,
				SQLResultLength.Get(Index)))
				{
				SystemDebug.printf("***Column Number : %d\n", Index+1);
				DB.PrintLastError();
				while(SQLResult.GetSize())
					{
					delete SQLResult.Get(0);
					SQLResult.RemoveAt(0);
					}
				while(SQLResultLength.GetSize())
					{
					delete SQLResultLength.Get(0);
					SQLResultLength.RemoveAt(0);
					}
				return ( FALSE );	// failed to bind column
				}
			++Index;
			}
		while (DB.NextRecord())
			{
			RDDO = new DICOMDataObject;
			Index = 0;
			while ( Index < CCIndex )
				{
				VRLength = SQLRealSize(SQLResult.Get(Index),
							*SQLResultLength.Get(Index));
				vr = ConstructVRFromSQL (
					DBQMaster.Get(Index),
					EMask.Get(Index)->Group,
					EMask.Get(Index)->Element,
					VRLength,
					SQLResult.Get(Index));
				RDDO->Push(vr);

				if (qSops) 
					qSops->Push(ConstructVRFromSQL (DBQMaster.Get(Index), EMask.Get(Index)->Group, EMask.Get(Index)->Element, VRLength, SQLResult.Get(Index)));
			
				++Index;
				}

			if (QueriesReturnISO_IR)
				{
				vr = RDDO->GetVR(0x0008, 0x0005);
				if(!vr)
					{
					VR *vrNew = new VR(0x0008, 0x0005, 10, TRUE);
					sprintf((char*)vrNew->Data, "ISO_IR %d", QueriesReturnISO_IR);
					RDDO->Push(vrNew);
					}
				}

			if(SendAE)
				RDDO->Push(ConstructAE());

			if (qSops)
				{
				RDDO->Push(ComputeField(qSops, 0x0020, 0x1209, mask, DB.db_type!=DT_DBASEIII));
				qSops->Reset();
				}

			ADDO->Add(RDDO);
			}
		}
	else
		{
		char res[256];
		SQLLEN len;
		DB.BindField (1, SQL_C_CHAR, res, 255, &len);
		DB.NextRecord();
		DICOMDataObject *rd=NULL;
		for (int i=0; i<atoi(res); i++) ADDO->Add(rd);
		}

	SystemDebug.printf("Records = %d\n", ADDO->GetSize());

	while(SQLResult.GetSize())
		{
		delete SQLResult.Get(0);
		SQLResult.RemoveAt(0);
		}
	while(SQLResultLength.GetSize())
		{
		delete SQLResultLength.Get(0);
		SQLResultLength.RemoveAt(0);
		}

	/* LJ: Leak!! */
	while(EMaskSeries.GetSize())
		{
		delete EMaskSeries.Get(0);
		EMaskSeries.RemoveAt(0);
		}
	while(EMaskStudy.GetSize())
		{
		delete EMaskStudy.Get(0);
		EMaskStudy.RemoveAt(0);
		}
	while(EMaskPatient.GetSize())
		{
		delete EMaskPatient.Get(0);
		EMaskPatient.RemoveAt(0);
		}

	if (qSops) delete qSops;

	return ( TRUE );

	}

#define MAXLENGTH (MAXQUERYLENGTH-10000)

BOOL	QueryOnImage (
	DICOMDataObject *DDO,
	Array < DICOMDataObject *> *ADDO)
	{
	UINT				Index, CIndex, CCIndex;
	UINT32				VRLength;
	DICOMDataObject			*RDDO;
	Database			DB;
	char				*SQLResultString;
	Array < char * >		SQLResult, SQLResultStudy, SQLResultPatient,
					SQLResultSeries, SQLResultImage;
	Array < DBENTRY * >		DBQPatient, DBQStudy, DBQSeries, DBQImage;
	Array < DBENTRY * >		DBQMaster;
	Array < VR * >			EMaskPatient, EMaskStudy, EMaskSeries, EMaskImage, EMask;
	Array < SQLLEN *>		SQLResultLength;
	DBENTRY				*TempDBEPtr;
	char				*DBR;
	SQLLEN				*DBL;
	VR				*vr;
	BOOL				SendAE = FALSE, SendObjectFile = FALSE;
	char				*TempString;
	char				*SearchString;
	char				Order [ 128 ];
	char				OrderCalc[128];
	char				OrderExp[128];
	char				ColumnString [ 4096 ];
	char				Tables [ 256 ];
        char				*Sort=NULL;
	UINT16				mask = 0xffff;
	BOOL				CountOnly = FALSE;

// Analysis of length of strings:
//
// let us assume max #fields to query is 100 (>> total fields in DB)
// max size columnstring is about 100 x (maxfieldname+2) = 1200 -> no problem
// max size tempstring is length of a single field = 'value' string
// max size searchstring is total (length of field = 'value' strings + 5)
//                          + 172 (for joins)
// it is no problem here to malloc these strings at really large size
// values add up to max 1000 if no multiple used, fields = add up to max 3000
// if multiple is used at one level (reasonable), max 67 per entry
// Assume max slices is 4000 for 1 series, max searchstring: 
// 4000 + 4000*67 = 272000, max tempstring: 30 + 4000*67 = 268030, 
// set lets say 300 kb each. This value is defined in MAXLENGTH

// Most likely the max at lower query levels are much smaller, and 8 kb 
// will be fine at these levels (allows multiplicity of 59)

	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		{
		DB.PrintLastError();
          	return ( FALSE );	// failed open
        	}

        TempString = NULL;
        SearchString = NULL;

        TempString = (char *)malloc(MAXLENGTH+1);
        SearchString = (char *)malloc(MAXLENGTH+1);

	if (TempString==NULL || SearchString==NULL)
        	{ 
		free(TempString);
          	free(SearchString);
          	return FALSE;
        	}

	SystemDebug.printf("Query On Image\n");
	Order[0] = OrderCalc[0] = OrderExp[0] = 0;

	while ((vr = DDO->Pop()))
		{
		if(vr->Element == 0x0000)
			{
			delete	vr;
			continue;	// discard length codes
			}
		if(vr->Group == 0x0002)
			if(vr->Element == 0x0010)
				{
				delete	vr;
				continue;	// discard transfer syntax
				}
		if(vr->Group == 0x0008)
			if(vr->Element == 0x0052)
				{
				delete	vr;
				continue;	// discard model level
				}
		if(vr->Group == 0x0008)
			if(vr->Element == 0x0054)
				{
				SendAE = TRUE;
				delete vr;
				continue;	// discard it (but send it)
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0800)
				{
				SendObjectFile = TRUE;
				delete vr;
				continue;	// discard it (but send it)
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0801)
				{
				SendObjectFile = TRUE;
				delete vr;
				continue;	// discard it (but send it)
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0802)
				{
				mask = vr->GetUINT16();
				delete vr;
				continue;	// discard it
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0900)
				{
				mask = vr->GetUINT16();
				delete vr;
				continue;	// discard it
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0c00)
				{
				memcpy(Order, vr->Data, vr->Length);
				Order[vr->Length]=0;
				if (strstr(Order, "Number")) 
                                { Order[10]=0;
                                  if (DB.db_type!=DT_POSTGRES) sprintf(OrderCalc, "(%s+0)", Order);
				  else sprintf(OrderCalc, "CAST(%s AS integer)", Order);
                                }
                                else
				{ Order[10]=0;
			          strcpy(OrderCalc, Order);
				}
			        strcpy(OrderExp, OrderCalc);
				delete vr;
				continue;	// discard it
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0c01)
				{
                                int len = strlen(OrderCalc);
                                strcat(OrderCalc, " LIMIT ");
				memcpy(OrderCalc+len+7, vr->Data, vr->Length);
				OrderCalc[vr->Length+len+7]=0;
                                }
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0c02)
				{
                                int len = strlen(OrderCalc);
                                strcat(OrderCalc, " OFFSET ");
				memcpy(OrderCalc+len+8, vr->Data, vr->Length);
				OrderCalc[vr->Length+len+8]=0;
                                }
		if(vr->Group == 0x9999)
			if(vr->Element == 0x9999)
				{
				CountOnly = TRUE;
				delete vr;
				continue;	// discard it
				}

		if(VerifyIsInDBE(vr, StudyDB, TempDBEPtr))
			{
			SQLResultString = SetString(vr, NULL, 0);
			DICOM2SQLQuery(SQLResultString, &DB);
			SQLResultStudy.Add ( SQLResultString );
			EMaskStudy.Add ( vr );
			DBQStudy.Add ( TempDBEPtr );
			}
		else if(VerifyIsInDBE(vr, PatientDB, TempDBEPtr))
			{
			SQLResultString = SetString(vr, NULL, 0);
			DICOM2SQLQuery(SQLResultString, &DB);
			SQLResultPatient.Add ( SQLResultString );
			EMaskPatient.Add ( vr );
			DBQPatient.Add ( TempDBEPtr );
			}
		else if(VerifyIsInDBE(vr, SeriesDB, TempDBEPtr))
			{
			SQLResultString = SetString(vr, NULL, 0);
			DICOM2SQLQuery(SQLResultString, &DB);
			SQLResultSeries.Add ( SQLResultString );
			EMaskSeries.Add ( vr );
			DBQSeries.Add ( TempDBEPtr );
			}
		else if(VerifyIsInDBE(vr, ImageDB, TempDBEPtr))
			{
			SQLResultString = SetString(vr, NULL, 0);
			DICOM2SQLQuery(SQLResultString, &DB);
			SQLResultImage.Add ( SQLResultString );
			EMaskImage.Add ( vr );
			DBQImage.Add(TempDBEPtr);
			}
		else
			{
			SystemDebug.printf("Queried item %4.4x %4.4x is not in the database\n",
				vr->Group, vr->Element);
			delete	vr;
			continue;
			}
		}

	// Prepare the query string.
	// from EMasks, and SQLResults

	SearchString[0] = '\0';
	ColumnString[0] = '\0';

	Index = 0;CIndex = 0;CCIndex = 0;
	while ( Index < SQLResultImage.GetSize() )
		{
		SQLResultString = SQLResultImage.Get(Index);
		if(BuildSearchString(&DB, ImageDB, ImageTableName, EMaskImage.Get(Index),
			SQLResultString, TempString, MAXLENGTH-4000))
			{
			if(CIndex++)
				safestrcat(SearchString, " and ", MAXLENGTH);
			safestrcat(SearchString, TempString, MAXLENGTH);
			}
		BuildColumnString(ImageDB, ImageTableName, EMaskImage.Get(Index), TempString);
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, TempString);
		EMask.Add(EMaskImage.Get(Index));
		DBQMaster.Add(DBQImage.Get(Index));
		++Index;++CCIndex;
		}
	Index = 0;
	while ( Index < SQLResultSeries.GetSize() )
		{
		SQLResultString = SQLResultSeries.Get(Index);
		if(BuildSearchString(&DB, SeriesDB, SeriesTableName, EMaskSeries.Get(Index),
			SQLResultString, TempString, MAXLENGTH-4000))
			{
			if(CIndex++)
				safestrcat(SearchString, " and ", MAXLENGTH);
			safestrcat(SearchString, TempString, MAXLENGTH);
			}
		BuildColumnString(SeriesDB, SeriesTableName, EMaskSeries.Get(Index), TempString);
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, TempString);
		EMask.Add(EMaskSeries.Get(Index));
		DBQMaster.Add(DBQSeries.Get(Index));
		++Index;++CCIndex;
		}
	Index = 0;
	while ( Index < SQLResultPatient.GetSize() )
		{
		SQLResultString = SQLResultPatient.Get(Index);
		if(BuildSearchString(&DB, PatientDB, PatientTableName, EMaskPatient.Get(Index),
			SQLResultString, TempString, MAXLENGTH-4000))
			{
			if(CIndex++)
				safestrcat(SearchString, " and ", MAXLENGTH);
			safestrcat(SearchString, TempString, MAXLENGTH);
			}
		BuildColumnString(PatientDB, PatientTableName, EMaskPatient.Get(Index), TempString);
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, TempString);
		EMask.Add(EMaskPatient.Get(Index));
		DBQMaster.Add(DBQPatient.Get(Index));
		++Index;++CCIndex;
		}
	Index = 0;
	while ( Index < SQLResultStudy.GetSize() )
		{
		SQLResultString = SQLResultStudy.Get(Index);
		if(BuildSearchString(&DB, StudyDB, StudyTableName, EMaskStudy.Get(Index),
			SQLResultString, TempString, MAXLENGTH-4000))
			{
			if(CIndex++)
				safestrcat(SearchString, " and ", MAXLENGTH);
			safestrcat(SearchString, TempString, MAXLENGTH);
			}
		BuildColumnString(StudyDB, StudyTableName, EMaskStudy.Get(Index), TempString);
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, TempString);
		EMask.Add(EMaskStudy.Get(Index));
		DBQMaster.Add(DBQStudy.Get(Index));
		++Index;++CCIndex;
		}

	if (SQLResultPatient.GetSize())
		{
		if(CIndex++)
			safestrcat(SearchString, " and ", MAXLENGTH);
		// Join Study into Patient
		sprintf(TempString, "%s.%s = %s.%s",
			StudyTableName, StudyDB [ LastDBE(StudyDB) ].SQLColumn,
			PatientTableName, StudyDB [ LastDBE(StudyDB) ].SQLColumn);
		safestrcat(SearchString, TempString, MAXLENGTH);
		}

	if (SQLResultStudy.GetSize() || SQLResultPatient.GetSize())
		{
		// Join Series into Study
		if (CIndex++)
			safestrcat(SearchString, " and ", MAXLENGTH);
		sprintf(TempString, "%s.%s = %s.%s",
			SeriesTableName, SeriesDB [ LastDBE(SeriesDB) ].SQLColumn,
			StudyTableName, SeriesDB [ LastDBE(SeriesDB) ].SQLColumn);
		safestrcat(SearchString, TempString, MAXLENGTH);
		}

	if (SQLResultSeries.GetSize() || SQLResultStudy.GetSize() || SQLResultPatient.GetSize())
		{
		if(CIndex++)
			safestrcat(SearchString, " and ", MAXLENGTH);
		// Join Image into Series
		sprintf(TempString, "%s.%s = %s.%s",
			ImageTableName, ImageDB [ LastDBE(ImageDB) ].SQLColumn,
			SeriesTableName, ImageDB [ LastDBE(ImageDB) ].SQLColumn);
		safestrcat(SearchString, TempString, MAXLENGTH);
		}

	if (SendObjectFile)
		{
		strcat(ColumnString, ",");
		strcat(ColumnString, ImageTableName);
		strcat(ColumnString, ".ObjectFile");
		strcat(ColumnString, ",");
		strcat(ColumnString, ImageTableName);
		strcat(ColumnString, ".DeviceName");
		}

        if (Order[0])
		{
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, OrderExp);
		}
	else if (ImageQuerySortOrder[0]) 
		{
		if(CCIndex)
			strcat(ColumnString, ", ");
		strcat(ColumnString, ImageQuerySortOrder);
		}

	if (SQLResultPatient.GetSize())
	  sprintf(Tables, "%s, %s, %s, %s",
	  	ImageTableName, SeriesTableName, StudyTableName, PatientTableName);
	else if (SQLResultStudy.GetSize())
	  sprintf(Tables, "%s, %s, %s",
	  	ImageTableName, SeriesTableName, StudyTableName);
	else if (SQLResultSeries.GetSize())
	  sprintf(Tables, "%s, %s",
	  	ImageTableName, SeriesTableName);
        else
	  sprintf(Tables, "%s",
	  	ImageTableName);

	if (CountOnly)
	  sprintf(ColumnString, "COUNT(1)");

	while(SQLResultImage.GetSize())
		{
		delete SQLResultImage.Get(0);
		SQLResultImage.RemoveAt(0);
		}
	while(SQLResultSeries.GetSize())
		{
		delete SQLResultSeries.Get(0);
		SQLResultSeries.RemoveAt(0);
		}
	while(SQLResultStudy.GetSize())
		{
		delete SQLResultStudy.Get(0);
		SQLResultStudy.RemoveAt(0);
		}
	while(SQLResultPatient.GetSize())
		{
		delete SQLResultPatient.Get(0);
		SQLResultPatient.RemoveAt(0);
		}

	// Issue Query
        if (Order[0]) Sort = OrderCalc;
        else if (ImageQuerySortOrder[0]) Sort = ImageQuerySortOrder;
	if (CountOnly) Sort = NULL;

	SystemDebug.printf("Issue Query on Columns: %s\n", ColumnString);
	SystemDebug.printf("Values: %.1000s\n", SearchString);
	SystemDebug.printf("Tables: %.1000s\n", Tables);
	if (Sort) SystemDebug.printf("Sort: %.1000s\n", Sort);

	if(strlen(SearchString))
		{
		if (!DB.QueryDistinct ( Tables, ColumnString, SearchString, Sort) )
			{
			DB.PrintLastError();
			free(TempString);
        	  	free(SearchString);
			return ( FALSE );	// failed query
			}
		}
	else
		if (!DB.QueryDistinct ( Tables, ColumnString, NULL, Sort))
			{
			DB.PrintLastError();
			free(TempString);
        	  	free(SearchString);
			return ( FALSE );	// failed query
			}

	if (!CountOnly)
		{
		Index = 0;
		while ( Index < CCIndex )
			{
			DBR = new char[255];
			DBR[0] = 0;			// in case a field is NULL it does not read
			SQLResult.Add(DBR);
			DBL = new SQLLEN;
			SQLResultLength.Add(DBL);
			if(!DB.BindField (Index+1, SQL_C_CHAR,
				SQLResult.Get(Index), 255,
				SQLResultLength.Get(Index)))
				{
				SystemDebug.printf("Column Number : %d\n", Index+1);
				DB.PrintLastError();
				while(SQLResult.GetSize())
					{
					delete SQLResult.Get(0);
					SQLResult.RemoveAt(0);
					}
				while(SQLResultLength.GetSize())
					{
					delete SQLResultLength.Get(0);
					SQLResultLength.RemoveAt(0);
					}
				free(TempString);
        	  		free(SearchString);
				return ( FALSE );	// failed to bind column
				}
			++Index;
			}

		if (SendObjectFile)			// need two more columns
			for (int i=0; i<2; i++)
			{
			DBR = new char[255];
			DBR[0] = 0;			// in case a field is NULL it does not read
			SQLResult.Add(DBR);
			DBL = new SQLLEN;
			SQLResultLength.Add(DBL);
			if(!DB.BindField (Index+1, SQL_C_CHAR,
				SQLResult.Get(Index), 255,
				SQLResultLength.Get(Index)))
				{
				SystemDebug.printf("Column Number : %d\n", Index+1);
				DB.PrintLastError();
				while(SQLResult.GetSize())
					{
					delete SQLResult.Get(0);
					SQLResult.RemoveAt(0);
					}
				while(SQLResultLength.GetSize())
					{
					delete SQLResultLength.Get(0);
					SQLResultLength.RemoveAt(0);
					}
				free(TempString);
        	  		free(SearchString);
				return ( FALSE );	// failed to bind column
				}
			++Index;
			}

		while (DB.NextRecord())
			{
			RDDO = new DICOMDataObject;
			Index = 0;
			while ( Index < CCIndex )
				{
				VRLength = SQLRealSize(SQLResult.Get(Index),
							*SQLResultLength.Get(Index));
				vr = ConstructVRFromSQL (
					DBQMaster.Get(Index),
					EMask.Get(Index)->Group,
					EMask.Get(Index)->Element,
					VRLength,
					SQLResult.Get(Index));
				RDDO->Push(vr);
				++Index;
				}

			if ( SendObjectFile )	// filename in 0x9999,0x0800; devicename in 0x9999,0x801
				{
				VRLength = SQLRealSize(SQLResult.Get(Index),
								*SQLResultLength.Get(Index));
				vr = ConstructVRFromSQL (
					NULL,
					0x9999,
					0x800,
					VRLength,
					SQLResult.Get(Index));
				RDDO->Push(vr);
				++Index;

				VRLength = SQLRealSize(SQLResult.Get(Index),
							*SQLResultLength.Get(Index));
				vr = ConstructVRFromSQL (
					NULL,
					0x9999,
					0x801,
					VRLength,
					SQLResult.Get(Index));
				RDDO->Push(vr);
				++Index;
				}

			if (QueriesReturnISO_IR)
				{
				vr = RDDO->GetVR(0x0008, 0x0005);
				if(!vr)
					{
					VR *vrNew = new VR(0x0008, 0x0005, 10, TRUE);
					sprintf((char*)vrNew->Data, "ISO_IR %d", QueriesReturnISO_IR);
					RDDO->Push(vrNew);
					}
				}

			if(SendAE)
				RDDO->Push(ConstructAE());

			ADDO->Add(RDDO);
			}
		}
	else
		{
		char res[256];
		SQLLEN len;
		DB.BindField (1, SQL_C_CHAR, res, 255, &len);
		DB.NextRecord();
		DICOMDataObject *rd=NULL;
		for (int i=0; i<atoi(res); i++) ADDO->Add(rd);
		}

	SystemDebug.printf("Records = %d\n", ADDO->GetSize());

	while(SQLResult.GetSize())
		{
		delete SQLResult.Get(0);
		SQLResult.RemoveAt(0);
		}
	while(SQLResultLength.GetSize())
		{
		delete SQLResultLength.Get(0);
		SQLResultLength.RemoveAt(0);
		}

	/* LJ: Leak!! */
	while(EMaskImage.GetSize())
		{
		delete EMaskImage.Get(0);
		EMaskImage.RemoveAt(0);
		}
	while(EMaskSeries.GetSize())
		{
		delete EMaskSeries.Get(0);
		EMaskSeries.RemoveAt(0);
		}
	while(EMaskStudy.GetSize())
		{
		delete EMaskStudy.Get(0);
		EMaskStudy.RemoveAt(0);
		}
	while(EMaskPatient.GetSize())
		{
		delete EMaskPatient.Get(0);
		EMaskPatient.RemoveAt(0);
		}

	free(TempString);
        free(SearchString);
	return ( TRUE );
	}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Modality worklist query (out of database)
///////////////////////////////////////////////////////////////////////////////////////////////////

// unfold a query DDO with sequences in it to a flat query
// DDO: 	input DICOM sequence
// DB:  	database definition
// EMask: 	output array of VR's found in sequence and in DB
// SQLResult:	field='value' string to build query of VR's found in sequence and in DB
// DBQ:		points to field definition of VR's found in sequence and in DB
// Levels:	nesting in sequences of VR's found in sequence and in DB
// level:	nesting of this sequence

static void ProcessQuery(DICOMDataObject *DDO, DBENTRY *DB, Array <VR *> *EMask, Array <char *> *SQLResult, 
			Array <DBENTRY *> *DBQ, Array <int> *Levels, int level, Database *dbf)
	{
	VR *vr;
	DBENTRY	*TempDBEPtr;
	char *SQLResultString;

	while ((vr = DDO->Pop()))
		{
		if(vr->Element == 0x0000)
			{
			delete	vr;
			continue;	// discard length codes
			}

		if (vr->SQObjectArray)
			{
			Array < DICOMDataObject	* > *aSQArray;
			aSQArray = (Array < DICOMDataObject * > *) vr->SQObjectArray;
			if ( aSQArray->GetSize()<=1 )
				{
				char *c = NULL; SQLResult->Add ( c );			// NULL, vr, level, NULL: deeper sequence
				EMask->Add ( vr );
				int l = level; Levels->Add ( l );
				DBENTRY *d=NULL; DBQ->Add ( d );	

				ProcessQuery(aSQArray->Get(0), DB, EMask, SQLResult, DBQ, Levels, level+1, dbf);
				continue;		
				}
			continue;		
			}				

		if(!VerifyIsInDBE(vr, DB, TempDBEPtr))
			{
			SystemDebug.printf("Queried item %4.4x %4.4x is not in the database\n",
				vr->Group, vr->Element);
			delete	vr;
			continue;
			}
		else
			{
			SQLResultString = SetString(vr, NULL, 0);
			DICOM2SQLQuery(SQLResultString, dbf);

			SQLResult->Add ( SQLResultString );				// field=value, vr, level, DBE: vr
			EMask->Add ( vr );
			Levels->Add ( level );
			DBQ->Add ( TempDBEPtr );	
			}

		}
	}

// Build a N=1 sequence out of a flat data query into vr as from emask - taking all items at the same level as sequence
// EMask: 		input array of VR's found in sequence in query and in DB
// SQLResult:		input array of read value strings (NULL is sequence starter)
// SQLResultLength: 	input array of read value lenghts
// DBQMaster:		input array of field definitions
// Levels:		input array of nesting level in sequences, sequence stops when decreases, nests when increases
// DDO: 		DICOM object to append sequence too
// Index:		Index in array of sequence fields: array[Index]: vr of sequence, array[Index+N]: vr's of sequence contents

static void CodeSequence(Array < VR * >*EMask, Array <char *> *SQLResult, Array <SQLLEN *> *SQLResultLength, Array <DBENTRY *> *DBQMaster, 
			Array <int> *Levels, DICOMDataObject *DDO, UINT *Index)
	{
	UINT32	VRLength;
	VR 	*vr, *vrs;
	int	level;// ind = *Index;

	level = Levels->Get(*Index);

	SystemDebug.printf("Coding sequence (%04x,%04x), level=%d\n", EMask->Get(*Index)->Group, EMask->Get(*Index)->Element, level);

	if (*Index+1 >= Levels->GetSize() || Levels->Get(*Index+1) <= level) 
		{ 
		SystemDebug.printf("(sequence is empty)\n");
		vrs = new VR(EMask->Get(*Index)->Group, EMask->Get(*Index)->Element, 0, (void*)NULL, FALSE);
		DDO->Push(vrs);
		(*Index)++;
		return;	// allow empty sequence
		}

	Array < DICOMDataObject * > *SQE = new Array < DICOMDataObject * >;
	vrs = new VR(EMask->Get(*Index)->Group, EMask->Get(*Index)->Element, 0, (void*)NULL, FALSE);
	vrs->SQObjectArray = (void*) SQE;

	DICOMDataObject *D = new DICOMDataObject;

	(*Index)++;

	while ( *Index < EMask->GetSize() )
		{
		if (Levels->Get(*Index)<=level) break;
		if (SQLResult->Get(*Index))
			{
			VRLength = SQLRealSize(SQLResult->Get(*Index), *SQLResultLength->Get(*Index));
			vr = ConstructVRFromSQL (
				DBQMaster->Get(*Index),
				EMask->Get(*Index)->Group,
				EMask->Get(*Index)->Element,
				VRLength,
				SQLResult->Get(*Index));
			DumpVR(vr, NULL, level-1);
			D->Push(vr);
			(*Index)++;
			}
		else
			CodeSequence(EMask, SQLResult, SQLResultLength, DBQMaster, Levels, D, Index);
		}

	SQE->Add(D);
	DDO->Push(vrs);

//	SystemDebug.printf("end coding sequence (%04x,%04x), level=%d\n", EMask->Get(ind)->Group, EMask->Get(ind)->Element, level);
	}

BOOL	QueryOnModalityWorkList (
	DICOMDataObject *DDO,
	Array < DICOMDataObject *> *ADDO)
	{
	UINT				Index, CIndex, CCIndex, FIndex;
	UINT32				VRLength;
	DICOMDataObject			*RDDO;
	Database			DB;
	char				*SQLResultString;
	Array < char * >		SQLResult, SQLResultWorkList;
	Array < VR * >			EMaskWorkList, EMask;
	Array < SQLLEN *>		SQLResultLength;
	Array < DBENTRY * >		DBQWorkList;
	Array < DBENTRY * >		DBQMaster;
	Array < int >			Levels;
	DBENTRY				*TempDBEPtr;
	char				*DBR;
	SQLLEN				*DBL;
	VR				*vr;// *vrs;
	VR				VRPatientName;
	DBENTRY				*DBEntryPatientName;
	char				*Sorting;
	BOOL				DoSort;
	BOOL				SendAE = FALSE;
//	BOOL				SendScheduledProcedureStep = FALSE;
	char				TempString [ 8192 ];
	char				SearchString [ 8192 ];
	char				ColumnString [ 4096 ];
	char				Tables [ 256 ];
	char				SortOrder [ 256 ];
        char				*Sort=NULL;
	UINT16				mask = 0xffff;

	SystemDebug.printf("Query On Modality WorkList\n");

	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		{
		DB.PrintLastError();
		return ( FALSE );	// failed open
		}

	// First.  Check that all the asked-for elements are actually in the database.  
	DoSort = FALSE;
	while ((vr = DDO->Pop()))
		{
		if(vr->Element == 0x0000)
			{
			delete	vr;
			continue;	// discard length codes
			}
		if(vr->Group == 0x0002)
			if(vr->Element == 0x0010)
				{
				delete	vr;
				continue;	// discard transfer syntax
				}
		if(vr->Group == 0x0008)
			if(vr->Element == 0x0052)
				{
				delete	vr;
				continue;	// discard model level
				}
		if(vr->Group == 0x0008)
			if(vr->Element == 0x0054)
				{
				SendAE = TRUE;
				delete vr;
				continue;	// discard it (but send it)
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0802)
				{
				mask = vr->GetUINT16();
				delete vr;
				continue;	// discard it
				}
		if(vr->Group == 0x9999)
			if(vr->Element == 0x0900)
				{
				mask = vr->GetUINT16();
				delete vr;
				continue;	// discard it
				}

		if (vr->SQObjectArray)
			{
			Array < DICOMDataObject	* > *aSQArray;
			aSQArray = (Array < DICOMDataObject * > *) vr->SQObjectArray;
			if ( aSQArray->GetSize()<=1 )
				{
				char *c=NULL; SQLResultWorkList.Add ( c );	// NULL, vr, 1, NULL: code sequence
				EMaskWorkList.Add ( vr );
				int l=1; Levels.Add ( l );
				DBENTRY *d=NULL; DBQWorkList.Add ( d );	

				ProcessQuery(aSQArray->Get(0), WorkListDB, &EMaskWorkList, &SQLResultWorkList, &DBQWorkList, &Levels, 2, &DB);
				continue;		
				}
			delete vr;
			continue;		
			}				

		if(!VerifyIsInDBE(vr, WorkListDB, TempDBEPtr))
			{
			SystemDebug.printf("Queried item %4.4x %4.4x is not in the database\n",
				vr->Group, vr->Element);
			delete	vr;
			continue;
			}
		else
			{
			if(vr->Group == 0x0010)
				if(vr->Element == 0x0010)
					DoSort = TRUE;
			SQLResultString = SetString(vr, NULL, 0);
			DICOM2SQLQuery(SQLResultString, &DB);

			SQLResultWorkList.Add ( SQLResultString );		// field=value, vr, 1, DBE: code VR
			EMaskWorkList.Add ( vr );
			int l=1; Levels.Add ( l );
			DBQWorkList.Add ( TempDBEPtr );	
			}
		}

//	Index = 0;
//	while ( Index < Levels.GetSize() )
//		{
//		SystemDebug.printf("VR %4.4x %4.4x, level %d\n", EMaskWorkList.Get(Index)->Group, EMaskWorkList.Get(Index)->Element, Levels.Get(Index));
//		Index++;
//		}

	// Prepare the query string.
	// from EMasks, and SQLResults

	SearchString[0] = '\0';
	ColumnString[0] = '\0';

	Index = 0;CIndex = 0;CCIndex = 0;
	while ( Index < SQLResultWorkList.GetSize() )
		{
		SQLResultString = SQLResultWorkList.Get(Index);
		if (SQLResultString)						// flat query - but sequence starters (NULL) are not queried
			{
			if(BuildSearchString(&DB, WorkListDB, WorkListTableName, EMaskWorkList.Get(Index),
				SQLResultString, TempString, sizeof(TempString)-512))
				{
				if(CIndex++)
					safestrcat(SearchString, " and ", sizeof(SearchString));
				safestrcat(SearchString, TempString, sizeof(SearchString));
				}
			BuildColumnString(WorkListDB, WorkListTableName, EMaskWorkList.Get(Index), TempString);
			if(ColumnString[0])
				strcat(ColumnString, ", ");
			strcat(ColumnString, TempString);
			}
		EMask.Add(EMaskWorkList.Get(Index));
		DBQMaster.Add(DBQWorkList.Get(Index));
		++Index;++CCIndex;
		}

        sprintf(Tables, "%s",
	  	WorkListTableName);
        
	// Issue Query

	// (optionally) control sorting
	VRPatientName.Group = 0x0010;
	VRPatientName.Element = 0x0010;
	DBEntryPatientName = FindDBE(&VRPatientName);
	if(DBEntryPatientName)
		{
		sprintf(SortOrder, "%s.%s",
			WorkListTableName,
			DBEntryPatientName->SQLColumn);
		if(DoSort)
			Sorting = SortOrder;
		else
			Sorting = NULL;
		}
	else
		Sorting = NULL;

        Sort = Sorting;
	SystemDebug.printf("Issue Query on Columns: %s\n", ColumnString);
	SystemDebug.printf("Values: %.1000s\n", SearchString);
	SystemDebug.printf("Tables: %.1000s\n", Tables);
	if (DoSort) SystemDebug.printf("Sort: %s", Sort);

	if(strlen(SearchString))
		{
		if (!DB.QueryDistinct ( Tables, ColumnString, SearchString, Sort) )
			{
			DB.PrintLastError();
			return ( FALSE );	// failed query
			}
		}
	else
		if (!DB.QueryDistinct ( Tables, ColumnString, NULL, Sort))
			{
			DB.PrintLastError();
			return ( FALSE );	// failed query
			}

	// Allocate memory for and bind all fields that will actually be read (no sequence starters)
	Index = 0;
	FIndex = 1;
	while ( Index < CCIndex )
		{
		SQLResultString = SQLResultWorkList.Get(Index);
		if (SQLResultString)			// Sequence starters have a NULL here
			{
			DBR = new char[255];
			DBR[0] = 0;			// Initialize memory: in case a field is NULL it does not read
			SQLResult.Add(DBR);
			DBL = new SQLLEN;
			SQLResultLength.Add(DBL);
			if(!DB.BindField (FIndex, SQL_C_CHAR,
				SQLResult.Get(Index), 255,
				SQLResultLength.Get(Index)))
				{
				SystemDebug.printf("Column Number : %d\n", Index+1);
				DB.PrintLastError();
				while(SQLResult.GetSize())
					{
					delete SQLResult.Get(0);
					SQLResult.RemoveAt(0);
					}
				while(SQLResultLength.GetSize())
					{
					delete SQLResultLength.Get(0);
					SQLResultLength.RemoveAt(0);
					}
				while(SQLResultWorkList.GetSize())
					{
					delete SQLResultWorkList.Get(0);
					SQLResultWorkList.RemoveAt(0);
					}
				return ( FALSE );	// failed to bind column
				}
			FIndex++;
			}
		else
			{
			DBR = NULL;
			SQLResult.Add(DBR);
			DBL = NULL;
			SQLResultLength.Add(DBL);
			}
		++Index;
		}

	// Create dicom object out of each database record
	while (DB.NextRecord())
		{
		RDDO = new DICOMDataObject;
		Index = 0;
		while ( Index < CCIndex )
			{
			SQLResultString = SQLResultWorkList.Get(Index);
			if (!SQLResultString)		// sequence starter has a NULL here: build a sequence (with 1 item)
				CodeSequence(&EMask, &SQLResult, &SQLResultLength, &DBQMaster, &Levels, RDDO, &Index);

			else				// build a vr
				{
				VRLength = SQLRealSize(SQLResult.Get(Index), *SQLResultLength.Get(Index));
				vr = ConstructVRFromSQL (
					DBQMaster.Get(Index),
					EMask.Get(Index)->Group,
					EMask.Get(Index)->Element,
					VRLength,
					SQLResult.Get(Index));
	
				DumpVR(vr);
				RDDO->Push(vr);
				++Index;
				}
			}

		vr = RDDO->GetVR(0x0008, 0x1110);
		if(!vr)
			{
			VR *vrNew = new VR(0x0008, 0x1110, 0, (void*)NULL, FALSE);
			RDDO->Push(vrNew);
			}

		vr = RDDO->GetVR(0x0008, 0x1120);
		if(!vr)
			{
			VR *vrNew = new VR(0x0008, 0x1120, 0, (void*)NULL, FALSE);
			RDDO->Push(vrNew);
			}

		if (WorkListReturnsISO_IR_100)
			{
			vr = RDDO->GetVR(0x0008, 0x0005);
			if(!vr)
				{
				VR *vrNew = new VR(0x0008, 0x0005, 10, TRUE);
				sprintf((char*)vrNew->Data, "ISO_IR %d", WorkListReturnsISO_IR_100);
				RDDO->Push(vrNew);
				}
			}

		if(SendAE)
			RDDO->Push(ConstructAE());
		ADDO->Add(RDDO);
		}

	SystemDebug.printf("Records = %d\n", ADDO->GetSize());

	while(SQLResultWorkList.GetSize())
		{
		delete SQLResultWorkList.Get(0);
		SQLResultWorkList.RemoveAt(0);
		}
	while(SQLResult.GetSize())
		{
		delete SQLResult.Get(0);
		SQLResult.RemoveAt(0);
		}
	while(SQLResultLength.GetSize())
		{
		delete SQLResultLength.Get(0);
		SQLResultLength.RemoveAt(0);
		}
	while(EMaskWorkList.GetSize())
		{
		delete EMaskWorkList.Get(0);
		EMaskWorkList.RemoveAt(0);
		}
	while(Levels.GetSize())
		{
		Levels.RemoveAt(0);
		}

	return ( TRUE );
	}

