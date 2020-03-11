{ Modifications
  19990316    mvh    Fixed English of some messages
                     Added account and password as optional parameters for install
  19990325    mvh    Added option to start/stop/install/uninstall on remote machine
                     Works fine with start / stop / uninstall
                     Remote install works except for wrong exe path
                     Therefore added ExeName property and command line switch
  19990326    mvh    Added service option: makes it a generic installer!
                     Made ServiceName and ServiceDisplayName read/write
                     Added AllowSlaveProcess: command line can set slave process
                     Added SlaveProcess and SlaveProcessArgs: executable to start
                     Fixed arguments passing when /start is followed by more /
                     Fixed ServiceName: is read as first arg in DoCmdLineStart
                     Note: SlaveProcess is killed when service stops
  20100827    mvh    WaitforStop also tests the SlaveProcess: service stops when slave crashes
                     Version in help text: now V1.1
  20100901    mvh    Avoid RaiseLastWin32Error for common errors: it now shows a crash window
  20101111    mvh    Avoid RaiseLastWin32Error for failure to start, use ShowMessage
  20101113    mvh    Fix for vista - stop does not reach service                
  20101116    mvh    Stop before uninstall fixes uninstall for vista                
  20200311    mvh    Reverted to delphi5 version - note original author unknown
}

unit uService;

interface

uses Windows, WinSvc, SysUtils, Dialogs;

{$IFDEF WIN32}
  {$IFNDEF VER90} //Delphi2
     {$DEFINE DELPHI3UP}
     {$IFNDEF VER100}
       {$DEFINE DELPHI4UP}
     {$ENDIF}
  {$ENDIF}
{$ELSE}
  Sorry, this code required Win32.
{$ENDIF}

type
  PPChar = ^PChar;
  TService = class;
  TStopCallbackFunc = function (Service: TService; CurrentState: integer; Param: pointer): integer;
  TStartType = (stBoot, stSystem, stAuto, stManual, stDisabled);
  TService = class(TObject)
  private
    FServiceName: string;
    FServiceDisplayName: string;
    FAccount: string;
    FPassword: string;
    FMachineName: string;
    FExeName: string;
    FStartType: TStartType;
    FInteractive: boolean;
    FSlaveProcess: string;
    FSlaveProcessArgs: string;
    FAllowSlaveProcess: boolean;

    FServiceMainWrapper: TServiceMainFunction;
    FServiceHandlerWrapper: THandlerFunction;
    procedure CreateWrapperCode;
  protected

    FServiceStopEventHandle: THandle;
    ServiceStatusHandle: SERVICE_STATUS_HANDLE;
    ssStatus: TServiceStatus;
    InitOK: boolean;

    procedure ServiceMain(Argc: DWORD; Argv: PPChar); virtual;
    procedure Handler(dwCtrlCode: DWORD); virtual;

    procedure ServiceStop; virtual;
    procedure RunService; virtual; abstract;
    procedure InitService; virtual;
    procedure DoneService; virtual;

    function ReportStatusToSCMgr(dwState, dwExitCode, dwWait: DWORD): BOOL;

    procedure DoCmdLineStart(ParamCount: DWORD; Parameters: PChar); virtual;
    procedure DoCmdLineStop; virtual;
    procedure DoCmdLineInstall; virtual;
    procedure DoCmdLineUnInstall; virtual;
    procedure DoCmdLineHelp; virtual;

  public
    FThreadedProcess: integer;
    {$IFDEF DEBUG}
    IsTest: boolean;
    {$ENDIF}
    constructor Create(const AServiceName, AServiceDisplayName: string; AStartType: TStartType; AInteractive: boolean);
    destructor Destroy; override;

    function Install(StartType: TStartType; Interactive: boolean): integer; virtual;
    function UnInstall(StopCallbackFunc: TStopCallbackFunc; Param: pointer): integer; virtual;
    procedure AddToMessageLogPChar(Msg: PChar; const EventType, Category, EventID: DWord);
    procedure AddToMessageLog(const Msg: string; const EventType, Category, EventID: DWord);


    function ProcessCmdLine: boolean; virtual;

    procedure Run;
    function WaitForStop(TimeOut: DWord): boolean;

    property ServiceName: string read FServiceName write FServiceName;
    property ServiceDisplayName: string read FServiceDisplayName write FServiceDisplayName;
    property StartType:   TStartType read FStartType;
    property Interactive: boolean read FInteractive;
    property Account:     string read FAccount write FAccount;
    property PassWord:    string read FPassword write FPassword;
    property MachineName: string read FMachineName write FMachineName;
    property ExeName:     string read FExeName write FExeName;

    property AllowSlaveProcess: boolean read FAllowSlaveProcess write FAllowSlaveProcess;
    property SlaveProcess: string read FSlaveProcess write FSlaveProcess;
    property SlaveProcessArgs: string read FSlaveProcessArgs write FSlaveProcessArgs;
  end;

