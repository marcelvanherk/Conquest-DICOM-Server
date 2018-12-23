#ifndef	_DGATE_H_
#	define	_DGATE_H_

/*
MvH 19980705: Added CACHEDevices and JUKEBOXDevices, FindPhysicalDevice
ljz 19990108: Added LoadImplicitLittleEndianFile and NKI PrivateCompession algorithms
MvH 19990109: Regen has extra parameter
ljz 19990317: Changed parameters of LoadImplicitLittleEndianFile
ljz 19991117: Added parameter FileCompressMode to prototype of nki_private_compress
ljz 20000629: Added TroubleLogFile and UserLogFile
mvh 20010415: Added KeepImages flag to RemoveFromPacs: clear from DB only
              Added SubDir parameter to regen to allow regen of one directory only
mvh 20010416: Added ChangeUID routine and RegenFile - to allow modification of images
mvh 20010429: Changed decompressor: now has extra parameter
mvh 20020529: InitializeTables now has mode parameter (0=normal, 1=simple)
mvh 20021016: added patid to GetFilename routines
mvh 20021017: Added NeedPack here
mvh 20021018: GenerateFileName has NoKill option (for interactive dgate tasks)
ljz 20030120: Added prototype of FreeDeviceTables
mvh 20030701: compression parameter for amap
mvh 20030703: Added prototypes of recompression functions + ArchiveCompression
mvh 20030706: Optional filename parameter for recompress
mvh 20030706: Export VRType for implicit little endian support
mvh 20030921: Added DEVICE_TYPE_MIRROR
mvh 20040401: Added Changed and ActualMode flags to compress routines
mvh 20040626: Added study and series UID to getfilename interface
mvh 20040930: Adapted return type of SetString; added maxlen to BuildSearchString
mvh 20041013: Added MAXQUERYLENGTH
mvh 20041029: Added MergeUIDs
mvh 20041108: Added Syntax input to GenerateFileName
mvh 20050108: Adapted for linux compile
mvh 20050109: Added configurable TCPIPTimeOut
mvh 20050129: Added optional FILE to DumpVR, added CheckFreeStoreOnMIRRORDevice
mvh 20050401: Added QueryOnModalityWorkList, WorkListTableName, WorkListDB
mvh 20050404: Added DT_START/ENDSEQUENCE to code sequence in WorkList table
mvh 20050902: Made space for HL7Tag in DBEntry
mvh 20051229: Added iDepth to DumpVR
mvh 20060317: Added called and calling to GenerateFilename
mvh 20060324: Added StripGroup2 option to recompress
mvh 20060628: AddToDatabase has JustAdd parameter: skip one unnecessary query
mvh 20060702: Pass DB to GenerateFilename to avoid zillions of db open and closes
mvh 20070122: Added MIRRORDevices
mvh 20070201: Added DebugLevel
mvh 20070207: Added MakeTableString
mvh 20080818: DbaseIII check now uses DB flags, not PATHSEP in datasource name
              Added DT_MSTR
jf  20090616: Include file stuff
bcb 20091231: Changed char* to const char* for gcc4.2 warnings
mvh 20100111: Merged
mvh 20100123: Added DT_FL and DT_FD
mvh 20100125: Removed linux warning
bcb 20100309: Changed SQLLength to unsigned int
mvh 20100703: Merged
mvh 20110119: Moved two functions to the correct place
mvh 20160108: Updated ChangeUID calls to support staged anonymization
mvh 20160219: Removed BOOL TRUE and FALSE defines
mvh 20171122: Added DT_ISTR (case insensitive query string)
*/

//#define bool BOOL
//#define true TRUE
//#define false FALSE

#ifndef	WHEDGE
#	include	"dicom.hpp"	// DICOM routines
#	include	<stdlib.h>
#ifndef UNIX
#	include	<direct.h>
#else // UNIX
#include "wintypes.hpp"	// windows data types
#endif // UNIX
#	include	"odbci.hpp"	// ODBC Interface routines
#	include	"dprintf.hpp"	// Debug output
#endif

#ifndef	UNIX
#	define	PATHSEPCHAR	'\\'
#else
#	define	PATHSEPCHAR	'/'
#endif

#define	MAXQUERYLENGTH 310000

typedef	struct
	{
	UINT16		Group;
	UINT16		Element;
	char		SQLColumn[32];
	unsigned int 	SQLLength;
	int		SQLType;
	int		DICOMType;
	char		HL7Tag[32];
	}	DBENTRY;

typedef	struct	_ACRNemaAddress
	{
		char	Name[20];
		char	IP[128];
		char	Port[16];
		char	Compress[16];
	}	ACRNemaAddress;

enum
	{
	DT_STR	=1,
	DT_DATE,
	DT_UINT16,
	DT_UINT32,
	DT_UI,
	DT_FL,
	DT_FD,
	DT_STARTSEQUENCE,
	DT_ENDSEQUENCE,
	DT_MSTR,
	DT_ISTR
	};

