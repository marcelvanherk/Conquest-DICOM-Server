/* 19980321 Mvh Added define TRUNCATEFIELDNAMES for DBF compatibity (apply in makefile) */
/* 19980325 Mvh Added TruncateFieldNames entry in dicom.ini to allow DBF compatibility	*/
/* 19980327 Mvh Added MaxFieldLength entry in dicom.ini to allow DBF compatibility	*/
/* 20001105 mvh Renamed some enums and variables to allow compile in one big chunk      */
/* 20010713 mvh Renamed DT_UINTxx to DT_INTxx as temp fix for bug in LEX.CPP            */
/* 20010829 mvh Undone above change as LEX.CPP has been fixed by ljz			*/
/* 20050107 mvh Added include file gpps for GetPrivateProfileString			*/ 
/* 20050401 mvh Added WorkList Support							*/
/* mvh 20050404: Added DT_START/ENDSEQUENCE to code sequence in WorkList table          */
/* mvh 20050902: Added HL7 column to be used for WorkList table                         */
/* mvh 20050908: Fixed pointer error that crashed linux version                         */
/* mvh 20080818: Added DT_MSTR                                                          */
/* mvh 20100122: Hex2Dec incorrect for uppercase HEX numbers!				*/
/* mvh 20100123: Added FL and FD types							*/
/* mvh 20100125: GetPrivateProfileString -> MyGetPrivateProfileString			*/
/* bcb 20100309: Changed int to unsigned. (gcc4.2 Warnings) 				*/
/* mvh 20100703: Merged					 				*/
/* mvh 20171122: Added DT_ISTR (case insensitive query string) 				*/

#ifndef	WHEDGE
#	include	"dgate.hpp"
#	include	"lex.hpp"
#else
#	include	"master.h"
#endif

#include "gpps.hpp"

enum
	{
	TK_SCOMMENT_ = 1,
	TK_ECOMMENT_,
	TK_PATIENT,
	TK_STUDY,
	TK_SERIES,
	TK_IMAGE,
	TK_WORKLIST,
	TK_BOPEN,
	TK_BCLOSE,
	TK_COMMA,
	TK_QT,
	TK_SQL_C_CHAR,
	TK_SQL_C_DATE,
	TK_DT_STR,
	TK_DT_MSTR,
	TK_DT_ISTR,
	TK_DT_DATE,
	TK_DT_UINT16,
	TK_DT_UINT32,
	TK_DT_TIME,
	TK_DT_UI,
	TK_DT_FL,
	TK_DT_FD,
	TK_DT_STARTSEQUENCE,
	TK_DT_ENDSEQUENCE
	};
	
TOKENSTRUCT	G	[]	=
	{
		{	TK_SCOMMENT_,	"/*"},
		{	TK_ECOMMENT_,	"*/"  },
		{	TK_PATIENT,	"*Patient*" },
		{	TK_STUDY,	"*Study*"	},
		{	TK_SERIES,	"*Series*" },
		{	TK_IMAGE,	"*Image*" },
		{	TK_WORKLIST,	"*WorkList*" },
		{	TK_BOPEN,	"{" },
		{	TK_BCLOSE,	"}"	},
		{	TK_COMMA,	","	},
		{	TK_QT,		"\"" },
		{	TK_SQL_C_CHAR,	"SQL_C_CHAR" },
		{	TK_SQL_C_DATE,	"SQL_C_DATE" },
		{	TK_DT_STR,		"DT_STR" },
		{	TK_DT_MSTR,		"DT_MSTR" },
		{	TK_DT_ISTR,		"DT_ISTR" },
		{	TK_DT_DATE,		"DT_DATE"	},
		{	TK_DT_UINT16,		"DT_UINT16"	},
		{	TK_DT_UINT32,		"DT_UINT32"	},
		{	TK_DT_TIME,		"DT_TIME"	},
		{	TK_DT_UI,		"DT_UI"	},
		{	TK_DT_FL,		"DT_FL"	},
		{	TK_DT_FD,		"DT_FD"	},
		{	TK_DT_STARTSEQUENCE,	"DT_STARTSEQUENCE"	},
		{	TK_DT_ENDSEQUENCE,	"DT_ENDSEQUENCE"	},
		{	0,	"\0" }
	};


static	BOOL	Parse (	Lex	&,
						Array < DBENTRY > *,
						Array < DBENTRY > *,
						Array < DBENTRY > *,
						Array < DBENTRY > *,
						Array < DBENTRY > *
						);
BOOL	ParseComment ( Lex & );
static	BOOL	ParseDB ( Lex &, Array < DBENTRY > *, BOOL HasHL7);
static	UINT16	ConvertNumber ( char	*);
static	BOOL	ConvertDB(DBENTRY	**, Array < DBENTRY > * );

#ifndef	RootConfig
extern	char	RootConfig[];
#endif
#ifndef	ConfigFile
extern	char	ConfigFile[];
#endif

