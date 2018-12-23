/* MvH 19980619: Added some doc on command line options and modes 				*/
/*               Fixed -L option (moved db.close) 						*/
/* MvH 19980620: Made code suitable for external calling; copied some of it to device.cpp 	*/
/* 		 Killer.exe is not needed anymore with NKI Dgate.exe except as sleeper task 	*/
/*		 because the alert task has been integrated into Dgate.exe			*/
/* mvh 20030702: Adapted because of compression externals required by device.cpp		*/
/* mvh 20030706: Attach VRType to PDU's for implicit little endian support			*/
/* mvh 20050129: Adapted dummy for DumpVR							*/
/* mvh 20071118: Adapted for 64 bits (use SQLLEN for BindField)					*/

#	include	"dgate.hpp"

#ifndef UNIX	// Win32...

#define	SleepForSeconds(s)	Sleep((s) * 1000)	// Win32 version

#else	// UNIX...

#define SleepForSeconds(s)  sleep((s))	// UNIX version
#include "gpps.hpp"
#include <unistd.h>
#include <signal.h>

#endif	// UNIX


// Killer-Bee.

// What this does is manage the free space available on each of the MAG
// devices.  It does this in two ways.
//
//	a) By a sleeper task which "wakes up" at pre-programmed intervals
//
//	b) By an alert task which forks this off this task if the system is
//     running low on disk space
//
// options : none = cleanup old patients if enabled     (Mode==0)
//           -A   = make space on disk                  (Mode==1)
//           -L   = list patients that would be deleted (Mode==2)

#ifndef	UNIX
char	ConfigFile[] = ".\\dicom.ini";
#else
char	ConfigFile[] = "dicom.ini";
#endif
char	RootConfig[] = "sscscp";

// dummies for device.cpp without dgate.cpp

BOOL GenUID(char	*oString)
	{
	return TRUE;
	}

char ArchiveCompression[16]="";

RTC	VRType(TRUE);					// VR translation table

BOOL
LoadKillParameters (int *KillDeadWood, int *KillSleepTime, int *MAGDeviceThreshHold)
	{
	char	Temp[255];
	char	RootSC[255];

	ConfigMicroPACS();

	GetPrivateProfileString ( RootConfig, "MicroPACS", RootConfig,
		(char*) RootSC, 64, ConfigFile);

	GetPrivateProfileString( RootSC, "KillDeadWood", "0",
		(char*) Temp, 64, ConfigFile);

	*KillDeadWood = atoi(Temp);

	GetPrivateProfileString ( RootSC, "KillSleepTime", "3600",
		(char*) Temp, 64, ConfigFile);

	*KillSleepTime = atoi(Temp);

	GetPrivateProfileString ( RootSC, "MAGDeviceThreshHold", "40",
		(char*)Temp, 64, ConfigFile);

	*MAGDeviceThreshHold = atoi(Temp);

	SystemDebug.On(stdout);
	OperatorConsole.OnMsgPipe(OCPipeName);
	AccessUpdate.OnMsgPipe(ACPipeName);
	if(!LoadKFactorFile((char*)KFACTORFILE))
		{
		fprintf(stderr, "Error loading kfactor file:%s\n", KFACTORFILE);
		return ( 0 );
		}

	return ( TRUE );
	}

