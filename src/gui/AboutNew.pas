{************************************************************************}
{*                                                                      *}
{*      file       : AboutNew.PAS                                       *}
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
20000625        mvh     Added NewInstallForm database selector; default to MS ACCESS
20010418        mvh     Changed text; added default install button
20010830        mvh     Version to 1.3.10
20011127        mvh     Version to 1.3.11
20020816        mvh     Version to 1.3.12
20021016        mvh     Added DBF without ODBC
20021017        mvh     Version to 1.4.0
20021018        mvh     Default to USEDBASEIIIWITHOUTODBC
20030324        mvh     Version to 1.4.2
20030810        mvh     Version to 1.4.3 - removed ODBC DBASEIII choice
20040102        mvh     Version to 1.4.4
20040426        mvh     Version to 1.4.4a
20040530        mvh     Version to 1.4.5
20040713        mvh     Version to 1.4.6
20041029        mvh     Version to 1.4.7
20050106        mvh     Version to 1.4.7a
20050901        mvh     Version to 1.4.9
20051024        mvh     Version to 1.4.9a
20051230        mvh     Version to 1.4.10
20060312        mvh     Version to 1.4.11
20060313        mvh     Added experimental native Mysql install when libmysql.dll is found
20060618        mvh     Version to 1.4.12
20070126        mvh     Version to 1.4.12b
20070201        mvh     Version to 1.4.12c
20070305        mvh     Version to 1.4.13alpha
20070305        mvh     Added experimental (not fully functional) native SqLite install when sqlite3.dll is found
20070706        mvh     Version to 1.4.13beta; sqlite always enabled (driver linked in)
20071103        mvh     Version to 1.4.13
20080322        mvh     Version to 1.4.14beta
20080819        mvh     version to 1.4.14
20081121        mvh     Added postgres; mysql unconditional; version to 1.4.14a
20090411        mvh     version to 1.4.15alpha
20090427        mvh     version to 1.4.15
20100202        mvh     Default to SQLITE; version to 1.4.16alpha
20100207        mvh     version comes from define in the serverdriver
20160318        mvh     Added UoM to labels
20260914        mvh     Asks libpq.dll/libmysql.dll location and copy drivers
20260915        mvh     Added update button to update existing conquest
20260916        mvh     Make empty folders such that is can install a minimal version
*}

unit AboutNew;

interface

uses Windows, SysUtils, Classes, Graphics, Forms, Controls, StdCtrls,
  Buttons, ExtCtrls, ServerDriver, Dialogs;

type
  TNewInstallBox = class(TForm)
    Panel1: TPanel;
    ProgramIcon: TImage;
    ProductName: TLabel;
    Version: TLabel;
    Copyright: TLabel;
    Comments: TLabel;
    OKButton: TButton;
    ComboBox1: TComboBox;
    Label1: TLabel;
    CancelButton: TButton;
    DefaultInstallButton: TButton;
    Label2: TLabel;
    Button1: TButton;
    procedure FormCreate(Sender: TObject);
    procedure OKButtonClick(Sender: TObject);
    procedure DefaultInstallButtonClick(Sender: TObject);
    procedure UpdateButtonClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  NewInstallBox: TNewInstallBox;

implementation

{$R *.DFM}

procedure TNewInstallBox.FormCreate(Sender: TObject);
var UseDBF, UseSQLServer, UseMSAccess, UseDBFWithoutODBC, UseMysql, UseSqLite, UsePostgres : boolean;
begin
  ComboBox1.Items.Add('Native MySQL driver');
  ComboBox1.Items.Add('Built-in SqLite driver');
  ComboBox1.Items.Add('Native Postgres driver');

  UseSQLServer := FileExists(curdir + '\USESQLSERVER');
  UseDBF       := FileExists(curdir + '\USEDBASEIII');
  UseMSAccess  := FileExists(curdir + '\USEMSACCESS');
  UseMysql     := FileExists(curdir + '\USEMYSQL');
  UseSqLite    := FileExists(curdir + '\USESQLITE');
  UsePostgres  := FileExists(curdir + '\USEPOSTGRES');
  UseDBFWithoutODBC  := FileExists(curdir + '\USEDBASEIIIWITHOUTODBC');

  if (not UseSQLServer) and (not UseDBF) and (not UseMSAccess) and (not UseMysql)
      and (not UseDBFWithoutODBC) and (not UseSQLite) and (not UsePostgres) then
    UseSqLite  := true;

  if UsePostgres            then ComboBox1.ItemIndex := 5
  else if UseSqLite         then ComboBox1.ItemIndex := 4
  else if UseMysql          then ComboBox1.ItemIndex := 3
  else if UseDBFWithoutODBC then ComboBox1.ItemIndex := 2
  else if UseSQLServer      then ComboBox1.ItemIndex := 1
  else if UseMSAccess       then ComboBox1.ItemIndex := 0;
