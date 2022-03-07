/*
MvH 19980327: put messages in one SystemDebug for UDP message layout
MvH 19980415: changed messages to OperatorConsole
MvH 19980605: Need to add regen for optical devices and cachedevices !!!!!
MvH 19981218: temp use pdu.load
ljz 19990108: Replaced PDU.Load by LoadImplicitLittleEndian in RegenToDatabase
MvH 19990109: Set HeaderOnly flag for LoadImplicitLittleEndianFile to TRUE
	      Added regen of CACHE and JUKEBOX devices; 
	      NOTE: full regen imposes name restrictions on JUKEBOX and CACHE directories
MvH 19990117: Filenames with .v2 are assumed raw; added extension .dcm for chapter 10 format	
ljz 19990317: Parameters of LoadImplicitLittleEndianFile have been changed
mvh 19990521: .img files react same as .dcm files
ljz 20000629: Logging of trouble now starts with '***'
mvh 20010415: Added SubDir parameter to regen to allow regen of one directory only
mvh 20010416: Added RegenFile - to allow modification of images
mvh 20011109: Made file extension checking case insensitive
mvh 20020409: Experimental multithread version.
mvh 20020413: Made NTHREADS variable and tested. Time to regen 6900 files:
              #            		time (min:s)
	      1            		5:41
	      4            		4:06
              10           		3:34
              20           		3:14
              40           		3:41
             100           		3:36 gives sql server deadlock errors (even with 20xretry)

              20 using PDU 		10:48 (CPU limited)
              20 using 64 cache		3:09
              20 using 128 cache        3:05
              20 using 512 cache        3:06
              20 using 1024 cache       3:00
              20 using 1000 cache	2:37 (?)
              20 using 1000 cache	3:08
              1 using 256,unique index	4:13	<- chosen because is reliable and fast
              20 using 1000,uniqueindex	3:14
              1 using 256,unique index	3:40	with new buffered loadddo code by ljz
mvh 20020415: Tested with new loaddo code by ljz.
              Note: multithread works and has allowed fixing MT some errors in other
              modules. However, due to a problem in concurrent db writing (in 
              UpdateOrAddToTable) it should not be used clinically.
ljz 20020613: Removed some warnings
mvh 20020802: Simplified regen code; traverse subdirectories recursively (allow e.g., e-film data format)
mvh 20020804: Use PATHSEPCHAR where possible
mvh 20021014: Exclude directory PRINTER_FILES during regen (contains slices without ID and such)
ljz 20030120: Removed some warnings
mvh 20030706: Attach VRType to PDU for implicit little endian support
mvh 20030819: Allow longer filenames
mvh 20040614: Do not regen CACHE devices; first regen JUKEBOX then MAG (e.g., in historical order)
mvh 20050118: Adapted for LINUX compile
mvh 20060628: AddToDatabase has JustAdd parameter
mvh 20070210: Increased maxvrsize for regen v2 files from 0x400 to 0x4000 -> UIDs were sometimes not found
bcb 20070308: Ignore .DS_Store for DARWIN
bcb 20091231: Changed char* to const char* for gcc4.2 warnings
mvh 20100111: Merged; fixed WIN32 changes
mvh 20120630: Regen now allows files without 3 letter extension
mvh 20201224: Added regen and regenFail converter
mvh 20220206: Allow regen of selected folders/files (pass own * wildcards to RegenDir)
*/

// #define MULTITHREAD
#	include	"dgate.hpp"

#ifdef MULTITHREAD
#define NTHREADS 20
static void regen_queue(char *basename, char *filename, char *device);
static int queueempty();
#endif

#ifdef WIN32
#define PATHSEPSTR "\\"
#define PATHSEPCHAR '\\'
#else
#define PATHSEPSTR "/"
#define PATHSEPCHAR '/'
#define stricmp(s1, s2) strcasecmp(s1, s2)
#include <unistd.h>
#include <dirent.h>
#endif

