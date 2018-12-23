/*
20000211	ljz	Removed MajorEvent and MinorEvent instances of 'Debug'.
			Cosmetics. Added UserLog and TroubleLog.
			Added timestamp option.
			Added CriticalSection when printing to file or stdout.
20011110        mvh     Post release 1.3.10: keep log files closed while working
20021027        mvh     Avoid crash when logfile cannot be opened
20041003        mvh     Note: max 1200 chars (use %.1000s to print long strings)
20060618	mvh	Define _SH_DENYNO if needed
20070105	mvh	Added timestamp for linux users (thanks, Mark Pearson)
20070406	mvh	Use reentrant ctime_r and localtime_r when available
20070408	mvh	Protected entire .printf() with global critical section
20070712	mvh	Closed/opened critical section where recursive call to printf
			Would hang linux version with sqlite
20071120	mvh	Use fputs instead of fprintf at end: otherwise % removed
20080821	mvh	Fix for Solaris SUNwspro compiler
20090620	jf	Include file stuff
20091231	bcb	Changed char* to const char* for gcc4.2 warnings
20100111	mvh	Merged
20100125	mvh	Removed linux warning
20100309	bcb	Removed Windows only variable from unix build.
20100619	bcb	Added #ifndefs made Port a local lPort (gcc4.0 Warnings)
20100717	mvh	Merged
20100816	mvh	removed (a) from #ifndef EnterCriticalSection(a) etc
20150902	mvh	Added Progress socket
20160221	mvh	Made print buffer much larger
20181219	mvh	Added OnTCP
*/

#	include	<stdio.h>
#	include	<stdarg.h>

#ifndef	UNIX
#	include <time.h>
#	include	<windows.h>
#	include	<share.h>
#else
#	include	"wintypes.hpp"
#	include	"npipe.hpp"
#endif
#	include	"dicom.hpp"
#	include	"dprintf.hpp"

#ifdef UNIX
#       include <pthread.h> //DUCKHEAD92
#endif

#ifndef _SH_DENYNO 
#define _SH_DENYNO SH_DENYNO
#endif

extern	char	ServerName[];

#ifndef WIN32
# 	include <unistd.h>
# 	include <dirent.h>
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
#endif

static CRITICAL_SECTION	CriticalFile;
static int CriticalFileExists=0;

/* Constructor and Destructor */

Debug::Debug()
#ifdef __GNUC__ //Faster with member initialization.
:BDebug(0),Debugfp(NULL),CloseOnOff(0), UseMessagePipe(0), UseUDPTCP(0),
bAddTimeStamps(0), DebugSocket(), DescribeSource(FALSE)
#endif
{
  if (!CriticalFileExists) 
  { CriticalFileExists=1;
    InitializeCriticalSection(&CriticalFile);
  }
#ifndef __GNUC__
  BDebug = 0;
  Debugfp = NULL;
  CloseOnOff = 0;
  UseMessagePipe = 0;
  UseUDPTCP = 0;
  bAddTimeStamps = 0;
#endif
}

Debug::~Debug()
{
  Off();
}

/* Several overloaded Debug 'ON' functions */

void Debug::On()
{
  Off();
  if (!CriticalFileExists) 
  { CriticalFileExists=1;
    InitializeCriticalSection(&CriticalFile);
  }  
  Debugfp = stdout;
  BDebug = 1 ;
  CloseOnOff = 0;
  UseMessagePipe = 0;
}

void Debug::On(FILE* fp)
{
  Off();
  if(!fp) return;

  if (!CriticalFileExists) 
  { CriticalFileExists=1;
    InitializeCriticalSection(&CriticalFile);
  }  

  Debugfp = fp;
  BDebug = 1;
  CloseOnOff = 0;
  UseMessagePipe = 0;
}

void Debug::On(char* filename)
{
  Off();
  if(!filename) return;

  if (!CriticalFileExists) 
  { CriticalFileExists=1;
    InitializeCriticalSection(&CriticalFile);
  }  

//#ifndef	UNIX
//  Debugfp = _fsopen(filename, "a", _SH_DENYNO);
//#else
//  Debugfp = fopen(filename, "a");
//#endif
//  if(!Debugfp)
//    return;

  Debugfp = NULL;
  strcpy(FilePipeName, filename);
  BDebug = 3;
  CloseOnOff = 0;
  UseMessagePipe = 0;
}

void Debug::OnMsgPipe(char* MsgPipe)
{
  Off();
  if (!CriticalFileExists) 
  { CriticalFileExists=1;
    InitializeCriticalSection(&CriticalFile);
  }  
  BDebug = 1;
  UseMessagePipe = 1;
  CloseOnOff = 0;
  DescribeSource = TRUE;
  strcpy(FilePipeName, MsgPipe);
}

void Debug::OnMsgPipe(char* MsgPipe, BOOL DSource)
{
  Off();
  if (!CriticalFileExists) 
  { CriticalFileExists=1;
    InitializeCriticalSection(&CriticalFile);
  }  
  BDebug = 1;
  UseMessagePipe = 1;
  CloseOnOff = 0;
  DescribeSource = DSource;
  strcpy(FilePipeName, MsgPipe);
}

