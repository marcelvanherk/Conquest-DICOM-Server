/*
19980415   MvH    blocked one error message
19990628 MvH+ljz  experimental locking call
19990628   MvH    temp defined DB_DEBUG and introduced experimental statement options
19990707   MvH    undefined DB_DEBUG and removed statement options
19990827   MvH    Added CreateIndex
20000629   ljz    Logging of trouble now starts with '***'
20001105   MvH    Run-time load ODBC routines for BC and WC
20001128   ljz    Added InitializeCriticalSection in 'Open' and DeleteCriticalSection
		  in 'Close'
20010829   mvh    Enabled DB_DEBUG to show db errors in verbose mode
20020412   mvh    Use SQLExecDirectWithRetry to retry in case of any error (deadlock=1205)
                  Added CreateUniqueIndex function, and GetLastError
                  Ignore error from unique index to make multi-user safe
20020413   mvh    NOTE: A SINGLE DB MAY NOT BE USED BY MULTIPLE THREADS
20020417   mvh    Extended deadlock wait time to about 1 min for safety
20021014   mvh    Added built-in dbf support for stable ODBC less-operation
                  Note: requires fully de-normalized database
                  notes: in dbsql, first bind row not set; in vrtosql; debug query print with %s in search crashes
20021016   mvh    Added about 1 MB of in-memory hashed index in patient ID for all tables; speed OK to 10.000 objects 
                  pack db on server start (for speed it is wise to keep DBF files on local harddisk)
20021017   mvh    Made NeedPack a global for control from dgate; fix dbase_trunc; fix in Updaterecords; 
                  fix first num field in AddRecord
20021020   mvh    Made querymodes an array, so that each query field can be key, key% or %key% 
                  Use querymode also when hashing the entries (part of db entry)
                  Fix process_escape for '' and \[ and also use it before dbase_write
20021028   mvh    Set open retries shorter for normal readonly access
20021030   mvh    Match alternative patientID fieldname such that built-in DBF queries work
20021113   mvh    Added <= comparison in dbase_query (used for date comparison)
20030113   mvh    Do not use () in sort order when multiple columns specified
20030128   mvh    Added alternative field names for series
20030321   ljz    Also handle 'write lock' for M$Access (NativeError 1205 for sql-server
                  equals -1102 in M$Access). Other dbf-drivers???
20030819   mvh    Allow longer filenames
20031104   mvh    Added \\ to process_escape
20040425   mvh    Added DoubleBackSlashToDB control
20040520   mvh    Speedup by fix dbaseIII hashing, setting hashfield was wrong
20040523   mvh    Implemented in-memory index to speed dbase queries; added IndexDBF and PackDBF settings
20040528   mvh    Fix built-in DbaseIII for > 2 GB; index fixed at 10.000.000 records
		  Fix problem when spaces in patient ID in index generation
20040528   mvh    Added ExtractRecords (dbase III only); fix update/delete for record 0
20040530   mvh    IndexDBF gives MB extra to allocate; skip dbf files starting with X
                  Convert normal queries in SeriesInstanceUID or StudyInstanceUID to indexed ones
20040601   mvh    Added LongQueryDBF flag: report if query took longer than LongQueryDBF ms (default 1000)
                  Min 20 MB index during regen; allow pack on command (NeedPack=TRUE; DB.Open)
                  Increase query buffer size to 16384 (often reread), pack buffer to 65536
20040605   mvh    Optimize speed for huge files, extract max 100000 records; made lockstart __int64
                  Do not lock for read in extract (is very slow for huge files)
                  Record lock no longer on physical bytes but on rec#; attempt to speed memory index
		  NeedPack==3 -> force pack
20040606   mvh    Fix lock whole file; fix index patient ID database; fix in dbcreate for test
20040609   mvh    Implemented fast in-memory locking and enabled for all shared files
20040610   mvh    on dbase_create set indexcount to 0; dbase_trunc truncs indexcount
20040612   mvh    put index creation in a thread; server runs (but does not find non yet indexed 
                  records) during indexing; implemented indexdbf command NeedPack=4
20040614   mvh    Added NeedPack=5: create non-threaded index (for archival options)
20040615   mvh    Added warning when index is full; keep indices when creating db
20040930   mvh    Added multiple value query; protect length of copy of where string
20041003   mvh    Reused very long SQLStatement string for wheres string
                  For lenghts see dbsql.cpp; tructated debug printfs
                  set cols length to 4096; malloc tmp in query at 310000;
                  for length calculation see dbsql.cpp
20041013   mvh	  Used MAXQUERYLENGTH
20041029   mvh    Fixes in multiple query (query string truncated + mode mix problem)
20041101   mvh	  DB is read only while in-memory indexing of DbaseIII active
20050102   mvh	  Small modifications to allow compile through total.cxx
20050107   mvh	  Export NumIndexing to tell dgate when indexing is done
20050116   mvh    Adapted for use with LINUX (built-in dbase driver only)
		  Notes: 1) delay poorly implemented; 2) no support for >2 GB db
20050118   mvh    Detach pthreads to avoid leaks
20050119   mvh    Documented rare crash in Fill_Memory_Index
20050124   mvh    Added support for huge files under Linux (not suitable for ext2 filesystem)
		  Allow = without spaces in query
20050205   mvh    Added counters for db activity
20050206   mvh    Small fixes to counters; allow "&" instead of " and " for CGI use
20050905   mvh    Postgres code by Piotr Filipczuk checked by mvh, will not affect 
                  other operations, but not tested yet (postgres will not compile on 
                  my linux system)
20050907   mvh    Merged change by Hans-Peter Hellemann: lseek64 not always defined, should be mapped by unistd.h
20050909   mvh    Put check for = in query below >=: date range matching failed (Thanks Paolo Marcheschi)
20050911   mvh    Removed postgres print statements
20051125   mvh    Fixed BindField for postgres - now returns actual and not reserved length
20051217   mvh    Maximum locks in dbaseIII driver from 1000 to 100 to speed up qunlock (20% regen speed)
20051218   mvh    Respond to -b = NOTHREAD: run server as single process, useful for debugging and profiling
20051219   mvh    Fixed slow leak of queryfields for built-in dbase driver
20051230   mvh    Put index of DicomWorkList on AccessionNumber
20060219   mvh    Start on native mysql driver
20060220   mvh    Some more work: get correct length in bindfield; todo: user and password
20060226   mvh    Runtime load MYSQL in WIN32
20060313   mvh    Use username and password for mysql
20060402   mvh    Start index threads with a short delay to give GUI priority to test startup
20060607   mvh    Changed escape char processing for sql values (not queries) to match dbsql.cpp change
20060618   mvh    Fix for Borland C++ builder
20060628   mvh    Moved location of disabled query logging
20060630   mvh    Speeded dbaseIII AddRecord lots and linux string handling
20060701   mvh    Added and use mysql_fetch_lengths; protect query result
20060702   mvh	  Tested native mysql driver: does not like fast open/close 
		  clonedb crashed after 4000 or so (avoided now)
20061212   mvh    Found critical error in built-in dbase driver sql parser:
                  Adding patient with ID containing certain character sequences will
                  immediately overwrite the whole patient database. Queries with these
		  sequences will return too many records (less dangerous)
		  Issue not yet realy fixed but greatly reduced its likelyhood
20061213   mvh    Fixed issue by replacing spaces inside strings in sql statement 
		  with non breaking spaces. Only match on keywords and field names 
		  which include spaces; replace with normal spaces in process_escape
20070123   mvh    clear NativeError, State and Msg on db.open (e.g. for dbf that does not use them but prints)
20070126   mvh    Added missing break in non breaking space field of process_escape: failed on multiple spaces in string
20070127   mvh    NativeError etc only defined in win32 version
20070201   mvh    DebugLevel 4 prints all sql activity
20070203   mvh    Fixed >2GB file support in linux: use O_LARGEFILE and lseek64() - sorry Hans-Peter Hellemann
20070206   mvh    Added CreateDatabase (created db and login)
20070207   mvh    Removed creation of login from CreateDtabase for mysql and postgres
20070210   mvh    Moved debug prints to SystemDebug; removed double log in QueryDistinct for dbaseIII
20070218   mvh    Added SqLite (win32 dll version): seems to run now; very slow when sync, very fast async
20070307   mvh    Made database type flag local, added Database(type) creator
20770308   bcb    Fixed DARWIN ifs, made DARWIN/MySQL auto start.( MySQL is included with DARWIN )
20070315   mvh    Merged; changed two nested ifs that had an else that would be wrong
20070406   mvh    Use reentrant rand_r and localtime_r when available
20070507   mvh    Native mysql does not correctly return empty fields (Steini)
20070702   mvh    Fixed above fix; test version for Stephan Winter
20070703   mvh    Removed test version code; display mysql connection errors on OperatorConsole
20070705   mvh    Added DT_DBASEIIINOINDEX: disables indexing and packing
 		  #define USESQLITE_DLL to load DLL; otherwise use statically linked sqlite3.c
		  Extented max number of dbf files to 20
20070709   mvh    Fixed crash when mysql not started: format error in the error print
20070711   mvh    Some fixes to compile with sqlite under linux
20070715   mvh    Removed debug print statements for sqlite
20071027   mvh    Maybe fix postgres error handling: reset ntuples
20071030   mvh    More fixes to postgres error handling: missing returns in PrintLastError
20071115   mvh    Fixed SQL database creation for SQL server 2005
20071118   mvh    Adapted for 64 bits (use SQLLEN for BindField)
20071124   mvh    Removed dangling WHERE and ORDER in SQL server interface when passing "" instead of NULL
20081116   mvh    Adapted for 64 bits linux (avoid use of "long", in dbase code and when reading SQL_C_ULONG)
20081117   mvh    Allow compile with both POSTGRES and USEMYSQL
20081119   mvh    added run-time loading of postgres for WIN32
20081120   mvh    Added UseEscapeStringConstants for dbase (is needed only in one place because in other 
		  place the E  in E'\\...' is igored by the current code)
20081121   mvh    Implemented CreateDatabase for Postgres; also try load libmysql64.dll (for easy 64 bit deployment)
20081124   mvh    Added DT_NULL (black hole)
20081125   mvh    Fix critical section in DT_NULL
20081126   mvh    Todo: all realloc of dbase index when full; don't use threads to fill
20090202   mvh    Fixed problem of database fail open in GUI after install: 
		  XADICOMPatients was never closed, as connected was not set when there were no databases
20090203   mvh    Fix crash on SQLite error
20090429   mvh    Added libpq64.dll
20090616   mvh    Added OldUID as hashfield: indices UIDMods table; increased default index to 10 MB
20090620   jf	  Include file stuff
20091230   mvh    Merged bcb change
20091231   bcb    Changed char* to const char* and added blank[]="" for gcc4.2 warnings
20100111   mvh    Merged
20100123   mvh    Allow host:port syntax for hostname in MySQL and PostGres
20100125   mvh    GetPrivateProfileString -> MyGetPrivateProfileString
20100309   bcb    Cast k to unsigned int, commented out unused variables (gcc4.2 Warnings)
20100309   bcb    Added retunn FALSE to the end of CreateDatabase
20100309   bcb    Added double parentheses (gcc4.2 Warnings)
20100619   bcb    Fix gcc4 warnings, improve speed and added #ifndefs and UNUSED_ARGUMENT.
20100703   mvh    Merged; but fixed random seed change (initialize static)
20100717   mvh    Merged
20100815   mvh    Fixed linux compile for SqLite
20100816   mvh    removed (a) from #ifndef EnterCriticalSection(a) etc
20101121   mvh    use strncpy to avoid access errors in Sqlite where fields may exceed defined length
20101121   mvh    Disallow SqLite with DoubleBackSlashToDB
20101129   mvh    Filter [x] like sequences in dbaseIII driver process_escape
20110102   mvh    retry connect to postgres once; more postgres error handling
20110104   mvh    Undone [x] change of 20101129
20110305   mvh    Replaced Sleep(555) in linux code by delay(555)
20110319   mvh    Normalized line endings
20110502   mvh    Added db:Exec
20111113   mvh    Fixed leak in DT_NULL driver (SQLStatement not freed)
20120107   mvh    Accept 64 bits DLL's also as e.g., x64\libpq.dll
20131029   mvh    typo in lPassword
20140629   mvh    Allow CONSTRAINT errors in mysql, pqsql and sqlite to deal wih race condition in UpdateOrAddToTable
20140701   mvh    Fixed CONSTRAINT for MySQL
20160313   bcb	  Removed incorrect * in mysql_errno checks
20160317   mvh	  Initialize th->n on dbase_open; rare accesses invalid index if non-init
20180805   mvh    Added SQLiteStartup parameter; defaults to "PRAGMA synchronous=OFF" as it was
20181231   mvh    Reset bind type array when binding 1, avoids write into unbound variables
20200119   mvh    Hardcode MYSQL_PORT to 3306
20200314   mvh    Removed UNENCRYPTED clause for postgres
20201020   mvh    Added SQL server 2627 as exception (duplicate index)
20201107   mvh    Allow multiple statements for MySQL (flag 65536)
20210509   mvh    Reduce ODBC retries to 6 times
20220817   mvh    Allow LIMIT and OFFSET to be added to the Sort parameter; some postprocessing for SQL server done; () rule for multiple sort was wrong
20230609   mvh    Added retry on SQLITE_LOCKED
*/

/*

Hi Stephan,

it is a mysql/windows issue: mysql uses temporary ports that are not
released quickly enough. Mysql dies after several thousand
connects/disconnects because all temporary ports are allocated. With the
debug log, the server is slightly slower and the problem does not happen.

See:

http://support.microsoft.com/kb/q196271/

http://bugs.mysql.com/bug.php?id=6580

http://bugs.mysql.com/bug.php?id=10498


Quotes:

2. Locate the following subkey in the registry, and then click Parameters:
HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters

3. On the Edit menu, click New, and then add the following registry entry:
Value Name: MaxUserPort
Value Type: DWORD
Value data: 65534
Valid Range: 5000-65534 (decimal)
Default: 0x1388 (5000 decimal)
Description: This parameter controls the maximum port number that is used
when a program requests any available user port from the system. Typically
, ephemeral (short-lived) ports are allocated between the values of 1024
and 5000 inclusive.
4. Quit Registry Editor.
Note An additional TCPTimedWaitDelay registry parameter determines how
long a closed port waits until the closed port can be reused.


So in the registry:

HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters

set:

MaxUserPort = 65534
TcpTimedWaitDelay = 30


*/

#ifndef DEBUG_MODE
#	define	DEBUG_MODE
#endif
//#define USESQLITE_DLL
#ifndef UNUSED_ARGUMENT
#define UNUSED_ARGUMENT(x) (void)x
#endif

#ifdef WIN32
#	include	<stdio.h>
#	include	<windows.h>
#	include	<sql.h>
#	include	<sqlext.h>
#endif
//#ifdef POSTGRES
//#	include "libpq-fe.h"
//#endif

#include <string.h> //DUCKHEAD92
#include <ctype.h> //DUCKHEAD92
#include <time.h> //DUCKHEAD92

#ifdef WIN32
# 	include <io.h>
#	include <sys\stat.h>
# 	include <sys/locking.h>
#  	include <dos.h>
# 	define delay Sleep
#       define O_LARGEFILE 0
#else
# 	include <unistd.h>
# 	include <dirent.h>
# 	define delay(a) sleep(1)
#	define O_BINARY 0
#	define sopen(a, b, c, d) open(a, b)
#ifndef EnterCriticalSection
#	define EnterCriticalSection(a) pthread_mutex_lock(a)
#endif
#ifndef LeaveCriticalSection
#	define LeaveCriticalSection(a) pthread_mutex_unlock(a)
#endif
#ifndef CRITICAL_SECTION
#	define CRITICAL_SECTION pthread_mutex_t
#endif
#ifndef InitializeCriticalSection
#	define InitializeCriticalSection(a) pthread_mutex_init(a, NULL);
#endif
#ifndef DeleteCriticalSection
#	define DeleteCriticalSection(a) pthread_mutex_destroy(a);
#endif
	void strupr(char *s)
	{ int L=strlen(s); for (int i=0; i<L; i++) s[i]=toupper(s[i]);
	}
	void strlwr(char *s)
	{ int L=strlen(s); for (int i=0; i<L; i++) s[i]=tolower(s[i]);
	}
	int GetTickCount()
	{ return time(NULL)*1000;
	}
	#define WINAPI
#ifdef BSD
#       define O_LARGEFILE 0
#endif //BSD
#endif

#	include <fcntl.h>
#	include <errno.h>

# 	ifndef _LK_NBLCK
#   		define _LK_NBLCK LK_NBLCK
#   		define _LK_UNLCK LK_UNLCK
# 	endif
#	if !defined(_errno)
# 		define _errno errno
#	endif

#	define open_retry_count 4
#	define open_retry_delay 50
#	define lock_retry_count 4
#  	define lock_retry_delay 50
#	define file_lock_error   -1
#	define file_access_error  5

#include "gpps.hpp" //DUCKHEAD92
#include "dgate.hpp"

int	NeedPack = TRUE;		// controls pack of dbase files on startup
static  int OdbcDoubleBackSlashToDB=0;
static  int OdbcUseEscapeStringConstants=0;
static  int PackDBF=0;
static  int IndexDBF=0;
static  int LongQueryDBF=1000;
int 	NumIndexing=-1;					// is >0 during in-memory indexing

extern BOOL NoThread;					// debug non-threaded