const
  //
  // Service Types (Bit Mask)
  //
  SERVICE_KERNEL_DRIVER          = $00000001;
  SERVICE_FILE_SYSTEM_DRIVER     = $00000002;
  SERVICE_ADAPTER                = $00000004;
  SERVICE_RECOGNIZER_DRIVER      = $00000008;
  SERVICE_DRIVER                 = (SERVICE_KERNEL_DRIVER+SERVICE_FILE_SYSTEM_DRIVER+SERVICE_RECOGNIZER_DRIVER);

  SERVICE_WIN32_OWN_PROCESS      = $00000010;
  SERVICE_WIN32_SHARE_PROCESS    = $00000020;
  SERVICE_WIN32                  = (SERVICE_WIN32_OWN_PROCESS+SERVICE_WIN32_SHARE_PROCESS);
  SERVICE_INTERACTIVE_PROCESS    = $00000100;

  SERVICE_TYPE_ALL               = (SERVICE_WIN32+SERVICE_ADAPTER+SERVICE_DRIVER+SERVICE_INTERACTIVE_PROCESS);

  //
  // Start Type
  //
  SERVICE_BOOT_START             = $00000000;
  SERVICE_SYSTEM_START           = $00000001;
  SERVICE_AUTO_START             = $00000002;
  SERVICE_DEMAND_START           = $00000003;
  SERVICE_DISABLED               = $00000004;

  //
  // Error control type
  SERVICE_ERROR_IGNORE           = $00000000;
  SERVICE_ERROR_NORMAL           = $00000001;
  SERVICE_ERROR_SEVERE           = $00000002;
  SERVICE_ERROR_CRITICAL         = $00000003;


  EVENTLOG_SUCCESS          = $0000;
  EVENTLOG_ERROR_TYPE       = $0001;
  EVENTLOG_WARNING_TYPE     = $0002;
  EVENTLOG_INFORMATION_TYPE = $0004;
  EVENTLOG_AUDDIT_SUCCESS   = $0008;
  EVENTLOG_AUDIT_FAILURE    = $0010;

function GetLastErrorText: string;
function DefStopCallbackFunc(Service: TService; CurrentState: integer; Param: pointer): integer;

type
  TSysCharSet = set of char;

function FindCmdLineSwitch(const Switch: string; SwitchChars: TSysCharSet; IgnoreCase: Boolean): integer;


implementation

const
  MagicConst = $789ABCDE;

procedure ServiceMainWrapperCode(Argc: DWORD; Argv: PPChar); stdcall;
begin
  TService(pointer(MagicConst)).ServiceMain(Argc,Argv);
end;

procedure HandlerWrapperCode(dwCtrlCode: DWORD); stdcall;
begin
  TService(pointer(MagicConst)).Handler(dwCtrlCode);
end;

procedure DummyProc;
begin
end;

// Get error message
function GetLastErrorText: string;
begin
  SetLength(Result,512);
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM or FORMAT_MESSAGE_ARGUMENT_ARRAY,
    nil,GetLastError,LANG_NEUTRAL,PChar(Result),Length(Result),nil);
  Result:=PChar(Result);
end;

function DefStopCallbackFunc(Service: TService; CurrentState: integer; Param: pointer): integer;
begin
  case CurrentState of
    -1 :             WriteLn('Service '+Service.ServiceDisplayName+' is not stopped. Trying to stop.');
    SERVICE_STOPPED: WriteLn('Service '+Service.ServiceDisplayName+' is stopped.');
    else             Write('.');
  end;
  Result:=1000;
end;

function FindCmdLineSwitch(const Switch: string; SwitchChars: TSysCharSet; IgnoreCase: Boolean): integer;
var
  S: string;
  Found: boolean;
begin
  Result:=1; Found:=False;
  while not Found and (Result<=ParamCount) do begin
    S:=ParamStr(Result);
    Found:=(SwitchChars=[]) or (S[1] in SwitchChars);
    if SwitchChars<>[] then
      S:=Copy(S,2,Length(S)-1);
    Found:=Found and
       ((IgnoreCase and (AnsiCompareText(S,Switch)=0)) or
        (not IgnoreCase and (AnsiCompareStr(S,Switch)=0)));
    if not Found then
      inc(Result);
  end;
  if not Found then
    Result:=-1;
end;

constructor TService.Create(const AServiceName, AServiceDisplayName: string; AStartType: TStartType; AInteractive: boolean);
begin
  inherited Create;
  FServiceName:=AServiceName; {$IFDEF DELPHI3UP} assert(FServiceName<>'');  {$ENDIF}
  FServiceDisplayName:=AServiceDisplayName; {$IFDEF DELPHI3UP}  assert(FServiceDisplayName<>''); {$ENDIF}
  FStartType:=AStartType;
  FInteractive:=AInteractive;
  CreateWrapperCode;
end;

