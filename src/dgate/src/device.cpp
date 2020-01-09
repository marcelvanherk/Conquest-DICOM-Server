/*
19980620	mvh	Setting MAGDeviceThreshHold to 0 causes out of disk error below 10 MB without cleanup
19980620	mvh	Implemented PanicKillOff thread here as alternative to spawning killer.exe
19980702	mvh	Started support functions for maintaining/determining amount of used space on device
19980704	mvh	NOTE: JukeBox code only TruncateFieldNames compatible for now; used mp's filecopy
19980709	mvh	Finished first version of archive options: prepare,undo,copy,compare,delete
19980721	mvh	Set default # cache and jukebox devices to 0
19990110	mvh	Made strings longer since got truncated, fixed some messages
19990110	mvh	Archive one MAG device or MAG?; fixed SQL syntax for update; speeded 0x7fffffff
			return False if SelectLRUForarchival selects no data
19990111	mvh	Reset archive flag reports to operator console; protected against illegal device#
19990113	mvh	Run up to 10 patient further to try and fill CD more exact; max patients = 10000
19990114	mvh	Added quick routine MakeListOfPatientsOnDevice and used instead of 0x7fffffff
19990117	mvh	Accept any, not only *.v2, for computing patient size, assume max file size=30 MB
19990630	mvh	Added support for MIRROR devices (e.g., try MIRROR1 if MAG1 fails)
19990707	mvh	Fixed default MIRRORDevices (now 0)
19990708	mvh	NOTE: archiving should make use of mirror device if required as well!
19990712	mvh	Fixed that MagRampage thread was started over and over again (fixed for WIN32 only)
19990831	ljz	Fix: Several BOOL functions returned -1, which is TRUE !!
20000221	ljz	Changed layout of this update-history
20000629	ljz	Logging of trouble now starts with '***'
20001104        mvh     Fixed aDB.Close -> should be aDB.Close(); added WINAPI to MagRampage thread
20010328	mvh	Added verify mirror disk
20010329        mvh     DeleteBunchAfterBurning also deletes data on MIRROR0 if it exists
			verify mirror disk keeps on going after error but does return error
20010312        mvh     Check if we already selected some patients for archiving from the device
20010509        mvh     Added RenameDevice
20010522      mvh+ljz   Do not call RestoreMagFlags if something goes in wrong in PrepareBunchForBurning
20011114      ljz+mvh   Replaced slow updates for magdevice by much faster ones
                        The update that now no longer has the IN part might not work for ACCESS or DBF
                        So: for jukebox purposes, SQL server is required
20011115      ljz+mvh   Replaced 'LIKE' by faster '=' in query strings; fixed previous fix
20020412        mvh     PanicKilloff will delete now delete 5 MB data at a time. (high water = low water + 5)
20020415        mvh     Added TestImages: sanity check of images on disk
20020416        mvh     Reversed read order for TestImages and VerifyMirrorDisk: MRU first
20020802	mvh	Allow multiple subdirectories in rmdir and mkdir code
20020804        mvh     Cleaned up a bit
20021018	mvh	GenerateFileName has NoKill option (for interactive dgate tasks); moved ALERT warning
20021020        mvh     Simplified and speeded queries for archiving using ImagePat field (requires rev5+ of db)
                        Removed " and added [ in MakeSafeString; hand-code one query for built-in dbase driver
                        Changed SQL construct in SelectLRUForArchival for compatibility with built-in driver
20030120	ljz	Added FreeDeviceTables
20030702        mvh     Start on ArchiveCompression; for now KB is specified -before!- compression
20030705        mvh     Replaced ProcessDDO by more correct DecompressNKI to check pixel data
20030706	mvh	Attach VRType to PDU's for implicit little endian support
20030724	ljz	Fix in GetKBUsedForPatient: missing %s in printf
20030819	mvh	Allow longer filenames
20030905	mvh	Allow longer filenames when reading device paths
20030921	mvh	Added DEVICE_TYPE_MIRROR in GetDevice; allow PanicKillOff to be called with variable treshold
20031217	ljz	Fix: Support harddisks bigger than 2 GigaByte!!! in CalcMegsOnDevice (Thanks to Clifford Sweet)
20040403        mvh     Moved ArchiveCompression to SelectLRUForArchival -> fills disks correctly
20040614        mvh     Added MoveDataToDevice
20050107        mvh     Adapted for linux compile (no threads yet, no check of disk space on device)
20050109        mvh     Added threads for linux
20050118        mvh     Detach thread after starting it to avoid leaks
20050119        mvh     GetKBUsedOnDevice is not used --> no problem to return -1 under linux
20050119        mvh     GetKBUsedOnDevice is not used --> no problem to return -1 under linux
20050414	mvh	Fix CalcMegsOnDevice for relative path
20050421	mvh     Fix: MAGThreshHold is space for largest file != space for cleanup
                        Now set to 30 MB independent of MAGDeviceThreshHold
20050901	mvh	Fix free space check for unc paths (error counting \)
20060311	mvh	When GetDiskFreeSpaceEx fails allow storing data anyway (fix for Marius Petruc)
20061213	mvh	Fixed nasty bug in MakeListOfPatientsOnDevice: Query without bindfield overwrites previous bound strings!
20070316	mvh	Cleanup now starts at MAGDeviceThreshHold to MAGDeviceThreshHold+1000, was start on 30 to MAGDeviceThreshHold
			Allow both MAG0 and MAG0.Archiving as input for PrepareBunchForBurning
20071027	mvh	Protect MakeListOfPatientsOnDevice on MAXPATIENTS; LRU selection now configurable using routine
                        MakeListOfOldestPatientsOnDevice. E.g. if LRUSort = StudyDate, deletes or archives least recent 
			scanned patients first. The default "" means that the original sort order on db entry date is kept
20071031	mvh	Catch out of memory errors and return error status if so
20071118	mvh	Adapted for 64 bits (use SQLLEN for BindField)
20071128	mvh	Fix by kamil.krasnik of Cache device disk space test for UNIX
			mvh Also fix Cache and Mirror for UNC paths
20080126	mvh	Fixed bug reported by Ali: second move to MAG1 of more data if same patient would fail on delete
			Added new logic: attempt to delete file from MAG0 that is not in MAG0 but is in MAG1 is OK
			Similar attempt to delete file from MAG0 that is in MAG0 but not in MAG1 is an error
20080302	mvh	Fixed case of DICOM table names
20080317	mvh	Fixed deletebunchafterburning change (Ali)
20080604	mvh	Modified device searching order when file not found: JUKEBOX now also searches other JUKEBOX
20080617	mvh	Fixed bug in above: second jukebox not accessed correctly
20080818	mvh	DbaseIII check now uses DB flags, not PATHSEP in datasource name
20081016	mvh	Fix check of max patients in MakeListofOldestPatientsOnDevice
20081120	mvh	Added UseEscapeStringConstants and DoubleBackSlashToDB
20081124	mvh	restrict PanicKillOff to 10 runs (to stop when no patients left)
20090821	mvh     Added new FileExists function that uses stat; maybe does not try to mount tape or disk to early
20090926	mvh     MakeListOfOldestPatientsOnDevice returns newest if Max<0
                        SelectLRUForArchival always use new code (use KB<0 to select MRU); 
			Default MakeListOfOldestPatientsOnDevice use AccessTime to emulate old code
20091231	bcb	Changed char* to const char* for gcc4.2 warnings
20100111	mvh	Merged
20100120	mvh	Fixed two const issues detected with ms8
20100124	mvh	Use MyGetPrivateProfileString
20100309	bcb	Added double parentheses (gcc4.2 Warnings)
20100309	bcb	Changed int to unsigned int, commented out unused variables and routines (gcc4.2 Warnings)
20100619	bcb	Added UNUSED_ARGUMENT.
20100717	mvh	Merged
20100901	mvh	Added IgnoreMAGDeviceThreshHold
20101121	mvh	Added ArchiveConverter0 (1900)
20110105	mvh	Do not stop deleting on single error; added MoveDeviceConverter0 (2000)
20110105	mvh	SelectSeriesForArchival: selects data older than age days (age>0) or younger (age<0) from device
20110106	mvh	added kb limit to SelectSeriesForArchival; Added moveseriestodevice
20110106	mvh	fixes in series code and new MakeSafeString; #if code to allow select on AccessTime instead of SeriesDate
20110119	mvh	Pass PDU to some archiving code as script context
20110320	mvh	Added MAGDeviceFullThreshHold, system will only write to device if more than # MB avail (def 30)
20110603	mvh	Fully init vr used for MakeSafeString
20110606	mvh	RecompressPatient also changes filename if script (ArchiveConverter0, MoveDeviceConverter0) passed
20110904	mvh	Fixed leak in InitializeDeviceTable (occurs in read_ini command)
20120723	mvh	Fixed string overflow found by bcb in MakeListOfLRUPatients
20130817	mvh	TestFile will uncompress all read data to test it
Spectra0011 Wed, 5 Feb 2014 14:36:36 +0000: Fix cppcheck bug #4 (strncpy termination)
Spectra0010 Wed, 5 Feb 2014 11:54:40 +0000: Fix cppcheck bug #3 (and others in same function)
20140215	mvh	Fixed potential Patlist memory leaks; and also always free PatientIDList in calling;
Spectra0018 Thu, 6 Mar 2014 15:43:54 -0300: Fix mismatched new/delete in device.cpp
20150629	mvh	Use configurable table names for sql queries
20150730	mvh	Expose KeepFile parameter of ModifyData
20181115	mvh	Changed default of MAGDeviceThreshHold to 0
20191221        mvh     Fix MakeListOfOldestPatientsOnDevice for no device passed
*/

#ifndef UNUSED_ARGUMENT
#define UNUSED_ARGUMENT(x) (void)x
#endif

#ifndef	WHEDGE
#	include	"dgate.hpp"
#else
#	include	"master.h"
#endif

#ifndef UNIX
#	include	<direct.h>
#	include <io.h>
#define PATHSEPSTR "\\"
#define PATHSEPCHAR '\\'
#else // UNIX
#define PATHSEPSTR "/"
#define PATHSEPCHAR '/'
#define stricmp(s1, s2) strcasecmp(s1, s2)
#define memicmp(s1, s2, n) strncasecmp(s1, s2, n)
#define strnicmp(s1, s2, n) strncasecmp(s1, s2, n)
#define memicmp(s1, s2, n) strncasecmp(s1, s2, n)
#define O_BINARY 0
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#endif

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#include "gpps.hpp"
#include "nkiqrsop.hpp"

// Device level support for MAG & System support

UINT		MAGDevices = 0, 
		MIRRORDevices = 0, 
		MOPDevices = 0, 
		CDRDevices = 0, 
		CACHEDevices = 0,
		JUKEBOXDevices = 0;

UINT		MAGDeviceThreshHold = 0;
UINT		MAGDeviceFullThreshHold = 30;
int		IgnoreMAGDeviceThreshHold = 0;

char		**MAGDeviceTable = NULL,	// on-line storage (load distributed)
		**MIRRORDeviceTable = NULL,	// mirror devices, e.g., MIRROR1 mirrors MAG1
		**MOPDeviceTable = NULL,	// not used
		**CDRDeviceTable = NULL,	// not used
		**CACHEDeviceTable = NULL,	// data to burn CDs from (each subdirectory is a disk)
		**JUKEBOXDeviceTable = NULL;	// CD-ROM towers (each subdirectory is a disk)

int		*MAGDeviceSpace = NULL;		// to maintain used disk space (for later chunk mode)
int		*MIRRORDeviceSpace = NULL;	// to maintain used disk space (for later chunk mode)

#ifndef	ConfigFile
extern	char	ConfigFile[];
#endif

// prototypes

int GetKBUsedOnDevice (char *Device);
int MakeListOfOldestPatientsOnDevice(char **PatientIDList, int Max, const char *Device, char *Sort);


////////////////////////////////////////////////////////////////////////
// Cleanup device tables
void FreeDeviceTables(void)
{ int i;

  for (i=0; i<(int)MAGDevices; i++)
    delete [] MAGDeviceTable[i];
  delete MAGDeviceTable;

  for (i=0; i<(int)MIRRORDevices; i++)
    delete [] MIRRORDeviceTable[i];
  delete MIRRORDeviceTable;

  for (i=0; i<(int)MOPDevices; i++)
    delete [] MOPDeviceTable[i];
  delete MOPDeviceTable;

  for (i=0; i<(int)CDRDevices; i++)
    delete [] CDRDeviceTable[i];
  delete CDRDeviceTable;

  for (i=0; i<(int)CACHEDevices; i++)
    delete [] CACHEDeviceTable[i];
  delete CACHEDeviceTable;

  for (i=0; i<(int)JUKEBOXDevices; i++)
    delete [] JUKEBOXDeviceTable[i];
  delete JUKEBOXDeviceTable;

  delete [] MAGDeviceSpace;

  delete [] MIRRORDeviceSpace;
}



////////////////////////////////////////////////////////////////////////
// Delete least recently accessed patients to make free disk space

char LRUSort[64] = "StudyDate";
unsigned int HasArchiveConverter=0;
unsigned int HasMoveDeviceConverter=0;

BOOL
PanicKillOff(unsigned int MAGThreshHold)
	{
	// Query the patient table, and sort it by access time
	SQLLEN			sdword,sdword1;
	DWORD			iTime;
	Database		DB;
//	char			s[255];
	char			PID[256];
	int			i;

	if(MAGThreshHold==0) MAGThreshHold = 10;


	// new code that deletes patients with oldest most recent study first 

	if (LRUSort[0])
		{
		char *PatientIDList;
		int Patients;
		int MAXPATIENTS = 1000;

		Patients = MakeListOfOldestPatientsOnDevice(&PatientIDList, MAXPATIENTS, "MAG0", LRUSort);

		for (i=0; i<Patients; i++)
			{
			strncpy(PID, PatientIDList + 256*i, 255);
			PID[255]=0;

			VR		*vr = new VR(0x0010, 0x0020, strlen(PID), (void*)PID, (BOOL) FALSE );
			DICOMDataObject	DDO;

			OperatorConsole.printf("DISK SPACE: %d MB / NEED %d MB - Deleting patient on %s: %s\n", LargestFreeMAG(), MAGThreshHold, LRUSort, PID);
			DDO.Push(vr);
			RemoveFromPACS(&DDO);
			if(LargestFreeMAG()>MAGThreshHold + 5)		// make 5 MB extra room
				break;
			else
				continue;
			}

		free(PatientIDList);
		return TRUE;
		}


	// original code that deletes patient on db order

	for (i=0; i<10; i++)
		{
		if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
			return ( FALSE );

		if (!DB.Query(PatientTableName, "AccessTime, PatientID", NULL, "AccessTime"))
			return (FALSE);

		if(!DB.BindField (1, SQL_C_ULONG,
				&iTime, 4,
				&sdword1))
				return ( FALSE );

		if(!DB.BindField (2, SQL_C_CHAR,
				PID, 255,
				&sdword))
				return ( FALSE );

		while(DB.NextRecord())
			{
			VR		*vr = new VR(0x0010, 0x0020, strlen(PID), (void*)PID, (BOOL) FALSE );
			DICOMDataObject	DDO;

			OperatorConsole.printf("DISK SPACE: %d MB / NEED %d MB - Deleting patient: %s\n", LargestFreeMAG(), MAGThreshHold, PID);
			DDO.Push(vr);
			RemoveFromPACS(&DDO);
			if(LargestFreeMAG()>MAGThreshHold + 5)		// make 5 MB extra room
				{
				DB.Close();
				return ( TRUE );
				}
			else
				continue;
			}
		}
	DB.Close();
	OperatorConsole.printf("DISK SPACE: %d MB / NEED %d MB - but stopping\n", LargestFreeMAG(), MAGThreshHold);
	return ( TRUE );
	}


////////////////////////////////////////////////////////////////////////
// start a killer to make some space by deleting LRU patients

#ifndef UNIX
	static HANDLE ThreadHandle=NULL;
#endif

BOOL
#ifndef UNIX
WINAPI
#endif
MAGRampage()
	{
	PanicKillOff(MAGDeviceThreshHold+1000);

#ifndef UNIX
	CloseHandle(ThreadHandle);
	ThreadHandle = NULL;
#endif

	return ( TRUE );
	}