static short *Indices[20] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
static int IndicesCount[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// end for built-in dbase support

#	include	"odbci.hpp"
#	include	"dicom.hpp"
#	include	"dprintf.hpp"

typedef	unsigned char	BYTE;
typedef	unsigned int	UINT;

#ifndef DB_DEBUG
#	define	DB_DEBUG
#endif
#	define	QUERY	1
#	define	INSERT	2

#if defined(__BORLANDC__) || defined(__WATCOMC__)
typedef SQLRETURN  (SQL_API *_SQLAllocConnect)(SQLHENV EnvironmentHandle,
           SQLHDBC *ConnectionHandle);

typedef SQLRETURN  (SQL_API *_SQLAllocEnv)(SQLHENV *EnvironmentHandle);

typedef SQLRETURN  (SQL_API *_SQLAllocStmt)(SQLHDBC ConnectionHandle,
           SQLHSTMT *StatementHandle);

typedef SQLRETURN  (SQL_API *_SQLConnect)(SQLHDBC ConnectionHandle,
           SQLCHAR *ServerName, SQLSMALLINT NameLength1,
           SQLCHAR *UserName, SQLSMALLINT NameLength2,
           SQLCHAR *Authentication, SQLSMALLINT NameLength3);

typedef SQLRETURN  (SQL_API *_SQLDisconnect)(SQLHDBC ConnectionHandle);

typedef SQLRETURN  (SQL_API *_SQLFreeConnect)(SQLHDBC ConnectionHandle);

typedef SQLRETURN  (SQL_API *_SQLFreeEnv)(SQLHENV EnvironmentHandle);

typedef SQLRETURN  (SQL_API *_SQLFreeStmt)(SQLHSTMT StatementHandle,
           SQLUSMALLINT Option);

typedef SQLRETURN  (SQL_API *_SQLTables)(SQLHSTMT StatementHandle,
           SQLCHAR *CatalogName, SQLSMALLINT NameLength1,
           SQLCHAR *SchemaName, SQLSMALLINT NameLength2,
           SQLCHAR *TableName, SQLSMALLINT NameLength3,
           SQLCHAR *TableType, SQLSMALLINT NameLength4);

typedef SQLRETURN  (SQL_API *_SQLBindCol)(SQLHSTMT StatementHandle, 
		   SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, 
		   SQLPOINTER TargetValue, SQLINTEGER BufferLength, 
	   	   SQLINTEGER *StrLen_or_Ind);

typedef SQLRETURN  (SQL_API *_SQLFetch)(SQLHSTMT StatementHandle);

typedef SQLRETURN  (SQL_API *_SQLError)(SQLHENV EnvironmentHandle,
           SQLHDBC ConnectionHandle, SQLHSTMT StatementHandle,
           SQLCHAR *Sqlstate, SQLINTEGER *NativeError,
           SQLCHAR *MessageText, SQLSMALLINT BufferLength,
           SQLSMALLINT *TextLength);

typedef SQLRETURN  (SQL_API *_SQLExecDirect)(SQLHSTMT StatementHandle,
           SQLCHAR *StatementText, SQLINTEGER TextLength);

_SQLAllocConnect __SQLAllocConnect;
_SQLAllocEnv __SQLAllocEnv;
_SQLAllocStmt __SQLAllocStmt;
_SQLConnect __SQLConnect;
_SQLDisconnect __SQLDisconnect;
_SQLFreeConnect __SQLFreeConnect;
_SQLFreeEnv __SQLFreeEnv;
_SQLFreeStmt __SQLFreeStmt;
_SQLTables __SQLTables;
_SQLBindCol __SQLBindCol;
_SQLFetch __SQLFetch;
_SQLError __SQLError;
_SQLExecDirect __SQLExecDirect;
#endif

// for built-in dbase support

// code for in-memory record locking

struct LOCK_DATA
{ int  start;
  int  end;
  char filename[512];
};

CRITICAL_SECTION 	lock_critical;
int 			lock_count  = 100;
struct LOCK_DATA 	*lock_table = NULL;

// wait for lock to be passed and/or place lock in the lock table
int qlock(char *filename, int start, int len)
{ int i, lock_failure=1, end=start+len-1;

  // initialize
  if (lock_table==NULL) 
  { lock_table = (struct LOCK_DATA*)malloc(lock_count * sizeof(LOCK_DATA));
    memset(lock_table, 0, lock_count * sizeof(LOCK_DATA));
    InitializeCriticalSection(&lock_critical);
  }

  // scan all locks
  EnterCriticalSection(&lock_critical);
  for (i=0; i<lock_count; i++)
  { if (lock_table[i].start==0 && lock_table[i].end==0) 
    { lock_table[i].start=start;
      lock_table[i].end=end; 
      strcpy(lock_table[i].filename, filename);
      lock_failure=0;
      break;
    }

    if (start<lock_table[i].start && end<lock_table[i].start) continue;
    if (start>lock_table[i].end   && end>lock_table[i].end  ) continue;
    if (stricmp(lock_table[i].filename, filename)) continue;
    break;
  }

  LeaveCriticalSection(&lock_critical);
  return lock_failure;
}

// unlock earlier made entry in the lock table
int qunlock(char *filename, int start, int len)
{ int i, unlock_failure=1, end=start+len-1;//, j;

  // initialize
  if (lock_table==NULL) 
  { lock_table = (struct LOCK_DATA*)malloc(lock_count * sizeof(LOCK_DATA));
    memset(lock_table, 0, lock_count * sizeof(LOCK_DATA));
    InitializeCriticalSection(&lock_critical);
  }

  // scan all locks
  EnterCriticalSection(&lock_critical);
  for (i=0; i<lock_count; i++)
  { if (lock_table[i].start!=start) continue;
    if (lock_table[i].end  !=end  ) continue;
    if (stricmp(lock_table[i].filename, filename)) continue;
    lock_table[i].start=lock_table[i].end=0;
    unlock_failure=0;
    break;
  }
  LeaveCriticalSection(&lock_critical);
  return unlock_failure;
}

#if defined(__BORLANDC__)
#define _lseeki64 lseek
#endif
#ifdef BSD
#define lseek64 lseek
#endif //BSD

#ifdef WIN32
static void LongerSeek(int fh, int rec, int mult, int add)
{ __int64 t;
  t = (__int64)rec * (__int64)mult + (__int64)add;

  if (t>(__int64)0x7fff * (__int64)0xffff)
    _lseeki64(fh, t, SEEK_SET);
  else
    lseek(fh, t, SEEK_SET);
}
#else
static void LongerSeek(int fh, int rec, int mult, int add)
#ifdef __GNUC__ //ISO C++ does not support long long (and my long long was 32).
{ uint64_t t;
  t = (uint64_t)rec * (uint64_t)mult + (uint64_t)add;
#else
{ long long t;
  t = (long long)rec * (long long)mult + (long long)add;
#endif
  // lseek(fh, t, SEEK_SET);
  // unistd.h should map lseek to 
  lseek64(fh, t, SEEK_SET);
}
#endif


//static int qlock(int fh, __int64 start, long length)
//{ int s;
//  int l;
//  s = tell(fh);
//  if (s!=start) lseek(fh, start, SEEK_SET);
//  l = locking(fh, _LK_NBLCK, length);
//  if (s!=start) lseek(fh, s, SEEK_SET);
//  return l;
//}

//static int qunlock(int fh, __int64 start, long length)
//{ int s;
//  int l;
//  s = tell(fh);
//  if (s!=start) lseek(fh, start, SEEK_SET);
//  l = locking(fh, _LK_UNLCK, length);
//  if (s!=start) lseek(fh, s, SEEK_SET);
//  return l;
//}

void write_dbheader(struct dbase_header *th)
					/* write new header (recordcount) */
{ lseek (th->filehandle,0,SEEK_SET);    /* seek to file start */
  write(th->filehandle,&th->junk1,32);  /* write the header */
  th->currentrecord=-1;                 /* force new seek for next access */
}

void read_dbheader(struct dbase_header *th)
					/* read file header (recordcount) */
{ lseek(th->filehandle,0,SEEK_SET);     /* seek to file start */
  read(th->filehandle,&th->junk1,32);   /* and read the header */

  if (th->oldupdatecount)               /* auto-move the updatecount field */
  { th->updatecount = th->oldupdatecount; /* for DBASEIII compatibility */
    th->oldupdatecount = 0;
  }

  th->currentrecord=-1;                 /* force new seek for next access */
}

#ifndef localtime_r
#define localtime_r(a, b) memcpy(b, localtime(a), sizeof(struct tm))
#endif

void set_dbdate(struct dbase_header *th)
					/* put current date in file header */
{
  time_t time_of_day;
  struct tm tmbuf;

  time_of_day = time(NULL);
  localtime_r(&time_of_day, &tmbuf);

  th->junk1[1] = tmbuf.tm_year%100;   /* put in the header */
  th->junk1[2] = tmbuf.tm_mon+1;
  th->junk1[3] = tmbuf.tm_mday;
}


/* lock_dbheader : locks database header;
   returns : 0=OK, 1=no network, 2,3=error
*/

static unsigned int seedp = 0x55555555;

#ifndef rand_r
#define rand_r(a) rand()
#endif
					/* lock file header only */
int lock_dbheader(struct dbase_header *th)
{ int i;
					/* already locked or not shared */
  if ((th->lockcount!=0 && th->lockstart==0) || !th->sharemode) return 1;

  for (i=0; i<lock_retry_count; i++)
  { if (qlock(th->filename,0,1/*th->headerlength*/)==0) return 0;
					/* lock succeeded */

    if (i!=lock_retry_count-1) delay(rand_r(&seedp)%lock_retry_delay);
					/* wait a while and retry */
  }
  UNUSED_ARGUMENT(seedp);//For unused variable gcc warning
  return 3;                             /* failed after retries */
}

void unlock_dbheader(struct dbase_header *th)
{ if ((th->lockcount!=0 && th->lockstart==0) || !th->sharemode) return;

  qunlock(th->filename,0,1/*th->headerlength*/);
}

int dbase_lock(struct dbase_header *th, int firstrec, int reccount, int retries);

/*************** open a dbase file for read and or write ***************/

/* shared open (will also retry 10 s)

   Returns 0, returns -errorcode on failure.

   Will retry each 500 ms for 20 times if 'access denied'. If 'access denied'
   the routine will not try to use another dbase_path entry.

   mvh, 16-12-91: do NOT set handle pointer before complete success !
*/

#define ER_INARGUMENTS     1
#define ER_NOSUCHFILE      2
#define ER_NOSUCHFIELD     3
#define ER_ONCREATINGFILE  4
#define ER_ONDBASETRUNC    5
#define ER_FAULTYHANDLE    6
#define ER_SRCEQUALSDEST   7
#define ER_DBLOCKFAILED    8
#define ER_INVPACK1        9
#define ER_INVPACK2        10
#define ER_OUTOFHANDLES    11
#define ER_TOOMANYFIELDS   12
#define ER_TOOMANYFILES    13
#define ER_NOACCESS        14
#define ER_OPEN            15
#define ER_INVALIDHANDLE   16
#define ER_INVALIDRECORD   17
#define ER_HEADERREAD      18
#define ER_HEADERWRITE     19
#define ER_RECORDREAD      20
#define ER_RECORDWRITE     21
#define ER_SHAREDTRUNC     22
#define ER_SHAREDPACK      23
#define ER_SHAREDEXTRACT   24
#define ER_SHAREDSORT      25
#define ER_SORTTOOLARGE    26
#define ER_OPENOUTPUTFILE  27
#define ER_WRITETOREADONLY 28
#define ER_FILEALREADYOPEN 29
#define ER_INVALIDFORMAT   30
#define ER_OUTOFMEMORY     31
#define ER_INTERNAL        32

int dbase_open(char *filename, struct dbase_header **th)
{ int i,j,h,l,rret;
  int errcond, rdonly;
  char string[1024], blank[] = "", *p;
  int mode=1;

  strcpy(string,filename);              /* search path */
  p=blank;

  errcond = file_lock_error;            /* DBUSE dos error = -1 */

  if (mode)
    errcond = file_access_error;        /* DBSHARE dos error = 5 */

  rdonly = 0;

  while (1)
  { for (i=0; i<open_retry_count; i++)  /* retry some times */
    { if (!mode)
        h = sopen(string, O_RDWR + O_BINARY + O_LARGEFILE, SH_DENYRW, 0);
      else
	h = sopen(string, O_RDWR + O_BINARY + O_LARGEFILE, SH_DENYNO, 0);

					/* OK / other error 'no access' */
      if (h != -1 ) break;
//|| (h == -1 &&
//                      (int)_doserrno!=errcond &&
//                      (int)_doserrno!=ERROR_SHARING_VIOLATION
//                     )) break;

      if (i!=open_retry_count-1) delay(rand_r(&seedp)%open_retry_delay);
    }

    /* break path search if : found/path empty/path exhausted/access denied */

    if ( h != -1 || p==NULL || *p==0 ) break;
//||
//        (h == -1 && ((int)_doserrno==errcond ||
//                     (int)_doserrno==ERROR_SHARING_VIOLATION)))
//      break;

    strcpy(string,p);                   /* get new path entry */
    l = strcspn(p,";");
    if (string[l-1] != PATHSEPCHAR) string[l] = PATHSEPCHAR;
    string[l+1] = 0;
    strcat(string,filename);
    p += l+1;
  }

  /* try once to open read only (without path) if read/write open failed (useful for CDROM access) */

  if (h == -1)
  { strcpy(string, filename);

    if (!mode)
      h = sopen(string, O_RDONLY+ O_BINARY+ O_LARGEFILE, SH_DENYRW, 0);
    else
      h = sopen(string, O_RDONLY+ O_BINARY+ O_LARGEFILE, SH_DENYNO, 0);

    if (h != -1) rdonly = 1;
  }

  if (h == -1)
  { switch (_errno)                     /* return failure */
    { case ENOENT : return -ER_NOSUCHFILE;
      case EMFILE : return -ER_TOOMANYFILES;
      case EACCES : return -ER_NOACCESS;
#if defined(__WATCOMC__) && defined(WIN32)
      case -1: switch (_doserrno) 
               { case ERROR_SHARING_VIOLATION:
      	           return -ER_NOACCESS;
               }
               /* else fall through */
#endif /* __WATCOMC__ && WIN32 */
      default     : perror("dbase_open"); return -ER_OPEN;
    }
  }

		/* must allocate also for extra header space before junk1 */
  if (((*th)=(struct dbase_header *)malloc(sizeof(struct dbase_header)+32*MAXFIELDS+512))==NULL)
  { close (h);
    return -ER_OUTOFMEMORY;
  }

  strcpy((*th)->filename, string);

				  	   /* read and process the header */
  if (mode)
  { (*th)->filehandle   = h;               /* assume some header to lock */
    (*th)->currentrecord=-1;
    (*th)->bufrecord    =-1;
    (*th)->rawrecord    =-1;
    (*th)->recordlength = 1;
    (*th)->headerlength =32;               /* first lock 'raw' header only */
    (*th)->touched      = 0;
    (*th)->fieldcount   = 1;
    (*th)->lockstart    = 0;
    (*th)->lockcount    = 0;
    (*th)->sharemode    = 1;

    if (lock_dbheader((*th)) > 0)          /* lock header with 1 sec. retry */
      mode=0;                           /* if no network, disable locking */
  }

  rret=read(h,&(*th)->junk1,32);           /* read 'raw' header */

  if ((*th)->oldupdatecount)               /* auto-move the updatecount field */
  { (*th)->updatecount = (*th)->oldupdatecount; /* for DBASEIII compatibility */
    (*th)->oldupdatecount = 0;
  }

  if (mode) unlock_dbheader((*th));

  if (rret != 32)                       /* failed to read 'raw' header ? */
  {
    free((*th)); close(h); return -ER_HEADERREAD; 
  }

  if ((*th)->hlength>32*MAXFIELDS+32 || (*th)->hlength<64) /* DBASE IV etc has much larger header */
  { free((*th)); close(h); return -ER_INVALIDFORMAT; }

  (*th)->headerlength = (*th)->hlength;       /* read rest of the header */
  if (mode) lock_dbheader((*th));
  rret = read(h, &(*th)->fd, (*th)->hlength-32);
  if (mode) unlock_dbheader((*th));

  (*th)->recordlength=1;                   /* count fields and recordsize */
  for(j=0; j<MAXFIELDS; j++)
  { if ((*th)->fd[j].name[0] == 0x0d) break;
    (*th)->recordlength += (*th)->fd[j].len;
  }

  if (j==MAXFIELDS)                            /* too many fields --> fail */
  { free((*th)); close(h); return -ER_TOOMANYFIELDS; }

  if (rret != (*th)->hlength-32)           /* failed to read complete header ? */
  { free((*th)); close(h); return -ER_HEADERREAD; }

					/* allocate record buffers */
  (*th)->recordbuffer = (char *)malloc(2 * (*th)->recordlength + j + 2);
  (*th)->rawbuffer    = (*th)->recordbuffer + (*th)->recordlength + j + 1;
  if ((*th)->recordbuffer == NULL)
  { free((*th)); close(h); return -ER_OUTOFMEMORY; }

  (*th)->fieldcount   = j;                 /* fill in the dbase handle */
  (*th)->filehandle   = h;
  (*th)->currentrecord= 0;
  (*th)->bufrecord    = -1;
  (*th)->rawrecord    = -1;
  (*th)->headerlength = (j+1)*32+1;
  (*th)->touched      = 0;

  (*th)->lockstart = 0;                    /* file not locked at start */
  (*th)->lockcount = 0;
  (*th)->sharemode = mode;
  (*th)->readonly  = rdonly;
  (*th)->n  = 0;

  return 0;                        	/* and return OK */
}

/* Note : will lock whole file shortly in shared mode when touched.
   The routine must be careful to read fresh file length before the
   file is closed and a trailing ctrl-z is added.

   update, 15-12-91, mvh: just like write, lock only when has been touched
*/

void dbase_close(struct dbase_header *th)
{ char c;
					/* update header if file changed */
  if (th->touched)
  { if (th->sharemode)                  /* lock whole file if shared */
    { dbase_lock(th,-1,0x7fffffff,lock_retry_count);
      read_dbheader(th);                /* get latest record count */
    }

    set_dbdate(th);
    write_dbheader(th);

					/* and add trailing ctrl-z */
    LongerSeek( th->filehandle,
	        th->reccount, th->recordlength, th->headerlength
	      );
    c=0x1a; write(th->filehandle,&c,1);

    if (th->sharemode)                  /* unlock the file */
      dbase_lock(th,0,0,lock_retry_count);
  }


  if (th->lockcount)                    /* free 'hanging' locks */
    dbase_lock(th,0,0,lock_retry_count);

  close(th->filehandle);

  free(th->recordbuffer);
  free(th);
}

/*************** create a dbase file for read and or write ***************/
/*
   The **fielddata array should point to strings with the
   following structure (last pointer should be NULL) :

   NAME:Cnn            for nn (decimal) length character field
   NAME:Nnn            for nn (decimal) length numerical field
   NAME:L              logical field (one byte)
   NAME:D              date field (8 bytes in form YYYYMMDD).
   NAME may be followed by spaces.

   Returns 0, returns -errorcode on failure.

*/

int dbase_create(char *filename, char **fielddata, struct dbase_header **th)
{ int i,j,h,len;
  char name[20], *p, type;

  h = creat(filename, S_IWRITE | S_IREAD);
  if (h == -1) return -ER_ONCREATINGFILE; /* return creation failure */

  close(h);
  h = open(filename, O_RDWR | O_BINARY | O_LARGEFILE, 0);

		/* must allocate also for extra header space before junk1 */
  if (((*th)=(struct dbase_header *)malloc(sizeof(struct dbase_header)+32*MAXFIELDS+512))==NULL)
  { close(h);
    return -ER_OUTOFMEMORY;
  }

  (*th)->recordlength=1;                   /* count fields and recordsize */
  for(j=0; j<MAXFIELDS; j++)                   /* fill in the field data */
  { if (fielddata[j]==NULL) break;

    p = strchr(fielddata[j], ':');      /* split in name, type and length */
    if (p==NULL)                        /* no : found, use default type/len */
    { len  = 1;
      type = 'C';
    }
    else                                /* get type and length if necessary */
    { *p++ = 0;

      while (isspace(*p) && *p!=0) p++;
      type = toupper(*p);

      if      (type == 0)   type = 'L'; /* when no type specified, use 'L' */

      if      (type == 'L') len = 1;    /* get lenght or use default */
      else if (type == 'D') len = 8;
      else                  len = atoi(p+1);
    }

    strupr(fielddata[j]);               /* polish up the name a bit */
    sprintf(name,"%-10.10s",fielddata[j]);
    for (i=9; i>=0; i--) if (isspace(name[i])) name[i]=0;

    memcpy ((*th)->fd[j].name,name,11);    /* fill in the field data */
    memset ((*th)->fd[j].junk1,0,4);
    memset ((*th)->fd[j].junk2,0,14);

    (*th)->fd[j].type      = type;
    (*th)->fd[j].len       = len;
    (*th)->fd[j].deccount  = 0;

    (*th)->recordlength += (*th)->fd[j].len;  /* and count the total recordlength */
  }

  if (j==MAXFIELDS)                            /* too many fields --> fail */
  { free((*th));
    close(h);
    unlink(filename);
    return -ER_TOOMANYFIELDS;
  }

  (*th)->fd[j].name[0] = 0x0d;             /* place a field terminator */

  memset((*th)->junk2, 0, 20);             /* fill in the dbase handle */

  (*th)->fieldcount    =  j;
  (*th)->filehandle    =  h;
  (*th)->currentrecord =  0;
  (*th)->bufrecord     = -1;
  (*th)->rawrecord     = -1;
  (*th)->reccount      =  0;
  (*th)->updatecount   =  0;
  (*th)->oldupdatecount=  0;

  (*th)->headerlength  = (j+1)*32+1;

  (*th)->hlength       = (*th)->headerlength;
  (*th)->rlength       = (*th)->recordlength;

  (*th)->junk1[0]      = 0x03;             /* dbase III file identifier */

  (*th)->touched       = 1;

  (*th)->lockstart     = 0;                /* file not locked at start */
  (*th)->lockcount     = 0;
  (*th)->sharemode     = 0;
  (*th)->readonly      = 0;
  (*th)->n             = 0;

#if 1					/* set filename for check double opens */
  strcpy((*th)->filename, filename);
#endif

					/* allocate record buffer */
  (*th)->recordbuffer= (char *)malloc(2 * (*th)->recordlength + (*th)->fieldcount + 2);
  (*th)->rawbuffer   = (*th)->recordbuffer + (*th)->recordlength + (*th)->fieldcount + 1;
  if ((*th)->recordbuffer == NULL)
  { free((*th));
    close(h);
    unlink(filename);
    return -ER_OUTOFMEMORY;
  }

  set_dbdate((*th));

					/* write the fresh header */
  i = write((*th)->filehandle,&(*th)->junk1,(*th)->headerlength);

  if (i !=  (*th)->headerlength)           /* write failed ? */
  { free((*th)->recordbuffer);
    free((*th));
    close(h);
    unlink(filename);
    return -ER_HEADERWRITE;
  }

  lseek(h, (*th)->headerlength, SEEK_SET); /* rewind file to record 0 */

  dbase_close((*th));
  dbase_open(filename, th);

  return 0;                        	/* and return ok */
}

/************************ record locking operator **************************/

/* Parameters : handle   = dbase handle of file to lock
		firstrec = first record to lock, use -1 to lock header also
		reccount = number of records to lock (use 0 to unlock file)
		retries  = number of 100 ms retries before failure

   Returns    : 0 : locked O.K.
		1 : no network/multitasking locking function available
		2 : error
		3 : lock failed after some retries

   Note       : Locking a new file part automatically frees previous lock.
*/


int dbase_lock(struct dbase_header *th, int firstrec, int reccount, int retries)
{ int i,j;
  int locks;
  int lockc;

  /********** Check file handle *********/

  if (!th->sharemode) return 1;                 /* no locking on normal file */

  th->bufrecord = -1;                           /* invalidate buffer(s) */
  th->rawrecord = -1;

  /******** Free previous lock *********/

  if (th->lockcount)
  { i = qunlock(th->filename, th->lockstart, th->lockcount);
    th->lockstart=0;
    th->lockcount=0;
    if (i) return 2;                            /* failed to unlock error */
  }

  /********** make a new lock *********/

  if (reccount == 0x7fffffff)                     /* use LONG_MAX to lock all */
    reccount = (0x7fffffff - th->headerlength - 256) / th->recordlength;
						/* but filesize < 32 bits */


  if (firstrec != -1)                           /* calculate lock range */
  { if (reccount==0) return 0;
    locks = firstrec+1; //(__int64) th->headerlength + (__int64) th->recordlength * (__int64)firstrec;
    lockc = 1; //(long) th->recordlength * reccount;
  }
  else
  {
    locks = 0;
    lockc = reccount+1; //(long) th->headerlength + (long) th->recordlength * reccount;
  }

  for (j=0; j<retries; j++)
  { i = qlock(th->filename,locks,lockc);

    if (i==0)                                   /* lock succeeded */
    { th->lockstart=locks;
      th->lockcount=lockc;
      return 0;
    }

    if (j!=retries-1) delay(rand_r(&seedp)%lock_retry_delay);
						/* wait a while and retry */
  }

  return 3;                                     /* failed after retries */
}

/************** query a dbase file : find matching records **************/

/* Note : records in the file may NOT be locked manually during a query.
   The routine will lock the whole file during query, and will get the
   latest record count from the file, if the file is shared.

   mode values are :
   0 : search for exact match of key.
   1 : search for "field contains key" match.
  +2 : if set, comparison is case insensitive.
  +4 : if set, query backwards (much slower).
   8 : search for "field starts with key" match.
   9 : as 8, but trailing spaces trimmed of keys first.
   16: field >= value comparison (identical string length only)
   32: field <= comparison (identical string length only)
   48: field inclusive range comparison ((identical string length only, limits must be concatenated)
   64: multiple exact value matching (a|b|c)
*/

// help routine: match data[len] against values = a|b|c
BOOL mmatch(char *data, char *values, int len, int mode)
{ char temp[256];
  char *p, *q;

  q = values;
  p = strchr(values, '|');
  
  while(p)
  { *p = 0;
    sprintf(temp,"%-*s", len, q);
    *p = '|';
    if (mode&2)
    { if (memicmp(data, temp, len)==0) return TRUE;
    }
    else
    { if (memcmp(data, temp, len)==0) return TRUE;
    }
    q = p+1;
    p = strchr(q, '|');
  }	  

  sprintf(temp,"%-*s", len, q);
  if (mode&2)
  { if (memicmp(data, temp, len)==0) return TRUE;
  }
  else
  { if (memcmp(data, temp, len)==0) return TRUE;
  }

  return FALSE;
}

int dbase_query(struct dbase_header *th, int *recno, char **fields, int *mode, int indexval)
{ int i,k,found,j,offs,offsets[MAXFIELDS],buffers,rbuflen,rlength, b;//, t;
  char *comparestrings[MAXFIELDS],*p,*q,*pbuffer;
  int record, count, returnvalue;
  int clens[MAXFIELDS], flens[MAXFIELDS], types[MAXFIELDS], fcount=0, backwards = mode[0]&4;
  int *of, *of1, cl0, fl0, of0, first=1;
  char **cs, *cs0, **cs1, *q0;
  short *index = NULL;
  int indexcount = 0;

//OperatorConsole.printf("enter query at record %d, indexval = %d\n", *recno, indexval);
// Init for warnings
  b = 0;
  pbuffer = NULL;

  if (Indices[th->n] && indexval) 
  { index = Indices[th->n];
    indexcount = IndicesCount[th->n];
  }

  for (i=0; i<MAXFIELDS; i++)
  { offsets[i]=0;
    comparestrings[i]=NULL;
    types[i]=0;
  }

  buffers=16384/(th->recordlength+1);
  if (buffers==0 || mode[0]&4 || index) buffers=1;

  q=(char *)malloc(th->recordlength*buffers);
  if (q==NULL) return -ER_OUTOFMEMORY;

  p=(char *)malloc(th->recordlength+1);
  if (p==NULL) {free(q); return -ER_OUTOFMEMORY; }

  offs=1;

  /* process the comparestrings */

  for(i=0; i<th->fieldcount; i++)
  { if (fields[i] != NULL)
    { if (strlen(fields[i])!=0)
      { k = th->fd[i].len;		/* range comparison requires double input string */
        if ((mode[i]&(64+48))==48) k *= 2;
        else if ((mode[i]&64)==64) k = strlen(fields[i]);	/* multiple match */

        comparestrings[fcount]=(char *)malloc(k + 1);

					/* truncate key at field length */
	if (strlen(fields[i]) > (unsigned int)k)
	  fields[i][k]=0;
					/* out of memory ? */
	if (comparestrings[fcount]==NULL)
	{ returnvalue = -ER_OUTOFMEMORY; goto error; }

	switch(mode[i] & (64+9))        /* processing depends on search mode */
	{ case 0:                       /* exact comparison or >= etc comparison: */
	    if (th->fd[i].type == 'C')  /* character field left justified */
	      sprintf(comparestrings[fcount],"%-*s", k, fields[i]);

	    else                         /* other fields right justified */
	      sprintf(comparestrings[fcount],"%*s", k, fields[i]);

            types[fcount] = mode[i]&(48+11);

	    break;

	  case 1:                        /* "includes" or "starts with" II */
	  case 9:
	    strcpy(comparestrings[fcount],fields[i]);
	    for (j=strlen(comparestrings[fcount])-1; j>=0; j--)
	      if (comparestrings[fcount][j]!=' ') break;
	    comparestrings[fcount][j+1]=0;    /* trim trailing spaces */
            types[fcount] = mode[i]&(48+11);
	    break;

	  case 8:                        /* exact key for "starts with" I */
	    strcpy(comparestrings[fcount],fields[i]);
            types[fcount] = mode[i]&(48+11);

	  default:			// multiple exact value matching
	    strcpy(comparestrings[fcount],fields[i]);
            types[fcount] = mode[i]&(64+2);
	}

	if (mode[i] & 2)                   /* mode & 2 --> non case sensitive */
	  strlwr(comparestrings[fcount]);

	offsets[fcount] = offs;
	clens  [fcount] = strlen(comparestrings[fcount]);
	flens  [fcount] = th->fd[i].len;
	fcount++;
      }
    }
    offs += th->fd[i].len;
  }

  if (th->sharemode)                    /* get latest record count */
  { lock_dbheader  (th);
    read_dbheader  (th);
    unlock_dbheader(th);
  }

  if (th->currentrecord != *recno)
  { LongerSeek( th->filehandle,                /* seek first record */
	      *recno, th->recordlength, th->headerlength
              );
    th->currentrecord = *recno;
  }

  of0     = offsets[0];
  of1     = &offsets[1];
  cl0     = clens[0];
  fl0     = flens[0];
  cs0     = comparestrings[0];
  cs1     = &comparestrings[1];

  for (k=0; k<fcount; k++)
    offsets[k] -= of0;

  count   = th->reccount;

  k       = buffers;
  rlength = th->recordlength;
  rbuflen = rlength * buffers;
  found   = 0;
  record  = *recno;
  q0      = q + of0;

  if (fcount==0) types[0] = -1;             /* no fields to compare */

  /* scan records for match */

  if (record >= 0 && record < count)
  { while (1)
    { if (index)
      { /* skip records with wrong index */
        while (record<indexcount && record<count && index[record]!=(short)indexval)
          record++;

        if (th->currentrecord != record)
        { LongerSeek( th->filehandle,                /* seek first record */
	             record, th->recordlength, th->headerlength
                     );
          th->currentrecord = record;
        }

        k     = buffers;
        first = 1;
      }

      if (++k >= buffers)               /* read new buffer full of records */
      { if (backwards)
	{ if (!first) record--; else first=0;
	  if (record < 0) break;
	  LongerSeek ( th->filehandle,
		       record, th->recordlength, th->headerlength
	             );
          th->currentrecord = record;
	}
        else
	{ if (!first) record += buffers; else first=0;
	  if (count - record < buffers)
	  { buffers = count - record;
	    if (buffers <= 0) break;    /* out of record range */
	  }
	}

	if ( dbase_lock(th,record,buffers,lock_retry_count) > 1) break;
	if ( read(th->filehandle, q, rbuflen)                   < 1)
	{ dbase_lock(th, 0, 0, lock_retry_count) ; /* free lock after failed read */
	  break;
	};
	if ( dbase_lock(th, 0, 0,lock_retry_count) > 1) break;
        th->currentrecord += buffers;

	pbuffer = q0;
	k = 0;
      }

      switch (types[0])       		/* all bits except 4 (backwards) */
      { case 0:                         /* exact compare (strcmp->memcmp) */
	  if (!memcmp(pbuffer, cs0, fl0))
	  { found = 1;
	    cs = cs1;
	    of = of1;
	    for (i=1; i<fcount; i++)
	    { if (types[i]==0)
              { if (memcmp(pbuffer + *of++, *cs++, flens[i]))
	        { found=0; break; }
              }
              else if (types[i]==2)
              { if (memicmp(pbuffer + *of++, *cs++, flens[i]))
	        { found=0; break; }              
              }
              else if (types[i]==8 || types[i]==9)
              { if (memcmp(pbuffer + *of++, *cs++, clens[i]))
  	        { found=0; break; }
              }
              else if (types[i]==10 || types[i]==11)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]))
  	        { found=0; break; }
              }
              else if (types[i]==16)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]) < 0)
  	        { found=0; break; }
              }
              else if (types[i]==32)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]) > 0)
  	        { found=0; break; }
              }
              else if (types[i]==48)
              { int a  = (memicmp(pbuffer + *of, *cs,              clens[i]/2) < 0);
                    a |= (memicmp(pbuffer + *of, *cs + clens[i]/2, clens[i]/2) > 0);
                of++; cs++;
                if (a)
  	        { found=0; break; }
              }
              else if (types[i]==64 || types[i]==66)
              { if (!mmatch(pbuffer + *of++, *cs++, flens[i], types[i]))
  	        { found=0; break; }
              }
              else if (types[i]==1)
	      { memcpy(p, pbuffer + *of++, flens[i]);
	        p[flens[i]]=0;               /* copy: buffer needed for EXTRACT */
  	        if (strstr(p, *cs++)==NULL)
	        { found=0; break; }
	      }
              else if (types[i]==3)
	      { memcpy(p, pbuffer + *of++, flens[i]);
	        p[flens[i]]=0; strlwr(p);    /* copy: buffer needed for EXTRACT */
	        if (strstr(p, *cs++)==NULL)
	        { found=0; break; }
	      }
	    }
	  }
	  break;

	case 2:                         /* idem, case insensitive */
	  if (!memicmp(pbuffer, cs0, fl0))
	  { found = 1;
	    cs = cs1;
	    of = of1;
	    for (i=1; i<fcount; i++)
	    { if (types[i]==0)
              { if (memcmp(pbuffer + *of++, *cs++, flens[i]))
	        { found=0; break; }
              }
              else if (types[i]==2)
              { if (memicmp(pbuffer + *of++, *cs++, flens[i]))
	        { found=0; break; }              
              }
              else if (types[i]==8 || types[i]==9)
              { if (memcmp(pbuffer + *of++, *cs++, clens[i]))
  	        { found=0; break; }
              }
              else if (types[i]==10 || types[i]==11)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]))
  	        { found=0; break; }
              }
              else if (types[i]==16)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]) < 0)
  	        { found=0; break; }
              }
              else if (types[i]==32)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]) > 0)
  	        { found=0; break; }
              }
              else if (types[i]==48)
              { int a  = (memicmp(pbuffer + *of, *cs,              clens[i]/2) < 0);
                    a |= (memicmp(pbuffer + *of, *cs + clens[i]/2, clens[i]/2) > 0);
                of++; cs++;
                if (a)
  	        { found=0; break; }
              }
              else if (types[i]==64 || types[i]==66)
              { if (!mmatch(pbuffer + *of++, *cs++, flens[i], types[i]))
  	        { found=0; break; }
              }
              else if (types[i]==1)
	      { memcpy(p, pbuffer + *of++, flens[i]);
	        p[flens[i]]=0;               /* copy: buffer needed for EXTRACT */
  	        if (strstr(p, *cs++)==NULL)
	        { found=0; break; }
	      }
              else if (types[i]==3)
	      { memcpy(p, pbuffer + *of++, flens[i]);
	        p[flens[i]]=0; strlwr(p);    /* copy: buffer needed for EXTRACT */
	        if (strstr(p, *cs++)==NULL)
	        { found=0; break; }
	      }
	    }
	  }
	  break;

	case 1:                         /* "includes" compare */
	  memcpy(p, pbuffer, fl0);
	  p[fl0]=0;                     /* copy: buffer needed for EXTRACT */
	  if (strstr(p, cs0))
	  { found = 1;
	    cs = cs1;
	    of = of1;
	    for (i=1; i<fcount; i++)
	    { if (types[i]==0)
              { if (memcmp(pbuffer + *of++, *cs++, flens[i]))
	        { found=0; break; }
              }
              else if (types[i]==2)
              { if (memicmp(pbuffer + *of++, *cs++, flens[i]))
	        { found=0; break; }              
              }
              else if (types[i]==8 || types[i]==9)
              { if (memcmp(pbuffer + *of++, *cs++, clens[i]))
  	        { found=0; break; }
              }
              else if (types[i]==10 || types[i]==11)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]))
  	        { found=0; break; }
              }
              else if (types[i]==16)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]) < 0)
  	        { found=0; break; }
              }
              else if (types[i]==32)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]) > 0)
  	        { found=0; break; }
              }
              else if (types[i]==48)
              { int a  = (memicmp(pbuffer + *of, *cs,              clens[i]/2) < 0);
                    a |= (memicmp(pbuffer + *of, *cs + clens[i]/2, clens[i]/2) > 0);
                of++; cs++;
                if (a)
  	        { found=0; break; }
              }
              else if (types[i]==64 || types[i]==66)
              { if (!mmatch(pbuffer + *of++, *cs++, flens[i], types[i]))
  	        { found=0; break; }
              }
              else if (types[i]==1)
	      { memcpy(p, pbuffer + *of++, flens[i]);
	        p[flens[i]]=0;               /* copy: buffer needed for EXTRACT */
  	        if (strstr(p, *cs++)==NULL)
	        { found=0; break; }
	      }
              else if (types[i]==3)
	      { memcpy(p, pbuffer + *of++, flens[i]);
	        p[flens[i]]=0; strlwr(p);    /* copy: buffer needed for EXTRACT */
	        if (strstr(p, *cs++)==NULL)
	        { found=0; break; }
	      }
	    }
	  }
	  break;

	case 3:                         /* idem, case insensitive */
	  memcpy(p, pbuffer, fl0);
	  p[fl0]=0; strlwr(p);          /* copy: buffer needed for EXTRACT */
	  if (strstr(p, cs0))
	  { found = 1;
	    cs = cs1;
	    of = of1;
	    for (i=1; i<fcount; i++)
	    { if (types[i]==0)
              { if (memcmp(pbuffer + *of++, *cs++, flens[i]))
	        { found=0; break; }
              }
              else if (types[i]==2)
              { if (memicmp(pbuffer + *of++, *cs++, flens[i]))
	        { found=0; break; }              
              }
              else if (types[i]==8 || types[i]==9)
              { if (memcmp(pbuffer + *of++, *cs++, clens[i]))
  	        { found=0; break; }
              }
              else if (types[i]==10 || types[i]==11)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]))
  	        { found=0; break; }
              }
              else if (types[i]==16)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]) < 0)
  	        { found=0; break; }
              }
              else if (types[i]==32)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]) > 0)
  	        { found=0; break; }
              }
              else if (types[i]==48)
              { int a  = (memicmp(pbuffer + *of, *cs,              clens[i]/2) < 0);
                    a |= (memicmp(pbuffer + *of, *cs + clens[i]/2, clens[i]/2) > 0);
                of++; cs++;
                if (a)
  	        { found=0; break; }
              }
              else if (types[i]==64 || types[i]==66)
              { if (!mmatch(pbuffer + *of++, *cs++, flens[i], types[i]))
  	        { found=0; break; }
              }
              else if (types[i]==1)
	      { memcpy(p, pbuffer + *of++, flens[i]);
	        p[flens[i]]=0;               /* copy: buffer needed for EXTRACT */
  	        if (strstr(p, *cs++)==NULL)
	        { found=0; break; }
	      }
              else if (types[i]==3)
	      { memcpy(p, pbuffer + *of++, flens[i]);
	        p[flens[i]]=0; strlwr(p);    /* copy: buffer needed for EXTRACT */
	        if (strstr(p, *cs++)==NULL)
	        { found=0; break; }
	      }
	    }
	  }
	  break;

	case 8:                         /* "starts with" compare I and II */
	case 9:
	  if (!memcmp(pbuffer, cs0, cl0))
	  { found = 1;
	    cs = cs1;
	    of = of1;
	    for (i=1; i<fcount; i++)
	    { if (types[i]==0)
              { if (memcmp(pbuffer + *of++, *cs++, flens[i]))
	        { found=0; break; }
              }
              else if (types[i]==2)
              { if (memicmp(pbuffer + *of++, *cs++, flens[i]))
	        { found=0; break; }              
              }
              else if (types[i]==8 || types[i]==9)
              { if (memcmp(pbuffer + *of++, *cs++, clens[i]))
  	        { found=0; break; }
              }
              else if (types[i]==10 || types[i]==11)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]))
  	        { found=0; break; }
              }
              else if (types[i]==16)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]) < 0)
  	        { found=0; break; }
              }
              else if (types[i]==32)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]) > 0)
  	        { found=0; break; }
              }
              else if (types[i]==48)
              { int a  = (memicmp(pbuffer + *of, *cs,              clens[i]/2) < 0);
                    a |= (memicmp(pbuffer + *of, *cs + clens[i]/2, clens[i]/2) > 0);
                of++; cs++;
                if (a)
  	        { found=0; break; }
              }
              else if (types[i]==64 || types[i]==66)
              { if (!mmatch(pbuffer + *of++, *cs++, flens[i], types[i]))
  	        { found=0; break; }
              }
              else if (types[i]==1)
	      { memcpy(p, pbuffer + *of++, flens[i]);
	        p[flens[i]]=0;               /* copy: buffer needed for EXTRACT */
  	        if (strstr(p, *cs++)==NULL)
	        { found=0; break; }
	      }
              else if (types[i]==3)
	      { memcpy(p, pbuffer + *of++, flens[i]);
	        p[flens[i]]=0; strlwr(p);    /* copy: buffer needed for EXTRACT */
	        if (strstr(p, *cs++)==NULL)
	        { found=0; break; }
	      }
	    }
	  }
	  break;

	case 10:                        /* idem, case insensitive */
	case 11:
	  if (!memicmp(pbuffer, cs0, cl0))
	  { found = 1;
	    cs = cs1;
	    of = of1;
	    for (i=1; i<fcount; i++)
	    { if (types[i]==0)
              { if (memcmp(pbuffer + *of++, *cs++, flens[i]))
	        { found=0; break; }
              }
              else if (types[i]==2)
              { if (memicmp(pbuffer + *of++, *cs++, flens[i]))
	        { found=0; break; }              
              }
              else if (types[i]==8 || types[i]==9)
              { if (memcmp(pbuffer + *of++, *cs++, clens[i]))
  	        { found=0; break; }
              }
              else if (types[i]==10 || types[i]==11)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]))
  	        { found=0; break; }
              }
              else if (types[i]==16)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]) < 0)
  	        { found=0; break; }
              }
              else if (types[i]==32)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]) > 0)
  	        { found=0; break; }
              }
              else if (types[i]==48)
              { int a  = (memicmp(pbuffer + *of, *cs,              clens[i]/2) < 0);
                    a |= (memicmp(pbuffer + *of, *cs + clens[i]/2, clens[i]/2) > 0);
                of++; cs++;
                if (a)
  	        { found=0; break; }
              }
              else if (types[i]==64 || types[i]==66)
              { if (!mmatch(pbuffer + *of++, *cs++, flens[i], types[i]))
  	        { found=0; break; }
              }
              else if (types[i]==1)
	      { memcpy(p, pbuffer + *of++, flens[i]);
	        p[flens[i]]=0;               /* copy: buffer needed for EXTRACT */
  	        if (strstr(p, *cs++)==NULL)
	        { found=0; break; }
	      }
              else if (types[i]==3)
	      { memcpy(p, pbuffer + *of++, flens[i]);
	        p[flens[i]]=0; strlwr(p);    /* copy: buffer needed for EXTRACT */
	        if (strstr(p, *cs++)==NULL)
	        { found=0; break; }
	      }
	    }
	  }
	  break;

	case 16:                        /* >= comparison etc */
	case 32:
        case 48:
          if (types[0]==16) b = (memicmp(pbuffer, cs0, cl0) < 0);
          if (types[0]==32) b = (memicmp(pbuffer, cs0, cl0) > 0);
          if (types[0]==48) b = (memicmp(pbuffer, cs0,         cl0/2) < 0) 
                             || (memicmp(pbuffer, cs0 + cl0/2, cl0/2) > 0);

	  if (!b)
	  { found = 1;
	    cs = cs1;
	    of = of1;
	    for (i=1; i<fcount; i++)
	    { if (types[i]==0)
              { if (memcmp(pbuffer + *of++, *cs++, flens[i]))
	        { found=0; break; }
              }
              else if (types[i]==2)
              { if (memicmp(pbuffer + *of++, *cs++, flens[i]))
	        { found=0; break; }              
              }
              else if (types[i]==8 || types[i]==9)
              { if (memcmp(pbuffer + *of++, *cs++, clens[i]))
  	        { found=0; break; }
              }
              else if (types[i]==10 || types[i]==11)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]))
  	        { found=0; break; }
              }
              else if (types[i]==16)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]) < 0)
  	        { found=0; break; }
              }
              else if (types[i]==32)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]) > 0)
  	        { found=0; break; }
              }
              else if (types[i]==48)
              { int a  = (memicmp(pbuffer + *of, *cs,              clens[i]/2) < 0);
                    a |= (memicmp(pbuffer + *of, *cs + clens[i]/2, clens[i]/2) > 0);
                of++; cs++;
                if (a)
  	        { found=0; break; }
              }
              else if (types[i]==64 || types[i]==66)
              { if (!mmatch(pbuffer + *of++, *cs++, flens[i], types[i]))
  	        { found=0; break; }
              }
              else if (types[i]==1)
	      { memcpy(p, pbuffer + *of++, flens[i]);
	        p[flens[i]]=0;               /* copy: buffer needed for EXTRACT */
  	        if (strstr(p, *cs++)==NULL)
	        { found=0; break; }
	      }
              else if (types[i]==3)
	      { memcpy(p, pbuffer + *of++, flens[i]);
	        p[flens[i]]=0; strlwr(p);    /* copy: buffer needed for EXTRACT */
	        if (strstr(p, *cs++)==NULL)
	        { found=0; break; }
	      }
	    }
	  }
	  break;

	case 64:                        /* multiple comparison */
	case 66:
	  if (mmatch(pbuffer, cs0, fl0, types[0]))
	  { found = 1;
	    cs = cs1;
	    of = of1;
	    for (i=1; i<fcount; i++)
	    { if (types[i]==0)
              { if (memcmp(pbuffer + *of++, *cs++, flens[i]))
	        { found=0; break; }
              }
              else if (types[i]==2)
              { if (memicmp(pbuffer + *of++, *cs++, flens[i]))
	        { found=0; break; }              
              }
              else if (types[i]==8 || types[i]==9)
              { if (memcmp(pbuffer + *of++, *cs++, clens[i]))
  	        { found=0; break; }
              }
              else if (types[i]==10 || types[i]==11)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]))
  	        { found=0; break; }
              }
              else if (types[i]==16)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]) < 0)
  	        { found=0; break; }
              }
              else if (types[i]==32)
              { if (memicmp(pbuffer + *of++, *cs++, clens[i]) > 0)
  	        { found=0; break; }
              }
              else if (types[i]==48)
              { int a  = (memicmp(pbuffer + *of, *cs,              clens[i]/2) < 0);
                    a |= (memicmp(pbuffer + *of, *cs + clens[i]/2, clens[i]/2) > 0);
                of++; cs++;
                if (a)
  	        { found=0; break; }
              }
              else if (types[i]==64 || types[i]==66)
              { if (!mmatch(pbuffer + *of++, *cs++, flens[i], types[i]))
  	        { found=0; break; }
              }
              else if (types[i]==1)
	      { memcpy(p, pbuffer + *of++, flens[i]);
	        p[flens[i]]=0;               /* copy: buffer needed for EXTRACT */
  	        if (strstr(p, *cs++)==NULL)
	        { found=0; break; }
	      }
              else if (types[i]==3)
	      { memcpy(p, pbuffer + *of++, flens[i]);
	        p[flens[i]]=0; strlwr(p);    /* copy: buffer needed for EXTRACT */
	        if (strstr(p, *cs++)==NULL)
	        { found=0; break; }
	      }
	    }
	  }
	  break;

	case -1:                        /* no fields to compare */
	  found = 1;                    /* so always found */
	  break;
      }

      if (found)                        /* this record matches */
      { if (pbuffer[-of0] != '*')       /* don't process marked records */
	{ *recno = record + (int)k;
	  break;              		/* after action, compare more ? */
	}
	found = 0;
      }

      pbuffer += rlength;
      if (index) record++;
    }
  }

  if (!found)
  { *recno= -1; returnvalue = 0; }
  else
  { returnvalue = *recno; }