destructor TService.Destroy;
begin
  if Assigned(FServiceMainWrapper) then
    VirtualFree(Pointer(FServiceMainWrapper),0,MEM_RELEASE);
  FServiceMainWrapper:=nil;
  inherited;
end;

procedure TService.CreateWrapperCode;
  function Replace(Mem: PChar; Size: integer; FromI, ToI : integer): integer;
  begin
    Result:=0;
    dec(Size,sizeof(integer)-1);
    while Size>0 do begin
      if PInteger(Mem)^=FromI then begin
        PInteger(Mem)^:=ToI;
        inc(Result);
        inc(Mem,sizeof(integer)-1); dec(Size,sizeof(integer)-1);
      end;
      inc(Mem); dec(Size);
    end;
  end;
var
  CodeLen: integer;
  ServiceMainWrapperAddr: PChar;
  HandlerWrapperAddr: PChar;
  DummyProcAddr: PChar;
  OldProtect: DWord;
begin
  ServiceMainWrapperAddr:=PChar(@ServiceMainWrapperCode);
  HandlerWrapperAddr:=PChar(@HandlerWrapperCode);
  DummyProcAddr:=PChar(@DummyProc);
  CodeLen:=DummyProcAddr-ServiceMainWrapperAddr;
  Pointer(FServiceMainWrapper):=VirtualAlloc(nil,CodeLen,MEM_COMMIT,PAGE_READWRITE);
  {$IFDEF DELPHI3UP} Assert(Assigned(FServiceMainWrapper)); {$ENDIF}
  Move(ServiceMainWrapperAddr^,PChar(FServiceMainWrapper)^,CodeLen);
  PChar(FServiceHandlerWrapper):=PChar(FServiceMainWrapper);
  inc(PChar(FServiceHandlerWrapper),HandlerWrapperAddr-ServiceMainWrapperAddr);
  if Replace(PChar(FServiceMainWrapper),HandlerWrapperAddr-ServiceMainWrapperAddr,MagicConst,integer(self))<>1 then
    {$IFDEF DELPHI3UP} Assert(False) {$ENDIF};
  if Replace(PChar(FServiceHandlerWrapper),DummyProcAddr-HandlerWrapperAddr,MagicConst,integer(self))<>1 then
    {$IFDEF DELPHI3UP} Assert(False) {$ENDIF};
  if not VirtualProtect(Pointer(FServiceMainWrapper),CodeLen,PAGE_EXECUTE,@OldProtect) then
    {$IFDEF DELPHI3UP} Assert(False) {$ENDIF};
end;

function RunProcess(exe, args: string): integer;
  var sinfo: tstartupInfo;
  var pinfo : TProcessInformation;
  s, p: string;
begin
  ZeroMemory(@sinfo, sizeof(TStartupInfo));
  ZeroMemory(@pinfo, sizeof(TProcessInformation));
  sinfo.cb      := sizeof(TStartupInfo);
  p := ExtractFileDir(ParamStr(0)) + '\' + exe;
  s := p + ' ' + args;

  ChDir(ExtractFileDir(ParamStr(0)));

  if not CreateProcess(Pchar(p), Pchar(s), nil, nil, true,
                 DETACHED_PROCESS, nil, nil, sinfo, pinfo) then
  begin
    // ShowMessage('Failed to start process ' + p);
    result := 0;
  end
  else
    result := pinfo.hProcess;

  CloseHandle(pinfo.hThread);
end;

procedure TService.DoCmdLineStart(ParamCount: DWORD; Parameters: PChar);
var
  schSCManager: SC_HANDLE;
  schService: SC_HANDLE;
  ssStatus: TServiceStatus;
  ServiceStartOK: boolean;
  Machine: PChar;
begin
  Machine := nil;
  if Length(FMachineName)>0 then Machine := Pchar(FMachineName);
  schSCManager:=OpenSCManager(Machine,nil,SC_MANAGER_ALL_ACCESS);
  if (schSCManager=0) then
  begin
    //RaiseLastWin32Error;
    WriteLn('Cannot open service manager');
    WriteLn(GetLastErrorText);
    exit;
  end;
  try
    schService:=OpenService(schSCManager,PChar(FServiceName),SERVICE_ALL_ACCESS);
    if (schService=0) then
    begin
      //RaiseLastWin32Error;
      WriteLn('Service '+ServiceDisplayName+' does not exist.');
      WriteLn(GetLastErrorText);
      exit;
    end
    else
    try
      if not QueryServiceStatus(schService,ssStatus) then
      begin
        //RaiseLastWin32Error;
        WriteLn('No access to Service ' + ServiceDisplayName);
        WriteLn(GetLastErrorText);
        exit;
      end;
      WriteLn('Service '+IntToStr(ssStatus.dwCurrentState));
      if ssStatus.dwCurrentState<>SERVICE_STOPPED then begin
        WriteLn('Service '+ServiceDisplayName+' is not stopped.');
        exit;
      end;
      WriteLn('Service '+ServiceDisplayName+' is not running, trying to start.');
      if Assigned(Parameters) and (ParamCount>0)  then
        ServiceStartOK:=StartService(schService,ParamCount,PPChar(Parameters)^)
      else
        ServiceStartOK:=StartService(schService,0,Parameters);
      if ServiceStartOK then
        Writeln('Starting service.')
      else
        Writeln('Failed to start service ' + ServiceDisplayName);
        //RaiseLastWin32Error;
        Writeln(GetLastErrorText);
    finally
      CloseServiceHandle(schService);
    end;
  finally
    CloseServiceHandle(schSCManager);
  end;
