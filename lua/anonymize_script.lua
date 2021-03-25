-- =============================================================================
-- Anonymize DICOM tags to protect Private Health Information (HIPAA)
-- Based on:	DicomAnonymize.cq (c) Bart Hoekstra UMCG
-- Author: 	Marcel van Herk
-- Reference: 	PS 3.15-2009 Table E.1-1
-- Modification
-- 20120827	mvh	Use script() instead of os.execute() to avoid blinking window
-- 20130127	mvh	for 1.4.17beta: Match updated dictionary; added MaintainAge, MaintainSex,
--			Reversible options; use lua crc calls
-- 20130211	mvh	Make sure PatientID used as filename has no invalid characters
-- 20130522	mvh	Cleanup for release; fixed when birthdate empty
-- 20130718	mvh	Set logroot not to c:\DicomAnonymized but to DicomAnonymized_Log
-- 20130813	mvh	Command line overrules generated patientid and patientname
-- 20140304	mvh	Remove OtherPatienIDSequence
-- 20140309	mvh	Protect against any missing data
-- 20140618	mvh	Protect against missing PatientID (if used for queries)
-- 20160113	mvh	Added staged operation using "subjectkey|stage" command_line
-- 20160118	mvh	Original birthdate and sex coded with original patient rather than new one
--			Pass key names to changeuid
-- 20180917  mvh+aba+gp Take 0x9999,0x1234 as anomymized ID
-- 20181110     mvh     Change directory separator for Linux to /, apply BaseDir and assert(open)
-- 20200202     mvh     Allow . in stage
-- 20201021   mvh+aza   Consider removing 0040,2017 = FillerOrderNumberImagingServiceRequest
-- 20210118     mvh     Added newname option for inholland system; remove FillerOrderNumberImagingServiceRequest
-- 20210204     mvh     Small fix in that
-- 20210208     mvh     Fix 3 parameter mode; newname overrules patientname anonymisation
-- 20210209     mvh     Use newname parameter before anonymisation
-- 20210211     mvh     If using single parameter, make sure patid and patname are different
-- 20210211   mvh+evo   Make sure patid and patname are different before anonymisation

-- =============================================================================

local scriptversion = "1.12; date 20210211"

---------------------------------- configuration -----------------------------
-- entries that show up in log but are NOT modified (except implicitly the UIDs)
local TagsToPrint = {
"FrameOfReferenceUID", "StudyInstanceUID", "SeriesInstanceUID", "SOPInstanceUID",
"Modality", "BodyPartExamined", "Manufacturer", "ManufacturerModelName",
"StudyDate", "StudyTime", "SeriesNumber", "ImageID"}

-- entries that are emptied
local TagsToEmpty = {
"AccessionNumber", "InstitutionName", "ReferringPhysicianName", "PerformingPhysicianName", "StudyID"}

-- entries that are removed
local TagsToRemove = {
"InstanceCreatorUID", "InstitutionAddress", "ReferringPhysicianAddress", "ReferringPhysicianTelephoneNumbers",
"StationName", "StudyDescription", --[["SeriesDescription",]] "InstitutionalDepartmentName",
"PhysiciansOfRecord", "NameOfPhysiciansReadingStudy", "OperatorsName", "AdmittingDiagnosesDescription",
"DerivationDescription", "OtherPatientIDs", "OtherPatientNames", "PatientAge",
"PatientSize", "PatientWeight", "MedicalRecordLocator", "EthnicGroup",
"Occupation", "AdditionalPatientHistory", "DeviceSerialNumber", "ProtocolName",
"ImageComments", "RequestAttributesSequence", "ContentSequence", "StorageMediaFileSetUID",
"PatientInsurancePlanCodeSequence", "PatientBirthName", "PatientAddress", "InsurancePlanIdentification",
"PatientMotherBirthName", "MilitaryRank", "BranchOfService", "RegionOfResidence",
"PatientTelephoneNumbers", "PatientComments", "StudyComments", "ScheduledPerformingPhysicianName",
"PerformingPhysicianIdentificationSequence", "OtherPatientIDsSequence", "FillerOrderNumberImagingServiceRequest" }

local DirSep      = '/'
if string.find(Global.BaseDir, '\\') then DirSep = '\\' end
local MaintainAge = false
local MaintainSex = false
local reversible  = true
local logroot     = Global.BaseDir.."DicomAnonymized_Log"..DirSep

---------------------------------- end configuration -----------------------------

-- entries that show up in log and that are individually modified
local TagsToModify = {
"PatientID", "PatientName", "PatientBirthDate", "PatientSex"}

local function CRC32(val)
  return crc(tostring(val))
end;

-- remove characters that are not allowed in a filename
local pid = string.gsub(Data.PatientID or 'unknown', '[\\/:*?"<>|]', '_')