#ifndef WIN32
BOOL IsDirectory(char *path)
	{
	char temp[1024];
	struct stat statbuf;

	int lc= strlen(path) - 1;			/* index for last char in path */
	strcpy(temp, path);

	if (temp[lc] == '\\' || temp[lc] == '/')
		temp[lc] = 0;

	if (stat(temp, &statbuf) == -1) return FALSE;	/* file or path not found */

	if (statbuf.st_mode & S_IFDIR)
		return TRUE;
	else
		return FALSE;
	}
#endif

// This routine checks the file extension and only accepts v2=raw, dcm/img=chapter 10 (or other)

extern "C" void lua_setvar(ExtendedPDU_Service *pdu, char *name, char *value);

BOOL
RegenToDatabase(Database *DB, char *basename, char *filename, const char *device)
	{
	DICOMDataObject*	pDDO;
	int			len;
	ExtendedPDU_Service     PDU;
	
	PDU.AttachRTC(&VRType);

	len = strlen(filename);
	
	// The excludes some impossible short names (including path) like a.v2 which are unlikely to occur
	
	if (len < 4) return FALSE;
	
	if (stricmp(filename + len - 3, ".v2")==0 )
		{
		pDDO = LoadImplicitLittleEndianFile(filename, 0x4000);	// increased from 400 to allow load large icons
		if(!pDDO)
			{
			OperatorConsole.printf("***[Regen] %s -FAILED: Error on Load\n", filename);
			return ( FALSE );
			}
		}
	else if (stricmp(filename + len - 4, ".dcm")==0 ||
	         stricmp(filename + len - 4, ".img")==0 ||
	         strchr(filename + len - 4, '.')==NULL
		)
		{
		pDDO = PDU.LoadDICOMDataObject(filename);
		if(!pDDO)
			{
			OperatorConsole.printf("***[Regen] %s -FAILED: Error on Load\n", filename);
			DICOMDataObject	DDO;
			lua_setvar(&PDU, "Filename", filename);
			int rc = CallImportConverterN(NULL, &DDO, 2600, NULL, NULL, NULL, NULL, &PDU, NULL, NULL);
			return ( FALSE );
			}
		}
	else
		return FALSE;

	lua_setvar(&PDU, "Filename", filename);
	int rc = CallImportConverterN(NULL, pDDO, 2500, NULL, NULL, NULL, NULL, &PDU, NULL, NULL);
        if (rc==2) return ( FALSE );

	if(!SaveToDataBase(*DB, pDDO, basename, device, FALSE))
		{
		delete pDDO;
		OperatorConsole.printf("***[Regen] %s -FAILED: Error SQL Add\n", filename);
		return ( FALSE );
		}
	delete pDDO;
	OperatorConsole.printf("[Regen] %s -SUCCESS\n", filename);
	return ( TRUE );
	}

// This function enters a file into the database, assuming it is on one of the MAG devices

BOOL
RegenFile(char *filename)
	{
	Database	DB;
	UINT		Index;
	char		Device[255], Physical[1024], BaseName[1024];

	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		{
		fprintf(stderr, "Error Connecting to SQL\n");
		return ( FALSE );
		}


	Index = 0;
	while ( Index < MAGDevices )
		{
		sprintf(Device, "MAG%d", Index);
		GetPhysicalDevice(Device, Physical);
		if (memicmp(Physical, filename, strlen(Physical))==0)
			{
			strcpy(BaseName, filename + strlen(Physical));
			RegenToDatabase(&DB, BaseName, filename, Device);
			break;
			}
		++Index;
		}

	if (Index == MAGDevices)
		{
		fprintf(stderr, "File to enter is not on a MAG device\n");
		return ( FALSE );
		}

	return ( TRUE );
	}


// Help function to regen an arbitrary directory (PathString must end with PATHSEPSTR)

#ifdef WIN32

