/* MvH 19980322: played around; added comments in query client */
/* MvH 19980322: Added command line interface for c-find       */
/* MvH 19980323: Added command line interface for c-move       */
/* MvH 20000318: Added optional NKI command vr's to echoclient   */
/* MvH 20020315: Started on -fe: forwarding server; implemented echo, working on move and find */
/* MvH 20020316: Added cstoreclient -t; load dd.txt as dicom dictionary; -fd=diagnostoc mode (logs) */
/*               Added IARQ; note: cast of PDU to dump() seems to crash sometimes */
/*               Forwarder works between QUIRT/conquest apps and conquest dicom server */
/* MvH 20020317: Made forwarder more general; works between EFilm and Conquest */
/* MvH 20020811: Fixed forwarder such that it works for dicom-print
/*               Fix: Forwarder did not worked with OFFIS because OFFIS checks message 
                 command/data pdv bit 6[1](?) that is was not set correctly by forwarder (command/data object)
   MvH 20020822: Made this sample program part of code base
   ljz 20030630: Fixed crash in cast of Dump(PDU) (c.f. 20020316)
		 Fixed passing of cmds to Find and Move
		 Fixed continuous CEchoServer
		 Removed calls to obsolete PDU.Write()
   ljz 20030721: Fixed SetStringVR (pad uneven strings with a space)
		 C-Find and C-Move wildcards "*" seem not to be valid
   mvh 20040711: Fixed forwarder: status=0xff00 loops responses; status==0 keeps association open
   mvh 20040712: Added auto-returner to forwarder - functions with NKI viewers (assumes viewerAE of X%ip or XX%ip).
		 You need to set AE and port of return forwarder in your PACS though
		 see gatewayname, gatewayport, returndump and viewerport for configuration
   mvh 20040713: Attempt to make it multithreaded; however return connection only starts once (supports single client only!)
   mvh 20040713: Split into modes -fp: for printer; -fb: forwards and returns; -fe normal forward; -ff fixing forwards
   mvh 20051217: Use new ReAlloc method to replace data in VR
   mvh 20060618: Allow (and disabled) looping of c-move to test repeated c-move problem in read-ahead thread
   mvh 20061127: Allow (and disabled) looping alternating c-move and c-find to test server
   mvh 20070902: Fixed for ms7 and ms8
   mvh 20090411: mode -fp adds more transfer syntaxes uid2..uid8 (experimental)
*/

/* Usage summary:

   test -e HOST PORT NKI1 NKI2								echo client
   test -f -l:CALLER -r:CALLED host port QUERYLEVEL PATID STUDYDATE SERIESNUMBER	find client
   test -m -l:CALLER(dest) -r:CALLED host port QUERYLEVEL PATID STUDYDATE SERIESNUMBER	move client

   test -se PORT								echo server
   test -ss PORT								storage server
   test -sc PORT								storage/echo server

   test -fe PORT host port loglevel						forward PORT to host:port
	note: loglevel 	0(default)=log nothing, 
			1=log request commands
			2=+log request data and response command
			3=+log all except image data 
			4=+log all including image data
	loglevel2 is used for logging in auto-return thread

   test -fb PORT host port loglevel loglevel2					forward PORT to host:port and return GATEWAY:1233 to V%ip:1234

   test -fp PORT host port loglevel 						as -fe mode but for printer tests

   test -ff PORT host port loglevel						as -fe mode but fixis Fhilips and Kodak patient ID
										10 digit: strips 2 or 3 leading zeros, 8 digits: strips 1 leading zero

   test -t file host port							store file on host:port

   test -v file									dump file
*/

#	include	"dicom.hpp"

#	define	CECHO_SERVER	1
#	define	CECHO_CLIENT	2
#	define	CFIND_CLIENT	3
#	define	CSTORE_SERVER	4
#	define	CMOVE_CLIENT	5
#	define	CECHOSTORE_SERVER	6
#	define	CDUMPFILE		7

#ifdef	MAC
#	undef	stdout
#	define	stdout	stderr
#endif

	int		ForkAProc;
	BOOL	(*ForkProc)(DICOMDataObject *);

RTC		VRType(TRUE);

BYTE	LOCAL_ACR[20]	=	"none";
BYTE	REMOTE_ACR[20]	=	"none";
BYTE	PORT[20]	=		"10004";
BYTE	Directory[128]	=	".\\";
BYTE	PNAME[64]		=	"\0";

void
IARQ (AAssociateRQ	&ARQ)
	{
	char	s[128];

	printf("A-ASSOCIATE-RQ Packet Dump\n");
	memset(s, 0, 64);
	memcpy(s, ARQ.CallingApTitle, 16);
	printf("\tCalling Application Title : \"%s\"\n", s);
	memset(s, 0, 64);
	memcpy(s, ARQ.CalledApTitle, 16);
	printf("\tCalled Application Title : \"%s\"\n", s);
	memset(s, 0, 64);
	memcpy(s, ARQ.AppContext.ApplicationContextName.GetBuffer(1),
		ARQ.AppContext.ApplicationContextName.GetSize() );
	printf("\tApplication Context : \"%s\"\n", s);
	printf("\tNumber of Proposed Presentation Contexts: %d\n",
		ARQ.PresContexts.GetSize() );

	int	Index = 0;
	while (Index < ARQ.PresContexts.GetSize() )
		{
		PresentationContext	PC;

		PC = ARQ.PresContexts.Get(Index);
		PC.TrnSyntax.ClearType = FALSE;

		memset(s, 0, 64);
		memcpy(s,
		PC.AbsSyntax.AbstractSyntaxName.GetBuffer(1),
			PC.AbsSyntax.AbstractSyntaxName.GetSize()%64 );

		printf("\tPresentation Context %d \"%s\"\n", Index, s);
		++Index;
		}
	}

void
Dump (
	AAssociateRQ	&ARQ)
	{
	char	s[128];

	printf("A-ASSOCIATE-RQ Packet Dump\n\n");
	memset(s, 0, 64);
	memcpy(s, ARQ.CallingApTitle, 16);
	printf("\tCalling Application Title : \"%s\"\n", s);
	memset(s, 0, 64);
	memcpy(s, ARQ.CalledApTitle, 16);
	printf("\tCalled Application Title : \"%s\"\n", s);
	memset(s, 0, 64);
	memcpy(s, ARQ.AppContext.ApplicationContextName.GetBuffer(1),
		ARQ.AppContext.ApplicationContextName.GetSize() );
	printf("\tApplication Context : \"%s\"\n", s);
	printf("\tNumber of Proposed Presentation Contexts: %d\n",
		ARQ.PresContexts.GetSize() );

	int	Index = 0;
	int	Index2 = 0;
	while (Index < ARQ.PresContexts.GetSize() )
		{
		PresentationContext	PC;

		PC = ARQ.PresContexts.Get(Index);
		PC.TrnSyntax.ClearType = FALSE;

		memset(s, 0, 64);
		memcpy(s,
		PC.AbsSyntax.AbstractSyntaxName.GetBuffer(1),
			PC.AbsSyntax.AbstractSyntaxName.GetSize()%64 );

		printf("\tPresentation Context %d \"%s\"\n", Index, s);
		Index2 = 0;
		while ( Index2 < PC.TrnSyntax.GetSize() )
			{
			memset(s, 0, 64);
			memcpy(s,
			PC.TrnSyntax.Get(Index2).TransferSyntaxName.GetBuffer(1),
				PC.TrnSyntax.Get(Index2).TransferSyntaxName.GetSize()%64 );

			printf("\t\tTransfer Syntax: \"%s\"\n",s);
			++Index2;
			}
		
		++Index;
		}
	}

void
Dump (
	AAssociateAC	&AAC)
	{
	char	s[128];

	printf("A-ASSOCIATE-AC Packet Dump\n\n");
	memset(s, 0, 64);
	memcpy(s, AAC.CallingApTitle, 16);
	printf("\tCalling Application Title : \"%s\"\n", s);
	memset(s, 0, 64);
	memcpy(s, AAC.CalledApTitle, 16);
	printf("\tCalled Application Title : \"%s\"\n", s);
	memset(s, 0, 64);
	memcpy(s, AAC.AppContext.ApplicationContextName.GetBuffer(1),
		AAC.AppContext.ApplicationContextName.GetSize() );
	printf("\tApplication Context : \"%s\"\n", s);
	printf("\tNumber of Accepted Presentation Contexts: %d\n",
		AAC.PresContextAccepts.GetSize() );

	int	Index = 0;
	while (Index < AAC.PresContextAccepts.GetSize() )
		{
		PresentationContextAccept	PC;
		PC = AAC.PresContextAccepts.Get(Index);

		memset(s, 0, 64);
		memcpy(s,
		PC.TrnSyntax.TransferSyntaxName.GetBuffer(1),
			PC.TrnSyntax.TransferSyntaxName.GetSize()%64 );

		printf("\t[%d] Presentation Context ID=%d\n\t\tTransfer Syntax: \"%s\"\n",
			Index, PC.PresentationContextID, s);
		++Index;
		}
	}

BOOL	IsAlpha(BYTE	c)
	{
	if((c >= ' ')&&(c <= 125))
		return ( TRUE );
	if(!c)
		return ( TRUE );
	return ( FALSE );
	}
			
BOOL	IsDataAlpha(UINT	Size, BYTE	*data)
	{
	if(Size > 16)
		Size = 16;
	if(!Size)
		return ( FALSE );
	--Size;
	while(Size)
		{
		if(!IsAlpha(data[Size]))
			return ( FALSE );
		--Size;
		}
	return ( TRUE );
	}
	
