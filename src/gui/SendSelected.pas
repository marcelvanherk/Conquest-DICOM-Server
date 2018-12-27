unit SendSelected;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, ExtCtrls;

type
  TSendSelectedForm = class(TForm)
    Panel1: TPanel;
    Panel2: TPanel;
    ListBox: TListBox;
    CancelButton: TButton;
    OKButton: TButton;
    SelectAllButton: TButton;
    SelectDownButton: TButton;
    SelectUpButton: TButton;
    OtherDownButton: TButton;
    OtherUpButton: TButton;
    DeselectAllButton: TButton;
    DeselectDownButton: TButton;
    DeselectUpButton: TButton;
    ComboBox1: TComboBox;
    Label1: TLabel;
    IP: TLabel;
    PORT: TLabel;
    AE: TLabel;
    procedure SelectAllButtonClick(Sender: TObject);
    procedure SelectDownButtonClick(Sender: TObject);
    procedure SelectUpButtonClick(Sender: TObject);
    procedure OtherDownButtonClick(Sender: TObject);
    procedure OtherUpButtonClick(Sender: TObject);
    procedure DeselectAllButtonClick(Sender: TObject);
    procedure DeselectDownButtonClick(Sender: TObject);
    procedure DeselectUpButtonClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  SendSelectedForm: TSendSelectedForm;

implementation

{$R *.DFM}

procedure TSendSelectedForm.SelectAllButtonClick(Sender: TObject);
var i, j: integer;
begin
  j := ListBox.ItemIndex;
  for i:=0 to ListBox.Items.Count-1 do ListBox.Selected[i] := true;
  ListBox.ItemIndex := j;
end;

procedure TSendSelectedForm.SelectDownButtonClick(Sender: TObject);
var i, j: integer;
begin
  j := ListBox.ItemIndex;
  for i:=j to ListBox.Items.Count-1 do ListBox.Selected[i] := true;
  ListBox.ItemIndex := j;
end;

procedure TSendSelectedForm.SelectUpButtonClick(Sender: TObject);
var i, j: integer;
begin
  j := ListBox.ItemIndex;
  for i:=ListBox.ItemIndex downto 0 do ListBox.Selected[i] := true;
  ListBox.ItemIndex := j;
end;

procedure TSendSelectedForm.OtherDownButtonClick(Sender: TObject);
var i, j: integer;
begin
  j := ListBox.ItemIndex;
  for i:=j to ListBox.Items.Count-1 do ListBox.Selected[i] := not odd(i-j);
  ListBox.ItemIndex := j;
end;

procedure TSendSelectedForm.OtherUpButtonClick(Sender: TObject);
var i, j: integer;
begin
  j := ListBox.ItemIndex;
  for i:=j downto 0 do ListBox.Selected[i] := not odd(j-i);
  ListBox.ItemIndex := j;
end;

procedure TSendSelectedForm.DeselectAllButtonClick(Sender: TObject);
var i, j: integer;
begin
  j := ListBox.ItemIndex;
  for i:=0 to ListBox.Items.Count-1 do ListBox.Selected[i] := false;
  ListBox.ItemIndex := j;
end;

procedure TSendSelectedForm.DeselectDownButtonClick(Sender: TObject);
var i, j: integer;
begin
  j := ListBox.ItemIndex;
  for i:=j to ListBox.Items.Count-1 do ListBox.Selected[i] := false;
  ListBox.ItemIndex := j;
end;

procedure TSendSelectedForm.DeselectUpButtonClick(Sender: TObject);
var i, j: integer;
begin
  j := ListBox.ItemIndex;
  for i:=j downto 0 do ListBox.Selected[i] := false;
  ListBox.ItemIndex := j;
end;

end.