error :
  free(p); free(q);
  for (i=0; i<MAXFIELDS; i++) free(comparestrings[i]);

//  th->currentrecord = -1;
  return returnvalue;
}

/********* duplicate dbase file structure to new empty file ***********/

/* Note : duplicated database is NOT opened for shared access,
   so preferably store it on local disk.

   The parameter oldhandle must be a handle of database to clone.
   Return new filehandle, returns -errorcode on failure.
*/

struct dbase_header *dbase_dup(char *filename, struct dbase_header *th)
{ int file,wret;//, i;
  struct dbase_header *newth;

  file = creat(filename, S_IWRITE | S_IREAD); /* try and create the file */
  if (file == -1) return NULL;

  close(file);
  file = open(filename, O_RDWR | O_BINARY | O_LARGEFILE, 0);

					/* allocate filehandle storage */
  if ((newth=(struct dbase_header *)malloc(sizeof(struct dbase_header)+32*MAXFIELDS+512))==NULL)
  { close(file);
    return NULL;
  }

  memcpy(newth,                         /* duplicate the old handle */
	 th,
	 sizeof(struct dbase_header)+32*MAXFIELDS+512
	);

  newth->filehandle    =  file;            /* fill in some fields */
  newth->currentrecord =  0;
  newth->bufrecord     = -1;
  newth->rawrecord     = -1;
  newth->reccount      =  0;
  newth->touched       =  1;

  newth->lockstart     = 0;                /* don't duplicate locks */
  newth->lockcount     = 0;
  newth->sharemode     = 0;
  newth->readonly      = 0;

  strcpy(newth->filename, filename);

  newth->updatecount   = 0;
  newth->oldupdatecount= 0;
  set_dbdate(newth);                       /* fill in the date */

					/* allocate record buffer */
  newth->recordbuffer= (char *)malloc(2 * newth->recordlength + newth->fieldcount + 2);
  newth->rawbuffer   = newth->recordbuffer + newth->recordlength + newth->fieldcount + 1;
  if (newth->recordbuffer == NULL)
  { free(newth);
    close(file);
    unlink(filename);
    return NULL;
  }

					/* and write the fresh header */
  wret = write(file,&newth->junk1,newth->headerlength);

  if (wret != newth->headerlength)         /* check for errors */
  { free(newth->recordbuffer);
    free(newth);
    close(file);
    unlink(filename);
    return NULL;
  }

  return newth;
}

/******************** read a record from a dbase file ******************/

/* Note : will lock record during read, UNLESS manual lock is active.
   In the latter case, you are out on your own ! Use manual locking
   only to lock a quick read/write transfer or when appending to dbase.

   Single record buffering is now applied for exclusively opened
   databases or when a manual file lock is active.

   The pointer array fields[] is filled with (fieldcount) pointers to the
   newly read data. These pointers should not be freed anymore.

   Returns -errorcode on failure, 0 on success, 1 when record is marked.
*/

int dbase_read(dbase_header *th, int recno, char** fields)
{ int i,j,offs,rret,newdata=1;
  char *p;

  if (recno >= th->reccount || recno < 0) return -ER_INVALIDRECORD;

  p = th->rawbuffer;                    /* use rawbuffer as workspace */

  if (th->currentrecord != recno)       /* seek the requested entry */
  { LongerSeek ( th->filehandle,
		 recno, th->recordlength, th->headerlength
	       );
    th->currentrecord = recno;
  }

  if (th->sharemode && !th->lockcount)
  { dbase_lock(th,recno,1,lock_retry_count);        /* lock if needed */
    read(th->filehandle,p,th->recordlength);            /*    and read it  */
    dbase_lock(th,0,0,lock_retry_count);            /* unlock if locked */
    th->currentrecord++;
    th->bufrecord = -1;                                 /* invalidate buf. */
    th->rawrecord = -1;
  }
  else
  { if (recno != th->rawrecord)                 /* not in buffer so read */
    { rret = read(th->filehandle,p,th->recordlength);

      if (rret != th->recordlength && newdata)  /* check for error */
	return -ER_RECORDREAD;

      th->currentrecord++;
      th->rawrecord = recno;
    }

    if (recno == th->bufrecord) newdata = 0;
    else                        th->bufrecord = recno;
  }

  offs=1;                               /* fill in the fields */
  for(i=0; i<th->fieldcount; i++)
  { fields[i] = th->recordbuffer + offs + i;
    if (newdata)                        /* process new data */
    { if (th->fd[i].type == 'C')        /* trim trailing of character field */
      { for (j=th->fd[i].len-1; j>=0; j--)
	  if (p[offs+j]!=' ') break;
	j++;
	memcpy(fields[i],p+offs,j);
	fields[i][j]=0;
      }
      else                              /* trim leading spaces of any other */
      { for (j=0; j < th->fd[i].len; j++)
	  if (p[offs+j]!=' ') break;
	if (j < th->fd[i].len) memcpy(fields[i],p+offs+j,th->fd[i].len - j);
	fields[i][th->fd[i].len - j]=0;
      }
    }
    offs += th->fd[i].len;
  }

  if (newdata)
    th->recordbuffer[0] = *p;           /* buffer the record mark ! */

  if (th->recordbuffer[0] == '*') i=1;
  else                            i=0;  /* record marked ? */

  return i;
}

/****************** write a record to a dbase file *******************/

/* Note : will lock record during update, UNLESS manual lock is active.
   In the latter case, you are out in your own ! Use manual locking
   only to lock a quick read/write transfer or for appending to dbase.

   It is allowed to append records to the file. In that case it is advised
   to lock the WHOLE file manually before you get the recordcount that is
   used as recordnumber for the append. If you don't do that, the routine
   will still work. It can't assure, however, that the recordcount hasn't
   changed inbetween you asking it, and the actual write.

   The pointers in fields[] must point to valid strings or be NULL.

   Returns -errorcode on failure, 0 on success.

   mvh, 15-12-91: modification: writes only when data will be changed.
*/

