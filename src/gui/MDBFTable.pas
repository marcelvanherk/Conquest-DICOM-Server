{*******************************************************}
{                                                       }
{              MiTeC DBF Table Component                }
{                                                       }
{            version 1.5 for Delphi 5,6                 }
{                                                       }
{       Copyright © 1999,2002 Michal Mutl               }
{                                                       }
{*******************************************************}
// mvh 20030809: changed control page; added exception handling on CreateFields
// mvh 20030810: Added 'name*' partial key matching for filter
// mvh 20030810: Fixed huge leaks
// mvh 20040530: Fixed illegal memory access at CharToOemBuff
// mvh 20080215: Fixed empty buffer causing filter exceptions in case of no records
//               Open DBT file readonly
// mvh 20080325: Adapted for delphi 7
// lsp 20100907: Adapted to D2009
// lsp 20130730: Fixed Unicode issue in ProcessFilter(), respect
//               foCaseInsensitive and foNoPartialCompare
// lsp 20131020: Added matching *val*
// mvh 20150713: Fix in case insensitive matching for exact value
// mvh 20150719: Adapted for Delphi XE6 and XE8
// mvh 20160311: Fix ansichar / char mixup in locate

{$I MITEC_DEF.INC}

unit MDBFTable;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs, Db
{$ifndef VER130}
  ,variants
{$else}
  ,DsgnIntf
{$endif}
  ;

{$IFDEF D7PLUS}
{$WARN unsafe_type off}
{$WARN unsafe_cast off}
{$WARN unsafe_code off}
{$ENDIF D7PLUS}

const
   dBase_MaxRecCount   = 1000000000;
   dBase_MaxRecSize    = 4000;
   dBase_MaxFieldCount = 128;
   dBase_MaxFieldWidth = 255;
   dBase_MaxNumPrec    = 16;
   dBase_MaxMemoBytes  = 16384;
   dBase_MaxMemoRec    = 512;

   flgDeleted   = $2A;
   flgUndeleted = $20;
   flgEOF       = $1A;
   flgBOF       = $0D;

   err_ErrorCode = 'Error code: ';
   err_Warning='Warning';
   err_Stop='Error';
   err_BookMark='Cannot find bookmark';
   err_NoRecords='No records';
   err_InValidValue='Invalid value';
   err_ChangeFileName='Cannot change Filename if table is active';
   err_IncorrectDBF='Incorrect DBF file';
   err_AccessOutRange='Access out of range';
   err_WrongFieldDef='Wrong field definition';
   err_FileNotOpen='File not open';

   dBaseIIIPlus     = $03;
   dBaseIIIPlusMemo = $83;

type
  PBoolean = ^Boolean;

  PBuffer = PByte;
  TBufElt = Byte;

{$ifdef VER130}
  TFilenameProperty = class(TStringProperty)
  public
    procedure Edit; override;
    function GetAttributes: TPropertyAttributes; override;
  end;
{$endif}

  TDBFError = procedure(Sender: TObject;ErrorMsg:String) of object;

  TFieldName = string[10];

  TDBFBuffer = array[1..dBase_MaxRecSize] of Byte;

  TdBaseIIIPlus_Header = record
    Version,   //$03 without memo, $83 with memo
    Year,
    Month,
    Day :Byte;
    Recordcount :Longint;
    HeaderSize,
    RecordSize :Word;
    Reserved1 :array[1..3] of Byte;
    LANRsvd :array[1..13] of Byte;
    Reserved2 :array[1..4] of Byte;
  end;

  TdBaseIVPlus_Header = record
    Version,  { 01234567
                vvvmsssa
                |  ||  |
                |  ||  - presence of any memo file
                |  |- presence of an SQL table
                |  - presence of a dBASE IV memo
                - indicate version number}
    Year,
    Month,
    Day :Byte;
    Recordcount :Longint;
    HeaderSize,
    RecordSize :Word;
    Reserved1 :array[1..2] of Byte;   //filled with 0
    IncompleteTranstactionFlag,  //$01 transaction protected
    EncryptionFlag :byte;  //$01 encrypted
    MultiuserRsvd :array[1..12] of Byte;
    MDXFlag,   //$01 presence
    LangDrvID :byte;
    { 001 - cp 437
      002 - cp 850
      100 - cp 852
      102 - cp 865
      101 - cp 866
      104 - cp 895
      200 - cp 1250
      201 - cp 1251
      003 - cp 1252                      }
  end;

  TdBaseIIIPlus_Field = record
    Name :array[1..10] of AnsiChar;
    ClosingZero :byte;
    FieldType: AnsiChar;
    MemAddress :array[1..4] of Byte;
    Width,
    Decimals :Byte;
    LAN1 :array[1..2] of Byte;
    WorkAreaID :byte;
    LAN2 :array[1..2] of Byte;
    SetFields :byte;
    Rsvd :byte;
  end;

  TdBaseIVPlus_Field = record
    Name :array[1..10] of AnsiChar;
    ClosingZero :byte;
    FieldType: AnsiChar;
    Rsvd1 :array[1..4] of Byte;
    Width,
    Decimals :Byte;
    Rsvd2 :array[1..2] of Byte;
    WorkAreaID :byte;
    Rsvd3 :array[1..10] of Byte;
    Indexed: byte;
  end;

  TDBTHeader = record
    NextBlock :DWORD;
    BlockSize :DWORD;
    Reserved :array[1..504] of AnsiChar;
  end;

  PDBFField = ^TDBFField;
  TDBFField = record
    FieldName :TFieldname;
    FieldType :TFieldType;
    Size :word;
    Decimals :Byte;
    Offset :Word;
    Indexed :boolean;
  end;

  TDBFStructure = record
    FileName :string;
    Year, Month, Day :Byte;
    Version :byte;
    RecordCount, DeletedCount :LongInt;
    HeaderSize :Word;
    RecordSize :Word;
    MDXPresent :boolean;
    Encrypted,
    TransProt,
    Memo, AnyMemo :boolean;
    CodePage :word;
    FieldCount :Word;
    Data :TStringList;
    Fields :TList;
  end;

  PRecInfo = ^TRecInfo;
  TRecInfo = packed record
    BookMark: integer;
    UpdateStatus: TUpdateStatus;
    BookmarkFlag: TBookmarkFlag;
  end;

  TDBFAccess = class(TPersistent)
  private
    FFile :TFileStream;
    Fdb3Header :TdBaseIIIPlus_Header;
    Fdb4Header :TdBaseIVPlus_Header;
    Fdb3Field :TdBaseIIIPlus_Field;
    Fdb4Field :TdBaseIVPlus_Field;
    FDBF :TDBFStructure;
    FResult :integer;
    FOnDBFError:TDBFError;
    FPackOnSave: Boolean;
    FDB3 :boolean;
    function CheckRange(FileSize :longint) :integer;
    function CheckField(Field :TDBFField) :integer;
    procedure OpenFile(const Filename :string);
    procedure CreateFile(const Filename :string);
    procedure CloseFile;
    function ReadDBFHeader :integer;
    function ReadDBFFieldDefs :integer;
    function ReadDBFData :integer;
    function WriteDB3Header(HB :TdBaseIIIPlus_Header) :integer;
    function WriteDB4Header(HB :TdBaseIVPlus_Header) :integer;
    function WriteDBFFieldDefs(AFields :TList) :integer;
    function WriteDBFData(AFields :TList; AData :TStringList) :integer;
  protected
    procedure RaiseDBFError;
  public
    constructor Create;
    destructor Destroy; override;
    procedure LoadFromFile(const Filename :string);
    procedure SaveToFile(const Filename :string);
    property Structure :TDBFStructure read FDBF write FDBF;
    property LastError :integer read FResult write FResult;
    property OnDBFError:TDBFError read FOnDBFError write FOnDBFError;
    property PackOnSave :Boolean read FPackOnSave write FPackOnSave;
    property Dbase3 :boolean read FDB3 write FDB3;
  end;

  //PdBaseMemoRecord = ^TdBaseMemoRecord;
  //TdBaseMemoRecord = array [0..dBase_MaxMemoBytes] of byte;

  TMDBFTable = class(TDataset)
  private
    FAccess: TDBFAccess;
    FStructure :TDBFStructure;
    FData :tstringlist;
    FRecSize, FRecBufSize: Integer;
    FRecInfoOfs: Integer;
    FCurRec: Integer;
    FAbout, FFileName: string;
    FLastBookmark: Integer;
    FLastUpdate :string;
    FOnDBFError:TDBFError;
    FMakeBackup :Boolean;
    FShowDeleted: Boolean;
    FModified: Boolean;
    FVersion: string;
    FActiveFilter :boolean;
    FMemoFile :TFileStream;
    DBTHeader: TDBTHeader;
    FMemoFilename :string;
    function GetField(Index :integer) :TDBFField;
    procedure DBFError(Sender: TObject; ErrorMsg :string);
    function GetDeleted: Boolean;
    procedure SetShowDeleted(const Value: Boolean);
    procedure SetFilterActive(const Value: Boolean);
    function GetFilterActive: Boolean;
    function GetCodePage: word;
    function GetDeletedCount: integer;
    function GetEncrypted: boolean;
    function GetPackOnSave: Boolean;
    function GetTransactionProtected: boolean;
    function GetWithMemo: Boolean;
    procedure SetPackOnSave(const Value: Boolean);
  protected
    procedure SetNone(value :string);
    procedure SetFilename(value :string);
    function AllocRecordBuffer: TRecordbuffer; override;
    procedure FreeRecordBuffer(var Buffer: TRecordbuffer); override;
    procedure GetBookmarkData(Buffer: TRecordbuffer; Data: TBookMark); override;
    function GetBookmarkFlag(Buffer: TRecordbuffer): TBookmarkFlag; override;
    function GetRecord(Buffer: TRecordbuffer; GetMode: TGetMode; DoCheck: Boolean): TGetResult; override;
    function GetRecordSize: Word; override;
    function GetFieldPointer(Buffer: TRecordbuffer; Fields :TField): PBuffer;
    function GetActiveRecordBuffer: TRecordbuffer;
    procedure InternalAddRecord(Buffer: TRecordbuffer; Append: Boolean); override;