BOOL RegenDir(Database *DB, char *PathString, const char *Device)
	{
	HANDLE		fdHandle;
	WIN32_FIND_DATA	FileData;

	char		TempPath[1024];
	char		Physical[1024];
        char            *hasstar, *q;

	GetPhysicalDevice(Device, Physical);

	strcpy(TempPath, PathString);
        hasstar=strchr(TempPath, '*');
	if (hasstar==NULL)
	  strcat(TempPath, "*.*");
            
	// Traverse first level of subdirectories	

	fdHandle = FindFirstFile(TempPath, &FileData);
	if(fdHandle == INVALID_HANDLE_VALUE)
		return ( FALSE );

	while ( TRUE )
		{
		if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
			if 
				(	
				stricmp(FileData.cFileName, "INCOMING") != 0 &&
				stricmp(FileData.cFileName, "PRINTER_FILES") != 0 &&
				strcmp(FileData.cFileName, ".") != 0 &&
				strcmp(FileData.cFileName, "..") != 0 &&
				stricmp(FileData.cFileName, "BIN") != 0
		  		)
				{
				strcpy(TempPath, PathString);
                                if (hasstar)
                                        { 
                                        q=strrchr(TempPath, PATHSEPCHAR);
                                        if (q) q[1]=0;
                                        }
                                strcat(TempPath, FileData.cFileName);
				strcat(TempPath, PATHSEPSTR);
				RegenDir(DB, TempPath, Device);
				}
			}
		else
			{
			strcpy(TempPath, PathString);
                        if (hasstar)
                                { 
                                q=strrchr(TempPath, PATHSEPCHAR);
                                if (q) q[1]=0;
                                }

			strcat(TempPath, FileData.cFileName);

			RegenToDatabase (DB, TempPath + strlen(Physical), TempPath, Device);
			}
			
		if(!FindNextFile(fdHandle, &FileData))
			break;
		}

		FindClose(fdHandle);

	return TRUE;
	}

#else	// linux version

BOOL RegenDir(Database *DB, char *PathString, const char *Device)
	{
	DIR		*dird;
	dirent		*diren;

	char		TempPath[1024];
	char		Physical[1024];
	char		*n;

	GetPhysicalDevice(Device, Physical);

	strcpy(TempPath, PathString);
	TempPath[strlen(TempPath)-1]=0;

	// Traverse first level of subdirectories	

	dird = opendir(TempPath);
	if(dird == NULL)
		return ( FALSE );

	while ( TRUE )
		{
		diren = readdir(dird);
		if (diren)
			{
			n = diren->d_name;
			strcpy(TempPath, PathString);
			strcat(TempPath, n);
			
			if (IsDirectory(TempPath))
				{
				if
				(	
				stricmp(n, "INCOMING"     ) != 0 &&
				stricmp(n, "PRINTER_FILES") != 0 &&
				strcmp (n, "."            ) != 0 &&
				strcmp (n, ".."           ) != 0 &&
				stricmp(n, "BIN"          ) != 0
#ifdef DARWIN //A Macintosh OS X file.
				&& stricmp(n, ".DS_Store"    ) != 0
#endif //DARWIN
		  		)
					{
					strcat(TempPath, PATHSEPSTR);
					RegenDir(DB, TempPath, Device);
					}
				}
			else
				{
				RegenToDatabase (DB, TempPath + strlen(Physical), TempPath, Device);
				}
			}
		else
			break;
		}

		closedir(dird);

	return TRUE;
	}

#endif

// This Regen function recurses a single level of subdirecties if IsCacheOrJukeBox is FALSE 
// it recurses a two levels of subdirecties if IsCacheOrJukeBox is TRUE

#ifdef WIN32