////////////////////////////////////////////////////////////////////////
// find amount of space on disk with most space
UINT
LargestFreeMAG()
	{
	unsigned int		Index;
	UINT		Max = 0, iMax;
	Index = 0;

	while ( Index < MAGDevices)
		{
		iMax = CheckFreeStoreOnMAGDevice(Index);
		if(iMax>Max)
			Max = iMax;
		++Index;
		}
	return ( Max );
	}


////////////////////////////////////////////////////////////////////////
// find magnetic device with largest amount of space
UINT
MaxMagDevice ()
	{
	unsigned int		Index;
	UINT		Max = 0, iMax, Device = 0;
	Index = 0;

	while ( Index < MAGDevices)
		{
		iMax = CheckFreeStoreOnMAGDevice(Index);
		if(iMax>Max)
			{
			Max = iMax;
			Device = Index;
			}
		++Index;
		}
	return ( Device );
	}

////////////////////////////////////////////////////////////////////////
// find cache device with largest amount of space
int
MaxCACHEDevice 
	(UINT NeededSpace)
	{
	unsigned int Max = 0, iMax, Index;
	int		Device = -1;
	Index = 0;

	while ( Index < CACHEDevices)
		{
		iMax = CheckFreeStoreOnCACHEDevice(Index);
		if(iMax>Max)
			{
			Max = iMax;
			Device = Index;
			}
		++Index;
		}

	if (Max < NeededSpace)
		return -1;
	else
		return Device;
	}

////////////////////////////////////////////////////////////////////////
// read device table from dicom.ini
BOOL
InitializeDeviceTable(
	char	*SCRoot)
	{
	UINT	Index;
	char	iTemp[128];
	char	Temp[512];

	///////////////////// Threshold for panic kill off /////////////////////

	MyGetPrivateProfileString ( SCRoot, "MAGDeviceThreshHold", "0",
		(char*)Temp, 64, ConfigFile);
	MAGDeviceThreshHold = atoi(Temp);

	MyGetPrivateProfileString ( SCRoot, "MAGDeviceFullThreshHold", "30",
		(char*)Temp, 64, ConfigFile);
	MAGDeviceFullThreshHold = atoi(Temp);

	MyGetPrivateProfileString ( SCRoot, "IgnoreMAGDeviceThreshHold", "0",
		(char*)Temp, 64, ConfigFile);
	IgnoreMAGDeviceThreshHold = atoi(Temp);

	///////////////////// sort order for panic kill off and archival /////////////////////

	MyGetPrivateProfileString ( SCRoot, "LRUSort", "",
		(char*)LRUSort, 64, ConfigFile);

	///////////////////// on-line storage /////////////////////

	MyGetPrivateProfileString ( SCRoot, "MAGDevices", "1",
		(char*) Temp, 64, ConfigFile);
	MAGDevices = atoi(Temp);
	
	if (MAGDeviceTable)
	  FreeDeviceTables();

	MAGDeviceTable = new char*[MAGDevices];
	MAGDeviceSpace = new int[MAGDevices];

	Index = 0;
	while ( Index < MAGDevices )
		{
		sprintf(iTemp, "MAGDevice%d", Index);
		MyGetPrivateProfileString ( SCRoot, iTemp, "NULL",
			(char*) Temp, 500, ConfigFile);
		MAGDeviceTable[Index] = new char[strlen(Temp)+1];
		strcpy(MAGDeviceTable[Index], Temp);

		// sprintf(iTemp, "MAG%d", Index);
		// MAGDeviceSpace[Index] = GetKBUsedOnDevice (iTemp);

		MAGDeviceSpace[Index] = 0;

		++Index;
		}

	///////////////////// mirror storage /////////////////////

	MyGetPrivateProfileString ( SCRoot, "MIRRORDevices", "0",
		(char*) Temp, 64, ConfigFile);
	MIRRORDevices = atoi(Temp);

	MIRRORDeviceTable = new char*[MIRRORDevices];
	MIRRORDeviceSpace = new int[MIRRORDevices];

	Index = 0;
	while ( Index < MIRRORDevices )
		{
		sprintf(iTemp, "MIRRORDevice%d", Index);
		MyGetPrivateProfileString ( SCRoot, iTemp, "NULL",
			(char*) Temp, 500, ConfigFile);
		MIRRORDeviceTable[Index] = new char[strlen(Temp)+1];
		strcpy(MIRRORDeviceTable[Index], Temp);

		// sprintf(iTemp, "MIRROR%d", Index);
		// MIRRORDeviceSpace[Index] = GetKBUsedOnDevice (iTemp);

		MIRRORDeviceSpace[Index] = 0;

		++Index;
		}

	///////////////////// cache to off-line storage /////////////////////

	MyGetPrivateProfileString ( SCRoot, "CACHEDevices", "0",
		(char*) Temp, 64, ConfigFile);
	CACHEDevices = atoi(Temp);

	CACHEDeviceTable = new char*[CACHEDevices];

	Index = 0;
	while ( Index < CACHEDevices )
		{
		sprintf(iTemp, "CACHEDevice%d", Index);
		MyGetPrivateProfileString ( SCRoot, iTemp, "NULL",
			(char*) Temp, 500, ConfigFile);
		CACHEDeviceTable[Index] = new char[strlen(Temp)+1];
		strcpy(CACHEDeviceTable[Index], Temp);
		++Index;
		}

	///////////////////// off-line storage /////////////////////

	MyGetPrivateProfileString ( SCRoot, "JUKEBOXDevices", "0",
		(char*) Temp, 64, ConfigFile);
	JUKEBOXDevices = atoi(Temp);

	JUKEBOXDeviceTable = new char*[JUKEBOXDevices];

	Index = 0;
	while ( Index < JUKEBOXDevices )
		{
		sprintf(iTemp, "JUKEBOXDevice%d", Index);
		MyGetPrivateProfileString ( SCRoot, iTemp, "NULL",
			(char*) Temp, 500, ConfigFile);
		JUKEBOXDeviceTable[Index] = new char[strlen(Temp)+1];
		strcpy(JUKEBOXDeviceTable[Index], Temp);
		++Index;
		}

	MyGetPrivateProfileString ( SCRoot, "ArchiveConverter0", "",
		(char*) Temp, 512, ConfigFile);
	HasArchiveConverter = strlen(Temp);

	MyGetPrivateProfileString ( SCRoot, "MoveDeviceConverter0", "",
		(char*) Temp, 512, ConfigFile);
	HasMoveDeviceConverter = strlen(Temp);

	return ( TRUE );
	}


////////////////////////////////////////////////////////////////////////
// find amount of free space on a disk

#ifndef UNIX

UINT
CalcMegsOnDevice(
	char	*RootDirectory)
/*
	{
	DWORD	SectorsPerCluster;
	DWORD	BytesPerSector;
	DWORD	FreeClusters;
	DWORD	TotalClusters;
	DWORD	SP;
	if(GetDiskFreeSpace(RootDirectory,
		&SectorsPerCluster,
		&BytesPerSector,
		&FreeClusters,
		&TotalClusters))
		{
		SP = (SectorsPerCluster * FreeClusters) / 1024;
		SP = (SP * BytesPerSector) / 1024;
		SystemDebug.printf("FreeStore Left %d on %s (%d,%d,%d,%d)\n", SP, RootDirectory,
			SectorsPerCluster, BytesPerSector, FreeClusters, TotalClusters);
		return ( SP );
		}
	SystemDebug.printf("***Error getting free store for device %s\n", RootDirectory);
	return(0);
	}
*/
	/* Following code works correctly for harddisks with capacity above 2 GigaByte.
	   (thanks to Clifford Sweet)
	*/
        {
        ULARGE_INTEGER FreeBytesAvailable;
        ULARGE_INTEGER TotalNumberOfBytes;
        ULARGE_INTEGER TotalNumberOfFreeBytes;
        DWORD   SP;
	char	*rd;

	if (IgnoreMAGDeviceThreshHold) return 10000;

	// UNC paths and drive: are OK, but use NULL (cur drive) to fix for relative paths
	rd = RootDirectory;
	if (rd[1]!=':' && (rd[0]!='\\' || rd[1]!='\\')) rd=NULL;

        if(GetDiskFreeSpaceEx(rd,
                        &FreeBytesAvailable,
                        &TotalNumberOfBytes,
                        &TotalNumberOfFreeBytes))
                {
                SP = (DWORD) (FreeBytesAvailable.QuadPart >> 20);
                SystemDebug.printf("FreeStore Left %d on %s\n", SP, RootDirectory);
                return ( SP );
                }

        SystemDebug.printf("***Error getting free store for device %s\n", RootDirectory);
        return(1000);
        }


#else // UNIX 

UINT
CalcMegsOnDevice(const char *theRootDirectory)
	{
	size_t aFreeSpace = 0;
	struct statvfs aStat;

	// statvfs() takes any path name and returns info on the device
	// containing that path (file or directory)

	if (IgnoreMAGDeviceThreshHold) return 10000;

	if(!statvfs(theRootDirectory, &aStat))
	{
		aFreeSpace =
			(size_t)(((double)aStat.f_bavail * (double)aStat.f_frsize) /
			((double)1024 * (double)1024));

		SystemDebug.printf("FreeStore Left %d on %s\n", aFreeSpace,
			theRootDirectory);
	}
	else
	{
		SystemDebug.printf("***Error getting free store for device %s\n",
			theRootDirectory);
	}
	return aFreeSpace;
}

#endif // UNIX

////////////////////////////////////////////////////////////////////////
// Find amount of space on MAG device N
#ifndef UNIX
UINT
CheckFreeStoreOnMAGDevice(
	UINT	MagNumber)
	{
	int	Index;
	char	s[1024];

        if (MagNumber >= MAGDevices || MagNumber < 0) return 0;

	strcpy(s, MAGDeviceTable[MagNumber]);
	// Check for UNC style name
	if(!strncmp(s, "\\\\", 2))
		{
		// UNC Name
		Index = 2;
		while(1)
			{
			if(s[Index]=='\\')
				break;
			++Index;
			}
		++Index;	// mvh 20050901
		while(s[Index])
			{
			if(s[Index]=='\\')
				{
				s[Index]='\0';
				return(CalcMegsOnDevice(s));
				}
			++Index;
			}
		return(CalcMegsOnDevice(s));
		}
	s[3] = '\0';
	return(CalcMegsOnDevice(s));
	}
#else
// UNIX version...
UINT
CheckFreeStoreOnMAGDevice(
	UINT	MagNumber)
	{
        if (MagNumber >= MAGDevices || MagNumber < 0) return 0;
	return(CalcMegsOnDevice(MAGDeviceTable[MagNumber]));
	}
#endif

////////////////////////////////////////////////////////////////////////
// Find amount of space on MIRROR device N
#ifndef UNIX
UINT
CheckFreeStoreOnMIRRORDevice(
	UINT	MIRRORNumber)
	{
	int	Index;
	char	s[1024];

        if (MIRRORNumber >= MIRRORDevices || MIRRORNumber < 0) return 0;

	strcpy(s, MIRRORDeviceTable[MIRRORNumber]);
	// Check for UNC style name
	if(!strncmp(s, "\\\\", 2))
		{
		// UNC Name
		Index = 2;
		while(1)
			{
			if(s[Index]=='\\')
				break;
			++Index;
			}
		++Index;	// mvh 20071128
		while(s[Index])
			{
			if(s[Index]=='\\')
				{
				s[Index]='\0';
				return(CalcMegsOnDevice(s));
				}
			++Index;
			}
		return(CalcMegsOnDevice(s));
		}
	s[3] = '\0';
	return(CalcMegsOnDevice(s));
	}
#else
// UNIX version...
UINT
CheckFreeStoreOnMIRRORDevice(
	UINT	MIRRORNumber)
	{
        if (MIRRORNumber >= MIRRORDevices || MIRRORNumber < 0) return 0;
	return(CalcMegsOnDevice(MIRRORDeviceTable[MIRRORNumber]));
	}
#endif

////////////////////////////////////////////////////////////////////////
// Find amount of space on CACHE device N
#ifndef UNIX
UINT
CheckFreeStoreOnCACHEDevice(
	UINT	CACHENumber)
	{
	int	Index;
	char	s[1024];

        if (CACHENumber > CACHEDevices || CACHENumber < 0) return 0;

	strcpy(s, CACHEDeviceTable[CACHENumber]);
	// Check for UNC style name
	if(!strncmp(s, "\\\\", 2))
		{
		// UNC Name
		Index = 2;
		while(1)
			{
			if(s[Index]=='\\')
				break;
			++Index;
			}
		++Index;	// mvh 20071128
		while(s[Index])
			{
			if(s[Index]=='\\')
				{
				s[Index]='\0';
				return(CalcMegsOnDevice(s));
				}
			++Index;
			}
		return(CalcMegsOnDevice(s));
		}
	s[3] = '\0';
	return(CalcMegsOnDevice(s));
	}
#else
// UNIX version...
UINT
CheckFreeStoreOnCACHEDevice( 
        UINT    CACHENumber) 
        { 
        char s[1024]; 
        char* chp; 

        if (CACHENumber >= CACHEDevices || CACHENumber < 0) return 0; 

        strncpy(s, CACHEDeviceTable[CACHENumber], 1023); 
	s[1023] = '\0';

        if ((chp = strchr(s, '%'))) 
        { 
            chp[0] = '\0'; 
            if ((chp = strrchr(s, '/'))) 
            { 
                chp[0] = '\0'; 
            } 
        } 

        return(CalcMegsOnDevice(s)); 
        } 
#endif

////////////////////////////////////////////////////////////////////////
// Locate a device that is suitable to store a new image on
// Note: Assumes that 30 MB is enough to store a single image

BOOL
GetDevice(
	char	*DeviceName,
	UINT	DeviceFlags)
	{
	UINT			Index;
	//UINT            MAGThreshHold = 30;

#ifndef UNIX
	DWORD			ThreadID;
#else
	pthread_t		ThreadID;
#endif

// mvh 20050421: MAGThreshHold is space for largest file != space for cleanup MAGDeviceThreshHold
//	if(MAGDeviceThreshHold) MAGThreshHold = MAGDeviceThreshHold;

	if(DeviceFlags&DEVICE_TYPE_MAG)
		{
		Index = 0;
		while ( Index < MAGDevices )
			{
			unsigned int c = CheckFreeStoreOnMAGDevice(Index);
			if( c > MAGDeviceFullThreshHold)
				{
				sprintf(DeviceName, "MAG%d", Index);
				if (c < MAGDeviceThreshHold && MAGDeviceThreshHold!=0)
					goto cleanup;
				return ( TRUE );
				}
			++Index;
			}

                if (DeviceFlags&DEVICE_TYPE_MIRROR)
			{
			Index = 0;
			while ( Index < MIRRORDevices )
				{
				if(CheckFreeStoreOnMIRRORDevice(Index) > MAGDeviceFullThreshHold)
					{
					sprintf(DeviceName, "MIRROR%d", Index);
					return ( TRUE );
					}
				++Index;
				}
			}

		// Report out of disk without attempt to cleanup if MAGDeviceThreshHold==0 or when not a server
		if(!MAGDeviceThreshHold || (DeviceFlags & DEVICE_TYPE_NOKILL) ) return FALSE;

		// god help us; no disk found with enough space
		sprintf(DeviceName, "MAG%d", MaxMagDevice ());

cleanup:

#ifndef UNIX
		if (!ThreadHandle)
			{ 
 		        OperatorConsole.printf("***ALERT: RUNNING OUT OF FREE SPACE\n");
			ThreadHandle = CreateThread(NULL, 0x0000f000, 
				(LPTHREAD_START_ROUTINE) MAGRampage, NULL, 0, &ThreadID);
			}
#else // UNIX
		OperatorConsole.printf("***ALERT: RUNNING OUT OF FREE SPACE\n");
		pthread_create(&ThreadID, NULL, (void*(*)(void*))MAGRampage, NULL);
		pthread_detach(ThreadID);
#endif
		return ( TRUE );
		}
	else
		return ( FALSE );
	}