static int	TruncateFieldNamesP=0;
static unsigned int	MaxFieldLengthP=0;

static
void
GetDBSpecials(void)
	{
	char		RootSC[64];
	char		Temp[128];

	MyGetPrivateProfileString ( RootConfig, "MicroPACS", RootConfig,
		(char*) RootSC, 64, ConfigFile);

	MyGetPrivateProfileString ( RootSC, "TruncateFieldNames", "0",
		(char*) Temp, 128, ConfigFile);
	TruncateFieldNamesP = atoi(Temp);

	MyGetPrivateProfileString ( RootSC, "MaxFieldLength", "0",
		(char*) Temp, 128, ConfigFile);
	MaxFieldLengthP = atoi(Temp);
	}

BOOL
LoadKFactorFile(char	*KFile)
	{
	Lex	Lexxer;
	Array < DBENTRY >	ADBPatient, ADBStudy, ADBSeries, ADBImage, ADBWorkList;
	
	GetDBSpecials();

	if ( !Lexxer.Start(KFile, G) )
		return ( FALSE );

	if(!Parse ( Lexxer, &ADBPatient, &ADBStudy, &ADBSeries, &ADBImage, &ADBWorkList ))
		return ( FALSE );

	ConvertDB(&PatientDB, &ADBPatient);
	ConvertDB(&StudyDB, &ADBStudy);
	ConvertDB(&SeriesDB, &ADBSeries);
	ConvertDB(&ImageDB, &ADBImage);
	ConvertDB(&WorkListDB, &ADBWorkList);

	return ( TRUE );
	
	}

static
BOOL
ConvertDB(	DBENTRY	**ppDBE, Array < DBENTRY >	*ADB)
	{
	UINT	Index;
	DBENTRY	DBE;

	if(*ppDBE)
		free(*ppDBE);

	(*ppDBE) = (DBENTRY *)malloc(sizeof(DBENTRY) * (ADB->GetSize() + 2));

	Index = 0;
	while(Index < ADB->GetSize() )
		{
		(*ppDBE) [Index] = ADB->Get(Index);
		++Index;
		}
	DBE.Group = 0;
	DBE.Element = 0;
	memcpy((void *) (&(*ppDBE)[Index]), (void*) &DBE, sizeof(DBENTRY));
	return ( TRUE );
	}


static
BOOL	Parse (
	Lex	&Lexxer,
	Array < DBENTRY >	*ADBPatient,
	Array < DBENTRY >	*ADBStudy,
	Array < DBENTRY >	*ADBSeries,
	Array < DBENTRY >	*ADBImage,
	Array < DBENTRY >	*ADBWorkList
	)
	{
	TK		Tk;

	Lexxer.Get(&Tk);

	while ( TRUE )
		{
		switch ( Lexxer.Peek(&Tk) )
			{
			case	TK_SCOMMENT_:
				ParseComment (Lexxer);
				break;
			case	TK_PATIENT:
				ParseDB(Lexxer, ADBPatient, FALSE);
				break;
			case	TK_STUDY:
				ParseDB(Lexxer, ADBStudy, FALSE);
				break;
			case	TK_SERIES:
				ParseDB(Lexxer, ADBSeries, FALSE);
				break;
			case	TK_IMAGE:
				ParseDB(Lexxer, ADBImage, FALSE);
				break;
			case	TK_WORKLIST:
				ParseDB(Lexxer, ADBWorkList, TRUE);
				break;
			case	TOKEN_END:
				return ( TRUE );
			default:
				fprintf ( stderr, "Parse error : Parse : %s\n",
					Tk.Str);
				return ( FALSE );
			}
		Lexxer.Get(&Tk);
		}
	return ( FALSE );
	}

BOOL
ParseComment ( Lex 	&Lexxer )
	{
	TK	Tk;

	while ( Lexxer.Get(&Tk) != TK_ECOMMENT_ )
		{
		if(Tk.Value == TOKEN_END)
			return ( FALSE );
		}
	return ( TRUE );
	}

