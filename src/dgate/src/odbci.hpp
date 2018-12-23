/*
19990827   mvh   Added CreateIndex
20001128   ljz   Added CRITICAL_SECTION member of 'Database'
20020415   mvh   Added error info, GetNativeError, SQLExecDirectWithRetry and CreateUniqueIndex
20021020   mvh   Added built-in dbf support (for stable operation without ODBC)
                 Note: requires de-normalized database
20030120   ljz   Ignore Micro$oft complaint C4200
20030321   ljz   Enlarged buffer for SqlErrorMessages (Database->Msg)
20030819   mvh   Allow longer filenames
20040528   mvh   Added ExtractRecords (dbase III only)
20040530   mvh   Added InternalQuery (for converting normal to indexed queries)
20040605   mvh   Made lockstart __int64
20040612   mvh   Made lockstart in again - now pure record#; added TotalRec, MBExtra
20041003   mvh   Statement length set to 310 kb (see dbsql.cpp for explanation)
20041013   mvh   Malloc SQLStatement instead
20050109   mvh   Adapted for linux compile
20050905   mvh   Postgres code by Piotr Filipczuk checked by mvh, will not affect other operations
20060219   mvh   Start on native mysql driver
20060220   mvh   Include all required mysql defines here
20060226   mvh   Runtime load MYSQL in WIN32
20060628   mvh   Use mysql.h proper (for datastructures) even when runtime loading (sorry for the hard include path)
20060701   mvh   Added mysql_fetch_lengths
20070206   mvh   Added CreateDatabase
20070218   mvh   Added SqLite
20070307   mvh   Made database type local, added Database(type) creator
20070705   mvh   Added DT_DBASEIIINOINDEX: disables indexing and packing
		 Extented max number of dbf files to 20
20071115   mvh   Static mysql use <mysql.h>
20071118   mvh   Adapted for 64 bits (use SQLLEN for BindField)
20080107   mvh   Removed double declaration of SQLLEN
20080821   mvh   Fix for SUNWspro compiler
20081116   mvh   Adapted for 64 bits linux (avoid use of "long")
20081117   mvh   Allow compile with both POSTGRES and USEMYSQL
20081119   mvh   added run-time loading of postgres for WIN32
20081120   mvh   Finished it
20081124   mvh   Added DT_NULL
20090114   mvh   Increased MAXFIELDS to 100 for clone_xvi
20091231   bcb   Changed char* to const char* for gcc4.2 warnings
20100111   mvh   Merged
20100619   bcb   Added no-copy to the Database class.
20100717   mvh   Merged
20110502   mvh   Added db:Exec
20140629   mvh   Added access to last error functions for mysql and pqsql
*/



// ODBC Interface class.
#ifndef	_ODBCI_H_
#	define	_ODBCI_H_

#ifdef WIN32
#	include	<stdio.h>
#	include	<windows.h>
#	include	<sql.h>
#	include	<sqlext.h>
#else
#	include	"wintypes.hpp"
#	include	"pthread.h"
#	define CRITICAL_SECTION pthread_mutex_t
#endif

#ifdef POSTGRES
#	include "libpq-fe.h"
#endif

/////////////////////////////////////////////////////////////////////////

#ifdef USEMYSQL