////////////////////////////////////////////////////////////////////////
// Test if a directory exists
BOOL
DirectoryExists(
	char	*path)
	{
	char temp[1024];
	struct stat statbuf;

	int lc= strlen(path) - 1;			/* index for last char in path */
	strcpy(temp, path);

	if (temp[lc] == '\\' || temp[lc] == '/')
		temp[lc] = 0;

	if (stat(temp, &statbuf) == -1) return FALSE;	/* file or path not found */

							/* only directory allowed */
	if ((statbuf.st_mode & (S_IFCHR | S_IFREG)) == 0)
		return TRUE;
	else
		return FALSE;
	}

////////////////////////////////////////////////////////////////////////
// Get directory for given device (MAG, MIRROR, CACHE, or JUKEBOX)
BOOL
GetPhysicalDevice(
	const char	*Device,
	char		*Physical)
	{
        int     Num;

        Physical[0] = 0;

	// Check for MAG devices
	// Example: MAG0 = main data storage

	if(strnicmp(Device, "MAG", 3)==0)
		{
                Num = atoi(&Device[3]);
                if (Num >= (signed int)MAGDevices || Num < 0) return FALSE;
                
                strcpy(Physical, MAGDeviceTable[Num]);

		// ensure that Physical string ends with a path separator
		int aLength = (int)strlen(Physical);
		if(aLength <= 0)
			return(FALSE);
		if(Physical[aLength - 1] != PATHSEPCHAR)
			{
			Physical[aLength++] = PATHSEPCHAR;
			Physical[aLength] = '\0';
			}
		return ( TRUE );
		}

	if(strnicmp(Device, "MIRROR", 6)==0)
		{
                Num = atoi(&Device[6]);
                if (Num >= (signed int)MIRRORDevices || Num < 0) return FALSE;
                
                strcpy(Physical, MIRRORDeviceTable[Num]);

		// ensure that Physical string ends with a path separator
		int aLength = (int)strlen(Physical);
		if(aLength <= 0)
			return(FALSE);
		if(Physical[aLength - 1] != PATHSEPCHAR)
			{
			Physical[aLength++] = PATHSEPCHAR;
			Physical[aLength] = '\0';
			}
		return ( TRUE );
		}

	// Check CACHE device for images that has been copied for archival
	// Example: CACHE0.1.123 = On cache 0, data for jukebox 1, cd 123

	if(strnicmp(Device, "CACHE", 5)==0)
		{
		int 	JUKEBOXDevice=0;
		int 	CDNumber     =0;
		const char *p;
		char	Temp[1024];

                Num = atoi(&Device[5]);
                if (Num >= (signed int)CACHEDevices || Num < 0) return FALSE;

                strcpy(Physical, CACHEDeviceTable[Num]);

		p = strchr(Device, '.');
		if (p)
			{
			JUKEBOXDevice = atoi(p+1);
			p = strchr(p+1, '.');
			if (p)	CDNumber = atoi(p+1);
			}
		sprintf(Temp, Physical, JUKEBOXDevice, CDNumber);
		strcpy(Physical, Temp);
		
		int aLength = (int)strlen(Physical);
		if(aLength <= 0)
			return(FALSE);
		if(Physical[aLength - 1] != PATHSEPCHAR)
			{
			Physical[aLength++] = PATHSEPCHAR;
			Physical[aLength] = '\0';
			}
		return ( TRUE );
		}


	// Check for data on a jukebox
	// Example: JUKEBOX1.123 = jukebox 1, cd 123

	if(strnicmp(Device, "JUKEBOX", 7)==0)
		{
		int 	CDNumber     =0;
		const char 	*p;
		char	Temp[1024];

                Num = atoi(&Device[7]);
                if (Num >= (signed int)JUKEBOXDevices || Num < 0) return FALSE;

                strcpy(Physical, JUKEBOXDeviceTable[Num]);

		p = strchr(Device, '.');
		if (p)	CDNumber = atoi(p+1);
		sprintf(Temp, Physical, CDNumber);
		strcpy(Physical, Temp);

		// ensure that Physical string ends with a path separator
		int aLength = (int)strlen(Physical);
		if(aLength <= 0)
			return(FALSE);
		if(Physical[aLength - 1] != PATHSEPCHAR)
			{
			Physical[aLength++] = PATHSEPCHAR;
			Physical[aLength] = '\0';
			}
		return ( TRUE );
		}

	return(FALSE);
	}

////////////////////////////////////////////////////////////////////////
// Test if a file exists
/*static BOOL
FileExistsOld(char *Path)
	{
	FILE *f;
//	int res;

	f = fopen(Path, "rb");
	if (f==NULL) return FALSE;

	fclose(f);
	return TRUE;
	}
*/
static BOOL
FileExists(
	char	*path)
	{
	struct stat statbuf;

	if (stat(path, &statbuf) == -1) return FALSE;	// file or path not found

							// regular file
	if (statbuf.st_mode & S_IFREG)
		return TRUE;
	else
		return FALSE;
	}

////////////////////////////////////////////////////////////////////////
// Find directory that contains a given image file on all MAG, CACHE, or JUKEBOX devices
BOOL
FindPhysicalDevice(
	char	*Device, 
	char	*Physical, 
	char	*ObjectFile)
	{
	char 	FileName[1024], Temp[1024];
	int		number=-1, sub1=-1, sub2=-1;
	unsigned int i;
	char	*p;

	SystemDebug.printf("Locating file:%s %s\n", Device, ObjectFile);

	// interpret the device name
	if(strnicmp(Device, "MAG", 3)==0)
		number = atoi(Device + 3);
	else if(strnicmp(Device, "MIRROR", 6)==0)
		number = atoi(Device + 6);
	else if(strnicmp(Device, "CACHE", 5)==0)
		number = atoi(Device + 5);
	else if(strnicmp(Device, "JUKEBOX", 7)==0)
		number = atoi(Device + 7);

	// try listed device first (it may be right !)

        GetPhysicalDevice(Device, Physical);
	strcpy(FileName, Physical);
	strcat(FileName, ObjectFile);
	if (FileExists(FileName)) 
		return TRUE;

	// get device sub-numbers

	p = strchr(Device, '.');
	if (p)
		{
		sub1 = atoi(p+1);
		p = strchr(p+1, '.');
		if (p)	sub2 = atoi(p+1);
		}

	// for MAG try MIRROR device (disk may be corrupt or not updated yet from slow mirror server)

	if(strnicmp(Device, "MAG", 3)==0)
		{
		sprintf(Temp, "MIRROR%d", number);
		GetPhysicalDevice(Temp, Physical);
		strcpy(FileName, Physical);
		strcat(FileName, ObjectFile);
		if (FileExists(FileName)) 
			{
			SystemDebug.printf("***Located file on mirror device:%s instead of %s\n", Temp, Device);
			return TRUE;
			}
		}

	// for a jukebox device: try all other jukeboxes

	if(strnicmp(Device, "JUKEBOX", 7)==0)
		{
		for (i=0; i<JUKEBOXDevices; i++)
			{ 
			if (i==(unsigned int)number) continue;

			sprintf(Temp, "JUKEBOX%d", i);
			if (sub1 >= 0) sprintf(Temp+strlen(Temp), ".%d", sub1);

			GetPhysicalDevice(Temp, Physical);

			strcpy(FileName, Physical);
			strcat(FileName, ObjectFile);
			if (FileExists(FileName)) 
				{
				SystemDebug.printf("***Located file on wrong device:%s instead of %s\n", Temp, Device);
				return TRUE;
				}
			}
		}


	// for JUKEBOX try CACHE devices with jukebox number and sub1, e.g., from JUKEBOX1.123 tries CACHE0.1.123
	if(strnicmp(Device, "JUKEBOX", 7)==0)
		{
		for (i=0; i<CACHEDevices; i++)
			{ 
			sprintf(Temp, "CACHE%d", i);
			if (number>= 0) sprintf(Temp+strlen(Temp), ".%d", number);
			if (sub1  >= 0) sprintf(Temp+strlen(Temp), ".%d", sub1);

			GetPhysicalDevice(Temp, Physical);

			strcpy(FileName, Physical);
			strcat(FileName, ObjectFile);
			if (FileExists(FileName)) 
				{
				SystemDebug.printf("***Located file on wrong device:%s instead of %s\n", Temp, Device);
				return TRUE;
				}
			}
		}

	// for CACHE try JUKEBOX devices with cache sub2, e.g., from 'CACHE0.1.123' tries JUKEBOX0.123
	if(strnicmp(Device, "CACHE", 5)==0)
		{
		for (i=0; i<JUKEBOXDevices; i++)
			{ 
			sprintf(Temp, "JUKEBOX%d", i);
			if (sub2 >= 0) sprintf(Temp+strlen(Temp), ".%d", sub2);

			GetPhysicalDevice(Temp, Physical);

			strcpy(FileName, Physical);
			strcat(FileName, ObjectFile);
			if (FileExists(FileName)) 
				{
				SystemDebug.printf("***Located file on wrong device:%s instead of %s\n", Temp, Device);
				return TRUE;
				}
			}
		}

	// try all MAG devices
	for (i=0; i<MAGDevices; i++)
		{ 
		sprintf(Temp, "MAG%d", i);
		GetPhysicalDevice(Temp, Physical);
		strcpy(FileName, Physical);
		strcat(FileName, ObjectFile);
		if (FileExists(FileName)) 
			{
			SystemDebug.printf("***Located file on wrong device:%s instead of %s\n", Temp, Device);
			return TRUE;
			}
		}
	
	// try all MIRROR devices
	for (i=0; i<MIRRORDevices; i++)
		{ 
		sprintf(Temp, "MIRROR%d", i);
		GetPhysicalDevice(Temp, Physical);
		strcpy(FileName, Physical);
		strcat(FileName, ObjectFile);
		if (FileExists(FileName)) 
			{
			SystemDebug.printf("***Located file on wrong device:%s instead of %s\n", Temp, Device);
			return TRUE;
			}
		}

	Physical[0] = 0;
	return FALSE;
	}

////////////////////////////////////////////////////////////////////////
// Compute size of a file
static int 
FileSize(char *Path)
	{
	FILE *f;
	int res;

	f = fopen(Path, "rb");
	if (f==NULL) return -1;

	fseek(f, 0, 2);
  	res = ftell(f); /* ask position */
	fclose(f);

	return res;
	}

////////////////////////////////////////////////////////////////////////
// move a file (must remain on same drive)
static BOOL
FileMove(char *oldname, char *newname)
	{
	if (rename(oldname, newname)) return FALSE;
	return TRUE;
	}

////////////////////////////////////////////////////////////////////////
// copy a file (possibly opening as shared file)
static BOOL
FileCopy(char *source, char *target, int smode)
{
	BOOL		err_status = TRUE; 	/* Error status                 */
	char		*copybuffer;        	/* The copy buffer              */
	int		inhandle, outhandle; 	/* Handle of input, output files */
	unsigned 	bufsize;            	/* Size of copy buffer          */
	unsigned	bytes;              	/* Actual transferred bytes     */

	/* Open input file for read-only. parser error if operation fails.  */

#ifdef WIN32
	if (!smode)
		inhandle = open( source, O_RDONLY | O_BINARY, S_IREAD);
	else
 		inhandle = sopen( source, O_RDONLY | O_BINARY, SH_DENYNO, S_IREAD);
#else
	UNUSED_ARGUMENT(smode);
	inhandle = open( source, O_RDONLY);
#endif

	if( inhandle == -1) return FALSE;

	/* Ok, open output file. Parser error on permission failure: */

	outhandle = open( target, O_CREAT | O_TRUNC | O_BINARY | O_RDWR, 0666);

	if( outhandle == -1)
 		{ 
		close(inhandle);
		return FALSE;
		}

	/* get a large buffer */
	bufsize = 0xF000;

	copybuffer = (char *)malloc(bufsize);
	if (copybuffer == NULL)
   		{
		close(inhandle);
		close(outhandle);
		return FALSE;
		}

#ifdef WIN32		
	while( !eof(inhandle) && err_status)
#else
	while( err_status)
#endif
		{
		bytes= (unsigned)read( inhandle, copybuffer, bufsize);

		if (bytes == (unsigned) -1)
			{ 
			err_status= FALSE;
       			break;
			}

#ifndef WIN32		
		if (bytes == 0)
			break;
#endif
		
		if (bytes != (unsigned)write(outhandle, copybuffer, bytes))
			{
			err_status= FALSE;
       			break;
			}
		}

	free(copybuffer);


	close(outhandle);
	close(inhandle);

	/* Clean up mess in case of error : */
	if(err_status != TRUE) 
		unlink(target);

	return err_status;
	}

////////////////////////////////////////////////////////////////////////
// compare two files (possibly opening as shared file)
static BOOL
FileCompare(char *source, char *target, int smode)
{
	BOOL		err_status = TRUE; 	/* Error status                  */
	char		*buffer1, *buffer2;    	/* The compare buffers           */
	int		handle1, handle2; 	/* Handle files 		 */
	unsigned 	bufsize;            	/* Size of copy buffer           */
	unsigned	bytes1, bytes2;        	/* Actual transferred bytes      */

	/* Open input file for read-only. parser error if operation fails.  */

#ifdef WIN32
	if (!smode)
		handle1 = open( source, O_RDONLY | O_BINARY, S_IREAD);
	else
 		handle1 = sopen( source, O_RDONLY | O_BINARY, SH_DENYNO, S_IREAD);
#else
	UNUSED_ARGUMENT(smode);
	handle1 = open( source, O_RDONLY, S_IREAD);
#endif

	if( handle1 == -1) return FALSE;

	/* Open other file for read-only. parser error if operation fails.  */

#ifdef WIN32
	if (!smode)
		handle2 = open( target, O_RDONLY | O_BINARY, S_IREAD);
	else
 		handle2 = sopen( target, O_RDONLY | O_BINARY, SH_DENYNO, S_IREAD);
#else
	handle2 = open( target, O_RDONLY, S_IREAD);
#endif

	if( handle2 == -1)
 		{ 
		close(handle1);
		return FALSE;
		}

	/* get a large buffer */
	bufsize = 0xF000;

	buffer1 = (char *)malloc(bufsize);
	buffer2 = (char *)malloc(bufsize);
	if (buffer1 == NULL || buffer2 == NULL)
   		{
		close(handle1);
		close(handle2);
		free (buffer1);
		free (buffer2);
		return FALSE;
		}

#ifdef WIN32
	while( !eof(handle1))
#else
	while( TRUE)
#endif

		{
		bytes1= (unsigned)read( handle1, buffer1, bufsize);

		if (bytes1 == (unsigned) -1)
			{ 
			err_status= FALSE;
       			break;
			}

		bytes2= (unsigned)read( handle2, buffer2, bufsize);

		if (bytes2 == (unsigned) -1)
			{ 
			err_status= FALSE;
       			break;
			}

		if (bytes1 != bytes2)
			{ 
			err_status= FALSE;
       			break;
			}

#ifndef WIN32		
		if (bytes1 == 0)
			break;
#endif
		
#ifdef WIN32		
		if (eof(handle1) != eof(handle2))
			{ 
			err_status= FALSE;
       			break;
			}
#endif

		if (memcmp(buffer1, buffer2, bytes1))
			{
			err_status= FALSE;
       			break;
			}
		}

	free(buffer1);
	free(buffer2);

	close(handle1);
	close(handle2);

	return err_status;
	}

////////////////////////////////////////////////////////////////////////
// Add size of a file to the total KB for a device (not used yet)
BOOL
RegisterUsedSpace(
	char	*Device,
	int	Kbytes)
	{
	if(strnicmp(Device, "MAG", 3))
		return FALSE;

	MAGDeviceSpace[atoi(&Device[3])] += Kbytes;
	return TRUE;
	}

//////////////////////////////////////////////////////////////////////////////////////////////
// compute total space used by a device (may also be computed by GetKBUsedForPatient with ID=*)
// Implementation only correct for simple filenamesyntaxes with 1 level of subdirs