BOOL	PrintUDATA(UINT	Size, void	*data, UINT16	TypeCode)
	{
	UINT	Index = 0;
	char	*c;
	UINT16	tint16;
	UINT32	tint32;
	BYTE	*b;
	LE_UINT16	*w;
	char	Str[70];
	LE_UINT32	*l32 = (LE_UINT32*)data;
	LE_UINT16	*l16 = (LE_UINT16*)data;
	LE_INT32	*s32 = (LE_INT32*)data;
	LE_INT16	*s16 = (LE_INT16*)data;

	if(!data)
		{
		fprintf(stdout, "(null)");
		return ( TRUE );
		}
	if ( ! TypeCode )
		{
		switch ( Size )
			{
			case	2:
				tint16 = (*((UINT16 *) data));
				fprintf(stdout, "(%d|%x)", (UINT) tint16, (UINT) tint16);
				break;
			case	4:
				tint32 = (*((UINT32 *) data));
				fprintf(stdout, "[%d|%x]", (UINT) tint32, (UINT) tint32);
				break;
			default:
				if(IsDataAlpha(Size, (BYTE*) data))
					{
					if(Size > 64)
						Size = 64;
					strncpy(Str, (char*) data, Size);
					Str[Size] = '\0';
					fprintf(stdout, "\"%s\"", Str);
					break;
					}
				Index = 0;
				while(Index < Size)
					{
					if(Index >= 16)
						break;
					c = (char*)data;
					c += Index;
					fprintf(stdout, "%2.2x(%c)", *c, *c);
					++Index;
					}
			}
		}
	else
		{
		switch ( TypeCode )
			{
			case	'AE':
			case	'AS':
			case	'CS':
			case	'DA':
			case	'DS':
			case	'DT':
			case	'IS':
			case	'LO':
			case	'LT':
			case	'PN':
			case	'SH':
			case	'ST':
			case	'TM':
			case	'UI':
				if(Size > 64)
					Size = 64;
				strncpy(Str, (char*) data, Size);
				Str[Size] = '\0';
				fprintf(stdout, "\"%s\"", Str);
				break;
					
			case	'FL':
			case	'FD':
				break;				
			case	'AT':
				Index = 0;
				while ( Index < Size )
					{
					UINT16	Grp, Elm;
					if(Index > 10)
						break;
					
					Grp = (*l16);++l16;
					Elm = (*l16);++l16;
					fprintf(stdout, "(%4.4x, %4.4x) ", Grp, Elm);
					Index += 4;
					}
				break;
			case	'OB':
				b = (BYTE*)data;
				while ( Index < Size )
					{
					if ( Index > 10 )
						break;
					fprintf(stdout, "%2.2x ", b);
					++b;
					++Index;
					}
				break;
			case	'OW':
				w = (LE_UINT16*)data;
				
				while(Index < Size )
					{
					if ( Index > 20 )
						break;
					fprintf(stdout, "%4.4x ", (UINT16)(*w));
					++w;
					++Index;
					++Index;
					}
				break;
					
			case	'SQ':
				fprintf(stdout, "Sequence Item\n");
				break;
			case	'UL':
				
				while ( Index < Size )
					{
					if ( Index > 10 )
						break;
					
					fprintf(stdout, "%ld ", (UINT32)(*l32));
					++l32;
					Index += 4;
					}
				break;
			case	'US':
				
				while ( Index < Size )
					{
					if ( Index > 20 )
						break;
					
					fprintf(stdout, "%d ", (UINT32)(*l16));
					++l16;
					Index += 2;
					}
				break;
			case	'SL':
				
				while ( Index < Size )
					{
					if ( Index > 40 )
						break;
					
					fprintf(stdout, "%ld ", (UINT32)(*s32));
					++s32;
					Index += 4;
					}
				break;
			case	'SS':
				
				while ( Index < Size )
					{
					if ( Index > 20 )
						break;
						
					fprintf(stdout, "%d ", (UINT32)(*s16));
					++s16;
					Index += 2;
					}
				break;
		
			}
		}
	return ( TRUE );
	}
	
	
BOOL	DumpVR(VR	*vr)
	{
	INT		Index;
	char	*c;
	UINT16	TypeCode;
	char	s[256];

	s[0] = '\0';
	TypeCode = 0;
	TypeCode = VRType.RunTimeClass(vr->Group, vr->Element, s);
	if(TypeCode)
		fprintf(stdout, "(%4.4x, %4.4x, %02d, %c%c, %s) ",
			vr->Group, vr->Element, vr->Length,
			TypeCode>>8, TypeCode&0x00ff, s);
	else
		fprintf(stdout, "(%4.4x, %4.4x, %02d, Type Unknown) ",
			vr->Group, vr->Element, vr->Length);
	PrintUDATA ( vr->Length, vr->Data, TypeCode );
	fprintf(stdout, "\n");
	return(TRUE);
	}

BOOL
DumpDDO(
	DICOMDataObject	*DDO,
	UINT			TabCount)
	{
	VR			*vr;
	while (vr = DDO->Pop())
		{
		UINT	Index = 0;
		while ( Index < TabCount)
			{
			fprintf(stdout, "\t");
			++Index;
			}
		DumpVR(vr);
		if ( vr->SQObjectArray )
			{
			Array < DICOMDataObject * > 	*ADDO =
				(Array<DICOMDataObject*>*) vr->SQObjectArray;
			
			Index = 0;
			while ( Index < ADDO->GetSize() )
				{
				fprintf(stdout, "> Start of Sequence\n");
				DumpDDO(ADDO->Get(Index), TabCount+1);
				fprintf(stdout, "> End of Sequence\n");
				++Index;
				}
			}
		delete vr;
		}
	return ( TRUE );
	}
	
// Print a composite dicom object
BOOL
NonDestructiveDumpDICOMObject(
	DICOMObject	*DO)
	{
	DICOMObject	DO2;
	VR		*vr;

	while(vr=DO->Pop())
		{
		DumpVR(vr);
		DO2.Push(vr);
		}
	DO->Reset();
	while(vr=DO2.Pop())
		{
		DO->Push(vr);
		}
	return ( TRUE );
	}

BOOL
DumpDDOFile(char	*filename)
	{
	PDU_Service		PDU;
	DICOMDataObject	*DDO;
	VR				*vr;

	PDU.AttachRTC(&VRType);
	DDO = PDU.LoadDICOMDataObject(filename);
	if ( ! DDO )
		{
		fprintf(stderr, "Error loading filename: %s\n", filename);
		return ( FALSE );
		}
	fprintf(stdout, "[-------DICOM Object: %s--------]\n", filename);
	DumpDDO(DDO, 0);
	delete DDO;
	return ( TRUE );
	}

BOOL
CEchoServer(void)
	{
	PDU_Service			PDU;
	DICOMCommandObject	DCO;
	DICOMCommandObject	DCOR;
	VR					*vr;
	LE_UINT16				tuint16;
	UID			uid;

	if(PDU.Listen(PORT))
		{
		PDU.Read(&DCO);

		while(vr = DCO.Pop())
			{
			fprintf(stderr, "Object: (%4.4x, %4.4x), %d Length\n",
				vr->Group, vr->Element, vr->Length);

			switch(vr->Element)
				{
				case	0x0000:	delete vr; break;
				case	0x0002:	DCOR.Push(vr);	break;
				case	0x0100:
					tuint16 = 0x8030;	// 0x8030
					memcpy(vr->Data, &tuint16, 2);
					DCOR.Push(vr);	break;
				case	0x0800:	DCOR.Push(vr);	break;
				case	0x0110:
					vr->Element = 0x0120;
					DCOR.Push(vr);	break;
				default:
					delete vr;
				}
			}
		vr = new VR(0x0000, 0x0900, 0x0002, TRUE);
		tuint16 = 0;
		memcpy(vr->Data, &tuint16, 2);
		DCOR.Push(vr);

		uid.Set("1.2.840.10008.1.1");
		PDU.Write(&DCOR, uid);
		PDU.Close();
		return ( TRUE );
		}
	// The sender should intiate the close at this point, but send
	// one anyway (wont hurt).
	return ( FALSE );
	}

BOOL
CEchoClient(BYTE	*ip, BYTE	*port, char *NKIcommand1, char *NKIcommand2)
	{
	PDU_Service			PDU;
	DICOMCommandObject	DCO;
	DICOMCommandObject	DCOR;
	UID					uid;
	VR					*vr;
	LE_UINT16				tuint16, command, datasettype, messageid;
	BYTE				SOP[64];

	PDU.ClearAbstractSyntaxs();
	PDU.SetLocalAddress(LOCAL_ACR);
	PDU.SetRemoteAddress(REMOTE_ACR);
	uid.Set("1.2.840.10008.3.1.1.1");	// Dicom APP
	PDU.SetApplicationContext(uid);
	uid.Set("1.2.840.10008.1.1");		// Verification
	PDU.AddAbstractSyntax(uid);

	if(!PDU.Connect(ip, port))
		{
		Dump((AAssociateRQ&)PDU);
		// Some error.  Print out AAssociateRJ data

		fprintf(stderr, "AAssociateRJ: %d, %d, %d\n",
			PDU.AAssociateRJ :: Result,
			PDU.AAssociateRJ :: Source,
			PDU.AAssociateRJ :: Reason);
		return ( FALSE );
		}
	Dump((AAssociateRQ&)PDU);
	Dump((AAssociateAC&)PDU);
	
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

	messageid = 1;
	vr = new VR (0x0000, 0x0110, 0x0002, &messageid, FALSE);
	DCO.Push(vr);
	fprintf(stderr, "Sending CEcho-RQ %d\n", PDU.Write(&DCO, uid));
	fprintf(stderr, "Waiting for CEcho-RSP\n");

	if(!PDU.Read(&DCOR))
		return ( FALSE );	// associate lost

	while(vr = DCOR.Pop())
		{
		fprintf(stderr, "Object: (%4.4x, %4.4x), %d Length\n",
			vr->Group, vr->Element, vr->Length);
		fprintf(stderr, "%8.8x   ", vr);
		DumpVR ( vr );
		delete vr;
		}
	PDU.Close();
	return ( TRUE );
	}

void SetStringVR(VR **vr, int g, int e, char *String)
{ char*	pCopy;
  int	iLength = strlen(String);

  if (iLength & 0x01)
  { /* Pad the string with a space */
    pCopy = strdup(String);
    pCopy[iLength] = ' ';
    iLength++;
    *vr = new VR(g, e, iLength, (void*)pCopy, (BOOL) TRUE);
  }
  else
    *vr = new VR(g, e, iLength, (void*)String, (BOOL) FALSE); 
}

