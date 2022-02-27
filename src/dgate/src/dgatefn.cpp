/* MvH 19980326: changed file name generator to ensure correct sorting for avs_rdic    */
/* MvH 19980327: Added FileNameSyntax option in dicom.ini; 0=original, 1=allow sorting */
/* MvH 19980407: avoid /;<>| in filename; philips: up to 9999 series 999999 slices     */
/* MvH+LJZ 19980409: do not free passed DDOPtr on failure: crashed the lot             */
/* MvH 19980415: Error messages to operatorconsole                                     */
/* MvH 19980620: Check error flag from GetDevice (out of disk message)                 */
/* MvH 19981005: Added FileNameSyntax 2: filename starts with SeriesUID_               */
/* MvH 19981005: Replaced size%64 by min(size, 63) for correct truncation              */
/* MvH 19981217: FileNameSyntax 3: same as 2 but accept errors			       */
/* MvH 19981220: Fixed FileNameSyntax code					       */
/* MvH 19990117: FileNameSyntax 4: same as 3 but .dcm extension (forces ch10 format)   */
/* MvH 19990117: Avoid attempts to rewrite image that is not on MAG store (e.g., on CD)*/
/* MvH 19990315: Do not allow '.' in directory name                                    */
/*               in Syntax 3 directory name is patient ID only (better for ACRVIEW)    */
/*               uniqueify more by using time + counter                                */
/* MvH 20000127: FileNameSyntax 4 now does not truncate patid, as 3 already did        */
/* Ljz+MvH 20000131: FileNameSyntax 4 has patient ID only as directory name (same as 3)*/
/* MvH 20000701: Added MaxFileNameLenght option: truncates filename from start         */
/* MvH 20010419: FileNameSyntax 5 on request of Jason Foster - uses patientname as dir */
/* MvH 20011109: Made MAG name check case insensitive                                  */
/* MvH 20020416: Made filename counter 4 digits for when writing >100 files/s	       */
/* MvH 20020416: Made all instances of time() unsigned				       */
/* MvH 20020426: Cannot rewrite image not on MAG store is now a message without ***    */
/* MvH 20020508: The above message will not lead to a fail of GenerateFileName         */
/* MvH 20020802: Added FileNameSyntaxes 6..9; cleaned FileNameSyntax code a bit        */
/* MvH 20020804: Truncate patient ID to 32 chars for mode 6 and 8 (make sure len<255)  */
/* MvH 20021016: Pass indexed patid to GetFilename for speed			       */
/* MvH 20021018: GenerateFileName has NoKill option (for interactive dgate tasks)      */
/* MvH 20021223: Added forbidden chars *, ?, " and : (comment from Thomas Buerer)      */
/* ljz 20030401: Overrule '.v2' filenames with '.dcm' when object is not               */
/*               ImplicitLittleEndian                                                  */
/* ljz 20030522: Replaced 'static int counter' by 'unsigned int gl_iCounter'           */
/* ljz 20030606: Fixed incremental counter when creating UIDs or generating filenames  */
/* ljz 20030730: Overrule v2 only when not ImplicitLittleEndian, ExplicitLittleEndian  */
/*               or ExplicitBigEndian (c.f. 20020401)                                  */
/* MvH 20030819: Allow longer filenames      					       */
/* MVH 20030921: Added DEVICE_TYPE_MIRROR TO GetDevice call			       */
/* MvH 20040129: Added FileNameSyntax 10 = all files in one directory                  */
/* ljz 20040301: Small fix for FileNameSyntax 10                                       */
/* mvh 20041108: Added Syntax input to GenerateFileName (overrides ini setting)	       */
/* mvh 20050107: Adapted for linux compile					       */
/* ljz 20050126: Added FileNameSyntax 11: StudyUID\SeriesUID\SOPUID.dcm                */
/* ljz 20050131: Changes root to 'name' for FileNameSyntax 11                          */
/* mvh 20051210: Clear root output string when image already in database               */
/* kld 20060312: FileNameSyntax 12: similar to 11 but uses Modality_StudyID as dir     */
/* mvh 20060312: Checked it: 12=name_ID\Modality_StudyID\SeriesUID\SOPUID.dcm 	       */
/* mvh 20060314: Change: 12=name_ID\Modality_StudyID\SeriesID\SOPUID.dcm 	       */
/* mvh 20060314: new filenamesyntax %name_%id\%modality_%studyid\%seriesid\%sopuid.dcm */
/* mvh 20060317: Added calledae and callingae to GenerateFilename flexible syntax      */
/* mvh 20060325: imagenum is image number as text, fix imageid, accept all errors      */
/* mvh 20060402: Fix potential problem when patient ID not there                       */
/* mvh 20060701: Speeded string handling in new filenamesyntax                         */
/* mvh 20060702: Pass DB to GenerateFilename to avoid zillions of db open and closes   */
/* mvh 20070125: Fix %modality; add %studydate and substring (year = %studydate[0,3])  */
/* mvh 20071027: Extended length of FileNameSyntz parameter to 256		       */
/* mvh 20071027: Stripped spaces of called and calling in flexible filenamesyntax      */
/* mvh 20071030: Small fix in the above                                                */
/* mvh 20071104: Added %series: 4 digit series number                                  */
/*
20090620	jf	Include file stuff
20090926	mvh	Added %studydesc and %vxxxx,yyyy etc
20090927	mvh	Added Cleanup ro %vxxxx,yyyy
20091231	bcb	Changed char* to const char* for gcc4.2 warnings
20100111	mvh	Merged; but cast Time to print to (unsigned int)
20100125	mvh	GetPrivateProfileString -> MyGetPrivateProfileString
20100126	mvh	Fixed readout of studydate (typo mix with studydesc)
20100303	bcb	Changed int to unsigned int, commented out unused variables (gcc4.2 Warnings)
20100703	mvh	Merged
20100828	mvh	Update history fix
20101222	mvh	Fixed that ascii codes > 128 where replaced by zero (truncated) in Cleanup
20110201	mvh	Changed prototype of SearchDICOMObject
20110606	mvh	A change of extension when rewriting is now allowed; object is then deleted first
20120319	mvh	Implemented lua: filenamesyntax
20121208        mvh     Added %date to filenamesyntax gives yyyymmdd
20131013	mvh	Default FileNameSyntax set to 4
20160316	mvh	Avoid buffer overflow reading string[-1]
20220227	mvh	Added RenameOnRewrite flag: will force rename of any object rewritten 
*/