end;

procedure TNewInstallBox.OKButtonClick(Sender: TObject);
var name, s, t: string;
    f: textfile;
    sr: TSearchRec;
begin
  if FileExists(curdir + '\USEPOSTGRES')  then DeleteFile(curdir + '\USEPOSTGRES');
  if FileExists(curdir + '\USESQLITE')    then DeleteFile(curdir + '\USESQLITE');
  if FileExists(curdir + '\USEMYSQL')     then DeleteFile(curdir + '\USEMYSQL');
  if FileExists(curdir + '\USESQLSERVER') then DeleteFile(curdir + '\USESQLSERVER');
  if FileExists(curdir + '\USEMSACCESS' ) then DeleteFile(curdir + '\USEMSACCESS' );
  if FileExists(curdir + '\USEDBASEIII' ) then DeleteFile(curdir + '\USEDBASEIII' );
  if FileExists(curdir + '\USEDBASEIIIWITHOUTODBC' ) then DeleteFile(curdir + '\USEDBASEIIIWITHOUTODBC' );

  case ComboBox1.ItemIndex of
    5: name := 'USEPOSTGRES';
    4: name := 'USESQLITE';
    3: name := 'USEMYSQL';
    2: name := 'USEDBASEIIIWITHOUTODBC';
    1: name := 'USESQLSERVER';
    0: name := 'USEMSACCESS';
  end;

  AssignFile(f, curdir + '\' + name);
  Rewrite(f);
  WriteLn(f, 'The name of this file determines the type of database');
  WriteLn(f, 'used for a new installation of the Conquest DICOM server.');
  CloseFile(f);

  if (name='USEPOSTGRES') and not FileExists(curdir+ '\Install64\libpq64.dll') then
  begin
    Form1.OpenDialog1.Title       := 'Postgres driver (e.g. libpq.dll in C:\Program Files\PostgreSQL\xx\bin)';
    Form1.OpenDialog1.Filename    := '*.dll';
    Form1.OpenDialog1.Filter      := '*.dll';
    Form1.OpenDialog1.FilterIndex := 1;
    Form1.OpenDialog1.Options    := [];
    if Form1.OpenDialog1.Execute then
    begin
      s := ExtractFileDir(Form1.OpenDialog1.Filename);
      t := StringReplace(Form1.OpenDialog1.Filename, 'libpq.', 'lib*.', []);
      if FindFirst(t,faAnyFile,sr) = 0 then
      repeat
        if (sr.Attr and faDirectory ) < $00000008 then
          CopyFile(PChar(s+'\'+sr.Name),PChar(Curdir+'\Install64\'+sr.Name), false);
      until findNext(sr) <> 0
    end;
  end;

  if (name='USEMYSQL') and not FileExists(curdir+ '\Install64\libpq64.dll') then
  begin
    Form1.OpenDialog1.Title       := 'Mysql driver (e.g. libmysql.dll or libmariadb.dll in C:\Program Files\xx\x.x\lib)';
    Form1.OpenDialog1.Filename    := '*.dll';
    Form1.OpenDialog1.Filter      := '*.dll';
    Form1.OpenDialog1.FilterIndex := 1;
    Form1.OpenDialog1.Options    := [];
    if Form1.OpenDialog1.Execute then
    begin
      s := ExtractFileDir(Form1.OpenDialog1.Filename);
      t := StringReplace(Form1.OpenDialog1.Filename, 'libmysql.', 'lib*.', []);
      if FindFirst(t,faAnyFile,sr) = 0 then
      repeat
        if (sr.Attr and faDirectory ) < $00000008 then
          CopyFile(PChar(s+'\'+sr.Name),PChar(Curdir+'\Install64\'+sr.Name), false);
      until findNext(sr) <> 0
    end;
  end;
end;

procedure TNewInstallBox.UpdateButtonClick(Sender: TObject);
var s: string;
    sr: TSearchRec;
    a: integer;
begin
  if MessageDlg('Update/create other conquest folder with executable files from this one ?', mtConfirmation,
    [mbYes, mbNo], 0) = mrNo then Exit;

  Form1.OpenDialog1.Title       := 'Select any file in server folder to update or create';
  Form1.OpenDialog1.Filename    := '*.*';
  Form1.OpenDialog1.Filter      := '*.';
  Form1.OpenDialog1.FilterIndex := 1;
  Form1.OpenDialog1.Options    := [];
  if Form1.OpenDialog1.Execute then
  begin
    s := ExtractFileDir(Form1.OpenDialog1.Filename);

    if MessageDlg('This will overwrite exe, dll, php, and lua files in: '+s+'. Are you sure?', mtConfirmation,
       [mbYes, mbNo], 0) = mrNo then Exit;

    if MessageDlg('is the conquest instance in '+s+' is turned off?', mtConfirmation,
       [mbYes, mbNo], 0) = mrNo then Exit;

    try
      // minimal exes
      CopyFile(PChar(Curdir+'\7za.exe'),               PChar(s+'\7za.exe'), false);
      CopyFile(PChar(Curdir+'\ConquestDICOMServer.exe'), PChar(s+'\ConquestDICOMServer.exe'), false);
      CopyFile(PChar(Curdir+'\CqDicom.dll'),           PChar(s+'\CqDicom.dll'), false);
      CopyFile(PChar(Curdir+'\dgate.dic'),             PChar(s+'\dgate.dic'), false);
      CopyFile(PChar(Curdir+'\DgateServ.exe'),         PChar(s+'\DgateServ.exe'), false);

      // assume 64 bits
      CopyFile(PChar(Curdir+'\Install64\dgate64.exe'), PChar(s+'\dgate64.exe'), false);
      CopyFile(PChar(Curdir+'\Install64\lua5.1.dll'),  PChar(s+'\lua5.1.dll'), false);

      // empty data
      ForceDirectories(s+'\data');
      ForceDirectories(s+'\data\dbase');

      // install32 and 64 folders
      ForceDirectories(s+'\Install64');
      CopyFile(PChar(Curdir+'\Install64\dgate64.exe'), PChar(s+'\Install64\dgate64.exe'), false);
      CopyFile(PChar(Curdir+'\Install64\lua5.1.dll'),  PChar(s+'\Install64\lua5.1.dll'), false);

      ForceDirectories(s+'\Install32');
      CopyFile(PChar(Curdir+'\Install32\dgate.exe'), PChar(s+'\Install32\dgate.exe'), false);
      CopyFile(PChar(Curdir+'\Install32\lua5.1.dll'),  PChar(s+'\Install32\lua5.1.dll'), false);

      // clibs
      ForceDirectories(s+'\clibs');
      ForceDirectories(s+'\clibs\socket');
      CopyFile(PChar(Curdir+'\clibs\iup.dll'),         PChar(s+'\clibs\iup.dll'), false);
      CopyFile(PChar(Curdir+'\clibs\iuplua51.dll'),    PChar(s+'\clibs\iuplua51.dll'), false);
      CopyFile(PChar(Curdir+'\clibs\socket\core.dll'), PChar(s+'\clibs\socket\core.dll'), false);

      // lua
      ForceDirectories(s+'\lua');
      if FindFirst(Curdir+'\lua\*.lua',faAnyFile,sr) = 0 then
      repeat
        if (sr.Attr and faDirectory ) < $00000008 then
          CopyFile(PChar(Curdir+'\lua\'+sr.Name),PChar(s+'\lua\'+sr.Name), false);
      until findNext(sr) <> 0;

      // webserver
      ForceDirectories(s+'\webserver\htdocs');
      if FindFirst(Curdir+'\webserver\htdocs\*.*',faAnyFile,sr) = 0 then
      repeat
        if (sr.Attr and faDirectory ) < $00000008 then
          CopyFile(PChar(Curdir+'\webserver\htdocs\'+sr.Name),PChar(s+'\webserver\htdocs\'+sr.Name), false);
      until findNext(sr) <> 0;

      ForceDirectories(s+'\webserver\htdocs\api\dicom');
      if FindFirst(Curdir+'\webserver\htdocs\api\dicom\*.*',faAnyFile,sr) = 0 then
      repeat
        if (sr.Attr and faDirectory ) < $00000008 then
          CopyFile(PChar(Curdir+'\webserver\htdocs\api\dicom\'+sr.Name),PChar(s+'\webserver\htdocs\api\dicom\'+sr.Name), false);
      until findNext(sr) <> 0;

      ForceDirectories(s+'\webserver\htdocs\app\newweb');
      if FindFirst(Curdir+'\webserver\htdocs\app\newweb\*.*',faAnyFile,sr) = 0 then
      repeat
        if (sr.Attr and faDirectory ) < $00000008 then
          CopyFile(PChar(Curdir+'\webserver\htdocs\app\newweb\'+sr.Name),PChar(s+'\webserver\htdocs\app\newweb\'+sr.Name), false);
      until findNext(sr) <> 0;

      ForceDirectories(s+'\webserver\htdocs\app\ohif');
      if FindFirst(Curdir+'\webserver\htdocs\app\ohif\*.*',faAnyFile,sr) = 0 then
      repeat
        if (sr.Attr and faDirectory ) < $00000008 then
          CopyFile(PChar(Curdir+'\webserver\htdocs\app\ohif\'+sr.Name),PChar(s+'\webserver\htdocs\app\ohif\'+sr.Name), false);
      until findNext(sr) <> 0;
    except
    end;
    ShowMessage('Done copying files into: '+s);
  end;
end;

procedure TNewInstallBox.DefaultInstallButtonClick(Sender: TObject);
begin
  OKButtonClick(self);
end;

end.

