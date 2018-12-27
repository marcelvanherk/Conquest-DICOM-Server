{************************************************************************}
{*                                                                      *}
{*      file       : Launch.PAS                                         *}
{*                                                                      *}
{*      type       : unit                                               *}
{*                                                                      *}
{*      location   : \QUIRT\SRC\DELPHI                                  *}
{*                                                                      *}
{*      purpose    : Launch external programs                           *}
{*                                                                      *}
{*      author     : Lennert Ploeger (NKI / AVL)                        *}
{*                                                                      *}
{*      date       : 19980325                                           *}
{*                                                                      *}
{*      portability: 32 bit delphi only (V2.0 up)                       *}
{*                                                                      *}
{*      notes      : None                                               *}
{*                                                                      *}
{************************************************************************}
{* Updates:
When            Who     What
19980325        lsp     Created
19980331        lsp     Search for programs in PATH using SearchPath()
19980609        lsp     Allow white-spaces for the program to launch
19980709        lsp     Allow 'prog' to be empty in StartProgram
19980731        lsp     Enclose both file and directory names in double quotes
19980901        lsp     Removed some obsolete functions
19981004        mvh     Added RunProgram (waits until ready)
19981005        mvh     Renamed to RunProgramBlocking (waits until ready)
19981020        lsp     Program launched in RunProgramBlocking() is started
                        minimized and not given focus
19990110        mvh     RunProgramBlocking returns value
19990111        lsp     Fixed comment about CloseHandle()
19990112	mvh	Added RunProgramWaiting, shortened code by reuse
19990425        lsp     FileUtil -> QFileUtil
19990609        lsp     Removed QFileUtil dependency
20000114        lsp     Added show option to StartProgram
20000502      lsp+mvh   Fixed argument passing for batch files
20020802        lsp     More tweaking on passing command line arguments: only
                        enclose file names when they contain spaces
20021024        lsp     Implemented show=false by passing
                        SW_HIDE instead of SW_SHOWMINNOACTIVE
20040427        mvh     ELEKTA NKI-XVI0.1a RELEASE
20040608        mvh     WBH NKI-XVI0.1b RELEASE (for testing only)
20150719        mvh     Adapted for delphi XE
*}

unit Launch;

interface

function LocateProgramInSearchPath(ProgramName: string): string;
procedure StartProgram(prog, workdir: string; params: array of string; show: boolean = true);
function RunProgramBlocking(prog, workdir: string; params: array of string): integer;
function RunProgramWaiting(prog, workdir: string; params: array of string): integer;

implementation

uses
  SysUtils, Dialogs, Windows, Forms, FileCtrl, System.UITypes;

// Routine to retrieve file information using a file/dirname
function FileDirNameWin32FindData(FullPathName: string; var Win32FindData: TWin32FindData): boolean;
var
  Handle: THandle;
begin
  Handle := FindFirstFile(PChar(FullPathName), Win32FindData);
  if Handle <> INVALID_HANDLE_VALUE then
  begin
    Windows.FindClose(Handle);
    Result := True;
  end
  else
    Result := False;
end;

// Search for 'ProgramName' in %PATH% using Win API function 'SearchPath'
function LocateProgramInSearchPath(ProgramName: string): string;
type
  BufType = array[0..255] of char;
var
  SPPath: BufType;
  PathPtr: PChar;
begin
  if SearchPath(nil, PChar(ProgramName), nil, 255, SPPath, PathPtr)>0 then
    Result := StrPas(SPPath)
  else
    Result := ProgramName;
end;

function TestExistence(const Name: string): boolean;
begin
  Result := FileExists(Name) or SysUtils.DirectoryExists(Name);
end;

function LongToShortFileName(const LongName: string): string;
var
  Temp: TWin32FindData;
  SearchHandle: THandle;
begin
  SearchHandle := FindFirstFile(PChar(LongName), Temp);
  if SearchHandle <> INVALID_HANDLE_VALUE then
  begin
    Result := string(Temp.cAlternateFileName);
    if Result = '' then
      Result := string(Temp.cFileName);
  end
  else
    Result := '';
  Windows.FindClose(SearchHandle);
end;

function LongToShortPath(const LongName: string): string;
var
  LastSlash: PChar;
  TempPathPtr: PChar;
  LongNameCopy: string;
