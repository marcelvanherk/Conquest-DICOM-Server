{************************************************************************}
{*                                                                      *}
{*      file       : DgateServ.PAS                                      *}
{*                                                                      *}
{*      type       : CONSOLE                                            *}
{*                                                                      *}
{*      location   : \QUIRT\APP\CONQUEST\DICOMSERVER                    *}
{*                                                                      *}
{*      purpose    : Install/run shell for Dgate Dicom server as service*}
{*                                                                      *}
{*      author     : Marcel van Herk (NKI / AVL)                        *}
{*                                                                      *}
{*      date       : 20000316                                           *}
{*                                                                      *}
{*      portability: 32 bit delphi only (V3.0 up)                       *}
{*                                                                      *}
{*      notes      : None                                               *}
{*                                                                      *}
{************************************************************************}
{* Updates:
When            Who     What
20000316        mvh     Created
20000326        mvh     Close handle on terminate
                mvh     Dgate runs via new SlaveProcess mechanism of Uservice
20101113        mvh     Fix for vista - stop does not reach service                
*}

program DgateServ;

uses
  Windows,
  SysUtils,
  uService;

{$APPTYPE CONSOLE}

type
  TDgateService = class(TService)
  protected
    procedure RunService; override;
    procedure DoneService; override;
  end;

procedure TDgateService.RunService;
begin
  while not WaitForStop(1000) do
end;

procedure TDgateService.DoneService;
begin
end;

var
  Service: TDgateService;

begin
  Service:=TDgateService.Create('DgateSERV','Dgate service',stAuto,False);

  // allow command line override of default slave process and args
  Service.AllowSlaveProcess := true;

  // default slave process and args
  Service.SlaveProcess := 'dgate.exe';
  Service.SlaveProcessArgs := '-!1111';

  if not Service.ProcessCmdLine then
  begin
    Service.Run;
    Service.ServiceStop;
    Service.DoneService;
    if Service.FThreadedProcess<>0 then
    begin
      TerminateProcess(Service.FThreadedProcess, 0);
      CloseHandle(Service.FThreadedProcess);
      Service.FThreadedProcess := 0;
      //Service.AddToMessageLog('Stopped dgate',EVENTLOG_ERROR_TYPE,0,0);
    end;
  end;

  Service.Free;
end.
