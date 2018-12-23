/*
20020816        mvh     Fix in (unfinished) main server code
20020822	mvh	Made this sample program part of code base
20080113        mvh     Fixed for MS7 compiler
20100120        mvh     Fixed for MS8 compiler
*/


#include <iostream>
using namespace std;

#include	<math.h>

#include	"dicom.hpp" 
#include	"printsop.hpp" 
#include	"errdescr.hpp"
#include	"dc3tags.h"

class	PolaroidPDU_Service	:
	public	PDU_Service
	{
	public:
		BOOL	FixPolaroidConnection()
			{
			UINT	Index;
			UID		uidbad("1.2.840.10008.1.1");
			UID		uidgood("1.2.840.10008.1.2");
			
			Index = 0;
			while ( Index < AcceptedPresentationContexts.GetSize() )
				{
//				if (AcceptedPresentationContexts.Get(Index).TrnSyntax.TransferSyntaxName
//						== uidbad )
					AcceptedPresentationContexts.Get(Index).TrnSyntax.TransferSyntaxName
						= uidgood;
				printf("FPC: %s\n",
					AcceptedPresentationContexts.Get(Index).TrnSyntax.TransferSyntaxName.GetBuffer(1));
				++Index;
				}
			return ( TRUE );
			}					
	};
	
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

		printf("\t[%d] Presentation Context ID=%d : ABS=\"%s\"\n",
			Index, PC.PresentationContextID, s);
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
		fprintf(stderr, "(null)");
		return ( TRUE );
		}
	if ( ! TypeCode )
		{
		switch ( Size )
			{
			case	2:
				tint16 = (*((UINT16 *) data));
				fprintf(stderr, "(%d|%x)", (UINT) tint16, (UINT) tint16);
				break;
			case	4:
				tint32 = (*((UINT32 *) data));
				fprintf(stderr, "[%d|%x]", (UINT) tint32, (UINT) tint32);
				break;
			default:
				if(IsDataAlpha(Size, (BYTE*) data))
					{
					if(Size > 64)
						Size = 64;
					strncpy(Str, (char*) data, Size);
					Str[Size] = '\0';
					fprintf(stderr, "\"%s\"", Str);
					break;
					}
				Index = 0;
				while(Index < Size)
					{
					if(Index >= 16)
						break;
					c = (char*)data;
					c += Index;
					fprintf(stderr, "%2.2x(%c)", *c, *c);
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
				fprintf(stderr, "\"%s\"", Str);
				break;
					
			case	'FL':
			case	'FD':
				break;				
			case	'AT':
				Index = 0;
				while ( Index < Size )
					{
					UINT16	Grp, Elm;
					
					Grp = (*l16);++l16;
					Elm = (*l16);++l16;
					fprintf(stderr, "(%4.4x, %4.4x) ", Grp, Elm);
					Index += 4;
					}
				break;
			case	'OB':
				b = (BYTE*)data;
				while ( Index < Size )
					{
					if ( Index > 10 )
						break;
					fprintf(stderr, "%2.2x ", b);
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
					fprintf(stderr, "%4.4x ", (UINT16)(*w));
					++w;
					++Index;
					++Index;
					}
				break;
					
			case	'SQ':
				fprintf(stderr, "Sequence Item\n");
				break;
			case	'UL':
				
				while ( Index < Size )
					{
					if ( Index > 40 )
						break;
					
					fprintf(stderr, "%ld ", (UINT32)(*l32));
					++l32;
					Index += 4;
					}
				break;
			case	'US':
				
				while ( Index < Size )
					{
					if ( Index > 20 )
						break;
					
					fprintf(stderr, "%d ", (UINT32)(*l16));
					++l16;
					Index += 2;
					}
				break;
			case	'SL':
				
				while ( Index < Size )
					{
					if ( Index > 40 )
						break;
					
					fprintf(stderr, "%ld ", (UINT32)(*s32));
					++s32;
					Index += 4;
					}
				break;
			case	'SS':
				
				while ( Index < Size )
					{
					if ( Index > 20 )
						break;
						
					fprintf(stderr, "%d ", (UINT32)(*s16));
					++s16;
					Index += 2;
					}
				break;
		
			}
		}
	return ( TRUE );
	}
	
	
