{************************************************************************}
{*                                                                      *}
{*      file       : ServerAbout.PAS                                    *}
{*                                                                      *}
{*      type       : MAINFORM                                           *}
{*                                                                      *}
{*      location   : \QUIRT\APP\CONQUEST\DICOMSERVER                    *}
{*                                                                      *}
{*      purpose    : Dicom server for ConQuest based on Davis Dicom code*}
{*                                                                      *}
{*      author     : Marcel van Herk (NKI / AVL)                        *}
{*                                                                      *}
{*      date       : 19980405                                           *}
{*                                                                      *}
{*      portability: 32 bit delphi only (V3.0 up)                       *}
{*                                                                      *}
{*      notes      : None                                               *}
{*                                                                      *}
{************************************************************************}
{* Updates:
When            Who     What
19980405        mvh     Created
19980618        mvh     Added reference to Fpiette; use form also as file lister
19980624        mvh     Removed line about backdoor
19980721        mvh     Removed line about Mark Oskin going away
19990527        mvh     Release to 1.3.3
20000128        mvh     Release to 1.3.4
20000316        mvh     Release to 1.3.5
20000529        mvh     Release to 1.3.6
20001129        mvh     Release to 1.3.7
20010318        mvh     Release to 1.3.8
20010318        mvh     Release to 1.3.9; fixed dbaseIII text
20010830        mvh     Release to 1.3.10;
20011127        mvh     Release to 1.3.11;
20020816        mvh     Release to 1.3.12;
20020825        mvh     Made Lambert's e-mail address more prominent
20021018        mvh     Version to 1.4.0; added release date
20021020        mvh     Update release date
20021028        mvh     Update release date
20021215        mvh     Update release date
20030113        mvh     Version to 1.4.1; release date
20030128        mvh     new release date
20030303        mvh     new release date
20030710        mvh     Version to 1.4.2; release date
20030809        mvh     Version to 1.4.3
20030810        mvh     Added reference to offis, zipmaster, and mitec
20030922        mvh     Release date
20040402        mvh     Version to 1.4.4
20040406        mvh     Release date
20040426        mvh     Version to 1.4.4a; Release date
20040615        mvh     Version to 1.4.5; Release date
20040722        mvh     Version to 1.4.6; Release date
20040805        mvh     Version to 1.4.7; Release date
20041129        mvh     Release date
20050106        mvh     Version to 1.4.7a; Release date
20050109        mvh     Version to 1.4.8; Release date
20050303        mvh     Release date
20050912        mvh     Version to 1.4.9; Release date; worklist info
20051024        mvh     Version to 1.4.9a; Release date
20060103        mvh     Version to 1.4.10; Release date
20060314        mvh     Version to 1.4.11; Release date; acknowledge new components
20060402        mvh     Changed fpiette's website; Release date
20060708        mvh     Version to 1.4.12alpha; Release date
20061222	mvh	Version to 1.4.12; release date
20070126	mvh	Version to 1.4.12b; release date
20070201	mvh	Version to 1.4.12c; release date
20070305	mvh	Version to 1.4.13alpha; release date
20070706	mvh	Version to 1.4.13beta; release date; sqlite reference
20071116	mvh	Version to 1.4.13; release date
20080322	mvh	Version to 1.4.14beta; release date
20080902        mvh     version to 1.4.14; release date
20081121        mvh     version to 1.4.14a; release date
20090411        mvh     version to 1.4.15alpha
20090929        mvh     version to 1.4.15
20100202        mvh     version to 1.4.16alpha
20100207        mvh     version comes from define in the serverdriver
20160313        mvh     Update
*}

{************************************************************************}
{*                          START CODE AND USES                         *}
{************************************************************************}

unit ServerAbout;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, Serverdriver;

{************************************************************************}
{*                               TYPES                                  *}
{************************************************************************}