int dbase_write(dbase_header *th, int recno, char** fields, int mark)
{ int i,j,offs,rwret,locked=0,modified=0,goread=1;
  char *p, *save;

					/* attempt to write illegal record */
  if (recno > th->reccount || recno < 0 ) return -ER_INVALIDRECORD;

  if (th->readonly) return -ER_WRITETOREADONLY;

  p = th->rawbuffer;                    /* use rawbuffer as workspace */

  save=(char *)malloc(th->recordlength+1);      /* allocate more workspace */
  if (save==NULL) return -ER_OUTOFMEMORY;

  if (th->sharemode)                    /* shared : get latest record count */
  { lock_dbheader(th);
    read_dbheader(th);

    if (th->lockcount == 0)             /*    and lock the record to update */
    { dbase_lock(th, recno, 1, lock_retry_count);
      locked=1;
    }
  }

  if (recno == th->reccount)            /* append a record so : */
  { memset(p,' ',th->recordlength);     /*     blank the buffer */
    goread   = 0;
    modified = 1;
  }

  if (th->currentrecord != recno)       /* seek the requested entry */
  { LongerSeek ( th->filehandle,
		 recno, th->recordlength, th->headerlength
	       );
    th->currentrecord = recno;
  }

  if (th->sharemode && !th->lockcount)
  { th->bufrecord = -1;
    th->rawrecord = -1;
  }
  else
    if (recno == th->rawrecord) goread = 0;

  th->rawrecord = recno;

  if (goread)
  { rwret=read(th->filehandle,p,th->recordlength);
    th->currentrecord++;

    if (rwret != th->recordlength)      /* check for errors */
    { free(save);

      if (locked)                       /* unlock if needed */
	dbase_lock(th,0,0,lock_retry_count);

      unlock_dbheader(th);
      return -ER_RECORDREAD;
    }
  }

  if (!modified)
    memcpy(save, p, th->recordlength);  /* save data for later compare */

  if (mark) *p = '*'; else *p = ' ';    /* mark as deleted / not deleted */

  if (!modified)                        /* change in mark is modification ! */
    modified = (*p != save[0]);

  offs=1;                               /* fill in the fields */
  for(i=0; i<th->fieldcount; i++)
  { if (fields[i]!=NULL)                /* field contains data */
    { j=strlen(fields[i]);
      if (th->fd[i].type != 'N')        /* left justify non-numerical field */
      { strncpy(p+offs,fields[i],th->fd[i].len);
	if (j < th->fd[i].len) memset(p+offs+j,' ',th->fd[i].len-j);
      }
      else                              /* right justify numeric field */
      { if (j<th->fd[i].len)
	{ memset(p+offs,' ',th->fd[i].len-j);
	  memcpy(p+offs+th->fd[i].len-j, fields[i], j);
	}
	else
	  memcpy(p+offs, fields[i], th->fd[i].len);
      }
      if (!modified)                    /* test if write will modify data */
	modified = memcmp(save+offs, p+offs, th->fd[i].len) != 0;
    }
    offs += th->fd[i].len;
  }

  if (modified)                         /* write modified data only */
  { if (th->currentrecord != recno)     /* seek the requested entry */
    { LongerSeek( th->filehandle,          /* seek record */
 	          recno, th->recordlength, th->headerlength
                );
      th->currentrecord = recno;
    }

					/* invalidate buffer when written */
    if (th->bufrecord == recno) th->bufrecord = -1;

					/* and write the updated record */
    rwret=write(th->filehandle,p,th->recordlength);
    th->currentrecord++;

    if (rwret != th->recordlength)      /* check for errors */
    { free(save);

      if (locked)                       /* unlock if needed */
	dbase_lock(th,0,0,lock_retry_count);

      unlock_dbheader(th);
      return -ER_RECORDWRITE;
    }
  }

  if (th->reccount==recno)              /* appended so :         */
    th->reccount++;                     /* increase record count */

  th->updatecount++;                    /* suppressed write is an update ! */
  th->touched=1;                        /* register that file has changed */

  if (th->sharemode)                    /* and update header if shared */
  { set_dbdate(th);
    write_dbheader(th);
    unlock_dbheader(th);
  }

  if (locked)                           /* unlock if needed */
    dbase_lock(th,0,0,lock_retry_count);

  free(save);

  return 0;
}

/************ dbase_trunc : truncate database to certain length *********/

/* Truncates a database to a length of reccount records.
   Returns -errorcode on failure, 0 on success.

   NOTE: FAILS FOR TRUNC AT >2GB
*/

int dbase_trunc(dbase_header *th, int reccount)
{ unsigned file_end;
  char eof_marker=0x1a;
  int r;

  if (th->readonly) return -ER_WRITETOREADONLY;

#ifdef WIN32
  if ((__int64)reccount * (__int64)th->recordlength > (__int64)65536 * (__int64)32767)
#else
#ifdef __GNUC__ //ISO C++ does not support long long (and my long long was 32).
  if ((uint64_t)reccount * (uint64_t)th->recordlength > (uint64_t)65536 * (uint64_t)32767)
#else
  if ((long long)reccount * (long long)th->recordlength > (long long)65536 * (long long)32767)
#endif
#endif
    return ER_ONDBASETRUNC;

  dbase_lock(th,-1,0x7fffffff,lock_retry_count);

  file_end=(long)th->recordlength * reccount + (long) th->headerlength;

  lseek(th->filehandle,file_end,SEEK_SET);

  write(th->filehandle,&eof_marker,1);  /* write EOF marker */
#ifdef WIN32  
  r=chsize(th->filehandle,file_end+1);  /* and truncate the file */
#else
  r=ftruncate(th->filehandle,file_end+1);  /* and truncate the file */
#endif

  if (r == -1) return -ER_ONDBASETRUNC; /* error exit */

  th->reccount      = reccount;         /* log the changes in handle */
  th->currentrecord = -1;
  th->bufrecord     = -1;
  th->rawrecord     = -1;
  th->touched       = 1;
  th->updatecount++;

  write_dbheader(th);			/* write the header */
  dbase_lock(th,0,0,lock_retry_count);

  return 0;
}

/*****************  pack a dbase file : remove marked records *************/

/* Returns -errorcode on failure, 0 on success.
   Notes : 1) should only be used single-user; 2) invalidates the patient ID index.
*/

int dbase_pack(struct dbase_header *th)
{ int k,buffers,bytesread,wbuffer;
  int readrec,writerec;
  char *buf,*p,*q;
	
// Init for warnings
  wbuffer = 0;
  p = q = NULL;

  if (th->reccount==0) return 0;        /* no need to pack empty file */
  if (th->readonly) return -ER_WRITETOREADONLY;

  dbase_trunc(th, th->reccount);     /* invalidates buffer(s) */

  dbase_lock(th,-1,0x7fffffff,lock_retry_count);

  buffers=65536/(th->recordlength+1);    /* allocate 64 k buffer space */
  if (buffers==0) buffers=1;

  buf=(char *) malloc(th->recordlength*buffers);
  if (buf==NULL) return -ER_OUTOFMEMORY;

  k=0; readrec=0; writerec=0;

  do
  { if (k==0)                           /* if no more records : read buffer */
    { p=buf; q=buf; wbuffer=0;
      LongerSeek ( th->filehandle,
		   readrec, th->recordlength, th->headerlength
	         );
      bytesread = read(th->filehandle,buf,th->recordlength*buffers);
      if (bytesread==0 || bytesread==-1) break;
    }

    if (*p == ' ')                      /* process records : copy non-marked */
    { if (p != q) memcpy(q,p,th->recordlength);
      wbuffer++;
      p += th->recordlength;
      q += th->recordlength;
    }
    else if (*p == '*')                 /* skip marked records */
    { p += th->recordlength;
    }
    else if (*p == 0x1a) break;         /* break at end-of-file */

    k++; if (k >= buffers)              /* all 8 k processed -> write */
    { k=0;
      if (writerec!=readrec || p!=q)    /* write if something changed */
      { LongerSeek ( th->filehandle,
		     writerec, th->recordlength, th->headerlength
	           );
	if(wbuffer) write(th->filehandle,buf,th->recordlength*wbuffer);
					/* if.. added, 17/11/91, mvh */
      }
      writerec+=wbuffer;
      readrec+=buffers;
    }
  } while (1);

					/* flush buffer at end of file */
  if (wbuffer != 0 && (p!=q || writerec!=readrec))
  { LongerSeek ( th->filehandle,
		 writerec, th->recordlength, th->headerlength
	       );
    if (wbuffer) write(th->filehandle,buf,th->recordlength*wbuffer);
					/* if.. added, 17/11/91, mvh */
  }
  writerec+=wbuffer;

  dbase_trunc(th, writerec);           /* truncate database file */
  free(buf);

  dbase_lock(th,0,0,lock_retry_count);
  return 0;
}

// filters SQL escape characters from dbase_query string

void process_escape(char *in)
{ signed char *p=(signed char *)in, *q=(signed char *)in;
  while (*p)
  { switch (*p)
    { case '\\': p++; if (*p!='%' && *p!='_' && *p!='[' && (*p!='\\' || !OdbcDoubleBackSlashToDB))
                        *q++='\\'; break;			// \% \_ \[    -> % _ [
								// \\          -> \ is optional
      // case '[':  p++; *q++ = *p++; p++; break;			// [x] -> x
      case '\'': p++; if (*p=='\'') *q++='\'', p++;  	break;	// ''	       -> '
      case -1  : *q++ = ' ', p++;			break;	// 20061213: non breaking space used internally by query parser as space
      default  : *q++ = *p++;
    }
  }
  *q = 0;
}

// filters SQL escape characters from dbase_write string

void process_escapeValue(char *in)
{ char *p=in, *q=in;
  while (*p)
  { switch (*p)
    { case '\\': p++; if (*p!='\\' || !OdbcDoubleBackSlashToDB)
                        *q++='\\'; break;			// \\          -> \ is optional
      case '\'': p++; if (*p=='\'') *q++='\'', p++;  	break;	// ''	       -> '
      default  : *q++ = *p++;
    }
  }
  *q = 0;
}

// code for hashed in-memory index on patient ID

struct HASH_DATA
{ int hash;
  int db;
  int record;
  char ID[68];
};

char 			hash_fields[4][11] = {"PATIENTID", "STUDYPAT", "IMAGEPAT", "SERIESPAT"};
CRITICAL_SECTION 	hash_critical;
int 			hash_count  = 12345;
struct HASH_DATA 	*hash_table = NULL;

// number of entries to attempt on hash collision
#define TRY_FURTHER 5

// compute hash function
int hash_id(char *ID, int db)
{ int i, v=db * 7654321, L=strlen(ID);
  for (i=0; i<L; i++) 
    if (ID[i]>='a' && ID[i]<='z') v = v*13 + ID[i] - 32;
    else                          v = v*13 + ID[i];
  if (v==0) v=1;
  return v;
}

// search an entry in the hash index; returns first record at with ID is used
int search_hash(char *ID, int db)
{ int i, h, n;

  // initialize
  if (hash_table==NULL) 
  { hash_table = (struct HASH_DATA*)malloc(hash_count * sizeof(HASH_DATA));
    memset(hash_table, 0, hash_count * sizeof(HASH_DATA));
    InitializeCriticalSection(&hash_critical);
  }

  h = hash_id(ID, db);
  n = abs(h % hash_count);

  EnterCriticalSection(&hash_critical);

  // search exact match
  if (hash_table[n].hash==h && hash_table[n].db == db)
    if (strcmp(ID, hash_table[n].ID)==0)
    { LeaveCriticalSection(&hash_critical);
      return hash_table[n].record;
    }

  // search some further; may have been placed there
  for (i=0; i<TRY_FURTHER; i++)
  { n++;
    if (n>hash_count) n=0;

    if (hash_table[n].hash==h && hash_table[n].db == db)
      if (strcmp(ID, hash_table[n].ID)==0)
      { LeaveCriticalSection(&hash_critical);
        return hash_table[n].record;
      }
  }

  // not found
  LeaveCriticalSection(&hash_critical);
  return 0;
}
  
// enter the first record for a patient ID into the hash index
void enter_hash(char *ID, int db, int record)
{ int i, h, n, o;

  h = hash_id(ID, db);
  n = o = abs(h % hash_count);

  EnterCriticalSection(&hash_critical);

  // search exact match
  if (hash_table[n].hash==0)
    if (hash_table[n].ID[0]==0)
    { hash_table[n].record= record;
      hash_table[n].hash  = h;
      hash_table[n].db    = db;
      strcpy(hash_table[n].ID, ID);
      LeaveCriticalSection(&hash_critical);
      return;
    }

  // search some further; may be placed there
  for (i=0; i<TRY_FURTHER; i++)
  { n++;
    if (n>hash_count) n=0;

    if (hash_table[n].hash==0)
      if (hash_table[n].ID[0]==0)
      { hash_table[n].record= record;
        hash_table[n].hash  = h;
        hash_table[n].db    = db;
        strcpy(hash_table[n].ID, ID);
        LeaveCriticalSection(&hash_critical);
        return;
      }
  }

  // no place: overwrite original item
  hash_table[o].record= record;
  hash_table[o].hash  = h;
  hash_table[o].db    = db;
  strcpy(hash_table[o].ID, ID);
  LeaveCriticalSection(&hash_critical);
}

static int findhashfield(struct dbase_header *th)
{ int i, k;

  if (stricmp(th->fd[0].name, "ACCESSIONN")==0)
    return 0;

  if (th->fieldcount>1)
    if (stricmp(th->fd[1].name, "OLDUID")==0)
      return 1;
  
  for (i=0; i<th->fieldcount; i++)
    for (k=0; k<4; k++) 
      if (stricmp(th->fd[i].name, hash_fields[k])==0)
        return  i;

  return -1;
}

static BOOL WINAPI Fill_Memory_Index(struct dbase_header *th)
{ int i, j, hashfield=-1, offset, len;//, k, m;
  int t = GetTickCount();
  char *field;
  int totalrec=th->TotalRec;
  int MBextra=th->MBExtra;
  int f;

  // free old index
  if (Indices[th->n]) free(Indices[th->n]);
  Indices[th->n] = NULL;

  hashfield = findhashfield(th);
  if (hashfield<0) return TRUE;

  NumIndexing++;

  // divide MBextra free space according to current DB size or equally
  if (totalrec > 1000) i = (int)(500000.0*(float)MBextra*(float)th->reccount/(float)totalrec);
  else                 i = (int)(500000.0*(float)MBextra/5.0);
  if (i<10000)         i = 10000;

  // allocate index
  IndicesCount[th->n] = th->reccount + i;
  Indices[th->n] = (short *)malloc(IndicesCount[th->n] * sizeof(short));
  memset(Indices[th->n], 0, IndicesCount[th->n] * sizeof(short));	// rare crash: page fault about 2k before end of memset

  // field spec
  offset = th->headerlength + 1;
  for (i=0; i<hashfield; i++) offset += th->fd[i].len;
  len = th->fd[hashfield].len;

  OperatorConsole.printf("Creating index on field %d for %s with %d records\n", hashfield, th->filename, th->reccount);

  // mvh 20060402
  delay(rand_r(&seedp) % 500);

  field = (char *)malloc(th->recordlength * 64 + 1);

  f = sopen(th->filename, O_RDWR + O_BINARY + O_LARGEFILE, SH_DENYNO, 0);

  for (i=0; i<th->reccount; i++)
  { LongerSeek(f, i, th->recordlength, offset);
    read(f, field, len);
    field[len] = 0;
    for (j=len-1; j>=0; j--) if(field[j]==' ') field[j]=0; else break;	// dbaseIII pads with spaces

    Indices[th->n][i] = hash_id(field, 0);
    if ((i%100000)==99999) OperatorConsole.printf("Indexing %s: %d records processed\n", th->filename, i+1);
  }
  close(f);
  free(field);
  OperatorConsole.printf("Index for %s finished in %d ms; total space = %d records\n", th->filename, GetTickCount()-t, IndicesCount[th->n]);

  free(th);

  NumIndexing--;

  return TRUE;
}
 
void UpdateIndex(struct dbase_header *th, int rec, int hashfield)
{ char *strings[MAXFIELDS];

  if (th->n<0 || th->n>19)
  { printf("ERROR INDEX %d", th->n);
    return;
  }

  if (Indices[th->n] && rec<IndicesCount[th->n] && hashfield>=0)
  { dbase_read(th, rec, strings);
    Indices[th->n][rec] = hash_id(strings[hashfield], 0);
  }

  if (Indices[th->n] && rec>=IndicesCount[th->n] && hashfield>=0)
  { OperatorConsole.printf("***Index for %s is full - speed will degrade until server is restarted\n", th->filename);
  }
}

// end for built-in dbase support

///////////////////////////////////////////////////////////////////////////

int	DatabaseOpen=0;				// counters for db activity
int	DatabaseClose=0;
int	DatabaseQuery=0;
int	DatabaseAddRecord=0;
int	DatabaseDeleteRecord=0;
int	DatabaseNextRecord=0;
int	DatabaseCreateTable=0;
int	DatabaseUpdateRecords=0;
int	QueryTime=0;

#ifndef	RootConfig
extern	char	RootConfig[];
#endif
#ifndef	ConfigFile
extern	char	ConfigFile[];
#endif

#ifdef USEMYSQL
#ifdef WIN32
  _mysql_close mysql_close=NULL;
  _mysql_init mysql_init=NULL;
  _mysql_real_connect mysql_real_connect=NULL;
  _mysql_select_db mysql_select_db=NULL;
  _mysql_free_result mysql_free_result=NULL;
  _mysql_query mysql_query=NULL;
  _mysql_store_result mysql_store_result=NULL;
  _mysql_num_rows mysql_num_rows=NULL;
  _mysql_num_fields mysql_num_fields=NULL;
  _mysql_fetch_row mysql_fetch_row=NULL;
  _mysql_error mysql_error=NULL;
  _mysql_fetch_field_direct mysql_fetch_field_direct=NULL;
  _mysql_fetch_lengths mysql_fetch_lengths=NULL;
  _mysql_errno mysql_errno=NULL;
  void LoadMySQL(void)
  { HINSTANCE hInst;
    if (mysql_close) return;

    hInst             = LoadLibrary("libmysql.dll");
    if (!hInst) hInst = LoadLibrary("libmysql64.dll");
    if (!hInst) hInst = LoadLibrary("x64\\libmysql.dll");
    if (hInst)
    { mysql_close = (_mysql_close)GetProcAddress(hInst, "mysql_close");
      mysql_init = (_mysql_init)GetProcAddress(hInst, "mysql_init");
      mysql_real_connect = (_mysql_real_connect)GetProcAddress(hInst, "mysql_real_connect");
      mysql_select_db = (_mysql_select_db)GetProcAddress(hInst, "mysql_select_db");
      mysql_free_result = (_mysql_free_result)GetProcAddress(hInst, "mysql_free_result");
      mysql_query = (_mysql_query)GetProcAddress(hInst, "mysql_query");
      mysql_store_result = (_mysql_store_result)GetProcAddress(hInst, "mysql_store_result");
      mysql_num_rows = (_mysql_num_rows)GetProcAddress(hInst, "mysql_num_rows");
      mysql_num_fields = (_mysql_num_fields)GetProcAddress(hInst, "mysql_num_fields");
      mysql_fetch_row = (_mysql_fetch_row)GetProcAddress(hInst, "mysql_fetch_row");
      mysql_error = (_mysql_error)GetProcAddress(hInst, "mysql_error");
      mysql_fetch_field_direct = (_mysql_fetch_field_direct)GetProcAddress(hInst, "mysql_fetch_field_direct");
      mysql_fetch_lengths = (_mysql_fetch_lengths)GetProcAddress(hInst, "mysql_fetch_lengths");
      mysql_errno = (_mysql_errno)GetProcAddress(hInst, "mysql_errno");
    }
    else
    { MessageBox(NULL, "*** Fatal error: could not load libmysql.dll or libmysql64.dll to access mysql", "dgate.exe", 0);
      exit(999);
    }
  }
#endif
#endif

#ifdef POSTGRES
#ifdef WIN32
 _PQfinish               __PQfinish=NULL;            
 _PQstatus               __PQstatus=NULL;            
 _PQsetdbLogin           __PQsetdbLogin=NULL;        
 _PQclear                __PQclear=NULL;             
 _PQexec                 __PQexec=NULL;              
 _PQresultStatus         __PQresultStatus=NULL;      
 _PQntuples              __PQntuples=NULL;           
 _PQnfields              __PQnfields=NULL;           
 _PQfmod                 __PQfmod=NULL;              
 _PQgetvalue             __PQgetvalue=NULL;          
 _PQgetlength            __PQgetlength=NULL;         
 _PQresultErrorMessage   __PQresultErrorMessage=NULL;
 _PQerrorMessage         __PQerrorMessage=NULL;
 _PQresultErrorField     __PQresultErrorField=NULL;

  void LoadPostgres(void)
  { HINSTANCE hInst;
    if (PQfinish) return;

    hInst             = LoadLibrary("libpq.dll");
    if (!hInst) hInst = LoadLibrary("libpq64.dll");
    if (!hInst) hInst = LoadLibrary("x64\\libpq.dll");
    if (hInst)
    { PQfinish = (_PQfinish)GetProcAddress(hInst, "PQfinish");
      PQstatus = (_PQstatus)GetProcAddress(hInst, "PQstatus");
      PQsetdbLogin = (_PQsetdbLogin)GetProcAddress(hInst, "PQsetdbLogin");
      PQclear = (_PQclear)GetProcAddress(hInst, "PQclear");
      PQexec = (_PQexec)GetProcAddress(hInst, "PQexec");
      PQresultStatus = (_PQresultStatus)GetProcAddress(hInst, "PQresultStatus");
      PQntuples = (_PQntuples)GetProcAddress(hInst, "PQntuples");
      PQnfields = (_PQnfields)GetProcAddress(hInst, "PQnfields");
      PQfmod = (_PQfmod)GetProcAddress(hInst, "PQfmod");
      PQgetvalue = (_PQgetvalue)GetProcAddress(hInst, "PQgetvalue");
      PQgetlength = (_PQgetlength)GetProcAddress(hInst, "PQgetlength");
      PQresultErrorMessage = (_PQresultErrorMessage)GetProcAddress(hInst, "PQresultErrorMessage");
      PQerrorMessage = (_PQerrorMessage)GetProcAddress(hInst, "PQerrorMessage");
      PQresultErrorField = (_PQresultErrorField)GetProcAddress(hInst, "PQresultErrorField");

      if (!PQfinish || !PQstatus || !PQsetdbLogin || !PQclear || !PQexec || !PQresultStatus || !PQntuples || !PQnfields || !PQfmod || !PQgetvalue || !PQgetlength || !PQresultErrorMessage || !PQerrorMessage || !PQresultErrorField)
      { MessageBox(NULL, "*** Fatal error: libpq.dll for accessing postgres 8.x is not compatible", "dgate.exe", 0);
        exit(999);
      }
    }
    else
    { MessageBox(NULL, "*** Fatal error: could not load libpq.dll to access postgres8", "dgate.exe", 0);
      exit(999);
    }
  }
#endif
#endif

#if USESQLITE
#include "sqlite3.h"
#endif

#ifdef USESQLITE_DLL
typedef int (*sqlite3_callback)(void*,int,char**, char**);
typedef int (*_sqlite3_open)(const char *filename, struct sqlite3 **ppDb);
typedef int (*_sqlite3_exec)(struct sqlite3*, const char *sql, sqlite3_callback, void *, char **errmsg);
typedef int (*_sqlite3_get_table)(struct sqlite3 *, const char *sql, char ***resultp, int *nrow, int *ncolumn, char **errmsg);
typedef int (*_sqlite3_free_table)(char **result);
typedef int (*_sqlite3_close)(struct sqlite3 *);
typedef int (*_sqlite3_free)(void *);

  _sqlite3_open       __sqlite3_open=NULL;
  _sqlite3_exec       __sqlite3_exec=NULL;
  _sqlite3_get_table  __sqlite3_get_table=NULL;
  _sqlite3_free_table __sqlite3_free_table=NULL;
  _sqlite3_close      __sqlite3_close=NULL;
  _sqlite3_free       __sqlite3_free=NULL;

  void LoadSQLITE(void)
  { HINSTANCE hInst;
    if (__sqlite3_close) return;

    hInst = LoadLibrary("sqlite3.dll");
    if (!hinst) hInst = LoadLibrary("sqlite364.dll");
    if (!hinst) hInst = LoadLibrary("x64\\sqlite3.dll");
    if (hInst)
    { __sqlite3_open       = (_sqlite3_open      )GetProcAddress(hInst, "sqlite3_open");
      __sqlite3_exec       = (_sqlite3_exec      )GetProcAddress(hInst, "sqlite3_exec");
      __sqlite3_get_table  = (_sqlite3_get_table )GetProcAddress(hInst, "sqlite3_get_table");
      __sqlite3_free_table = (_sqlite3_free_table)GetProcAddress(hInst, "sqlite3_free_table");
      __sqlite3_close      = (_sqlite3_close     )GetProcAddress(hInst, "sqlite3_close");
      __sqlite3_free       = (_sqlite3_free      )GetProcAddress(hInst, "sqlite3_free");
    }
    else
    { MessageBox(NULL, "*** Fatal error: could not load sqlite3.dll to access SQLite", "dgate.exe", 0);
      exit(999);
    }
  }