end;

procedure TService.DoCmdLineStop;
var
  schSCManager: SC_HANDLE;
  schService: SC_HANDLE;
  ssStatus: TServiceStatus;
  Machine: PChar;
begin
  Machine := nil;
  if Length(FMachineName)>0 then Machine := Pchar(FMachineName);
  schSCManager:=OpenSCManager(Machine,nil,SC_MANAGER_ALL_ACCESS);
  if (schSCManager=0) then
  begin
    //RaiseLastWin32Error;
    WriteLn('Cannot open service manager');
    WriteLn(GetLastErrorText);
    exit;
  end;
  try
    schService:=OpenService(schSCManager,PChar(FServiceName),SERVICE_ALL_ACCESS);
    if (schService=0) then
    begin
      // RaiseLastWin32Error;
      WriteLn('Service '+ServiceDisplayName+' does not exist.');
      exit;
    end
    else
    try
      if not QueryServiceStatus(schService,ssStatus) then
      begin
        WriteLn('Cannot access Service '+ServiceDisplayName);
        WriteLn(GetLastErrorText);
        //RaiseLastWin32Error;
        exit;
      end;
      if ssStatus.dwCurrentState<>SERVICE_STOPPED then
      begin
        WriteLn('Service '+ServiceDisplayName+' is not stopped, trying to stop.');
          if not ControlService(schService,SERVICE_CONTROL_STOP,ssStatus) then
          begin
            WriteLn('Error stopping - but the service stops anyway: '+ServiceDisplayName+'.');
            //WriteLn(GetLastErrorText);
          end;
        Sleep(100);
        if not QueryServiceStatus(schService,ssStatus) then
        begin
          WriteLn('Cannot access Service '+ServiceDisplayName);
          WriteLn(GetLastErrorText);
          //RaiseLastWin32Error;
          exit;
        end;
        while (ssStatus.dwCurrentState<>SERVICE_STOPPED) do begin
          Write('.' + IntToStr(ssStatus.dwCurrentState) + '.');
          Sleep(500);
          if not QueryServiceStatus(schService,ssStatus) then
          begin
            WriteLn('Cannot access Service '+ServiceDisplayName);
            WriteLn(GetLastErrorText);
            //RaiseLastWin32Error;
            exit;
          end;
          Writeln('Waiting for service '+ServiceDisplayName+' to stop.');
        end;
        WriteLn;
      end; //end not stopped
    finally
      CloseServiceHandle(schService);
    end;
  finally
    CloseServiceHandle(schSCManager);
  end;
  //Writeln('Service '+ServiceDisplayName+' stopped.');
end;

procedure TService.DoCmdLineInstall;
var
  SuccessStatus: integer;
begin
  SuccessStatus:=Install(StartType,Interactive);
  if SuccessStatus<>0 then begin
    WriteLn('Service '+ServiceDisplayName+' install error ('+IntToStr(SuccessStatus)+') :');
    WriteLn(GetLastErrorText);
  end
  else
    WriteLn('Service '+ServiceDisplayName+' installed.');
end;

procedure TService.DoCmdLineUnInstall;
var
  SuccessStatus: integer;
begin
  DoCmdLineStop;
  
  SuccessStatus:=UnInstall(DefStopCallbackFunc,nil);
  if SuccessStatus<>0 then begin
    WriteLn('Service '+ServiceDisplayName+' uninstall error ('+IntToStr(SuccessStatus)+') :');
    WriteLn(GetLastErrorText);
  end
  else
    WriteLn('Service '+ServiceDisplayName+' uninstalled.');
end;

procedure TService.DoCmdLineHelp;
var
  ServExe: string;
begin
  ServExe:=ChangeFileExt(ExtractFileName(ParamStr(0)),'');
  Writeln('----------------------------------------------------------------------------');
  Writeln(ServExe+' v1.2 usage help');
  Writeln('----------------------------------------------------------------------------');
  Writeln(ServExe+' /install                               : install service');
  Writeln(ServExe+' /install DomainName\Username Password  : install service in account');
  Writeln(ServExe+' /uninstall                             : uninstall service');
  Writeln(ServExe+' /start                                 : start service');
  Writeln(ServExe+' /start arg1 arg2 etc                   : start with command line args');
  Writeln(ServExe+' /stop                                  : stop service');
  Writeln(ServExe+' /?                                     : this help');
  Writeln('');
  Writeln('Optional flags:');
  Writeln('/service NAME     : servicename; use to install a service more times');
  Writeln('/machine NAME     : remote machine on which service (will) run');
  Writeln('/exename NAME.EXE : local exe path (use with /install for remote machine)');

  if AllowSlaveProcess then
    Writeln('/process NAME args: overrule exe to run as slave (use with /start)');