int
GetKBUsedOnDevice (char	*Device)
	{
#ifdef WIN32
	int		itemp;
	double		result;
	HANDLE	fdHandle, ffHandle;
	WIN32_FIND_DATA	FileData;
	char	PathString[1024], TempPath[1024], s[1024], FileString[1024];
	char	BasePath[1024];

	result = 0.0;

	// convert device specification to a path

	GetPhysicalDevice(Device, PathString);
	itemp = strlen(PathString);
	if(!itemp)
		return -1;

	// add a '/' to end of directory name if not yet present

	if(PathString[itemp-1] != PATHSEPCHAR)
		strcat(PathString, PATHSEPSTR);

	// PathString now contains Image file storage root directory path

	strcpy(TempPath, PathString);
	strcat(TempPath, "*.*");
	
	fdHandle = FindFirstFile(TempPath, &FileData);
	
	if(fdHandle == INVALID_HANDLE_VALUE)
		return -1;


	while ( TRUE )
		{

		/* Filter out INCOMING and BIN directories */		
		if(stricmp(FileData.cFileName, "INCOMING"))
		if(strcmp(FileData.cFileName, "."))
		if(strcmp(FileData.cFileName, ".."))
		if(stricmp(FileData.cFileName, "BIN"))
			{
			strcpy(TempPath, PathString);
			strcat(TempPath, FileData.cFileName);
			strcat(TempPath, PATHSEPSTR);
			strcat(TempPath, "*.*");

			strcpy(FileString, FileData.cFileName);
			ffHandle = FindFirstFile(TempPath, &FileData);
			if(ffHandle!=INVALID_HANDLE_VALUE)
				{
				while ( TRUE )
					{
					strcpy(TempPath, PathString);
					strcat(TempPath, FileString);
					strcat(TempPath, PATHSEPSTR);
					strcat(TempPath, FileData.cFileName);

					strcpy(BasePath, FileString);
					strcat(BasePath, PATHSEPSTR);
					strcat(BasePath, FileData.cFileName);

					if(strcmp(FileData.cFileName, "."))
					if(strcmp(FileData.cFileName, ".."))
						result += FileSize(TempPath);

					if(!FindNextFile(ffHandle, &FileData))
						break;
					}
				FindClose(ffHandle);
				}
			}

		if(!FindNextFile(fdHandle, &FileData))
			break;
		}
	FindClose ( fdHandle );

	return (int)(result/1024 + 0.5);
#else
	// Not implemented under linux
	UNUSED_ARGUMENT(Device);
	return -1;
#endif
	}

////////////////////////////////////////////////////////////////////////
// Make a SQL safe string from a character string (how about the '_' ?)
// uses MakeSafeString from VRTOSQL.CPP (is database type dependent)

BOOL MakeSafeString (VR	*vr, char *string, Database *db);

BOOL MakeSafeString (const char *in, char *string, Database *db)
	{
	VR       vr;
	char	*s1;
	char    s[512];

	vr.Data = (void*)in;
	vr.Length = strlen(in);
	vr.Group = 0;
	vr.Element = 0;
	MakeSafeString(&vr, s, db);
	vr.Data = NULL;
	vr.Length = 0;

	if (s[0]=='?') sprintf(string, "LIKE %s", s+1);
	else           sprintf(string, "= %s", s);

	return TRUE;
	}

/////////////////////////////////////////////////////////////////////
// compute total space used or number of files for a patient on a device
int
GetKBUsedForPatient (char *Device, char *PatientID, int ComputeSpace)
	{
	Database aDB;
	char Tables[512], DeviceName[512], ObjectFile[1024], FileName[1024];
	char QueryString[512], PatientIDValue[512], DeviceValue[512];
	SQLLEN SQLResultLength;
	double TotalSize;
	int Size;

	sprintf(Tables, "%s", ImageTableName);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return -1;
		}
	
	MakeSafeString(PatientID, PatientIDValue, &aDB);
	MakeSafeString(Device,    DeviceValue,    &aDB);

	sprintf(QueryString, 	"DICOMImages.ImagePat %s and "
				"DICOMImages.DeviceName %s",
				PatientIDValue, DeviceValue);

	if (!aDB.Query(Tables, "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
		{
		OperatorConsole.printf("***Unable to query for image records to compute total size\n");
		aDB.Close();
		return -1;
		}

	aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
	aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);

	TotalSize = 0;
	while (aDB.NextRecord())
		{ 	
                if (ComputeSpace)
			{
			GetPhysicalDevice(DeviceName, FileName);
			strcat(FileName, ObjectFile);
			Size = FileSize(FileName);
			if (Size<0)
				OperatorConsole.printf("***Unable to determine size of file: '%s'\n", FileName);
			else
				{
				SystemDebug.printf("%s = %d KB\n", FileName, FileSize(FileName)/1024);
				TotalSize += Size / 1024.0;
				}
			}
		else
			TotalSize++;
		}

	aDB.Close();	

	if(TotalSize>0)
		{
                if (ComputeSpace)
			SystemDebug.printf("Total Size = %d KB\n", (int)(TotalSize + 0.512));
		else
			SystemDebug.printf("Total files = %d\n", (int)(TotalSize + 0.5));
		}

	return (int)(TotalSize + 0.512);
	}

/////////////////////////////////////////////////////////////////////
// compute total space used or number of files for a patient on a device
int
GetKBUsedForSeries (char *Device, char *SeriesUID, int ComputeSpace)
	{
	Database aDB;
	char Tables[512], DeviceName[512], ObjectFile[1024], FileName[1024];
	char QueryString[512], DeviceValue[512];
	SQLLEN SQLResultLength;
	double TotalSize;
	int Size;

	sprintf(Tables, "%s", ImageTableName);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return -1;
		}
	
	MakeSafeString(Device,    DeviceValue,    &aDB);

	sprintf(QueryString, 	"DICOMImages.SeriesInst = '%s' and "
				"DICOMImages.DeviceName %s",
				SeriesUID, DeviceValue);

	if (!aDB.Query(Tables, "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
		{
		OperatorConsole.printf("***Unable to query for image records to compute total size\n");
		aDB.Close();
		return -1;
		}

	aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
	aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);

	TotalSize = 0;
	while (aDB.NextRecord())
		{ 	
                if (ComputeSpace)
			{
			GetPhysicalDevice(DeviceName, FileName);
			strcat(FileName, ObjectFile);
			Size = FileSize(FileName);
			if (Size<0)
				OperatorConsole.printf("***Unable to determine size of file: '%s'\n", FileName);
			else
				{
				SystemDebug.printf("%s = %d KB\n", FileName, FileSize(FileName)/1024);
				TotalSize += Size / 1024.0;
				}
			}
		else
			TotalSize++;
		}

	aDB.Close();	

	if(TotalSize>0)
		{
                if (ComputeSpace)
			SystemDebug.printf("Total Size = %d KB\n", (int)(TotalSize + 0.512));
		else
			SystemDebug.printf("Total files = %d\n", (int)(TotalSize + 0.5));
		}

	return (int)(TotalSize + 0.512);
	}

/////////////////////////////////////////////////////////////////////
// Recompress all files of a patient on device (unless Compression "as" "is" or "")
// Use some ArchiveConverter0 e.g., "nop" to modify filename when syntax changed

BOOL ModifyImageFile(char *filename, char *script, ExtendedPDU_Service *PDU, BOOL DeleteFile);

void
RecompressPatient (char *Device, char *PatientID, char *Compression, ExtendedPDU_Service *PDU)
	{
	Database aDB;
	char Tables[512], DeviceName[512], ObjectFile[1024], FileName[1024];
	char QueryString[512], PatientIDValue[512], DeviceValue[512];
	SQLLEN SQLResultLength;
	
	sprintf(Tables, "%s", ImageTableName);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s for archive recompression\n",
					DataSource, UserName, DataHost);
		return;
		}
	
	MakeSafeString(PatientID, PatientIDValue, &aDB);
	MakeSafeString(Device,    DeviceValue,    &aDB);

	sprintf(QueryString, 	"DICOMImages.ImagePat %s and "
				"DICOMImages.DeviceName %s",
				PatientIDValue, DeviceValue);
	
	if (HasArchiveConverter)
		{
		if (!aDB.Query(Tables, "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
			{
			OperatorConsole.printf("***Unable to query for image records to modify for archival\n");
			aDB.Close();
			return;
			}

		aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
		aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);

		while (aDB.NextRecord())
			{ 	
			GetPhysicalDevice(DeviceName, FileName);
			strcat(FileName, ObjectFile);
			
			ModifyImageFile(FileName, "1900", PDU, TRUE);
			}
		}

	if (stricmp(Compression, "")==0 || stricmp(Compression, "as")==0 || stricmp(Compression, "is")==0) 
		{
		aDB.Close();
		return;
		}

	if (!aDB.Query(Tables, "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
		{
		OperatorConsole.printf("***Unable to query for image records to recompress for archival\n");
		aDB.Close();
		return;
		}

	aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
	aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);

	while (aDB.NextRecord())
		{ 	
		GetPhysicalDevice(DeviceName, FileName);
		strcat(FileName, ObjectFile);

		recompressFile(FileName, Compression, PDU);
		}

	aDB.Close();	
	}

/////////////////////////////////////////////////////////////////////
// Run move script on all files of a patient on selected device
// Use some MoveDeviceConverter0 e.g., "nop" to force modify filename when syntax changed

void
ProcessMoveDevicePatient (char *Device, char *PatientID)
	{
	Database aDB;
	char Tables[512], DeviceName[512], ObjectFile[1024], FileName[1024];
	char QueryString[512], PatientIDValue[512], DeviceValue[512];
	SQLLEN SQLResultLength;
	ExtendedPDU_Service PDU;

	if (!HasMoveDeviceConverter) return;

	sprintf(Tables, "%s", ImageTableName);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return;
		}
	
	MakeSafeString(PatientID, PatientIDValue, &aDB);
	MakeSafeString(Device,    DeviceValue,    &aDB);

	sprintf(QueryString, 	"DICOMImages.ImagePat %s and "
				"DICOMImages.DeviceName %s",
				PatientIDValue, DeviceValue);

	if (!aDB.Query(Tables, "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
		{
		OperatorConsole.printf("***Unable to query for image records to run move script\n");
		aDB.Close();
		return;
		}

	aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
	aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);

	while (aDB.NextRecord())
		{ 	
		GetPhysicalDevice(DeviceName, FileName);
		strcat(FileName, ObjectFile);
		ModifyImageFile(FileName, "2000", &PDU, TRUE);
		}

	aDB.Close();	
	}

/////////////////////////////////////////////////////////////////////
// Run move script on all files of a series on selected device

void
ProcessMoveDeviceSeries (char *Device, char *SeriesUID)
	{
	Database aDB;
	char Tables[512], DeviceName[512], ObjectFile[1024], FileName[1024];
	char QueryString[512], SeriesUIDValue[512], DeviceValue[512];
	SQLLEN SQLResultLength;
	ExtendedPDU_Service PDU;

	if (!HasMoveDeviceConverter) return;

	sprintf(Tables, "%s", ImageTableName);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return;
		}
	
	MakeSafeString(SeriesUID, SeriesUIDValue, &aDB);
	MakeSafeString(Device,    DeviceValue,    &aDB);

	sprintf(QueryString, 	"DICOMImages.SeriesInst %s and "
				"DICOMImages.DeviceName %s",
				SeriesUIDValue, DeviceValue);

	if (!aDB.Query(Tables, "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
		{
		OperatorConsole.printf("***Unable to query for image records to run move script\n");
		aDB.Close();
		return;
		}

	aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
	aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);

	while (aDB.NextRecord())
		{ 	
		GetPhysicalDevice(DeviceName, FileName);
		strcat(FileName, ObjectFile);
		ModifyImageFile(FileName, "2000", &PDU, TRUE);
		}

	aDB.Close();	
	}

/////////////////////////////////////////////////////////////////////
// Modify device name for patient on specific device
BOOL
ModifyDeviceNameForPatient (char *Device, const char *PatientID, char *UpdateString)
	{
	Database aDB;
	char Table[256], QueryString[512], PatientIDValue[256], DeviceValue[256], SetString[512];

  	sprintf(Table, "%s", ImageTableName);
	strcpy(SetString, UpdateString);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return FALSE;
		}
	
	MakeSafeString(PatientID,    PatientIDValue, &aDB);
	MakeSafeString(Device,       DeviceValue,    &aDB);


        if (strlen(PatientID))
		sprintf(QueryString, 	"DICOMImages.DeviceName %s and "
					"DICOMImages.ImagePat %s",
					DeviceValue, PatientIDValue);
	else
		{
 		sprintf(QueryString, 	"DICOMImages.DeviceName %s", DeviceValue);
		}

	if(!aDB.UpdateRecords(Table, SetString, QueryString))
		{
		OperatorConsole.printf("***Unable to update image records to new device\n");
		aDB.Close();
		return FALSE;
		}

	aDB.Close();
	return TRUE;
	}

/////////////////////////////////////////////////////////////////////
// Modify device name for study on specific device
BOOL
ModifyDeviceNameForSeries (char *Device, const char *SeriesUID, char *UpdateString)
	{
	Database aDB;
	char Table[256], QueryString[512], SeriesUIDValue[256], DeviceValue[256], SetString[512];

  	sprintf(Table, "%s", ImageTableName);

	strcpy(SetString, UpdateString);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return FALSE;
		}
	
	MakeSafeString(SeriesUID,  SeriesUIDValue, &aDB);
	MakeSafeString(Device,    DeviceValue,     &aDB);

	sprintf(QueryString, 	"DICOMImages.DeviceName %s and "
				"DICOMImages.SeriesInst %s",
				DeviceValue, SeriesUIDValue);

	if(!aDB.UpdateRecords(Table, SetString, QueryString))
		{
		OperatorConsole.printf("***Unable to update image records to new device\n");
		aDB.Close();
		return FALSE;
		}

	aDB.Close();
	return TRUE;
	}

/////////////////////////////////////////////////////////////////////
// move files of a patient specified to be on a device from / to a directory

BOOL
MovePatientData (char *Device, char *PatientID, char *From, char *To)
	{
	Database aDB;
	char Tables[512], DeviceName[512], ObjectFile[512], FileNameFrom[1024], FileNameTo[1024];
	char QueryString[512], PatientIDValue[512], DeviceValue[512], Temp[512];
	SQLLEN SQLResultLength;
	BOOL Status=TRUE;
	unsigned int i;
	int j;

	sprintf(Tables, "%s", ImageTableName);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return FALSE;
		}
	
	MakeSafeString(PatientID, PatientIDValue, &aDB);
	MakeSafeString(Device,    DeviceValue,    &aDB);

	sprintf(QueryString, 	"DICOMImages.ImagePat %s and "
				"DICOMImages.DeviceName %s",
				PatientIDValue, DeviceValue);

	if (!aDB.Query(Tables, "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
		{
		OperatorConsole.printf("***Unable to query for image records to move data\n");
		aDB.Close();
		return FALSE;
		}

	aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
	aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);

	while (aDB.NextRecord())
		{ 	
		strcpy(FileNameFrom, From);
		strcat(FileNameFrom, ObjectFile);

		strcpy(FileNameTo, To);
		strcat(FileNameTo, ObjectFile);

		// make patient directory and any required subdirectories

		for (i=strlen(To); i<strlen(FileNameTo); i++)
			{
			if (FileNameTo[i] == PATHSEPCHAR)
				{
				strcpy(Temp, FileNameTo);
				Temp[i] = 0;
#ifdef WIN32				
				mkdir(Temp);
#else
				mkdir(Temp, 0777);
#endif
				}
			}

		SystemDebug.printf("Moving file: %s to %s\n", FileNameFrom, FileNameTo);
		if (!FileMove(FileNameFrom, FileNameTo))
			{
			OperatorConsole.printf("***Unable to move file: %s to %s\n", FileNameFrom, FileNameTo);
			Status = FALSE;
			break;
			}

		// attempt to remove source directory and any subdirectories (ignore fail if not empty)

		strcpy(Temp, FileNameFrom);
		for (j=strlen(FileNameFrom)-1; j>=(signed int)strlen(From); j--)
			{
			if (Temp[j] == PATHSEPCHAR)
				{
				Temp[j] = 0;
				rmdir(Temp);
				}
			}
		}

	aDB.Close();	

	return Status;
	}