BOOL
CFindClient(BYTE	*ip, BYTE	*port, int argc, char *argv[])
	{
	BOOL				quit;
	PDU_Service			PDU;
	DICOMCommandObject	DCO;
	DICOMCommandObject	DCOR;
	DICOMDataObject		DDO;
	DICOMDataObject		DDOR;
	UID					uid;
	VR					*vr;
	LE_UINT16				tuint16, command, datasettype, messageid, priority;
	BYTE				SOP[64];
	BYTE				QLevel[16];
	BYTE				AppTitle[64];
	BYTE				Star[32];
		
// Start setting up connection

	PDU.ClearAbstractSyntaxs();
	PDU.SetLocalAddress(LOCAL_ACR);
	PDU.SetRemoteAddress(REMOTE_ACR);

	uid.Set("1.2.840.10008.3.1.1.1");	// Application context (DICOM App)
	PDU.SetApplicationContext(uid);

// MvH: Proposed Presentation Contexts (ask if the server can handle these) */

//	uid.Set("1.2.840.10008.5.1.4.1.2.1.1");	// PatientRootQuery
//	PDU.AddAbstractSyntax(uid);

	uid.Set("1.2.840.10008.5.1.4.1.2.2.1"); // StudyRootQuery (C-Find SOP)
	PDU.AddAbstractSyntax(uid);

//	uid.Set("1.2.840.10008.5.1.4.1.2.3.1");	// PatientStudyOnlyQuery
//	PDU.AddAbstractSyntax(uid);


// Make the association on port/ip

	if(!PDU.Connect(ip, port))
		{ // Some error.  Print out AAssociateRJ data
		fprintf(stderr, "AAssociateRJ: %d, %d, %d\n",
			PDU.AAssociateRJ :: Result,
			PDU.AAssociateRJ :: Source,
			PDU.AAssociateRJ :: Reason);
		return ( FALSE );
		}


// Start a StudyRootQuery

	strcpy((char*) SOP, "1.2.840.10008.5.1.4.1.2.2.1"); // StudyRootQuery (C-Find SOP)
//	strcpy((char*) SOP, "1.2.840.10008.5.1.4.1.2.1.1"); // PatientRootQuery (C-Find SOP)
	vr = new VR (0x0000, 0x0002, strlen((char*)SOP), (void*) SOP, FALSE);
	DCO.Push(vr);
	command = 0x0020;
	vr = new VR (0x0000, 0x0100, 0x0002, &command, FALSE);
	DCO.Push(vr);
	priority = 0;	// MEDIUM
	vr = new VR (0x0000, 0x0700, 0x0002, &priority, FALSE);
	DCO.Push(vr);
	datasettype = 0x0102;	
	vr = new VR (0x0000, 0x0800, 0x0002, &datasettype, FALSE);
	DCO.Push(vr);
	messageid = 3;
	vr = new VR (0x0000, 0x0110, 0x0002, &messageid, FALSE);
	DCO.Push(vr);


// Start of data object. Query level may come from command line

	if (argc <= 4)
	{ SetStringVR(&vr, 0x0008, 0x0052, "STUDY "); 
	  DDO.Push(vr); 
	}
	else
	{ SetStringVR(&vr, 0x0008, 0x0052, argv[4]); 
	  DDO.Push(vr); 
	}

// AE title is not necessary
//	strcpy((char*) AppTitle, "UCD_MRS1_IC0    ");
//	vr = new VR(0x0008, 0x0054, 16, (void*)AppTitle, FALSE);		// Retreive AE title
//	DDO.Push(vr);
	
// Transmit type and data to query on; ask what you need; you will always get the primary key
// for the requested level. However, you need to ask at least one item for the requested level !

	vr = new VR(0x0020, 0x0013, 0, (BOOL) FALSE);	DDO.Push(vr); // imageno
	vr = new VR(0x0008, 0x0018, 0, (BOOL) FALSE);	DDO.Push(vr); // studyinstuid
	vr = new VR(0x0010, 0x0010, 0, (BOOL) FALSE);	DDO.Push(vr); // patientname
	vr = new VR(0x0008, 0x0060, 0, (BOOL) FALSE);	DDO.Push(vr); // modality

// Get patient ID from command line
	if (argc <= 5)
	{ SetStringVR(&vr, 0x0010, 0x0020, ""); 
	  DDO.Push(vr); 
	}
	else
	{ SetStringVR(&vr, 0x0010, 0x0020, argv[5]); 
	  DDO.Push(vr); 
	}

// Get study date from command line
	if (argc <= 6)
	{ SetStringVR(&vr, 0x0008, 0x0020, ""); 
	  DDO.Push(vr); 
	}
	else
	{ SetStringVR(&vr, 0x0008, 0x0020, argv[6]); 
	  DDO.Push(vr); 
	}

// Get series number from command line
	if (argc <= 7)
	{ SetStringVR(&vr, 0x0020, 0x0011, ""); 
	  DDO.Push(vr); 
	}
	else
	{ SetStringVR(&vr, 0x0020, 0x0011, argv[7]); 
	  DDO.Push(vr); 
	}

// Write command object and data object
	uid.Set(SOP);
	PDU.Write(&DCO, uid);
	PDU.Write(&DDO, uid);

// Get response

	quit = 0;
	while(!quit)
		{
		
//		fprintf(stderr, "Waiting for CFind-RSP\n");
		if(!PDU.Read(&DCOR))
			return ( FALSE );	// associate lost

		// is this a C-Find-RSP ?
		if(DCOR.GetUINT16(0x0000, 0x0100)!=0x8020)
			{
//			fprintf(stderr, "not a C-Find-RSP: %4.4x\n",
//				DCOR.GetUINT16(0x0000, 0x0100));
			PDU.Close();
			return(FALSE);
			}

		// is there a data set?
		if(DCOR.GetUINT16(0x0000, 0x0800)==0x0101)
			{
//			fprintf(stderr, "no data set included\n");
//			fprintf(stderr, "status: %4.4x\n",
//				DCOR.GetUINT16(0x0000, 0x0900));
//			while(vr = DCOR.Pop())
//				{
//				DumpVR(vr);
//				}
			PDU.Close();
			return( TRUE );
			}

//		fprintf (stderr, "(%4.4x)", DCOR.GetUINT16(0x0000, 0x0900) );
		
		if(DCOR.GetUINT16(0x0000, 0x0900)==0x0000)	// no success ?
			{
			PDU.Close();
			return ( TRUE );
			}
			
//		fprintf(stderr, "Waiting for data section\n");
		if(!PDU.Read(&DDOR))
			{
			PDU.Close();
			return(FALSE);
			}

//		fprintf(stderr, "Command Object:\n");
//		while(vr = DCOR.Pop())
//			{
//			DumpVR(vr);
//			fprintf(stderr, "\tObject: (%4.4x, %4.4x), %d Length\n",
//				vr->Group, vr->Element, vr->Length);
//			}
		// figure out what we got in the data section
//		fprintf(stderr, "Data Object:\n");
		while(vr = DDOR.Pop())
			{
// MvH, dump response in case of error ??
			if(vr->Group == 0x0000)
				{
				fprintf(stderr, "Data Object:\n");
				DumpVR(vr);
				delete vr;
				while(vr = DDOR.Pop())
					{
					DumpVR(vr);
					delete vr;
					}
				fprintf(stderr, "Command Object:\n");
				while(vr = DCOR.Pop())
					{
					DumpVR(vr);
					delete vr;
					}
				PDU.Close();
				return ( TRUE );
				}

// MvH 980322 List everything we recieve except group length
			if (vr->Element != 0x0000) DumpVR(vr);
//			if(vr->Group == 0x0010)
//			{	 if(vr->Element == 0x0010)
//					DumpVR(vr);
//			}

			delete vr;
			}
		while(vr = DCOR.Pop())
			delete vr;

		}

	PDU.Close();
	return ( TRUE );
	}


// MvH, 19980324: C-Move is almost a copy of C-Find; also uses same query mechanism

