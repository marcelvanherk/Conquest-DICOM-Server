// Small echo DLL used to control the dicom server from the delphi GUI
// mvh 20181215; based on code by ljz and lsp
// mvh 20200110; Allow change of transfer syntax

#include "dicom.hpp"

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