/////////////////////////////////////////////////////////////////////
// copy files of a patient specified to be on a device from / to a directory
BOOL
CopyPatientData (char *Device, char *PatientID, char *From, char *To)
	{
	Database aDB;
	char Tables[512], DeviceName[512], ObjectFile[512], FileNameFrom[1024], FileNameTo[1024];
	char QueryString[512], PatientIDValue[512], DeviceValue[512], Temp[512];
	SQLLEN SQLResultLength;
	BOOL Status=TRUE;
	unsigned int i;

	sprintf(Tables, "%s", ImageTableName);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return FALSE;
		}
	
	MakeSafeString(PatientID, PatientIDValue, &aDB);
	MakeSafeString(Device,    DeviceValue,    &aDB);

	sprintf(QueryString, 	"DICOMImages.ImagePat %s and "
				"DICOMImages.DeviceName %s",
				PatientIDValue, DeviceValue);

	if (!aDB.Query(Tables, "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
		{
		OperatorConsole.printf("***Unable to query for image records to copy data\n");
		aDB.Close();
		return FALSE;
		}

	aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
	aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);

	while (aDB.NextRecord())
		{ 	
		strcpy(FileNameFrom, From);
		strcat(FileNameFrom, ObjectFile);

		strcpy(FileNameTo, To);
		strcat(FileNameTo, ObjectFile);


		// make patient directory and any required subdirectories

		for (i=strlen(To); i<strlen(FileNameTo); i++)
			{
			if (FileNameTo[i] == PATHSEPCHAR)
				{
				strcpy(Temp, FileNameTo);
				Temp[i] = 0;
#ifdef WIN32				
				mkdir(Temp);
#else
				mkdir(Temp, 0777);
#endif
				}
			}

		SystemDebug.printf("Copying file: %s to %s\n", FileNameFrom, FileNameTo);
		if (!FileCopy(FileNameFrom, FileNameTo, 0))
			{
			OperatorConsole.printf("***Unable to copy file: %s to %s\n", FileNameFrom, FileNameTo);
			Status = FALSE;
			break;
			}
		}

	aDB.Close();	

	return Status;
	}

/////////////////////////////////////////////////////////////////////
// copy files of a patient specified to be on a device from / to a directory
BOOL
CopySeriesData (char *Device, char *SeriesUID, char *From, char *To)
	{
	Database aDB;
	char Tables[512], DeviceName[512], ObjectFile[512], FileNameFrom[1024], FileNameTo[1024];
	char QueryString[512], SeriesUIDValue[512], DeviceValue[512], Temp[512];
	SQLLEN SQLResultLength;
	BOOL Status=TRUE;
	unsigned int i;

	sprintf(Tables, "%s", ImageTableName);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return FALSE;
		}
	
	MakeSafeString(SeriesUID, SeriesUIDValue, &aDB);
	MakeSafeString(Device,    DeviceValue,    &aDB);

	sprintf(QueryString, 	"DICOMImages.SeriesInst %s and "
				"DICOMImages.DeviceName %s",
				SeriesUIDValue, DeviceValue);

	if (!aDB.Query(Tables, "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
		{
		OperatorConsole.printf("***Unable to query for image records to copy data\n");
		aDB.Close();
		return FALSE;
		}

	aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
	aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);

	while (aDB.NextRecord())
		{ 	
		strcpy(FileNameFrom, From);
		strcat(FileNameFrom, ObjectFile);

		strcpy(FileNameTo, To);
		strcat(FileNameTo, ObjectFile);


		// make patient directory and any required subdirectories

		for (i=strlen(To); i<strlen(FileNameTo); i++)
			{
			if (FileNameTo[i] == PATHSEPCHAR)
				{
				strcpy(Temp, FileNameTo);
				Temp[i] = 0;
#ifdef WIN32				
				mkdir(Temp);
#else
				mkdir(Temp, 0777);
#endif
				}
			}

		SystemDebug.printf("Copying file: %s to %s\n", FileNameFrom, FileNameTo);
		if (!FileCopy(FileNameFrom, FileNameTo, 0))
			{
			OperatorConsole.printf("***Unable to copy file: %s to %s\n", FileNameFrom, FileNameTo);
			Status = FALSE;
			break;
			}
		}

	aDB.Close();	

	return Status;
	}

/////////////////////////////////////////////////////////////////////
// compare files of a patient specified to be on a device between two directories
BOOL
ComparePatientData (char *Device, char *PatientID, char *From, char *To)
	{
	Database aDB;
	char Tables[512], DeviceName[512], ObjectFile[512], FileNameFrom[1024], FileNameTo[1024];
	char QueryString[512], PatientIDValue[512], DeviceValue[512];
	SQLLEN SQLResultLength;
	BOOL Status=TRUE;

	sprintf(Tables, "%s", ImageTableName);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return FALSE;
		}
	
	MakeSafeString(PatientID, PatientIDValue, &aDB);
	MakeSafeString(Device,    DeviceValue,    &aDB);

	sprintf(QueryString, 	"DICOMImages.ImagePat %s and "
				"DICOMImages.DeviceName %s",
				PatientIDValue, DeviceValue);

	if (!aDB.Query(Tables, "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
		{
		OperatorConsole.printf("***Unable to query for image records to compare data\n");
		aDB.Close();
		return FALSE;
		}

	aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
	aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);

	while (aDB.NextRecord())
		{ 	
		strcpy(FileNameFrom, From);
		strcat(FileNameFrom, ObjectFile);

		strcpy(FileNameTo, To);
		strcat(FileNameTo, ObjectFile);

		SystemDebug.printf("Comparing files: %s and %s\n", FileNameFrom, FileNameTo);
		if (!FileCompare(FileNameFrom, FileNameTo, 0))
			{
			OperatorConsole.printf("***Comparison failed for files: %s and %s\n", FileNameFrom, FileNameTo);
			Status = FALSE;
			break;
			}
		}

	aDB.Close();	

	return Status;
	}


/////////////////////////////////////////////////////////////////////
// compare files of a patient specified to be on a device between two directories
BOOL
CompareSeriesData (char *Device, char *SeriesUID, char *From, char *To)
	{
	Database aDB;
	char Tables[512], DeviceName[512], ObjectFile[512], FileNameFrom[1024], FileNameTo[1024];
	char QueryString[512], SeriesUIDValue[512], DeviceValue[512];
	SQLLEN SQLResultLength;
	BOOL Status=TRUE;

	sprintf(Tables, "%s", ImageTableName);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return FALSE;
		}
	
	MakeSafeString(SeriesUID, SeriesUIDValue, &aDB);
	MakeSafeString(Device,    DeviceValue,    &aDB);

	sprintf(QueryString, 	"DICOMImages.SeriesInst %s and "
				"DICOMImages.DeviceName %s",
				SeriesUIDValue, DeviceValue);

	if (!aDB.Query(Tables, "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
		{
		OperatorConsole.printf("***Unable to query for image records to compare data\n");
		aDB.Close();
		return FALSE;
		}

	aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
	aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);

	while (aDB.NextRecord())
		{ 	
		strcpy(FileNameFrom, From);
		strcat(FileNameFrom, ObjectFile);

		strcpy(FileNameTo, To);
		strcat(FileNameTo, ObjectFile);

		SystemDebug.printf("Comparing files: %s and %s\n", FileNameFrom, FileNameTo);
		if (!FileCompare(FileNameFrom, FileNameTo, 0))
			{
			OperatorConsole.printf("***Comparison failed for files: %s and %s\n", FileNameFrom, FileNameTo);
			Status = FALSE;
			break;
			}
		}

	aDB.Close();	

	return Status;
	}

/////////////////////////////////////////////////////////////////////
// test read single file
BOOL TestFile(char *FullFilename, char *status)
	{
	int 		len = strlen(FullFilename);
	PDU_Service	PDU;
	int 		UseChapter10 = 0;
	DICOMDataObject	*DDOOut;
	VR		*vr;

	PDU.AttachRTC(&VRType);

	if (len>4)
		UseChapter10 = (stricmp(FullFilename+len-4, ".dcm")==0);

	if (!UseChapter10)
	  	DDOOut = LoadImplicitLittleEndianFile(FullFilename, 0xffffffffu);
	else	
		DDOOut = PDU.LoadDICOMDataObject(FullFilename);

	// can we read the file ?
	if (!DDOOut) 
		{
		strcpy(status, "failed to read");
		return FALSE;
		}

	// expect PatientID
	vr = DDOOut->GetVR(0x0010, 0x0020);
	if(!vr)
		{
		strcpy(status, "PatientID missing");
		delete DDOOut;
		return FALSE;
		}

	// expect StudyInstanceUID
        vr = DDOOut->GetVR(0x0020, 0x000d);
	if(!vr)
		{
		strcpy(status, "StudyInstanceUID missing");
		delete DDOOut;
		return FALSE;
		}

	// expect SeriesInstanceUID
	vr = DDOOut->GetVR(0x0020, 0x000e);
	if(!vr)
		{
		strcpy(status, "SeriesInstanceUID missing");
		delete DDOOut;
		return FALSE;
		}

	// expect SOPInstanceUID
	vr = DDOOut->GetVR(0x0008, 0x0018);
	if(!vr)
		{
		strcpy(status, "SOPInstanceUID missing");
		delete DDOOut;
		return FALSE;
		}

	// This will try to decompress the pixel data and check the CRC

	if (!recompress(&DDOOut, "un", "", FALSE, NULL))
	//if (!DecompressNKI(DDOOut))
		{
		strcpy(status, "decompression or CRC error");
		delete DDOOut;
		return FALSE;
		}

	delete DDOOut;
	return TRUE;
	}

/////////////////////////////////////////////////////////////////////
// test files of a patient specified to be on a device
BOOL
TestPatientData (char *Device, char *PatientID, char *From)
	{
	Database aDB;
	char Tables[512], DeviceName[512], ObjectFile[1024], FileNameFrom[1024];
	char QueryString[512], PatientIDValue[512], DeviceValue[512];
	SQLLEN SQLResultLength;
	BOOL Status=TRUE;
	char error[256];

	sprintf(Tables, "%s", ImageTableName);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return FALSE;
		}
	
	MakeSafeString(PatientID, PatientIDValue, &aDB);
	MakeSafeString(Device,    DeviceValue,    &aDB);

	sprintf(QueryString, 	"DICOMImages.ImagePat %s and "
				"DICOMImages.DeviceName %s",
				PatientIDValue, DeviceValue);

	if (!aDB.Query(Tables, "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
		{
		OperatorConsole.printf("***Unable to query for image records to compare data\n");
		aDB.Close();
		return FALSE;
		}

	aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
	aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);

	while (aDB.NextRecord())
		{ 	
		strcpy(FileNameFrom, From);
		strcat(FileNameFrom, ObjectFile);

		SystemDebug.printf("Testing file: %s\n", FileNameFrom);
		if (!TestFile(FileNameFrom, error))
			{
			OperatorConsole.printf("***Test read failed for file: %s (%s)\n", FileNameFrom, error);
			Status = FALSE;
			}
		}

	aDB.Close();	

	return Status;
	}

/////////////////////////////////////////////////////////////////////
// test files of a patient specified to be on a device
BOOL
TestSeriesData (char *Device, char *SeriesUID, char *From)
	{
	Database aDB;
	char Tables[512], DeviceName[512], ObjectFile[1024], FileNameFrom[1024];
	char QueryString[512], SeriesUIDValue[512], DeviceValue[512];
	SQLLEN SQLResultLength;
	BOOL Status=TRUE;
	char error[256];

	sprintf(Tables, "%s", ImageTableName);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return FALSE;
		}
	
	MakeSafeString(SeriesUID, SeriesUIDValue, &aDB);
	MakeSafeString(Device,    DeviceValue,    &aDB);

	sprintf(QueryString, 	"DICOMImages.SeriesInst %s and "
				"DICOMImages.DeviceName %s",
				SeriesUIDValue, DeviceValue);

	if (!aDB.Query(Tables, "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
		{
		OperatorConsole.printf("***Unable to query for image records to compare data\n");
		aDB.Close();
		return FALSE;
		}

	aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
	aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);

	while (aDB.NextRecord())
		{ 	
		strcpy(FileNameFrom, From);
		strcat(FileNameFrom, ObjectFile);

		SystemDebug.printf("Testing file: %s\n", FileNameFrom);
		if (!TestFile(FileNameFrom, error))
			{
			OperatorConsole.printf("***Test read failed for file: %s (%s)\n", FileNameFrom, error);
			Status = FALSE;
			}
		}

	aDB.Close();	

	return Status;
	}

/////////////////////////////////////////////////////////////////////
// Delete files of a patient specified to be on a device from a given directory
// but only if the file is present in To (may be NULL to disable this check)

BOOL
DeletePatientData (char *Device, char *PatientID, char *From, char *To)
	{
	Database aDB;
	char Tables[512], DeviceName[512], ObjectFile[1024], FileNameFrom[1024], FileNameTo[1024];
	char QueryString[512], PatientIDValue[512], DeviceValue[512], Temp[512];
	SQLLEN SQLResultLength;
	BOOL Status=TRUE;
	int i;

	sprintf(Tables, "%s", ImageTableName);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return FALSE;
		}
	
	MakeSafeString(PatientID, PatientIDValue, &aDB);
	MakeSafeString(Device,    DeviceValue,    &aDB);

	sprintf(QueryString, 	"DICOMImages.ImagePat %s and "
				"DICOMImages.DeviceName %s",
				PatientIDValue, DeviceValue);

	if (!aDB.Query(Tables, "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
		{
		OperatorConsole.printf("***Unable to query for image records to delete data\n");
		aDB.Close();
		return FALSE;
		}

	aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
	aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);

	while (aDB.NextRecord())
		{
		BOOL t, f;

		strcpy(FileNameFrom, From);
		strcat(FileNameFrom, ObjectFile);

		if (To)
			{
			strcpy(FileNameTo, To);
			strcat(FileNameTo, ObjectFile);

			t = FileExists(FileNameTo);
			f = FileExists(FileNameFrom);

			// it is already copied but there is no source file - ignore
        	        if (t && !f) continue;

			// it is not copied but we want to delete the source file - this is an error
                	if (!t && f)
				{
				OperatorConsole.printf("***Attempt to delete file that was not copied: %s\n", FileNameFrom);
				Status = FALSE;
				break;
				}
			}

		SystemDebug.printf("Deleting file: %s\n", FileNameFrom);
		if (unlink(FileNameFrom))
			{
			OperatorConsole.printf("***Unable to delete file: %s\n", FileNameFrom);
			Status = FALSE;
			// break; mvh 20110105: do not stop deleting on single error
			}

		// attempt to remove source directory and any subdirectories (ignore fail if not empty)

		strcpy(Temp, FileNameFrom);
		for (i=strlen(FileNameFrom)-1; i>=(signed int)strlen(From); i--)
			{
			if (Temp[i] == PATHSEPCHAR)
				{
				Temp[i] = 0;
				rmdir(Temp);
				}
			}
		}

	aDB.Close();

	return Status;
	}

/////////////////////////////////////////////////////////////////////
// Delete files of a patient specified to be on a device from a given directory
// but only if the file is present in To (may be NULL to disable this check)

BOOL
DeleteSeriesData (char *Device, char *SeriesUID, char *From, char *To)
	{
	Database aDB;
	char Tables[512], DeviceName[512], ObjectFile[1024], FileNameFrom[1024], FileNameTo[1024];
	char QueryString[512], SeriesUIDValue[512], DeviceValue[512], Temp[512];
	SQLLEN SQLResultLength;
	BOOL Status=TRUE;
	int i;

	sprintf(Tables, "%s", ImageTableName);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return FALSE;
		}
	
	MakeSafeString(SeriesUID, SeriesUIDValue, &aDB);
	MakeSafeString(Device,    DeviceValue,    &aDB);

	sprintf(QueryString, 	"DICOMImages.SeriesInst %s and "
				"DICOMImages.DeviceName %s",
				SeriesUIDValue, DeviceValue);

	if (!aDB.Query(Tables, "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
		{
		OperatorConsole.printf("***Unable to query for image records to delete data\n");
		aDB.Close();
		return FALSE;
		}

	aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
	aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);

	while (aDB.NextRecord())
		{
		BOOL t, f;

		strcpy(FileNameFrom, From);
		strcat(FileNameFrom, ObjectFile);

		if (To)
			{
			strcpy(FileNameTo, To);
			strcat(FileNameTo, ObjectFile);

			t = FileExists(FileNameTo);
			f = FileExists(FileNameFrom);

			// it is already copied but there is no source file - ignore
        	        if (t && !f) continue;

			// it is not copied but we want to delete the source file - this is an error
                	if (!t && f)
				{
				OperatorConsole.printf("***Attempt to delete file that was not copied: %s\n", FileNameFrom);
				Status = FALSE;
				break;
				}
			}

		SystemDebug.printf("Deleting file: %s\n", FileNameFrom);
		if (unlink(FileNameFrom))
			{
			OperatorConsole.printf("***Unable to delete file: %s\n", FileNameFrom);
			Status = FALSE;
			// break; mvh 20110105: do not stop deleting on single error
			}

		// attempt to remove source directory and any subdirectories (ignore fail if not empty)

		strcpy(Temp, FileNameFrom);
		for (i=strlen(FileNameFrom)-1; i>=(signed int)strlen(From); i--)
			{
			if (Temp[i] == PATHSEPCHAR)
				{
				Temp[i] = 0;
				rmdir(Temp);
				}
			}
		}

	aDB.Close();

	return Status;
	}