BOOL
CMoveClient(BYTE	*ip, BYTE	*port, int argc, char *argv[])
	{
	BOOL				quit;
	PDU_Service			PDU;
	DICOMCommandObject	DCO;
	DICOMCommandObject	DCOR;
	DICOMDataObject		DDO;
	DICOMDataObject		DDOR;
	UID					uid;
	VR					*vr;
	LE_UINT16				tuint16, command, datasettype, messageid, priority;
	BYTE				SOP[64];
	BYTE				QLevel[16];
	BYTE				AppTitle[64];
	BYTE				Star[32];
		
// Start setting up connection

	PDU.ClearAbstractSyntaxs();
	PDU.SetLocalAddress(LOCAL_ACR);
	PDU.SetRemoteAddress(REMOTE_ACR);

	uid.Set("1.2.840.10008.3.1.1.1");	// Application context (DICOM App)
	PDU.SetApplicationContext(uid);

// MvH: Proposed Presentation Contexts (ask if the server can handle these) */

	uid.Set("1.2.840.10008.5.1.4.1.2.2.2"); // StudyRootRetrieve (C-Move SOP)
	PDU.AddAbstractSyntax(uid);

///////////////////
	uid.Set("1.2.840.10008.5.1.4.1.2.2.1"); // StudyRootQuery (C-Find SOP)
	PDU.AddAbstractSyntax(uid);

// Make the association on port/ip

	if(!PDU.Connect(ip, port))
		{ // Some error.  Print out AAssociateRJ data
		fprintf(stderr, "AAssociateRJ: %d, %d, %d\n",
			PDU.AAssociateRJ :: Result,
			PDU.AAssociateRJ :: Source,
			PDU.AAssociateRJ :: Reason);
		return ( FALSE );
		}


// Start a StudyRootRetrieve (N times)

for(int i=0; i<1; i++)
{
DCO.Reset();
DDO.Reset();


      strcpy((char*) SOP, "1.2.840.10008.5.1.4.1.2.2.2"); // (C-Move SOP)
	vr = new VR (0x0000, 0x0002, strlen((char*)SOP), (void*) SOP, FALSE);
	DCO.Push(vr);
	command = 0x0021;
	vr = new VR (0x0000, 0x0100, 0x0002, &command, FALSE);
	DCO.Push(vr);
	priority = 0;	// MEDIUM
	vr = new VR (0x0000, 0x0700, 0x0002, &priority, FALSE);
	DCO.Push(vr);
	datasettype = 0x0102;	
	vr = new VR (0x0000, 0x0800, 0x0002, &datasettype, FALSE);
	DCO.Push(vr);
	messageid = 5;
	vr = new VR (0x0000, 0x0110, 0x0002, &messageid, FALSE);
	DCO.Push(vr);

// this is the destination modality (usually local) (-l command line)

	memset((char*)AppTitle, ' ', 20);
	memcpy((void*)AppTitle, LOCAL_ACR, strlen((char *)LOCAL_ACR));
	vr = new VR(0x0000, 0x0600, 16, (void*)&AppTitle[0], FALSE);
	DCO.Push(vr);

// Start of data object. Query level may come from command line

	if (argc <= 4)
	{ SetStringVR(&vr, 0x0008, 0x0052, "STUDY"); 
	  DDO.Push(vr); 
	}
	else
	{ SetStringVR(&vr, 0x0008, 0x0052, argv[4]); 
	  DDO.Push(vr); 
	}

// AE title is not necessary
//	strcpy((char*) AppTitle, "UCD_MRS1_IC0    ");
//	vr = new VR(0x0008, 0x0054, 16, (void*)AppTitle, FALSE);		// Retreive AE title
//	DDO.Push(vr);
	
// Transmit type and data to query on; ask what you need; you will always get the primary key
// for the requested level. However, you need to ask at least one item for the requested level !

	vr = new VR(0x0020, 0x0013, 0, (BOOL) FALSE);	DDO.Push(vr); // imageno
	vr = new VR(0x0008, 0x0018, 0, (BOOL) FALSE);	DDO.Push(vr); // studyinstuid
	vr = new VR(0x0010, 0x0010, 0, (BOOL) FALSE);	DDO.Push(vr); // patientname
	vr = new VR(0x0008, 0x0060, 0, (BOOL) FALSE);	DDO.Push(vr); // modality

// Get patient ID from command line
	if (argc <= 5)
	{ SetStringVR(&vr, 0x0010, 0x0020, ""); 
	  DDO.Push(vr); 
	}
	else
	{ SetStringVR(&vr, 0x0010, 0x0020, argv[5]); 
	  DDO.Push(vr); 
	}

// Get study date from command line
	if (argc <= 6)
	{ SetStringVR(&vr, 0x0008, 0x0020, ""); 
	  DDO.Push(vr); 
	}
	else
	{ SetStringVR(&vr, 0x0008, 0x0020, argv[6]); 
	  DDO.Push(vr); 
	}

// Get series number from command line
	if (argc <= 7)
	{ SetStringVR(&vr, 0x0020, 0x0011, ""); 
	  DDO.Push(vr); 
	}
	else
	{ SetStringVR(&vr, 0x0020, 0x0011, argv[7]); 
	  DDO.Push(vr); 
	}

// Write command object and data object

	PDU.Write(&DCO, uid);
	PDU.Write(&DDO, uid);

// Get response

	quit = 0;
	while(!quit)
		{
		if(!PDU.Read(&DCOR))
			{
			fprintf(stderr, "Remote modality dropped Association\n");
			return ( FALSE );	// associate lost
			}

		if(DCOR.GetUINT16(0x0000, 0x0100)!=0x8021)
			{
			fprintf(stderr, "not a C-Move-RSP: %4.4x\n",
				DCOR.GetUINT16(0x0000, 0x0100));
			PDU.Close();
			return(FALSE);
			}

		// ignore the data set
		if(DCOR.GetUINT16(0x0000, 0x0800)!=0x0101)
			{
			PDU.Read(&DDOR);
			DDOR.Reset();
			}

		if(DCOR.GetUINT16(0x0000, 0x0900)==0x0000)	// success ?
			{
			fprintf(stderr, "C-Move %d Complete\n", i+1);
//	PDU.Close();
//	return ( TRUE );
DCOR.Reset();
goto next;
			}
		else
		if (DCOR.GetUINT16(0x0000, 0x0900)==0xff00)	// continuing no problem
			{
			char	s[256];

			sprintf (s, "C-Move-Rsp: (%d) Images sent/ (%d) Image to go\n",
				DCOR.GetUINT16(0x0000, 0x1021),
				DCOR.GetUINT16(0x0000, 0x1020));

			fprintf (stderr, s);

			DCOR.Reset();
			continue;
			}
		else
			{
			VR	*vr = DCOR.GetVR(0x0000, 0x0902);
			char	s[256];

			if(vr)
				{
				memset((void*)s, 0, 256);
				memcpy((void*)s, vr->Data, vr->Length%250);
				}
			else
				{
				UINT16	RemoteError =	DCOR.GetUINT16(0x0000, 0x0900);
				fprintf (stderr, "Remote DICOM Error code %x (%d)\n", RemoteError, RemoteError);
				}
			PDU.Close();	// all done, some error
			return ( FALSE );
			}
		DCOR.Reset();
		}

next:;

///////////////////// alternate c-move / c-find
if (0)
	{

DCO.Reset();
DDO.Reset();

// Start a StudyRootQuery

	strcpy((char*) SOP, "1.2.840.10008.5.1.4.1.2.2.1"); // StudyRootQuery (C-Find SOP)
//	strcpy((char*) SOP, "1.2.840.10008.5.1.4.1.2.1.1"); // PatientRootQuery (C-Find SOP)
	vr = new VR (0x0000, 0x0002, strlen((char*)SOP), (void*) SOP, FALSE);
	DCO.Push(vr);
	command = 0x0020;
	vr = new VR (0x0000, 0x0100, 0x0002, &command, FALSE);
	DCO.Push(vr);
	priority = 0;	// MEDIUM
	vr = new VR (0x0000, 0x0700, 0x0002, &priority, FALSE);
	DCO.Push(vr);
	datasettype = 0x0102;	
	vr = new VR (0x0000, 0x0800, 0x0002, &datasettype, FALSE);
	DCO.Push(vr);
	messageid = 3;
	vr = new VR (0x0000, 0x0110, 0x0002, &messageid, FALSE);
	DCO.Push(vr);


// Start of data object. Query level may come from command line

	if (argc <= 4)
	{ SetStringVR(&vr, 0x0008, 0x0052, "STUDY "); 
	  DDO.Push(vr); 
	}
	else
	{ SetStringVR(&vr, 0x0008, 0x0052, argv[4]); 
	  DDO.Push(vr); 
	}

// AE title is not necessary
//	strcpy((char*) AppTitle, "UCD_MRS1_IC0    ");
//	vr = new VR(0x0008, 0x0054, 16, (void*)AppTitle, FALSE);		// Retreive AE title
//	DDO.Push(vr);
	
// Transmit type and data to query on; ask what you need; you will always get the primary key
// for the requested level. However, you need to ask at least one item for the requested level !

	vr = new VR(0x0020, 0x0013, 0, (BOOL) FALSE);	DDO.Push(vr); // imageno
	vr = new VR(0x0008, 0x0018, 0, (BOOL) FALSE);	DDO.Push(vr); // studyinstuid
	vr = new VR(0x0010, 0x0010, 0, (BOOL) FALSE);	DDO.Push(vr); // patientname
	vr = new VR(0x0008, 0x0060, 0, (BOOL) FALSE);	DDO.Push(vr); // modality

// Get patient ID from command line
	if (argc <= 5)
	{ SetStringVR(&vr, 0x0010, 0x0020, ""); 
	  DDO.Push(vr); 
	}
	else
	{ SetStringVR(&vr, 0x0010, 0x0020, argv[5]); 
	  DDO.Push(vr); 
	}

// Get study date from command line
	if (argc <= 6)
	{ SetStringVR(&vr, 0x0008, 0x0020, ""); 
	  DDO.Push(vr); 
	}
	else
	{ SetStringVR(&vr, 0x0008, 0x0020, argv[6]); 
	  DDO.Push(vr); 
	}

// Get series number from command line
	if (argc <= 7)
	{ SetStringVR(&vr, 0x0020, 0x0011, ""); 
	  DDO.Push(vr); 
	}
	else
	{ SetStringVR(&vr, 0x0020, 0x0011, argv[7]); 
	  DDO.Push(vr); 
	}

// Write command object and data object
	uid.Set(SOP);
	PDU.Write(&DCO, uid);
	PDU.Write(&DDO, uid);

// Get response

	quit = 0;
	while(!quit)
		{
		
//		fprintf(stderr, "Waiting for CFind-RSP\n");
		if(!PDU.Read(&DCOR))
			return ( FALSE );	// associate lost

		// is this a C-Find-RSP ?
		if(DCOR.GetUINT16(0x0000, 0x0100)!=0x8020)
			{
//			fprintf(stderr, "not a C-Find-RSP: %4.4x\n",
//				DCOR.GetUINT16(0x0000, 0x0100));
			PDU.Close();
			return(FALSE);
			}

		// is there a data set?
		if(DCOR.GetUINT16(0x0000, 0x0800)==0x0101)
			{
//			fprintf(stderr, "no data set included\n");
//			fprintf(stderr, "status: %4.4x\n",
//				DCOR.GetUINT16(0x0000, 0x0900));
//			while(vr = DCOR.Pop())
//				{
//				DumpVR(vr);
//				}
DCOR.Reset();
goto next2;
//			PDU.Close();
//			return( TRUE );
			}

//		fprintf (stderr, "(%4.4x)", DCOR.GetUINT16(0x0000, 0x0900) );
		
		if(DCOR.GetUINT16(0x0000, 0x0900)==0x0000)	// no success ?
			{
//			PDU.Close();
//			return ( TRUE );
DCOR.Reset();
goto next2;
			}
			
//		fprintf(stderr, "Waiting for data section\n");
		if(!PDU.Read(&DDOR))
			{
			PDU.Close();
			return(FALSE);
			}

//		fprintf(stderr, "Command Object:\n");
//		while(vr = DCOR.Pop())
//			{
//			DumpVR(vr);
//			fprintf(stderr, "\tObject: (%4.4x, %4.4x), %d Length\n",
//				vr->Group, vr->Element, vr->Length);
//			}
		// figure out what we got in the data section
//		fprintf(stderr, "Data Object:\n");
		while(vr = DDOR.Pop())
			{
// MvH, dump response in case of error ??
			if(vr->Group == 0x0000)
				{
				fprintf(stderr, "Data Object:\n");
				DumpVR(vr);
				delete vr;
				while(vr = DDOR.Pop())
					{
					DumpVR(vr);
					delete vr;
					}
				fprintf(stderr, "Command Object:\n");
				while(vr = DCOR.Pop())
					{
					DumpVR(vr);
					delete vr;
					}
//				PDU.Close();
//				return ( TRUE );
DCOR.Reset();
goto next2;
				}

// MvH 980322 List everything we recieve except group length
			if (vr->Element != 0x0000) DumpVR(vr);
//			if(vr->Group == 0x0010)
//			{	 if(vr->Element == 0x0010)
//					DumpVR(vr);
//			}

			delete vr;
			}
		while(vr = DCOR.Pop())
			delete vr;

		}
	}
/////////////////////

next2:;



	}


	PDU.Close();
	return ( TRUE );
	}