#ifdef WIN32			   /* load mysql dynamically */
extern "C"
{
#include "C:\mysql-5.0.22-win32\include\mysql.h"

  typedef void (STDCALL *_mysql_close)(MYSQL *sock);
  typedef MYSQL *(STDCALL *_mysql_init)(MYSQL *mysql);
  typedef MYSQL *(STDCALL *_mysql_real_connect)(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db,
  					   unsigned int port, const char *unix_socket, unsigned long clientflag);
  typedef int (STDCALL *_mysql_select_db)(MYSQL *mysql, const char *db);
  typedef void (STDCALL *_mysql_free_result)(MYSQL_RES *result);
  typedef int (STDCALL *_mysql_query)(MYSQL *mysql, const char *q);
  typedef MYSQL_RES *(STDCALL *_mysql_store_result)(MYSQL *mysql);
  typedef my_ulonglong (STDCALL *_mysql_num_rows)(MYSQL_RES *res);
  typedef unsigned int (STDCALL *_mysql_num_fields)(MYSQL_RES *res);
  typedef MYSQL_ROW (STDCALL *_mysql_fetch_row)(MYSQL_RES *result);
  typedef const char * (STDCALL *_mysql_error)(MYSQL *mysql);
  typedef MYSQL_FIELD *(STDCALL *_mysql_fetch_field_direct)(MYSQL_RES *res, unsigned int fieldnr);
  typedef unsigned long *(STDCALL  *_mysql_fetch_lengths)(MYSQL_RES *result);
  typedef unsigned int *(STDCALL *_mysql_errno)(MYSQL *mysql);
  
  extern _mysql_close __mysql_close;
  extern _mysql_init __mysql_init;
  extern _mysql_real_connect __mysql_real_connect;
  extern _mysql_select_db __mysql_select_db;
  extern _mysql_free_result __mysql_free_result;
  extern _mysql_query __mysql_query;
  extern _mysql_store_result __mysql_store_result;
  extern _mysql_num_rows __mysql_num_rows;
  extern _mysql_num_fields __mysql_num_fields;
  extern _mysql_fetch_row __mysql_fetch_row;
  extern _mysql_error __mysql_error;
  extern _mysql_fetch_field_direct __mysql_fetch_field_direct;
  extern _mysql_fetch_lengths __mysql_fetch_lengths;
  extern _mysql_errno __mysql_errno;

#define mysql_close              __mysql_close
#define mysql_init               __mysql_init
#define mysql_real_connect       __mysql_real_connect
#define mysql_select_db          __mysql_select_db
#define mysql_free_result        __mysql_free_result
#define mysql_query              __mysql_query
#define mysql_store_result       __mysql_store_result
#define mysql_num_rows           __mysql_num_rows
#define mysql_num_fields         __mysql_num_fields
#define mysql_fetch_row          __mysql_fetch_row
#define mysql_error              __mysql_error
#define mysql_fetch_field_direct __mysql_fetch_field_direct
#define mysql_fetch_lengths      __mysql_fetch_lengths
#define mysql_errno              __mysql_errno
}
#else			/* load mysql in a static way */
# include <mysql.h>
#endif

#endif

#ifdef POSTGRES

#ifdef WIN32			   /* load postgres dynamically */
extern "C"
{
#define PGCALL 
typedef void (PGCALL *_PQfinish)(PGconn *conn);
typedef ConnStatusType (PGCALL *_PQstatus)(const PGconn *conn);
typedef PGconn *(PGCALL *_PQsetdbLogin)(const char *pghost, const char *pgport, const char *pgoptions, const char *pgtty,
			 const char *dbName, const char *login, const char *pwd);
typedef void (PGCALL *_PQclear)(PGresult *res);
typedef PGresult *(PGCALL *_PQexec)(PGconn *conn, const char *query);
typedef ExecStatusType (PGCALL *_PQresultStatus)(const PGresult *res);
typedef int	(PGCALL *_PQntuples)(const PGresult *res);
typedef int	(PGCALL *_PQnfields)(const PGresult *res);
typedef int	(PGCALL *_PQfmod)(const PGresult *res, int field_num);
typedef char *(PGCALL *_PQgetvalue)(const PGresult *res, int tup_num, int field_num);
typedef int	(PGCALL *_PQgetlength)(const PGresult *res, int tup_num, int field_num);
typedef char *(PGCALL *_PQresultErrorMessage)(const PGresult *res);
typedef char *(PGCALL *_PQerrorMessage)(const PGconn *con);
typedef char *(PGCALL *_PQresultErrorField)(const PGresult *res, int fieldcode);
}

extern _PQfinish               __PQfinish;            
extern _PQstatus               __PQstatus;            
extern _PQsetdbLogin           __PQsetdbLogin;        
extern _PQclear                __PQclear;             
extern _PQexec                 __PQexec;              
extern _PQresultStatus         __PQresultStatus;      
extern _PQntuples              __PQntuples;           
extern _PQnfields              __PQnfields;           
extern _PQfmod                 __PQfmod;              
extern _PQgetvalue             __PQgetvalue;          
extern _PQgetlength            __PQgetlength;         
extern _PQresultErrorMessage   __PQresultErrorMessage;
extern _PQerrorMessage         __PQerrorMessage;
extern _PQresultErrorField     __PQresultErrorField;

