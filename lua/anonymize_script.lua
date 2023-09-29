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
-- 20230920     mvh     Accept #xxx as stage; log stage
-- 20230924     mvh     Add optional configuration file; TagsToKeep (reverse logic); documentation; 
--                      configure log level; allow hiding any Name tag from log
-- 20230925     mvh     fix typos and changed order
-- 20230928     mvh     Fixed reading empty newid, configuration file, added newlines to output

-- =============================================================================

-- command_line is optional parameter = newid|stage|newname, default all empty
--
-- stage can be 
--  empty (reversable anonymisation using database)
--  name (reversable staged anonymisation using database) 
--  #name (hash based irreversible staged anonymization)
--  if stage is set, will attempt to load stage.cfg from basedir to override default anonymisation configuration
--
-- patientName will be set to (in increasing priority)
--  PAT_ hash of patientID
--  contents of [9999,1234]
--  newname
--
-- patientId will be set to (in increasing priority)
--  hash of patientID + PatientBirthDate
--  contents of [9999,1234]
--  newid
--
-- logging goes to logname() filename (Stage_studyDate_Modality_SOP.log)
-- default at DicomAnonymized_Log/patientID/Stage_studyDate_Modality_SOP.log
-- if you take replace SOP by e.g. SeriesInstanceUID the logging will be per Series

-- =============================================================================

local scriptversion = "1.2; date 20230925"

local DirSep      = '/'
if string.find(Global.BaseDir, '\\') then DirSep = '\\' end

------------------------------ default configuration --------------------------
-- entries that show up in log but are NOT modified (except implicitly the UIDs)
TagsToPrint = {
"FrameOfReferenceUID", "StudyInstanceUID", "SeriesInstanceUID", "SOPInstanceUID",
"Modality", "BodyPartExamined", "Manufacturer", "ManufacturerModelName",
"StudyDate", "StudyTime", "SeriesNumber", "ImageID"}

-- entries that are emptied
TagsToEmpty = {
"AccessionNumber", "InstitutionName", "ReferringPhysicianName", "PerformingPhysicianName", "StudyID"}