-- get stage for staged operation
local stage = ''
local newname = ''
if string.find(command_line or '', "([%w_%.]+)|([%w_%.]+)|([%w_%.]+)") then
  local dum
  dum, dum, command_line, stage, newname = string.find(command_line, "([%w_%.]+)|([%w_%.]+)|([%w_%.]+)")
elseif string.find(command_line or '', "([%w_%.]+)|([%w_%.]+)") then
  local dum
  dum, dum, command_line, stage = string.find(command_line, "([%w_%.]+)|([%w_%.]+)")
end

-- Log file handling (trailing directory separator required for mkdir)
local logdir = logroot..pid..DirSep;
local logfile = stage..'_'..pid..'_'..(Data.StudyDate or '19700101')..'_'..(Data.Modality or 'UN')..'_'..(Data.SOPInstanceUID or 'unknown')..'.log'
script('mkdir '..logdir);

local f = assert(io.open(logdir .. logfile, "w"))
f:write("DicomAnonymize.lua script version: ", scriptversion, "\n")
f:write("Logfile name                     : ", logfile, "\n")
f:write("Logfile created                  : ", os.date(), "\n")

-- Check dictionary
for key2, val2 in ipairs({TagsToModify, TagsToPrint, TagsToEmpty, TagsToRemove}) do
  for key, val in ipairs(val2) do
    if dictionary(val)==nil and string.sub(val,1,1)~='0' then
      f:write("*** Error: '", val, "' not in dictionary\n")
    end
  end
end

-- Log data in original object
f:write("===== ORIGINAL DICOM DATA =====\n");
for key2, val2 in ipairs({TagsToModify, TagsToPrint, TagsToEmpty, TagsToRemove}) do
  for key, val in ipairs(val2) do
    f:write(val, ': ', tostring(Data[val]), "\n")
   end
end

-- modify and log modified object, possibly for stage 2 anonymization with named keys
f:write("===== MODIFIED DICOM DATA =====\n");
if stage~='' then
  script('newuids stage ' .. stage);
  f:write("Generated new UIDs stage "..stage.."\n")
else
  script('newuids');
  f:write("Generated new UIDs\n")
end

local oldID = Data.PatientID

-- the changes in patient ID etc are hardcoded, or can be picked up from private tag 0x9999,0x1234 (which is then deleted from the anomymized data)
local pre, pne
if Data["9999,1234"] then
  pre = Data["9999,1234"]
  pne = Data["9999,1234"]
  Data["9999,1234"]=nil
else
  pre = CRC32(Data.PatientID)..'.'..CRC32(Data.PatientBirthDate or '');
  pne = 'PAT'..CRC32(Data.PatientID)
end

-- but the command_line always overrules any patient ID generated
if version and command_line and command_line~='' then pre=command_line end
if version and command_line and command_line~='' then pne='PAT_'..command_line end
if Data.PatientID == Data.PatientName then Data.PatientName = "PAT_" .. Data.PatientName end
if newname~='' then pne = newname end

if Data.PatientID~='' then
  if reversible==true then
    Data.PatientID = changeuid(Data.PatientID, pre, stage, 'PatientID')
  else
    Data.PatientID = pre;
  end
  f:write('Anonymized PatientID to: ', Data.PatientID, "\n");
end
if Data.PatientName~='' then
  if reversible==true then
    Data.PatientName = changeuid(Data.PatientName, pne, stage, 'PatientName')
  else
    Data.PatientName = pne;
  end
  f:write('Anonymized PatientName to: ', Data.PatientName, "\n");
end
if Data.PatientBirthDate and Data.PatientBirthDate~='' then
  local org = Data.PatientBirthDate;
  if MaintainAge==true then
    Data.PatientBirthDate = string.sub(org, 1, 4)..'0101'
  else
    Data.PatientBirthDate = '';
  end
  if reversible==true then
    changeuid(oldID..'.bd.'..org, pre..'.bd.'..Data.PatientBirthDate, stage, 'PatientBirthDate')
  end
  f:write('Changed patient birthdate to: ', tostring(Data.PatientBirthDate), "\n");
end
if (MaintainSex==false) and (Data.PatientSex~='') then
  local org = Data.PatientSex or 'UN';
  Data.PatientSex = '';
  f:write('Made patient sex empty: ', tostring(Data.PatientSex), "\n");
  if reversible==true then
    changeuid(oldID..'.ps.'..org, pre..'.ps.'..Data.PatientSex, stage, 'PatientSex')
  end
end

-- log modified UIDs
local i;
for i=1, 4 do
  f:write(TagsToPrint[i], ': ', tostring(Data[TagsToPrint[i]]), "\n");
end
-- empty tags
for key, val in ipairs(TagsToEmpty) do
  Data[val]='';
  f:write('Made ', val, ' empty: ', tostring(Data[val]), "\n");
end
-- remove tags
for key, val in ipairs(TagsToRemove) do
  Data[val]=nil;
  f:write('Removed ', val, ': ', tostring(Data[val]), "\n");
end

f:close();