////////////////////////////////////////////////////////////////////////
// assume DeviceTo = 'JUKEBOX1.123' and CACHEDevice0 = d:\davis\cache_%d_04d
// then this routine sets PhysicalCache to d:\davis\cache_1_0123 (if this directory exists)

BOOL
FindCacheDirectory(char *DeviceTo, char *PhysicalCache)
	{
	unsigned int i;
	char 	Temp[1024];
	char	*p;
	int 	JUKEBOXDevice=-1, CDNumber=-1;//, CacheNumber=-1;

	if(strnicmp(DeviceTo, "JUKEBOX", 7)==0)
		{
		JUKEBOXDevice = atoi(DeviceTo + 7);
		p = strchr(DeviceTo, '.');
		if (p)	
			CDNumber = atoi(p+1);
		if (CDNumber < 0)
			{
			OperatorConsole.printf("Archival: *** to-device (%s) has no CD number specified\n", DeviceTo);
			return FALSE;
			}
		}
	else
		{
		OperatorConsole.printf("Archival: *** to-device (%s) should have been a JUKEBOX device\n", DeviceTo);
		return FALSE;
		}

	for (i=0; i<CACHEDevices; i++)
		{
		sprintf(Temp, "CACHE%d.%d.%d", i, JUKEBOXDevice, CDNumber);
		GetPhysicalDevice(Temp, PhysicalCache);
		if (DirectoryExists(PhysicalCache))
			{
			SystemDebug.printf("Located cache directory: %s\n", PhysicalCache);
			return TRUE;
			}
		}

	PhysicalCache[0] = 0;
	OperatorConsole.printf("Archival: *** could not find cache directory for: %s\n", DeviceTo);
	return FALSE;
	}

////////////////////////////////////////////////////////////////////////
// Make list of least recently accessed patients up to specified KB (allocates 
// list and returns amount). If KB=0x7fffffff, all patients are selected (slowly).
// Maximal 10 LRU patients are inspected more to try and fill exactly the specified KB
// This routine will also recompress the data for archival. Note: up to 10 patients
// more are compressed as will be archived.
// KB<0 select most recent patients

static int
MakeListOfLRUPatients(char *Device, int KB, char **PatientIDList, char *LRUSort, ExtendedPDU_Service *PDU)
	{
	// Query the patient table, and sort it by access time
	SQLLEN			sdword,sdword1;
	DWORD			iTime;
	Database		DB;
//	char			s[255];
	char			PatientID[256];
	int			KBTotal, KBPatient, Patients, RunOver=0, Result=0, i;
	int 			MAXPATIENTS = 10000;

	// new code that sorts on e.g. studydate (but default on accesstime)
        // now also use this code

	if (LRUSort[0] || 1)
		{
		Patients = MakeListOfOldestPatientsOnDevice(PatientIDList, KB>0 ? MAXPATIENTS : -MAXPATIENTS, Device, LRUSort);
		KBTotal  = 0;

		for (i=0; i<Patients; i++)
			{
			strncpy(PatientID, *PatientIDList + 256*i, 255);
			PatientID[255]=0;

			RecompressPatient (Device, PatientID, ArchiveCompression, PDU);

			KBPatient = GetKBUsedForPatient (Device, PatientID, KB!=0x7fffffff);
			if (KBPatient + KBTotal > abs(KB))
				{
				RunOver++;
				if (RunOver==10) 
					break;
				else 
					continue;
				}

			KBTotal += KBPatient;
			strcpy(*PatientIDList + 256*Result, PatientID);

			if (KBPatient>0) Result++;
			}

		return Result;
		}

	// original code that sorts on database order (no longer used)

	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		return -1;

	if (!DB.Query(PatientTableName, "AccessTime, PatientID", NULL, "AccessTime"))
		return -1;

	if(!DB.BindField (1, SQL_C_ULONG, &iTime, 4, &sdword1))
		return -1;

	if(!DB.BindField (2, SQL_C_CHAR, PatientID, 255, &sdword))
		return -1;

	*PatientIDList = (char *)malloc(256 * MAXPATIENTS);
	if (*PatientIDList==NULL) return -1;

	KBTotal        = 0;
	Patients       = 0;

	while(DB.NextRecord())
		{
		if (Patients == MAXPATIENTS)
			break;

		RecompressPatient (Device, PatientID, ArchiveCompression, PDU);

		KBPatient = GetKBUsedForPatient (Device, PatientID, KB!=0x7fffffff);
		if (KBPatient + KBTotal > KB)
			{
			RunOver++;
			if (RunOver==10) 
				break;
			else 
				continue;
			}

		KBTotal += KBPatient;

		strcpy(*PatientIDList + 256*Patients, PatientID);

		if (KBPatient>0) Patients++;
		}

	DB.Close();
	return Patients;
	}


////////////////////////////////////////////////////////////////////////
// Make list of all patients which have at least one image on a given device
// The list is sorted on accesstime

int
MakeListOfPatientsOnDevice(char *Device, char **PatientIDList)
	{
	// Query the patient table, and sort it by access time
	SQLLEN			sdword,sdword1;
	DWORD			iTime;
	Database		DB;
	char			s[256], DeviceValue[256];
	char			Tables[256], PatientID[256], QueryString[512], Dum[256], FieldString[512], SortString[256];
	int 			Patients, NPat, MAXPATIENTS = 10000, i;
	char			*PatList;
	
	*PatientIDList = NULL;
	
	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		return -1;

	MakeSafeString(Device,  DeviceValue, &DB);

	// query is hand-written since built-in dbase driver cannot handle the original one

	if (DB.db_type == DT_DBASEIII)
		{
		// count #patients
		sprintf(SortString, "%s.AccessTime", PatientTableName);
		if (!DB.Query(PatientTableName, "PatientID", "", SortString))
			return -1;

		DB.BindField (1, SQL_C_CHAR, Dum, 255, &sdword);

		NPat = 0;
		while(DB.NextRecord())
			{
			NPat++;
			}

		// some new patients may recently have come in!
		PatList = (char *)malloc(68 * (NPat + 100));
		if (PatList==NULL) return -1;

		if (!DB.QueryDistinct(PatientTableName, "PatientID", "", "DICOMPatients.AccessTime"))
			{
			free(PatList);
			return -1;
			}

		if(!DB.BindField (1, SQL_C_CHAR, PatientID, 68, &sdword))
			{
			free(PatList);
			return -1;
			}

		NPat = 0;
		while(DB.NextRecord())
			{
			strcpy(PatList + 68*NPat, PatientID);
			NPat++;
			}

		// now we have the list all patients, query the image table for each of them and return those on the selected device
		*PatientIDList = (char *)malloc(256 * MAXPATIENTS);
		if (*PatientIDList==NULL) 
			{
			free(PatList);
			return -1;
			}

                Patients       = 0;

		for (i=0; i<NPat; i++)
			{
			if (Patients==MAXPATIENTS) 
				break;

			MakeSafeString(PatList + 68*i, s, &DB);
	
			sprintf(QueryString, 	"DICOMImages.DeviceName %s and "
						"DICOMImages.ImagePat %s",
						DeviceValue, s);

			if (!DB.Query(ImageTableName, "ImagePat", QueryString, ""))
				{
				free (PatList);
				return -1;
				}

  			DB.BindField (1, SQL_C_CHAR, Dum, 255, &sdword);

 			if (DB.NextRecord())
				{
				strcpy(*PatientIDList + 256*Patients, PatList + 68 * i);
				Patients++;
				}
			}

		free (PatList);
		DB.Close();
		return Patients;
		}
	

	// This original query cannot be handled by the built-in driver !

	sprintf(Tables, "%s, %s, %s, %s",
		ImageTableName, SeriesTableName, StudyTableName, PatientTableName);

	sprintf(QueryString, 	"%s.DeviceName %s and "
				"%s.PatientID = %s.PatientID and "
				"%s.SeriesInst = %s.SeriesInst and "
				"%s.StudyInsta = %s.StudyInsta",
				ImageTableName, DeviceValue, StudyTableName, PatientTableName, ImageTableName, SeriesTableName, 
				SeriesTableName, StudyTableName);
				
	sprintf(FieldString, "%s.PatientID, %s.AccessTime", PatientTableName, PatientTableName); 
	sprintf(SortString,  "%s.AccessTime", PatientTableName); 

	if (!DB.QueryDistinct(Tables, FieldString, QueryString, SortString))
		return -1;

	if(!DB.BindField (1, SQL_C_CHAR, PatientID, 255, &sdword))
		return -1;

	if(!DB.BindField (2, SQL_C_ULONG, &iTime, 4, &sdword1))
		return -1;

	*PatientIDList = (char *)malloc(256 * MAXPATIENTS);
	if (*PatientIDList==NULL) 
		return -1;

	Patients       = 0;

	while(DB.NextRecord())
		{
		if (Patients == MAXPATIENTS)
			break;

		strcpy(*PatientIDList + 256*Patients, PatientID);

		Patients++;
		}

	DB.Close();
	return Patients;
	}

////////////////////////////////////////////////////////////////////////
// This routine replaces devicenames like 'MAG0.xxxxx' in database by 'MAG0' (undo archiving)
// Works for all MAG devices

BOOL RestoreMAGFlags()
	{
	unsigned int DeviceNo;
	char    FromName[20], ToString[256];

	for (DeviceNo=0; DeviceNo<MAGDevices; DeviceNo++)
		{
		sprintf(FromName, "MAG%d.*", DeviceNo);
		sprintf(ToString, "DeviceName = 'MAG%d'", DeviceNo);

		OperatorConsole.printf("Resetting archive flag on device: MAG%d\n", DeviceNo);
		if (!ModifyDeviceNameForPatient (FromName, "", ToString))
			{
			OperatorConsole.printf("Archival: *** could not restore device name (%s)\n", ToString);
			}
		}

	return TRUE;
	}


////////////////////////////////////////////////////////////////////////
// Select LRU patients from a single device up to the specified amount of KB 
// and set their flags to archival; if KB is smaller than 0 this routine selects MRU

BOOL
SelectLRUForArchival(char *Device, int KB, ExtendedPDU_Service *PDU)
	{
	int 	i, NPatients, Size, Total;
	char 	*PatientIDList, *PatientID;
	char	DeviceArchiving[256], DeviceString[256];

	// process input parameters
	
	sprintf(DeviceArchiving, "%s.Archiving", Device);
	sprintf(DeviceString,    "DeviceName = '%s.Archiving'", Device);

	// check if we already selected some patients for archiving from this device

	NPatients = MakeListOfPatientsOnDevice(DeviceArchiving, &PatientIDList);
	if (NPatients > 0)
		{
		OperatorConsole.printf("Archival: *** Patients were already selected for archiving\n");
		free (PatientIDList);
		return FALSE;
		}
	free (PatientIDList);

	// make list of patients that are not yet archived (also recompresses)

	NPatients = MakeListOfLRUPatients(Device, KB, &PatientIDList, LRUSort, PDU);
	if (NPatients < 0)
		{
		OperatorConsole.printf("Archival: *** could not create patient list\n");
		return FALSE;
		}

	// set flags from e.g., MAG0 to MAG0.Archiving

	for (i=0; i<NPatients; i++)
		{
		PatientID = PatientIDList + i*256;
		if (!ModifyDeviceNameForPatient (Device, PatientID, DeviceString))
			{
			OperatorConsole.printf("Archival: *** could not set device name to archiving for patient: %s\n", PatientID);
			free (PatientIDList);
			RestoreMAGFlags();
			return FALSE;
			}
		else
			OperatorConsole.printf("Archival: set archive flag for patient: %s\n", PatientID);
		}

	// compute amount for double check

	Total = 0;
	for (i=0; i<NPatients; i++)
		{
		PatientID = PatientIDList + i*256;
		Size = GetKBUsedForPatient (DeviceArchiving, PatientID, 1);
		if (Size < 0)
			{
			OperatorConsole.printf("Archival: *** could not compute KB used for patient: %s\n", PatientID);
			free (PatientIDList);
			RestoreMAGFlags();
			return FALSE;
			}
		else
			OperatorConsole.printf("Archival: %6d KB will be written for patient: %s\n", Size, PatientID);

		Total += Size;
		}

	if (Total==0)
		{
		OperatorConsole.printf("Archival: nothing to do\n");
		free (PatientIDList);
		return FALSE;
		}
	else
		OperatorConsole.printf("Archival: total archive amount: %d KB\n", Total);


	free (PatientIDList);
	return TRUE;
	}

////////////////////////////////////////////////////////////////////////
// Make list of all series on a given device older than age (>0) or younger (age<0) days
// limit to a maximum size of kb (if >0) passed
// The list is sorted on studydate

int
MakeListOfSeriesOnDevice(char *Device, char **SeriesList, int age, int kb)
	{
	// Query the patient table, and sort it by access time
	SQLLEN			sdword,sdword1,sdword2;
	Database		DB;
	char			DeviceValue[256];
	char			Tables[256], SeriesInst[256], SeriesDate[256], AccessTime[256], QueryString[512], FieldString[512], SortString[256];
	int 			Series, MAXSERIES = 10000, i, Size;
	struct tm		tmbuf1;
	
	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		return -1;

	MakeSafeString(Device,  DeviceValue, &DB);

	sprintf(Tables, "%s, %s", ImageTableName, SeriesTableName);

	sprintf(QueryString, 	"%s.DeviceName %s and "
				"%s.SeriesInst = %s.SeriesInst",
				ImageTableName, DeviceValue, ImageTableName, SeriesTableName);
				
	sprintf(FieldString, "%s.SeriesInst, %s.SeriesDate, %s.AccessTime", SeriesTableName,  SeriesTableName,  SeriesTableName);
	sprintf(SortString, "%s.SeriesDate", SeriesTableName);

	if (!DB.QueryDistinct(Tables, FieldString, QueryString, SortString))
		return -1;

	if(!DB.BindField (1, SQL_C_CHAR, SeriesInst, 255, &sdword))
		return -1;

	if(!DB.BindField (2, SQL_C_CHAR, SeriesDate, 255, &sdword1))
		return -1;

	if(!DB.BindField (3, SQL_C_CHAR, AccessTime, 255, &sdword2))
		return -1;

	*SeriesList = (char *)malloc(256 * MAXSERIES);
	if (*SeriesList==NULL) return -1;

	Series       = 0;
	Size         = 0;

	while(DB.NextRecord())
		{
		if (Series == MAXSERIES) break;

#if 1
		memset(&tmbuf1, 0, sizeof(tmbuf1));
		sscanf(SeriesDate, "%04d%02d%02d", &tmbuf1.tm_year, &tmbuf1.tm_mon, &tmbuf1.tm_mday);
		tmbuf1.tm_year -= 1900;
		time_t t = mktime(&tmbuf1);
#else
		time_t t = (time_t)atoi(AccessTime);
#endif

		if (age>0 && t>time(NULL)-age*24*3600) continue;
		if (age<0 && t<time(NULL)+age*24*3600) continue;

		strcpy(*SeriesList + 256*Series, SeriesInst);
		Series++;

		if (kb>0)
			{
			Size += GetKBUsedForSeries (Device, SeriesInst, 1);
			if (Size>kb) break;
			}
		}

	DB.Close();
	return Series;
	}

////////////////////////////////////////////////////////////////////////
// Select series from a single device older than a specified age