begin
  LongNameCopy := LongName;
  UniqueString(LongNameCopy);
  Result := '';
  TempPathPtr := PChar(LongNameCopy);
  LastSlash := StrRScan(TempPathPtr, '\');
  while LastSlash <> nil do
  begin
    Result := '\' + LongToShortFileName(TempPathPtr) + Result;
    if LastSlash <> nil then
    begin
      LastSlash^ := char(0);
      LastSlash := StrRScan(TempPathPtr, '\');
    end;
  end;
  Result := TempPathPtr + Result;
end;

// Internal common code for the 3 exported program starters

function ProgramStarter(prog, workdir: string;
                        params: array of string; showmode: integer): integer;
var
  StartInf : TStartupInfo;
  ProcInf  : TProcessInformation;
  args     : string;
  progtmp  : string;
  pAppName : PChar;
  pworkdir : PChar;
  i        : integer;
begin
  // It appears that it is also possible to call ShellExecute, what seems
  // to be a bit a higher level function:
  // ShellExecute(handle, "open", path_to_file, NULL, NULL, SW_SHOWNORMAL);
  // Another alternative can be calling WinExec, but with the drawback of
  // not having the possibility to specify the work directory.

  ZeroMemory(@StartInf, sizeof(TStartupInfo));
  ZeroMemory(@ProcInf, sizeof(TProcessInformation));
  StartInf.cb      := sizeof(TStartupInfo);
  StartInf.dwFlags := STARTF_USESHOWWINDOW;
  StartInf.wShowWindow := showmode;

  // To call CreateProcess() we should be carefull to enclose files
  // passed in 'lpCommandLine' in double quotes ('"') to make sure that
  // it understands where one starts and ends. The pecularity is that the
  // name of the executable should NOT be enclosed in double quotes. Passing
  // both the file and the arguments in lpCommandLine and leaving
  // lpApplicationName empty is no option, since the application and
  // arguments should be white-space delimited that way.

  progtmp := prog;
  if not FileExists(progtmp) then
    progtmp := LocateProgramInSearchPath(progtmp);

  if (Length(prog)>0) then
    if (ChangeFileExt(prog, '.bat')<>prog) then
      args := '"' + prog + '"'
    else
      args := LongToShortPath(prog)
  else
    args := '';
  for i:=low(params) to high(params) do
  begin
    // Enclose file and directory names in double quotes to get them seperated properly
    if TestExistence(params[i]) then
    begin
      if Pos(' ', params[i])>0 then
      begin
        if Length(args)>0 then // Try to add spaces only when necessary
          args := args + ' "' + params[i] + '"'
        else
          args := '"' + params[i] + '"'
      end
      else
      begin
        if Length(args)>0 then // Try to add spaces only when necessary
          args := args + ' ' + params[i]
        else
          args := params[i]
      end
    end
    else if Length(params[i])>0 then
    begin
      if Length(args)>0 then // Try to add spaces only when necessary
        args := args + ' ' + params[i]
      else
        args := params[i];
    end;
  end;

  if Length(workdir)>0 then
    pworkdir := PChar(workdir)
  else
    pworkdir := nil;

  if Length(progtmp)>0 then
    pAppName := PChar(progtmp)
  else
    pAppName := nil;

  if not CreateProcess(pAppName, PChar(args), nil, nil, False, 0, nil,
    pworkdir, StartInf, ProcInf) then
    MessageDlg('The program ' + prog + ' was not executed', mtError, [mbOk], 0);

  // The handles for both the process and the main thread must be closed through
  // calls to CloseHandle. These handles are not needed, so it is best to close
  // them immediately after the process is created.
  result := ProcInf.hProcess;
  CloseHandle(ProcInf.hThread);
end;

// routine to start program 'prog' using the file-parameters in 'params'.
// Be carefull with passing [] for the params, since the stackpointer seems
// to get corrupt. However, starting a program with no arguments can be done
// using StartProgram(program, workdir, [ ' ' ]).
procedure StartProgram(prog, workdir: string; params: array of string; show: boolean);
var
  hProcess: integer;
begin
  if show then
    hProcess := ProgramStarter(prog, workdir, params, SW_SHOWDEFAULT)
  else
    hProcess := ProgramStarter(prog, workdir, params, SW_HIDE);
  CloseHandle(hProcess);
end;

// routine to run program 'prog' using the file-parameters in 'params'.
// and wait until it is finished
// Be careful with passing [] for the params, since the stackpointer seems
// to get corrupt. However, starting a program with no arguments can be done
// using StartProgram(program, workdir, [ ' ' ]).
function RunProgramBlocking(prog, workdir: string; params: array of string): integer;
var
  hProcess: integer;
  j       : Cardinal;
begin
  hProcess := ProgramStarter(prog, workdir, params, SW_SHOWMINNOACTIVE);

  WaitForSingleObject(hProcess, INFINITE);
  GetExitCodeProcess(hProcess, j);
  result := integer(j);

  CloseHandle(hProcess);
end;

// routine to run program 'prog' using the file-parameters in 'params'.
// and wait until it is finished. During the wait, however, messages are
// processed so that the user interface remains 'live'.
function RunProgramWaiting(prog, workdir: string; params: array of string): integer;
var
  hProcess: integer;
  j        : Cardinal;
begin
  hProcess := ProgramStarter(prog, workdir, params, SW_SHOWMINNOACTIVE);

  while WaitForSingleObject(hProcess, 10) = WAIT_TIMEOUT do
    Application.ProcessMessages;

  GetExitCodeProcess(hProcess, j);
  result := integer(j);

  CloseHandle(hProcess);
end;

end.
