// 20150719     mvh     Always operate readonly
// 20160306	mvh	Send click messages to main form to control image visibility
// 20160319	mvh	F11 is close
// 20161120     mvh     Hidden edit buttons; updated layout image list

unit EditDatabaseForm;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, DBCtrls, ExtCtrls, Grids, DBGrids, Db;

type
  TForm4 = class(TForm)
    DBGrid2: TDBGrid;
    DBNavigator1: TDBNavigator;
    DBNavigator2: TDBNavigator;
    DBLookupComboBox2: TDBLookupComboBox;
    DBGrid1: TDBGrid;
    DBNavigator3: TDBNavigator;
    DBLookupComboBox3: TDBLookupComboBox;
    DBGrid3: TDBGrid;
    DBNavigator4: TDBNavigator;
    DBLookupComboBox4: TDBLookupComboBox;
    Label6: TLabel;
    Label7: TLabel;
    Label8: TLabel;
    Label9: TLabel;
    DBGrid4: TDBGrid;
    procedure FormShow(Sender: TObject);
    procedure FormHide(Sender: TObject);
    procedure DBGrid1CellClick(Column: TColumn);
    procedure DBGrid4CellClick(Column: TColumn);
    procedure FormKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form4: TForm4;

implementation

uses Serverdriver;

{$R *.DFM}

var B1, B2, B3, B4: TBookMark;

procedure TForm4.FormShow(Sender: TObject);
begin
  //if FileExists(curdir + '\USEDBASEIIIWITHOUTODBC') or (BrowseThroughDBF='1') then
  begin
    Form4.Caption := 'Database is read only - for viewing only';
    Enabled := true;
    exit;
  end;

  {B4 := Form1.Table4.BookMark;
  B3 := Form1.Table3.BookMark;
  B2 := Form1.Table2.BookMark;
  B1 := Form1.Table1.BookMark;

  Form1.Table4.Close;
  Form1.Table3.Close;
  Form1.Table2.Close;
  Form1.Table1.Close;

  Form1.BDETable4.Readonly := false;
  Form1.BDETable3.Readonly := false;
  Form1.BDETable2.Readonly := false;
  Form1.BDETable1.Readonly := false;

  Form1.ADOTable4.Readonly := false;
  Form1.ADOTable3.Readonly := false;
  Form1.ADOTable2.Readonly := false;
  Form1.ADOTable1.Readonly := false;

  Form1.Table1.Open;
  Form1.Table2.Open;
  Form1.Table3.Open;
  Form1.Table4.Open;

  Enabled := true;

  Form1.Table1.BookMark := B1;
  Form1.Table2.BookMark := B2;
  Form1.Table3.BookMark := B3;
  Form1.Table4.BookMark := B4;
  }
end;

// series
procedure TForm4.DBGrid1CellClick(Column: TColumn);
begin
  Form1.DBGrid1DblClick(self);
end;

// study
procedure TForm4.DBGrid4CellClick(Column: TColumn);
begin
  //Form1.DBGrid2DblClick(self)
end;

procedure TForm4.FormHide(Sender: TObject);
begin
  {if FileExists(curdir + '\USEDBASEIIIWITHOUTODBC') then exit;

  B4 := Form1.Table4.BookMark;
  B3 := Form1.Table3.BookMark;
  B2 := Form1.Table2.BookMark;
  B1 := Form1.Table1.BookMark;

  Form1.Table4.Close;
  Form1.Table3.Close;
  Form1.Table2.Close;
  Form1.Table1.Close;

  Form1.BDETable4.Readonly := true;
  Form1.BDETable3.Readonly := true;
  Form1.BDETable2.Readonly := true;
  Form1.BDETable1.Readonly := true;

  Form1.ADOTable4.Readonly := true;
  Form1.ADOTable3.Readonly := true;
  Form1.ADOTable2.Readonly := true;
  Form1.ADOTable1.Readonly := true;

  Form1.Table1.Open;
  Form1.Table2.Open;
  Form1.Table3.Open;
  Form1.Table4.Open;

  Enabled := false;

  Form1.Table1.BookMark := B1;
  Form1.Table2.BookMark := B2;
  Form1.Table3.BookMark := B3;
  Form1.Table4.BookMark := B4;
  }
end;

procedure TForm4.FormKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
  if Key=VK_F11 then
  begin
    Key := 0;
    Close;
  end;
end;

end.