static
BOOL
ParseDB(Lex &Lexxer, Array < DBENTRY > *DBName, BOOL HasHL7)
	{
	TK	Tk, TKGroup, TKElement, TKColumn, TKLength, TKSQLType, TKDICOMType, TKHL7Tag;
	DBENTRY	DBE;

	Lexxer.Get(&Tk);
	if(Tk.Value!=TK_BOPEN)
		{
		fprintf(stderr, "Expected: Database  { : %s\n", Tk.Str);
		exit(1);
		return ( FALSE );
		}
	Lexxer.Get(&Tk);
	while ( Tk.Value == TK_BOPEN )
		{
		Lexxer.Get(&TKGroup);
		Lexxer.Get(&Tk);
		Lexxer.Get(&TKElement);
		Lexxer.Get(&Tk);
		Lexxer.Get(&Tk);
		Lexxer.Get(&TKColumn);
		Lexxer.Get(&Tk);
		Lexxer.Get(&Tk);
		Lexxer.Get(&TKLength);
		Lexxer.Get(&Tk);
		Lexxer.Get(&TKSQLType);
		Lexxer.Get(&Tk);
		Lexxer.Get(&TKDICOMType);
		Lexxer.Get(&Tk);
		if(HasHL7 && Tk.Value==TK_COMMA)
			{
			Lexxer.Get(&Tk);
			Lexxer.Get(&TKHL7Tag);
			Lexxer.Get(&Tk);
			Lexxer.Get(&Tk);
			}
		if(Tk.Value!=TK_BCLOSE)
			{
			fprintf(stderr, "Expected { Group, Element, Column, Length, SQLType, DICOMType }\n");
			fprintf(stderr, "Got { %s, %s, %s, %s, %s, %s }\n",
					TKGroup.Str, TKElement.Str, TKColumn.Str,
					TKLength.Str, TKSQLType.Str, TKDICOMType.Str);


			exit(1);
			return ( FALSE );
			}
		Lexxer.Get(&Tk);
		if(Tk.Value ==  TK_COMMA)
			Lexxer.Get(&Tk);
		
		DBE.Group = ConvertNumber(TKGroup.Str);
		DBE.Element = ConvertNumber(TKElement.Str);
		strcpy(DBE.SQLColumn, TKColumn.Str);
		if (HasHL7) strcpy(DBE.HL7Tag, TKHL7Tag.Str); else DBE.HL7Tag[0]=0;

		if (TruncateFieldNamesP)
                	DBE.SQLColumn[TruncateFieldNamesP] = 0;

		DBE.SQLLength = ConvertNumber(TKLength.Str);

		if (MaxFieldLengthP)
			if (DBE.SQLLength >= MaxFieldLengthP) DBE.SQLLength = MaxFieldLengthP;

		if(TKSQLType.Value == TK_SQL_C_CHAR)
			DBE.SQLType = SQL_C_CHAR;
		else
		if(TKSQLType.Value == TK_SQL_C_DATE)
			DBE.SQLType = SQL_C_DATE;
		else
			{
			fprintf(stderr, "Unknown SQL Type: %s\n", TKSQLType.Str);
			exit(1);
			}
		if(TKDICOMType.Value == TK_DT_STR)
			DBE.DICOMType = DT_STR;
		else
		if(TKDICOMType.Value == TK_DT_MSTR)
			DBE.DICOMType = DT_MSTR;
		else
		if(TKDICOMType.Value == TK_DT_ISTR)
			DBE.DICOMType = DT_ISTR;
		else
		if(TKDICOMType.Value == TK_DT_UI)
			DBE.DICOMType = DT_UI;
		else
		if(TKDICOMType.Value == TK_DT_DATE)
			DBE.DICOMType = DT_DATE;
		else
		if(TKDICOMType.Value == TK_DT_UINT16)
			DBE.DICOMType = DT_UINT16;
		else
		if(TKDICOMType.Value == TK_DT_UINT32)
			DBE.DICOMType = DT_UINT32;
		else
		if(TKDICOMType.Value == TK_DT_TIME)
			DBE.DICOMType = DT_DATE;	// yes, intentional
		else
		if(TKDICOMType.Value == TK_DT_FL)
			DBE.DICOMType = DT_FL;		
		else
		if(TKDICOMType.Value == TK_DT_FD)
			DBE.DICOMType = DT_FD;
		else
		if(TKDICOMType.Value == TK_DT_STARTSEQUENCE)
			DBE.DICOMType = DT_STARTSEQUENCE;
		else
		if(TKDICOMType.Value == TK_DT_ENDSEQUENCE)
			DBE.DICOMType = DT_ENDSEQUENCE;
		else
			{
			fprintf(stderr, "Unknown DICOM Type: %s\n", TKDICOMType.Str);
			exit(1);
			}
		DBName->Add(DBE);
		}
	return ( TRUE );
	}

static
UINT16
Hex2Dec(char	ch)
	{
	if((ch>='a')&&(ch<='f'))
		return(ch-'a'+10);
	if((ch>='A')&&(ch<='F'))
		return(ch-'A'+10);
	if((ch>='0')&&(ch<='9'))
		return(ch-'0'+0);
	return ( 0 );
	}

static
UINT16
ConvertNumber(char	*s)
	{
	UINT16	Value;
	int		Index;

	if(strlen(s)>2)
		{
		if((s[1] == 'x')||(s[1] == 'X'))
			{
			Value = 0;
			Index = 2;
			while(s[Index])
				{
				Value = (Value << 4) + Hex2Dec(s[Index]);
				++Index;
				}
			return ( Value );
			}
		}
	return((UINT16)atoi(s));
	}