#ifndef	WHEDGE
#	include	"dgate.hpp"

#else
#	include	"master.h"
#endif

#include "gpps.hpp" //DUCKHEAD92
#include <time.h>

#ifndef UNIX
#define PATHSEPSTR "\\"
#define PATHSEPCHAR '\\'
#else
#define PATHSEPSTR "/"
#define PATHSEPCHAR '/'
//#define memicmp(s1, s2, n) strncasecmp(s1, s2, n)
#endif

#ifndef	RootConfig
extern	char	RootConfig[];
#endif
#ifndef	ConfigFile
extern	char	ConfigFile[];
#endif

unsigned int gl_iFileCounter=0;

static int
dgatefnmin(int a, int b)
	{
	if (a<b) 
		return a;
	else
		return b;
	}

static char FileNameSyntaxString[256];
static int RenameOnRewrite=0;

static
int
GetFileNameSyntax(unsigned int *maxfilenamelength)
	{
	static int 	FileNameSyntax=-1;
	static unsigned int 	MaxLength     = 255;
	char		RootSC[64];
	char		Temp[128];

	if (FileNameSyntax<0)
		{
		MyGetPrivateProfileString ( RootConfig, "MicroPACS", RootConfig,
			(char*) RootSC, 64, ConfigFile);

		MyGetPrivateProfileString ( RootSC, "FileNameSyntax", "4",
			(char*) FileNameSyntaxString, 256, ConfigFile);
		FileNameSyntax = atoi(FileNameSyntaxString);
		if (strchr(FileNameSyntaxString, '%'))          FileNameSyntax=999;	// configured by string
		if (memcmp(FileNameSyntaxString, "lua:", 4)==0) FileNameSyntax=998;	// lua done by caller (dcm assumed)

		MyGetPrivateProfileString ( RootSC, "MaxFileNameLength", "255",
			(char*) Temp, 128, ConfigFile);
		MaxLength = (unsigned int)atoi(Temp);

		MyGetPrivateProfileString ( RootSC, "RenameOnRewrite", "0",
			(char*) Temp, 128, ConfigFile);
		RenameOnRewrite = atoi(Temp);
		}

	if (maxfilenamelength) *maxfilenamelength = MaxLength;
        return FileNameSyntax;
	}


/* Cleanup e.g., patient name */