end;

function TService.ProcessCmdLine: boolean;
var
  ParamIndex: integer;
  i, count: integer;
  {$IFDEF DELPHI4UP}
  Params: array of string;
  t: string;
  {$ELSE}
  Params: PChar;
  TmpPString: PString;
  {$ENDIF}
  SlavePars: array of string;
begin
  Result:=True;

  ParamIndex:=FindCmdLineSwitch('MACHINE',['/','-'],True);
  if ParamIndex<>-1 then
    MachineName := ParamStr(ParamIndex+1);

  ParamIndex:=FindCmdLineSwitch('EXENAME',['/','-'],True);
  if ParamIndex<>-1 then
    ExeName := ParamStr(ParamIndex+1);

  if AllowSlaveProcess then
  begin
    ParamIndex:=FindCmdLineSwitch('PROCESS',['/','-'],True);
    if ParamIndex<>-1 then
    begin
      SlaveProcessArgs := '';
      inc(ParamIndex);
      SlaveProcess := ParamStr(ParamIndex);
      inc(ParamIndex);

      while ParamIndex <= ParamCount do
      begin
        if ParamStr(ParamIndex)[1] in ['/','-'] then
        begin
          if uppercase(copy(ParamStr(ParamIndex), 2, 250))='MACHINE'   then break;
          if uppercase(copy(ParamStr(ParamIndex), 2, 250))='EXENAME'   then break;
          if uppercase(copy(ParamStr(ParamIndex), 2, 250))='PROCESS'   then break;
          if uppercase(copy(ParamStr(ParamIndex), 2, 250))='SERVICE'   then break;
          if uppercase(copy(ParamStr(ParamIndex), 2, 250))='START'     then break;
          if uppercase(copy(ParamStr(ParamIndex), 2, 250))='STOP'      then break;
          if uppercase(copy(ParamStr(ParamIndex), 2, 250))='INSTALL'   then break;
          if uppercase(copy(ParamStr(ParamIndex), 2, 250))='UNINSTALL' then break;
          if uppercase(copy(ParamStr(ParamIndex), 2, 250))='?'         then break;
{$IFDEF DEBUG}
          if uppercase(copy(ParamStr(ParamIndex), 2, 250))='TEST'      then break;
{$ENDIF}
        end;

        if Length(SlaveProcessArgs)<>0 then
          SlaveProcessArgs := SlaveProcessArgs + ' ';
        SlaveProcessArgs := SlaveProcessArgs + ParamStr(ParamIndex);

        inc(ParamIndex);
      end;
      SetLength(SlavePars, 3);
      SlavePars[0] := '*&#SlaveProcess#&*';
      SlavePars[1] := SlaveProcess;
      SlavePars[2] := SlaveProcessArgs;
    end;
  end;

  ParamIndex:=FindCmdLineSwitch('SERVICE',['/','-'],True);
  if ParamIndex<>-1 then
  begin
    ServiceName        := ParamStr(ParamIndex+1);
    ServiceDisplayName := ParamStr(ParamIndex+1);
  end;

  ParamIndex:=FindCmdLineSwitch('START',['/','-'],True);
  if ParamIndex<>-1 then
  begin
    count := 0;
    for i:=0 to ParamCount-ParamIndex-1 do
    begin
      t := ParamStr(i+ParamIndex+1);
      if uppercase(copy(t, 2, 250))='MACHINE'   then break;
      if uppercase(copy(t, 2, 250))='EXENAME'   then break;
      if uppercase(copy(t, 2, 250))='PROCESS'   then break;
      if uppercase(copy(t, 2, 250))='SERVICE'   then break;
      if uppercase(copy(t, 2, 250))='START'     then break;
      if uppercase(copy(t, 2, 250))='STOP'      then break;
      if uppercase(copy(t, 2, 250))='INSTALL'   then break;
      if uppercase(copy(t, 2, 250))='UNINSTALL' then break;
      if uppercase(copy(t, 2, 250))='?'         then break;
{$IFDEF DEBUG}
      if uppercase(t, 2, 250))='TEST'      then break;
{$ENDIF}
      count := i+1;
    end;

    if Length(SlaveProcess)>0 then
    begin
      if count>0 then Writeln('Warning: Options to /start are ignored if /process is used');
      DoCmdLineStart(3,PChar(SlavePars));
    end
    else
    begin
      {$IFDEF DELPHI4UP}
      SetLength(Params,count);
      for i:=0 to count-1 do
        Params[i]:=ParamStr(i+ParamIndex+1);
      DoCmdLineStart(count,PChar(Params));
      {$ELSE}
      GetMem(Params,SizeOf(string)*count);
      try
        TmpPString:=PString(Params);
        Initialize(TmpPString^,count);
        for i:=0 to count-1 do begin
          TmpPString^:=ParamStr(i+ParamIndex+1);
          inc(TmpPString);
        end;
        DoCmdLineStart(count,Params);
      finally
        TmpPString:=PString(Params);
        Finalize(TmpPString^,ParamCount-ParamIndex);
        FreeMem(Params);
      end;
      {$ENDIF}
    end;
  end
  else if FindCmdLineSwitch('STOP',['/','-'],True)<>-1 then
    DoCmdLineStop
  else if FindCmdLineSwitch('INSTALL',['/','-'],True)<>-1 then
  begin
    ParamIndex:= FindCmdLineSwitch('INSTALL',['/','-'],True);
    FAccount  := ParamStr(ParamIndex+1);
    FPassword := ParamStr(ParamIndex+2);
    DoCmdLineInstall;
  end
  else if FindCmdLineSwitch('UNINSTALL',['/','-'],True)<>-1 then
    DoCmdLineUnInstall
  else if (FindCmdLineSwitch('?',['/','-'],True)<>-1) or (FindCmdLineSwitch('HELP',['/','-'],True)<>-1) then
    DoCmdLineHelp
{$IFDEF DEBUG}
  else if FindCmdLineSwitch('TEST',['/','-'],True)<>-1 then begin
    IsTest:=True;
    Result:=False;
  end
{$ENDIF}
  else
    Result:=False;