BOOL	DumpVR(VR	*vr, RTC *theRTC)
	{
	INT		Index;
	char	*c;
	UINT16	TypeCode;
	char	s[256];

	s[0] = '\0';
	TypeCode = 0;
	if(theRTC)
		TypeCode = theRTC->RunTimeClass(vr->Group, vr->Element, s);
	if(TypeCode)
		fprintf(stderr, "Object: (%4.4x, %4.4x, %d, %c%c, %s) ",
			vr->Group, vr->Element, vr->Length,
			TypeCode>>8, TypeCode&0x00ff, s);
	else
		fprintf(stderr, "Object: (%4.4x, %4.4x, %d, Type Unknown) ",
			vr->Group, vr->Element, vr->Length);
	PrintUDATA ( vr->Length, vr->Data, TypeCode );
	fprintf(stderr, "\n");
	return(TRUE);
	}

BOOL
DumpDDO(
	DICOMDataObject	*DDO,
	UINT			TabCount,
	RTC *theRTC)
	{
	VR			*vr;
	DICOMDataObject aTempDDO;
	while (vr = DDO->Pop())
		{
		UINT	Index = 0;
		while ( Index < TabCount)
			{
			fprintf(stderr, "\t");
			++Index;
			}
		DumpVR(vr, theRTC);
		if ( vr->SQObjectArray )
			{
			Array < DICOMDataObject * > 	*ADDO =
				(Array<DICOMDataObject*>*) vr->SQObjectArray;
			
			Index = 0;
			while ( Index < ADDO->GetSize() )
				{
				fprintf(stderr, "> Start of Sequence\n");
				DumpDDO(ADDO->Get(Index), TabCount+1, theRTC);
				fprintf(stderr, "> End of Sequence\n");
				++Index;
				}
			}
			aTempDDO.Push(vr);
		}
	while(vr = aTempDDO.Pop())
		{
		DDO->Push(vr);
		}
	return ( TRUE );
	}

class MyGrayscalePrintManagement : public BasicGrayscalePrintManagementMeta
{

		// define stubs for missing SCP functions we don't need

	private:

		virtual BOOL    ActionPrint ( AbstractFilmSession *, DICOMDataObject * )
		{
			return FALSE;
		}

		virtual BOOL    ActionPrint ( AbstractFilmBox *, DICOMDataObject * )
		{
			return FALSE;
		}

		virtual void    HandlePrinterEvent ( DICOMDataObject *theDDO,
			UID		&theUID,
			UINT16 theEventTypeID,
			void *theArg)
		{
			cerr << endl;
			cerr << "Received a Printer Event " << theEventTypeID <<
				" Report!" << endl;

			RTC *aRTC = (RTC *)theArg;
			DumpDDO(theDDO, 0, aRTC);
			cerr << endl;
			return;
		}

		virtual void    HandlePrintJobEvent ( DICOMDataObject *theDDO,
			UID		&theUID,
			UINT16 theEventTypeID,
			void *theArg)
		{
			cerr << endl;
			cerr << "Received a Print Job Event " << theEventTypeID <<
				" Report!" << endl;

			RTC *aRTC = (RTC *)theArg;
			DumpDDO(theDDO, 0, aRTC);
			cerr << endl;
			return;
		}
		virtual int		HandleError (
				const	ErrorDescription	&theError ) const
			{
			cerr << "HandleError: " <<
				theError.GetFile() << ": (" <<
				theError.GetLine() << ") " << endl
				<<
				theError.GetClass() <<
				" :: " << theError.GetMethod() << " Msg = " <<
				theError.GetMessage() << endl;
				
				
			return ( TRUE );
			
			}
};
#define COLLATED FALSE

BOOL
StripDDOForPrinting (
	DICOMDataObject	*DDO )
	{
	DICOMDataObject	TDDO;
	VR				*vr;
	
	while ( vr = DDO->Pop() )
		{	
		if ( vr->Group == 0x0028 && vr->Element == 0x0002 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x0028 && vr->Element == 0x0004 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x0028 && vr->Element == 0x0010 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x0028 && vr->Element == 0x0011 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x0028 && vr->Element == 0x0034 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x0028 && vr->Element == 0x0100 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x0028 && vr->Element == 0x0101 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x0028 && vr->Element == 0x0102 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x0028 && vr->Element == 0x0103 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x7fe0 && vr->Element == 0x0010 )	{ TDDO.Push(vr);continue; }
		
		delete vr;
		}
	while ( vr = TDDO.Pop() )
		DDO->Push(vr);

	return ( TRUE );
	}
	