void Debug::OnUDP(char* Host, const char* lPort)
{
  Off();
  if (!CriticalFileExists) 
  { CriticalFileExists=1;
    InitializeCriticalSection(&CriticalFile);
  }  
  if(!DebugSocket.BindUDPClient(Host, lPort))
  { printf("FAILED TO BIND UDP CLIENT\n");
    return;
  }
  UseUDPTCP = 1;
  BDebug = 1;
  CloseOnOff = 0;
  DescribeSource = TRUE;
}
	
void Debug::OnTCP(char* Host, const char* lPort)
{
  Off();
  if (!CriticalFileExists) 
  { CriticalFileExists=1;
    InitializeCriticalSection(&CriticalFile);
  }  
  if(!DebugSocket.Open(Host, (char *)lPort))
  { printf("FAILED TO BIND TCP CLIENT\n");
    return;
  }
  UseUDPTCP = 1;
  BDebug = 1;
  CloseOnOff = 0;
  DescribeSource = TRUE;
}

/* One Debug 'OFF' function */

void Debug::Off()
{
  if(!BDebug)
    return;
//  DeleteCriticalSection(&CriticalFile);
  BDebug = 0;
  if(CloseOnOff && Debugfp != NULL)
    fclose(Debugfp);
  CloseOnOff = 0;
  UseMessagePipe = 0;
  UseUDPTCP = 0;
}


/* Finally the PRINT function */

#ifndef localtime_r
#define localtime_r(a, b) memcpy(b, localtime(a), sizeof(struct tm))
#endif

int Debug::printf(const char *fmt, ...)
{
  int		rc = 0;
  va_list	vargs;
  char		s[12500];
  time_t	Time;
#ifndef	UNIX
  struct tm	LocalTime;
#endif
  int		iLength;

  /* Debug::On() must be called first */
  if(!BDebug)
    return rc;

  EnterCriticalSection(&CriticalFile);

  if(DescribeSource)
    sprintf(s, "[%s] ", ServerName);
  else
    s[0] = 0;

#ifndef	UNIX
  if (bAddTimeStamps)
  {
    time(&Time);				/* Get time as long integer. */
    localtime_r(&Time, &LocalTime);		/* Convert to local time. */
    sprintf(s + strlen(s), "%04d%02d%02d %02d:%02d:%02d ",
      LocalTime.tm_year + 1900,
      LocalTime.tm_mon + 1,
      LocalTime.tm_mday,
      LocalTime.tm_hour,
      LocalTime.tm_min,
      LocalTime.tm_sec);
  }
#else
  if (bAddTimeStamps)
  { char TimeString[128], buf[64];
    Time = time(NULL);
#ifdef SOLARIS
    strcpy(TimeString, ctime_r(&Time, buf, 64));
#else
    strcpy(TimeString, ctime_r(&Time, buf));
#endif
    TimeString[strlen(TimeString)-1] = '\0';
    strcat(s, TimeString);
    strcat(s, " ");
  }
#endif

  iLength = strlen(s);
  va_start(vargs, fmt);
  vsprintf(s + iLength, fmt, vargs);
  va_end(vargs);

  LeaveCriticalSection(&CriticalFile);

  if ((strstr(s + iLength, "***")) && (this != &TroubleLog))
    TroubleLog.printf("%s", s + iLength);

  EnterCriticalSection(&CriticalFile);

  if(!UseMessagePipe)
  { if(!UseUDPTCP)
    { if (BDebug==3)
      { // append to a file that is opened/closed everytime
#ifndef	UNIX
        Debugfp = _fsopen(FilePipeName, "a", _SH_DENYNO);
#else
        Debugfp = fopen(FilePipeName, "a");
#endif
        if (Debugfp)
	{ rc = fputs(s, Debugfp);
          fclose(Debugfp);
        }
      }

      else if(Debugfp)
      { // Output to a file that remains open or stdout
	rc = fputs(s, Debugfp);
	fflush(Debugfp);
      }
    }
    else
    { // Output to a UDP PORT
      rc = 1;
      DebugSocket.SendBinary((BYTE*)s,(UINT)strlen(s));
    }
  }
  else
  { // Output to a named pipe
#ifndef	UNIX
    if(WaitNamedPipe(FilePipeName, 1))
    { rc = 1;
      CallNamedPipe(FilePipeName, s, strlen(s)+1, NULL, 0, NULL, NMPWAIT_NOWAIT);
    }
#else
    NamedPipe aMessagePipe(FilePipeName);
    aMessagePipe.Connect();
    if(aMessagePipe.Write((unsigned char*)s, strlen(s)+1) == strlen(s) + 1)
      rc = 1;
    aMessagePipe.Disconnect();
#endif
  }

  LeaveCriticalSection(&CriticalFile);
  return rc;
}


/* Global instances of the logging functionality */

Debug	SystemDebug;
Debug	OperatorConsole;
Debug	AccessUpdate;
//Debug	MajorEvent;
//Debug	MinorEvent;
Debug	UserLog;
Debug	TroubleLog;
Debug	Progress;