type
  TForm2 = class(TForm)
    Memo1: TMemo;
    procedure FormCreate(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form2: TForm2;

implementation

{************************************************************************}
{*                             MODULE FUNCTIONS                         *}
{************************************************************************}

{$R *.DFM}

procedure TForm2.FormCreate(Sender: TObject);
begin

Memo1.Lines.Text :=

'This is the ConQuest DICOM server version '+VERSION+'. Date of this release: '+BUILDDATE+'.' +
'This server has been developed within '+
'the E.U. ConQuest project as a small image archive.'+
'In addition, it can maintain a modality worklist that can be loaded with HL7 data '+
'and queried by the modalities to load patient demographic information. On reception ' +
'of images by the server, the demographic information can be reconsiliated with the '+
'data from the worklist.' + #13 +
#13 +
'The DICOM server is an adapted version of the University of California at Davis '+
'DICOM software (Personal PACS). Information about that software follows. '+
'In addition, we have used the excellent Delphi TCP/IP components from François Piette '+
'(http://www.overbyte.be), '+
'the MiTeC DBFTable by MichaL MutL, '+
'the EZDicom viewer by Chris Rorden (chris.rorden@nottingham.ac.uk) modified by'+
' Andreas Knopke (andreas@k-pacs.de), the fine Lua language for scripting, '+
'the sqlite database by Richard Hipp (http://www.sqlite.org/index.html) '+
'and JPEG (JpegLS and Jpeg2000) compression and decompression tools from Bruce Barton. ' + #13 +
#13 +
'The user interface was developed at the Radiotherapy Department of the Netherlands '+
'Cancer Institute (NKI) and is now maintained by Marcel van Herk at the University of Manchester.'+
'We have extended the original Davis code significantly. '+
'Contact Marcel van Herk (vanherkmarcel@gmail.com) for any questions '+
'or bug reports of the ConQuest DICOM server.' + #13 +
#13 +
'The server talks DICOM on one side (the front door) and stores image data in '+
'files in a user specified directory. The image files are stored either in ACRNEMA2.0 '+
'format (i.e., a raw VR dump), or preferable in chapter 10 DICOM format (with optional JPEG/JPEGLs/JPEG2000 compression).' + #13 +
#13 +
'The description of these images is stored '+
'in a SQLite or DBASEIII database formats using a built-in drivers or using other types of databases '+
'through a an ODBC interface or a native MySql or Postgres interface.'+ #13 +
#13+
'For information at University of Manchester:'+#13+
'Marcel van Herk'+#13+
'vanherkmarcel@gmail.com'+#13+
'-----------------------------------------------------------------'+#13+
#13+
'uPACS is the Windows NT PACS system.  It supports all storage, and query '+
'classes as a provider and all storage classes as a user.  It is also a '+
'verification provider. Terry Rosenbaum at MSU did a Solaris port of this. The '+
'same source compiles for both.  Much of the quality of the uPACS (i.e. bug '+
'fixes) are due to his efforts.'+#13+
#13 +
'Copy Right information'+#13+
#13+
'Copyright (c) 1995,96,97 Regents of the University of California.'+#13+
'All rights reserved.'+#13+
#13+
'Redistribution and use in source and binary forms are permitted '+
'provided that the above copyright notice and this paragraph are '+
'duplicated in all such forms and that any documentation, '+
'advertising materials, and other materials related to such '+
'distribution and use acknowledge that the software was developed '+
'by the University of California, Davis.  The name of the '+
'University may not be used to endorse or promote products derived '+
'from this software without specific prior written permission.'+#13+
'THIS SOFTWARE IS PROVIDED ''AS IS'' AND WITHOUT ANY EXPRESS OR '+
'IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED '+
'WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.'+#13+
#13+
#13+
'-'+#13+
'Human interpretation of the above:'+#13+
''+#13+
'You may use this work in publicly available releases, if you include '+
'a copy of the copy right above and acknowledge the origonal author.'+#13+
#13+
'You may use this work in commericial software, if you include '+
'a copy of the copy right above and acknowledge the origonal author.'+#13+
#13+
'You MAY NOT use the fact that your work was derived from this '+
'work as an advertising point (if it''s for commercial use, I can''t '+
'see why you would want to...).  Another words, no cute jingles with '+
'rymes about UCDMC in them.'+#13+
#13+
'You MAY redistribute this work in any form if you include the '+
'copy right and acknowledge the origonal author.'+#13+
'-----------------------------------------------------------------'+#13+
#13+
'Purpose of this release:'+#13+
#13+
'DICOM image viewers on PC''s should be free. Come on imaging vendors! If you '+
'want to sell your modalities in a competitive bid, give away a free network '+
'DICOM viewer.  Just repackage NewView and write an install program.  If you want '+
'to make money on the viewer, then sell support contracts.  Otherwise, stick to '+
'making good modalities with good network connectivity, that'' where the money '+
'is.'+#13+
#13+
'If your a University, then hopefully you can coax some of your programmers to '+
'use this source as a base and role your own viewer.  Put a DICOM network viewer '+
'on every desk.  Turn your departmental WindowsNT server into a PACS server.'+#13+
#13+
'The uPACS is in production use at UCDMC.  It has been for two years now.  It is '+
'very stable on WindowsNT.  It makes a great image archive when coupled with '+
'Microsoft SQL.  Great for research purposes.  Personal PACS is ideal for use on '+
'doctors desks.  This way they can have Windows95 and their own digital teaching '+
'file.'+#13+
#13+
'Thus the purpose of this release is to spawn the production of viewers and PACS '+
'for free use throughout the Internet.'+#13+
#13+
'-----------------------------------------------------------------'+#13+
#13+
'Anyway, you can contact me (Mark Oskin) at mhoskin@ucdavis.edu.  Skip Kennedy '+
'can be contacted at rlkennedy@ucdavis.edu.  Please don'' send me questions about '+
'installation or compilation of the stuff on this CD.  Please direct those to '+
'ucdmc-dicom@netrad.ucdmc.ucdavis.edu (join the list first, send mail to '+
'owner-ucdmc-dicom@netrad.ucdmc.ucdavis.edu). '+#13+
#13+
#13+
'Mark Oskin'+#13+
'mhoskin@ucdavis.edu';
//+#13+
//#13+
//'p.s.  Several of you probably are aware of this, but I am migrating out of the '+
//'DICOM/Medical Informatics field.  Lately, my interests are elsewhere (Computer '+
//'Architecture/Microchip design).  It has been fun working with you all over the '+
//'years.  Best of luck!';

end;

end.