end;

function TService.Install(StartType: TStartType; Interactive: boolean): integer;
var
  schSCManager: SC_HANDLE;
  schService: SC_HANDLE;
  ExeName: string;
  PAccount, PPassword: PChar;
  Machine: PChar;
begin
  Machine := nil;
  if Length(FMachineName)>0 then Machine := Pchar(FMachineName);

  if Length(FAccount) <>0 then PAccount  := PChar(FAccount)  else PAccount := nil;
  if Length(FPassWord)<>0 then PPassword := PChar(FPassword) else PPassword := nil;

  Result:=1;
  SetLength(ExeName,512);
  if GetModuleFileName(0,PChar(ExeName),Length(ExeName))<>0 then begin
    ExeName:=PChar(ExeName);
    if Pos(' ',ExeName)<>0 then //Space in file name
      ExeName:='"'+ExeName+'"';

    if Length(FExeName)>0 then ExeName := FExeName;

    Result:=2;
    schSCManager:=OpenSCManager(Machine,nil,SC_MANAGER_ALL_ACCESS);
    if (schSCManager>0) then begin
      try
        Result:=3;
        schService:=CreateService(schSCManager,PChar(FServiceName),PChar(FServiceDisplayName),
          SERVICE_ALL_ACCESS,SERVICE_WIN32_OWN_PROCESS+ord(Interactive)*SERVICE_INTERACTIVE_PROCESS,
          ord(StartType),SERVICE_ERROR_NORMAL,PChar(ExeName),nil,nil,nil,PAccount,PPassword);
        if (schService>0) then begin
          Result:=0;
          CloseServiceHandle(schService);
        end
      finally
        CloseServiceHandle(schSCManager);
      end;
    end;
  end;
end;

function TService.UnInstall(StopCallbackFunc: TStopCallbackFunc; Param: pointer): integer;
var
  schSCManager: SC_HANDLE;
  schService: SC_HANDLE;
  ssStatus: TServiceStatus;
  TimeOut: integer;
  Machine: PChar;
begin
  Machine := nil;
  if Length(FMachineName)>0 then Machine := Pchar(FMachineName);

  Result:=1;
  schSCManager:=OpenSCManager(Machine,nil,SC_MANAGER_ALL_ACCESS);

  if (schSCManager>0) then begin
    try
      Result:=2;
      schService:=OpenService(schSCManager,PChar(FServiceName),SERVICE_ALL_ACCESS);
      if (schService>0) then begin
        try
          Result:=3;
          if QueryServiceStatus(schService,ssStatus) then begin
            if ssStatus.dwCurrentState<>SERVICE_STOPPED then begin
              if Assigned(StopCallbackFunc) then StopCallbackFunc(Self,-1,Param);
              Result:=4;
              if ControlService(schService,SERVICE_CONTROL_STOP,ssStatus) then begin
                repeat
                  if Assigned(StopCallbackFunc) then
                    TimeOut:=StopCallbackFunc(Self,ssStatus.dwCurrentState,Param)
                  else
                    TimeOut:=1000;
                  if TimeOut>=0 then
                    Sleep(TimeOut)
                  else //abort
                    Result:=-1;
                  if (Result=4) and not QueryServiceStatus(schService,ssStatus) then
                    Result:=6;
                until (Result<>4) or (ssStatus.dwCurrentState=SERVICE_STOPPED);
                if (Result=4) and Assigned(StopCallbackFunc) then
                  StopCallbackFunc(Self,ssStatus.dwCurrentState,Param);
              end
              else
                if GetLastError<>ERROR_SERVICE_NOT_ACTIVE then
                  Result:=7;
            end; //end stop
            if (Result in [3,4]) and DeleteService(schService) then
              Result:=0
            else
              Result:=5;
          end; //QueryServiceStatus
        finally
          CloseServiceHandle(schService);
        end;
      end; //OpenService
    finally
      CloseServiceHandle(schSCManager);
    end;
  end; //OpenSCManager