#define sqlite3_open 		__sqlite3_open
#define sqlite3_exec 		__sqlite3_exec
#define sqlite3_get_table 	__sqlite3_get_table
#define sqlite3_free_table 	__sqlite3_free_table
#define sqlite3_close 		__sqlite3_close
#define sqlite3_free 		__sqlite3_free
#else
#if USESQLITE
void LoadSQLITE(void)
{
}
#endif
#endif

#if defined(__BORLANDC__) || defined(__WATCOMC__)
  void LoadODBC(void)
  { HINSTANCE hInst = LoadLibrary("odbc32.dll");

    __SQLAllocConnect = (_SQLAllocConnect)GetProcAddress(hInst, "SQLAllocConnect");
    __SQLAllocEnv = (_SQLAllocEnv)GetProcAddress(hInst, "SQLAllocEnv");
    __SQLAllocStmt = (_SQLAllocStmt)GetProcAddress(hInst, "SQLAllocStmt");
    __SQLConnect = (_SQLConnect)GetProcAddress(hInst, "SQLConnect");
    __SQLDisconnect = (_SQLDisconnect)GetProcAddress(hInst, "SQLDisconnect");
    __SQLFreeConnect = (_SQLFreeConnect)GetProcAddress(hInst, "SQLFreeConnect");
    __SQLFreeEnv = (_SQLFreeEnv)GetProcAddress(hInst, "SQLFreeEnv");
    __SQLFreeStmt = (_SQLFreeStmt)GetProcAddress(hInst, "SQLFreeStmt");
    __SQLTables = (_SQLTables)GetProcAddress(hInst, "SQLTables");
    __SQLBindCol = (_SQLBindCol)GetProcAddress(hInst, "SQLBindCol");
    __SQLFetch = (_SQLFetch)GetProcAddress(hInst, "SQLFetch");
    __SQLError = (_SQLError)GetProcAddress(hInst, "SQLError");
    __SQLExecDirect = (_SQLExecDirect)GetProcAddress(hInst, "SQLExecDirect");
   }
#endif