extern	UINT	DebugVRs;
extern	int	DebugLevel;
extern	DBENTRY	*PatientDB, *StudyDB, *SeriesDB, *ImageDB, *WorkListDB;
extern	char	PatientTableName[],
		StudyTableName[],
		SeriesTableName[],
		ImageTableName[],
		WorkListTableName[];
extern	BYTE	Directory [];
extern	BYTE	Port [];
extern	BYTE	KFACTORFILE [];
extern	BYTE	MYACRNEMA [];
extern	char	SOPClassFile [];
extern	char	OCPipeName[];
extern	char	ACPipeName[];
extern	char	ServerName[];
extern  char	UserLogFile[];
extern  char	TroubleLogFile[];
extern	BYTE	ACRNEMAMAP[];
// DataHost added for UNIX version
// holds network hostname of machine running SQL server
extern	char	DataHost[];
extern	char	DataSource[];
extern	char	UserName[];
extern	char	Password[];
extern  int 	NeedPack;
extern  int 	TCPIPTimeOut;

extern  RTC	VRType;


BOOL	CheckOrMakeTable(Database	&);
BOOL	DropTables();
BOOL	MakeSafeString ( VR *, char	*);
BOOL	RemoveDuplicates ( Database &, DICOMDataObject *);
BOOL	FixImage(DICOMDataObject	*);
BOOL	LoadKFactorFile(char	*);
BOOL	InitializeTables (int mode);
BOOL	QueryOnPatient (DICOMDataObject *, Array < DICOMDataObject *> *);
BOOL	QueryOnStudy (DICOMDataObject *, Array < DICOMDataObject *> *);
BOOL	QueryOnSeries (DICOMDataObject *, Array < DICOMDataObject *> *);
BOOL	QueryOnImage (DICOMDataObject *, Array < DICOMDataObject *> *);
BOOL	QueryOnModalityWorkList (DICOMDataObject *, Array < DICOMDataObject *> *);
char *	SetString(VR	*vr, char	*, int);
BOOL	DumpVR(VR	*, FILE * f = NULL, int iDepth = 0);
BOOL	GenerateFileName(DICOMDataObject*, char *Dev, char *Root, char *Filename, BOOL NoKill, int Syntax, char *called, char *calling, Database *db);

DICOMDataObject*	LoadDICOMDataObject(char	*filename);
DICOMDataObject*	LoadDICOMDataObjectFrame(char	*filename);
DICOMDataObject*	LoadImplicitLittleEndianFile(char* filename,
	unsigned int iVrSizeLimit);
int nki_private_decompress(short int *dest, signed char *src, int size);
int nki_private_compress(signed char  *dest, short int  *src, int npixels, int iMode);
int get_nki_private_decompressed_length(signed char *src);

BOOL GenUID(char	*oString);

BOOL CompressNKIImageFile(char *file, int FileCompressMode, int *ActualMode);
BOOL CompressJPEGImageFile(char *file, int FileCompressMode, int *ActualMode);
BOOL DecompressImageFile(char *file, int *Changed);
BOOL DecompressImage(DICOMDataObject **pDDO, int *Changed);
BOOL CompressJPEGImage(DICOMDataObject **pDDO, int FileCompressMode, int *ActualMode);

extern char ArchiveCompression[16];

