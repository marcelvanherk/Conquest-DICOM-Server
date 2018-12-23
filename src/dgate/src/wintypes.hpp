// 20050111	mvh 	added SWORD and UWORD

#ifndef WINTYPES_HPP
#define WINTYPES_HPP
#ifdef UNIX

// Windows datatypes not defined in UNIX environment
typedef unsigned long DWORD;
typedef long SDWORD;
typedef short SWORD;
typedef unsigned short UWORD;

// some of the possible ODBC datatypes

typedef enum
{
	SQL_C_NONE,
	SQL_C_CHAR,
	SQL_C_SSHORT,
	SQL_C_USHORT,
	SQL_C_SLONG,
	SQL_C_ULONG,
	SQL_C_FLOAT,
	SQL_C_DOUBLE,
	SQL_C_DATE

} ODBC_datatype;

#endif
#endif