static void Cleanup(char *name)
	{
	int Index;
	unsigned char *Name = (unsigned char *)name;
	
	Index = 0;
	while(Name[Index])
		{
		switch(Name[Index])
			{
			case	'/':
			case	'\\':
			case	';':
			case	'>':
			case	'<':
			case	'|':
			case	',':
			case	'.':
			case	' ':
			case	':':
			case	'?':
			case	'*':
			case	'"':
				Name[Index] = '_';
				break;
			case	13:
			case	10:
				Name[Index] = '\0';
				break;
				
			}
		if(Name[Index]<33)
			{
			Name[Index] = '\0';
			break;
			}
		++Index;
		if(Index>64)
			{
			Name[Index] = '\0';
			break;
			}
		}

	while(Name[strlen(name)-1]=='_')
		Name[strlen(name)-1] = '\0';

	if(!strlen(name))
		strcpy(name, "unknown");
	}

#define __SearchDICOMObject
int SearchDICOMObject(DICOMObject *DDO, const char *desc, char *result, Array < DICOMDataObject * > **A = NULL, DICOMDataObject **O = NULL);

BOOL DeleteImageFile(char *filename, BOOL KeepImages);

// Generate a file name prefix for an IOD
BOOL
GenerateFileName(
	DICOMDataObject	*DDOPtr,
	char	*Device,
	char	*Root,
	char	*filename,
	BOOL	NoKill,
	int	Syntax,
	char	*called,
	char	*calling,
	Database *db)
	{
	VR	*ImageNumber;
	VR	*ImageID;
	VR	*SeriesNumber;
	VR	*SeriesUID;
	VR	*StudyUID;
	VR	*StudyID;
	VR	*Modality;
	VR	*SOPUID;
	VR	*PatientID;
	VR	*PatientName;
//	VR	*TempVR;
	VR	*StudyDate;
	VR	*StudyDesc;
	unsigned int maxfilenamelength;
	int	iSeries, iImage;//, Index;
	char	Path[1024];//, Path1[1024];
	char	CName[65];
	char	CID[65];
	char	CSeries[65];
	char	CImageNum[65];
	char	CImageID[65];
	char	CSeriesUID[65];
	char	CStudyUID[65];
	char	CStudyID[65];
	char	CModality[65];
	char	CStudyDate[65];
	char	CStudyDesc[65];
	char	CSOPUID[65];
	char	CMod[130];
	char	Called[64];
	char	Calling[64];
	int	AcceptError;
	int	iFileNameSyntax;
	int	i, l, L;
	char	*p, *q;

	// Name syntax >=3 do not use image or series number and can store items without these
	AcceptError = GetFileNameSyntax(NULL)>=3;

	if (Syntax) iFileNameSyntax = Syntax;
        else        iFileNameSyntax = GetFileNameSyntax(NULL);

	// First, make test if image is already in the database.  In which
	// case we allow the PACS to rewrite it.

	memset((void*)CID, 0, 65);
	PatientID = DDOPtr->GetVR(0x0010, 0x0020);
	if(PatientID)
		{
	        strncpy(CID, (char*) PatientID->Data, dgatefnmin(PatientID->Length,64));
		if (PatientID->Length)
	        	if (CID[PatientID->Length-1]==' ') CID[PatientID->Length-1]=0;
		}

	if (GetFileName ( DDOPtr->GetVR(0x0008, 0x0018),
			filename, Device, *db, FALSE, CID))
		{
		char dcm[] = ".dcm";
		char v2[]  = ".v2";

		if (memicmp(Device, "MAG", 3) != 0)
			{
			OperatorConsole.printf("Cannot rewrite image that is not on MAG store\n");
			return TRUE;
			}

		*Root=0;

		// check that the extension is changing; if so delete the original
		p = strrchr(filename, '.');

		if (iFileNameSyntax==4 || iFileNameSyntax==998 || (iFileNameSyntax>=8 && iFileNameSyntax<=12)) 
			q = dcm;
		else if (iFileNameSyntax!=999) 
			q = v2;
		else
			q = strrchr(FileNameSyntaxString, '.');
	
		if ((p && q && strcmp(p, q)!=0) || RenameOnRewrite!=0)
			{
			// changing extension: removed stored image
			//DeleteImageFile(filename, FALSE);
			DICOMDataObject	DDO;
      			VR *vr;

			vr = DDOPtr->GetVR(0x0008, 0x0018);
			vr = new VR(0x0008, 0x0018, vr->Length, vr->Data, (BOOL) FALSE );
			DDO.Push(vr);

			vr = DDOPtr->GetVR(0x0010, 0x0020);
			vr = new VR(0x0010, 0x0020, vr->Length, vr->Data, (BOOL) FALSE );
			DDO.Push(vr);

			RemoveFromPACS(&DDO, FALSE);
			}
		else
			return TRUE;
		}


	// return if GetDevice fails (out of disk space ?)

	if (!GetDevice(Device, NoKill ? (DEVICE_TYPE_MAG | DEVICE_TYPE_MIRROR | DEVICE_OPTI | DEVICE_TYPE_NOKILL):(DEVICE_TYPE_MAG | DEVICE_TYPE_MIRROR | DEVICE_OPTI)) ) 
		{
		OperatorConsole.printf("*** Out of disk space\n");
		return ( FALSE );
		}

	memset((void*)CID, 0, 65);
	memset((void*)CName, 0, 65);
	memset((void*)CSeries, 0, 65);
	memset((void*)CImageNum, 0, 65);
	memset((void*)CImageID, 0, 65);
	memset((void*)CSeriesUID, 0, 65);
	memset((void*)CStudyUID, 0, 65);
	memset((void*)CStudyID, 0, 65);
	memset((void*)CModality, 0, 65);
	memset((void*)CStudyDate, 0, 65);
	memset((void*)CStudyDesc, 0, 65);
	memset((void*)CSOPUID, 0, 65);

	ImageNumber = DDOPtr->GetVR(0x0020, 0x0013);
	if(!ImageNumber)
		{
		OperatorConsole.printf("No image number\n");
		strcpy(CImageNum, "empty");
		iImage = 0;
		//if (!AcceptError) return ( FALSE );
		}
	else
		{
		strncpy(CImageNum, (char*) ImageNumber->Data, dgatefnmin(ImageNumber->Length,64));
		iImage = atoi(CImageNum);
		}

	SeriesNumber = DDOPtr->GetVR(0x0020, 0x0011);
	if(!SeriesNumber)
		{
		OperatorConsole.printf("No Series number\n");
		strcpy(CSeries, "empty");
		iSeries = 0;
		//if (!AcceptError) return ( FALSE );
		}
	else
		{
		strncpy(CSeries, (char*) SeriesNumber->Data, dgatefnmin(SeriesNumber->Length,64));
		iSeries = atoi(CSeries);
		}

	SeriesUID = DDOPtr->GetVR(0x0020, 0x000e);
	if(!SeriesUID)
		{
		OperatorConsole.printf("No Series UID\n");
		strcpy(CSeriesUID, "empty");
		//if (!AcceptError) return ( FALSE );
		}
	else
		strncpy(CSeriesUID, (char*) SeriesUID->Data, dgatefnmin(SeriesUID->Length,64));

	StudyID = DDOPtr->GetVR(0x0020, 0x0010);
	if(!StudyID)
		{
		//OperatorConsole.printf("No Study ID\n");
		strcpy(CStudyID, "empty");
		//if (!AcceptError) return ( FALSE );
		}
	else
		{
		strncpy(CStudyID, (char*) StudyID->Data, dgatefnmin(StudyID->Length,64));
		if (StudyID->Length) if (CStudyID[strlen(CStudyID)-1]==' ') CStudyID[strlen(CStudyID)-1]=0;
		}

	Modality = DDOPtr->GetVR(0x0008, 0x0060);
	if(!Modality)
		{
		//OperatorConsole.printf("No Modality\n");
		strcpy(CModality, "empty");
		//if (!AcceptError) return ( FALSE );
		}
	else
		{
		strncpy(CModality, (char*) Modality->Data, dgatefnmin(Modality->Length,64));
		if (Modality->Length) if (CModality[strlen(CModality)-1]==' ') CModality[strlen(CModality)-1]=0;
		}

	StudyDate = DDOPtr->GetVR(0x0008, 0x0020);
	if(!StudyDate)
		{
		//OperatorConsole.printf("No Study Date\n");
		strcpy(CStudyDate, "empty");
		//if (!AcceptError) return ( FALSE );
		}
	else
		{
		strncpy(CStudyDate, (char*) StudyDate->Data, dgatefnmin(StudyDate->Length,64));
		if (StudyDate->Length) if (CStudyDate[strlen(CStudyDate)-1]==' ') CStudyDate[strlen(CStudyDate)-1]=0;
		}

	StudyDesc = DDOPtr->GetVR(0x0008, 0x1030);
	if(!StudyDesc)
		{
		//OperatorConsole.printf("No Study Date\n");
		strcpy(CStudyDesc, "empty");
		//if (!AcceptError) return ( FALSE );
		}
	else
		{
		strncpy(CStudyDesc, (char*) StudyDesc->Data, dgatefnmin(StudyDesc->Length,64));
		if (StudyDesc->Length) if (CStudyDesc[strlen(CStudyDesc)-1]==' ') CStudyDesc[strlen(CStudyDesc)-1]=0;
		}

	StudyUID = DDOPtr->GetVR(0x0020, 0x000d);
	if(!StudyUID)
		{
		OperatorConsole.printf("No Study UID\n");
		strcpy(CStudyUID, "empty");
		//if (!AcceptError) return ( FALSE );
		}
	else
		strncpy(CStudyUID, (char*) StudyUID->Data, dgatefnmin(StudyUID->Length,64));

	SOPUID = DDOPtr->GetVR(0x0008, 0x0018);
	if(!SOPUID)
		{
		OperatorConsole.printf("No SOP UID\n");
		strcpy(CSOPUID, "empty");
		//if (!AcceptError) return ( FALSE );
		}
	else
		strncpy(CSOPUID, (char*) SOPUID->Data, dgatefnmin(SOPUID->Length,64));

	PatientID = DDOPtr->GetVR(0x0010, 0x0020);
	if(!PatientID)
		{
		OperatorConsole.printf("No Patient ID\n");
		strcpy(CID, "empty");
		//if (!AcceptError) return(FALSE);
		}
	else
		strncpy(CID, (char*) PatientID->Data, dgatefnmin(PatientID->Length,64));

	PatientName = DDOPtr->GetVR(0x0010, 0x0010);
	if(!PatientName)
		{
		OperatorConsole.printf("No Patient Name\n");
		strcpy(CName, "empty");
		//if (!AcceptError) return(FALSE);
		}
	else
		strncpy(CName, (char*) PatientName->Data, dgatefnmin(PatientName->Length,64));

	ImageID = DDOPtr->GetVR(0x0054, 0x0400);
	if(!ImageID)
		{
		//OperatorConsole.printf("No Image ID\n");
		strcpy(CImageID, "empty");
		//if (!AcceptError) return ( FALSE );
		}
	else
		{
		strncpy(CImageID, (char*) ImageID->Data, dgatefnmin(ImageID->Length,64));
		if (ImageID->Length) if (CImageID[strlen(CImageID)-1]==' ') CImageID[strlen(CImageID)-1]=0;
		}

	/* Cleanup various strings */
	Cleanup(CName);
	Cleanup(CID);
	Cleanup(CStudyID);
	Cleanup(CModality);
	Cleanup(CSeries);
	Cleanup(CImageNum);
	Cleanup(CImageID);
	Cleanup(CStudyDesc);

	Called[0]=0;
	if (called) strcpy(Called, called);
        while (strlen(Called)>0 && Called[strlen(Called)-1]==' ') Called[strlen(Called)-1] = 0;

	Calling[0]=0;
	if (calling) strcpy(Calling, calling);
        while (strlen(Calling)>0 && Calling[strlen(Calling)-1]==' ') Calling[strlen(Calling)-1] = 0;

	/* make directory and filename for a range of FilenameSyntax values */
	switch (iFileNameSyntax)
		{
		case 1:
  		CID [8]   = '\0';
		CName [8] = '\0';
		strcpy(Root, CName);
	        strcat(Root, "_");
	  	strcat(Root, CID);
		sprintf(filename, "%s%s%04d_%06d_%u%04x.v2", Root, PATHSEPSTR, iSeries, iImage, (unsigned int)time(NULL), (gl_iFileCounter++) & 0xffff);
		break;

		case 2:
  		CID [8]   = '\0';
		CName [8] = '\0';
		strcpy(Root, CName);
	        strcat(Root, "_");
	  	strcat(Root, CID);
		sprintf(filename, "%s%s%s_%04d_%06d_%u%04x.v2", Root, PATHSEPSTR, CSeriesUID, iSeries, iImage, (unsigned int)time(NULL), (gl_iFileCounter++) & 0xffff);
		break;

		case 3:
  		CID [16] = '\0';
		strcpy(Root, CID);
		sprintf(filename, "%s%s%s_%04d_%06d_%u%04x.v2", Root, PATHSEPSTR, CSeriesUID, iSeries, iImage, (unsigned int)time(NULL), (gl_iFileCounter++) & 0xffff);
		break;

		case 4:
	  	CID [16] = '\0';
		strcpy(Root, CID);
		sprintf(filename, "%s%s%s_%04d_%06d_%u%04x.dcm", Root, PATHSEPSTR, CSeriesUID, iSeries, iImage, (unsigned int)time(NULL), (gl_iFileCounter++) & 0xffff);
		break;

		case 5:
		strcpy(Root, CName);
		sprintf(filename, "%s%s%s_%04d_%06d_%u%04x.v2", Root, PATHSEPSTR, CSeriesUID, iSeries, iImage, (unsigned int)time(NULL), (gl_iFileCounter++) & 0xffff);
		break;

		case 6:
  		CID [32] = '\0';
		strcpy(Root, CID);
		sprintf(filename, "%s%s%s%s%s%s%s.v2", CID, PATHSEPSTR, CStudyUID, PATHSEPSTR, CSeriesUID, PATHSEPSTR, CSOPUID);
		break;

		case 7:
		strcpy(Root, CStudyUID);
		sprintf(filename, "%s%s%s%s%s.v2", CStudyUID, PATHSEPSTR, CSeriesUID, PATHSEPSTR, CSOPUID);
		break;

		case 8:
  		CID [32] = '\0';
		strcpy(Root, CID);
		sprintf(filename, "%s%s%s%s%s%s%s.dcm", CID, PATHSEPSTR, CStudyUID, PATHSEPSTR, CSeriesUID, PATHSEPSTR, CSOPUID);
		break;

		case 9:
		strcpy(Root, CStudyUID);
		sprintf(filename, "%s%s%s%s%s.dcm", CStudyUID, PATHSEPSTR, CSeriesUID, PATHSEPSTR, CSOPUID);
		break;

		case 10:
		strcpy(Root, "");
		sprintf(filename, "Images%s%s.dcm", PATHSEPSTR, CSOPUID);
		break;

		case 11:
		strcpy(Root, CName);
		sprintf(filename, "%s%s%s%s%s%s%s.dcm", Root, PATHSEPSTR, CStudyUID, PATHSEPSTR, CSeriesUID, PATHSEPSTR, CSOPUID);
		break;

		case 12:
		strcpy(Root, CName);
		strcat(Root, "_");
		strcat(Root, CID);
		strcpy(CMod, CModality);
		strcat(CMod, "_");
		strcat(CMod, CStudyID);
		sprintf(filename, "%s%s%s%s%s%s%s.dcm", Root, PATHSEPSTR, CMod, PATHSEPSTR, CSeries, PATHSEPSTR, CSOPUID);
		break;

		case 998:	// lua filename done by caller
		strcpy(filename, FileNameSyntaxString);
		break;

		case 999:	// configured filename: any string with %, e.g., %studydate[0,3]\%name_%id\%sopuid.v2
                filename[0] = 0;
                i = 0;
                L = strlen(FileNameSyntaxString);
                while (i<L)
                { if (FileNameSyntaxString[i] == '%')
                  { char *ps = filename + strlen(filename);

                    i++;
                    p = FileNameSyntaxString + i;
		    if      (memcmp(p, "name"     , 4)==0) i+=4, strcat(filename, CName);
		    else if (memcmp(p, "id"       , 2)==0) i+=2, strcat(filename, CID);
		    else if (memcmp(p, "modality" , 8)==0) i+=8, strcat(filename, CModality);
		    else if (memcmp(p, "studyid"  , 7)==0) i+=7, strcat(filename, CStudyID);
		    else if (memcmp(p, "studyuid" , 8)==0) i+=8, strcat(filename, CStudyUID);
		    else if (memcmp(p, "seriesid" , 8)==0) i+=8, strcat(filename, CSeries);
		    else if (memcmp(p, "seriesuid", 9)==0) i+=9, strcat(filename, CSeriesUID);
		    else if (memcmp(p, "sopuid"   , 6)==0) i+=6, strcat(filename, CSOPUID);
		    else if (memcmp(p, "imageid"  , 7)==0) i+=7, strcat(filename, CImageID);
		    else if (memcmp(p, "imagenum" , 8)==0) i+=8, strcat(filename, CImageNum);
		    else if (memcmp(p, "image"    , 5)==0) i+=5, sprintf(filename+strlen(filename), "%06d", iImage);
		    else if (memcmp(p, "series"   , 6)==0) i+=6, sprintf(filename+strlen(filename), "%04d", iSeries);
		    else if (memcmp(p, "time"     , 4)==0) i+=4, sprintf(filename+strlen(filename), "%u", (unsigned int)time(NULL));
		    else if (memcmp(p, "date"     , 4)==0) 
		    { i+=4;
		      time_t rawtime;
                      struct tm * timeinfo;
                      char buffer [80];
 
                      time ( &rawtime );
                      timeinfo = localtime ( &rawtime );
                      strftime (buffer,80,"%Y%m%d", timeinfo);
                      sprintf(filename+strlen(filename), "%s", buffer);
		    }
		    else if (memcmp(p, "counter"  , 7)==0) i+=7, sprintf(filename+strlen(filename), "%04x", (gl_iFileCounter++) & 0xffff);
		    else if (memcmp(p, "calledae" , 8)==0) i+=8, strcat(filename, Called);
		    else if (memcmp(p, "callingae", 9)==0) i+=9, strcat(filename, Calling);
		    else if (memcmp(p, "studydate", 9)==0) i+=9, strcat(filename, CStudyDate);
		    else if (memcmp(p, "studydesc", 9)==0) i+=9, strcat(filename, CStudyDesc);
                    else if (memcmp(p, "v"        , 1)==0) { i+=1+SearchDICOMObject(DDOPtr, p+1, ps); Cleanup(ps); }
                    else if (memcmp(p, "V"        , 1)==0) { i+=1+SearchDICOMObject(DDOPtr, p+1, ps); Cleanup(ps); }


  	  	    // get substring of variable
                    if (FileNameSyntaxString[i]=='[')
                    { int a=0, b=-1;
                      sscanf(FileNameSyntaxString+i, "[%d,%d]", &a, &b);
                      for (;i<L;i++) if (FileNameSyntaxString[i-1]==']') break;
                      if (a>(signed int)strlen(ps)) a=strlen(ps);
                      if (b>(signed int)strlen(ps)) b=strlen(ps);
                      if (b>=a) {memmove(ps, ps+a, b-a+1); ps[b-a+1]=0;}
                    }

		    // unrecognized names are not translated and appear as such in filename - good for debugging
                  }
                  else
                  { l = strlen(filename);
                    filename[l]   = FileNameSyntaxString[i++];
                    filename[l+1] = 0;
                  }
                }
		break;

		default:
  		CID [8]   = '\0';
		CName [8] = '\0';
		strcpy(Root, CName);
	        strcat(Root, "_");
	  	strcat(Root, CID);
		sprintf(filename, "%s%s%d_%d_%u.v2", Root, PATHSEPSTR, iSeries, iImage, (unsigned int)time(NULL));
		break;
		}

	/* Overrule the FileNameSyntax when 'v2' is desired but the DicomObject is
	   not ImplicitLittleEndian, ExplicitLittleEndian or ExplicitBigEndian.
	   In that case, make 'dcm' files.
	*/
	if ((iFileNameSyntax != 4) &&
	    (iFileNameSyntax != 8) &&
	    (iFileNameSyntax != 9) &&
	    (iFileNameSyntax != 10) &&
	    (iFileNameSyntax != 11) &&
	    (iFileNameSyntax != 12))
		{
		VR*	pVR;
		char	s[256];

		pVR = DDOPtr->GetVR(0x0002, 0x0010);	// TransferSyntaxUID
		if (pVR && pVR->Data)
			{
			strncpy(s, (char*)pVR->Data, pVR->Length);
			s[pVR->Length] = 0;
			if ((strcmp(s, "1.2.840.10008.1.2")   != 0) &&
			    (strcmp(s, "1.2.840.10008.1.2.1") != 0) &&
			    (strcmp(s, "1.2.840.10008.1.2.2") != 0))
				strcpy(strrchr(filename, '.'), ".dcm");
			}
		}

        GetFileNameSyntax(&maxfilenamelength);
	maxfilenamelength += strlen(Root) + 1;

        if (strlen(filename) > maxfilenamelength)
		{
		strcpy(Path, filename + strlen(Root) + 1 + strlen(filename) - maxfilenamelength);
		strcpy(filename + strlen(Root) + 1, Path);
		}

	return ( TRUE );
	}