Database :: Database()
#ifdef __GNUC__ //Faster with member initialization.
:db_type(DT_ODBC),
#ifdef WIN32
hEnv(NULL),
hDbc(NULL),
hStmt(NULL),
Mode(NULL),
//SWORD
//SDWORD
//State
//Msg
#endif //WIN32
#ifdef POSTGRES
conn(NULL),
pgres(NULL),
#endif //POSTGRES
#ifdef USEMYSQL
mydata(NULL),
res(NULL),
#endif
#ifdef USESQLITE
sqlitedata(NULL),
//	sqlitedata(NULL),
resultp(NULL),
#endif //USESQLITE
#ifdef POSTGRES
Postgres(0),
#endif //POSTGRES
#ifdef USEMYSQL
Mysql(0),
#endif //USEMYSQL
#ifdef USESQLITE
SqLite(0),
#endif //USESQLITE
#if defined(POSTGRES) || defined(USEMYSQL) || defined (USESQLITE)
ncolumns(0),
ntuples(0),
current_tuple(0),
#endif
Connected(FALSE),
thq(),
queryrecno(0),
hashfield(-1),
m_Critical(),
#ifdef WIN32
RetCode(),
#endif
SQLStatement(NULL)
{
#else
{
	db_type = DT_ODBC;
#endif

#ifdef WIN32
#ifndef __GNUC__
	hEnv = NULL;
	hDbc = NULL;
	hStmt = NULL;
	Mode = 0;
#endif
#endif

#ifdef POSTGRES
	{
	char RootSC[64];
	char Temp[128];

	MyGetPrivateProfileString ( RootConfig, "MicroPACS", RootConfig,
	(char*) RootSC, 64, ConfigFile);

	MyGetPrivateProfileString ( RootSC, "Postgres", "0", (char*) Temp, 128, ConfigFile);
	Postgres = atoi(Temp);
	if (Postgres) db_type = DT_POSTGRES;

#ifndef __GNUC__
	conn = NULL;
	pgres = NULL;
	ntuples = 0;
	current_tuple = 0;
#endif
#ifdef WIN32
	if (Postgres) LoadPostgres();
#endif
	}
#endif

#ifdef USEMYSQL
	{
	char RootSC[64];
	char Temp[128];

	MyGetPrivateProfileString ( RootConfig, "MicroPACS", RootConfig,
	(char*) RootSC, 64, ConfigFile);

	MyGetPrivateProfileString ( RootSC, "Mysql", "0", (char*) Temp, 128, ConfigFile);
	Mysql = atoi(Temp);
	if (Mysql) db_type = DT_MYSQL;

#ifndef __GNUC__
	mydata = NULL;
	res = NULL;
	ntuples = 0;
	current_tuple = 0;
#endif
		
#ifdef WIN32
	if (Mysql) LoadMySQL();
#endif
	}
#endif

#ifdef USESQLITE
	{
	char RootSC[64];
	char Temp[128];

	MyGetPrivateProfileString ( RootConfig, "MicroPACS", RootConfig,
	(char*) RootSC, 64, ConfigFile);

	MyGetPrivateProfileString ( RootSC, "SqLite", "0", (char*) Temp, 128, ConfigFile);
	SqLite = atoi(Temp);
	if (SqLite) db_type = DT_SQLITE;

#ifndef __GNUC__
	sqlitedata = NULL;
        resultp    = NULL;
	ntuples = 0;
	current_tuple = 0;
#endif

#ifdef WIN32
	if (SqLite) LoadSQLITE();
#endif
	}
#endif

#ifndef __GNUC__
	Connected = FALSE;
#endif
        SQLStatement = (char *)malloc(MAXQUERYLENGTH);

	// built-in dbase support (when datasource is a directory, must end with \)
	for (int i=0; i<20; i++) dbase_handles[i]=NULL;
        memset(queryfields, 0, sizeof(queryfields));
        dir[0] = 0;

#if defined(__BORLANDC__) || defined(__WATCOMC__)
	LoadODBC();
#endif
	}

Database :: Database(enum DBTYPE dbtype)
#ifdef __GNUC__ //Faster with member initialization.
	:db_type(dbtype),
#ifdef WIN32
	hEnv(NULL),
	hDbc(NULL),
	hStmt(NULL),
	Mode(NULL),
	//SWORD
	//SDWORD
	//State
	//Msg
#endif //WIN32
#ifdef POSTGRES
	conn(NULL),
	pgres(NULL),
#endif //POSTGRES
#ifdef USEMYSQL
	mydata(NULL),
	res(NULL),
#endif
#ifdef USESQLITE
	sqlitedata(NULL),
	//	sqlitedata(NULL),
	resultp(NULL),
#endif //USESQLITE
#ifdef POSTGRES
	Postgres(0),
#endif //POSTGRES
#ifdef USEMYSQL
	Mysql(0),
#endif //USEMYSQL
#ifdef USESQLITE
	SqLite(0),
#endif //USESQLITE
#if defined(POSTGRES) || defined(USEMYSQL) || defined (USESQLITE)
	ncolumns(0),
	ntuples(0),
	current_tuple(0),
#endif
	Connected(FALSE),
	thq(),
	queryrecno(0),
	hashfield(-1),
	m_Critical(),
#ifdef WIN32
	RetCode(),
#endif
	SQLStatement(NULL)
	{
#else
	{
	db_type = dbtype;
#endif
	if (db_type == DT_NULL) return;

#ifdef WIN32
#ifndef __GNUC__
	hEnv = NULL;
	hDbc = NULL;
	hStmt = NULL;
	Mode = 0;
#endif
#endif

#ifdef POSTGRES
	Postgres = db_type==DT_POSTGRES;
#ifndef __GNUC__
	conn = NULL;
	pgres = NULL;
	ntuples = 0;
	current_tuple = 0;
#endif
#endif

#ifdef USEMYSQL
	Mysql = db_type==DT_MYSQL;
#ifndef __GNUC__
	mydata = NULL;
	res = NULL;
	ntuples = 0;
	current_tuple = 0;
#endif

#ifdef WIN32
	if (Mysql) LoadMySQL();
#endif
#endif

#ifdef USESQLITE
	SqLite = db_type==DT_SQLITE;
#ifndef __GNUC__
	sqlitedata = NULL;
        resultp    = NULL;
	ntuples = 0;
	current_tuple = 0;
#endif

#ifdef WIN32
	if (SqLite) LoadSQLITE();
#endif
#endif

#ifndef __GNUC__
	Connected = FALSE;
#endif
        SQLStatement = (char *)malloc(MAXQUERYLENGTH);

	// built-in dbase support (when datasource is a directory, must end with \)
	for (int i=0; i<20; i++) dbase_handles[i]=NULL;
        memset(queryfields, 0, sizeof(queryfields));
        dir[0] = 0;

#if defined(__BORLANDC__) || defined(__WATCOMC__)
	LoadODBC();
#endif
	}

#if defined(__BORLANDC__) || defined(__WATCOMC__)
#define SQLAllocConnect __SQLAllocConnect
#define SQLAllocEnv __SQLAllocEnv
#define SQLAllocStmt __SQLAllocStmt
#define SQLConnect __SQLConnect
#define SQLDisconnect __SQLDisconnect
#define SQLFreeConnect __SQLFreeConnect
#define SQLFreeEnv __SQLFreeEnv
#define SQLFreeStmt __SQLFreeStmt
#define SQLTables __SQLTables
#define SQLBindCol __SQLBindCol
#define SQLFetch __SQLFetch
#define SQLError __SQLError
#define SQLExecDirect __SQLExecDirect
#endif

Database :: ~Database()
	{
	Close();
	//if (db_type == DT_NULL) return;

#ifdef POSTGRES
	if (Postgres)
	if (conn != NULL)
	{
		PQfinish(conn);
		conn = NULL;
	}
#endif

#ifdef USEMYSQL
	if (mydata != NULL)
	{
		mysql_close(mydata);
		mydata = NULL;
	}
#endif

#ifdef USESQLITE
	if (sqlitedata != NULL)
	{
		if (resultp)
			{ 
			sqlite3_free_table(resultp);
			resultp = NULL;
			}
		sqlite3_close(sqlitedata);
		sqlitedata = NULL;
	}
#endif

        free(SQLStatement);
	}
	
struct dbase_header *copies[20];

BOOL	Database :: Open (
	const char	*lDataSource,  // Made locals.
	const char	*User,
	const char	*lPassword,
	const char 	*lServerName)	// used in Sybase context, ignored here
	{
	UNUSED_ARGUMENT(lServerName);

	if ( Connected )
		Close();

	DatabaseOpen++;

	if (lDataSource[0]==0) 
		db_type = DT_NULL;

	if (db_type == DT_NULL) 
		{
		Connected = TRUE;
  	    InitializeCriticalSection(&m_Critical);
		return TRUE;
		}

        if (NumIndexing<0) NumIndexing=0;	// signal dgate.cpp that indexing started

#ifdef WIN32
	NativeError = 0;			// clear error info (maybe filled later)
	State[0] = 0;
	Msg  [0] = 0;
#endif

	// test before built-in dbase because sqlite uses a database file that may contain PATHSEPCHAR
#ifdef USESQLITE
	if (SqLite)
	{
	  	char RootSC[64];
	  	char Temp[128];

		MyGetPrivateProfileString ( RootConfig, "MicroPACS", RootConfig,
	  		(char*) RootSC, 64, ConfigFile);
  	  	MyGetPrivateProfileString ( RootSC, "DoubleBackSlashToDB", "0",
			(char*) Temp, 128, ConfigFile);
	  	if (atoi(Temp))
			{
			OperatorConsole.printf ("*** cannot use DoubleBackSlashToDB=1 with sqlite database\n");
			return FALSE;
			}

		if (sqlite3_open(lDataSource, &sqlitedata))
			{
			OperatorConsole.printf ("*** cannot connect to sqlite database %s\n", lDataSource);
			return FALSE;
			}
		Connected = TRUE;
		InitializeCriticalSection(&m_Critical);

  	  	MyGetPrivateProfileString ( RootSC, "SQLiteStartup", "PRAGMA synchronous = OFF", // makes it MUCH faster
			(char*) Temp, 128, ConfigFile);
		SQLITEExec(Temp, FALSE);	

		return (TRUE);
	}
#endif

	// built-in dbase support (when datasource is a directory, must end with \)
	if (strchr(lDataSource, PATHSEPCHAR))
	{ 
	  if (db_type != DT_DBASEIII && db_type != DT_DBASEIIINOINDEX) db_type = DT_DBASEIII;

#ifdef WIN32
	  WIN32_FIND_DATA	FileData;
	  HANDLE		fdHandle;
#else
	  DIR			*dird;
	  dirent		*diren;
#endif

	  char 			TempPath[1024];
	  int 			dbnum=0, n;
  	  char			RootSC[64];
	  char			Temp[128];

          memset(queryfields, 0, sizeof(queryfields));
	  
	  MyGetPrivateProfileString ( RootConfig, "MicroPACS", RootConfig,
	  	(char*) RootSC, 64, ConfigFile);

  	  MyGetPrivateProfileString ( RootSC, "DoubleBackSlashToDB", "0",
		(char*) Temp, 128, ConfigFile);
	  OdbcDoubleBackSlashToDB = atoi(Temp);

  	  MyGetPrivateProfileString ( RootSC, "UseEscapeStringConstants", "0",
		(char*) Temp, 128, ConfigFile);
	  OdbcUseEscapeStringConstants = atoi(Temp);

  	  MyGetPrivateProfileString ( RootSC, "PackDBF", "0",
		(char*) Temp, 128, ConfigFile);
	  PackDBF = atoi(Temp);
	  // NeedPack=1:                        /* normal server operation */
	  if (NeedPack==2) PackDBF = FALSE;     /* 20 MB index, no pack - used during full regen (please clear db first) */
          if (NeedPack==3) PackDBF = TRUE;	/* 20 MB index force pack - used for c-echo maintenance */
          if (NeedPack==4) PackDBF = FALSE;	/* 20 MB index force index - used for c-echo maintenance */
          if (NeedPack==5) PackDBF = FALSE;	/* 20 MB index force non-threaded index - used for command line maintenance */

  	  MyGetPrivateProfileString ( RootSC, "IndexDBF", "10",
		(char*) Temp, 128, ConfigFile);
	  IndexDBF = atoi(Temp);
	  if (NeedPack>1 && IndexDBF<20) IndexDBF=20;	// allocate at least 20 MB during regen - (for 2.5*10^6 images)

          if (db_type == DT_DBASEIIINOINDEX)
          { IndexDBF = 0;
            PackDBF  = FALSE;
          }

  	  MyGetPrivateProfileString ( RootSC, "LongQueryDBF", "1000",
		(char*) Temp, 128, ConfigFile);
	  LongQueryDBF = atoi(Temp);

          strcpy(dir, lDataSource);

	  strcpy(TempPath, lDataSource);
	  strcat(TempPath, "*.dbf");

#ifdef WIN32
	  fdHandle = FindFirstFile(TempPath, &FileData);
	  if(fdHandle == INVALID_HANDLE_VALUE)
	  { Connected = TRUE;
    	    InitializeCriticalSection(&m_Critical);
	    return ( TRUE );	// no databases found: is not an error!
          }
#else
	  dird = opendir(lDataSource);
	  if(dird == NULL)
	  { Connected = TRUE;
    	    InitializeCriticalSection(&m_Critical);
	    return ( TRUE );	// no databases found: is not an error!
          }
#endif

	  while ( TRUE )
	  { 
#ifdef WIN32
 	    if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	    { strcpy(TempPath, lDataSource);
	      strcat(TempPath, FileData.cFileName);
#else
            diren = readdir(dird);
 	    if (diren)
	    { strcpy(TempPath, lDataSource);
	      strcat(TempPath, diren->d_name);
#endif

	      // do not open X files (extracted database parts for selected patient)
#ifdef WIN32
	      if (FileData.cFileName[0]!='x' && FileData.cFileName[0]!='X')
#else
	      if (diren->d_name[0]!='x' && diren->d_name[0]!='X' && 
                  (strstr(diren->d_name, ".dbf")||strstr(diren->d_name, ".DBF")))
#endif
              { if ((n=dbase_open(TempPath, &dbase_handles[dbnum++]))<0)
                { OperatorConsole.printf("*** Open dbase %s error code %d\n", TempPath, n);
#ifdef WIN32
                  FindClose(fdHandle);
#else
                  closedir(dird);
#endif
                  return FALSE;
                }

	        if (dbnum<1 || dbnum>19)
		  printf("ERROR DB HANDLES");

                dbase_handles[dbnum-1]->n = dbnum-1;
              }
            }
			
#ifdef WIN32
	    if(!FindNextFile(fdHandle, &FileData))
#else
	    if (diren==NULL)
#endif
	      break;
	  }

	  if (NeedPack)
          { int i, totalrec=0;
#ifdef WIN32
            HANDLE handles[20];
#else
	    pthread_t handles[20];
#endif

	    for (i=0; i<dbnum; i++) totalrec += dbase_handles[i]->reccount;

            if (hash_table)
              memset(hash_table, 0, hash_count * sizeof(HASH_DATA));
            
            for (i=0; i<dbnum; i++)
            { if (PackDBF)
              { OperatorConsole.printf("Packing database %s\n", dbase_handles[i]->filename);
  		dbase_pack(dbase_handles[i]);
              }
              if (IndexDBF)
	      { 
#ifdef WIN32
		unsigned long ThreadID;
#endif

                dbase_handles[i]->TotalRec   = totalrec;
		dbase_handles[i]->MBExtra    = IndexDBF;

		copies[i] = (dbase_header*)malloc(sizeof(dbase_header)+32*MAXFIELDS+512);
  		memcpy(copies[i], dbase_handles[i], sizeof(dbase_header)+32*MAXFIELDS+512);

		if (NeedPack==5 || NeedPack==2 || NoThread)
		  Fill_Memory_Index(copies[i]);
		else
#ifdef WIN32
    		  handles[i] = CreateThread(NULL, 0x000ff000, (LPTHREAD_START_ROUTINE) Fill_Memory_Index, copies[i], 0, &ThreadID);
#else
		{ pthread_create(&handles[i], NULL, (void*(*)(void*))Fill_Memory_Index, (void *)copies[i]);
		  pthread_detach(handles[i]);
		}
#endif
              }
            }
          }

#ifdef WIN32
          FindClose(fdHandle);
#else
          closedir(dird);
#endif
  	  Connected = TRUE;
  	  InitializeCriticalSection(&m_Critical);
	  NeedPack = FALSE;
	  return TRUE;
	}

#ifdef POSTGRES
	if (Postgres)
	{	char *port = NULL;
		char host[256], *p;

		strcpy(host, DataHost);	// allows host:port syntax for Postgres
		if(p=strchr(host, ':'))
		{
			port = p+1;
			*p = 0;
		}
		if (conn != NULL && (PQstatus(conn) == CONNECTION_OK))
		{
			Connected = TRUE;
			InitializeCriticalSection(&m_Critical);
			return (TRUE);
		}
		conn = PQsetdbLogin(host,port,NULL,NULL,lDataSource,User,lPassword);
		if (PQstatus(conn) == CONNECTION_BAD)
		{	// retry once a bit later
			delay(555);
			conn = PQsetdbLogin(host,port,NULL,NULL,lDataSource,User,lPassword);
			if (PQstatus(conn) == CONNECTION_BAD)
				{
				OperatorConsole.printf("*** %s\n",PQerrorMessage(conn));
				PQfinish(conn);
				conn = NULL;
				return (FALSE);
				}
		}
		Connected = TRUE;
		InitializeCriticalSection(&m_Critical);
		return (TRUE);
	}
#endif

#ifdef USEMYSQL
	if (Mysql)
	{	int port = 3306; // MYSQL_PORT
		char host[256], *p;

		strcpy(host, DataHost);	// allows host:port syntax for MySQL
		if((p=strchr(host, ':')))
		{
			port = atoi(p+1);
			*p = 0;
		}

		if (mydata != NULL)
		{
			Connected = TRUE;
			InitializeCriticalSection(&m_Critical);
			return (TRUE);
		}
#ifndef DARWIN
		if (!((mydata = mysql_init((MYSQL*)NULL)) && mysql_real_connect(mydata,host,User,lPassword,NULL,port,NULL,65536)))
		{
			OperatorConsole.printf("***Cannot connect to MySQL on port %d\n", port);
			OperatorConsole.printf("***MySQL Error message: %s\n", mysql_error(mydata));
			mysql_close(mydata);
			mydata = NULL;
			return (FALSE);
		}
#else //DARWIN has mysql built in, lets try and start it.
		if (!((mydata = mysql_init((MYSQL*)NULL)) && mysql_real_connect(mydata,host,User,lPassword,NULL,port,NULL,65536)))
		{
			mysql_close(mydata);
			mydata = NULL;
			char mysqldPath[40] = "/usr/local/mysql/bin";
			DIR	*mysqldDir;
			// Open the directory, if we can, we have MySQL.
			if((mysqldDir = opendir(mysqldPath)) != NULL)
			{
				closedir(mysqldDir);// Don't need it, just a test.
				strcat(mysqldPath,"/mysqld_safe &");//Add the file name
				system(mysqldPath);//Start it
				sleep(4); //Wait for it to start, it's slow.
			}
			//Try again, if not, report & bail
			if (!((mydata = mysql_init((MYSQL*)NULL)) && mysql_real_connect(mydata,host,User,lPassword,NULL,port,NULL,65536)))
			{
				OperatorConsole.printf("***Cannot connect to MySQL on port %d\n", port);
				OperatorConsole.printf("***MySQL Error message: %s\n", mysql_error(mydata));
				mysql_close(mydata);
				mydata = NULL;
				return (FALSE);
			}
		}
#endif //DARWIN Note:I removed the else{}, it's not needed, return()s do it.
		if (mysql_select_db( mydata, lDataSource)<0)
		{
			OperatorConsole.printf("***Cannot select MySQL database %s\n", lDataSource);
			mysql_close(mydata);
			mydata = NULL;
			return (FALSE);
		}
		Connected = TRUE;
		InitializeCriticalSection(&m_Critical);
		return (TRUE);
	}
#endif

#ifdef WIN32
	if ( (RetCode = SQLAllocEnv (&hEnv)) != SQL_SUCCESS )
		return ( FALSE );
	if ( (RetCode = SQLAllocConnect(hEnv, &hDbc)) != SQL_SUCCESS)
		{
		SQLFreeEnv ( hEnv );
		hEnv = NULL;
#ifdef	DB_DEBUG
		SystemDebug.printf("***Failed to SQLAllocConnect()\n");
#endif
		return ( FALSE );
		}
	RetCode =  SQLConnect(hDbc, (BYTE*)lDataSource, SQL_NTS,
						(BYTE*)User, SQL_NTS, (BYTE*)lPassword, SQL_NTS);
	if ( !((RetCode == SQL_SUCCESS) || (RetCode == SQL_SUCCESS_WITH_INFO)))
		{
		SQLError (hEnv, hDbc, hStmt, (BYTE*)State, &NativeError,
		(BYTE*)Msg, 512, &MsgL);
#ifdef	DB_DEBUG
		PrintLastError();
#endif
		SQLFreeConnect ( hDbc );
		SQLFreeEnv ( hEnv );
		hDbc = NULL;
		hEnv = NULL;
#ifdef	DB_DEBUG
			SystemDebug.printf("***Failed to SQLConnect()\n");
#endif
		return ( FALSE );
		}
 
	
	if ( (RetCode = SQLAllocStmt (hDbc, &hStmt)) != SQL_SUCCESS)
		{
		SQLDisconnect ( hDbc );
		SQLFreeConnect ( hDbc );
		SQLFreeEnv ( hEnv );
		hDbc = NULL;
		hEnv = NULL;
		hStmt = NULL;
#ifdef	DB_DEBUG
		SystemDebug.printf("***Failed to SQLAllocStmt()\n");
#endif
		return ( FALSE );
		}
	Connected = TRUE;
	InitializeCriticalSection(&m_Critical);
#endif
	return ( TRUE );
	}

BOOL	Database :: Close ()
	{

	DatabaseClose++;
	if (db_type == DT_NULL) 
		{
        DeleteCriticalSection(&m_Critical);
		Connected = FALSE;
		return TRUE;
		}

	// built-in dbf support
	if (dir[0])
	{ if ( Connected )
  	  { int i;
            for (i=0; i<20; i++) 
              if (dbase_handles[i]!=NULL)
              {	dbase_close(dbase_handles[i]);
                dbase_handles[i]=NULL;
              }
            DeleteCriticalSection(&m_Critical);
	    for(i=0; i<MAXFIELDS; i++) free(queryfields[i]);
            memset(queryfields, 0, sizeof(queryfields));
          }
          Connected = FALSE;
          return (TRUE);
        }
	// end built-in dbf support

#ifdef POSTGRES
	if (Postgres)
	{
		if (pgres != NULL)
		{
			PQclear(pgres);
			pgres = NULL;
		}
		if (Connected)
		{
			/*if (conn != NULL)
			{
				PQfinish(conn);
				conn = NULL;
			}*/
			DeleteCriticalSection(&m_Critical);
			Connected = FALSE;
		}
		return TRUE;
	}
#endif

#ifdef USEMYSQL
	if (Mysql)
	{
		if (res != NULL)
		{
			mysql_free_result(res);
			res = NULL;
		}
		if (Connected)
		{
			if (mydata != NULL)
			{
				mysql_close(mydata);
				mydata = NULL;
			}
			DeleteCriticalSection(&m_Critical);
			Connected = FALSE;
		}
		return TRUE;
	}
#endif

#ifdef USESQLITE
	if (SqLite)
	{
		if (Connected)
		{
			if (resultp)
			{ 
				sqlite3_free_table(resultp);
				resultp = NULL;
			}
			if (sqlitedata != NULL)
			{
				sqlite3_close(sqlitedata);
				sqlitedata = NULL;
			}
			DeleteCriticalSection(&m_Critical);
			Connected = FALSE;
		}
		return TRUE;
	}
#endif

#ifdef WIN32
	if ( hStmt )
		SQLFreeStmt ( hStmt, SQL_DROP );
	if ( Connected )
	{
		if ( hDbc )
			SQLDisconnect ( hDbc );
		DeleteCriticalSection(&m_Critical);
	}
	if ( hDbc )
		SQLFreeConnect ( hDbc );
	if ( hEnv )
		SQLFreeEnv ( hEnv );
	hDbc = NULL;
	hEnv = NULL;
	hStmt = NULL;
	Connected = FALSE;
	return ( TRUE );
#else
	return ( FALSE );
#endif
	}

BOOL	Database	::	NewStatement ()
	{

	if (db_type == DT_NULL) 
		return TRUE;

#ifdef POSTGRES
	if (Postgres)
		return TRUE;
#endif	

#ifdef USEMYSQL
	if (Mysql)
		return TRUE;
#endif	

#ifdef USESQLITE
	if (SqLite)
		return TRUE;
#endif	

#ifdef WIN32
	if ( hStmt )
		SQLFreeStmt ( hStmt, SQL_DROP );

	Mode = 0;	
	if ( (RetCode = SQLAllocStmt (hDbc, &hStmt)) != SQL_SUCCESS)
		{
		hStmt = NULL;
#ifdef	DB_DEBUG
		SystemDebug.printf("***Failed to SQLAllocStmt()\n");
#endif
		return ( FALSE );
		}
 	return ( TRUE );
#else
 	return ( FALSE );
#endif
	}

BOOL Database :: QueryTables (
	char	*SearchTableQualifier,
	char	*SearchTableOwner,
	char	*SearchTableName,
	char	*SearchTableType )
	{

	if (db_type == DT_NULL) 
		return FALSE;

	// built-in dbf support - unsupported
	if (dir[0])
          return FALSE;
	// end built-in dbf support

#ifdef POSTGRES
	if (Postgres)
		return FALSE;
#endif

#ifdef USEMYSQL
	if (Mysql)
		return FALSE;
#endif

#ifdef USESQLITE
	if (SqLite)
		return FALSE;
#endif

#ifdef WIN32
	NewStatement();

	RetCode = SQLTables ( hStmt,
					(BYTE*)SearchTableQualifier, SQL_NTS,
					(BYTE*)SearchTableOwner, SQL_NTS,
					(BYTE*)SearchTableName, SQL_NTS,
					(BYTE*)SearchTableType, SQL_NTS );
	Mode = QUERY;
	if ((RetCode != SQL_SUCCESS) &&
		(RetCode != SQL_SUCCESS_WITH_INFO) )
		{
		SQLError (hEnv, hDbc, hStmt, (BYTE*)State, &NativeError,
		(BYTE*)Msg, 512, &MsgL);
#ifdef	DB_DEBUG
		SystemDebug.printf("***Failed to SQLTables()\n");
#endif
		return ( FALSE );
		}
	return ( TRUE );
#else
	UNUSED_ARGUMENT(SearchTableQualifier);
	UNUSED_ARGUMENT(SearchTableOwner);
	UNUSED_ARGUMENT(SearchTableName);
	UNUSED_ARGUMENT(SearchTableType);

	return ( FALSE );
#endif
	}
	

// since any SQL statement can fail with a DeadLock, one must retry failed statements
#ifdef POSTGRES
int	Database :: PGSQLExec(char *StatementText)
	{
	if (!Postgres) return -1;

	if (pgres != NULL)
		{
		PQclear(pgres);
		pgres = NULL;
		}
		
	pgres=PQexec(conn,StatementText);
	
	if (pgres==NULL)
	{	OperatorConsole.printf("*** %s\n",PQerrorMessage(conn));
		return -1;
	}
	
	if (PQresultStatus(pgres)==PGRES_TUPLES_OK)
	{
		ntuples = PQntuples(pgres);
		ncolumns = PQnfields(pgres);
		current_tuple = 0;
		return 0;
	}
	else if (PQresultStatus(pgres)==PGRES_BAD_RESPONSE || PQresultStatus(pgres)==PGRES_FATAL_ERROR)
	{	OperatorConsole.printf("*** %s\n",PQerrorMessage(conn));
		return -1;
	}
	else
	{
		ntuples = 0;
		ncolumns = 0;
		current_tuple = 0;
	}

	if (PQresultStatus(pgres)==PGRES_COMMAND_OK)
		return 0;
	
	if (strcmp(PQresultErrorField(pgres, PG_DIAG_SQLSTATE), "23505")==0) //UNIQUE VIOLATION 
		return 0;

	return -1;
	
	}
#endif

#ifdef USEMYSQL
int	Database :: MYSQLExec(char *StatementText, BOOL q)
	{
//SystemDebug.printf ( "MysqlExec %.1000s\n", StatementText);
	if (res != NULL)
		{
		mysql_free_result(res);
		res=NULL;
		}
		
	if (!mysql_query(mydata, StatementText))
		{
		if (q)
			{
			res = mysql_store_result( mydata );
			if (res)
				{
				ntuples = (long)mysql_num_rows(res);
				ncolumns = mysql_num_fields(res);
				}
			else
				{
				ntuples = 0;
				ncolumns = 0;
				}
			current_tuple = 0;
			return 0;
			}
		return 0;
		}
	
	if ((unsigned int)mysql_errno(mydata)==(unsigned int)1169) // ER_DUP_UNIQUE
		return 0;
	if ((unsigned int)mysql_errno(mydata)==(unsigned int)1062) // ER_DUP_UNIQUE
		return 0;

	return -1;
	}
#endif

#ifdef USESQLITE
#define SQLITE_BUSY         5
#define SQLITE_LOCKED       6
#define SQLITE_CONSTRAINT   19

int	Database :: SQLITEExec(char *StatementText, BOOL q)
	{
	int i, rc;
	char *err=NULL;

	if (resultp)
		{ 
		sqlite3_free_table(resultp);
		resultp = NULL;
		}

	if (!(rc=sqlite3_get_table(sqlitedata, StatementText, &resultp, &ntuples, &ncolumns, &err)))
		{
		if (!q && resultp) 
			{ 
			sqlite3_free_table(resultp);
			resultp = NULL;
			ntuples = 0;
			ncolumns = 0;
			current_tuple = 0;
			return 0;
			}
		current_tuple = 0;
		return 0;
		}
	
	if (sqlite3_errcode(sqlitedata)==SQLITE_CONSTRAINT)
		{
		sqlite3_free(err);
		return 0;
		}

	if (sqlite3_errcode(sqlitedata)==SQLITE_BUSY || sqlite3_errcode(sqlitedata)==SQLITE_LOCKED) 
		for(i=0; i<60; i++)
			{
			sqlite3_free(err);
			err = NULL;

			delay(100 + rand_r(&seedp)%2000);
			if (!(rc=sqlite3_get_table(sqlitedata, StatementText, &resultp, &ntuples, &ncolumns, &err)))
				{
				if (!q && resultp) 
					{ 
					sqlite3_free_table(resultp);
					resultp = NULL;
					ntuples = 0;
					ncolumns = 0;
					current_tuple = 0;
					return 0;
					}
				current_tuple = 0;
				return 0;
				}
			}
	
	if (sqlite3_errcode(sqlitedata)==SQLITE_CONSTRAINT)
		{
		sqlite3_free(err);
		return 0;
		}

	OperatorConsole.printf("***SQLITEExec error: %s\n", err);
	sqlite3_free(err);

	return -1;
	}
#endif

#ifdef WIN32
int	Database :: SQLExecDirectWithRetry(SQLHSTMT StatementHandle, SQLCHAR *StatementText, SQLINTEGER TextLength)
	{
	int RetCode, i;

	RetCode = SQLExecDirect ( StatementHandle, StatementText, TextLength );
	if ( (RetCode == SQL_SUCCESS) ||
		 (RetCode == SQL_SUCCESS_WITH_INFO) )
		return ( RetCode );

	SQLError (hEnv, hDbc, hStmt, (BYTE*)State, &NativeError,
		(BYTE*)Msg, 512, &MsgL);

	if (GetNativeError()==2601 || GetNativeError()==2627) 
		 return SQL_SUCCESS_WITH_INFO;		// duplicate index -> ignore

	if ((GetNativeError() != 1205) && (GetNativeError() != -1102))// deadlock -> retry
		return ( RetCode );

	for(i=0; i<6; i++)
		{
		Sleep(100 + rand_r(&seedp)%2000);
		RetCode = SQLExecDirect ( StatementHandle, StatementText, TextLength );
		if ( (RetCode == SQL_SUCCESS) ||
			 (RetCode == SQL_SUCCESS_WITH_INFO) )
			return ( RetCode );
			
		if (i==5)
			SQLError (hEnv, hDbc, hStmt, (BYTE*)State, &NativeError,
				(BYTE*)Msg, 512, &MsgL);
		}

	return ( RetCode );
	}
#endif

BOOL	Database :: Query (	const char	*Table,
				const char	*Columns,
				const char	*Where,
				const char	*Order )
	{ if (DebugLevel>=4)
	  { SystemDebug.printf("Query Tables: %.1000s\n", Table);
            SystemDebug.printf("Columns     : %.1000s\n", Columns);
            SystemDebug.printf("Where       : %.1000s\n", Where);
            SystemDebug.printf("Order       : %.1000s\n", Order);
          }

          return InternalQuery(Table, Columns, Where, Order, strcmp(Table, WorkListTableName)!=0);
	}						     // not worklist->recurse

BOOL	Database :: InternalQuery (	const char	*Table,
				const char	*Columns,
				const char	*Where,
				const char	*Order,
				BOOL	recurse )
	{

	DatabaseQuery++;

	if (db_type == DT_NULL) 
		{
		return TRUE;
		}

	// built-in dbf support (limited to simple single table queries)
	if (dir[0])
	{ char *tmp = (char *)malloc(MAXQUERYLENGTH); 	// need a lot of space!
          char *wheres = SQLStatement;			// need a lot of space!
          char cols[4096];
          char fname[64];
          char *p, *q, *r;
          int i, j, k, n, a;

          if (tmp==NULL) return FALSE;

          hashfield=-1;

          for(i=0; i<MAXFIELDS; i++) free(queryfields[i]);
          memset(queryfields, 0, sizeof(queryfields));

          wheres[0]=0;
          if (Where) if ( Where[0] != '\0' ) strcpy(wheres, Where);  // Add second if for DARWIN

	  strlwr(wheres);
	  
	  // start 20061213
	  if (1)	    				// replace spaces in strings by code -1 = 255 (non breaking space)
	  { int instring=0;				// in this way the sql parsing becomes much safer
            int L = strlen(wheres);
            for (k=0; k<L; k++) 
            { if (wheres[k]=='\'') instring = !instring;
              if (wheres[k]==' ' && instring==1) wheres[k] = -1;  // non breaking space
	    }
	  }
 	  // end 20061213

          strcpy(cols, Columns);
          strlwr(cols);

          for (i=0; i<MAXFIELDS; i++) querymodes[i] = 2;
          for (i=0; i<MAXFIELDS; i++) querycolumns[i] = -1;

	  strcpy(tmp, Table);
          p = strchr(tmp, ',');
          if (p) *p=0;
          strcat(tmp, ".dbf");

          for (i=0; i<20; i++)
          { if (!dbase_handles[i]) continue;
            p = strrchr(dbase_handles[i]->filename, PATHSEPCHAR);
            if (!p) continue;
            if (stricmp(p+1, tmp)==0)
            { thq = dbase_handles[i];

              hashfield = findhashfield(thq);

              for (j=0; j<thq->fieldcount; j++)
              { strcpy(fname, thq->fd[j].name);
                strlwr(fname);

                p = strstr(cols, fname);		// match the column name
                if (!p)
	        { if (strcmp(fname, "imagepat")==0)
                  { p = strstr(cols, "patientid");      // match alternative column names
                    if (!p) p = strstr(cols, "seriespat");
                  }
	          else if (strcmp(fname, "seriespat")==0)
                  { p = strstr(cols, "patientid");      // match alternative column names
                    if (!p) p = strstr(cols, "imagepat");
                  }
	          else if (strcmp(fname, "patientid")==0)
                  { p = strstr(cols, "seriespat");      // match alternative column names
                    if (!p) p = strstr(cols, "imagepat");
                  }
                }

                if (p) 
                { n = 0; for (k=0; k<p-cols; k++) if (cols[k]==',') n++;
                  querycolumns[j] = n;
                }

	        strcat(fname, " ");			 // 20061213 (plus subsequent constant names adapted)
                p = strstr(wheres, fname);		 // match the fieldname (with space no longer possible inside text strings)
                if (!p)
		{ if (strcmp(fname, "imagepat ")==0)
                  { p = strstr(wheres, "patientid ");    // match alternative fieldnames
                    if (!p) p = strstr(wheres, "seriespat ");
                  }
                  else if (strcmp(fname, "seriespat ")==0)
                  { p = strstr(wheres, "patientid ");    // match alternative fieldnames
                    if (!p) p = strstr(wheres, "imagepat ");
                  }
                  else if (strcmp(fname, "patientid ")==0)
                  { p = strstr(wheres, "seriespat ");    // match alternative fieldnames
                    if (!p) p = strstr(wheres, "imagepat ");
                  }
                }
                if (!p) continue;

                strcpy(tmp, p);				// get first clause for this field

                        p = strstr(tmp, " and ");	// clause ends with " AND "
                if (!p) p = strstr(tmp, " AND ");

		// start 20061213
                //if (!p) p = strstr(tmp, ") and ");
                //if (!p) p = strstr(tmp, ") AND ");
                //if (!p) p = strstr(tmp, "&");
                //if (!p) p = strstr(tmp, ")&");
		// end 20061213

		if (p) *p=0;
		if (p) r = p+1; else r = NULL;

                a = 0;

                p = strstr(tmp, " = ");			// value after ' = ' or ' LIKE' or 'in'
                if (!p) p = strstr(tmp, " LIKE ");
                if (!p) p = strstr(tmp, " like ");
                if (!p) { p = strstr(tmp, " in "); if (p) a = 66; }
                if (!p) { p = strstr(tmp, " IN "); if (p) a = 66; }
                if (!p) { p = strstr(tmp, " >= "); if (p) a = 16; }
                if (!p) { p = strstr(tmp, " <= "); if (p) a = 32; }
                if (!p) p = strstr(tmp, "="     );
                if (!p) continue;

                q = strrchr(p, '\'');			// value must be enclosed by ' : this will ignore E in E'...' or e'....'
                if (!q) continue;
                *q=0;
                q = strchr(p, '\'');
                if (!q) continue;
              
                if (q[1]=='%') 
                  queryfields[j] = strdup(q+2), querymodes[j]=3;	// %key? -> substring
                else
                  queryfields[j] = strdup(q+1);

		p = strchr(queryfields[j], '%');
                if (p)
                { if (p[1] == 0) 
                  { *p=0;
                    if (querymodes[j]!=3) querymodes[j]=11;		// key% -> starts with
		    							// else %key% -> substring
                  }
                }

                if (a==16 && r)						// >= and <= -> (date) range comparison
		{ r = strstr(r, fname);					// match the field name again for range comparison

                  if (r) r = strstr(r, " <= ");

                  if (r)
		  { strcpy(tmp, queryfields[j]);			// append the upper value to the query string
                    free(queryfields[j]);				// free string, need new dup
                    strcat(tmp, r+4);
                    r = strstr(tmp, "\' and ");				// lower range value end at ' and
                    if (r) *r=0;
                    queryfields[j] = strdup(tmp);			// new dup
                    a = 48;
                  }
		}

                if (a) querymodes[j] = a;				// <= or >= or range comparison

                process_escape(queryfields[j]);
              }
              queryrecno=0;

	      // try to convert normal query to an indexed one (works when Study and/or SeriesInstanceUID passed)
              if (recurse && Where!=NULL && strlen(Where)!=0 && hashfield>=0 && queryfields[hashfield]==NULL)
              { char pat[128]="";
                SQLLEN len;

                // try to get patient ID for query in image table from study or series table
                if (memicmp(Table, ImageTableName, strlen(ImageTableName))==0) 
                { InternalQuery (StudyTableName, "PatientID", Where, NULL, FALSE);
                  if (queryfields[0] && querymodes[0]==2)
                  { BindField (1, SQL_C_CHAR, pat, 128, &len);
                    NextRecord();
                  }
                  else
                  { InternalQuery (SeriesTableName, "SeriesPat", Where, NULL, FALSE);
                    if (queryfields[0] && querymodes[0]==2)
                    { BindField (1, SQL_C_CHAR, pat, 128, &len);
                      NextRecord();
                    }
                  }
                }

                // try to get patient for series table from study table
                else if (memicmp(Table, SeriesTableName, strlen(SeriesTableName))==0) 
                { InternalQuery (StudyTableName, "PatientID", Where, NULL, FALSE);
                  if (queryfields[0] && querymodes[0]==2)
                  { BindField (1, SQL_C_CHAR, pat, 128, &len);
                    NextRecord();
                  }
                }

		// re-create original query              
                InternalQuery (Table, Columns, Where, NULL, FALSE);
              
		// add indexed patient ID if found
                if (strlen(pat)) 
                { queryfields[hashfield] = strdup(pat);
                  querymodes [hashfield] = 2;
                }
              }

	      // performed in NextRecord
              // dbase_query(thq, &queryrecno, queryfields, querymodes);
              free(tmp);
              return TRUE;
            }
          }
          free(tmp);
          return FALSE;
	}
	// end built-in dbf support

#ifdef POSTGRES
	if (Postgres)
	{
		sprintf (SQLStatement, "SELECT %s FROM %s",
                                	Columns, Table);
        	if ( Where ) if ( Where[0] != '\0' ) // Add second if for DARWIN
                {
                	strcat ( SQLStatement, " WHERE " );
                	strcat ( SQLStatement, Where );
                }
        	if ( Order ) if ( Order[0] != '\0' ) // Add second if for DARWIN
                {
                	if ( strchr(Order, ',')==NULL )
                        {
                        	strcat ( SQLStatement, " ORDER BY " );
                        	strcat ( SQLStatement, Order );
                        }
                	else
                        {
                        	strcat ( SQLStatement, " ORDER BY (" );
                        	strcat ( SQLStatement, Order );
                        	strcat ( SQLStatement, ")" );
                        }
                }
		if (PGSQLExec(SQLStatement) == 0)
			return TRUE;
		else
			return FALSE;
	}
#endif

#ifdef USEMYSQL
	if (Mysql)
	{
		sprintf (SQLStatement, "SELECT %s FROM %s",
                                	Columns, Table);
        	if ( Where ) if ( Where[0] != '\0' ) // Add second if for DARWIN
                {
                	strcat ( SQLStatement, " WHERE " );
                	strcat ( SQLStatement, Where );
                }
        	if ( Order ) if ( Order[0] != '\0' ) // Add second if for DARWIN
                {
                	if ( strchr(Order, ',')==NULL )
                        {
                        	strcat ( SQLStatement, " ORDER BY " );
                        	strcat ( SQLStatement, Order );
                        }
                	else
                        {
                        	strcat ( SQLStatement, " ORDER BY (" );
                        	strcat ( SQLStatement, Order );
                        	strcat ( SQLStatement, ")" );
                        }
                }
		if (MYSQLExec(SQLStatement, TRUE) == 0)
			return TRUE;
		else
			return FALSE;
	}
#endif

#ifdef USESQLITE
	if (SqLite)
	{
		sprintf (SQLStatement, "SELECT %s FROM %s",
                                	Columns, Table);
        	if ( Where ) if ( Where[0] != '\0' ) // Add second if for DARWIN
                {
                	strcat ( SQLStatement, " WHERE " );
                	strcat ( SQLStatement, Where );
                }
        	if ( Order ) if ( Order[0] != '\0' ) // Add second if for DARWIN
                {
                	if ( strchr(Order, ',')==NULL )
                        {
                        	strcat ( SQLStatement, " ORDER BY " );
                        	strcat ( SQLStatement, Order );
                        }
                	else
                        {
                        	strcat ( SQLStatement, " ORDER BY (" );
                        	strcat ( SQLStatement, Order );
                        	strcat ( SQLStatement, ")" );
                        }
                }
		if (SQLITEExec(SQLStatement, TRUE) == 0)
			return TRUE;
		else
			return FALSE;
	}
#endif

#ifdef WIN32
	NewStatement();

	sprintf (SQLStatement, "SELECT %s FROM %s",
				Columns, Table);
	if ( Where ) if ( Where[0] != '\0' )
		{
		strcat ( SQLStatement, " WHERE " );
		strcat ( SQLStatement, Where );
		}
	if ( Order ) if ( Order[0] != '\0' )
		{
		if ( strchr(Order, ',')==NULL )
			{
			strcat ( SQLStatement, " ORDER BY " );
			strcat ( SQLStatement, Order );
			}
		else
			{
			strcat ( SQLStatement, " ORDER BY (" );
			strcat ( SQLStatement, Order );
			strcat ( SQLStatement, ")" );
			}
		char *p = strstr(SQLStatement, " LIMIT ");             // sql server does not support LIMIT in the same way
		if ( p )
			{
			char *q=strstr(p+1, " OFFSET ");
                	sprintf (SQLStatement, "SET ROWCOUNT %d SELECT %s FROM %s WHERE %s ORDER BY %s OFFSET %d",
			atoi(p+7), Columns, Table, Where, Order, q?atoi(q+8):0);
			}
                }


	Mode = QUERY;
	RetCode = SQLExecDirectWithRetry ( hStmt, (unsigned char *) SQLStatement, SQL_NTS );
	if ( (RetCode == SQL_SUCCESS) ||
		 (RetCode == SQL_SUCCESS_WITH_INFO) )
		return ( TRUE );

#ifdef	DB_DEBUG
	SystemDebug.printf("***Failed SQLExecDirect : %.1000s\n", SQLStatement);
	PrintLastError();
#endif

#endif
	return ( FALSE );
	}

BOOL	Database :: QueryDistinct (	char	*Table,
					const char	*Columns,
					const char	*Where,
					const char	*Order )
        { 
	if (DebugLevel>=4)
	{ SystemDebug.printf("Query Distinct Tables: %.1000s\n", Table);
          SystemDebug.printf("Columns              : %.1000s\n", Columns);
          SystemDebug.printf("Where                : %.1000s\n", Where);
          SystemDebug.printf("Order                : %.1000s\n", Order);
        }

	if (db_type == DT_NULL) 
		{
		return TRUE;
		}

	// built-in dbf support
	if (dir[0])
          return InternalQuery(Table, Columns, Where, Order, strcmp(Table, WorkListTableName)!=0);
          // return Query(Table, Columns, Where, Order ); remove double log
	// end built-in dbf support

#ifdef POSTGRES
	if (Postgres)
	{
		DatabaseQuery++;
		sprintf (SQLStatement, "SELECT DISTINCT %s FROM %s",
                	                Columns, Table);
        	if ( Where ) if ( Where[0] != '\0' ) // Add second if for DARWIN
        	{
                	strcat ( SQLStatement, " WHERE " );
                	strcat ( SQLStatement, Where );
        	}
        	if ( Order ) if ( Order[0] != '\0' ) // Add second if for DARWIN
        	{
        		if ( strchr(Order, ',')==NULL )
                	{
                		strcat ( SQLStatement, " ORDER BY " );
                		strcat ( SQLStatement, Order );
                	}
                	else
                	{
                		strcat ( SQLStatement, " ORDER BY (" );
                		strcat ( SQLStatement, Order );
                		strcat ( SQLStatement, ")" );
                	}
        	}
		if (PGSQLExec(SQLStatement) == 0)
                        return TRUE;
                else
                        return FALSE;
	}
#endif

#ifdef USEMYSQL
	if (Mysql)
	{
		DatabaseQuery++;
		sprintf (SQLStatement, "SELECT DISTINCT %s FROM %s",
                	                Columns, Table);
        	if ( Where ) if ( Where[0] != '\0' ) // Add second if for DARWIN
        	{
                	strcat ( SQLStatement, " WHERE " );
                	strcat ( SQLStatement, Where );
        	}
        	if ( Order ) if ( Order[0] != '\0' ) // Add second if for DARWIN
        	{
        		if ( strchr(Order, ',')==NULL )
                	{
                		strcat ( SQLStatement, " ORDER BY " );
                		strcat ( SQLStatement, Order );
                	}
                	else
                	{
                		strcat ( SQLStatement, " ORDER BY (" );
                		strcat ( SQLStatement, Order );
                		strcat ( SQLStatement, ")" );
                	}
        	}
		if (MYSQLExec(SQLStatement, TRUE) == 0)
                        return TRUE;
                else
                        return FALSE;
	}
#endif

#ifdef USESQLITE
	if (SqLite)
	{
		DatabaseQuery++;
		sprintf (SQLStatement, "SELECT DISTINCT %s FROM %s",
                	                Columns, Table);
        	if ( Where ) if ( Where[0] != '\0' ) // Add second if for DARWIN
        	{
                	strcat ( SQLStatement, " WHERE " );
                	strcat ( SQLStatement, Where );
        	}
        	if ( Order ) if ( Order[0] != '\0' ) // Add second if for DARWIN
        	{
        		if ( strchr(Order, ',')==NULL )
                	{
                		strcat ( SQLStatement, " ORDER BY " );
                		strcat ( SQLStatement, Order );
                	}
                	else
                	{
                		strcat ( SQLStatement, " ORDER BY (" );
                		strcat ( SQLStatement, Order );
                		strcat ( SQLStatement, ")" );
                	}
        	}
		if (SQLITEExec(SQLStatement, TRUE) == 0)
                        return TRUE;
                else
                        return FALSE;
	}
#endif

#ifdef WIN32
	NewStatement();
	DatabaseQuery++;

	sprintf (SQLStatement, "SELECT DISTINCT %s FROM %s",
				Columns, Table);
	if ( Where ) if ( Where[0] != '\0' )
		{
		strcat ( SQLStatement, " WHERE " );
		strcat ( SQLStatement, Where );
		}
	if ( Order ) if ( Order[0] != '\0' )
		{
		if ( strchr(Order, ',')==NULL )
			{
			strcat ( SQLStatement, " ORDER BY " );
			strcat ( SQLStatement, Order );
			}
		else
			{
			strcat ( SQLStatement, " ORDER BY (" );
			strcat ( SQLStatement, Order );
			strcat ( SQLStatement, ")" );
			}
		char *p = strstr(SQLStatement, " LIMIT ");             // sql server does not support LIMIT in the same way
		if ( p )
			{
			char *q=strstr(p+1, " OFFSET ");
                	sprintf (SQLStatement, "SET ROWCOUNT %d SELECT %s FROM %s WHERE %s ORDER BY %s OFFSET %d",
				atoi(p+7), Columns, Table, Where, Order, q?atoi(q+8):0);
			}
		}

	RetCode = SQLExecDirectWithRetry ( hStmt, (unsigned char *) SQLStatement, SQL_NTS );
	Mode = QUERY;
	if ( (RetCode == SQL_SUCCESS) ||
		 (RetCode == SQL_SUCCESS_WITH_INFO) )
		return ( TRUE );

#ifdef	DB_DEBUG
	SystemDebug.printf("***Failed SQLExecDirect : %.1000s\n", SQLStatement);
	PrintLastError();
#endif

#endif
	return ( FALSE );
	}

BOOL	Database :: AddRecord (	const char	*Table,
				const char	*Columns,
				const char	*Values )
	{

	DatabaseAddRecord++;

        if (DebugLevel>=4)
        { SystemDebug.printf("Add to Table: %.1000s\n", Table);
          SystemDebug.printf("Columns: %.1000s\n", Columns);
          SystemDebug.printf("Values: %.1000s\n", Values);
        }

	if (db_type == DT_NULL) 
		{
		return TRUE;
		}

	// built-in dbf support
	if (dir[0])
	{ char tmp[512];
          char cols[4096];
          char *fields[MAXFIELDS];
          int i, j, k, n, L;
          char *p;
          struct dbase_header *th;
          int hfield=-1;

	  if (NumIndexing>0) 
	  { OperatorConsole.printf("*** AddRecord: DbaseIII read only during indexing\n");
            return FALSE;
          }

          memset(fields, 0, sizeof(fields));

          strcpy(cols, Columns);
          strlwr(cols);

          strcpy(tmp, Table);
          strcat(tmp, ".dbf");

          for (i=0; i<20; i++)
          { if (!dbase_handles[i]) continue;
            th = dbase_handles[i];
            p = strrchr(th->filename, PATHSEPCHAR);
            if (!p) continue;
            if (stricmp(p+1, tmp)==0)
            { hfield = findhashfield(th);

              for (j=0; j<th->fieldcount; j++)
              { strcpy(tmp, th->fd[j].name);
                strlwr(tmp);

                p = strstr(cols, tmp);			// match the column name
                if (!p)
	        { if (strcmp(tmp, "imagepat")==0)
                  { p = strstr(cols, "patientid");      // match alternative column names
                    if (!p) p = strstr(cols, "seriespat");
                  }
	          else if (strcmp(tmp, "seriespat")==0)
                  { p = strstr(cols, "patientid");      // match alternative column names
                    if (!p) p = strstr(cols, "imagepat");
                  }
	          else if (strcmp(tmp, "patientid")==0)
                  { p = strstr(cols, "seriespat");      // match alternative column names
                    if (!p) p = strstr(cols, "imagepat");
                  }
                }

                if (p) 
                { int instring=0;
                  n = 0; for (k=0; k<p-cols; k++)  if (cols[k]==',') n++;
                  L = strlen(Values);
                  for (k=0; k<L; k++) 
                  { if (Values[k]=='\'') instring = !instring;
                    if (Values[k]==',' && instring==0) n--;
                    if (n==0) 
                    { if (k==0) k--;
	              if (Values[k+1]==' ') k++;

                      if (OdbcUseEscapeStringConstants) 
                        if ((Values[k+1]=='E' || Values[k+1]=='e') && Values[k+2]=='\'') k++;

                      if (Values[k+1]=='\'') k++;
                      
                      strncpy(tmp, Values+k+1, 511);
                      tmp[511]=0;
                      p = strstr(tmp, "\',");
                      if (!p) p = strchr(tmp, ',');
                      if (!p) p = strchr(tmp, '\'');
                      if (p)*p=0;
                      fields[j] = strdup(tmp);
                      process_escapeValue(fields[j]);
                      break;
                    }
                  }
                }
              }

              dbase_write(th, th->reccount, fields, 0);
              UpdateIndex(th, th->reccount-1, hfield);
  
              for (i=0; i<MAXFIELDS; i++) free(fields[i]);
              return TRUE;
            }
          }
          return FALSE;
        }
	// end built-in dbf support

#ifdef POSTGRES
	if (Postgres)
	{
		sprintf ( SQLStatement, "INSERT INTO %s (%s) VALUES (%s)",
        	        Table, Columns, Values );

		if (PGSQLExec(SQLStatement) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
	        SystemDebug.printf ( "***Failed PGSQLExec : %.1000s\n", SQLStatement);
	        PrintLastError();
#endif		
		return FALSE;
	}
	
#endif

#ifdef USEMYSQL
	if (Mysql)
	{
		sprintf ( SQLStatement, "INSERT INTO %s (%s) VALUES (%s)",
        	        Table, Columns, Values );

		if (MYSQLExec(SQLStatement, FALSE) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
	        SystemDebug.printf ( "***Failed MYSQLExec : %.1000s\n", SQLStatement);
	        PrintLastError();
#endif		
		return FALSE;
	}
#endif

#ifdef USESQLITE
	if (SqLite)
	{
		sprintf ( SQLStatement, "INSERT INTO %s (%s) VALUES (%s)",
        	        Table, Columns, Values );

		if (SQLITEExec(SQLStatement, FALSE) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
	        SystemDebug.printf ( "***Failed SQLITEExec : %.1000s\n", SQLStatement);
	        PrintLastError();
#endif		
		return FALSE;
	}
#endif

#ifdef WIN32
	NewStatement();

	sprintf ( SQLStatement, "INSERT INTO %s (%s) VALUES (%s)",
		Table, Columns, Values );

	RetCode = SQLExecDirectWithRetry ( hStmt, (unsigned char *) SQLStatement, SQL_NTS );

	if ( (RetCode == SQL_SUCCESS ) ||
		 (RetCode == SQL_SUCCESS_WITH_INFO) )
		{
		return ( TRUE );
		}
#ifdef	DB_DEBUG
	SystemDebug.printf ( "***Failed SQLExecDirect : %.1000s\n", SQLStatement);
	PrintLastError();
#endif

#endif
	return ( FALSE );
	}

BOOL	Database :: DeleteRecord (	const char	*Table,
					const char	*Where )
	{

        if (DebugLevel>=4)
	{ SystemDebug.printf("Delete Record Table: %.1000s\n", Table);
          SystemDebug.printf("Where              : %.1000s\n", Where);
        }

	DatabaseDeleteRecord++;

	if (db_type == DT_NULL) 
		{
		return TRUE;
		}

	// built-in dbf support
	if (dir[0])
        { char *fields[MAXFIELDS];

	  if (NumIndexing>0) 
	  { OperatorConsole.printf("*** DeleteRecord: DbaseIII read only during indexing\n");
            return FALSE;
          }

          memset(fields, 0, sizeof(fields));
          Query(Table, "", Where, NULL);
          NextRecord();
          if (queryrecno>0) dbase_write(thq, queryrecno-1, fields, 1);
          // leave index as is
          return TRUE;
        }
	// end built-in dbf support

#ifdef POSTGRES
        if (Postgres)
        {
		if ( Where ) if ( Where[0] != '\0' ) // Add second if for DARWIN
                {
                	sprintf ( SQLStatement, "DELETE FROM %s WHERE %s",
                        	Table, Where );
                }
        	else
                {
                	sprintf ( SQLStatement, "DELETE FROM %s",
                        	Table );
                }


                if (PGSQLExec(SQLStatement) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed PGSQLExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
		return FALSE;
        }
#endif


#ifdef USEMYSQL
        if (Mysql)
        {
		if ( Where && Where[0] ) // Add second if for DARWIN
                {
                	sprintf ( SQLStatement, "DELETE FROM %s WHERE %s",
                        	Table, Where );
                }
        	else
                {
                	sprintf ( SQLStatement, "DELETE FROM %s",
                        	Table );
                }


                if (MYSQLExec(SQLStatement, FALSE) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed MYSQLExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
		return FALSE;
        }
#endif

#ifdef USESQLITE
        if (SqLite)
        {
		if ( Where && Where[0] ) // Add second if for DARWIN
                {
                	sprintf ( SQLStatement, "DELETE FROM %s WHERE %s",
                        	Table, Where );
                }
        	else
                {
                	sprintf ( SQLStatement, "DELETE FROM %s",
                        	Table );
                }


                if (SQLITEExec(SQLStatement, FALSE) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed SQLITEExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
		return FALSE;
        }
#endif

#ifdef WIN32
	NewStatement();

	if ( Where )
		{
		sprintf ( SQLStatement, "DELETE FROM %s WHERE %s",
			Table, Where );
		}
	else
		{
		sprintf ( SQLStatement, "DELETE FROM %s",
			Table );
		}

	RetCode = SQLExecDirectWithRetry ( hStmt, (unsigned char *) SQLStatement, SQL_NTS );
	if ( (RetCode == SQL_SUCCESS )||
		 (RetCode == SQL_SUCCESS_WITH_INFO) )
		{
		return ( TRUE );
		}
#ifdef	DB_DEBUG
	SystemDebug.printf ( "***Failed SQLExecDirect : %.1000s\n", SQLStatement);
	PrintLastError();
#endif

#endif
	return ( FALSE );
	}
		
									
BOOL	Database :: BindField (	UWORD	Column,
				SWORD	CTypeCode,
				void	*vPtr,
				SDWORD	vPtrLength,
				SQLLEN	*LengthNeeded	)
	{

	if (db_type == DT_NULL) 
		{
		*LengthNeeded = 4;
		return TRUE;
		}
		
	// disable non bound columns
	if (Column==1)
          for (int i=0; i<MAXFIELDS; i++)
            CTypeCodes[i]=0;

	// built-in dbf support (limited to simple single table queries)
	if (dir[0])
	{ int i;
          vPtrs[Column-1]=vPtr;
	  vPtrLengths[Column-1]=vPtrLength;
          CTypeCodes[Column-1]=CTypeCode;
          LengthNeededs[Column-1] = LengthNeeded;

          switch(CTypeCodes[Column-1])
          { case SQL_C_CHAR:  
              for (i=0; i<MAXFIELDS; i++)
              { if (querycolumns[i]<0) continue;
                if (querycolumns[i]==Column-1)
                { *LengthNeeded = thq->fd[i].len;
                  return TRUE;
                }
              }
              *LengthNeeded = 0; break;
            case SQL_C_ULONG: 
              *LengthNeeded = 4; break;
            default: return FALSE;
          }
          return TRUE;
        }
        // end built-in dbf support

#ifdef POSTGRES
	if (Postgres)
	{ 
		if ((pgres == NULL) || (PQresultStatus(pgres)!=PGRES_TUPLES_OK))
			return FALSE;

		vPtrs[Column-1]=vPtr;
		vPtrLengths[Column-1]=vPtrLength;
		CTypeCodes[Column-1]=CTypeCode;
		LengthNeededs[Column-1] = LengthNeeded;

		switch(CTypeCodes[Column-1])
		{ 
		case SQL_C_CHAR:
				*LengthNeeded = PQfmod(pgres,Column-1) -4 ;
				break;
		case SQL_C_ULONG: 
				*LengthNeeded = 4; 
				break;
		default: 
				return FALSE;
		}
		return TRUE;
	}
#endif

#ifdef USEMYSQL
	if (Mysql)
	{ 
		if ((res == NULL))
			return FALSE;

		vPtrs[Column-1]=vPtr;
		vPtrLengths[Column-1]=vPtrLength;
		CTypeCodes[Column-1]=CTypeCode;
		LengthNeededs[Column-1] = LengthNeeded;

		switch(CTypeCodes[Column-1])
		{ 
		case SQL_C_CHAR:
				*LengthNeeded = mysql_fetch_field_direct(res, Column-1)->length;
				break;
		case SQL_C_ULONG: 
				*LengthNeeded = 4; 
				break;
		default: 
				return FALSE;
		}
		return TRUE;
	}
#endif

#ifdef USESQLITE
	if (SqLite)
	{ 
		vPtrs[Column-1]=vPtr;
		vPtrLengths[Column-1]=vPtrLength;
		CTypeCodes[Column-1]=CTypeCode;
		LengthNeededs[Column-1] = LengthNeeded;

		switch(CTypeCodes[Column-1])
		{ 
		case SQL_C_CHAR:
				*LengthNeeded = 256;
				break;
		case SQL_C_ULONG: 
				*LengthNeeded = 4; 
				break;
		default: 
				return FALSE;
		}
		return TRUE;
	}
#endif

#ifdef WIN32
	if ( ! hStmt )
		return ( FALSE );
	if ( Mode == QUERY )
		{
		RetCode = SQLBindCol (	hStmt,
							Column,
							CTypeCode,
							vPtr,
							vPtrLength,
							LengthNeeded );
		if ( (RetCode != SQL_SUCCESS ) &&
			 (RetCode != SQL_SUCCESS_WITH_INFO ) )
			{
			SQLError (hEnv, hDbc, hStmt, (BYTE*)State, &NativeError,
				(BYTE*)Msg, 512, &MsgL);
#ifdef	DB_DEBUG
			SystemDebug.printf("***Failed to SQLBindCol()\n");
#endif
			return ( FALSE );
			}
		return ( TRUE );
		}
	else if ( Mode == INSERT )
		{
		return ( FALSE );
		}
	else
		{
#ifdef	DB_DEBUG
		SystemDebug.printf("***Mode not Query / Insert\n");
#endif
		return ( FALSE );
		}
#else
	return FALSE;
#endif
	}

BOOL	Database	::	NextRecord ()
	{

	DatabaseNextRecord++;

	if (db_type == DT_NULL) 
		{
		return FALSE;
		}

	// built-in dbf support
	if (dir[0])
        { char *strings[MAXFIELDS];
          int i, n, indexval=0, usehash=0;
	  char *qf[MAXFIELDS];
          int t = GetTickCount();

          if (hashfield>=0 && querymodes[hashfield]==2 && queryfields[hashfield])
            indexval = hash_id(queryfields[hashfield], 0);

          // use index for any query from record 0 that contains a index field

          if (queryrecno==0 && hashfield>=0 && queryfields[hashfield])
          { queryrecno = usehash = search_hash(queryfields[hashfield], thq->n + (querymodes[hashfield]<<8));
//OperatorConsole.printf("Hashing found record in db %s: %d\n", thq->filename, queryrecno);

	    // finish the query from index position
	    if (queryrecno)
              dbase_query(thq, &queryrecno, queryfields, querymodes, indexval);

            else
            { // perform partial query, hash it, and finish the full query
              memset(qf, 0, MAXFIELDS * sizeof(void *));
              qf[hashfield] = queryfields[hashfield];
              dbase_query(thq, &queryrecno, qf, querymodes, indexval);
              if (queryrecno>=0)
              { enter_hash(qf[hashfield], thq->n + (querymodes[hashfield]<<8), queryrecno);
                // OperatorConsole.printf("hashing entered record in db %s: %d\n", thq->filename, queryrecno);

  	        // finish the full query from the partial query
                dbase_query(thq, &queryrecno, queryfields, querymodes, indexval);
              }
            }
          }
          else
            dbase_query(thq, &queryrecno, queryfields, querymodes, indexval);

          t = GetTickCount() - t;
          QueryTime += t;

          if (t>LongQueryDBF)
          { char f[512], g[30];
            strcpy(f, "(fields:");

            for (i=0; i<MAXFIELDS; i++) 
            { if (queryfields[i])
              { switch(querymodes[i])
                { case 2:  if (hashfield==i) sprintf(g, " %d-indexed;", i);
			   else              sprintf(g, " %d-exact;",   i);
                           break;
                  case 3:  sprintf(g, " %d-substring;", i); break;
                  case 11: sprintf(g, " %d-starting;",  i); break;
                  case 16: sprintf(g, " %d->=;",        i); break;
                  case 32: sprintf(g, " %d-<=;",        i); break;
                  case 48: sprintf(g, " %d-range;",     i); break;
                  case 66: sprintf(g, " %d-multiple;",  i); break;
                  default: sprintf(g, " %d-unknown;",   i); break;
                }
                strcat(f, g);
              }
            }
            strcat(f, ")");

            if (!IndexDBF && usehash)
              OperatorConsole.printf("Hashed  query in %s %s took long: %d ms\n", thq->filename, f, t);
            else if (IndexDBF && (indexval || usehash))
              OperatorConsole.printf("Indexed query in %s %s took long: %d ms\n", thq->filename, f, t);
            else
              OperatorConsole.printf("Normal  query in %s %s took long: %d ms\n", thq->filename, f, t);
          }

          if (queryrecno<0) return FALSE;

          memset(strings, 0, sizeof(strings));
          dbase_read(thq, queryrecno, strings);
          queryrecno++;
          for (i=0; i<MAXFIELDS; i++)
          { if (!strings[i]) break;
            if (querycolumns[i]<0) continue;
            n = querycolumns[i];
            switch(CTypeCodes[n])
            { case SQL_C_CHAR:  strcpy((char *)(vPtrs[n]), strings[i]); *LengthNeededs[n] = strlen(strings[i]); break;
              case SQL_C_ULONG: *(unsigned int *)(vPtrs[n]) = atol(strings[i]); *LengthNeededs[n]=4; break;
              default: break; // ignored : error in dbsql+1, first bind row not set !!
            }
          }
	  return TRUE;
        }
        // end built-in dbf support

#ifdef POSTGRES
	if (Postgres)
	{
		int i;
		if ((pgres == NULL) || (PQresultStatus(pgres)!=PGRES_TUPLES_OK))
			return FALSE;
		if (current_tuple >= ntuples) //already processed last record
			return FALSE;
		for(i=0;i<ncolumns;i++)
		{
			switch (CTypeCodes[i])
			{
			case SQL_C_CHAR:
				strcpy((char *)(vPtrs[i]),PQgetvalue(pgres,current_tuple,i));
				*LengthNeededs[i] = PQgetlength(pgres,current_tuple,i);
				break;
			case SQL_C_ULONG:
				*(unsigned int *)(vPtrs[i]) = atol(PQgetvalue(pgres,current_tuple,i));
				*LengthNeededs[i] = 4;
				break;
			default:
				break;
			}
		}
		current_tuple++;
		return TRUE;
	}
#endif

#ifdef USEMYSQL
	if (Mysql)
	{
		int i;
		MYSQL_ROW	row ;
		unsigned long *lengths;

		if ((res == NULL))
			return FALSE;
		if (current_tuple >= ntuples) //already processed last record
			return FALSE;
		row = mysql_fetch_row( res );
		if (!row)
			return FALSE;

		lengths = mysql_fetch_lengths(res);

		for(i=0;i<ncolumns;i++)
		{
		if(row[i]) //Steini 20050507
			switch (CTypeCodes[i])
			{
			case SQL_C_CHAR:
				strcpy((char *)(vPtrs[i]),row[i]);
				*LengthNeededs[i] = lengths[i]; // strlen(row[i]);
				break;
			case SQL_C_ULONG:
				*(unsigned int *)(vPtrs[i]) = atol(row[i]);
				*LengthNeededs[i] = 4;
				break;
			default:
				break;
			}
		else
			switch (CTypeCodes[i])
			{
			case SQL_C_CHAR:
				strcpy((char *)(vPtrs[i]),"");
				*LengthNeededs[i] = 0;
				break;
			case SQL_C_ULONG:
				*(unsigned int *)(vPtrs[i]) = 0;
				*LengthNeededs[i] = 4;
				break;
			default:
				break;
			}
		}
		current_tuple++;
		return TRUE;
	}
#endif

#ifdef USESQLITE
	if (SqLite)
	{
		int i;
		unsigned long *lengths;

		if ((resultp == NULL))
			return FALSE;
		if (current_tuple >= ntuples) //already processed last record
			{
			if (resultp) sqlite3_free_table(resultp);
			resultp = NULL;
			return FALSE;
			}

		for(i=0;i<ncolumns;i++)
		{	switch (CTypeCodes[i])
			{
			case SQL_C_CHAR:
			        if (resultp[(current_tuple+1)*ncolumns+i])
				{ strncpy((char *)(vPtrs[i]), resultp[(current_tuple+1)*ncolumns+i], vPtrLengths[i]);
				  ((char *)(vPtrs[i]))[vPtrLengths[i]-1]=0;
				}
				else
				  ((char *)(vPtrs[i]))[0] = 0;
				*LengthNeededs[i] = strlen((char *)(vPtrs[i]));
				break;
			case SQL_C_ULONG:
			        if (resultp[(current_tuple+1)*ncolumns+i])
				  *(unsigned int *)(vPtrs[i]) = atol(resultp[(current_tuple+1)*ncolumns+i]);
				else
				  *(unsigned int *)(vPtrs[i]) = 0;
				*LengthNeededs[i] = 4;
				break;
			default:
				break;
			}
		}
		current_tuple++;
		return TRUE;
	}
#endif

#ifdef WIN32
	if ( ! hStmt )
		return ( FALSE );
	RetCode = SQLFetch ( hStmt );
	
	if ((RetCode != SQL_SUCCESS ) &&
		(RetCode != SQL_SUCCESS_WITH_INFO ) )
		{
		SQLError (hEnv, hDbc, hStmt, (BYTE*)State, &NativeError,
			(BYTE*)Msg, 512, &MsgL);
#ifdef	DB_DEBUG
//		SystemDebug.printf("Failed to SQLFetch() : %d\n", RetCode);
//		(This is not a real failure, just reached last record)
#endif
//		PrintLastError();
		return ( FALSE );
		}
	return ( TRUE );
#else
	return ( FALSE );
#endif
	}

BOOL	Database :: CreateTable (
	const char	*Table,
	const char	*Columns )
	{

        if (DebugLevel>=4)
	{ SystemDebug.printf("Create Table: %.1000s\n", Table);
          SystemDebug.printf("Columns     : %.1000s\n", Columns);
        }

	DatabaseCreateTable++;

	if (db_type == DT_NULL) 
		{
		return TRUE;
		}

	// built-in dbf support
	if (dir[0])
	{ char filename[1024];
          char temp[8192];
          char *fielddata[MAXFIELDS];
          char *p, *q, *r;
          int i, f=0, n;

          strcpy(temp, Columns);
          memset(fielddata, 0, sizeof(fielddata));
                 
          p = temp;
          while (p)
          { q = strchr(p, ',');
            if (q) *q=0;
            fielddata[f] = (char *)malloc(256);
            r = p; while(*r==' ')r++; strncpy(fielddata[f], r, 255); fielddata[f][255]=0;
            r = strstr(fielddata[f], " int");
            if (r)
            { strcpy(r, ":C10");
            }
            else
            { r = strchr(fielddata[f], '(');
              if (r) n = atoi(r+1); else n = 255;

              r = strstr(fielddata[f], " varchar");
              if (!r) r = strstr(fielddata[f], " char");
              if (r) *r = 0;
              sprintf(r, ":C%d", n);
            }

            if (q) p = q+1; else p = NULL;
            f++;
          }

          strcpy(filename, dir);
          strcat(filename, Table);
          strcat(filename, ".DBF");

          for(i=0; i<20; i++)
          { if (dbase_handles[i])
            { if (stricmp(filename, dbase_handles[i]->filename)==0)
              { dbase_close(dbase_handles[i]); dbase_handles[i]=NULL;
                dbase_create(filename, fielddata, &dbase_handles[i]);
                for (int j=0; j<MAXFIELDS; j++) free(fielddata[j]);
//                if (Indices[i])				/* clear in-memory index if any */
//                  IndicesCount[i]=0;
                return TRUE;
              }
            }
          }
          for(i=0; i<20; i++)
          { if (!dbase_handles[i])
            { dbase_create(filename, fielddata, &dbase_handles[i]);
              for (int j=0; j<MAXFIELDS; j++) free(fielddata[j]);
              dbase_handles[i]->n = i;
//              if (Indices[i])				/* clear in-memory index if any */
//                IndicesCount[i]=0;
              return TRUE;
            }
          }
          return FALSE;
        }
	// end built-in dbf support

#ifdef POSTGRES
        if (Postgres)
        {
		sprintf (SQLStatement, "CREATE TABLE  %s (%s)",
	                Table, Columns );


                if (PGSQLExec(SQLStatement) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed PGSQLExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif


#ifdef USEMYSQL
        if (Mysql)
        {
		sprintf (SQLStatement, "CREATE TABLE  %s (%s)",
	                Table, Columns );

                if (MYSQLExec(SQLStatement, FALSE) == 0)
                        return TRUE;

#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed MYSQLExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif

#ifdef USESQLITE
        if (SqLite)
        {
		sprintf (SQLStatement, "CREATE TABLE  %s (%s)",
	                Table, Columns );

                if (SQLITEExec(SQLStatement, FALSE) == 0)
                        return TRUE;

#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed SQLITEExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif

#ifdef WIN32
	NewStatement();
	sprintf (SQLStatement, "CREATE TABLE  %s (%s)",
		Table, Columns );

	RetCode = SQLExecDirectWithRetry ( hStmt, (unsigned char *) SQLStatement, SQL_NTS );

	if ( (RetCode == SQL_SUCCESS) ||
		 (RetCode == SQL_SUCCESS_WITH_INFO) )
		{
		return ( TRUE );
		}
#ifdef	DB_DEBUG
	SystemDebug.printf ( "***Failed SQLExecDirect : %.1000s\n", SQLStatement);
	PrintLastError();
#endif

#endif
	return ( FALSE );
	}

BOOL	Database :: CreateIndex (
	const char	*Table,
	const char	*Index,
	const char	*Column )
	{

        if (DebugLevel>=4)
 	{ SystemDebug.printf("Create Index Table: %.1000s\n", Table);
          SystemDebug.printf("Index             : %.1000s\n", Index);
          SystemDebug.printf("Column            : %.1000s\n", Column);
        }

	if (db_type == DT_NULL) 
		{
		return TRUE;
		}

        // ignored for built-in dbf support
	if (dir[0])
          return TRUE;
	// end built-in dbf support


#ifdef POSTGRES
        if (Postgres)
        {
		sprintf (SQLStatement, "CREATE INDEX %s ON %s (%s)",
	                Index, Table, Column );


                if (PGSQLExec(SQLStatement) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed PGSQLExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif

#ifdef USEMYSQL
        if (Mysql)
        {
		sprintf (SQLStatement, "CREATE INDEX %s ON %s (%s)",
	                Index, Table, Column );


                if (MYSQLExec(SQLStatement, FALSE) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed MYSQLExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif

#ifdef USESQLITE
        if (SqLite)
        {
		sprintf (SQLStatement, "CREATE INDEX %s ON %s (%s)",
	                Index, Table, Column );

                if (SQLITEExec(SQLStatement, FALSE) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed SQLITEExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif

#ifdef WIN32
	NewStatement();
	sprintf (SQLStatement, "CREATE INDEX %s ON %s (%s)",
		Index, Table, Column );

	RetCode = SQLExecDirectWithRetry ( hStmt, (unsigned char *) SQLStatement, SQL_NTS );

	if ( (RetCode == SQL_SUCCESS) ||
		 (RetCode == SQL_SUCCESS_WITH_INFO) )
		{
		return ( TRUE );
		}
#ifdef	DB_DEBUG
	SystemDebug.printf ( "***Failed SQLExecDirect : %.1000s\n", SQLStatement);
	PrintLastError();
#endif

#endif
	return ( FALSE );
	}

BOOL	Database :: CreateUniqueIndex (
	char	*Table,
	char	*Index,
	const char	*Column )
	{

        if (DebugLevel>=4)
 	{ SystemDebug.printf("Create Unique Index Table: %.1000s\n", Table);
          SystemDebug.printf("Index                    : %.1000s\n", Index);
          SystemDebug.printf("Column                   : %.1000s\n", Column);
        }

        // ignored for built-in dbf support
	if (dir[0])
          return TRUE;
	// end built-in dbf support

#ifdef POSTGRES
        if (Postgres)
        {
		sprintf (SQLStatement, "CREATE UNIQUE INDEX %s ON %s (%s)",
                	Index, Table, Column );

                if (PGSQLExec(SQLStatement) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed PGSQLExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif

#ifdef USEMYSQL
        if (Mysql)
        {
		sprintf (SQLStatement, "CREATE UNIQUE INDEX %s ON %s (%s)",
                	Index, Table, Column );

                if (MYSQLExec(SQLStatement, FALSE) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed MYSQLExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif

#ifdef USESQLITE
        if (SqLite)
        {
		sprintf (SQLStatement, "CREATE UNIQUE INDEX %s ON %s (%s)",
                	Index, Table, Column );

                if (SQLITEExec(SQLStatement, FALSE) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed SQLITEExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif


#ifdef WIN32
	NewStatement();
	sprintf (SQLStatement, "CREATE UNIQUE INDEX %s ON %s (%s)",
		Index, Table, Column );

	RetCode = SQLExecDirectWithRetry ( hStmt, (unsigned char *) SQLStatement, SQL_NTS );

	if ( (RetCode == SQL_SUCCESS) ||
		 (RetCode == SQL_SUCCESS_WITH_INFO) )
		{
		return ( TRUE );
		}
#ifdef	DB_DEBUG
	SystemDebug.printf ( "***Failed SQLExecDirect : %.1000s\n", SQLStatement);
	PrintLastError();
#endif

#endif
	return ( FALSE );
	}

BOOL	Database :: DeleteTable (
	const char	*Table )
	{

	if (db_type == DT_NULL) 
		{
		return TRUE;
		}

	// built-in dbf support
	if (dir[0])
		{
//		file may be locked by several open DB
//		char filename[1024];
//		strcpy(filename, dir);
//	        strcat(filename, Table);
//        	strcat(filename, ".DBF");
//		unlink(filename);
//		strcpy(filename, dir);
//	        strcat(filename, Table);
//        	strcat(filename, ".dbf");
//		unlink(filename);
		return TRUE;
		}

	// end built-in dbf support

#ifdef POSTGRES
        if (Postgres)
        {
		sprintf(SQLStatement, "DROP TABLE %s",
	                Table);

                if (PGSQLExec(SQLStatement) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed PGSQLExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif

#ifdef USEMYSQL
        if (Mysql)
        {
		sprintf(SQLStatement, "DROP TABLE %s",
	                Table);

                if (MYSQLExec(SQLStatement, FALSE) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed MYSQLExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif

#ifdef USESQLITE
        if (SqLite)
        {
		sprintf(SQLStatement, "DROP TABLE %s",
	                Table);

                if (SQLITEExec(SQLStatement, FALSE) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed SQLITEExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif

#ifdef WIN32
	NewStatement();
	sprintf(SQLStatement, "DROP TABLE %s",
		Table);
	RetCode = SQLExecDirectWithRetry ( hStmt, (unsigned char *) SQLStatement, SQL_NTS );

	if ( (RetCode == SQL_SUCCESS) ||
		 (RetCode == SQL_SUCCESS_WITH_INFO) )
		{
		return ( TRUE );
		}
#ifdef	DB_DEBUG
	SystemDebug.printf ( "***Failed SQLExecDirect : %.1000s\n", SQLStatement);
	PrintLastError();
#endif

#endif
	return ( FALSE );
	}

BOOL	Database :: Exec (
	const char	*SQLStatement )
	{

	if (db_type == DT_NULL) 
		{
		return TRUE;
		}

	// built-in dbf support
	if (dir[0])
		{
		return FALSE;
		}


#ifdef POSTGRES
        if (Postgres)
        {
                if (PGSQLExec((char *)SQLStatement) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed PGSQLExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif

#ifdef USEMYSQL
        if (Mysql)
        {
                if (MYSQLExec((char *)SQLStatement, FALSE) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed MYSQLExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif

#ifdef USESQLITE
        if (SqLite)
        {
                if (SQLITEExec((char *)SQLStatement, FALSE) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed SQLITEExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif

#ifdef WIN32
	NewStatement();
	RetCode = SQLExecDirectWithRetry ( hStmt, (unsigned char *) SQLStatement, SQL_NTS );

	if ( (RetCode == SQL_SUCCESS) ||
		 (RetCode == SQL_SUCCESS_WITH_INFO) )
		{
		return ( TRUE );
		}
#ifdef	DB_DEBUG
	SystemDebug.printf ( "***Failed SQLExecDirect : %.1000s\n", SQLStatement);
	PrintLastError();
#endif

#endif
	return ( FALSE );
	}

BOOL	Database	::	UpdateRecords(
	char	*TableName,
	char	*Updates,
	const char	*Where)
	{
        if (Where==NULL) return FALSE;
        if (strlen(Where)==0) return FALSE;

        if (DebugLevel>=4)
 	{ SystemDebug.printf("Update Table: %.1000s\n", TableName);
          SystemDebug.printf("Updates     : %.1000s\n", Updates);
          SystemDebug.printf("Where       : %.1000s\n", Where);
        }

	DatabaseUpdateRecords++;

	if (db_type == DT_NULL) 
		{
		return TRUE;
		}

	// built-in dbf support
	if (dir[0])
        { char *fields[MAXFIELDS];
		  int j;//, k;
          char tmp[512];
          char upds[8192];
          char lupds[8192];
          char *p;
	  int hfield=-1;

	  if (NumIndexing>0) 
	  { OperatorConsole.printf("*** UpdateRecords: DbaseIII read only during indexing\n");
            return FALSE;
          }

          memset(fields, 0, sizeof(fields));
          strcpy(upds, Updates);
          strcpy(lupds, Updates);
          strlwr(lupds);

          Query(TableName, "", Where, NULL);
          NextRecord();
          if (queryrecno>0) 
          { hfield = findhashfield(thq);

            for (j=0; j<thq->fieldcount; j++)
            { strcpy(tmp, thq->fd[j].name);
              strlwr(tmp);

              p = strstr(lupds, tmp);		// match the fieldname
              if (!p)
	      { if (strcmp(tmp, "imagepat")==0)
                { p = strstr(lupds, "patientid");    // match alternative fieldnames
                  if (!p) p = strstr(lupds, "seriespat");
                }
                else if (strcmp(tmp, "seriespat")==0)
                { p = strstr(lupds, "patientid");    // match alternative fieldnames
                  if (!p) p = strstr(lupds, "imagepat");
                }
                else if (strcmp(tmp, "patientid")==0)
                { p = strstr(lupds, "seriespat");    // match alternative fieldnames
                  if (!p) p = strstr(lupds, "imagepat");
                }
              }

              if (!p) continue;

              strncpy(tmp, upds + (p-lupds) + strlen(tmp), 511); // get clause for this field
              tmp[511]=0;

              p = strstr(tmp, "\', ");		//    clause ends with ',
              if (p==NULL) p = strstr(tmp, ", ");	// or clause ends with ,
              if (p==NULL) p = strrchr(tmp, '\'');	// or clause ends with '
              if (p) *p=0;

              p = strstr(tmp, " = ");		// value after ' = '
              if (!p) continue;
              p = p+3;

              if (OdbcUseEscapeStringConstants) 
		if ((p[0]=='E' || p[0]=='e') && p[1]=='\'') p++;

              if (*p=='\'') p++;
              fields[j] = strdup(p);
              process_escapeValue(fields[j]);
            }
          }

          while (queryrecno>0) 
          // if (queryrecno>=0) 
          { dbase_write(thq, queryrecno-1, fields, 0);
            UpdateIndex(thq, queryrecno-1, hfield);
            NextRecord();
          }
          for (j=0; j<MAXFIELDS; j++) free(fields[j]);
          return TRUE;
        }
	// end built-in dbf support

#ifdef POSTGRES
        if (Postgres)
        {
		sprintf(SQLStatement, "UPDATE %s SET %s WHERE %s",
                	TableName, Updates, Where);

                if (PGSQLExec(SQLStatement) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed PGSQLExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif

#ifdef USEMYSQL
        if (Mysql)
        {
		sprintf(SQLStatement, "UPDATE %s SET %s WHERE %s",
                	TableName, Updates, Where);

                if (MYSQLExec(SQLStatement, FALSE) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed MYSQLExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif

#ifdef USESQLITE
        if (SqLite)
        {
		sprintf(SQLStatement, "UPDATE %s SET %s WHERE %s",
                	TableName, Updates, Where);

                if (SQLITEExec(SQLStatement, FALSE) == 0)
                        return TRUE;
#ifdef  DB_DEBUG
                SystemDebug.printf ( "***Failed SQLITEExec : %.1000s\n", SQLStatement);
                PrintLastError();
#endif
                return FALSE;
        }
#endif

#ifdef WIN32
	NewStatement();
	sprintf(SQLStatement, "UPDATE %s SET %s WHERE %s",
		TableName, Updates, Where);

	RetCode = SQLExecDirectWithRetry ( hStmt, (unsigned char *) SQLStatement, SQL_NTS );
	if ( (RetCode == SQL_SUCCESS) ||
		 (RetCode == SQL_SUCCESS_WITH_INFO) )
		{
		return ( TRUE );
		}
#ifdef	DB_DEBUG
	SystemDebug.printf ( "***Failed SQLExecDirect : %.1000s\n", SQLStatement);
	PrintLastError();
#endif

#endif
	return ( FALSE );
	}

BOOL	Database	::	ExtractRecords(
	const char	*TableName,
	const char	*Where,
	char	*Filename)
	{
	struct	dbase_header *xhq;
	int	*records = (int *)malloc(100000 * sizeof(int));
 	int	i, count = 0;

        if (Where==NULL)
		{
		free(records);
		return FALSE;
		}

        if (strlen(Where)==0)
		{
		free(records);
		return FALSE;
		}

	// only for built-in dbf support
	if (!dir[0]) 
		{
		free(records);
		return FALSE;
		}

//	if (db_type == DT_NULL) 
//		{
//		free(records);
//		return TRUE;
//		}

        if (DebugLevel>=4)
 	{ SystemDebug.printf("Extract Records: %.1000s\n", TableName);
          SystemDebug.printf("Where          : %.1000s\n", Where);
          SystemDebug.printf("Filename       : %.1000s\n", Filename);
        }

        char *fields[MAXFIELDS];
//        int j;

        memset(fields, 0, sizeof(fields));

        Query(TableName, "", Where, NULL);
        NextRecord();

        xhq = dbase_dup(Filename, thq);
        if (xhq==NULL) return FALSE;

        while (queryrecno>0 && count<100000) 
        { records[count++] = queryrecno-1;
          NextRecord();
        }

        for (i=0; i<count; i++)
        { dbase_read(thq, records[i], fields);
          dbase_write(xhq, i, fields, 0);
        }

        dbase_close(xhq);
        free(records);
        return TRUE;
	}

BOOL	Database :: PrintLastError ()
	{
//	SQLCHAR		cState;
//	SQLINTEGER	iNativeError;

	/* The 'old' odbc interface */
//	SQLError (hEnv, hDbc, hStmt, (BYTE*)State, &NativeError,
//		(BYTE*)Msg, 512, &MsgL);
	/* The 3.x odbc interface */
//	SQLGetDiagRec(
//		SQL_HANDLE_STMT,//SQLSMALLINT HandleType,
//		hStmt,		//SQLHANDLE Handle,
//		1,		//SQLSMALLINT RecNumber,
//		&cState,	//SQLCHAR *Sqlstate,
//		&iNativeError,	//SQLINTEGER *NativeErrorPtr,
//		(BYTE*)Msg,		//SQLCHAR *MessageText,
//		512,		//SQLSMALLINTBufferLength,
//		&MsgL);		//SQLSMALLINT *TextLengthPtr);
//	OperatorConsole.printf("***Error: %d: %c: %s\n", iNativeError, cState, Msg);

	/* SQLError is already called, so don't do it again.
	   Maybe these errors and the statements that caused them should be
	   printed to the operator console.
	*/

	if (db_type == DT_NULL) 
		{
		SystemDebug.printf("***Error: I fell into a black hole\n");
		return TRUE;
		}

#ifdef POSTGRES
	if (Postgres)
	{
		SystemDebug.printf("***Error: %s\n",PQresultErrorMessage(pgres));
		return TRUE;
	}
#endif

#ifdef USEMYSQL
	if (Mysql)
	{
		SystemDebug.printf("***Error: %s\n", mysql_error(mydata));
		return TRUE;
	}
#endif

#ifdef USESQLITE
	if (SqLite)
	{
		return TRUE;
	}
#endif

#ifdef WIN32
	SystemDebug.printf("***Error: %d: %s: %s\n", NativeError, State, Msg);
#else
	SystemDebug.printf("***DB error\n");
#endif
	return ( TRUE );
	}

SDWORD	Database :: GetNativeError ()
	{
#ifdef WIN32
	return (NativeError);
#else
	return 0;
#endif
	}


BOOL	Database :: CreateDatabase (
	char	*Name,
	char	*User,
	char	*lPassword )// Made Password local
	{

        if (DebugLevel>=4)
 	{ SystemDebug.printf("Create Database: %s, user: %s, password: %s\n", Name, User, lPassword);
        }

        // ignored for built-in dbf support
	if (dir[0])
          return TRUE;
	// end built-in dbf support


	if (db_type == DT_NULL) 
		{
		return TRUE;
		}

#ifdef POSTGRES
        if (Postgres)
        	{
		sprintf (SQLStatement, "CREATE ROLE %s LOGIN PASSWORD '%s' VALID UNTIL 'infinity'", User, lPassword);
                if (PGSQLExec(SQLStatement))
			{ 
			SystemDebug.printf ( "***Failed PGSQLExec : %.1000s\n", SQLStatement);
	                PrintLastError();
                	//return FALSE;
			}
		sprintf (SQLStatement, "CREATE DATABASE %s OWNER %s", Name, User);
                if (PGSQLExec(SQLStatement))
			{ 
			SystemDebug.printf ( "***Failed PGSQLExec : %.1000s\n", SQLStatement);
	                PrintLastError();
                	return FALSE;
			}
		return TRUE;
        	}
#endif

#ifdef USEMYSQL
        if (Mysql)
        	{
		sprintf (SQLStatement, "CREATE DATABASE %s", Name);
                if (MYSQLExec(SQLStatement, FALSE))
			{ 
			SystemDebug.printf ( "***Failed MYSQLExec : %.1000s\n", SQLStatement);
	                PrintLastError();
                	return FALSE;
			}
		return TRUE;
        	}
#endif

#ifdef WIN32
	int i;
	NewStatement();
	sprintf (SQLStatement, "CREATE DATABASE %s", Name);

	i = SQLExecDirectWithRetry ( hStmt, (unsigned char *) SQLStatement, SQL_NTS );
        if (i != SQL_SUCCESS_WITH_INFO && i != SQL_SUCCESS)
		{ 
		SystemDebug.printf ( "***Failed SQLExecDirect : %.1000s\n", SQLStatement);
                PrintLastError();
               	return FALSE;
		}
	NewStatement();
	sprintf (SQLStatement, "exec sp_addlogin '%s','%s','%s'", User, lPassword, Name);
	i = SQLExecDirectWithRetry ( hStmt, (unsigned char *) SQLStatement, SQL_NTS );
        if (i != SQL_SUCCESS_WITH_INFO && i != SQL_SUCCESS)
		{ 
		SystemDebug.printf ( "***Failed SQLExecDirect : %.1000s\n", SQLStatement);
                PrintLastError();
               	return FALSE;
		}
	NewStatement();
	sprintf (SQLStatement, "use %s", Name);
	i = SQLExecDirectWithRetry ( hStmt, (unsigned char *) SQLStatement, SQL_NTS );
        if (i != SQL_SUCCESS_WITH_INFO && i != SQL_SUCCESS)
		{ 
		SystemDebug.printf ( "***Failed SQLExecDirect : %.1000s\n", SQLStatement);
                PrintLastError();
               	return FALSE;
		}
	sprintf (SQLStatement, "exec sp_changedbowner '%s'", User);
	i = SQLExecDirectWithRetry ( hStmt, (unsigned char *) SQLStatement, SQL_NTS );
        if (i != SQL_SUCCESS_WITH_INFO && i != SQL_SUCCESS)
		{ 
		SystemDebug.printf ( "***Failed SQLExecDirect : %.1000s\n", SQLStatement);
		PrintLastError();
                return FALSE;
		}
		return TRUE;
#else
		return FALSE;// Added for warning gcc4.2 bcb
#endif
	}


#ifdef	DB_MAIN

int
main()
	{

	Database	DB;
	SDWORD		DBNameLength;
	char		DBName [ 128 ];
		
	if (!DB.Open ("LABRAD2", "Administrator", "password", "localhost" ))
		{
		printf ("***Could not open datasource\n");
		return (1);
		}
/*f(!DB.QueryTables ( NULL, NULL, NULL, NULL ))
		{
		printf("Could not QueryTables\n");
		return(2);
		}
		*/
	if(!DB.Query ( "DCMGate", "PatientName" ))
		{
		printf("***Could not perform Query\n");
		return(2);
		}		
	if(!DB.BindField (1, SQL_C_CHAR, &DBName, 128, &DBNameLength))
		{
		printf("***Could not bind field\n");
		return ( 4 );
		}
	
	while(DB.NextRecord () )
		{
		printf("Database: %s\n", DBName);
		}
		
	sprintf(DBName, "PatientName = \"Test Record #2\"");
	//DB.Add("DCMGate", "PatientName, ObjectFile", DBName);
	DB.Delete("DCMGate", DBName);
	DB.PrintLastError();
	DB.Close();
	return ( 0 );
	}
#endif