BOOL
DICOMParseProc(DICOMDataObject	*DDOPtr)
	{
//	fprintf(stderr, "Child running in background\n");


	delete DDOPtr;
#ifdef	WINDOWS	
	ExitThread(1);
#endif
	return (FALSE);	
	}

BOOL
Fork(void	*Data)
	{
#ifdef	WINDOWS	
	DWORD	ThreadID;
	if(!CreateThread(NULL, 0x00008000, (LPTHREAD_START_ROUTINE) DICOMParseProc,
		Data, 0, &ThreadID))
		return ( FALSE );
#else
	DICOMParseProc ( (DICOMDataObject *) Data );
#endif
	return ( TRUE );
	}
		
BOOL
CStoreServer()
	{
	PDU_Service			PDU;
	DICOMCommandObject	DCO;
	DICOMCommandObject	*DCORPtr;
	DICOMDataObject		*DDOPtr;
	VR			*vr;
	LE_UINT16		tuint16;
	LE_UINT16		Command, MId, MRId, DSet, E200, E300, E700,E900;
	UID			uid;

	if(PDU.Listen(PORT))
		{
		while(TRUE)
			{
			if(!PDU.Read(&DCO))
				return ( FALSE );
			if(DCO.GetUINT16(0x0000, 0x0100)!=0x0001)
				return ( FALSE );	// not a CStore
				DDOPtr = new DICOMDataObject;
			if(!DDOPtr)
				return(FALSE);
			
			if(!PDU.Read(DDOPtr))
				return ( FALSE );
			if(ForkAProc)
				if(!Fork((void*) DDOPtr))
					{
					fprintf(stderr, "Unable to create thread\n");
					delete DDOPtr;
					}
			DCORPtr = new DICOMCommandObject;
			if(!DCORPtr)
				return ( FALSE );
		
			Command = 0x8001;
			MId = DCO.GetUINT16(0x0000, 0x0110);
			MRId = MId;
			DSet = 0x0101;
			E200 = DCO.GetUINT16(0x0000, 0x0200);
			E300 = DCO.GetUINT16(0x0000, 0x0300);
			E700 = 0;
			E900 = 0;

			vr = new VR(0x0000, 0x0100, 2, &Command, FALSE);	DCORPtr->Push(vr);
			vr = new VR(0x0000, 0x0110, 2, &MId, FALSE);		DCORPtr->Push(vr);
			vr = new VR(0x0000, 0x0120, 2, &MRId, FALSE);		DCORPtr->Push(vr);
			vr = new VR(0x0000, 0x0200, 2, &E200, FALSE);		DCORPtr->Push(vr);
			vr = new VR(0x0000, 0x0300, 2, &E300, FALSE);		DCORPtr->Push(vr);
			vr = new VR(0x0000, 0x0700, 2, &E700, FALSE);		DCORPtr->Push(vr);
			vr = new VR(0x0000, 0x0800, 2, &DSet, FALSE);		DCORPtr->Push(vr);
			vr = new VR(0x0000, 0x0900, 2, &E900, FALSE);		DCORPtr->Push(vr);
			while(vr = DCO.Pop())
				;

			/* Get the uid from the received image */
			vr = DDOPtr->GetVR(0x0008, 0x0016);
			if (vr && vr->Length)
				uid.Set((char*)vr->Data);
			PDU.Write(DCORPtr, uid);
			}
		}
	PDU.Close();
	return ( TRUE );
	}

// Combined C-Echo / C-Store SCP's.  Performs same functions as
// CEchoServer() and CStoreServer().  This routine is ment to be
// run from a "driver" routine which handles the listen / accept socket
// I/O.
	

BOOL
CMEchoStoreServerProc(int	socketfd)
	{
	PDU_Service		PDU;
	DICOMCommandObject	DCO;
	DICOMCommandObject	*DCORPtr;
	DICOMDataObject		*DDOPtr;
	DICOMCommandObject	DCOR;
	VR			*vr;
	LE_UINT16		tuint16;
	LE_UINT16		Command, MId, MRId, DSet, E200, E300, E700,E900;
	UID			uid;

	// NOTE: PDU.Multiplex(filedes).  This call is saying, here's a just
	// connected socket, now go through the regular PDU association, and
	// return to me a connected DICOM link.
	if(PDU.Multiplex(socketfd))
		{
		while(TRUE)
			{
			if(!PDU.Read(&DCO))
				return ( FALSE );
			switch ( DCO.GetUINT16 ( 0x0000, 0x0100 ) )
				{
				case	0x0030:
					while(vr = DCO.Pop())
						{
					//	fprintf(stderr, "Object: (%4.4x, %4.4x), %d Length\n",
					//		vr->Group, vr->Element, vr->Length);

						switch(vr->Element)
							{
							case	0x0000:	delete vr; break;
							case	0x0002:	DCOR.Push(vr);	break;
							case	0x0100:
								tuint16 = 0x8030;	// 0x8030
								memcpy(vr->Data, &tuint16, 2);
								DCOR.Push(vr);	break;
							case	0x0800:	DCOR.Push(vr);	break;
							case	0x0110:
								vr->Element = 0x0120;
								DCOR.Push(vr);	break;
							default:
								delete vr;
							}
						}
					vr = new VR(0x0000, 0x0900, 0x0002, TRUE);
					tuint16 = 0;
					memcpy(vr->Data, &tuint16, 2);
					DCOR.Push(vr);
					uid.Set("1.2.840.10008.1.1");
					PDU.Write(&DCOR, uid);
					break;
				case	0x0001:		// CStore
					DDOPtr = new DICOMDataObject;
					if(!DDOPtr)
						return(FALSE);
					if(!PDU.Read(DDOPtr))
						return ( FALSE );
					if(ForkAProc)
						if(!Fork((void*) DDOPtr))
							{
//							fprintf(stderr, "Unable to create thread\n");
							delete DDOPtr;
							}
					DCORPtr = new DICOMCommandObject;
					if(!DCORPtr)
						return ( FALSE );
		
					Command = 0x8001;
					MId = DCO.GetUINT16(0x0000, 0x0110);
					MRId = MId;
					DSet = 0x0101;
					E200 = DCO.GetUINT16(0x0000, 0x0200);
					E300 = DCO.GetUINT16(0x0000, 0x0300);
					E700 = 0;
					E900 = 0;

					vr = new VR(0x0000, 0x0100, 2, &Command, FALSE);	DCORPtr->Push(vr);
					vr = new VR(0x0000, 0x0110, 2, &MId, FALSE);		DCORPtr->Push(vr);
					vr = new VR(0x0000, 0x0120, 2, &MRId, FALSE);		DCORPtr->Push(vr);
					vr = new VR(0x0000, 0x0200, 2, &E200, FALSE);		DCORPtr->Push(vr);
					vr = new VR(0x0000, 0x0300, 2, &E300, FALSE);		DCORPtr->Push(vr);
					vr = new VR(0x0000, 0x0700, 2, &E700, FALSE);		DCORPtr->Push(vr);
					vr = new VR(0x0000, 0x0800, 2, &DSet, FALSE);		DCORPtr->Push(vr);
					vr = new VR(0x0000, 0x0900, 2, &E900, FALSE);		DCORPtr->Push(vr);
					while(vr = DCO.Pop())
						;
						
					/* Get the uid from the received image */
					vr = DDOPtr->GetVR(0x0008, 0x0016);
					if (vr && vr->Length)
						uid.Set((char*)vr->Data);
					PDU.Write(DCORPtr, uid);
					break;
				}
			}
		}
	PDU.Close();
	return ( TRUE );
	}

// Driver routine for Multiplexed Socket I/O (re-use-address type).  Note,
// in a real server the child should be forked off.
BOOL
CMEchoStoreServer()
	{
	Socket	MSocket;

	if(!MSocket.Listen((char*)PORT))
		return ( FALSE );
	while(MSocket.Accept())
		{
		CMEchoStoreServerProc(MSocket.Socketfd);
		MSocket.Socketfd = 0;
		MSocket.Connected = FALSE;
		}
	return ( FALSE );	// should never return except on error
	}



