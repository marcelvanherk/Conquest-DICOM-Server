// Small echo DLL used to control the dicom server from the delphi GUI
// mvh 20181215; based on code by ljz and lsp
// mvh 20200110; Allow change of transfer syntax
// mvh 20220809; Added executable version servertask lua_chunk
// mvh 20221013; Allow '<file' mode to return response
// mvh 20221014; Suppress dicom error messages for luastart
// mvh 20221016; Padd VR identifying odd length if needed for file upload
// mvh 20230607; Fix that any --d command did dolua:
// mvh 20230619; Fix in above fix

#include "dicom.hpp"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#ifndef UNIX
#	include <io.h>
#else
#       include <unistd.h>
#       include <dirent.h>
#       include <pthread.h>
#       include <stdlib.h>
#endif

#	include <stdio.h>
#	include <ctype.h>
#	include <fcntl.h>
#	include <sys/stat.h>

#ifndef UNIX
int gl_LastError;
#define DCM_E_OK 0
#define DCM_E_CONNECT 7
#define DCM_E_ASSOC 12
#define DCM_E_MOVE_RSP 14
#define DCM_E_REMOTE 20
#define DCM_E_OUTPUTFILE 21

class	EPDU_Service	:
	public		PDU_Service
	{
		BOOL	AddTransferSyntaxs(PresentationContext &);	// override original function
	};

BOOL	EPDU_Service	::	AddTransferSyntaxs(PresentationContext &PresContext)
	{
	UID		uid;
	TransferSyntax	TrnSyntax;

	uid.Set("1.2.840.10008.1.2");		// ImplicitLittleEndian
	TrnSyntax.Set(uid);
	PresContext.AddTransferSyntax(TrnSyntax);
	return ( TRUE );
	}

RTC	VRType(TRUE);					// VR translation table
	
int WINAPI DcmEcho(	char* pszLocalAE,
			char* pszRemoteAE,
			char* pszRemoteIP,
			char* pszRemotePort,
			char* pszNKIcommand1,
			char* pszNKIcommand2)
{
	int			rc;
	EPDU_Service		PDU;
	DICOMCommandObject	DCO;
	DICOMCommandObject	DCOR;
	DICOMDataObject		DDO;
	DICOMDataObject		DDOR;
	UID			uid;
	VR			*vr;
	LE_UINT16		command, datasettype, messageid;
	BYTE			SOP[64];
	TransferSyntax		TrnSyntax;
	PresentationContext     PresContext;
	
        gl_LastError = DCM_E_OK;

        VRType.AttachRTC(".\\dgate.dic");
	PDU.AttachRTC(&VRType);
	PDU.ClearAbstractSyntaxs();
	PDU.SetLocalAddress((unsigned char*)pszLocalAE);
	PDU.SetRemoteAddress((unsigned char*)pszRemoteAE);
	

/*FILE *f; char text[64]; int typ;
typ=VRType.RunTimeClass(0x9999, 0x0300, text);
f = fopen("test.txt", "wb");
fwrite(text, 1, 64, f);
fprintf(f, "%c%c", typ/256, typ&255);
fclose(f);*/

	uid.Set("1.2.840.10008.3.1.1.1");	// Application context (DICOM App)
	PDU.SetApplicationContext(uid);
	
	uid.Set("1.2.840.10008.1.1");	// Verification
	PDU.AddAbstractSyntax(uid);

	if(!PDU.Connect((unsigned char*)pszRemoteIP,
		(unsigned char*)pszRemotePort))
	  return DCM_E_CONNECT;

	strcpy((char*) SOP, "1.2.840.10008.1.1"); // Verification
	vr = new VR (0x0000, 0x0002, strlen((char*)SOP), (void*) SOP, FALSE);
	DCO.Push(vr);

	command = 0x0030;
	vr = new VR (0x0000, 0x0100, 0x0002, &command, FALSE);
	DCO.Push(vr);
	messageid = 7;
	vr = new VR (0x0000, 0x0110, 0x0002, &messageid, FALSE);
	DCO.Push(vr);
	datasettype = 0x0101;
	vr = new VR (0x0000, 0x0800, 0x0002, &datasettype, FALSE);
	DCO.Push(vr);

	if (pszNKIcommand1 && strlen(pszNKIcommand1) && (pszNKIcommand1[0] != '#'))
	{ vr = new VR (0x9999, 0x0300, strlen((char*)pszNKIcommand1), (void*)pszNKIcommand1, FALSE);
          vr->TypeCode='UN';
	  DCO.Push(vr);
	}

	if (pszNKIcommand2 && strlen(pszNKIcommand2))
	{ vr = new VR (0x9999, 0x0400, strlen((char*)pszNKIcommand2), (void*)pszNKIcommand2, FALSE);
          vr->TypeCode='UN';
	  DCO.Push(vr);
	}

	uid.Set("1.2.840.10008.1.1");	// Verification
	PDU.Write(&DCO, uid);

	if(!PDU.Read(&DCOR))
          return DCM_E_ASSOC;

	// is this a C-ECHO-RSP ?
	if(DCOR.GetUINT16(0x0000, 0x0100)!=0x8030)
	{ rc = DCM_E_MOVE_RSP;
	  goto EXIT;
	}

	if (pszNKIcommand1 && strlen(pszNKIcommand1) && (pszNKIcommand1[0] == '#'))
	{	// Dump EchoResponse to file
		FILE*		f;
		unlink(pszNKIcommand1 + 1);
		f = fopen(pszNKIcommand1 + 1, "wb");
		if (!f)
		{
			rc = DCM_E_OUTPUTFILE;
			goto EXIT;
		}
		vr = DCOR.GetVR(0x9999, 0x0401);
		if (vr)
			/* Don't write terminating nil or space */
			if ((vr->Length > 0) &&
			    ((((char*)vr->Data)[vr->Length] == ' ') ||
			     (((char*)vr->Data)[vr->Length] == 0x00)))
				fwrite(vr->Data, 1, vr->Length - 1, f);
			else
				fwrite(vr->Data, 1, vr->Length, f);
		fclose(f);
	}
	if(DCOR.GetUINT16(0x0000, 0x0900)==0x0000)	// Completed ?
	{
		rc = DCM_E_OK;
		goto EXIT;
	}
	else
	{
		vr = DCOR.GetVR(0x0000, 0x0902);
		char	s[256];
		if(vr)
		{
			memset((void*)s, 0, 256);
			memcpy((void*)s, vr->Data, vr->Length%250);
		}
		else
		{
			UINT16	RemoteError =	DCOR.GetUINT16(0x0000, 0x0900);
		}

		rc = DCM_E_REMOTE;
		goto EXIT;
	}

EXIT:
	DCOR.Reset();
	PDU.Close();
        return gl_LastError==DCM_E_OK?rc:gl_LastError;
}