end;

//********************************************************************

// Write error message to Windows NT Event Log
procedure TService.AddToMessageLogPChar(Msg: PChar; const EventType, Category, EventID: DWord);
var
  sString: PPChar;
  hEventSource: THandle;
begin
  hEventSource:=RegisterEventSource(nil,PChar(FServiceName));
  if hEventSource>0 then begin
    try
      sString:=@Msg;
      {$IFDEF DELPHI3UP}
      ReportEvent(hEventSource,EventType,Category,EventID,nil,1,0,sString,nil);
      {$ELSE}
      ReportEvent(hEventSource,EventType,Category,EventID,nil,1,0,sString^,nil);
      {$ENDIF}
    finally
      DeregisterEventSource(hEventSource);
    end;
  end;
end;

procedure TService.AddToMessageLog(const Msg: string; const EventType, Category, EventID: DWord);
begin
  AddToMessageLogPChar(PChar(Msg),EventType,Category,EventID);
end;

function TService.ReportStatusToSCMgr(dwState, dwExitCode, dwWait: DWORD): BOOL;
begin
  //AddToMessageLog('Calling ReportStatusToSCMgr',EVENTLOG_ERROR_TYPE,0,0);

  if (dwState=SERVICE_START_PENDING) then
    ssStatus.dwControlsAccepted:=0
  else
    ssStatus.dwControlsAccepted:=SERVICE_ACCEPT_STOP;
  ssStatus.dwCurrentState:=dwState;
  ssStatus.dwCurrentState:=dwState;
  ssStatus.dwWin32ExitCode:=dwExitCode;
  ssStatus.dwWaitHint:=dwWait;
  if (dwState=SERVICE_RUNNING) or (dwState=SERVICE_STOPPED) then
    ssStatus.dwCheckPoint:=0
  else
    inc(ssStatus.dwCheckPoint);
{$IFDEF DEBUG}
  if IsTest then
    Result:=True
  else
{$ENDIF}
    Result:=SetServiceStatus(ServiceStatusHandle,ssStatus);
  if not Result then
    AddToMessageLog('SetServiceStatus error',EVENTLOG_ERROR_TYPE,0,0);
end;

procedure TService.Run;
var
  ServiceTableEntry: array [0..1] of TServiceTableEntry;
{$IFDEF DEBUG}
  i: integer;
  ParamIndex: integer;
  {$IFDEF DELPHI4UP}
  Params: array of string;
  {$ELSE}
  Params: PChar;
  TmpPString: PString;
  {$ENDIF}
{$ENDIF}
begin
  //AddToMessageLog('Calling Run',EVENTLOG_ERROR_TYPE,0,0);
{$IFDEF DEBUG}
  if IsTest then begin
    ParamIndex:=FindCmdLineSwitch('TEST',['/','-'],True);
    if ParamIndex=-1 then ParamIndex:=ParamCount;
    {$IFDEF DELPHI4UP}
    SetLength(Params,1+ParamCount-ParamIndex);
    Params[0]:=ServiceName;
    for i:=1 to ParamCount-ParamIndex do
      Params[i]:=ParamStr(i+ParamIndex);
    ServiceMain(1+ParamCount-ParamIndex,PPChar(Params));
    {$ELSE}
    GetMem(Params,SizeOf(string)*(1+ParamCount-ParamIndex));
    try
      TmpPString:=PString(Params);
      Initialize(TmpPString^,1+ParamCount-ParamIndex);
      TmpPString^:=ServiceName;
      for i:=ParamIndex+1 to ParamCount do begin
        inc(TmpPString);
        TmpPString^:=ParamStr(i);
      end;
      ServiceMain(1+ParamCount-ParamIndex,PPChar(Params));
    finally
      TmpPString:=PString(Params);
      Finalize(TmpPString^,1+ParamCount-ParamIndex);
      FreeMem(Params);
    end;
    {$ENDIF}
    exit;
  end;
{$ENDIF}
  // Setup service table which define all services in this process
  with ServiceTableEntry[0] do begin
    lpServiceName:=PChar(FServiceName);
    lpServiceProc:=FServiceMainWrapper;
  end;
  // Last entry in the table must have nil values to designate the end of the table
  with ServiceTableEntry[1] do begin
    lpServiceName:=nil;
    lpServiceProc:=nil;
  end;

  if not StartServiceCtrlDispatcher(ServiceTableEntry[0]) then begin
    AddToMessageLog('StartServiceCtrlDispatcher Error!',EVENTLOG_ERROR_TYPE,0,0);
    Halt;
  end;