BOOL	SaveDICOMDataObject(DICOMDataObject *, char	*filename, int FileCompressMode);
BOOL	SaveDICOMDataObjectC10(DICOMDataObject	*DDOPtr, char	*filename, int FileCompressMode);
BOOL	GetFileName(VR	*, char *, char *, BOOL UpdateLRU = TRUE, char *patid=NULL, char *study=NULL, char *series=NULL);
BOOL	GetFileName(VR	*, char *, char *, Database &, BOOL UpdateLRU = TRUE, char *patid=NULL, char *study=NULL, char *series=NULL);
BOOL	InitACRNemaAddressArray();
BOOL	CloseACRNemaAddressArray();
BOOL	GetACRNema(char *, char *, char *, char*);
BOOL	RemoveFromWorld(DICOMDataObject *, Database &);
BOOL	RemoveFromPACS(DICOMDataObject *, BOOL KeepImages = FALSE);
BOOL	BuildColumnString(DBENTRY	*DBE, char	*TableName, VR	*vr,	char	*TempString);
BOOL	BuildSearchString(Database *DB, DBENTRY	*DBE, char	*TableName, VR	*vr, char	*Search, char	*TempString, int maxlen);
BOOL	SaveToDataBase (Database &DB, DICOMDataObject	*, char	*filename, const char *Device, BOOL JustAdd);
BOOL	CheckOrMakeTable(Database	&);
BOOL	DropTables();
BOOL	MakeSafeString ( VR *, char	*);
BOOL	MakeSafeDate ( VR *, char *);
BOOL	RemoveDuplicates ( Database &, DBENTRY *, char	*, DICOMDataObject *, BOOL);
BOOL	FixImage(DICOMDataObject	*);
DBENTRY	*FindDBE(VR*);
UINT	LastDBE(DBENTRY*);
BOOL	VerifyIsInDBE( VR	*vr, DBENTRY	*DBE, DBENTRY	* &TempDBEPtr );
UINT	DBEIndex(DBENTRY	*DBE, VR	*vr);
BOOL	DICOM2SQLQuery ( char	*s );
const char	*UniqueKey(DBENTRY*);
const char	*UniqueLink(DBENTRY*);
BOOL	UpdateAccessTimes(Database &, char *);
BOOL	ChangeUID(char *OldUID, const char *Type, char *NewUID, char *Stage);
BOOL	MergeUIDs(char *OldUID[], int n, const char *Type, char *NewUID);
UINT32	SQLRealSize(char	*Str, SDWORD	Max);
BOOL	Regen();
BOOL	Regen(const char*, int IsCacheOrJukeBox, char *SubDir = NULL);
BOOL	RegenFile(char *filename);
BOOL	AddToTable(
	Database		&DB,
	DBENTRY			*DCMGateDB,
	char			*TableName,
	DICOMDataObject	*DDOPtr,
	char			*ObjectFile,
	char			*DeviceName);
BOOL	MakeTableString ( DBENTRY	*DBE, char	*s, int mode );

DICOMDataObject	*MakeCopy(DICOMDataObject	*DO);
VR		*ConstructVRFromSQL (
			DBENTRY	*,
			UINT16	Group,
			UINT16	Element,
			UINT32	Length,
			char	*);
VR		*ConstructAE();

// Type of System
#	define	E_WORKGROUP		1
#	define	E_PERSONAL		2
#	define	E_ENTERPRISE	3

extern	UINT	Edition;
extern	char	EditionText[];


// Device related flags
extern	UINT		MAGDevices, MOPDevices, CDRDevices, CACHEDevices, JUKEBOXDevices, MIRRORDevices;
extern	UINT		MAGDeviceThreshHold;



#	define	DEVICE_TYPE_MAG		0x01
#	define	DEVICE_TYPE_MOP		0x02
#	define	DEVICE_TYPE_CDR		0x04
#	define	DEVICE_TYPE_MIRROR	0x08
#	define	DEVICE_TYPE_NOKILL	0x100

#	define	DEVICE_OPTI			0x00
#	define	DEVICE_0			0x10
#	define	DEVICE_1			0x20
#	define	DEVICE_2			0x30
#	define	DEVICE_X(xxx)		((xxx+1)<<4)

#ifdef UNIX
//DUCKHEAD92 BEGIN
#define stricmp(s1, s2) strcasecmp(s1, s2)
#define memicmp(s1, s2, n) strncasecmp(s1, s2, n)

/* These next lines are scattered all around the place, 
   should they be WIN32 only? */
#define O_BINARY 0              /* LINE UNIX ONLY? */
#define _SH_DENYRW      0x10    /* deny read/write mode */
#define _SH_DENYWR      0x20    /* deny write mode */
#define _SH_DENYRD      0x30    /* deny read mode */
#define _SH_DENYNO      0x40    /* deny none mode */
#define SH_DENYNO       _SH_DENYNO
#define sopen(a, b, c, d) open(a, b) /* LINE UNIX ONLY? */
#define WINAPI /* LINE UNIX ONLY */
#define DeleteCriticalSection(a) pthread_mutex_destroy(a);  /* LINE UNIX ONLY */
void strupr(char *s);
void strlwr(char *s);
#define	closesocket(xxx)	close(xxx) /* LINE UNIX ONLY? */
#define strnicmp(s1, s2, n) strncasecmp(s1, s2, n)
//DUCKHEAD92 END
#endif

BOOL	InitializeDeviceTable(char	*SCRoot);
void	FreeDeviceTables(void);
BOOL	GetDevice(char	*Dev, UINT	Flags);
BOOL	GetPhysicalDevice(const char	*Dev, char	*Name);
BOOL	FindPhysicalDevice(char	*Dev, char	*Name, char	*ObjectFile);
UINT	CheckFreeStoreOnMAGDevice(UINT);
UINT	CheckFreeStoreOnCACHEDevice(UINT);
UINT	CheckFreeStoreOnMIRRORDevice(UINT);
UINT	LargestFreeMAG ();
UINT	MaxMagDevice ();

BOOL	ConfigMicroPACS();
DWORD	CurrentTime();
BOOL	ConfigEventNotification ( char	*ConfigSection, char	*ConfigFile );

#endif