#endif

#ifdef EXE

// Compute size of a file
static unsigned int 
DFileSize(char *Path)
	{
	FILE *f;
	unsigned int res;

	f = fopen(Path, "rb");
	if (f==NULL) return 0;

	fseek(f, 0, SEEK_END);
  	res = ftell(f); /* ask position */
	fclose(f);

	return res;
	}

int processhtml(char *out, char *in, int len)
	{	
	int i, outlen=0, instring=0;
	for (i=0; i<len; i++)
		{
		if (i>3 && in[i-3]=='<' && in[i-2]=='A' && instring==0) instring=1;
		if (in[i]=='>' && instring==1) instring=0;

		if (in[i]==' ' && instring) { out[outlen++] = '%'; out[outlen++] = '2'; out[outlen++] = '0'; }
		else 		              out[outlen++] = in[i];
		}
	return outlen;
	}
	
char MYACRNEMA[128]="CONQUESTSRV1";
char ServerCommandAddress[128]="127.0.0.1";
char Port[128]="5678";

void MyDicomError(int error, const char *message, int info)
	{ 
	}

// returns 1 on failure, 0 on success, -1 if value is returned on con, buf or L
static int SendServerCommand(const char *NKIcommand1, const char *NKIcommand2, int con, char *buf, BOOL html, BOOL upload, lua_State *L)
	{
	PDU_Service		PDU;
	DICOMCommandObject	DCO;
	DICOMCommandObject	DCOR;
	UID			uid;
	VR			*vr;
	LE_UINT16		command, datasettype, messageid;//, tuint16;
	BYTE			SOP[64];
	int			rc=0;
	
	if (strstr(NKIcommand2, "luastart:"))
	  SetDicomErrorHandler(&MyDicomError);

	PDU.ClearAbstractSyntaxs();
	PDU.SetLocalAddress((BYTE *)MYACRNEMA);
	PDU.SetRemoteAddress((BYTE *)MYACRNEMA);
	uid.Set("1.2.840.10008.3.1.1.1");	// Dicom APP
	PDU.SetApplicationContext(uid);
	uid.Set("1.2.840.10008.1.1");		// Verification
	PDU.AddAbstractSyntax(uid);

	if(!PDU.Connect((unsigned char *)ServerCommandAddress, (BYTE *)Port))
		return ( 1 );
	
	strcpy((char*) SOP, "1.2.840.10008.1.1"); // Verification
	vr = new VR (0x0000, 0x0002, strlen((char*)SOP), (void*) SOP, FALSE);
	DCO.Push(vr);

	if (strlen(NKIcommand1))
	{ vr = new VR (0x9999, 0x0300, strlen((char*)NKIcommand1), (void*)NKIcommand1, FALSE);
	  DCO.Push(vr);
	}

	if (strlen(NKIcommand2))
	{ vr = new VR (0x9999, 0x0400, strlen((char*)NKIcommand2), (void*)NKIcommand2, FALSE);
	  DCO.Push(vr);
	}

	command = 0x0030;
	vr = new VR (0x0000, 0x0100, 0x0002, &command, FALSE);
	DCO.Push(vr);

	datasettype = 0x0101;	
	vr = new VR (0x0000, 0x0800, 0x0002, &datasettype, FALSE);
	DCO.Push(vr);

	messageid = 0x0001;
	vr = new VR (0x0000, 0x0110, 0x0002, &messageid, FALSE);
	DCO.Push(vr);

	if (upload && buf)
	{ unsigned int len = DFileSize(buf);
	  vr = new VR(0x9999, 0x0402, len, TRUE);
	  FILE *f;
	  f = fopen(buf, "rb");
	  fread((char*)(vr->Data), 1, len, f);
          fclose(f); 
          DCO.Push(vr);
          if (len&1)
          { UINT16 oddlength = 1;	
            VR *vr4 = new VR (0x9999, 0x0404, 2, &oddlength, FALSE);
            DCO.Push(vr4); // requires change in dimsec.cpp
          }
	}
        
        if (buf && !L) *buf=0;
	PDU.Write(&DCO, uid);

	if(!PDU.Read(&DCOR))
		return ( 1 );	// associate lost
        
        int oddlength = DCOR.GetUINT16(0x9999,0x0404)?1:0;

	while((vr = DCOR.Pop()))
		{
		if (vr->Group == 0x9999 && vr->Element == 0x0401)
			{
			if (vr->Length < 100)
				rc = atoi((char *)vr->Data);

			if (L)
				{
				int len=vr->Length;
                                if (oddlength) len--;
                                if (buf==NULL)
				{ if (len>1 && ((char *)(vr->Data))[len-1]==' ') len--;
				  else if (len>1 && ((char *)(vr->Data))[len-1]==0) len--;
                                }
                                lua_pushlstring(L, (char *)(vr->Data), len);
				rc = -1;
				}
			else if (buf)
				{
				int len;
				if (html) len = processhtml(buf, (char *)vr->Data, vr->Length);
                                else      memcpy(buf, vr->Data, len = vr->Length);
				buf[len]=0;
				if (len>1 && buf[len-1]==' ') len--;
				rc = -1;
				}
			else
				{
				if (con==0)
					{
					con = fileno(stdout);
#ifdef WIN32
					setmode(con, O_BINARY);
#endif
					}

				if (html)
					{ 
					char *buf1 = (char *)malloc(vr->Length*3);  // worst case; every char replaced by %CC
					int len = processhtml(buf1, (char *)vr->Data, vr->Length);
					if (len>1 && buf1[len-1]==0 && (len&1)==0) len--;
					write(con, buf1, len);
					free(buf1);
					rc = -1;
					}
				else
					{ int len = vr->Length;
					  // assume header is EVEN for application/dicom
					//if (len>33)
					  //if (memcmp(vr->Data, "Content-type: application/dicom\n", 32)==0)
					    //len--;
                                        if (oddlength) len--;
					write(con, vr->Data, len);
					rc = -1;
					}
				}
			}
		delete vr;
		}

	PDU.Close();
	return ( rc );
	}

  static int luaservercommand(lua_State *L)
  { int n=lua_gettop(L);
    lua_State *L2=NULL;
    char *b = NULL; // buffer to SendServerCommand (only used to pass filename for upload)
    char *t = NULL; // mode
    int c   = 0;     // filehandle or console
    int rc = 1;
    BOOL html=FALSE; // convert to html
    BOOL upload=FALSE; // upload
    BOOL download=FALSE; // download
    int console = fileno(stdout);
    char bin[]="binary";

    if (lua_isstring(L,2)) 
    { t = (char *)lua_tostring(L,2);
      if      (strcmp(t, "cgi"      )==0) {c=console; }
      else if (strcmp(t, "cgibinary")==0) {c=console; }
      else if (strcmp(t, "cgihtml"  )==0) {c=console; html=TRUE;}
      else if (t[0]=='<') {b=t+1; html=FALSE; upload=TRUE; L2=L;}
      else if (t[0]=='>') {c=open(t+1, O_CREAT | O_TRUNC | O_BINARY | O_RDWR, 0666); html=FALSE; download=FALSE;}
      else if (strcmp(t, "binary"   )==0) {b=bin; L2=L;}
      else { L2=L; }
    }
    else
      L2=L;
    if (lua_isstring(L,1)) 
      rc = SendServerCommand("", lua_tostring(L,1), c, b, html, upload, L2);
    if (download) close(c);
    if (L2 && rc==-1) return 1;
    return 0;
  }