BOOL
Regen(const char *Device, int IsCacheOrJukeBox, char *SubDir)
	{
	int		itemp, i, j, CDNumber, JUKEBOXNumber;
	Database	DB;
	HANDLE		fdHandle;
	WIN32_FIND_DATA	FileData;
	char		PathString[1024], TempPath[1024], FileString[1024];
	char		ActualDevice[256];
	char		*p;

	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		{
		fprintf(stderr, "Error Connecting to SQL\n");
		return ( FALSE );
		}

	// convert device specification to a path

	GetPhysicalDevice(Device, PathString);
	itemp = strlen(PathString);
	if(!itemp)
		return FALSE;

	// strip the last subdirectory (e.g., CD0_%04d) for regen of complete CACHE or JUKEBOX devices

	if (IsCacheOrJukeBox)
		{
		PathString[itemp-1]=0;
		p = strrchr(PathString, PATHSEPCHAR);
		if (p) p[0]=0;
		OperatorConsole.printf("Regen directory = %s\n", PathString);
		itemp = strlen(PathString);
		if(!itemp)
			return (FALSE);
		}

	// add a '/' to end of directory name if not yet present

	if(PathString[itemp-1] != PATHSEPCHAR)
		strcat(PathString, PATHSEPSTR);

	// PathString now contains Image file storage root directory path
	// Regen all files and subdirectories below that or only the selected subdirectory

	if (!IsCacheOrJukeBox)
		{
		if (SubDir)
			{
			strcat(PathString, SubDir);
			if (strchr(SubDir, '*')==NULL) 
                          strcat(PathString, PATHSEPSTR);
			}

		return RegenDir(&DB, PathString, Device);
		}

	// remaining code is for cache or jukebox devices

	strcpy(TempPath, PathString);
	strcat(TempPath, "*.*");

	fdHandle = FindFirstFile(TempPath, &FileData);
	if(fdHandle == INVALID_HANDLE_VALUE)
		return ( FALSE );

	while ( TRUE )
		{
		// traverse one level for CD or cache directories (CDs to be burned)
		if (strcmp(FileData.cFileName, ".") != 0 &&
		    strcmp(FileData.cFileName, "..") != 0
		   )
			{
			strcpy(FileString, FileData.cFileName);

			// Decompose jukebox directory name to find CD number
			// Directory name must be like aaaCCCC where C=CD number a=non-number
			if(strnicmp(Device, "JUKEBOX", 7)==0)
				{
				CDNumber = 0;
				for (i=strlen(FileString)-1; i>=0; i--)
					{
					if (FileString[i] < '0' || FileString[i] > '9')
						{
						CDNumber = atoi(FileString + i + 1);
						break;
						}
					}
				sprintf(ActualDevice, "%s.%d", Device, CDNumber);
				}

			// Decompose cache directory name to find CD number and jukebox number
			// Directory name must be like aaaJJaCCCC where C=CD# J=jukebox# a=non-number
			if(strnicmp(Device, "CACHE", 5)==0)
				{
				CDNumber = 0;
				JUKEBOXNumber = 0;
				for (i=strlen(FileString)-1; i>=0; i--)
					{
					if (FileString[i] < '0' || FileString[i] > '9')
						{
						CDNumber = atoi(FileString + i + 1);
						for (j=i-1; j>=0; j--)
							{
							if (FileString[j] < '0' || FileString[j] > '9')
								{
								JUKEBOXNumber = atoi(FileString + j + 1);
								break;
								}
							}
						break;
						}
					}
				sprintf(ActualDevice, "%s.%d.%d", Device, JUKEBOXNumber, CDNumber);
				}

			strcpy(TempPath, PathString);
			strcat(TempPath, FileString);
			strcat(TempPath, PATHSEPSTR);

			RegenDir(&DB, TempPath, ActualDevice);
			}


		if(!FindNextFile(fdHandle, &FileData))
			break;
		}
	FindClose ( fdHandle );

#ifdef MULTITHREAD
        while(!queueempty()) Sleep(100);
#endif

	return ( TRUE );
	}
	
#else	// linux version