BOOL
SelectSeriesForArchival(char *Device, int age, int kb)
	{
	int 	i, NSeries, Size, Total;
	char 	*SeriesList, *SeriesUID;
	char	DeviceArchiving[256], DeviceString[256];

	// process input parameters
	
	sprintf(DeviceArchiving, "%s.Archiving", Device);
	//strcpy (DeviceString,    "DeviceName = DeviceName + '.Archiving'");
	sprintf(DeviceString,    "DeviceName = '%s.Archiving'", Device);

	// make list of patients that are not yet archived

	NSeries = MakeListOfSeriesOnDevice(Device, &SeriesList, age, kb);
	if (NSeries < 0)
		{
		OperatorConsole.printf("Archival: *** could not create series list\n");
		return FALSE;
		}

	// set flags from e.g., MAG0 to MAG0.Archiving

	for (i=0; i<NSeries; i++)
		{
		SeriesUID = SeriesList + i*256;
		if (!ModifyDeviceNameForSeries (Device, SeriesUID, DeviceString))
			{
			OperatorConsole.printf("Archival: *** could not set device name to archiving for series: %s\n", SeriesUID);
			free (SeriesList);
			RestoreMAGFlags();
			return FALSE;
			}
		else
			OperatorConsole.printf("Archival: set archive flag for series: %s\n", SeriesUID);
		}

	free (SeriesList);
	return TRUE;
	}

////////////////////////////////////////////////////////////////////////
// example: assume DeviceFrom=MAG0 (or MAG0.Archiving), DeviceTo = 'JUKEBOX1.123' and 
// CACHEDevice0 = d:\davis\cache_%d_%04d then this routine creates and fills directory
// 'd:\davis\cache_1_0123' from MAG0 and deletes the original data.  The DeviceNames in 
// the database is set to 'JUKEBOX1.123' which works both before and after burning.

BOOL
PrepareBunchForBurning(char *DeviceFrom, char *DeviceTo)
	{
	int 	i, NPatients, Size, Total;
	char 	*PatientIDList, *PatientID, *p;
	char 	PhysicalFrom[1024], PhysicalTo[1024], CacheDevice[1024], Temp[1024], PhysicalCache[1024];
	int 	JUKEBOXDevice=-1, CDNumber=-1, CacheNumber=-1;
	char	DeviceFromArchiving[256], DeviceToString[256];
	
	
	// process input parameters

	strcpy(DeviceFromArchiving, DeviceFrom);
	if (!strstr(DeviceFromArchiving, ".Archiving"))
		strcat(DeviceFromArchiving, ".Archiving");

	sprintf(DeviceToString, "DeviceName = '%s'", DeviceTo);

	if (!GetPhysicalDevice(DeviceFrom, PhysicalFrom))
		{
		OperatorConsole.printf("Archival: *** invalid from-device: %s\n", DeviceFrom);
		return FALSE;
		}

	if (!GetPhysicalDevice(DeviceTo, PhysicalTo))
		{
		OperatorConsole.printf("Archival: *** invalid to-device: %s\n", DeviceTo);
		return FALSE;
		}

	if(strnicmp(DeviceTo, "JUKEBOX", 7)==0)
		{
		JUKEBOXDevice = atoi(DeviceTo + 7);
		p = strchr(DeviceTo, '.');
		if (p)	
			CDNumber = atoi(p+1);
		if (CDNumber < 0)
			{
			OperatorConsole.printf("Archival: *** to-device (%s) has no CD number specified\n", DeviceTo);
			return FALSE;
			}
		}
	else
		{
		OperatorConsole.printf("Archival: *** to-device (%s) should have been a JUKEBOX device\n", DeviceTo);
		return FALSE;
		}

	// check if the CD is unique: exit if it has been burnt before !

	NPatients = MakeListOfPatientsOnDevice(DeviceTo, &PatientIDList);
	if (NPatients > 0)
		{
		OperatorConsole.printf("Archival: *** jukebox device has been used before: %s\n", DeviceTo);
		free (PatientIDList);
		return FALSE;
		}
	free (PatientIDList);


	// make list of patients that are busy with archiving

	NPatients = MakeListOfPatientsOnDevice(DeviceFromArchiving, &PatientIDList);
	if (NPatients < 0)
		{
		OperatorConsole.printf("Archival: *** could not create patient list\n");
		free (PatientIDList);
		return FALSE;
		}

	// compute amount for double check

	Total = 0;
	for (i=0; i<NPatients; i++)
		{
		PatientID = PatientIDList + i*256;
		Size = GetKBUsedForPatient (DeviceFromArchiving, PatientID, 1);
		if (Size < 0)
			{
			OperatorConsole.printf("Archival: *** could not compute KB used for patient: %s\n", PatientID);
			free (PatientIDList);
			// RestoreMAGFlags();
			return FALSE;
			}

		Total += Size;
		}

	// something to do ?

	if (Total==0)
		{
		OperatorConsole.printf("Archival: Nothing to do\n");
		free (PatientIDList);
		// RestoreMAGFlags();
                return FALSE;
		}

	// find cache drive with enough space to store the data
	// NOTE: SPACE CHECK IS IN PRINCIPLE NOT NECESSARY IF DATA IS MOVED !

	CacheNumber = MaxCACHEDevice((Total+10000)/1000);
	if (CacheNumber < 0)
		{
		OperatorConsole.printf("Archival: *** could not find cache drive with enough space (%d KB)\n", Total+10000);
		free (PatientIDList);
		// RestoreMAGFlags();
		return FALSE;
		}

	sprintf(CacheDevice, "CACHE%d.%d.%d", CacheNumber, JUKEBOXDevice, CDNumber);

	if (!GetPhysicalDevice(CacheDevice, PhysicalCache))
		{
		OperatorConsole.printf("Archival: *** invalid cache-device: %s\n", PhysicalCache);
		free (PatientIDList);
		// RestoreMAGFlags();
		return FALSE;
		}

	// attemp to make the cache directory

	OperatorConsole.printf("Archival: writing to cache-device: %s\n", PhysicalCache);

	strcpy(Temp, PhysicalCache);
	Temp[strlen(Temp)-1]=0;
#ifdef WIN32				
	mkdir(Temp);
#else
	mkdir(Temp, 0777);
#endif

	// Same device: move the data (fast) and set the devicename to the CD in the jukebox
	// NOTE: does not check UNC names; only drive letters!

	if (PhysicalCache[1] == ':' && memicmp(PhysicalCache, PhysicalFrom, 2)==0)
		{
		for (i=0; i<NPatients; i++)
			{
			PatientID = PatientIDList + i*256;
			if (!MovePatientData (DeviceFromArchiving, PatientID, PhysicalFrom, PhysicalCache))
				{
				OperatorConsole.printf("Archival: *** could not move image data for patient: %s\n", PatientID);
				free (PatientIDList);
				// RestoreMAGFlags();
				return FALSE;
				}
			else
				OperatorConsole.printf("Archival: moved data of patient: %s\n", PatientID);
			}

		if (!ModifyDeviceNameForPatient (DeviceFromArchiving, "", DeviceToString))
			{
			OperatorConsole.printf("Archival: *** could not set new device name\n");
			free (PatientIDList);
			// RestoreMAGFlags();
			return FALSE;
                        }
		}

	// different device: copy the data, compare it, set devicename to jukebox and delete source data

	else
		{
		for (i=0; i<NPatients; i++)
			{
			PatientID = PatientIDList + i*256;
			if (!CopyPatientData (DeviceFromArchiving, PatientID, PhysicalFrom, PhysicalCache))
				{
				OperatorConsole.printf("Archival: *** could not copy image data for patient: %s\n", PatientID);
				free (PatientIDList);
				// RestoreMAGFlags();
				return FALSE;
				}
			else
				OperatorConsole.printf("Archival: copied image data for patient: %s\n", PatientID);
			}

		// check of copy
		for (i=0; i<NPatients; i++)
			{
			PatientID = PatientIDList + i*256;
			if (!ComparePatientData (DeviceFromArchiving, PatientID, PhysicalFrom, PhysicalCache))
				{
				OperatorConsole.printf("Archival: *** error comparing data for patient: %s\n", PatientID);
				free (PatientIDList);
				// RestoreMAGFlags();
				return FALSE;
				}
			else
				OperatorConsole.printf("Archival: compared image data for patient: %s\n", PatientID);
			}

		if (!ModifyDeviceNameForPatient (DeviceFromArchiving, "", DeviceToString))
			{
			OperatorConsole.printf("Archival: *** could not set new device name\n");
			free (PatientIDList);
			// RestoreMAGFlags();
			return FALSE;
                        }


		for (i=0; i<NPatients; i++)
			{
			PatientID = PatientIDList + i*256;

			SystemDebug.printf("To delete: %s %s from %s\n", DeviceTo, PatientID, PhysicalFrom);

										   // extra check: file must be copied
			if (!DeletePatientData (DeviceTo, PatientID, PhysicalFrom, PhysicalCache))
				{
				OperatorConsole.printf("Archival: *** error deleting data for patient: %s\n", PatientID);
				free (PatientIDList);
				// RestoreMAGFlags();
				return FALSE;
				}
			else
				OperatorConsole.printf("Archival: deleted on-line image data for patient: %s\n", PatientID);
			}
		}

	free (PatientIDList);
        return TRUE;
	}

////////////////////////////////////////////////////////////////////////
// example: assume DeviceFrom=MAG0.Archiving, DeviceTo = MAG1 then this routine 
// moves all data selected for archiving to MAG1 and sets the DeviceName to MAG1

BOOL
MoveDataToDevice(char *DeviceFrom, char *DeviceTo)
	{
	int 	i, NPatients, Size, Total;
	char 	*PatientIDList, *PatientID;
	char 	PhysicalFrom[1024], PhysicalTo[1024], Temp[1024];
	char	DeviceToString[256];
		
	// process input parameters

	sprintf(DeviceToString, "DeviceName = '%s'", DeviceTo);

	if (!GetPhysicalDevice(DeviceFrom, PhysicalFrom))
		{
		OperatorConsole.printf("MoveDataToDevice: *** invalid from-device: %s\n", DeviceFrom);
		return FALSE;
		}

	if (!GetPhysicalDevice(DeviceTo, PhysicalTo))
		{
		OperatorConsole.printf("MoveDataToDevice: *** invalid to-device: %s\n", DeviceTo);
		return FALSE;
		}

	// make list of patients that are on DeviceFrom

	NPatients = MakeListOfPatientsOnDevice(DeviceFrom, &PatientIDList);
	if (NPatients < 0)
		{
		OperatorConsole.printf("MoveDataToDevice: *** could not create patient list\n");
		free (PatientIDList);
		return FALSE;
		}

	// compute amount of data to move

	Total = 0;
	for (i=0; i<NPatients; i++)
		{
		PatientID = PatientIDList + i*256;

		ProcessMoveDevicePatient (DeviceFrom, PatientID);
		Size = GetKBUsedForPatient (DeviceFrom, PatientID, 1);
		if (Size < 0)
			{
			OperatorConsole.printf("MoveDataToDevice: *** could not compute KB used for patient: %s\n", PatientID);
			free (PatientIDList);
			return FALSE;
			}

		Total += Size;
		}

	// something to do ?

	if (Total==0)
		{
		OperatorConsole.printf("MoveDataToDevice: Nothing to do\n");
		free (PatientIDList);
                return FALSE;
		}
	else
		OperatorConsole.printf("MoveDataToDevice: moving %d MB to: %s\n", (Total+512)/1024, PhysicalTo);

	// attemp to make the destination directory (it might not exist yet) - will fail silently if already exists

	strcpy(Temp, PhysicalTo);
	Temp[strlen(Temp)-1]=0;
#ifdef WIN32				
	mkdir(Temp);
#else
	mkdir(Temp, 0777);
#endif

	// copy the data

	for (i=0; i<NPatients; i++)
		{
		PatientID = PatientIDList + i*256;
		if (!CopyPatientData (DeviceFrom, PatientID, PhysicalFrom, PhysicalTo))
			{
			OperatorConsole.printf("MoveDataToDevice: *** could not copy image data for patient: %s\n", PatientID);
			free (PatientIDList);
			return FALSE;
			}
		else
			OperatorConsole.printf("MoveDataToDevice: copied image data for patient: %s\n", PatientID);
		}

	// verify copy
	for (i=0; i<NPatients; i++)
		{
		PatientID = PatientIDList + i*256;
		if (!ComparePatientData (DeviceFrom, PatientID, PhysicalFrom, PhysicalTo))
			{
			OperatorConsole.printf("MoveDataToDevice: *** error comparing data for patient: %s\n", PatientID);
			free (PatientIDList);
			// RestoreMAGFlags();
			return FALSE;
			}
		else
			OperatorConsole.printf("MoveDataToDevice: compared image data for patient: %s\n", PatientID);
		}

	if (!ModifyDeviceNameForPatient (DeviceFrom, "", DeviceToString))
		{
		OperatorConsole.printf("MoveDataToDevice: *** could not set new device name\n");
		free (PatientIDList);
		return FALSE;
                }

	for (i=0; i<NPatients; i++)
		{
		PatientID = PatientIDList + i*256;

		SystemDebug.printf("To delete: %s %s from %s\n", DeviceTo, PatientID, PhysicalFrom);

									   // extra check: file must be copied
									   // no error if file already copied before and cannot be deleted
		if (!DeletePatientData (DeviceTo, PatientID, PhysicalFrom, PhysicalTo))
			{
			OperatorConsole.printf("MoveDataToDevice: *** error deleting data for patient: %s\n", PatientID);
			// free (PatientIDList);
			//return FALSE;	// mvh 20110105
			}
		else
			OperatorConsole.printf("MoveDataToDevice: deleted source data for patient: %s\n", PatientID);
		}

	free (PatientIDList);
        return TRUE;
	}

////////////////////////////////////////////////////////////////////////
// example: assume DeviceFrom=MAG0.Archiving, DeviceTo = MAG1 then this routine 
// moves all data selected for archiving to MAG1 and sets the DeviceName to MAG1