BOOL
CEchoStoreServer()
	{
	PDU_Service			PDU;
	DICOMCommandObject	DCO;
	DICOMCommandObject	*DCORPtr;
	DICOMDataObject		*DDOPtr;
	DICOMCommandObject	DCOR;
	VR			*vr;
	LE_UINT16		tuint16;
	LE_UINT16		Command, MId, MRId, DSet, E200, E300, E700,E900;
	UID			uid;

	if(PDU.Listen(PORT))
		{
		while(TRUE)
			{
			if(!PDU.Read(&DCO))
				return ( FALSE );
			switch ( DCO.GetUINT16 ( 0x0000, 0x0100 ) )
				{
				case	0x0030:
					while(vr = DCO.Pop())
						{
					//	fprintf(stderr, "Object: (%4.4x, %4.4x), %d Length\n",
					//		vr->Group, vr->Element, vr->Length);

					//			delete vr;
						switch(vr->Element)
							{
							case	0x0000:	delete vr; break;
							case	0x0002:	DCOR.Push(vr);	break;
							case	0x0100:
								tuint16 = 0x8030;	// 0x8030
								memcpy(vr->Data, &tuint16, 2);
								DCOR.Push(vr);	break;
							case	0x0800:	DCOR.Push(vr);	break;
							case	0x0110:
								vr->Element = 0x0120;
							DCOR.Push(vr);	break;
							}
						}
					vr = new VR(0x0000, 0x0900, 0x0002, TRUE);
					tuint16 = 0;
					memcpy(vr->Data, &tuint16, 2);
					DCOR.Push(vr);
					uid.Set("1.2.840.10008.1.1");
					PDU.Write(&DCOR, uid);
					break;
				case	0x0001:		// CStore
					DDOPtr = new DICOMDataObject;
					if(!DDOPtr)
						return(FALSE);
					if(!PDU.Read(DDOPtr))
						return ( FALSE );
					if(ForkAProc)
						if(!Fork((void*) DDOPtr))
							{
							fprintf(stderr, "Unable to create thread\n");
							delete DDOPtr;
							}
					DCORPtr = new DICOMCommandObject;
					if(!DCORPtr)
						return ( FALSE );
		
					Command = 0x8001;
					MId = DCO.GetUINT16(0x0000, 0x0110);
					MRId = MId;
					DSet = 0x0101;
					E200 = DCO.GetUINT16(0x0000, 0x0200);
					E300 = DCO.GetUINT16(0x0000, 0x0300);
					E700 = 0;
					E900 = 0;

					vr = new VR(0x0000, 0x0100, 2, &Command, FALSE);	DCORPtr->Push(vr);
					vr = new VR(0x0000, 0x0110, 2, &MId, FALSE);		DCORPtr->Push(vr);
					vr = new VR(0x0000, 0x0120, 2, &MRId, FALSE);		DCORPtr->Push(vr);
					vr = new VR(0x0000, 0x0200, 2, &E200, FALSE);		DCORPtr->Push(vr);
					vr = new VR(0x0000, 0x0300, 2, &E300, FALSE);		DCORPtr->Push(vr);
					vr = new VR(0x0000, 0x0700, 2, &E700, FALSE);		DCORPtr->Push(vr);
					vr = new VR(0x0000, 0x0800, 2, &DSet, FALSE);		DCORPtr->Push(vr);
					vr = new VR(0x0000, 0x0900, 2, &E900, FALSE);		DCORPtr->Push(vr);
					while(vr = DCO.Pop())
						;
						
					/* Get the uid from the received image */
					vr = DDOPtr->GetVR(0x0008, 0x0016);
					if (vr && vr->Length)
						uid.Set((char*)vr->Data);
					PDU.Write(DCORPtr, uid);
					break;
				}
			}
		}
	// The sender should intiate the close at this point, but send
	// one anyway (wont hurt).
	PDU.Close();
	return ( TRUE );
	}
	
// clone of code from dgate for store client

class	RunTimeClassStorage	:
	public	StandardStorage
	{
		UID		MyUID;

	public:
		BOOL	uGetUID(UID &uid) { return ( GetUID(uid) ); };
		inline	int Read ( PDU_Service *PDU, DICOMCommandObject *DCO,
					DICOMDataObject *DDO )
			{ return ( StandardStorage :: Read ( PDU, DCO, DDO ) ); };
		inline	int Write ( PDU_Service *PDU, DICOMDataObject *DDO)
			{ return ( StandardStorage :: Write ( PDU, DDO ) ); };
		BOOL	GetUID(UID	&);
		BOOL	SetUID(UID	&);
		BOOL	SetUID(VR	*);
		BOOL	SetUID(DICOMDataObject	*);
	};

BOOL	RunTimeClassStorage :: GetUID ( UID	&uid )
	{
	uid = MyUID;
	return ( TRUE );
	}

BOOL	RunTimeClassStorage :: SetUID ( UID	&uid )
	{
	MyUID = uid;
	return ( TRUE );
	}

BOOL	RunTimeClassStorage :: SetUID ( VR	*vr )
	{
	char	s [ 66 ];

	memset((void*)s, 0, 64);
	if(vr)
		{
		memcpy((void*)s, vr->Data, vr->Length%64);
		MyUID.Set(s);
		return(TRUE);
		}
	return ( FALSE );
	}

BOOL	RunTimeClassStorage :: SetUID ( DICOMDataObject *DDOPtr)
	{
	return(SetUID (DDOPtr->GetVR(0x0008, 0x0016)));
	}

void cstoreclient(char *file, char *host, char *port)
{   char *p;
    RunTimeClassStorage	RTCStorage;
    PDU_Service		PDU;
    DICOMDataObject*	pDDO;
    UID			AppUID ("1.2.840.10008.3.1.1.1");
    UID			iUID;
    VR			*vr;

    pDDO = PDU.LoadDICOMDataObject(file);
    if (!pDDO)
    { fprintf(stderr, "failed to load %s\n", file);
      return;
    }

    PDU.SetApplicationContext ( AppUID );
    PDU.SetLocalAddress ( (unsigned char *) "caller" );
    PDU.SetRemoteAddress( (unsigned char *) "called" );

    vr = pDDO -> GetVR(0x0008, 0x0016);
    SetUID ( iUID, vr );
    PDU.AddAbstractSyntax ( iUID );

    if (!PDU.Connect((unsigned char *)host, (unsigned char *)port))
    { fprintf(stderr, "failed to connect\n");
      delete pDDO;
      return;
    }

    if (!PDU.GetPresentationContextID(iUID))
    { fprintf(stderr, "DICOM server does not accept type of sent image\n");
      PDU.Close();
      delete pDDO;
      return;
    }

    RTCStorage.SetUID(iUID);

    if (!RTCStorage.Write(&PDU, pDDO))
    { fprintf(stderr, "failed to send DICOM image\n");
      PDU.Close();
      delete pDDO;
      return;
    }

    delete pDDO;
    PDU.Close();
}

// Fix possible problems in an image.
BOOL
FixImage(DICOMDataObject *DDOPtr)
	{
	// This routine fixes some deficient images (sent from outdated modalities).
	VR	*vr;

	vr = DDOPtr->GetVR(0x0010, 0x0010);
	if(!vr)
		{
		vr = new VR(0x0010, 0x0010, 8, (BOOL)TRUE);
		memcpy(vr->Data, (void*)"NO NAME", 8);
		DDOPtr->Push(vr);
		}
			
	vr = DDOPtr->GetVR(0x0010, 0x0020);
	if(!vr)
		{
		vr = new VR(0x0010, 0x0020, 8, (BOOL)TRUE);
		memcpy(vr->Data, (void*)"00000000", 8);
		fprintf(stdout, "----------------------------------------------------------------\n");
		fprintf(stdout, "Added missing patient ID: 00000000\n");
		fprintf(stdout, "----------------------------------------------------------------\n");
		DDOPtr->Push(vr);
		}
	else if (vr->Length < 2)
		{
		int		Length;
		char	*s;
		
		s = (char*)vr->Data;
		Length = vr->Length;
		
		while ( Length )
			{
			if (s[Length] == ' ')
				{
				--Length;
				continue;
				}
			if (s[Length] == '\0')
				{
				--Length;
				continue;
				}
			break;
			}
		vr->Length = Length;
		
		if ( vr->Length < 2)
			{
                        vr->ReAlloc(8);
			memcpy(vr->Data, (void*)"00000000", 8);
			fprintf(stdout, "----------------------------------------------------------------\n");
			fprintf(stdout, "Added missing patient ID: 00000000\n");
			fprintf(stdout, "----------------------------------------------------------------\n");
			}
		}

	else if (vr->Length==10 && vr->Data)
		{
		char patid[11];
		char orgpatid[11];
		int Length;

		memcpy(patid, (char *)(vr->Data), 10);
		patid[10]=0;
		strcpy(orgpatid, patid);

		if ( patid[0] == '0' && patid[1] == '0' && atoi(patid)>1000000 )
			{
			memset(patid, 0, 10);
			if      (((char *)(vr->Data))[2]=='0') memcpy(patid, ((char *)(vr->Data)) + 3, Length=7);
			else if (((char *)(vr->Data))[1]=='0') memcpy(patid, ((char *)(vr->Data)) + 2, Length=8);
			else if (((char *)(vr->Data))[0]=='0') memcpy(patid, ((char *)(vr->Data)) + 1, Length=9);
			else	                   memcpy(patid, (char *)(vr->Data) + 0, Length=10);

			if (Length!=10)
				{
				fprintf(stdout, "----------------------------------------------------------------\n");
				fprintf(stdout, "stripped leading zeros of patient ID: %s to %s\n", orgpatid, patid);
				fprintf(stdout, "----------------------------------------------------------------\n");
				}

			if (Length&1) Length++;

                        vr->ReAlloc(Length);
			memcpy(vr->Data, patid, Length);
			}
		}
	
	else if (vr->Length==8 && vr->Data)
		{
		char patid[11];
		char orgpatid[11];
		int Length;

		memcpy(patid, (char *)(vr->Data), 8);
		patid[8]=0;
		strcpy(orgpatid, patid);

		if ( patid[0] == '0' && atoi(patid)>1000000 )
			{
			memset(patid, 0, 10);
			if (((char *)(vr->Data))[0]=='0') memcpy(patid, ((char *)(vr->Data)) + 1, Length=7);
			else	                          memcpy(patid, (char *)(vr->Data) + 0,   Length=8);

			if (Length!=8)
				{
				fprintf(stdout, "----------------------------------------------------------------\n");
				fprintf(stdout, "stripped leading zeros of patient ID: %s to %s\n", orgpatid, patid);
				fprintf(stdout, "----------------------------------------------------------------\n");
				}

			if (Length&1) Length++;

			vr->ReAlloc(Length);
			memcpy(vr->Data, patid, Length);
			}
		}

	return ( TRUE );
	}