//    procedure InternalRefresh; override;
    procedure InternalClose; override;
    procedure InternalDelete; override;
    procedure InternalFirst; override;
    procedure InternalGotoBookmark(Bookmark: Pointer); override;
    procedure InternalHandleException; override;
    procedure InternalInitFieldDefs; override;
    procedure InternalInitRecord(Buffer: TRecordbuffer); override;
    procedure InternalLast; override;
    procedure InternalOpen; override;
    procedure InternalPost; override;
    procedure InternalInsert; override;
    procedure InternalSetToRecord(Buffer: TRecordbuffer); override;
    function IsCursorOpen: Boolean; override;
    procedure SetBookmarkFlag(Buffer: TRecordbuffer; Value: TBookmarkFlag); override;
    procedure SetBookmarkData(Buffer: TRecordbuffer; Data: Pointer); override;
    procedure SetFieldData(Field: TField; Buffer: Pointer); override;
    function GetRecordCount: Integer; override;
    function GetRecNo: Integer; override;
    procedure SetRecNo(Value: Integer); override;
    function GetData(Field :TField; var Value: PBuffer; Buffer: PBuffer) :boolean;
    function FindRecord(Restart, GoForward: Boolean): Boolean; override;
    procedure Zap;
    function HasMemo :boolean;
    function ProcessFilter(Buffer: PBuffer):boolean;
  public
    constructor Create(AOwner :TComponent); override;
    procedure CreateTable(const FileName :string; Fields :TList; Dbase3 :boolean);
    procedure Save;
    function FindKey(const KeyValues: array of const): Boolean;
    procedure GetFields(var Fields :TList);
    function Locate(const KeyFields: string; const KeyValues: Variant;
      Options: TLocateOptions): Boolean; override;
    function GetMemoData(Field :TField): string;
    procedure SetMemoData(Field :TField; Text :string);
    function GetFieldData(Field: TField; var Buffer: TValueBuffer): Boolean; override;
    {function GetBLOBData(Field :TField): TMemoryStream;
    procedure SetBLOBData(Field :TField; BLOB :TMemoryStream);}
    procedure Undelete;
    property OriginalFields[Index :integer] :TDBFField read GetField;
    property LastUpdate :string read FLastUpdate;
    property Deleted :Boolean read GetDeleted;
    property WithMemo :Boolean read GetWithMemo;
    property DeletedCount :integer read GetDeletedCount;
    property Modified :Boolean read FModified write FModified;
    property Version :string read FVersion;
    property CodePage :word read GetCodePage;
    property TransactionProtected :boolean read GetTransactionProtected;
    property Encrypted :boolean read GetEncrypted;
  published
    property About :string read fabout write SetNone;
    property FileName: string read FFileName write SetFileName;
    property MakeBackup :Boolean read FMakeBackup write FMakeBackup;
    property ShowDeleted :Boolean read FShowDeleted write SetShowDeleted;
    property PackOnSave :Boolean read GetPackOnSave write SetPackOnSave;
    property Filter;
    property Filtered :Boolean read GetFilterActive write SetFilterActive;
    property Active;
    property BeforeOpen;
    property AfterOpen;
    property BeforeClose;
    property AfterClose;
    property BeforeInsert;
    property AfterInsert;
    property BeforeEdit;
    property AfterEdit;
    property BeforePost;
    property AfterPost;
    property BeforeCancel;
    property AfterCancel;
    property BeforeDelete;
    property AfterDelete;
    property BeforeScroll;
    property AfterScroll;
    property OnDBFError:TDBFError read FOnDBFError write FOnDBFError;
    property OnDeleteError;
    property OnEditError;
    property OnNewRecord;
    property OnPostError;
    property OnCalcFields;
  end;

const
   AboutInfo = 'MiTeC DBF Table 1.5 - © 1997,2002, MichaL MutL';

   DBFOK           = 0;
   DBFIncorectFile =-1;
   DBFOutOfRange   =-2;
   DBFWrongFieldDef=-3;
   DBFInvalidValue =-4;
   DBFNotOpened    =-5;

   DBFErrorMessages :array[0..5] of string = ('OK',Err_IncorrectDBF,
                                                   Err_AccessOutRange,
                                                   Err_WrongFieldDef,
                                                   Err_InValidValue,
                                                   Err_FileNotOpen);


procedure Register;

implementation

const
  NullChar = 0;
  SpaceChar = 32;

procedure Register;
begin
  RegisterComponents('3rdPartyVCL', [TMDBFTable]);
{$ifdef VER130}
  RegisterPropertyEditor(TypeInfo(String), TMDBFTable, 'FileName', TFileNameProperty);
{$endif}
end;

{$IFNDEF D7PLUS}
function AnsiDequotedStr(const S: string; AQuote: Char): string;
var
  LText: PChar;
