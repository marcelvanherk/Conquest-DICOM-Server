// mvh 20020825: added keyboard interface (see serverdriver.pas)
// 20160313	mvh	Added F9 is close
// 20181110	mvh	Upped search limit to 100 MB

unit HeaderLister;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls;

type
  TForm3 = class(TForm)
    Memo1: TMemo;
    procedure Memo1KeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form3: TForm3;

implementation

uses Serverdriver;

{$R *.DFM}

var s: string;
    l: integer = 0;

procedure TForm3.Memo1KeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
var i:integer;
    f: TextFile;
begin
  if (Key = VK_F3) or ((Key = Ord('F')) and (ssCtrl in Shift)) then
  begin
    if (Length(s)=0) or (Key=Ord('F')) then
      if not InputQuery('Search', 'Text', s) then exit;

    l := Memo1.SelStart + 1;

    i := pos(Lowercase(s), Lowercase(Copy(Memo1.text, l+1, 100000000)));
    if i>0 then
    begin
      Memo1.SelStart := l+i-1;
      Memo1.SelLength := Length(s);
      l := l + i - 1 + Length(s);
    end
    else
    begin
      ShowMessage('Text not found');
      l := 1;
    end;
  end;

  if (Key=VK_ESCAPE) or (Key=VK_F9) then
    Form3.Hide;

  if (Key=Ord('A')) and (ssCtrl in Shift) then
    Memo1.SelectAll;

  if (Key=Ord('C')) and (ssCtrl in Shift) then
  begin
    if Memo1.SelLength=0 then Memo1.SelectAll;
    Memo1.CopyToClipboard;
  end;

  if (Key=Ord('S')) and (ssCtrl in Shift) then
  begin
    Form1.SaveDialog1.Title := 'Save DICOM header as text file';
    Form1.SaveDialog1.DefaultExt := 'TXT';
    Form1.SaveDialog1.Filter := 'Text files (*.txt)|*.TXT';
    Form1.SaveDialog1.FilterIndex := 0;
    Form1.SaveDialog1.Filename := '';

    if Form1.SaveDialog1.Execute then
    begin
      if StrPos(Pchar(Form1.SaveDialog1.Filename), '.')=nil then
        Form1.SaveDialog1.Filename := Form1.SaveDialog1.Filename + '.txt';
      Screen.Cursor := crHourGlass;
      AssignFile(f, Form1.SaveDialog1.Filename);
      Rewrite(f);
      for i:=0 to Memo1.Lines.Count-1 do
        writeln(f, Memo1.Lines[i]);
      CloseFile(f);
      Screen.Cursor := crDefault;
    end;
  end;

  if Key=VK_F1 then
    ShowMessage('Use of this header lister: ' + #13 + #13 +
                'CTRL-A: Select all' + #13 +
                'CTRL-C: Copy text to clipboard' + #13 +
                'CTRL-F: Find' + #13 +
                'CTRL-S: Save text to file' + #13 +
                'FN3      : Find again' + #13);
end;

end.