BOOL
Regen(const char *Device, int IsCacheOrJukeBox, char *SubDir)
	{
	int		itemp, i, j, CDNumber, JUKEBOXNumber;
	Database	DB;
	DIR		*dird;
	dirent		*diren;
	char		PathString[1024], TempPath[1024], FileString[1024];
	char		ActualDevice[256];
	char		*p;

	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		{
		fprintf(stderr, "Error Connecting to SQL\n");
		return ( FALSE );
		}

	// convert device specification to a path

	GetPhysicalDevice(Device, PathString);
	itemp = strlen(PathString);
	if(!itemp)
		return FALSE;

	// strip the last subdirectory (e.g., CD0_%04d) for regen of complete CACHE or JUKEBOX devices

	if (IsCacheOrJukeBox)
		{
		PathString[itemp-1]=0;
		p = strrchr(PathString, PATHSEPCHAR);
		if (p) p[0]=0;
		OperatorConsole.printf("Regen directory = %s\n", PathString);
		itemp = strlen(PathString);
		if(!itemp)
			return (FALSE);
		}

	// add a '/' to end of directory name if not yet present

	if(PathString[itemp-1] != PATHSEPCHAR)
		strcat(PathString, PATHSEPSTR);

	// PathString now contains Image file storage root directory path
	// Regen all files and subdirectories below that or only the selected subdirectory

	if (!IsCacheOrJukeBox)
		{
		if (SubDir)
			{
			strcat(PathString, SubDir);
			strcat(PathString, PATHSEPSTR);
			}

		return RegenDir(&DB, PathString, Device);
		}

	// remaining code is for cache or jukebox devices

	strcpy(TempPath, PathString);
	TempPath[strlen(TempPath)-1]=0;

	dird = opendir(TempPath);
	if(dird == NULL)
		return ( FALSE );

	while ( TRUE )
		{
		
		diren = readdir(dird);
		if (!diren) break;
			
		// traverse one level for CD or cache directories (CDs to be burned)
		if (strcmp(diren->d_name, ".") != 0 &&
		    strcmp(diren->d_name, "..") != 0
		   )
			{
			strcpy(FileString, diren->d_name);

			// Decompose jukebox directory name to find CD number
			// Directory name must be like aaaCCCC where C=CD number a=non-number
			if(strnicmp(Device, "JUKEBOX", 7)==0)
				{
				CDNumber = 0;
				for (i=strlen(FileString)-1; i>=0; i--)
					{
					if (FileString[i] < '0' || FileString[i] > '9')
						{
						CDNumber = atoi(FileString + i + 1);
						break;
						}
					}
				sprintf(ActualDevice, "%s.%d", Device, CDNumber);
				}

			// Decompose cache directory name to find CD number and jukebox number
			// Directory name must be like aaaJJaCCCC where C=CD# J=jukebox# a=non-number
			if(strnicmp(Device, "CACHE", 5)==0)
				{
				CDNumber = 0;
				JUKEBOXNumber = 0;
				for (i=strlen(FileString)-1; i>=0; i--)
					{
					if (FileString[i] < '0' || FileString[i] > '9')
						{
						CDNumber = atoi(FileString + i + 1);
						for (j=i-1; j>=0; j--)
							{
							if (FileString[j] < '0' || FileString[j] > '9')
								{
								JUKEBOXNumber = atoi(FileString + j + 1);
								break;
								}
							}
						break;
						}
					}
				sprintf(ActualDevice, "%s.%d.%d", Device, JUKEBOXNumber, CDNumber);
				}

			strcpy(TempPath, PathString);
			strcat(TempPath, FileString);
			strcat(TempPath, PATHSEPSTR);

			RegenDir(&DB, TempPath, ActualDevice);
			}
		}
	closedir(dird);

#ifdef MULTITHREAD
        while(!queueempty()) sleep(1);
#endif

	return ( TRUE );
	}
	
#endif