static int PrintGrayScaleImages (
	BYTE	*MyAE,
	BYTE	*RemoteAE,
	BYTE	*RemoteHost,
	BYTE	*RemotePort,
	Array < DICOMDataObject *>	*ADDO,
	char	*FilmFormat,
	int		ImagesPerRow,
	int		ImagesPerColumn,
	char	*AnnotationText)
	{
	UID	aBasicFilmSessionSOPClassUID("1.2.840.10008.5.1.1.1");
	UID	aBasicFilmBoxSOPClassUID("1.2.840.10008.5.1.1.2");
	UID	aBasicGrayscaleImageBoxSOPClassUID("1.2.840.10008.5.1.1.4");
	UID	aPrintJobSOPClassUID("1.2.840.10008.5.1.1.14");
	UID	aBasicGrayscalePrintManagementMetaSOPClass("1.2.840.10008.5.1.1.9");
	UID	aBasicPrinterSOPClassUID("1.2.840.10008.5.1.1.16");
	UID	aBasicAnnotationBoxSOPClassUID("1.2.840.10008.5.1.1.15");
	UID	aAppuid("1.2.840.10008.3.1.1.1");
	PolaroidPDU_Service	PDU;
	RTC	VRType(TRUE);

	// create a run-time classer for the PDU_Service to use
	// in making explicit VR's out of implicit ones
	// tell RTC to keep descriptions and the dictionary name

	PDU.AttachRTC(&VRType);
	VRType.AttachRTC("dd.txt");

	// install the RTC in the PDU_Service
	// tell PDU not to destroy RTC when PDU is destroyed

	PDU.AttachRTC(&VRType, FALSE);

    MyGrayscalePrintManagement aSession;  

	// add the abstract syntaxes we need to use in the association

    PDU.SetApplicationContext(aAppuid);
    PDU.AddAbstractSyntax(aBasicGrayscalePrintManagementMetaSOPClass);
	//                ^^^^ this the one accepted by the MIR print_server app !

/*    PDU.AddAbstractSyntax(
		aBasicFilmSessionSOPClassUID);
    PDU.AddAbstractSyntax(
		aBasicFilmBoxSOPClassUID);
    PDU.AddAbstractSyntax(
		aBasicGrayscaleImageBoxSOPClassUID);
    PDU.AddAbstractSyntax(
		aBasicPrinterSOPClassUID);
*/	 
    PDU.AddAbstractSyntax(aPrintJobSOPClassUID);
    PDU.AddAbstractSyntax(aBasicAnnotationBoxSOPClassUID);

	// set application titles (MIR_PRINT_SCU for test with CTN)

    PDU.SetLocalAddress((BYTE *)MyAE);
    PDU.SetRemoteAddress((BYTE *)RemoteAE);

	if(!PDU.Connect((BYTE *)RemoteHost, (BYTE *)RemotePort))
		{
//		Dump((AAssociateRQ)PDU);	// LJ: Crashes when PDU is destructed
//		Dump((AAssociateAC)PDU);	// LJ: Crashes when PDU is destructed
		
//		cerr << "Connect failed" << endl;
		return 1;
		}
	PDU.FixPolaroidConnection ();
//	cerr << "Connected!" << endl;

	// add the abstract syntaxes making up the
	// BasicGrayscalePrintManagementMetaSOPClass
	// as aliases so that PDU can find a presentation context to use for them

    PDU.AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
		aBasicFilmSessionSOPClassUID);
    PDU.AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
		aBasicFilmBoxSOPClassUID);
    PDU.AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
		aBasicGrayscaleImageBoxSOPClassUID);
    PDU.AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
		aBasicPrinterSOPClassUID);
	PDU.FixPolaroidConnection ();
 
	// set an argument for event handler callbacks to use

//	aSession.SetPrinterEventUserArg((void *)&aRTC);
//	aSession.SetPrintJobEventUserArg((void *)&aRTC);
	aSession.SetPrinterEventUserArg((void *)&VRType);
	aSession.SetPrintJobEventUserArg((void *)&VRType);

	// attempt to get printer status

	DICOMDataObject aStatusDDO;

