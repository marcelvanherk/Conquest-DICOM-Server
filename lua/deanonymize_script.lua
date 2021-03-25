-- =============================================================================
-- DeAnonymize DICOM tags 
-- Author: 	Marcel van Herk
-- 20130125	mvh	Created
-- 20130211	mvh	Take out invalid characters for filenames
-- 20130522	mvh	Cleanup for release
-- 20130718	mvh	Moved log folder
-- 20130802	mvh	Detect if patientID cannot be deanonymized
-- 20140309	mvh	Protect against any missing data
-- 20160113	mvh	Added staged operation using "stage" command_line
-- 20200127	mvh	Allow stage *: pick stage based on actual patientID; reject on error
-- 20200202     mvh     * in command line sets 9999,9000 with orginal patient ID
-- 20200302     mvh     Fix DirSep for linux, add Global.basedir
-- 20200308     mvh     Used wrong quotes in stage * code; postgresql tripped over it
--			case of UIDMODS; mariadb tripped over it
-- 20201003     mvh     Fix crash when trying to deanonymize birthdate, name and sex if not there
-- 20210209     mvh     Also store name in 9999,9001
-- 20210218     mvh     Accept StudyInstanceUID as secondary source for stage *
-- =============================================================================

--[[ To test; r-click evaluate in console after project-run:
readdicom('0009703828:1.3.46.670589.5.2.10.2156913941.892665340.475317')
Data.Dump('c:\\data\\image_in.txt')
dofile('lua/anonymize_script.lua')
Data.Dump('c:\\data\\image_anonymized.txt')
dofile('lua/deanonymize_script.lua')
Data.Dump('c:\\data\\image_restored.txt')
]]

local scriptversion = "1.4; date 20210218"
local DirSep      = '/'
if string.find(Global.BaseDir, '\\') then DirSep = '\\' end

local pre, pid = Data.PatientID

if Data.PatientID~='' then
  if command_line=='*' then
    local r=dbquery("UIDMODS","distinct stage","newUID='"..Data.PatientID.."'")
    if r==nil then 
      print('** sql error **')
      reject()
      return
    end
    if r[1]==null then 
      r=dbquery("UIDMODS","distinct stage","newUID='"..Data.StudyInstanceUID.."'")
      if r[1]==null then 
        print('** patient '..pre..' was not anonymized on this system stage(*) **')
        reject()
        return
      end
    end
    command_line=r[1][1]
    Data["9999,9000"]=Data.PatientID
    Data["9999,9001"]=Data.PatientName
  end
  pid = changeuidback(pre, command_line)
  if pid==nil then
    print('** patient '..pre..' was not anonymized on this system stage('..command_line..') **')
    reject()
    return
  end
end

-- remove characters that are not allowed in a filename
local pid2 = string.gsub(pid, '[\\/:*?"<>|]', '_')

-- Log file handling (trailing backslash required for mkdir)
local logdir = Global.basedir.."DicomAnonymized_Log"..DirSep..pid2..DirSep;
local logfile = pid2..'_'..(Data.StudyDate or '19700101')..'_'..(Data.Modality or 'UN')..'_'..(Data.SOPInstanceUID or 'unknown')..'.deanonymize.log'
script('mkdir '..logdir);

f = io.open(logdir .. logfile, "wt");
f:write("DicomDeAnonymize.lua script version: ", scriptversion, "\n")
f:write("Logfile name                       : ", logfile, "\n")
f:write("Logfile created                    : ", os.date(), "\n")

-- modify and log modified object
f:write("===== MODIFIED DICOM DATA =====\n");
if command_line then
  script('olduids stage ' .. command_line)
else
  script('olduids')
end
f:write("Restoring UIDs\n")

if Data.PatientID~='' then
  Data.PatientID = changeuidback(pre, command_line)
  f:write('Restored patient ID to: ', Data.PatientID, '\n');
end

if true then
  local s= changeuidback(pre..'.bd.'..Data.PatientBirthDate, command_line)
  if s then
    Data.PatientBirthDate = string.sub(s, string.find(s, '%.', -10)+1);
    f:write('Restored patient birthdate to: ', tostring(Data.PatientBirthDate), "\n");
  end
end

if Data.PatientName~='' then
  local s = changeuidback(Data.PatientName, command_line)
  if s then 
    Data.PatientName = s;
    f:write('DeAnonymized PatientName to: ', Data.PatientName, "\n")
  end
end

if (Data.PatientSex=='') then
  local s = changeuidback(pre .. '.ps.' .. Data.PatientSex, command_line)
  if s then
    Data.PatientSex = string.sub(s, string.find(s, '%.', -3)+1);
    f:write('Restored patient sex to: ', tostring(Data.PatientSex), "\n");
  end
end

f:close();