lua_State *L2=NULL;
  
const char *do_lua(lua_State **L, char *cmd)
{ if (!*L) 
  {
#ifdef LUA_5_2
    *L = luaL_newstate();
#else
    *L = lua_open();
#endif
    luaL_openlibs (*L);

    lua_register      (*L, "servercommand", luaservercommand);
  }

  if (cmd[0]) 
  { if (luaL_loadstring(*L, cmd)) 
    { printf("*** lua syntax error %s in '%s'\n", lua_tostring(*L, -1), cmd);
      lua_pop(*L, 1);
      return NULL;
    } 
    else 
    { 
#ifdef LUA_5_2
      if (lua_pcallk(*L, 0, 1, 0, 0, NULL)) 
#else        
      if (lua_pcall(*L, 0, 1, 0))
#endif
      { printf("*** lua run error %s in '%s'\n", lua_tostring(*L, -1), cmd);
        lua_pop(*L, 1);
        return NULL;
      }
      else
      { if (lua_isstring(*L, -1))
          return lua_tostring(*L, -1);
        else
	  lua_pop(*L, 1);
      }
    }
  }
  return NULL;  
}

//__declspec(dllexport) int   luaopen_servertask(lua_State *L)
//{	lua_register(L,"servercommand",luaservercommand);
	//return 1;