/*
	cerr << "Request printer status:" << endl;
	if(!aSession.GetPrinter(&PDU, &aStatusDDO))
		{
		cerr << "GetPrinter() failed." << endl;
		return ( FALSE );
		
		}

	cerr << "Got printer status!" << endl;
	DumpDDO(&aStatusDDO, 0, &aRTC);
	cerr << "Got printer status!" << endl << endl;
*/

	// all supposedly optional parameters

	VR *aVR;

	char *aString = "1";
	aVR = new VR(DC3TagFromName(NumberOfCopies),
		strlen(aString), (void *)aString, FALSE);
    aSession.Push(aVR);

	aString = "MED";
	aVR = new VR(DC3TagFromName(PrintPriority),
		strlen(aString), (void *)aString, FALSE);
    aSession.Push(aVR);

//	aString = "PAPER";
	aString = "BLUE FILM";
	aVR = new VR(DC3TagFromName(MediumType), 
        strlen(aString), (void *)aString, FALSE);
    aSession.Push(aVR);

	aString = "MAGAZINE";
	aVR = new VR(DC3TagFromName(FilmDestination), 
        strlen(aString), (void *)aString, FALSE);
    aSession.Push(aVR);

	aString = "This Is A Test";
	aVR = new VR(DC3TagFromName(FilmSessionLabel), 
        strlen(aString), (void *)aString, FALSE);
    aSession.Push(aVR);

	// optional MemoryAllocation (not set)

	// Film box parameters:
	// Image display format STANDARD\2,1
	// FilmOrientation = PORTRAIT
	// FilmSizeID 8INX10IN
	// MagnificationType = REPLICATE
	// BorderDensity = BLACK
	// EmptyImageDensity = BLACK
	// print management object handles reference sequences for us...


	BOOL aCollationFlag = COLLATED;
	UINT aFilmBoxIndex=0;

	UINT	ImageIndex = 0;
	UINT	ImagesOnASheet = ImagesPerRow * ImagesPerColumn;
		
	while ( ImageIndex < ADDO->GetSize() )
		{
		// for each film
		// create a new film box

		AbstractFilmBox *aFilmBox = new AbstractFilmBox(aSession);


		// set film box'es attributes
		// when doing VR's with multiple values, don't forget
		// to escape the \ or you won't have one in your string...

		char	TempString[256];
		sprintf(TempString, "%s\\%d,%d",
			FilmFormat, ImagesPerRow, ImagesPerColumn);
			
//		aString = "STANDARD\\1,1";
		aVR = new VR(DC3TagFromName(ImageDisplayFormat), 
        	strlen(TempString), (BOOL)TRUE);
		memcpy((void*)aVR->Data, (void*)TempString, strlen(TempString));
    	aFilmBox->Push(aVR);

		aString = "PORTRAIT";
		aVR = new VR(DC3TagFromName(FilmOrientation), 
        	strlen(aString), (void *)aString, FALSE);
    	aFilmBox->Push(aVR);

		aString = "14INX17IN";
		aVR = new VR(DC3TagFromName(FilmSizeID), 
        	strlen(aString), (void *)aString, FALSE);
    	aFilmBox->Push(aVR);

//		aString = "NONE";
//		aString = "REPLICATE";
		aString = "BILINEAR";
//		aString = "CUBIC";
		aVR = new VR(DC3TagFromName(MagnificationType), 
        	strlen(aString), (void *)aString, FALSE);
    	aFilmBox->Push(aVR);

/*		aString = "MAYBE";
		aVR = new VR(0x2010, 0x0140,
			strlen(aString), (void*)aString, FALSE);
	aFilmBox->Push(aVR);
*/	

		aString = "BLACK";
		aVR = new VR(DC3TagFromName(BorderDensity), 
        	strlen(aString), (void *)aString, FALSE);
    	aFilmBox->Push(aVR);

		aString = "BLACK";
		aVR = new VR(DC3TagFromName(EmptyImageDensity), 
        	strlen(aString), (void *)aString, FALSE);
    	aFilmBox->Push(aVR);

/*		aString = "COMBINED";
		aVR = new VR(DC3TagFromName(AnnotationDisplayFormatID), 
        	strlen(aString), (void *)aString, FALSE);
    	aFilmBox->Push(aVR);
*/

		// the combined annotation style means that
		// annotation box at position 0 is a film label,
		// and annotation boxes at positions corresponding to
		// image positions are image labels -- this is printer
		// dependent and specified in the conformance statement...

		// add a film label

		// for each image on a film create and add a new image box

		UINT aImageBoxIndex=0;
/*
		AbstractAnnotationBox *aAnnotationBox =
			new AbstractAnnotationBox(aSession);
		
		LE_UINT16 *aNumber = new LE_UINT16((UINT16)(0));
		aVR = new VR(DC3TagFromName(AnnotationPosition), 
        	sizeof(*aNumber), (void *)aNumber, TRUE);
    	aAnnotationBox->Push(aVR);
		*/
	
/*		aString = "Sample Output";
		aVR = new VR(DC3TagFromName(TextString), 
        	strlen(AnnotationText)+1, (BOOL) TRUE );
		strcpy((char*)aVR->Data, AnnotationText);
    	aAnnotationBox->Push(aVR);

		aFilmBox->Add(aAnnotationBox);
		*/

		/*
		AbstractImageBox *aImageBox = new AbstractImageBox(aSession);

		LE_UINT16 *aNumber = new LE_UINT16((UINT16)(aImageBoxIndex + 1));
		aVR = new VR(DC3TagFromName(ImageBoxPosition), 
       		sizeof(*aNumber), (void *)aNumber, TRUE);
   		aImageBox->Push(aVR);

		// create the PreformattedGrayscaleImageSequence attribute
			
		aVR = new VR(DC3TagFromName(PreformattedGrayscaleImageSequence), 
       		0, FALSE);
   		aImageBox->Push(aVR);
		*/

		UINT	ImageOnFilmIndex = 0;
		while ( (ImageIndex < ADDO->GetSize()) && 
				(ImageOnFilmIndex < ImagesOnASheet ))
			{
			DICOMDataObject	*DDO = ADDO->Get(ImageIndex);
			ADDO->Get(ImageIndex) = NULL;
			StripDDOForPrinting (DDO);

// from dicomprn
			AbstractImageBox *aImageBox = new AbstractImageBox(aSession);
			LE_UINT16	*aNumber = new LE_UINT16((UINT16)(ImageOnFilmIndex + 1));
			aVR = new VR(DC3TagFromName(ImageBoxPosition), 
	       		sizeof(*aNumber), (void *)aNumber, TRUE);
   			aImageBox->Push(aVR);

			// create the PreformattedGrayscaleImageSequence attribute
			aVR = new VR(DC3TagFromName(PreformattedGrayscaleImageSequence), 
       			0, FALSE);
	   		aImageBox->Push(aVR);
// end from dicomprn

   			aImageBox->Push(DDO);
			++ImageIndex;
			++ImageOnFilmIndex;

			aFilmBox->Add(aImageBox);

			}
			
//		aFilmBox->Add(aImageBox);

		/*
		aAnnotationBox = new AbstractAnnotationBox(aSession);
		
		aNumber = new LE_UINT16((UINT16)(aImageBoxIndex + 1));
		aVR = new VR(DC3TagFromName(AnnotationPosition), 
       		sizeof(*aNumber), (void *)aNumber, TRUE);
   		aAnnotationBox->Push(aVR);

		aString = "Copyright 1996, UCDMC Radiology/MSU Radiology";
		aVR = new VR(DC3TagFromName(TextString), 
       		strlen(aString), (void *)aString, FALSE);
   		aAnnotationBox->Push(aVR);

		aFilmBox->Add(aAnnotationBox);
		*/

		// add the film box to the current session

		aSession.Add(aFilmBox);

		// print film now and delete if NO collation...

		if(!aCollationFlag)
			{
			UID aPrintJobUID("");

			if(!aSession.PrintFilm(&PDU, &aPrintJobUID))
				{
//				cerr << "PrintFilm(" <<  aFilmBoxIndex << ") failed." << endl;
				return 1;
				}
//			cerr << "Film " << aFilmBoxIndex << " Printed!" << endl;
//			cerr << "Print Job UID is " << (char *)aPrintJobUID.GetBuffer(1) << endl;

#if 0
			// look for an event report

			DICOMCommandObject aDCO;

			if(PDU.Read(&aDCO))
				{
				cerr << "Read something from association." << endl;
				}
			else
				{
				cerr << "PDU.Read() failed!" << endl;
				}

			if(aSession.ReadEventReport(&PDU, &aDCO))
				{
				cerr << "Received an event!" << endl;
				}
			else
				{
				cerr << "ReadEventReport failed!" << endl;
				}
#endif

			// do a Get on the print job...

// mvh causes problem when working with MIR print_server
#if 0
			DICOMDataObject aPrintJobDDO;

			aString = "";
			aVR = new VR(DC3TagFromName(ExecutionStatus), 
        		strlen(aString), (void *)aString, FALSE);
    		aPrintJobDDO.Push(aVR);
			aString = "";
			aVR = new VR(DC3TagFromName(ExecutionStatusInfo), 
        		strlen(aString), (void *)aString, FALSE);
    		aPrintJobDDO.Push(aVR);
			aString = "";
			aVR = new VR(DC3TagFromName(PrintPriority), 
        		strlen(aString), (void *)aString, FALSE);
    		aPrintJobDDO.Push(aVR);
			aString = "";
			aVR = new VR(DC3TagFromName(CreationDate), 
        		strlen(aString), (void *)aString, FALSE);
    		aPrintJobDDO.Push(aVR);
			aString = "";
			aVR = new VR(DC3TagFromName(CreationTime), 
        		strlen(aString), (void *)aString, FALSE);
    		aPrintJobDDO.Push(aVR);
			aString = "";
			aVR = new VR(DC3TagFromName(PrinterName), 
        		strlen(aString), (void *)aString, FALSE);
    		aPrintJobDDO.Push(aVR);
			aString = "";
			aVR = new VR(DC3TagFromName(Originator), 
        		strlen(aString), (void *)aString, FALSE);
    		aPrintJobDDO.Push(aVR);

			while( TRUE )
				{
				cerr << "Request print job status:" << endl;
				if ( aSession.GetPrintJob( &PDU, aPrintJobUID, &aPrintJobDDO ) )
					{
					cerr << "Got print job status!" << endl;
					DumpDDO(&aPrintJobDDO, 0, &aRTC);
					}
				else
					{
					cerr << "Can't get print job status!" << endl;
					break;
					}
				cerr << endl;

				aVR = aPrintJobDDO.GetVR(DC3TagFromName(ExecutionStatus));

				if( !aVR || !aVR->Data ||
					(strncmp((const char *)aVR->Data,
						"PENDING", strlen("PENDING")) &&
					strncmp((const char *)aVR->Data, "PRINTING",
						strlen("PRINTING"))))
					{
					break;
					}
#ifdef	WINDOWS
				Sleep(5000);
#else
				sleep(5);
#endif
				aPrintJobDDO.Reset();
				}
#endif


// mvh: deletes the MIR file from disk
#if 0
			if(!aSession.DeleteFilm(&PDU))
				{
				cerr << "DeleteFilm(" <<  aFilmBoxIndex << ") failed." << endl;
				return ( FALSE );
				}
			cerr << "Film " << aFilmBoxIndex << " Deleted!" << endl;
#endif
			}
		++aFilmBoxIndex;
		}

	// print the entire session if collation was requested

	if(aCollationFlag)
		{
		UID aPrintJobUID("");

		if(!aSession.PrintSession(&PDU))
			{
//			cerr << "PrintSession() failed." << endl;
			return 1;
			}

//		cerr << "Film session printed!" << endl;
//		cerr << "Print Job UID is " << (char *)aPrintJobUID.GetBuffer(1) << endl;

		// do a Get on the print job...

		DICOMDataObject aPrintJobDDO;
		aString = "";
		aVR = new VR(DC3TagFromName(ExecutionStatus), 
        	strlen(aString), (void *)aString, FALSE);
    	aPrintJobDDO.Push(aVR);
		aString = "";
		aVR = new VR(DC3TagFromName(ExecutionStatusInfo), 
        	strlen(aString), (void *)aString, FALSE);
    	aPrintJobDDO.Push(aVR);
		aString = "";
		aVR = new VR(DC3TagFromName(PrintPriority), 
        	strlen(aString), (void *)aString, FALSE);
    	aPrintJobDDO.Push(aVR);
		aString = "";
		aVR = new VR(DC3TagFromName(CreationDate), 
        	strlen(aString), (void *)aString, FALSE);
    	aPrintJobDDO.Push(aVR);
		aString = "";
		aVR = new VR(DC3TagFromName(CreationTime), 
        	strlen(aString), (void *)aString, FALSE);
    	aPrintJobDDO.Push(aVR);
		aString = "";
		aVR = new VR(DC3TagFromName(PrinterName), 
        	strlen(aString), (void *)aString, FALSE);
    	aPrintJobDDO.Push(aVR);
		aString = "";
		aVR = new VR(DC3TagFromName(Originator), 
        	strlen(aString), (void *)aString, FALSE);
    	aPrintJobDDO.Push(aVR);

//		cerr << "Request print job status:" << endl;
		if ( aSession.GetPrintJob( &PDU, aPrintJobUID, &aPrintJobDDO ) )
			{
//			cerr << "Got print job status!" << endl;
//			DumpDDO(&aPrintJobDDO, 0, &aRTC);
			DumpDDO(&aPrintJobDDO, 0, &VRType);
			}
		else
			{
//			cerr << "Can't get print job status!" << endl;
			}
//		cerr << endl;
		}

	// delete the film session

