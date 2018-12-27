
program ConquestDICOMServer;

uses
  forms in 'c:\Program Files\Borland\Delphi5\Source\Vcl\forms.pas',
  Serverdriver in 'Serverdriver.pas' {Form1},
  ServerAbout in 'ServerAbout.pas' {Form2},
  HeaderLister in 'HeaderLister.pas' {Form3},
  AboutNew in 'AboutNew.pas' {NewInstallBox},
  SendSelected in 'SendSelected.pas' {SendSelectedForm},
  EditDatabaseForm in 'EditDatabaseForm.pas' {Form4},
  controls in 'c:\Program Files\Borland\Delphi5\Source\Vcl\controls.pas',
{$IFDEF KPACS}
  Uviewer in 'Uviewer.pas' {FormKViewer},
{$ENDIF KPACS}
  ModifyDicom in 'ModifyDicom.pas' {FormModifyDICOM};

// delphi4:
//  EditDatabaseForm in 'EditDatabaseForm.pas'; {Form4}


{$R *.RES}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.CreateForm(TForm2, Form2);
  Application.CreateForm(TForm3, Form3);
  Application.CreateForm(TSendSelectedForm, SendSelectedForm);
  Application.CreateForm(TForm4, Form4);
{$IFDEF KPACS}
  Application.CreateForm(TFormKViewer, FormKViewer);
{$ENDIF KPACS}
  Application.CreateForm(TFormModifyDICOM, FormModifyDICOM);
  Application.Run;
end.