//}

int main(int argc, char **argv)
{
#ifdef WIN32
 setmode(fileno(stdout), O_BINARY);
#endif
  if (argc<2)
  { printf("Conquest server control client: -? provides help\n");
    exit(0);
  }
  for (int i=1; i<argc; i++)
  { if (argv[i][0]=='-')
    { if (argv[i][1]=='$'); // ignored
      else if (argv[i][1]=='w'); // ignored
      else if (argv[i][1]=='?') 
      { printf("Conquest server control client: runs lua chunk, has servercommand function\n");
        printf("  Usage: servertask -hAE -pPort -qIP --dolua:chunk\n");
        printf("         servertask -hAE -pPort -qIP chunk\n");
        printf("         servertask chunk (with default parameters)\n");
        printf("  E.g.   servertask \"servercommand([[lua:print('hi')]])\"\n");
        printf("  dgate -- commands executed by server also allowed. e.g. servertask --echo:AE\n");
      }
      else if (argv[i][1]=='h') strcpy(MYACRNEMA, argv[i]+2);
      else if (argv[i][1]=='p') strcpy(Port, argv[i]+2);
      else if (argv[i][1]=='q') strcpy(ServerCommandAddress, argv[i]+2);
      else if (argv[i][1]=='-')
      { if (argv[i][2]=='d' && strlen(argv[i])>8 && memcmp(argv[i], "--dolua:", 8)==0) 
	  do_lua(&L2, argv[i]+8);
        else
	{ int rc = SendServerCommand("Server command sent using DGATE -- option", argv[i]+2, 0, NULL, FALSE, FALSE, NULL);
	  exit(rc);
	}
          // printf("unrecognised argument: %s\n", argv[i]);
      }
      else
       printf("unrecognised argument: %s\n", argv[i]);
    }
    else
      do_lua(&L2, argv[i]);
  }
}

#endif