// mvh: deletes the MIR file from disk
#if 0
	if(!aSession.Delete(&PDU))
		{
		cerr << "aSession.Delete() failed." << endl;
		return ( FALSE );
		}
	cerr << "Film session deleted!" << endl;
#endif

	// terminate the association

	PDU.Close();

	return 0;
	}

static int PrintSingleGrayScaleImage (
	BYTE	*MyAE,
	BYTE	*RemoteAE,
	BYTE	*RemoteHost,
	BYTE	*RemotePort,
	DICOMDataObject	*DDO,
	char	*AnnotationText)
	{
	Array<DICOMDataObject	*>	ADDO;
	
	ADDO.Add(DDO);
	return ( PrintGrayScaleImages (
		MyAE,
		RemoteAE,
		RemoteHost,
		RemotePort,
		&ADDO,
		"STANDARD",
		1,
		1,
		AnnotationText));
	}

int main (
	int	argc,
	char	*argv[])
	{

	// start of printer server code

	if (argc == 2)
		{
		MyGrayscalePrintManagement 	aSession;  
		PDU_Service			PDU;
		DICOMCommandObject		DCO;

		UID	aAppuid("1.2.840.10008.3.1.1.1");
		UID	aBasicGrayscalePrintManagementMetaSOPClass("1.2.840.10008.5.1.1.9");
		UID	aBasicFilmSessionSOPClassUID("1.2.840.10008.5.1.1.1");
		UID	aBasicFilmBoxSOPClassUID("1.2.840.10008.5.1.1.2");
		UID	aBasicGrayscaleImageBoxSOPClassUID("1.2.840.10008.5.1.1.4");
		UID	aPrintJobSOPClassUID("1.2.840.10008.5.1.1.14");
		UID	aBasicPrinterSOPClassUID("1.2.840.10008.5.1.1.16");
		UID	aBasicAnnotationBoxSOPClassUID("1.2.840.10008.5.1.1.15");
		UID	uid;

		PDU.SetApplicationContext(aAppuid);
		PDU.AddAbstractSyntax(aBasicGrayscalePrintManagementMetaSOPClass);


		while(1)
			{
			PDU.Listen((BYTE *)argv[1]);

			PDU.AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
				aBasicFilmSessionSOPClassUID);
			PDU.AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
				aBasicFilmBoxSOPClassUID);
			PDU.AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
				aBasicGrayscaleImageBoxSOPClassUID);
			PDU.AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
				aBasicPrinterSOPClassUID);

			while (1)
				{
				PDU.Read(&DCO);
				aSession.Read(&PDU, &DCO);
				}
			}
		}

	if ( argc != 6 )
		{
		printf("Error: usage: dicomp myae remoteae host port filename\nor: dicomp port\n");
		return(1);
		}
	PDU_Service	PDU;
	
	DICOMDataObject	*DDO = PDU.LoadDICOMDataObject(argv[5]);
	if ( ! DDO )
		{
		printf("Error loading image file: %s\n", argv[5]);
		return(2);
		}
	if ( PrintSingleGrayScaleImage (
			(BYTE*)argv[1],
			(BYTE*)argv[2],
			(BYTE*)argv[3],
			(BYTE*)argv[4],
			DDO, "Sample Output" ) )
		{
		printf("Error printing file\n");
		return(3);
		}
	return(0);
	}