#define PQfinish              __PQfinish            
#define PQstatus              __PQstatus            
#define PQsetdbLogin          __PQsetdbLogin        
#define PQclear               __PQclear             
#define PQexec                __PQexec              
#define PQresultStatus        __PQresultStatus      
#define PQntuples             __PQntuples           
#define PQnfields             __PQnfields           
#define PQfmod                __PQfmod              
#define PQgetvalue            __PQgetvalue          
#define PQgetlength           __PQgetlength
#define PQresultErrorMessage  __PQresultErrorMessage
#define PQerrorMessage        __PQerrorMessage
#define PQresultErrorField    __PQresultErrorField

#endif
#endif

/////////////////////////////////////////////////////////////////////////

// for built-in dbase support

/* The array size as it must be declared for a flexible array member of a struct. */
#ifdef SOLARIS
# define DECLARED_FLEXIBLE_ARRAY_SIZE  1
#else
#ifdef __GNUC__
# define DECLARED_FLEXIBLE_ARRAY_SIZE  0
#else
# define DECLARED_FLEXIBLE_ARRAY_SIZE  /* as nothing */
#ifdef _MSC_VER
/*warning C4200: nonstandard extension used : zero-sized array in ...*/
#pragma warning( disable : 4200 )
#endif
#endif
#endif

struct field_descriptor
{ char          name[11];              /* should be zero terminated */
  char          type;
  char          junk1[4];
  unsigned char len;
  unsigned char deccount;
  char          junk2[14];
};

struct dbase_header
{ int           filehandle;             /* info calculated at dbopen */
  int           currentrecord;
  int           recordlength;
  int           headerlength;
  int           fieldcount;
  int           touched;
  int		lockstart;              /* added for file locking */
  int           lockcount;              /* lockcount = 0 for unlocked */
  int           sharemode;              /* 1 if file opened for share */
  int           readonly;               /* 1 if file opened read-only */
  char          *recordbuffer;          /* added for record buffering */
  int           bufrecord;              /* record number in buffer */
  char          *rawbuffer;
  int           rawrecord;              /* idem in raw buffer */
  char          filename[1024];         /* check for duplicate opens */
  int           n;			/* dbase number used for hashing */
  int		TotalRec;		/* total records to divide MBExtra */
  int		MBExtra;		/* extra MB allocated for index */

  char          junk1[4];               /* Dbase III file header format */
  int           reccount;
  short int     hlength;                /* contains header/record length */
  short int     rlength;
  char          junk2[4];
  int           updatecount;            /* multiuser ext. moved on 01-09-95 */
  char          junk3[8];               /* should contain zeros */
  int           oldupdatecount;         /* old MBI30 multiuser extension */
  struct field_descriptor fd[DECLARED_FLEXIBLE_ARRAY_SIZE];
                                        /* we limit ourselves to 32 fields */
};

enum actmode {QUERY, EXTRACT};
#define MAXFIELDS 100

enum DBTYPE {DT_ODBC, DT_DBASEIII, DT_POSTGRES, DT_MYSQL, DT_SQLITE, DT_DBASEIIINOINDEX, DT_NULL};

// end for built-in dbase support

#ifndef _WIN64
#ifndef SQLLEN
#define SQLLEN SDWORD
#endif
#endif