end;


procedure TService.ServiceMain(Argc: DWORD; Argv: PPChar);
type
  TExceptHandler = procedure(ExceptObject: TObject; ExceptAddr: Pointer);
var
  Args: PPChar;
begin
  //AddToMessageLog('Calling ServiceMain',EVENTLOG_ERROR_TYPE,0,0);
  Args := Argv;

  // Correct ServiceName is passed as first argument
  FserviceName := Args^;
  if argc=4 then
  begin
    inc(args);
    if Args^ = '*&#SlaveProcess#&*' then
    begin
      inc(args); FSlaveProcess := Args^;
      inc(args); FSlaveProcessArgs := Args^;
    end;
  end;

  try
  {$IFDEF DEBUG}
    if not IsTest then begin
  {$ENDIF}
    // Register the handler function with dispatcher;
    ServiceStatusHandle:=RegisterServiceCtrlHandler(PChar(FServiceName),FServiceHandlerWrapper);
    if ServiceStatusHandle=0 then
    begin
      ReportStatusToSCMgr(SERVICE_STOPPED,GetLastError,0);
      //AddToMessageLog(GetLastErrorText,EVENTLOG_ERROR_TYPE,0,0);
      exit;
    end;
  {$IFDEF DEBUG}
    end;
  {$ENDIF}

    ssStatus.dwServiceType:=SERVICE_WIN32_OWN_PROCESS;
    ssStatus.dwServiceSpecificExitCode:=0;
    ssStatus.dwCheckPoint:=1;

    // Report current status to SCM (Service Control Manager)
    if not ReportStatusToSCMgr(SERVICE_START_PENDING,NO_ERROR,1000) then
    begin
      ReportStatusToSCMgr(SERVICE_STOPPED,GetLastError,0);
      //AddToMessageLog(GetLastErrorText,EVENTLOG_ERROR_TYPE,0,0);
      exit;
    end;

    // Start Service
    if not ReportStatusToSCMgr(SERVICE_START_PENDING,NO_ERROR,1000) then exit;

    try
      InitService;
      try
        if InitOK and ReportStatusToSCMgr(SERVICE_RUNNING,NO_ERROR,0) then
        begin
          if Length(FSlaveProcess)>0 then
          begin
            FThreadedProcess := RunProcess(FSlaveProcess, FSlaveProcessArgs);
            // AddToMessageLog('Started dgate',EVENTLOG_ERROR_TYPE,0,0);
            RunService;
          end
          else
          begin
            RunService;
          end;
        end;
      finally
        ServiceStop;
        DoneService;
        if FThreadedProcess<>0 then
        begin
          TerminateProcess(FThreadedProcess, 0);
          CloseHandle(FThreadedProcess);
          FThreadedProcess := 0;
        end;
      end;
    finally
      ReportStatusToSCMgr(SERVICE_STOPPED,GetLastError,0);
    end;
  except
    //Default exception handling
    TExceptHandler(ExceptProc)(ExceptObject,ExceptAddr);
  end;
end;

procedure TService.Handler(dwCtrlCode: DWORD);
begin
  // Handle the requested control code.
  case dwCtrlCode of
    SERVICE_CONTROL_STOP: begin
      ServiceStop;
      exit;
    end;

    SERVICE_CONTROL_INTERROGATE: begin
    end;

    SERVICE_CONTROL_PAUSE: begin
    end;

    SERVICE_CONTROL_CONTINUE: begin
    end;

    SERVICE_CONTROL_SHUTDOWN: begin
    end;

    // invalid control code
    else
  end;
  // Update the service status.
  ReportStatusToSCMgr(ssStatus.dwCurrentState, NO_ERROR, 0);
end;

procedure TService.InitService;
begin
  InitOK:=False;
  FServiceStopEventHandle:=CreateEvent(nil,TRUE,False,nil);
  if FServiceStopEventHandle=0 then
    AddToMessageLog('CreateEvent error',EVENTLOG_ERROR_TYPE,0,0)
  else
    InitOK:=True;
end;

procedure TService.DoneService;
begin
  if FServiceStopEventHandle<>0 then
    CloseHandle(FServiceStopEventHandle);
end;

procedure TService.ServiceStop;
begin
  if (FServiceStopEventHandle<>0) then
    SetEvent(FServiceStopEventHandle);
end;

function TService.WaitForStop(TimeOut: DWord): boolean;
begin
  Result:=WaitForSingleObject(FServiceStopEventHandle,TimeOut)=WAIT_OBJECT_0;
  if FThreadedProcess<>0 then
    Result:=Result or (WaitForSingleObject(FThreadedProcess,TimeOut)<>WAIT_TIMEOUT);
end;

initialization
  IsMultiThread:=True;
end.