BOOL
KillOff(int Mode, int MAGDeviceThreshHold, int KillDeadWood)
	{
	// Query the patient table, and sort it by access time
	SQLLEN			sdword,sdword1;
	DWORD			iTime;
	Database		DB;
	char			s[255];
	char			PID[255];

	while ( TRUE )
		{
		if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
			return ( FALSE );

		if (!DB.Query(PatientTableName, "AccessTime, PatientID", NULL, "AccessTime"))
			return (FALSE);

		if(!KillDeadWood)
			if(Mode!=1)
				return ( FALSE );

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
			if(Mode==1)
				{
				VR		*vr = new VR(0x0010, 0x0020, strlen(PID), (void*)PID, (BOOL) FALSE );
				DICOMDataObject	DDO;

				if(LargestFreeMAG()>MAGDeviceThreshHold)
					{
					printf("Enough disk scace - no delete required; oldest patient ID := '%s'\n", PID);
					DB.Close();
					return (TRUE);
					}

				OperatorConsole.printf("***PANIC DELETE Patient ID := '%s'\n", PID);
				DDO.Push(vr);
				RemoveFromPACS(&DDO);
				printf("BACK FROM REMOVEFROMPACS\n");
				if(LargestFreeMAG()>MAGDeviceThreshHold)
					{
					DB.Close();
					return ( TRUE );
					}
				else
					continue;
				}
			if(Mode == 2)
				{
				printf("iTime = %d, Patient = %s, WouldBeDeleted = %d\n", iTime, PID,
								(CurrentTime() - iTime) >= KillDeadWood);
				continue;
				}
			if((CurrentTime() - iTime) >= KillDeadWood)
				{
				VR		*vr = new VR(0x0010, 0x0020, strlen(PID), (void*)PID, (BOOL) FALSE );
				DICOMDataObject	DDO;

				OperatorConsole.printf("Patient ID '%s' Removed (no longer being used)\n", PID);
				DDO.Push(vr);
				RemoveFromPACS(&DDO);
				continue;
				}
			else
				{	
				DB.Close();
				return ( TRUE );
				}
			}
		if(Mode == 2 || Mode == 0) return TRUE;
		}
	DB.Close();
	return ( TRUE );
	}

int
main(
	int		argc,
	char	*argv[])
	{

#ifdef UNIX

	// set that pesky broken pipe signal to be ignored
	// I/O that would have generated a SIGPIPE will return EPIPE instead
	// this applies to named pipe writes and read/write of sockets

	signal(SIGPIPE, SIG_IGN);

#endif

	int		valid_argc = 1;
	int		Mode=0;

	int	KillDeadWood;  // delete patients accessed more than killdeadwood time (seconds) ago
                               // default 0=disable this option
	int	KillSleepTime; // time in seconds between invocations of killdeadwood
                               // default=3600 = one hour
	int	MAGDeviceThreshHold; // minimal space on disk in megabyte before deleting occurs

	LoadKillParameters (&KillDeadWood, &KillSleepTime, &MAGDeviceThreshHold);

	while ( valid_argc < argc )
		{
#ifndef UNIX	// Win32...
		if(argv[valid_argc][0]=='-' || argv[valid_argc][0]=='/' || argv[valid_argc][0]=='?' )
#else
		if(argv[valid_argc][0]=='-' || argv[valid_argc][0]=='?')
#endif
			{
			switch ( argv[valid_argc][1] )
				{
				case	'a':
				case	'A':
					Mode = 1;
					OperatorConsole.printf("Killer.exe is making space on disk:\n");
					fprintf(stderr, "Killer.exe is making space on disk:\n");
					break;

				case	'l':
				case	'L':
					Mode = 2;
					fprintf(stderr, "List of patients that would be cleaned:\n");
					break;

				default:
					{
					fprintf(stderr, "Usage: killer:    Delete old (least recently used) patients\n");					fprintf(stderr, "       killer -L: List old patients that would be deleted\n");

					fprintf(stderr, "       killer -A: Make space on disk by deleting least recently used patients\n\n");
					fprintf(stderr, "Parameters: KillDeadWood        = %d (max file usage age in seconds)\n", KillDeadWood);
					fprintf(stderr, "            KillSleepTime       = %d (time between cleanups in seconds)\n", KillSleepTime);
					fprintf(stderr, "            MAGDeviceThreshHold = %d (minimal space on disk in MB)\n\n", MAGDeviceThreshHold);
					exit(1);
					break;
					}
				}
			}
		++valid_argc;
		}


	if (Mode==0)
		fprintf(stderr, "Killer.exe is running as periodic database cleaner.\n");

	while ( TRUE )
		{
		KillOff(Mode, MAGDeviceThreshHold, KillDeadWood);

		if(Mode)
			break;
		SleepForSeconds(KillSleepTime);
		}
	return ( 0 );
	}

DumpVR(
	VR	*vr,
	FILE  	*f)
	{
	return ( TRUE );
	}