begin
  LText := PChar(S);
  Result := AnsiExtractQuotedStr(LText, AQuote);
  if ((Result = '') or (LText^ = #0)) and
     (Length(S) > 0) and ((S[1] <> AQuote) or (S[Length(S)] <> AQuote)) then
    Result := S;
end;
{$ENDIF D7PLUS}

{TDBFAccess}

constructor TDBFAccess.Create;
begin
  inherited create;
  with fdbf do
  begin
    data:=tstringlist.create;
    fields:=tlist.create;
    filename:='';
  end;
end;

destructor TDBFAccess.Destroy;
begin
  with fdbf do
  begin
    data.free;
    fields.free;
  end;
  inherited destroy;
end;

procedure TDBFAccess.OpenFile;
begin
  if (filename<>'') and not assigned(ffile) then
    ffile:=tfilestream.create(filename,fmopenread or fmsharedenywrite);
end;

procedure TDBFAccess.CreateFile;
begin
  if (filename<>'') then
  begin
    if assigned(ffile) then
    begin
      ffile.free;
      ffile:=nil;
    end;
    if fileexists(filename) then
      deletefile(filename);
    ffile:=tfilestream.create(filename,fmcreate or fmshareexclusive);
  end;
end;

procedure TDBFAccess.CloseFile;
begin
  if assigned(ffile) then
  begin
    ffile.free;
    ffile:=nil;
  end;
end;

function TDBFAccess.CheckRange;
begin
  result:=dbfok;
  if FileSize<(fdbf.recordcount*fdbf.recordsize+fdbf.HeaderSize) then
    result:=DBFIncorectFile
  else
    if fdbf.recordcount>dBase_MaxReccount then
      result:=DBFOutOfRange
    else
      if fdbf.RecordSize>dBase_MaxRecSize then
        result:=DBFOutOfRange
      else
        if fdbf.FieldCount>dBase_MaxFieldCount then
          result:=DBFOutOfRange;
end;

function TDBFAccess.CheckField;
begin
  result:=dbfok;
  with field do
  begin
    if Size>dBase_MaxFieldWidth then
      result:=DBFOutOfRange
    else
      case fieldType of
        ftstring :if Decimals<>0 then
           result:=DBFOutOfRange;
        ftinteger :if (Size>30) or (Decimals<>0) then
           result:=DBFOutOfRange;
        ftfloat :if (Size>30) or (Decimals>size-2) then
           result:=DBFOutOfRange
        else
          if Size+2<Decimals then
            result:=DBFOutOfRange;
        ftdate: if (Size<>8) or (Decimals<>0) then
           result:=DBFOutOfRange;
        ftboolean :if (Size<>1) or (Decimals<>0) then
          result:=DBFOutOfRange;
        ftmemo :if (Size<>10) or (Decimals<>0) then
           result:=DBFOutOfRange;
        ftblob :if (Size<>10) or (Decimals<>0) then
           result:=DBFOutOfRange;
        ftdbaseole :if (Size<>10) or (Decimals<>0) then
           result:=DBFOutOfRange;
        else
          result:=DBFWrongFieldDef;
      end;
    end;
end;

function TDBFAccess.ReadDBFHeader;
var
  buffer :byte;
begin
  if assigned(ffile) then
  begin
    try
      setlasterror(0);
      ffile.Seek(0,sofrombeginning);
      ffile.Read(Buffer,1);
      ffile.Seek(0,sofrombeginning);
      if not(buffer in [$03..$F5]) then
        result:=DBFIncorectFile
      else
        if buffer in [dBaseIIIPlus,dBaseIIIPlusMemo] then
        begin
          fdb3:=true;
          ffile.Read(Fdb3Header,32);
          with fdbf do
          begin
            Year:=Fdb3Header.Year;
            Month:=Fdb3Header.Month;
            Day:=Fdb3Header.Day;
            version:=3;
            Recordcount:=Fdb3Header.Recordcount;
            HeaderSize:=Fdb3Header.HeaderSize;
            RecordSize:=Fdb3Header.RecordSize;
            FieldCount:=Pred(Pred(HeaderSize) div 32);
            mdxpresent:=false;
            memo:=Fdb3Header.Version=dBaseIIIPlusMemo;
            anymemo:=memo;
            codepage:=0;
            encrypted:=false;
            transprot:=false;
            result:=CheckRange(ffile.Size);
          end;
        end
        else
        begin
          ffile.Read(Fdb4Header,32);
          fdb3:=false;
          with fdbf do
          begin
            Year:=Fdb4Header.Year;
            Month:=Fdb4Header.Month;
            Day:=Fdb4Header.Day;
            case Fdb4Header.Version and 7 of
              3 :version:=4;
              else version:=5;
            end;
            Recordcount:=Fdb4Header.Recordcount;
            HeaderSize:=Fdb4Header.HeaderSize;
            RecordSize:=Fdb4Header.RecordSize;
            FieldCount:=Pred(Pred(HeaderSize) div 32);
            mdxpresent:=Fdb4Header.MDXFlag=$01;
            memo:=(Fdb4Header.Version and 128)=128;
            anymemo:=(Fdb4Header.Version and 8)=8;
            transprot:=Fdb4Header.IncompleteTranstactionFlag=$01;
            encrypted:=Fdb4Header.EncryptionFlag=$01;
            case Fdb4Header.LangDrvID of
              001 :codepage:=437;
              002 :codepage:=850;
              100 :codepage:=852;
              102 :codepage:=865;
              101 :codepage:=866;
              104 :codepage:=895;
              200 :codepage:=1250;
              201 :codepage:=1251;
              003 :codepage:=1252;
              else
                 codepage:=0;
            end;
            result:=CheckRange(ffile.Size);
          end;
        end;
    except
      result:=getlasterror;
    end;
  end else
    result:=dbfnotopened;
end;

function TDBFAccess.ReadDBFFieldDefs;
var
  i,o :word;
  b :byte;
  Field :PDBFField;
begin
  o:=1;
  result:=dbfok;
  if assigned(ffile) then
  begin
    setlasterror(0);
    try
      ffile.Seek(32,sofrombeginning);
      if fdb3 then
      begin
        for i:=1 to fdbf.FieldCount do
          with fdbf do
          begin
            new(field);
            ffile.Read(Fdb3Field,32);
            b:=1;
            while (b<=10) and (Fdb3Field.Name[b]<>#0) do
              Inc(b);
            system.Move(Fdb3Field.Name[1],Field^.fieldName[1],Pred(b));
            Field^.fieldName[0]:=AnsiChar(Chr(Pred(b)));
            if b>0 then
              Field^.fieldName[0]:=AnsiChar(Chr(Pred(b)));
            case Fdb3Field.fieldType of
              'C':Field^.FieldType:=ftstring;
              'N': if Fdb3Field.Decimals>0 then
                     Field^.FieldType:=ftfloat
                   else
                     Field^.FieldType:=ftinteger;
              'F':Field^.FieldType:=ftfloat;
              'D':Field^.FieldType:=ftdate;
              'L':Field^.FieldType:=ftboolean;
              'M':Field^.FieldType:=ftmemo;
              'B':Field^.FieldType:=ftblob;
              'O':Field^.FieldType:=ftdbaseole;
            end;
            Field^.Size:=Fdb3Field.Width;
            Field^.Decimals:=Fdb3Field.Decimals;
            Field^.Offset:=o;
            Field^.indexed:=Fdb3Field.SetFields=1;
            Inc(o,Fdb3Field.Width);
            result:=CheckField(Field^);
            if result=dbfok then
              fields.add(field)
            else
              exit;
          end;
      end
      else
      begin
        for i:=1 to fdbf.FieldCount do
          with fdbf do
          begin
            new(field);
            ffile.Read(Fdb4Field,32);
            b:=1;
            while (b<=10) and (Fdb4Field.Name[b]<>#0) do
              Inc(b);
            system.Move(Fdb4Field.Name[1],Field^.fieldName[1],Pred(b));
            Field^.fieldName[0]:=AnsiChar(Chr(Pred(b)));
            if b>0 then
              Field^.fieldName[0]:=AnsiChar(Chr(Pred(b)));
            case Fdb4Field.fieldType of
              'C':Field^.FieldType:=ftstring;
              'N': if Fdb4Field.Decimals>0 then
                     Field^.FieldType:=ftfloat
                   else
                     Field^.FieldType:=ftinteger;
              'F':Field^.FieldType:=ftfloat;
              'D':Field^.FieldType:=ftdate;
              'L':Field^.FieldType:=ftboolean;
              'M':Field^.FieldType:=ftmemo;
              'B':Field^.FieldType:=ftblob;
              'O':Field^.FieldType:=ftdbaseole;
            end;
            Field^.Size:=Fdb4Field.Width;
            Field^.Decimals:=Fdb4Field.Decimals;
            Field^.Offset:=o;
            Field^.indexed:=Fdb4Field.Indexed=1;
            Inc(o,Fdb4Field.Width);
            result:=CheckField(Field^);
            if result=dbfok then
              fields.add(field)
            else
              exit;
          end;
      end;
    except
      result:=getlasterror;
    end;
  end else
    result:=dbfnotopened;
end;

function TDBFAccess.ReadDBFData;
var
  r :integer;
  fbuffer :^TDBFBuffer;
begin
  if assigned(ffile) then
  begin
    ffile.seek(fDBF.HeaderSize,sofrombeginning);
    r:=fDBF.RecordSize;
    new(fbuffer);	// mvh 20030810
    while (ffile.position+fDBF.RecordSize<ffile.Size) and (r=fDBF.RecordSize) do
    begin
      // new(fbuffer);	// mvh 20030810
      r:=ffile.Read(fbuffer^,fDBF.RecordSize);
      fdbf.data.add(string(AnsiString(PAnsiChar(fbuffer))));
    end;
    dispose(fbuffer);	// mvh 20030810
    result:=dbfok;
  end else
    result:=dbfnotopened;
end;

function TDBFAccess.WriteDB3Header;
var
  y,m,d :word;
begin
  if assigned(ffile) then
  begin
    result:=dbfok;
    decodedate(date,y,m,d);
    if y>2000 then
      y:=y-2000
    else
      y:=y-1900;
    hb.Year:=Y;
    hb.Month:=M;
    hb.Day:=D;
    setlasterror(0);
    try
      ffile.Seek(0,sofrombeginning);
      ffile.Write(hb,32);
    except
      result:=getlasterror;
    end;
  end else
    result:=dbfnotopened;
end;

function TDBFAccess.WriteDB4Header(HB: TdBaseIVPlus_Header): integer;
var
  y,m,d :word;
begin
  if assigned(ffile) then
  begin
    result:=dbfok;
    decodedate(date,y,m,d);
    if y>2000 then
      y:=y-2000
    else
      y:=y-1900;
    hb.Year:=Y;
    hb.Month:=M;
    hb.Day:=D;
    setlasterror(0);
    try
      ffile.Seek(0,sofrombeginning);
      ffile.Write(hb,32);
    except
      result:=getlasterror;
    end;
  end else
    result:=dbfnotopened;
end;

function TDBFAccess.WriteDBFFieldDefs;
var
  i :word;
begin
  result:=dbfok;
  if assigned(ffile) then
  begin
    setlasterror(0);
    try
      if fdb3 then
        for i:=0 to aFields.Count-1 do
        begin
          FillChar(Fdb3Field,SizeOf(Fdb3Field),0);
          Move(tdbffield(aFields.items[i]^).fieldName[1],Fdb3Field.Name,Length(tdbffield(aFields.items[i]^).fieldName));
          case tdbffield(aFields.items[i]^).FieldType of
            ftstring: Fdb3Field.FieldType:='C';
            ftinteger: Fdb3Field.FieldType:='N';
            ftdate: Fdb3Field.FieldType:='D';
            ftboolean: Fdb3Field.FieldType:='L';
            ftmemo: Fdb3Field.FieldType:='M';
          end;
          Fdb3Field.Width:=tdbffield(aFields.items[i]^).size;
          Fdb3Field.Decimals:=tdbffield(aFields.items[i]^).Decimals;
          Fdb3Field.SetFields:=byte(tdbffield(aFields.items[i]^).indexed);
          ffile.Write(Fdb3Field,32);
        end
      else
        for i:=0 to aFields.Count-1 do
        begin
          FillChar(Fdb4Field,SizeOf(Fdb4Field),0);
          Move(tdbffield(aFields.items[i]^).fieldName[1],Fdb4Field.Name,Length(tdbffield(aFields.items[i]^).fieldName));
          case tdbffield(aFields.items[i]^).FieldType of
            ftstring: Fdb4Field.FieldType:='C';
            ftinteger: Fdb4Field.FieldType:='N';
            ftfloat: Fdb4Field.FieldType:='F';
            ftdate: Fdb4Field.FieldType:='D';
            ftboolean: Fdb4Field.FieldType:='L';
            ftmemo: Fdb4Field.FieldType:='M';
            ftblob: Fdb4Field.FieldType:='B';
            ftdbaseole: Fdb4Field.FieldType:='O';
          end;
          Fdb4Field.Width:=tdbffield(aFields.items[i]^).size;
          Fdb4Field.Decimals:=tdbffield(aFields.items[i]^).Decimals;
          Fdb4Field.Indexed:=byte(tdbffield(aFields.items[i]^).indexed);
          ffile.Write(Fdb4Field,32);
        end
    except
      result:=getlasterror;
    end;
  end else
    result:=dbfnotopened;
end;


function TDBFAccess.WriteDBFData;
var
  fbuffer: PChar;
  recsize,i: integer;
  S: TBufElt;
begin
  if assigned(ffile) then
  begin
    result:=dbfok;
    setlasterror(0);
    recsize:=1;
    for i:=0 to afields.count-1 do
      recsize:=recsize+tdbffield(afields.items[i]^).size;
    fbuffer:=StrAlloc(recsize+1);
    try
      S:=flgBOF;
      ffile.write(S,1);
      if assigned(adata) then
      begin
        for i:=0 to adata.count-1 do
          if not packonsave or (packonsave and (PBuffer(adata[i])[0]<>flgDeleted)) then
          begin
            StrCopy(FBuffer,PChar(adata[i]));
            ffile.write(fBuffer^,recsize);
          end;
       S:=flgEOF;
      end;
      ffile.write(S,1);
    finally
      strdispose(fbuffer);
    end;
  end else
    result:=dbfnotopened;
end;

procedure TDBFAccess.RaiseDBFError;
var
  s :string;
begin
  if (fresult<0) then
    s:=DBFErrorMessages[abs(fresult)]
  else
    s:=Err_ErrorCode+inttostr(fresult);
  if Assigned(FOnDBFError) then
    FOnDBFError(Self,s);
  Abort;
end;

procedure TDBFAccess.LoadFromFile;
var i: integer;
begin
  fdbf.filename:=filename;

  // mvh 20030810
  for i:=0 to fdbf.fields.count-1 do Dispose(fdbf.fields[i]);

  fdbf.fields.clear;
  fdbf.data.clear;

  openfile(fdbf.filename);
  fresult:=readdbfheader;
  if fresult=dbfok then
  begin
    fresult:=readdbffielddefs;
    if fresult=dbfok then
begin
      fresult:=readdbFData;
end;
  end;
  closefile;
  if fresult<>dbfok then
    raisedbferror;
end;

procedure TDBFAccess.SaveToFile;
begin
  createfile(filename);
  if fdbf.version=3 then
  begin
    {if fdbf.Memo then
      Fdb3Header.Version:=dbaseiiiplusmemo
    else
      Fdb3Header.Version:=dbaseiiiplus;}
    if PackOnSave then
      Fdb3Header.Recordcount:=fDBF.data.Count-FDBF.DeletedCount
    else
      Fdb3Header.Recordcount:=fDBF.data.count;
    Fdb3Header.HeaderSize:=fDBF.HeaderSize;
    Fdb3Header.RecordSize:=fDBF.RecordSize;
    fresult:=writedb3header(Fdb3Header);
  end
  else
  begin
    {Fdb4Header.Version:=0;
    Fdb4Header.Version:=Fdb4Header.Version or 3;
    if fdbf.AnyMemo then
      Fdb4Header.Version:=Fdb4Header.Version or 128;
    if fdbf.Memo then
      Fdb4Header.Version:=Fdb4Header.Version or 8;
    Fdb4Header.MDXFlag:=byte(fdbf.MDXPresent);
    Fdb4Header.EncryptionFlag:=byte(fdbf.Encrypted);
    Fdb4Header.IncompleteTranstactionFlag:=byte(fdbf.TransProt);}
    if PackOnSave then
      Fdb4Header.Recordcount:=fDBF.data.Count-FDBF.DeletedCount
    else
      Fdb4Header.Recordcount:=fDBF.data.Count;
    Fdb4Header.HeaderSize:=fDBF.HeaderSize;
    Fdb4Header.RecordSize:=fDBF.RecordSize;
    fresult:=writedb4header(Fdb4Header);
  end;
  if fresult=dbfok then
  begin
    fresult:=writedbffielddefs(fdbf.fields);
    if fresult=dbfok then
       fresult:=writedbFData(fdbf.fields,fdbf.data);
  end;
  closefile;
  if fresult<>dbfok then
    raisedbferror;
end;

{TMDBFTable}

constructor TMDBFTable.Create;
begin
  inherited create(aowner);
  faccess:=tdbfaccess.create;
  faccess.OnDbfError:=DBFError;
  fabout:=aboutinfo;
  fversion:='Unknown';
end;

function TMDBFTable.GetField;
begin
  result:=tdbffield(FStructure.fields[index]^);
end;

procedure TMDBFTable.DBFError(Sender: TObject; ErrorMsg:String);
begin
  if Assigned(FOnDBFError) then
     FOnDBFError(Self,ErrorMsg) else
     MessageBox(0,PChar(ErrorMsg),Err_Stop,mb_Ok or mb_IconStop or mb_DefButton1);
end;

procedure TMDBFTable.SetFilename;
begin
  if active then
  begin
    if Assigned(FOnDBFError) then
      FOnDBFError(Self,Err_ChangeFileName)
    else
      MessageBox(0,Err_ChangeFileName,Err_Warning,mb_Ok or mb_IconExclamation or mb_DefButton1);
  end else
    ffilename:=value;
end;

procedure TMDBFTable.SetNone;
begin
end;

procedure TMDBFTable.InternalOpen;
var
  I: Integer;
  y :word;
begin
  faccess.loadfromfile(ffilename);
  FStructure:=FAccess.Structure;
  FData:=FStructure.data;
  FStructure.DeletedCount:=0;
  for I:=1 to FData.Count do
  begin
    FData.Objects[I-1]:=Pointer(I);
    if PBuffer(FData[i-1])[0]=flgDeleted then
      Inc(FStructure.Deletedcount);
  end;
  if WithMemo then
  begin
    try
      FMemoFilename:=changefileext(FFileName,'.dbt');
      FMemoFile:=TFileStream.Create(FMemoFilename,fmopenread or fmsharedenywrite);
      FMemoFile.Seek(0,soFromBeginning);
      FMemoFile.ReadBuffer(DBTHeader,Sizeof(DBTHeader));
    except
      on e:exception do
        showmessage(e.message);
    end;
  end;
  fversion:='dBase ?';
  case FStructure.version of
    3: fversion:='dBase III+';
    4: fversion:='dBase IV';
    5: fversion:='dBase/Win';
  end;
  if FStructure.Year>50 then
    y:=FStructure.Year+1900
  else
    y:=FStructure.Year+2000;
  FLastUpdate:=datetostr(encodedate(y,FStructure.month,FStructure.day));
  FLastBookmark:=FData.Count;
  FCurRec:=-1;
  frecsize:=1;
  for i:=0 to FStructure.fields.count-1 do
    frecsize:=frecsize+tdbffield(FStructure.fields.items[i]^).size;
  FRecInfoOfs:=frecsize;
  FRecBufSize:=FRecInfoOfs+SizeOf(TRecInfo);
  BookmarkSize:=SizeOf(Integer);
  InternalInitFieldDefs;

  if DefaultFields then
  try                 // mvh 20030809
    CreateFields;
  except
  end;

  BindFields(True);
end;


procedure TMDBFTable.InternalClose;
begin
  // mvh 20030810
  FData.clear;

  if Assigned(FMemoFile) then
  begin
    FMemoFile.Free;
    FMemoFile:=nil;
  end;
  if DefaultFields then
    DestroyFields;
  FLastBookmark:=0;
  FCurRec:=-1;
end;

function TMDBFTable.IsCursorOpen: Boolean;
begin
  result:=Assigned(FData);
end;

procedure TMDBFTable.InternalInitFieldDefs;
var
  i,s :integer;
begin
  FieldDefs.Clear;
  with FStructure.fields do
    for i:=0 to FStructure.fieldcount-1 do
    begin
      if tdbffield(items[i]^).fieldtype in [ftstring,ftmemo] then
        s:=tdbffield(items[i]^).size
      else
        s:=0;
      TFieldDef.Create(FieldDefs,string(tdbffield(items[i]^).fieldname),
        tdbffield(items[i]^).fieldtype,s,False,i+1)
    end;
end;

procedure TMDBFTable.InternalHandleException;
begin
  Application.HandleException(Self);
end;

procedure TMDBFTable.InternalGotoBookmark(Bookmark: Pointer);
var
  Index: Integer;
begin
  Index:=FData.IndexOfObject(TObject(PInteger(Bookmark)^));
  if Index<>-1 then
    FCurRec:=Index
  else
    DatabaseError(Err_BookMark);
end;

procedure TMDBFTable.InternalSetToRecord(Buffer: TRecordbuffer);
begin
  InternalGotoBookmark(@PRecInfo(Buffer + FRecInfoOfs).Bookmark);
end;

function TMDBFTable.GetBookmarkFlag(Buffer: TRecordbuffer): TBookmarkFlag;
begin
  Result:=PRecInfo(Buffer+FRecInfoOfs).BookmarkFlag;
end;

procedure TMDBFTable.SetBookmarkFlag(Buffer: TRecordbuffer; Value: TBookmarkFlag);
begin
  PRecInfo(Buffer+FRecInfoOfs).BookmarkFlag := Value;
end;

procedure TMDBFTable.GetBookmarkData(Buffer: TRecordbuffer; Data: TBookMark);
begin
  PInteger(Data)^:=PRecInfo(Buffer+FRecInfoOfs).Bookmark;
end;

procedure TMDBFTable.SetBookmarkData(Buffer: TRecordbuffer; Data: Pointer);
begin
  PRecInfo(Buffer+FRecInfoOfs).Bookmark:=PInteger(Data)^;
end;

function TMDBFTable.GetRecordSize: Word;
begin
  Result:=frecsize;
end;

function TMDBFTable.AllocRecordBuffer: TRecordbuffer;
begin
  GetMem(Result, FRecBufSize);
end;

procedure TMDBFTable.FreeRecordBuffer(var Buffer: TRecordbuffer);
begin
  FreeMem(Buffer, FRecBufSize);
end;

function TMDBFTable.GetRecord(Buffer: TRecordbuffer; GetMode: TGetMode;
  DoCheck: Boolean): TGetResult;
var
  accept :Boolean;
begin
  Result:=grOk;
  if FData.count < 1 then
    Result:=grEOF
  else
    repeat
      case GetMode of
        gmNext:
          if FCurRec >= RecordCount - 1  then
            result:=grEOF
          else
            Inc(FCurRec);
        gmPrior:
          if FCurRec <= 0 then
            result:=grBOF
          else
            Dec(FCurRec);
        gmCurrent:
          if (FCurRec < 0) or (FCurRec >= RecordCount) then
            result:=grError;
      end;
      if result=grOK then
      begin
        StrLCopy(PAnsiChar(Buffer), PAnsiChar(AnsiString(FData[FCurRec])), frecsize);
        ClearCalcFields(Buffer);
        GetCalcFields(Buffer);
        with PRecInfo(Buffer+FRecInfoOfs)^ do
        begin
          BookmarkFlag:=bfCurrent;
          Bookmark:=Integer(FData.Objects[FCurRec]);
        end;
      end else
        if (result=grError) and DoCheck then
        begin
          DatabaseError(Err_NoRecords);
          buffer[0] := NullChar;
        end
        else
          buffer[0] := NullChar;

      accept:=(fshowdeleted or (not fshowdeleted and (buffer[0]<>flgDeleted)));
      if Filtered or FActiveFilter then
        accept:=accept and ProcessFilter(Buffer);
      if (GetMode=gmCurrent) and not Accept then
        Result:=grError;
    until (Result<>grOK) or Accept;
  if ((Result=grEOF) or (Result=grBOF)) and (Filtered or FActiveFilter) and not (ProcessFilter(Buffer)) then
    Result := grError;
end;

procedure TMDBFTable.InternalInitRecord(Buffer: PBuffer);
begin
  FillChar(Buffer^,RecordSize,0);
end;

function TMDBFTable.GetFieldData(Field: TField; var Buffer: TValueBuffer): Boolean;
begin
  if (FData.count=0) and (fcurrec=-1) and (state=dsbrowse) then
    fillchar(Pointer(activeBuffer)^,RecordSize,0);
  if not (Field.datatype in [ftmemo,ftgraphic,ftblob,ftdbaseole]) then
    Result:=getdata(field,PBuffer(buffer),Pointer(activebuffer))
  else
    Result:=True;
end;

procedure TMDBFTable.SetFieldData(Field: TField; Buffer: Pointer);
var
  Offs,fs: integer;
  i,j,p: integer;
  ShortStr: string[255];
  s: string;
  dt: TDateTime;
  eof: boolean;
begin
  if (Field.FieldNo>=0) and (Assigned(Buffer)) then
  begin
    if PByte(activebuffer)[0]=NullChar then
      FillChar(Pointer(activeBuffer)^,RecordSize,32);
    offs:=tdbffield(FStructure.fields.Items[Field.fieldno-1]^).Offset;
    fs:=tdbffield(FStructure.fields.Items[Field.fieldno-1]^).Size;
    case Field.DataType of
      ftString: ;
      ftFloat :
      begin
        Str(pdouble(Buffer)^:fs:tdbffield(FStructure.fields.Items[Field.fieldno-1]^).Decimals, ShortStr);
        s := string(ShortStr); // convert shortstring into regular string
        while Length(s)<fs do
          s:=' '+s;
        p:=Pos(FormatSettings.DecimalSeparator,s);
        if (p>0) and (FormatSettings.DecimalSeparator<>'.') then
          s[p]:='.';
        strlcopy(buffer, PAnsiChar(AnsiString(s)),fs);
      end;
      ftInteger, ftMemo :begin
        s:=inttostr(pinteger(Buffer)^);
        while Length(s)<fs do
          s:=' '+s;
        strlcopy(buffer,PAnsiChar(AnsiString(s)),fs);
      end;
      ftDate :begin
        j:=pinteger(Buffer)^-693594;
        pdouble(@dt)^:=j;
        s:=FormatDateTime('yyyymmdd',dt);
        StrLCopy(Buffer,PAnsiChar(AnsiString(s)),fs);
      end;
      ftBoolean :if pboolean(Buffer)^ then
                   PAnsiChar(Buffer)^:='T'
                 else
                   PAnsiChar(Buffer)^:='F';
      ftgraphic,ftblob :;
      else
        raise Exception.Create('Unexpected field type.');
    end;
    eof:=false;
    if PByte(activebuffer)[frecsize]<>NullChar then
      PByte(activebuffer)[frecsize]:=NullChar;
    for i:=offs to offs+fs-1 do
    begin
      if not eof then
        eof:=(PBuffer(buffer)[i-offs]=NullChar) and (i>0);
      if not eof then
        PByte(activebuffer)[i]:=PBuffer(buffer)[i-offs]
      else
        PByte(activebuffer)[i]:=SpaceChar;
    end;
    {if Field.DataType=ftMemo then
      strcopy(PByte(FData[FCurRec]),activebuffer);}
    DataEvent(deFieldChange,Longint(Field));
  end;
end;

procedure TMDBFTable.InternalFirst;
begin
  FCurRec:=-1;
end;

procedure TMDBFTable.InternalLast;
begin
  FCurRec:=FData.Count;
end;

procedure TMDBFTable.InternalPost;
begin
  FModified:=True;
  if (State=dsEdit) then
  begin
    FData[FCurRec]:=string(AnsiString(PAnsiChar(ActiveBuffer)));
  end
  else
  begin
    Inc(FLastBookmark);
    if (State=dsInsert) and (FCurRec<0) then
      FCurRec:=0;
    FData.InsertObject(FCurRec, string(AnsiString(PAnsiChar(ActiveBuffer))), Pointer(FLastBookmark));
  end;
end;

procedure TMDBFTable.InternalAddRecord(Buffer: TRecordbuffer; Append: Boolean);
begin
  FModified:=True;
  Inc(FLastBookmark);
  if Append then
    InternalLast;
  FData.InsertObject(FCurRec, string(AnsiString(PAnsiChar(Buffer))), Pointer(FLastBookmark));
end;

procedure TMDBFTable.InternalDelete;
begin
{  FData.Delete(FCurRec);
  if FCurRec >= FData.Count then
    Dec(FCurRec);
  if fcurrec=-1 then
    FillChar(activeBuffer^,RecordSize,0);}
  PBuffer(FData[FCurRec])[0]:=flgDeleted;
  PBuffer(activebuffer)[0]:=flgDeleted;
  Inc(FStructure.DeletedCount);
  FModified:=True;
  //refresh;
end;

function TMDBFTable.GetRecordCount: Longint;
begin
  result:=FData.Count;
end;

function TMDBFTable.GetRecNo: Longint;
begin
  UpdateCursorPos;
  if (FCurRec = -1) and (RecordCount > 0) then
    result:=1 else
    result:=FCurRec + 1;
end;

procedure TMDBFTable.SetRecNo(Value: Integer);
begin
  if (Value >= 0) and (Value < FData.Count) then
  begin
    FCurRec := Value - 1;
    Resync([]);
  end;
end;

procedure TMDBFTable.CreateTable;
var
  hb :TdBaseIIIPlus_Header;
  i :integer;
begin
  with FAccess do
  begin
    createfile(filename);
    HB.Recordcount:=0;
    HB.HeaderSize:=succ(succ(Fields.Count)*32);
    hb.recordsize:=1;
    hb.version:=$03;
    FillChar(hb.Reserved1,3,0);
    FillChar(hb.LANRsvd,13,0);
    FillChar(hb.Reserved2,4,0);
    for i:=0 to fields.count-1 do
      hb.recordsize:=hb.recordsize+tdbffield(fields.items[i]^).size;
    fresult:=writedb3header(hb);
    if fresult=dbfok then
    begin
      fresult:=writedbffielddefs(fields);
      if fresult=dbfok then
        fresult:=writedbFData(fields,nil);
    end;
    closefile;
    if fresult<>dbfok then
      raisedbferror;
  end;
end;

function TMDBFTable.FindKey(const KeyValues: array of const): Boolean;
var
  i : integer;
begin
  result:=false;
  for i:=0 to FData.Count-1 do
    if pos(string(AnsiString(KeyValues[0].VPChar)), FData[i])>0 then
    begin
      result:=true;
      FCurRec := i;
      resync([]);
      break;
    end;
end;

procedure TMDBFTable.Save;
var
  bakfile :string;
begin
  if fmakebackup then
  begin
    bakfile:=changefileext(ffilename,'.~'+Copy(extractfileext(ffilename),2,255));
    if fileexists(bakfile) then
      deletefile(bakfile);
    renamefile(ffilename,bakfile);
  end;
  with FAccess do
  begin
    Structure:=FStructure;
    PackOnSave:=FPackOnSave;
    SaveToFile(ffilename);
  end;
  FModified:=false;
end;

procedure TMDBFTable.Zap;
var
  i :integer;
begin
  FModified:=True;
  for i:=0 to FData.count-1 do
    delete;
  FillChar(PByte(activeBuffer)^,RecordSize,0);
end;

procedure TMDBFTable.GetFields;
begin
  fields:=FStructure.fields;
end;

function TMDBFTable.Locate(const KeyFields: string;
  const KeyValues: Variant; Options: TLocateOptions): Boolean;
var
  i,j,a,n: integer;
  fn,s,k,v: ansistring;
  kv: variant;
  p: PBuffer;
  found: boolean;
begin
  kv:=keyvalues;
  if not varisarray(kv) then
    kv:=vararrayof([keyvalues]);
  n:=vararrayhighbound(kv,1)+1;
  if length(trim(keyfields))>0 then
  begin
    p:=PBuffer(AnsiStrAlloc(dBase_MaxFieldWidth));

    result:=false;
    for a:=0 to FData.count-1 do
    begin
      j:=1;
      for i:=0 to n-1 do
      begin
        fn:=extractfieldname(keyfields,j);
        getdata(fieldbyname(fn),p,PBuffer(ansistring(FData[a])));
        v:=String(AnsiString(PAnsiChar(p)));
        if fieldbyname(fn).datatype=ftstring then
        begin
          if locaseinsensitive in options then
          begin
            s:=uppercase(v);
            k:=uppercase(kv[i]);
          end
          else
          begin
            s:=v;
            k:=kv[i];
          end;
          if lopartialkey in options then
            found:=(pos(k,s)=1)
          else
            found:=(k=s);
        end else
          found:=(v=kv[i]);
        result:=found;
        if not found then
          break;
      end;
      if result then
      begin
        fcurrec:=a;
        resync([]);
        break;
      end;
    end;
    strdispose(PAnsiChar(p));
  end else
    result:=false;
end;

function TMDBFTable.GetData(Field :TField; var Value :PBuffer; Buffer :PBuffer) :boolean;
var
  Offs :integer;
  s :ansistring;
  Buf: PBuffer;
  i,j,l,p :integer;
  d :Double;
  n :integer;
  dt :TDateTime;
  OldDateFormat :string;
begin
  Result:=false;
  Buf:=Buffer;//ActiveBuffer;
  if (not IsEmpty) and (RecordCount>0) and (Field.FieldNo>0) and (Assigned(Buffer)) and (Assigned(Buf)) then
  begin
    offs:=tdbffield(FStructure.fields.Items[Field.fieldno-1]^).Offset;
    case Field.DataType of
      ftString:
      begin
        l:=tdbffield(FStructure.fields.Items[Field.fieldno-1]^).Size;
        s:='';
        i:=0;
        while (Buf[offs+i]<>NullChar) and (i<l) do
        begin
          s:=s+AnsiChar(AnsiChar(Buf[offs+i]));
          inc(i);
        end;
        s:=Trim(s);
        s:=s+#0;
        for I := 1 to Length(s)+1 do
         value[i-1] := byte(s[i]);

        // value := s;
        // CharToOemBuff(PAnsiChar(s),PAnsiChar(value),Length(s)+1);
        Result:=true;
      end;
      ftFloat,ftInteger :
      begin
        n:=tdbffield(FStructure.fields.Items[Field.fieldno-1]^).Size;
        s:='';
        for i:=offs to offs+n-1 do
          s:=s+AnsiChar(AnsiChar(Buf[i]));
        s:=Trim(s);
        if s='' then
          Result:=false
        else
        begin
          if Field.DataType=ftfloat then
          begin
            p:=pos('.',s);
            if (p>0) and (FormatSettings.DecimalSeparator<>'.') then
              s[p]:=ansichar(FormatSettings.DecimalSeparator);
            result:=true;
            try
              d:=StrToFloat(s);
            except
              d:=0;
              result:=false;
            end;
            PDouble(value)^:=d;
          end
          else
          begin
            Result:=True;
            try
              i:=StrToint(s);
            except
              i:=0;
              result:=false;
            end;
            Pinteger(value)^:=i;
          end;
        end;
      end;
      ftDate :
      begin
        s:='';
        for j:=0 to 7 do
          s:=s+AnsiChar(AnsiChar(Buf[offs+j]));
        SetLength(s,8);
        if (trim(s)='') or (s='00000000') then
          result:=false
        else
        begin
          s:=Copy(s,7,2)+FormatSettings.DateSeparator+Copy(s,5,2)+FormatSettings.DateSeparator+Copy(s,1,4);
          OldDateFormat:=FormatSettings.ShortDateFormat;
          FormatSettings.ShortDateFormat:='dd/mm/yyyy';
          dt:=StrToDate(s);
          FormatSettings.ShortDateFormat:=OldDateFormat;
          j:=Trunc(PDouble(@dt)^)+693594;
          pinteger(value)^:=j;
          result:=true;
        end;
      end;
      ftBoolean :
      begin
        result:=true;
        if AnsiChar(Buf[offs]) in ['S','T','Y'] then
          pboolean(value)^:=True
        else
          if AnsiChar(Buf[Offs]) in ['N','F'] then
            pboolean(value)^:=false
          else
            result:=false;
      end;
      ftMemo,ftgraphic,ftblob,ftdbaseole :
      begin
        n:=tdbffield(FStructure.fields.Items[Field.fieldno-1]^).Size;
        s:='';
        for i:=offs to offs+n-1 do
          s:=s+AnsiChar(AnsiChar(Buf[i]));
        s:=Trim(s);
        if s='' then
          Result:=false
        else
        begin
          Result:=True;
          try
            i:=StrToint(s);
          except
            i:=0;
            result:=false;
          end;
          Pinteger(value)^:=i;
        end;
      end;
      else
      begin
        ShowMessage('Unexpected field type.');
        result:=false;
      end;
    end;
  end;
end;

procedure TMDBFTable.UnDelete;
begin
  PBuffer(activebuffer)[0]:=flgUndeleted;
  PBuffer(FData[FCurRec])[0]:=flgUndeleted;
  Dec(FStructure.DeletedCount);
  FModified:=True;
  //refresh;
end;

function TMDBFTable.GetDeleted: Boolean;
begin
  Result:=PBuffer(activebuffer)[0]=flgDeleted;
end;

procedure TMDBFTable.SetShowDeleted(const Value: Boolean);
begin
  if FShowDeleted<>Value then
  begin;
    FShowDeleted := Value;
    if Active then
      refresh;
  end;
end;

function TMDBFTable.GetFieldPointer(Buffer: PBuffer; Fields: TField): PBuffer;
begin
  Result:=Buffer;
  if Buffer=nil then
    exit;
  inc(Result,tdbffield(FStructure.fields.items[fields.fieldno-1]^).offset);
end;

function TMDBFTable.GetActiveRecordBuffer: PBuffer;
begin
  if State=dsBrowse then
  begin
    if IsEmpty then
      Result:=nil
    else
      Result:=PByte(ActiveBuffer);
  end else
    Result:=PByte(ActiveBuffer);
end;

function TMDBFTable.HasMemo: boolean;
var
  i :integer;
begin
  result:=false;
  for i:=0 to fieldcount-1 do
    if originalfields[i].FieldType=ftmemo then
    begin
      result:=true;
      break;
    end;
end;

function TMDBFTable.ProcessFilter(Buffer: PBuffer): boolean;
var
  FilterExpresion : string;
  PosComp : integer;
  FName : string;
  FieldPos : integer;
  FieldOffset : integer;
  FieldValue : Variant;
  TestValue : Variant;
  FieldText : string;
  OldShortDateFormat : string;
  T: string;
begin
  FilterExpresion := Filter;
  PosComp := Pos('<',FilterExpresion);
  if PosComp=0 then
    PosComp := Pos('>',FilterExpresion);
  if PosComp=0 then
    PosComp := Pos('=',FilterExpresion);
  if PosComp=0 then
  begin
    result:=True;
    Exit;
  end;

  if buffer[0]=NullChar then
  begin
    result:=False;
    Exit;
  end;

  FName := Trim(Copy(FilterExpresion,1,PosComp-1));
  FieldPos := FieldDefs.IndexOf(FName);
  FieldOffset := tdbffield(FStructure.fields.Items[fieldpos]^).offset;
  if FieldPos < 0 then
    Result:=True
  else
    if tdbffield(FStructure.fields.Items[fieldpos]^).FieldType = ftString then
    begin // STRING
      try
        FieldValue := '';
        //FieldOffset := FieldOffset+1;
        While (Buffer[FieldOffset]<>NullChar) and (Length(FieldValue)<tdbffield(FStructure.fields.Items[fieldpos]^).Size) do
        begin
          FieldValue := FieldValue + Char(AnsiChar(Buffer[FieldOffset]));
          FieldOffset := FieldOffset+1;
        end;
        FieldValue := Trim(FieldValue);
        TestValue := Trim(Copy(FilterExpresion,PosComp+1,Length(FilterExpresion)-PosComp{-2}));
        TestValue := AnsiDequotedStr(TestValue, '''');
        if FilterExpresion[PosComp]='=' then
        begin
          T := TestValue;               // mvh 20030810: needs checking of FilterOptions! (lsp, 20100607)
          if foCaseInsensitive in FilterOptions then
          begin
            T := UpperCase(T);
            FieldValue := UpperCase(FieldValue);
          end;
          if (not (foNoPartialCompare in FilterOptions)) and
            (Length(T)<>0) and (T[Length(T)]='*') and (T[1]='*') then
            Result := Pos(Copy(T, 2, Length(T)-2), FieldValue)>=1
          else if (not (foNoPartialCompare in FilterOptions)) and
            (Length(T)<>0) and (T[Length(T)]='*') then
            Result := Pos(Copy(T, 1, Length(T)-1), FieldValue)=1
          else
            Result := (FieldValue=T);
        end
        else
          if FilterExpresion[PosComp]='>' then
          begin
            if FilterExpresion[PosComp+1]='=' then
              Result := (FieldValue>=Copy(TestValue,2,(Length(TestValue)-1)))
            else
              Result := (FieldValue>TestValue);
          end
        else
          if FilterExpresion[PosComp]='<' then
          begin
            if FilterExpresion[PosComp+1]='=' then
              Result := (FieldValue<=Copy(TestValue,2,(Length(TestValue)-1)))
            else
              if FilterExpresion[PosComp+1]='>' then
                Result := (FieldValue<>Copy(TestValue,2,(Length(TestValue)-1)))
              else
                Result := (FieldValue<TestValue);
          end else
            Result := False;
      except
        Result := False;
      end;
    end
  else
    if tdbffield(FStructure.fields.Items[fieldpos]^).FieldType in [ftFloat,ftinteger] then
    begin // FLOAT
      try
        FieldText := '';
        //FieldOffset := FieldOffset+1;
        while (Buffer[FieldOffset]<>NullChar) and (Length(FieldText)<tdbffield(FStructure.fields.Items[fieldpos]^).Size) do
        begin
          FieldText := FieldText + Char(AnsiChar(Buffer[FieldOffset]));
          FieldOffset := FieldOffset+1;
        end;
        FieldText := Trim(FieldText);
        if Pos('.',FieldText)>0 then
          FieldText[Pos('.',FieldText)] := FormatSettings.DecimalSeparator;
        FieldValue := StrToFloat(FieldText);
        if FilterExpresion[PosComp+1]='='then
          FieldText := Trim(Copy(FilterExpresion,PosComp+2,Length(FilterExpresion)-PosComp-1))
        else
          FieldText := Trim(Copy(FilterExpresion,PosComp+1,Length(FilterExpresion)-PosComp));
        if Pos('.',FieldText)>0 then
          FieldText[Pos('.',FieldText)] := FormatSettings.DecimalSeparator;
        TestValue := StrToFloat(FieldText);
        if FilterExpresion[PosComp]='=' then
          Result := (FieldValue=TestValue)
        else
          if FilterExpresion[PosComp]='>'then
          begin
            if FilterExpresion[PosComp+1]='='then
              Result := (FieldValue>=TestValue)
            else
              Result := (FieldValue>TestValue);
          end
        else
          if FilterExpresion[PosComp]='<'then
          begin
            if FilterExpresion[PosComp+1]='='then
              Result := (FieldValue<=TestValue)
            else
              if FilterExpresion[PosComp+1]='>'then
                Result := (FieldValue<>TestValue)
              else
                Result := (FieldValue<TestValue);
          end
        else
          Result := False;
      except
        Result := False;
      end;
    end
  else
    if tdbffield(FStructure.fields.Items[fieldpos]^).FieldType = ftDate then
    begin // DATE
      OldShortDateFormat := FormatSettings.ShortDateFormat;
      try
        FieldText := '';
        //FieldOffset := FieldOffset+1;
        while (Buffer[FieldOffset]<>NullChar) and (Length(FieldText)<tdbffield(FStructure.fields.Items[fieldpos]^).Size) do
        begin
          FieldText := FieldText + Char(AnsiChar(Buffer[FieldOffset]));
          FieldOffset := FieldOffset+1;
        end;
        FieldText := Trim(FieldText);
        FieldText := Copy(FieldText,1,4)+FormatSettings.DateSeparator+Copy(FieldText,5,2)+FormatSettings.DateSeparator+Copy(FieldText,7,2);
        FormatSettings.ShortDateFormat := 'yyyy/mm/dd';
        FieldValue := StrToDate(FieldText);
        if FilterExpresion[PosComp+1]='=' then
          FieldText := Trim(Copy(FilterExpresion,PosComp+2,Length(FilterExpresion)-PosComp-1))
        else
          FieldText := Trim(Copy(FilterExpresion,PosComp+1,Length(FilterExpresion)-PosComp));
        FieldText := Copy(FieldText,1,4)+FormatSettings.DateSeparator+Copy(FieldText,5,2)+FormatSettings.DateSeparator+Copy(FieldText,7,2);
        TestValue := StrToDate(FieldText);
        if FilterExpresion[PosComp]='=' then
        begin
          Result := (FieldValue=TestValue);
        end else
          if FilterExpresion[PosComp]='>' then
          begin
            if FilterExpresion[PosComp+1]='='then
              Result := (FieldValue>=TestValue)
            else
              Result := (FieldValue>TestValue);
          end
        else
          if FilterExpresion[PosComp]='<' then
          begin
            if FilterExpresion[PosComp+1]='='then
              Result := (FieldValue<=TestValue)
            else
              if FilterExpresion[PosComp+1]='>'then
                Result := (FieldValue<>TestValue)
              else
                Result := (FieldValue<TestValue);
          end
        else
          Result := False;
      except
        Result := False;
      end;
      FormatSettings.ShortDateFormat := OldShortDateFormat;
    end else
      if tdbffield(FStructure.fields.Items[fieldpos]^).FieldType = ftBoolean then
      begin
        try
          if (AnsiChar(Buffer[FieldOffset+1])='T') or (AnsiChar(Buffer[FieldOffset+1])='t') or
             (AnsiChar(Buffer[FieldOffset+1])='Y') or (AnsiChar(Buffer[FieldOffset+1])='y') or
             (AnsiChar(Buffer[FieldOffset+1])='S') or (AnsiChar(Buffer[FieldOffset+1])='s') then
            FieldValue := True
          else
            FieldValue := False;
        FieldText := Trim(Copy(FilterExpresion,PosComp+1,Length(FilterExpresion)-PosComp));
        if FilterExpresion[PosComp]='=' then
        begin
          if (FieldText='T') or (FieldText='t') or
             (FieldText='Y') or (FieldText='y') or
             (FieldText='S') or (FieldText='s') then
            TestValue := True
          else
            TestValue := False;
          Result := (FieldValue=TestValue)
        end
      else
        if FilterExpresion[PosComp]='>' then
        begin
          if FilterExpresion[PosComp+1]='=' then
          begin
              if (FieldText[2]='T') or (FieldText[2]='t') or
                 (FieldText[2]='Y') or (FieldText[2]='y') or
                 (FieldText[2]='S') or (FieldText[2]='s') then
                TestValue := True
              else
                TestValue := False;
              Result := (FieldValue>=TestValue);
            end
            else
            begin
              if (FieldText[1]='T') or (FieldText[1]='t') or
                 (FieldText[1]='Y') or (FieldText[1]='y') or
                 (FieldText[1]='S') or (FieldText[1]='s') then
                TestValue := True
              else
                TestValue := False;
              Result := (FieldValue>TestValue);
            end;
        end
      else
        if FilterExpresion[PosComp]='<' then
        begin
          if FilterExpresion[PosComp+1]='=' then
          begin
              if (FieldText[2]='T') or (FieldText[2]='t') or
                 (FieldText[2]='Y') or (FieldText[2]='y') or
                 (FieldText[2]='S') or (FieldText[2]='s') then
                TestValue := True
              else
                TestValue := False;
              Result := (FieldValue<=TestValue);
            end
          else
            if FilterExpresion[PosComp+1]='>' then
            begin
              if (FieldText[2]='T') or (FieldText[2]='t') or
                 (FieldText[2]='Y') or (FieldText[2]='y') or
                 (FieldText[2]='S') or (FieldText[2]='s') then
                TestValue := True
              else
                TestValue := False;
              Result := (FieldValue<>TestValue);
            end
            else
            begin
              if (FieldText[1]='T') or (FieldText[1]='t') or
                 (FieldText[1]='Y') or (FieldText[1]='y') or
                 (FieldText[1]='S') or (FieldText[1]='s') then
                TestValue := True
              else
                TestValue := False;
              Result := (FieldValue<TestValue);
            end;
        end
      else
        Result := False;
     except
       Result := False;
     end;
    end
  else
    Result := False;
end;

function TMDBFTable.FindRecord(Restart, GoForward: Boolean): Boolean;
var
  Status : Boolean;
begin
  CheckBrowseMode;
  DoBeforeScroll;
  UpdateCursorPos;
  CursorPosChanged;
  try
    if GoForward then
    begin
      if Restart then
        First;
      if Filtered then
        fActiveFilter := True;
      Status := GetNextRecord;
    end
    else
    begin
      if Restart then
        Last;
      if Filtered then
        fActiveFilter := True;
      Status := GetPriorRecord;
    end;
  finally
    if Filtered then
      fActiveFilter := False;
  end;
  Result := Status;
  if Result then
    DoAfterScroll;
end;

procedure TMDBFTable.SetFilterActive(const Value: Boolean);
begin
  SetFiltered(Value);
  if Active then
    refresh;
end;

function TMDBFTable.GetFilterActive: Boolean;
begin
  Result:=inherited Filtered;
end;

function TMDBFTable.GetMemoData(Field :TField): string;
var
  TmpS :string;
  Buff :array[1..512] of Char;
  Flag :boolean;
  Block :pinteger;
begin
  if assigned(FMemofile) then
  try
    TmpS:='';
    Flag:=false;
    new(Block);
    getdata(Field,PBuffer(block),PByte(activebuffer));
    FMemoFile.Seek(Block^*512,soFromBeginning);
    while (not Flag) do
    begin
      if FMemoFile.Read(Buff,512)<512 then
        Flag:=True;
      if Pos(#$1A#$1A,Buff)>0 then
      begin
        TmpS:=TmpS+Copy(Buff,1,Pos(#$1A#$1A,Buff)-1);
        Flag:=True;
      end else
        TmpS:=TmpS+Copy(Buff,1,Pos(#$1A,Buff)-1);
    end;
    while Pos(#$8D,TmpS)>0 do
      TmpS[Pos(#$8D,TmpS)]:=#$D;
    Result:=TmpS;
    dispose(block);
  except
    raise Exception.Create('Error occured while reading memo data.');
  end;
end;

procedure TMDBFTable.SetMemoData(Field: TField; Text: string);
var
  TmpL : Longint;
  Block: pinteger;
  s :string;
begin
  if assigned(FMemofile) then
  begin
    new(Block);
    Block^:=DBTHeader.NextBlock;
    SetFieldData(Field,PByte(Block));
    FMemoFile.Seek(-1,soFromEnd);
    s:=#$1A#$1A;
    for TmpL:=1 to Length(s) do
      FMemoFile.write(s[TmpL],1);
    FMemoFile.Seek(DBTHeader.NextBlock*512,soFromBeginning);
    Text:=Text+#$1A#$1A;
    for TmpL:=1 to Length(Text) do
      FMemoFile.Write(Text[TmpL],1);
    inc(DBTHeader.NextBlock);
    dispose(block);
    FModified:=True;
  end;
end;

{function TMDBFTable.GetBLOBData(Field: TField): TMemoryStream;
var
  p :PByte;
  Buff :PByte;
  Flag :boolean;
  Block :pinteger;
begin
  if assigned(FMemofile) then
  try
    result:=tmemorystream.create;
    buff:=stralloc(512);
    Flag:=false;
    new(Block);
    getdata(Field,PByte(block),activebuffer);
    FMemoFile.Seek(Block^*512,soFromBeginning);
    while (not Flag) do begin
      if FMemoFile.Read(Buff,512)<512 then
        Flag:=True;
      p:=StrPos(Buff,#$1A#$1A);
      if assigned(p) then begin
        strlcopy(buff,buff,strlen(buff)-strlen(p)-1);
        Flag:=True;
      end else begin
        p:=strPos(#$1A,Buff);
        strlcopy(buff,buff,strlen(buff)-strlen(p)-1);
      end;
    end;
    result.setsize(strlen(buff));
    result.write(buff,strlen(buff));
    dispose(block);
    strdispose(buff);
  except
    raise Exception.Create('Error occured while reading BLOB data.');
  end;
end;

procedure TMDBFTable.SetBLOBData(Field: TField; BLOB: TMemoryStream);
begin

end;}

procedure TMDBFTable.InternalInsert;
begin
end;

function TMDBFTable.GetCodePage: word;
begin
  Result:=FStructure.CodePage;
end;

function TMDBFTable.GetDeletedCount: integer;
begin
  Result:=FStructure.DeletedCount;
end;

function TMDBFTable.GetEncrypted: boolean;
begin
  Result:=FStructure.Encrypted;
end;

function TMDBFTable.GetPackOnSave: Boolean;
begin
  Result:=FAccess.PackOnSave;
end;

function TMDBFTable.GetTransactionProtected: boolean;
begin
  Result:=FStructure.TransProt;
end;

function TMDBFTable.GetWithMemo: Boolean;
begin
  Result:=FStructure.Memo;
end;

procedure TMDBFTable.SetPackOnSave(const Value: Boolean);
begin
  FAccess.PackOnSave:=Value;
end;

{ TFilenameProperty }

{$ifdef VER130}
procedure TFilenameProperty.Edit;
var
  FileOpen: TOpenDialog;
begin
  FileOpen:=TOpenDialog.Create(nil);
  FileOpen.Filename:=GetValue;
  FileOpen.Filter:='dBase Files (*.DBF)|*.DBF|All Files (*.*)|*.*';
  FileOpen.Options:=FileOpen.Options+[ofPathMustExist,ofFileMustExist];
  try
    if FileOpen.Execute then
      SetValue(FileOpen.Filename);
  finally
    FileOpen.Free;
  end;
end;


function TFilenameProperty.GetAttributes: TPropertyAttributes;
begin
  Result:=[paDialog, paRevertable];
end;
{$endif}

end.