-- entries that are removed
TagsToRemove = {
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

-- entries that show up in log and that are individually modified
TagsToModify = {
"PatientID", "PatientName", "PatientBirthDate", "PatientSex"}

-- alternative: only these entries are kept (if table loaded from configuration file)
TagsToKeep = {}

MaintainAge = false
MaintainSex = false
reversible  = true    -- if set; use database to change ID, Name, Sex; #stage disables this flag
logroot     = Global.BaseDir.."DicomAnonymized_Log"..DirSep
logname     = function(Data, stage, pid) 
                 return stage..'_'..pid..
                '_'..(Data.StudyDate or '19700101')..
                '_'..(Data.Modality or 'UN')..
                '_'..(Data.SOPInstanceUID or 'unknown')..'.log' 
              end
logoriginal = true
logmodified = true
lognames    = false

---------------------------------- end configuration -----------------------------

local function CRC32(val)
  return crc(tostring(val))
end;

function split(str, pat)
   local t = {} 
   local fpat = "(.-)" .. pat
   local last_end = 1
   local s, e, cap = str:find(fpat, 1)
   while s do
      if s ~= 1 or cap ~= "" then
	 table.insert(t,cap)
      end
      last_end = e+1
      s, e, cap = str:find(fpat, last_end)
   end
   if last_end <= #str then
      cap = str:sub(last_end)
      table.insert(t, cap)
   end
   return t
end

-- either define tage to keep or to remove
if TagsToKeep[1] then
  TagsToRemove = {}
  TagsToEmpty = {}
end

-- remove characters that are not allowed in a filename (used as folder for the log file)
local pid = string.gsub(Data.PatientID or 'unknown', '[\\/:*?"<>|]', '_')

-- get suggested patient ID, stage for staged operation and suggested patient name
local c = split(command_line or '', '|')
if string.find(command_line, '|')==1 then table.insert(c, 1, '') end
local newid = c[1] or ''
local stage = c[2] or ''
local newname = c[3] or ''
if string.find(stage, '#') then reversible=false end -- uid generation by MD5 is not reversible

-- optionally read stage.cfg to change anonymisation settings
local g=io.open(string.gsub(Global.BaseDir..stage..'.cfg', '#', ''), 'r')
if g then
  g:close()
  dofile(Global.BaseDir..string.gsub(stage, '#', '')..'.cfg')
  print("Read configuration file for stage: ", stage, "\n") 
else
  print("Anomymisation stage              : ", stage, "\n") 
end

-- Log file handling (trailing directory separator required for mkdir)
local logdir = logroot..pid..DirSep
local logfile = logname(Data, stage, pid)
script('mkdir '..logdir)

local f = io.open(logdir .. logfile, "a") -- append to allow log e.g. at series level
if not f then
  print("*** Error: cannot open logfile - object not processed\n")
  destroy() -- do not process image at all
  return
end

f:write("DicomAnonymize.lua script version: ", scriptversion, "\n")
f:write("Logfile name                     : ", logfile, "\n")
f:write("Processing at                    : ", os.date(), "\n")

-- Check dictionary to avoid crash on undefined tags
for key2, val2 in ipairs({TagsToModify, TagsToPrint, TagsToEmpty, TagsToRemove, TagsToKeep}) do
  for key, val in ipairs(val2) do
    if dictionary(val)==nil and string.sub(val,1,1)~='0' then
      f:write("*** Error: '", val, "' not in dictionary - object will not be processed\n")
      print("*** Error anonymize_script: configured tag '", val, "' is not in dictionary - object not processed\n")
      destroy() -- do not process image at all
      return
    end
  end
end

-- Log data in original object (optionally suppress all names for privacy)
if logoriginal then
  f:write("===== ORIGINAL DICOM DATA =====\n")
  for key2, val2 in ipairs({TagsToModify, TagsToPrint, TagsToEmpty, TagsToRemove, TagsToKeep}) do
    for key, val in ipairs(val2) do
      if string.find(val, 'Name') then
        if lognames then
          f:write(val, ': ', tostring(Data[val]), "\n")
        else
          f:write(val, ': ', '*** not shown ***', "\n")
        end
      else
        f:write(val, ': ', tostring(Data[val]), "\n")
      end
    end
  end
end

-- modify and log modified object, possibly for stage 2 anonymization with named keys
if logmodified then
  f:write("===== MODIFIED DICOM DATA =====\n");
  if stage~='' then
    script('newuids stage ' .. stage);
    f:write("Generated new UIDs stage "..stage.."\n")
  else
    script('newuids')
    f:write("Generated new UIDs\n")
  end
end

local oldID = Data.PatientID

-- the changes in patient ID etc are hardcoded, or can be picked up from private tag 0x9999,0x1234 (which is then deleted from the anomymized data)
local pre, pne
if Data["9999,1234"] then
  pre = Data["9999,1234"]
  pne = Data["9999,1234"]
  Data["9999,1234"]=nil
else
  pre = CRC32(Data.PatientID)..'.'..CRC32(Data.PatientBirthDate or '')
  pne = 'PAT'..CRC32(Data.PatientID)
end

-- but the command_line always overrules any patient ID generated
if newid~=''               then pre=newid  end
if stage~='' and newid~='' then pne='PAT_'..newid end
if newname~=''             then pne = newname end

if Data.PatientID == Data.PatientName then Data.PatientName = "PAT_" .. Data.PatientName end

if Data.PatientID~='' then
  if reversible==true then
    Data.PatientID = changeuid(Data.PatientID, pre, stage, 'PatientID')
  else
    Data.PatientID = pre
  end
  if logmodified then
    f:write('Anonymized PatientID to: ', Data.PatientID, "\n")
  end
end
if Data.PatientName~='' then
  if reversible==true then
    Data.PatientName = changeuid(Data.PatientName, pne, stage, 'PatientName')
  else
    Data.PatientName = pne
  end
  if logmodified then
    f:write('Anonymized PatientName to: ', Data.PatientName, "\n")
  end
end
if Data.PatientBirthDate and Data.PatientBirthDate~='' then
  local org = Data.PatientBirthDate
  if MaintainAge==true then
    Data.PatientBirthDate = string.sub(org, 1, 4)..'0101'
  else
    Data.PatientBirthDate = ''
  end
  if reversible==true then
    changeuid(oldID..'.bd.'..org, pre..'.bd.'..Data.PatientBirthDate, stage, 'PatientBirthDate')
  end
  if logmodified then
    f:write('Changed patient birthdate to: ', tostring(Data.PatientBirthDate), "\n")
  end
end
if (MaintainSex==false) and (Data.PatientSex~='') then
  local org = Data.PatientSex or 'UN'
  Data.PatientSex = ''
  if logmodified then
    f:write('Made patient sex empty: ', tostring(Data.PatientSex), "\n")
  end
  if reversible==true then
    changeuid(oldID..'.ps.'..org, pre..'.ps.'..Data.PatientSex, stage, 'PatientSex')
  end
end

-- log modified UIDs
if logmodified then
  for i=1, 4 do
    f:write(TagsToPrint[i], ': ', tostring(Data[TagsToPrint[i]]), "\n")
  end
end

-- keep tags (empties TagsToEmpty and TagsToRemove tables above)
if TagsToKeep[1] then
  Data2 = Data:Copy()
  Data:Clear()
  for key, val in ipairs(TagsToKeep) do
    local g, e = dictionary(val)
    Data:SetVR(g, e, Data2:GetVR(g, e))
    if logmodified then
      f:write('Kept ', val .. ': ', tostring(Data[val]), "\n")
    end
  end
end

-- empty tags
for key, val in ipairs(TagsToEmpty) do
  Data[val]=''
  if logmodified then
    f:write('Made ', val, ' empty: ', tostring(Data[val]), "\n")
  end
end

-- remove tags
for key, val in ipairs(TagsToRemove) do
  Data[val]=nil
  if logmodified then
    f:write('Removed ', val, ': ', tostring(Data[val]), "\n")
  end
end

f:write("\n\n")
f:close();