class	Database
	{
	public:

	enum DBTYPE db_type;

#ifdef WIN32
	HENV	hEnv;
	HDBC	hDbc;
	HSTMT	hStmt;
	UINT	Mode;
	SWORD	MsgL;
	SDWORD	NativeError;
	char	State[256];
	char	Msg [512];
#endif

#ifdef POSTGRES
	PGconn  *conn;
	PGresult *pgres;
#endif

#ifdef USEMYSQL
	MYSQL  *mydata;
	MYSQL_RES *res;
#endif

#ifdef USESQLITE
	struct sqlite3 *sqlitedata;
//	void *sqlitedata;
	char **resultp;
#endif

#ifdef POSTGRES
	int Postgres;
#endif
#ifdef USEMYSQL
	int Mysql;
#endif
#ifdef USESQLITE
	int SqLite;
#endif

#if defined(POSTGRES) || defined(USEMYSQL) || defined (USESQLITE)
	int ncolumns;
	int ntuples;
	int current_tuple;
#endif

	BOOL	Connected;

	// for built-in dbase support
        struct 	dbase_header *dbase_handles[20];
	int 	querymodes[MAXFIELDS];
	char	*queryfields[MAXFIELDS];
	struct	dbase_header *thq;
        int 	queryrecno;
	int	querycolumns[MAXFIELDS];
	void	*vPtrs[MAXFIELDS];
	SWORD	CTypeCodes[MAXFIELDS];
	SDWORD	vPtrLengths[MAXFIELDS];
	SQLLEN	*LengthNeededs[MAXFIELDS];
        char	dir[1024];
        int     hashfield;
	// end for built-in dbase support

	private:
		BOOL	NewStatement ();
		SDWORD	GetNativeError ();

	public:
		CRITICAL_SECTION	m_Critical;

#ifdef WIN32
		RETCODE	RetCode;
#endif
		char	*SQLStatement;
		
				Database();
				Database(enum DBTYPE db_type);

		virtual		~Database();

		virtual	BOOL	Open (	const char *Database, const char	*User, const char	*Password,
								const char *Server = NULL);
								// Server used in Sybase version, not here
		virtual	BOOL	Close ();
		virtual	BOOL	QueryTables (	char	*SearchTableQualifier,
								char	*SearchTableOwner,
								char	*SearchTableName,
								char	*SearchTableType );
								
		virtual	BOOL	BindField (		UWORD	Column,
								SWORD	CTypeCode,
								void	*vPtr,
								SDWORD	vPtrLength,
								SQLLEN	*LengthNeeded	);
		virtual	BOOL	NextRecord ();
		virtual	BOOL	PrintLastError ();
#ifdef WIN32		
		virtual int 	SQLExecDirectWithRetry(SQLHSTMT StatementHandle, SQLCHAR *StatementText, SQLINTEGER TextLength);
#endif
#ifdef POSTGRES
		virtual int	PGSQLExec(char *StatementText);
#endif
#ifdef USEMYSQL
		virtual int	MYSQLExec(char *StatementText, BOOL q);
#endif
#ifdef USESQLITE
		virtual int	SQLITEExec(char *StatementText, BOOL q);
#endif
		virtual	BOOL	Query (	const char	*Table,
						const char	*Columns,
						const char	*Where = NULL ,
						const char	*Order = NULL );
		virtual	BOOL	InternalQuery (	const char	*Table,
						const char	*Columns,
						const char	*Where = NULL ,
						const char	*Order = NULL ,
						BOOL	recurse = FALSE);
		virtual	BOOL	QueryDistinct (	char	*Table,
						const char	*Columns,
						const char	*Where = NULL ,
						const char	*Order = NULL );
		virtual	BOOL	DeleteRecord ( const char	*Table,
						 const char	*Where );
		virtual	BOOL	AddRecord (	const char	*Table,
						const char	*Columns,
						const char	*Values );
		virtual	BOOL	DeleteTable ( const char	*Table );
		virtual	BOOL	Exec        ( const char	*SQLStatement );
		virtual	BOOL	CreateTable ( const char	*TableName, const char	*Columns );
		virtual	BOOL	CreateIndex ( const char	*Table,	const char	*Index,	const char	*Column );
		virtual	BOOL	CreateUniqueIndex ( char	*Table,	char	*Index,	const char	*Column );
		virtual	BOOL	UpdateRecords ( char	*TableName,
						char	*Updates,
						const char	*Where);
		virtual	BOOL	ExtractRecords( const char	*TableName,
						const char	*Where,
						char	*Filename);
		virtual	BOOL	CreateDatabase (char	*Name,
						char	*User,
						char	*Password);
#ifdef __GNUC__
	private:// This will prevent it from being copied (it has pointers)
		Database(const Database&);
		const	Database & operator = (const Database&);
#endif
	};


#endif
