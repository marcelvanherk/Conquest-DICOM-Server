// 20150730	mvh	started this added advanced modify dialog
// 20160313	mvh	protected against missing modifier.lua
// 20161120     mvh     test syntax for DBASEIII now limited in functionality
//                      to avoid it starting new database indexing
// 20201002     mvh     Quote command line to allow patient ID with spaces

unit ModifyDicom;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls;

type
  TFormModifyDICOM = class(TForm)
    MemoScript: TMemo;
    GroupBox1: TGroupBox;
    RadioButtonNewUids: TRadioButton;
    RadioButtonAnonymize: TRadioButton;
    RadioButtonKeepTheSame: TRadioButton;
    Label1: TLabel;
    GroupBox2: TGroupBox;
    Label2: TLabel;
    RadioButtonCopy: TRadioButton;
    RadioButtonReplace: TRadioButton;
    CancelButton: TButton;
    OKButton: TButton;
    Button1: TButton;
    Button2: TButton;
    Button3: TButton;
    Label3: TLabel;
    procedure Button3Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure OKButtonClick(Sender: TObject);
    procedure FormActivate(Sender: TObject);
    procedure OKButtonMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
  private
    { Private declarations }
  public
    AltPressed: Boolean;
    { Public declarations }
  end;

var
  FormModifyDICOM: TFormModifyDICOM;

implementation

uses Launch, ServerDriver;

{$R *.dfm}

procedure TFormModifyDICOM.Button1Click(Sender: TObject);
var
  OpenDialog: TFileOpenDialog;
begin
  OpenDialog := TFileOpenDialog.Create(Application.MainForm);
  try
    if not OpenDialog.Execute then
      Abort;
    MemoScript.Lines.LoadFromFile(OpenDialog.FileName);
  finally
    OpenDialog.Free;
  end;
end;

procedure TFormModifyDICOM.Button2Click(Sender: TObject);
var
  SaveDialog: TFileSaveDialog;
begin
  SaveDialog := TFileSaveDialog.Create(Application.MainForm);
  SaveDialog.Title := 'Save script';
  SaveDialog.DefaultExtension := 'lua';
  SaveDialog.Filename := '';

  if SaveDialog.Execute then
    MemoScript.Lines.SaveToFile(SaveDialog.Filename);
  SaveDialog.Free;
end;

procedure TFormModifyDICOM.Button3Click(Sender: TObject);
var CurDir: string;
    Log: TStringlist;
begin
  GetDir(0, CurDir);
  DeleteFile('modifier.log');
  MemoScript.Lines.SaveToFile('modifier.lua');
  if FileExists(curdir + '\USEDBASEIIIWITHOUTODBC') then
  begin
    RunProgramWaiting(dgateExe, CurDir, ['-lmodifier.log', '"--lua:readdicom('''+Form1.Table1.Fields[0].AsString+':'+Form1.Table4.Fields[0].AsString+''');dofile(''modifier.lua'')"']);
    ShowMessage('Output is displayed in server status window');
  end
  else
  begin
    RunProgramWaiting(dgateExe, CurDir, ['-lmodifier.log', '"--dolua:readdicom('''+Form1.Table1.Fields[0].AsString+':'+Form1.Table4.Fields[0].AsString+''');dofile(''modifier.lua'')"']);
    Log := TStringlist.Create;
    if FileExists('modifier.log') then
    begin
      Log.LoadFromFile('modifier.log');
      ShowMessage(Log.GetText);
      DeleteFile('modifier.log');
    end
    else
      ShowMessage('Syntax is OK');
    Log.Free;
  end;
end;

procedure TFormModifyDICOM.FormActivate(Sender: TObject);
begin
  if FileExists('modifier.lua') then
    MemoScript.Lines.LoadFromFile('modifier.lua');
  AltPressed := False;
end;

procedure TFormModifyDICOM.OKButtonClick(Sender: TObject);
begin
  MemoScript.Lines.SaveToFile('modifier.lua');
end;

procedure TFormModifyDICOM.OKButtonMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  AltPressed := ssAlt in Shift;
end;

end.