BOOL
CForwarder(BYTE *PORT, BYTE *ip, BYTE *port, int dump, int once, int SocketFd, int mode);
BOOL
CForwarderMain(BYTE *PORT, BYTE *ip, BYTE *port, int dump, int mode);

typedef struct ForwardData
	{
	BYTE	listen_port[64];
	BYTE	to_ip[64];
	BYTE	to_port[64];
	int	dump;
        int     SocketFd;
        int     mode;
	} dummy_fd;

BOOL
ForwardProc(ForwardData *p)
	{
	CForwarder(p->listen_port, p->to_ip, p->to_port, p->dump, 1, p->SocketFd, p->mode);
	return (FALSE);	
	}

BOOL
ForwardProcMain(ForwardData *p)
	{
	CForwarderMain(p->listen_port, p->to_ip, p->to_port, p->dump, p->mode);
	return (FALSE);	
	}

// Forwarder: test program to illustrate gateway operation; forwards everything; can be used as sniffer
// Example use for echo:
// 	test -e 104			runs echo server
// 	test -fe 105 127.0.0.1 104	runs forwarder (listen to 105; forward to localhost:104)
// 	test -e 127.0.0.1 105		send echo request to 105, is forwarded to 104
// dump = 0: log nothing, 1=log commands, 2=log commands and request data, 3=log commands and data
//        4: log everything including image header
// dump2: same for return connection

char	gatewayname[]="GATEWAY ";	/* how PACS knows the gateway as C-MOVE destination (empty to forward original) */
char	gatewayport[]="1233";		/* port of gateway for PACS (empty to not auto-forward returning C-MOVEs) */
int	returndump=0;			/* do you want to dump returning C-STOREs ? */
char	viewerport[]="1234";		/* port used by viewers */

BOOL
CForwarderMain(BYTE *PORT, BYTE *ip, BYTE *port, int dump, int mode)
	{
	Socket			MSocket;
	ForwardData 		FData;
	DWORD			ThreadID;

	if(!MSocket.Listen((char*)PORT))
		{
		fprintf(stdout, "failed to listen\n");
		return ( FALSE );
		}

	strcpy((char *)(FData.listen_port), 	(char*)PORT);
	strcpy((char *)(FData.to_port),		(char*)port);
	strcpy((char *)(FData.to_ip),		(char *)ip);
	FData.dump     = dump;
	FData.mode     = mode;

	while(MSocket.Accept())
		{
		FData.SocketFd = MSocket.Socketfd;
		CreateThread(NULL, 0x00080000, (LPTHREAD_START_ROUTINE) ForwardProc, &FData, 0, &ThreadID);
		MSocket.Socketfd = 0;
		MSocket.Connected = FALSE;
		}

	return FALSE;
	}

static int returnstarted=0;

BOOL
CForwarder(BYTE *PORT, BYTE *ip, BYTE *port, int dump, int once, int SocketFd, int mode)
	{
	PDU_Service		PDU;
	PDU_Service		PDUF;
	DICOMCommandObject	DCO;
	DICOMCommandObject	DCOF;
	DICOMCommandObject	DCOR;
	DICOMDataObject		DDO;
	DICOMDataObject		DDOF;
	VR			*vr, *vr2;
	UID			uid, uid2, uid3, uid4, uid5, uid6, uid7, uid8;
	LE_UINT16		command, status, dataset;
	char			Type[64];
	char			UID[64];
	unsigned char		called[17], calling[17], destination[17];
	int			first = 1;
	ForwardData 		FData;
	DWORD			ThreadID;
	HANDLE			Thread = NULL;

	// listen for incoming request; collect command object (DCO) and forward (DCOF)
	if (SocketFd)
		{
		if(!PDU.Multiplex(SocketFd)) return (FALSE);
		}
	else
		{
		if(!PDU.Listen(PORT)) return (FALSE);
		}

	if (dump>0) 
		{
		fprintf(stdout, "\n\n********************************************************\n");
		IARQ (PDU);
		}

again:
	if (!PDU.Read(&DCO)) 
		{
		return FALSE;
		}

	if (dump>0) 
		{
		fprintf(stdout, "--------------- command for request (%d) -----------------\n", once);
		NonDestructiveDumpDICOMObject(&DCO);
		}

	command = DCO.GetUINT16(0x0000, 0x0100);
	dataset = DCO.GetUINT16(0x0000, 0x0800);
	while(vr = DCO.Pop())
		{
		switch(vr->Element)
			{
			case	0x0000:	delete vr;      break;
			case	0x0002:	if (vr->Group==0)
						{ 
						memset(UID, 0, 64); 
  					  	memcpy(UID, vr->Data, vr->Length); 
						}
					DCOF.Push(vr);	break;
			case	0x0600:	if (vr->Group==0)
						{ 
						// save original C-MOVE destination
						memset(destination, 0, 17); 
  					  	memcpy(destination, vr->Data, vr->Length);

						// replace c-move destination for mode 'b'
						if ((mode=='b' || mode=='B') && gatewayname[0])
							{
							vr2 = new VR (0x0000, 0x0600, strlen(gatewayname), (void*)gatewayname, FALSE);
							DCOF.Push(vr2);	break;
							}
						}
					DCOF.Push(vr);	break;
			default:	DCOF.Push(vr);	break;
			}
		}

	if (first)
		{
		PDU.GetLocalAddress(calling);
		PDU.GetRemoteAddress(called);

		// create outgoing association
		PDUF.ClearAbstractSyntaxs();
		PDUF.SetLocalAddress (calling);
		PDUF.SetRemoteAddress(called);

		uid.Set("1.2.840.10008.3.1.1.1");	// Dicom APP
		PDUF.SetApplicationContext(uid);

		uid.Set(UID);				// get from caller!

		// use to test printer connections (overrides to...)
		if (mode=='p' || mode=='P')
			{
			uid.Set("1.2.840.10008.5.1.1.9");		//ljz
 			PDUF.AddAbstractSyntax(uid);
			uid2.Set("1.2.840.10008.1.1");
 			PDUF.AddAbstractSyntax(uid2);
			uid3.Set("1.2.840.10008.5.1.1.18");
 			PDUF.AddAbstractSyntax(uid3);
			uid4.Set("1.2.840.10008.5.1.1.1");
 			PDUF.AddAbstractSyntax(uid4);
			uid5.Set("1.2.840.10008.5.1.1.2");
 			PDUF.AddAbstractSyntax(uid5);
			uid6.Set("1.2.840.10008.5.1.1.4");
 			PDUF.AddAbstractSyntax(uid6);
			uid7.Set("1.2.840.10008.5.1.1.4.1");
 			PDUF.AddAbstractSyntax(uid7);
			uid8.Set("1.2.840.10008.5.1.1.16");
 			PDUF.AddAbstractSyntax(uid8);
			}
		else
 			PDUF.AddAbstractSyntax(uid);
		}

	switch(command)				// possible commands
	{ case 0x0001: 	strcpy(Type, "CSTORE");	break;
	  case 0x0010: 	strcpy(Type, "CGET");	break;
	  case 0x0020: 	strcpy(Type, "CFIND");	break;
	  case 0x0021: 	strcpy(Type, "CMOVE");

			// start thread to forward returning C-STOREs (mode 'b' only)
			if ((mode=='b' || mode=='B') && gatewayport[0] && returnstarted==0)
				{
				fprintf(stdout, "Start return connection  to %s\n", destination);
				strcpy((char *)(FData.listen_port), 	gatewayport);
				strcpy((char *)(FData.to_port),		viewerport);
				FData.dump = returndump;

				if (destination[1]>='0' && destination[1]<='9')
					strcpy((char *)(FData.to_ip),  	(char *)destination+1);
				else if (destination[2]>='0' && destination[2]<='9')
					strcpy((char *)(FData.to_ip),  	(char *)destination+2);

				Thread = CreateThread(NULL, 0x00008000, (LPTHREAD_START_ROUTINE) ForwardProcMain, &FData, 0, &ThreadID);

				returnstarted = 1;
				}

			break;
	  case 0x0030: 	strcpy(Type, "CECHO");	break;
	  case 0x0100: 	strcpy(Type, "NREPORT");break;
	  case 0x0110: 	strcpy(Type, "NGET");	break;
	  case 0x0120: 	strcpy(Type, "NSET");	break;
	  case 0x0130: 	strcpy(Type, "NACTION");break;
	  case 0x0140: 	strcpy(Type, "NCREATE");break;
	  case 0x0150: 	strcpy(Type, "NDELETE");break;
	  case 0x0fff: 	strcpy(Type, "CCANCEL");break;
	}

	if (dataset!=0x101)
		{
		PDU.Read(&DDO);
		if ((command!=1 && dump>1) || (command==1 && dump>3)) 
			{
			fprintf(stdout, "--------------- data for %s request (%d) -----------------\n", Type, once);
			NonDestructiveDumpDICOMObject(&DDO);
			}

		if (mode=='f' || mode=='F' && command==0x0001)
			FixImage(&DDO);
		}

	if (first)
		{
		if(!PDUF.Connect(ip, port))
			{
			Dump((AAssociateRQ&)PDUF);
			fprintf(stderr, "AAssociateRJ: %d, %d, %d\n",
				PDUF.AAssociateRJ :: Result,
				PDUF.AAssociateRJ :: Source,
				PDUF.AAssociateRJ :: Reason);
			return ( FALSE );
			}
		first = 0;
		}
	
	fprintf(stderr, "Forwarding %s-RQ %d\n", Type, PDUF.Write(&DCOF, uid));
	if (dataset!=0x101) PDUF.Write(&DDO, uid);

	if (dump>0) 
		fprintf(stderr, "Waiting for %s-RSP\n", Type);

	while (1)
		{
		if (!PDUF.Read(&DCOR)) 
			{
			if (once) 
				return FALSE;
			else
				break;				// association lost
			}

		if (dump>1) 
			{
			fprintf(stdout, "--------------- response (command) (%d) -----------------\n", once);
			NonDestructiveDumpDICOMObject(&DCOR);
			}

		// send the response back to the original client
		if (dump>0) 
			fprintf(stderr, "Forwarding %s-RSP\n", Type);

		if(DCOR.GetUINT16(0x0000, 0x0100)!=0x8000+command) break; // illegal response
		dataset = DCOR.GetUINT16(0x0000, 0x0800);
		status  = DCOR.GetUINT16(0x0000, 0x0900);
		PDU.Write(&DCOR, uid);

		if (status!=0 && status!=0xff00) 	    
			break;						// error

		if (dataset==0x101) 
			{
			if (command>=0x0100) 
				goto again;				// allow train of commands
			if (status==0xff00)
				continue;
			if (status==0)
				goto again;
			break;						// no data field
			}

		if(!PDUF.Read(&DDOF)) break;				// get data field
		if (dump>2) 
			{
			fprintf(stdout, "--------------- response (data) (%d) -----------------\n", once);
			NonDestructiveDumpDICOMObject(&DDOF);
			}

		if (dump>0) 
			fprintf(stderr, "Forwarding %s-RSP data field\n", Type);
		PDU.Write(&DDOF, uid);					// forward to original client

		if (command>=0x0100) 
			goto again;					// allow train of commands
		if (status==0xff00)
			continue;
		if (status==0)
			goto again;
		}


	PDUF.Close();
	PDU.Close();

	return ( TRUE );
	}

