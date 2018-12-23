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
-- =============================================================================

--[[ To test; r-click evaluate in console after project-run:
readdicom('0009703828:1.3.46.670589.5.2.10.2156913941.892665340.475317')
Data.Dump('c:\\data\\image_in.txt')
dofile('lua/anonymize_script.lua')
Data.Dump('c:\\data\\image_anonymized.txt')
dofile('lua/deanonymize_script.lua')
Data.Dump('c:\\data\\image_restored.txt')
]]

local scriptversion = "1.2; date 20160113"

local pre, pid = Data.PatientID

if Data.PatientID~='' then
  pid = changeuidback(pre, command_line)
  if pid==nil then
    print('** patient '..pre..' was not anonymized on this system **')
    return
  end
end

-- remove characters that are not allowed in a filename
local pid2 = string.gsub(pid, '[\\/:*?"<>|]', '_')

-- Log file handling (trailing backslash required for mkdir)
local logdir = "DicomAnonymized_Log\\"..pid2.."\\";
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
  Data.PatientBirthDate = string.sub(s, string.find(s, '%.', -10)+1);
  f:write('Restored patient birthdate to: ', tostring(Data.PatientBirthDate), "\n");
end

if Data.PatientName~='' then
  local s = changeuidback(Data.PatientName, command_line)
  Data.PatientName = s;
  f:write('DeAnonymized PatientName to: ', Data.PatientName, "\n")
end

if (Data.PatientSex=='') then
  local s = changeuidback(pre .. '.ps.' .. Data.PatientSex, command_line)
  Data.PatientSex = string.sub(s, string.find(s, '%.', -3)+1);
  f:write('Restored patient sex to: ', tostring(Data.PatientSex), "\n");
end

f:close();