// Regen *Each* device
BOOL
Regen()
	{
	UINT	Index;
	char	s[20];

//	Index = 0;
//	while ( Index < CACHEDevices )
//		{
//		sprintf(s, "CACHE%d", Index);
//		printf("Regen Device '%s'\n", s);
//		Regen(s, TRUE);
//		++Index;
//		}

	Index = 0;
	while ( Index < JUKEBOXDevices )
		{
		sprintf(s, "JUKEBOX%d", Index);
		printf("Regen Device '%s'\n", s);
		Regen(s, TRUE);
		++Index;
		}

	Index = 0;
	while ( Index < MAGDevices )
		{
		sprintf(s, "MAG%d", Index);
		printf("Regen Device '%s'\n", s);
		Regen(s, FALSE);
		++Index;
		}

	return(FALSE);
	}
	

//////////////////////////////////////////////////////////////////////////////////
// This code is a simple thread safe queue with its own processing thread (WIN32 only)
//////////////////////////////////////////////////////////////////////////////////

#ifdef MULTITHREAD

static struct queue
{ int    top;
  int    bottom;
  int    num;
  int    entrysize;
  char   *data;
  HANDLE threadhandle;
  int    delay;
  CRITICAL_SECTION critical;
  void   (*process)(char *);
} q1;

static BOOL WINAPI processthread(struct queue *q)
{ char *data = (char *)malloc(q->entrysize);

  while (1)
  { while (1)
    { EnterCriticalSection(&q->critical);

      if (q->top!=q->bottom) 
      { memcpy(data, q->data + q->bottom * q->entrysize, q->entrysize);
        q->bottom = (q->bottom+1)%q->num;
        LeaveCriticalSection(&q->critical);

        q->process(data);

        break;
      }

      LeaveCriticalSection(&q->critical);
      Sleep(q->delay);
    }
  }

  DeleteCriticalSection(&q->critical);

  return TRUE;
}

static struct queue *new_queue(int num, int size, int delay, void (*process)(char *))
{ struct queue *result;
  unsigned long ThreadID;

  result               = new queue;
  result->top          = 0;
  result->bottom       = 0;
  result->num          = num;
  result->entrysize    = size;
  result->delay        = delay;
  result->process      = process;
  result->data         = (char *)malloc(num * size);

  InitializeCriticalSection(&result->critical);

  /* Note: since the queue is thread safe it is possible to start more than one thread to service it */

  result->threadhandle = 
    CreateThread(NULL, 0x000ff000, (LPTHREAD_START_ROUTINE) processthread, result, 0, &ThreadID);

  return result;
};

static void into_queue(struct queue *q, char *in)
{ while (1)
  { EnterCriticalSection(&q->critical);
    if ((q->top+1)%q->num != q->bottom)
    { memcpy(q->data + q->top * q->entrysize, in, q->entrysize);
      q->top = (q->top+1)%q->num;
      LeaveCriticalSection(&q->critical);
      return;
    }

    LeaveCriticalSection(&q->critical);
    Sleep(q->delay);
  }
}

///////////////////////////////////////////////////////////////////////////////

static struct queue *q[NTHREADS];
static int qp=0;
static Database *DBs[NTHREADS];

static void regenprocess(char *data)
{ Database *DB;
  memcpy(&DB, data, sizeof(Database *));
  RegenToDatabase(DB, data+128, data+256, data+32);
}

static void regen_queue(char *basename, char *filename, char *device)
{ char data[1512];
  int i;

  if (!q[0]) 
  { for(i=0; i<NTHREADS; i++) 
    { q[i] = new_queue(50, 1512, 10, regenprocess);
      DBs[i] = new Database;
      if (!DBs[i]->Open ( DataSource, UserName, Password, DataHost ) )
      { fprintf(stderr, "Error Connecting to SQL\n");
        return;
      }
    }
    qp=0;
  }

  memcpy(data, &DBs[qp], sizeof(Database *));
  strcpy(data+32, device);
  strcpy(data+128, basename);
  strcpy(data+256, filename);

  into_queue(q[qp], data);
  qp = (qp+1)%NTHREADS;
}

static int queueempty()
{ int i;

  for(i=0; i<NTHREADS; i++)
     if (q[i]->top!=q[i]->bottom) return 0;

  return 1;
}

#endif