BOOL
PrintOptions()
	{
	fprintf(stderr, "usage: dicom [-v filename ] | [-l:local_acr_nema] [-r:remote_acr_nema] [-s<e|s|c>|e|f|m] [-1]<host &| port>\n");
	return ( TRUE );
	}

#ifdef	MAC
extern	int	SMain(int	, char	**);
main()
	{
	int		argc;
	char	*ch, *nch;
	char	s[256];
	char	str[15][128];
	char	*argv[15];
	
	VRType.AttachRTC("dd.txt");

	argc = 0;
	while(argc<15)
		{
		argv[argc] = str[argc];
		++argc;
		}
	
	GUSISetup(GUSIwithInternetSockets);

	while(TRUE)
		{
		printf("-> ");
		fgets(s, 256, stdin);
		argc = 1;
		if(!strncmp(s, "quit", 4))
			return(0);
		ch = s;
		while((*ch)==' ')
			++ch;
		while(nch=strchr(ch, ' '))
			{
			(*nch)='\0';
			strcpy(argv[argc], ch);
			++argc;
			ch = nch;
			++ch;
			while((*ch)==' ')
				++ch;
			}
		if(nch=strchr(ch, '\n'))
			{
			(*nch)='\0';
			strcpy(argv[argc], ch);
			++argc;				
			}
		SMain(argc, argv);	
		}
	}
#endif

#ifndef	MAC
int main (int	argc, char	*argv[])
#else
int
SMain(int	argc, char	*argv[])
#endif
	{
	int	valid_argc;
	int	Mode;
	int	ContOp;

	valid_argc = 1;
	Mode = CECHO_CLIENT;
	ForkAProc = FALSE;
	ForkProc = NULL;
	ContOp = 2;

#ifndef	MAC
	VRType.AttachRTC("dd.txt");
#endif

	// -fe/fd option: experimental request forwarder with parameters: listenport sendip sendport
	if (argc>3)
		{
		if (argv[1][1]=='f' || argv[1][1]=='F')
			{
			if (argv[1][2]=='e' || argv[1][2]=='E' || 
			    argv[1][2]=='p' || argv[1][2]=='P' ||
			    argv[1][2]=='f' || argv[1][2]=='F' ||
			    argv[1][2]=='b' || argv[1][2]=='B'
                           )
				{
				int dump=0;
				if (argc>4 && argv[5]) dump = atoi(argv[5]);
				if (argc>5 && argv[6]) returndump = atoi(argv[6]);
				strcpy((char*) PORT, argv[2]);
				CForwarderMain(PORT, (BYTE *)argv[3], (BYTE *)argv[4], dump, argv[1][2]);
				}
			}

		if (argv[1][1]=='t' || argv[1][1]=='T')
			{
			cstoreclient(argv[2], argv[3], argv[4]);
			exit(1);
			}
		}

	while(valid_argc < argc)
		{
		if((argv[valid_argc][0]=='-')||(argv[valid_argc][0]=='/'))
			{
			switch ( argv[valid_argc][1] )
				{
				case	'l':
				case	'L':
					strcpy((char*) LOCAL_ACR, &argv[valid_argc][3]);
					break;
				case	'r':
				case	'R':
					strcpy((char*) REMOTE_ACR, &argv[valid_argc][3]);
					break;
				case	'd':
				case	'D':
					strcpy((char*) Directory, &argv[valid_argc][3]);
					break;
				case	'P':
				case	'p':
					strcpy((char*) PNAME, &argv[valid_argc][3]);
					break;				
				case	'1':
					ContOp = FALSE;
					break;
				case	'v':
				case	'V':
						Mode = CDUMPFILE;
						break;
				case	'S':
				case	's':
					switch ( argv[valid_argc][2] )
						{
						case	'c':
						case	'C':
							Mode = CECHOSTORE_SERVER;
							if(ContOp==2)
								ContOp = TRUE;
							break;
						case	'E':
						case	'e':						
							Mode = CECHO_SERVER;
							if(ContOp==2)
								ContOp = TRUE;
							break;
						case	'S':
						case	's':
							Mode = CSTORE_SERVER;
							ForkProc = DICOMParseProc;
							ForkAProc = TRUE;
							if(ContOp==2)
								ContOp = TRUE;
							break;
						default:
							fprintf(stderr, "error server option: %c\n", argv[valid_argc][2]);
							PrintOptions();
							return ( 1 );
							
						}
					break;
				case	'F':
				case	'f':
					Mode = CFIND_CLIENT;
					break;
				case	'm':
				case	'M':
					Mode = CMOVE_CLIENT;
					break;
				case	'E':
				case	'e':
					Mode = CECHO_CLIENT;
					break;
				default:
					fprintf(stderr, "unknown option: %c\n",
						argv[valid_argc][1]);
					PrintOptions();
					return(1);
				}
			++valid_argc;
			}
		else
			break;
		}
	if(ContOp==2)
		ContOp = FALSE;
		
	switch ( Mode )
		{
		case	CDUMPFILE:
			while ( valid_argc < argc )
				DumpDDOFile(argv[valid_argc++]);
			return(0);
		case	CFIND_CLIENT:
			switch(argc - valid_argc)
				{
				case	0:
					PrintOptions();
					printf("Extended options (C-FIND):\n");
					printf("TEST -f host port QUERYLEVEL PATID STUDYDATE SERIESNUMBER\n");
					return(2);
				case	1:
					return(CFindClient((BYTE*) argv[valid_argc], (BYTE*) "104", 0, NULL));
				case	2:
					return(CFindClient((BYTE*) argv[valid_argc], (BYTE*) argv[valid_argc+1], 0, NULL));
				default:
					return(CFindClient((BYTE*) argv[valid_argc], (BYTE*) argv[valid_argc+1], argc - valid_argc + 2, argv + valid_argc - 2));

//					fprintf(stderr, "garbled command line\n");
//					PrintOptions();
//					return(3);
				}
			break;
		case	CMOVE_CLIENT:
			switch(argc - valid_argc)
				{
				case	0:
					PrintOptions();
					printf("Extended options (C-MOVE):\n");
					printf("TEST -m -l:LOCALAE(dest) host port QUERYLEVEL PATID STUDYDATE SERIESNUMBER\n");
					return(2);
				case	1:
					return(CMoveClient((BYTE*) argv[valid_argc], (BYTE*) "104", 0, NULL));
				case	2:
					return(CMoveClient((BYTE*) argv[valid_argc], (BYTE*) argv[valid_argc+1], 0, NULL));
				default:
					return(CMoveClient((BYTE*) argv[valid_argc], (BYTE*) argv[valid_argc+1], argc - valid_argc + 2, argv + valid_argc - 2));

//					fprintf(stderr, "garbled command line\n");
//					PrintOptions();
//					return(3);
				}
			break;
		case	CSTORE_SERVER:
			switch (argc - valid_argc)
				{
				case	1:
					strcpy((char*) PORT, argv[valid_argc]);
				case	0:
					if(ContOp)
						{
						while ( TRUE )
							{
							CStoreServer();
							}
						}
					else
						{
						CStoreServer();
						}
					break;
				}		
			break;
		
		case	CECHOSTORE_SERVER:
			switch (argc - valid_argc)
				{
				case	1:
					strcpy((char*) PORT, argv[valid_argc]);
				case	0:
					if(ContOp)
						{
						CMEchoStoreServer();
						}
					else
						{
						CEchoStoreServer();
						}
					break;
				}		
			break;
		case	CECHO_CLIENT:
			switch(argc - valid_argc)
				{
				case	0:
					PrintOptions();
					return(2);
				case	1:
					return(CEchoClient((BYTE*) argv[valid_argc], (BYTE*) "104", "", ""));
				case	2:
					return(CEchoClient((BYTE*) argv[valid_argc], (BYTE*) argv[valid_argc+1], "", ""));
				case	3:
					return(CEchoClient((BYTE*) argv[valid_argc], (BYTE*) argv[valid_argc+1], argv[valid_argc+2], ""));
				case	4:
					return(CEchoClient((BYTE*) argv[valid_argc], (BYTE*) argv[valid_argc+1], argv[valid_argc+2], argv[valid_argc+3]));
				default:
					fprintf(stderr, "garbled command line\n");
					PrintOptions();
					return(3);
				}
			break;
		case	CECHO_SERVER:
			switch(argc - valid_argc)
				{
				case	0:
					if(ContOp)
						while( TRUE )
							{
							if(!CEchoServer())
								break;
							}
					else
						return(CEchoServer());
					break;
				case	1:
					strcpy((char*) PORT, argv[valid_argc]);
					if(ContOp)
						while ( TRUE )
							{
							if(!CEchoServer())
								break;
							}
					else
						return(CEchoServer());
					break;
				default:
					fprintf(stderr, "garbled command line\n");
					PrintOptions();
				}
			break;
		}
	return(10000);	// should never get here
	}