BOOL
MoveSeriesToDevice(char *DeviceFrom, char *DeviceTo)
	{
	int 	i, NSeries, Size, Total;
	char 	*SeriesUIDList, *SeriesUID;
	char 	PhysicalFrom[1024], PhysicalTo[1024], Temp[1024];
	char	DeviceToString[256];
		
	// process input parameters

	sprintf(DeviceToString, "DeviceName = '%s'", DeviceTo);

	if (!GetPhysicalDevice(DeviceFrom, PhysicalFrom))
		{
		OperatorConsole.printf("MoveSeriesToDevice: *** invalid from-device: %s\n", DeviceFrom);
		return FALSE;
		}

	if (!GetPhysicalDevice(DeviceTo, PhysicalTo))
		{
		OperatorConsole.printf("MoveSeriesToDevice: *** invalid to-device: %s\n", DeviceTo);
		return FALSE;
		}

	// make list of patients that are on DeviceFrom

	NSeries = MakeListOfSeriesOnDevice(DeviceFrom, &SeriesUIDList, 0, 0);
	if (NSeries < 0)
		{
		OperatorConsole.printf("MoveSeriesToDevice: *** could not create series list\n");
		return FALSE;
		}

	// compute amount of data to move and run move script

	Total = 0;
	for (i=0; i<NSeries; i++)
		{
		SeriesUID = SeriesUIDList + i*256;

		ProcessMoveDeviceSeries (DeviceFrom, SeriesUID);
		Size = GetKBUsedForSeries (DeviceFrom, SeriesUID, 1);
		if (Size < 0)
			{
			OperatorConsole.printf("MoveSeriesToDevice: *** could not compute KB used for series: %s\n", SeriesUID);
			free (SeriesUIDList);
			return FALSE;
			}

		Total += Size;
		}

	// something to do ?

	if (Total==0)
		{
		OperatorConsole.printf("MoveSeriesToDevice: Nothing to do\n");
		free (SeriesUIDList);
                return FALSE;
		}
	else
		OperatorConsole.printf("MoveSeriesToDevice: moving %d MB to: %s\n", (Total+512)/1024, PhysicalTo);

	// attemp to make the destination directory (it might not exist yet) - will fail silently if already exists

	strcpy(Temp, PhysicalTo);
	Temp[strlen(Temp)-1]=0;
#ifdef WIN32				
	mkdir(Temp);
#else
	mkdir(Temp, 0777);
#endif

	// copy the data

	for (i=0; i<NSeries; i++)
		{
		SeriesUID = SeriesUIDList + i*256;
		if (!CopySeriesData (DeviceFrom, SeriesUID, PhysicalFrom, PhysicalTo))
			{
			OperatorConsole.printf("MoveSeriesToDevice: *** could not copy image data for series: %s\n", SeriesUID);
			free (SeriesUIDList);
			return FALSE;
			}
		else
			OperatorConsole.printf("MoveSeriesToDevice: copied image data for series: %s\n", SeriesUID);
		}

	// verify copy
	for (i=0; i<NSeries; i++)
		{
		SeriesUID = SeriesUIDList + i*256;
		if (!CompareSeriesData (DeviceFrom, SeriesUID, PhysicalFrom, PhysicalTo))
			{
			OperatorConsole.printf("MoveSeriesToDevice: *** error comparing data for series: %s\n", SeriesUID);
			free (SeriesUIDList);
			// RestoreMAGFlags();
			return FALSE;
			}
		else
			OperatorConsole.printf("MoveSeriesToDevice: compared image data for series: %s\n", SeriesUID);
		}

	if (!ModifyDeviceNameForPatient (DeviceFrom, "", DeviceToString))
		{
		OperatorConsole.printf("MoveSeriesToDevice: *** could not set new device name\n");
		free (SeriesUIDList);
		return FALSE;
                }

	for (i=0; i<NSeries; i++)
		{
		SeriesUID = SeriesUIDList + i*256;

		SystemDebug.printf("To delete: %s %s from %s\n", DeviceTo, SeriesUID, PhysicalFrom);

									   // extra check: file must be copied
									   // no error if file already copied before and cannot be deleted
		if (!DeleteSeriesData (DeviceTo, SeriesUID, PhysicalFrom, PhysicalTo))
			{
			OperatorConsole.printf("MoveSeriesToDevice: *** error deleting data for series: %s\n", SeriesUID);
			// free (SeriesUIDList);
			//return FALSE;	// mvh 20110105
			}
		else
			OperatorConsole.printf("MoveSeriesToDevice: deleted source data for series: %s\n", SeriesUID);
		}

	free (SeriesUIDList);
        return TRUE;
	}

////////////////////////////////////////////////////////////////////////
// assume DeviceTo = 'JUKEBOX1.123' and CACHEDevice0 = d:\davis\cache_%d_04d and JUKEBOXDevice0 = y:\CD%04d
// then this routine compares directory 'd:\davis\cache_1_0123' with 'y:\CD0123'

BOOL
CompareBunchAfterBurning(char *DeviceTo)
	{
	int 	i, NPatients;//, Total, Size
	char 	*PatientIDList, *PatientID;//, *p;
	char 	PhysicalTo[1024], PhysicalCache[1024];//, CacheFrom[512], CacheDevice[512], Temp[512];
//	int 	JUKEBOXDevice=-1, CDNumber=-1, CacheNumber=-1;

	// process input parameters

	if (!GetPhysicalDevice(DeviceTo, PhysicalTo))
		{
		OperatorConsole.printf("Archival: *** invalid jukebox device: %s\n", DeviceTo);
		return FALSE;
		}

	if (!FindCacheDirectory(DeviceTo, PhysicalCache))
		{
		OperatorConsole.printf("Archival: *** could not find cache device for: %s\n", DeviceTo);
		return FALSE;
		}

	// make list of patients that are busy with archiving

	NPatients = MakeListOfPatientsOnDevice(DeviceTo, &PatientIDList);
	if (NPatients < 0)
		{
		OperatorConsole.printf("Archival: *** could not create patient list\n");
		free (PatientIDList);
		return FALSE;
		}


	for (i=0; i<NPatients; i++)
		{
		PatientID = PatientIDList + i*256;
		if (!ComparePatientData (DeviceTo, PatientID, PhysicalCache, PhysicalTo))
			{
			OperatorConsole.printf("Archival: *** error comparing data for patient: %s\n", PatientID);
			free (PatientIDList);
			return FALSE;
			}
		else
			OperatorConsole.printf("Archival: verified jukebox data for patient: %s\n", PatientID);
		}

	free (PatientIDList);
        return TRUE;
	}

////////////////////////////////////////////////////////////////////////
// assume DeviceTo = 'JUKEBOX1.123' and CACHEDevice0 = d:\davis\cache_%d_04d and JUKEBOXDevice0 = y:\CD%04d
// then this routine deletes the contents of directory 'd:\davis\cache_1_0123' but only after the comparison
// between 'd:\davis\cache_1_0123' and 'y:\CD0123' has been succesful !
// Also deletes same images from MIRROR0 if MIRROR0 is defined

BOOL
DeleteBunchAfterBurning(char *DeviceTo)
	{
	int 	i, NPatients;//, Size, Total;
	char 	*PatientIDList, *PatientID;
	char 	PhysicalTo[1024], Temp[512], PhysicalCache[1024];//, CacheFrom[512], CacheDevice[512];
	char 	Mirror[512], PhysicalMirror[1024];
//	int 	JUKEBOXDevice=-1, CDNumber=-1, CacheNumber=-1;

	// process input parameters

	if (!GetPhysicalDevice(DeviceTo, PhysicalTo))
		{
		OperatorConsole.printf("Archival: *** invalid jukebox device: %s\n", DeviceTo);
		return FALSE;
		}

	if (!FindCacheDirectory(DeviceTo, PhysicalCache))
		{
		OperatorConsole.printf("Archival: *** could not find cache device for: %s\n", DeviceTo);
		return FALSE;
		}

	sprintf(Mirror, "MIRROR0");
	if (!GetPhysicalDevice(Mirror, PhysicalMirror))
		{
		Mirror[0] = 0;
		}

	// make list of patients that are busy with archiving

	NPatients = MakeListOfPatientsOnDevice(DeviceTo, &PatientIDList);
	if (NPatients < 0)
		{
		OperatorConsole.printf("Archival: *** could not create patient list\n");
		free (PatientIDList);
		return FALSE;
		}


	for (i=0; i<NPatients; i++)
		{
		PatientID = PatientIDList + i*256;
		if (!ComparePatientData (DeviceTo, PatientID, PhysicalCache, PhysicalTo))
			{
			OperatorConsole.printf("Archival: *** error comparing data for patient: %s\n", PatientID);
			free (PatientIDList);
			return FALSE;
			}
		else
			OperatorConsole.printf("Archival: verified jukebox data for patient: %s\n", PatientID);
		}

	for (i=0; i<NPatients; i++)
		{
		PatientID = PatientIDList + i*256;
		if (!DeletePatientData  (DeviceTo, PatientID, PhysicalCache, NULL))
			{
			OperatorConsole.printf("Archival: *** error deleting cache data for patient: %s\n", PatientID);
			free (PatientIDList);
			return FALSE;
			}
		else
			OperatorConsole.printf("Archival: deleted cache data for patient: %s\n", PatientID);

		if (Mirror[0])
			{
			if (!DeletePatientData  (DeviceTo, PatientID, PhysicalMirror, NULL))
				{
				OperatorConsole.printf("Archival: *** error deleting mirror data for patient: %s\n", PatientID);
				free (PatientIDList);
				return FALSE;
				}
			else
				OperatorConsole.printf("Archival: deleted mirror data for patient: %s\n", PatientID);
			}
		}

	free (PatientIDList);
	
	// attemp to remove cache directory (ignore fail if not empty)

	strcpy(Temp, PhysicalCache);
	Temp[strlen(Temp)-1]=0;
	rmdir(Temp);

        return TRUE;
	}

////////////////////////////////////////////////////////////////////////
// this routine compares patients from a MAG0 device (or e.g., MAG0.Archiving) to its MIRROR device

BOOL
VerifyMirrorDisk(char *DeviceFrom)
	{
	int 	i, NPatients, number;
	char 	*PatientIDList, *PatientID;
	char 	PhysicalFrom[1024], DeviceTo[512], PhysicalTo[1024];
	BOOL	status=TRUE;

	// process input parameters

	if (!GetPhysicalDevice(DeviceFrom, PhysicalFrom))
		{
		OperatorConsole.printf("Archival: *** invalid main device: %s\n", DeviceFrom);
		return FALSE;
		}

	if(strnicmp(DeviceFrom, "MAG", 3)==0)
 		{
		number = atoi(DeviceFrom + 3);
		sprintf(DeviceTo, "MIRROR%d", number);

		if (!GetPhysicalDevice(DeviceTo, PhysicalTo))
			{
			OperatorConsole.printf("Archival: *** invalid mirror device: %s\n", DeviceTo);
			return FALSE;
			}
		}
	else
		{
		OperatorConsole.printf("Archival: *** main device must be a MAG device: %s\n", DeviceFrom);
		return FALSE;
		}

	// make list of patients that are busy with archiving

	NPatients = MakeListOfPatientsOnDevice(DeviceFrom, &PatientIDList);
	if (NPatients < 0)
		{
		OperatorConsole.printf("Archival: *** could not create patient list\n");
		free (PatientIDList);
		return FALSE;
		}


	for (i=NPatients-1; i>=0; i--)
		{
		PatientID = PatientIDList + i*256;
		if (!ComparePatientData (DeviceFrom, PatientID, PhysicalFrom, PhysicalTo))
			{
			OperatorConsole.printf("Archival: *** error comparing mirror data for patient: %s\n", PatientID);
			status = FALSE;
			}
		else
			OperatorConsole.printf("Archival: verified mirror data for patient: %s\n", PatientID);
		}

	free (PatientIDList);
        return status;
	}

////////////////////////////////////////////////////////////////////////
// this routine test reads all images of a MAG device (or e.g., MAG0.Archiving)

BOOL
TestImages(char *DeviceFrom)
	{
	int 	i, NPatients;//, number;
	char 	*PatientIDList, *PatientID;
	char 	PhysicalFrom[1024];//, DeviceTo[512], PhysicalTo[1024];
	BOOL	status=TRUE;

	// process input parameters

	if (!GetPhysicalDevice(DeviceFrom, PhysicalFrom))
		{
		OperatorConsole.printf("Archival: *** invalid device to test read: %s\n", DeviceFrom);
		return FALSE;
		}

	// make list of patients on the device to test

	NPatients = MakeListOfPatientsOnDevice(DeviceFrom, &PatientIDList);
	if (NPatients < 0)
		{
		OperatorConsole.printf("Archival: *** could not create patient list\n");
		free (PatientIDList);
		return FALSE;
		}


	for (i=NPatients-1; i>=0; i--)
		{
		PatientID = PatientIDList + i*256;
		if (!TestPatientData(DeviceFrom, PatientID, PhysicalFrom))
			{
			OperatorConsole.printf("Archival: *** error test reading data for patient: %s\n", PatientID);
			status = FALSE;
			}
		else
			OperatorConsole.printf("Archival: test read all data for patient: %s\n", PatientID);
		}

	free (PatientIDList);
        return status;
	}

////////////////////////////////////////////////////////////////////////
// change device name in database for all images on that device
// useful to restore from optical disk or to repare a database

BOOL
RenameDevice(char *DeviceFrom, char *DeviceTo)
	{
	char	DeviceToString[256];

	sprintf(DeviceToString, "DeviceName = '%s'", DeviceTo);

	if (!ModifyDeviceNameForPatient (DeviceFrom, "", DeviceToString))
		{
		OperatorConsole.printf("Archival: *** could not set new device name %s\n", DeviceToString);
		return FALSE;
		}
	else
		{
		OperatorConsole.printf("Archival: set new device name %s\n", DeviceToString);
		}
	
        return TRUE;
	}


static int SortP(const void* pElem1, const void* pElem2)
	{
	int i = strnicmp((char *)pElem1, (char *)pElem2, 128);
	if (i==0) return -strnicmp((char *)pElem1+128, (char *)pElem2+128, 128);
	else	  return i;
	}

static int SortD(const void* pElem1, const void* pElem2)
	{
	return strnicmp((char *)pElem1+128, (char *)pElem2+128, 128);
	}

// example: Max=10, Device = "MAG0", Sort = "StudyDate" --> 
// gives 10 least recent scanned patients on MAG0
// Sort can typically be StudyDate, PatientBir, AccessTime
// If Max<0 give most recently used patients
// If Sort is empty sorts on AccessTime

int
MakeListOfOldestPatientsOnDevice(char **PatientIDList, int Max, const char *Device, char *Sort)
	{
	// Query the patient table, and sort it by access time
	SQLLEN			sdword,sdword1;
	Database		DB;
	char			s[256], DeviceValue[256];
	char			PatientID[128], StudyDate[128], QueryString[512];
	int 			Patients, NStudy, i, Result;
	char			*StudyList;
	
	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		return -1;

	// count #studies

	if (!DB.Query(StudyTableName, "PatientID", "", NULL))
		return -1;

	DB.BindField (1, SQL_C_CHAR, PatientID, 128, &sdword);
	NStudy = 0;
	while(DB.NextRecord())
		NStudy++;

	// list patientID and sort field for all studies
	// some new patients may recently have come in!
	StudyList = (char *)malloc(256 * (NStudy + 100));
	if (StudyList==NULL) return -1;

        strcpy(s, "PatientID");
	if (Sort && Sort[0]) 
		{
		strcat(s, ", ");
		strcat(s, Sort);
		}
        else
		strcat(s, ", AccessTime");

	if (!DB.Query(StudyTableName, s, "", NULL))
		{
		free(StudyList);
		return -1;
		}

	if(!DB.BindField (1, SQL_C_CHAR, PatientID, 128, &sdword))
		{
		free(StudyList);
		return -1;
		}

	if(!DB.BindField (2, SQL_C_CHAR, StudyDate, 128, &sdword1))
		{
		free(StudyList);
		return -1;
		}

	NStudy = 0;
	while(DB.NextRecord())
		{
		strncpy(StudyList + 256*NStudy,       PatientID, 128);
		strncpy(StudyList + 256*NStudy + 128, StudyDate, 128);
		NStudy++;
		}

	// sort list of patients (ascending), sorted on descending study date per patient
	// then delete all but newest entry per patient

	qsort(StudyList, NStudy, 256, SortP);

	Patients = 0;
	const char *prevpat="";
	char *p=StudyList;
	for (i=0; i<NStudy; i++)
		{
		if (strnicmp(prevpat, StudyList + 256*i, 128)==0) 
			continue;	// same patient

		prevpat = StudyList + 256*i;
		memcpy(p, prevpat, 256);
		p += 256;
		Patients++;
		}

	
	// sort patients on ascending newest study date
	// then test for each patient if it occurs on device

	qsort(StudyList, Patients, 256, SortD);

	// return the list: either testing on device or not

	*PatientIDList = (char *)malloc(256 * abs(Max));
	if (*PatientIDList==NULL) 
		{
		free (StudyList);
		return -1;
		}

	Result = 0;

	if (Device && Device[0]) 
		{
		MakeSafeString(Device,  DeviceValue, &DB);

		for (i=0; i<Patients && Result<abs(Max); i++)
			{
			MakeSafeString(StudyList + 256*i, s, &DB);

			sprintf(QueryString, 	"DICOMImages.DeviceName %s and "
						"DICOMImages.ImagePat %s",
						DeviceValue, s);

			if (!DB.Query(ImageTableName, "ImagePat", QueryString, ""))
				{
				free (StudyList);
				return -1;
				}

  			DB.BindField (1, SQL_C_CHAR, PatientID, 128, &sdword);

 			if (DB.NextRecord())
				{
				if (Max>0) strcpy(*PatientIDList + 256*Result, StudyList + 256 * i);
                                else       strcpy(*PatientIDList + 256*Result, StudyList + 256 * (Patients-1-i));
				Result++;
				}
			}
		}
	else
		{
		for (i=0; i<Patients && i<abs(Max); i++)
			{
			if (Max>0) strcpy(*PatientIDList + 256*i, StudyList + 256 * i);
			else       strcpy(*PatientIDList + 256*i, StudyList + 256 * (Patients-1-i));
			Result++;
			}
		}

	free (StudyList);
	DB.Close();
	return Result;
	}

