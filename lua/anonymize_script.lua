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
-- 20231018     mvh     No reset dicom object in TagsToKeep - use temporary file; reset reversible after cfg read
-- 20231021     mvh     Now use newly added Data:Reset
-- 20231107     mvh     Also accept (hex) group numbers in lists instead of name
-- 20231122     mvh     Keep TagsToEmpty and TagsToRemove to combine (after) TagsToKeep
-- 20250214     mvh     Catch missing command_line (when direcly called as lua file)
-- 20250911     mvh     Fix missing newuids if logmodified==false
-- 20250911     mvh     Split functions of and pass all config and control as parameters
-- 20250911     mvh     Added dateoffset to command_line or tag "9999,1235"
-- 20260107	mvh	Moved reading of alternative configuration to here
-- 20260109	mvh	Added TagsToSanitise
-- 20260110	mvh	Made config local
-- 20260913	mvh	Fix passing of patient ID and name from command line

-- =============================================================================

-- conquest importconverter command_line is optional parameter = newid|stage|newname|dateoffset, default all empty
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
-- all dates in dicom object will be changed by N days from (in increasing priority)
--  no change
--  contents of [9999,1235]
--  dateoffset
--
-- logging goes to logname() filename (Stage_studyDate_Modality_SOP.log)
-- default at DicomAnonymized_Log/patientID/Stage_studyDate_Modality_SOP.log
-- if you take replace SOP by e.g. SeriesInstanceUID the logging will be per Series

-- =============================================================================

local scriptversion = "1.5; date 20260110"

local DirSep      = '/'
if string.find(Global.BaseDir, '\\') then DirSep = '\\' end

require('anonymizer')

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
-- e.g. TagsToKeep = {"PatientID", "SOPInstanceUID", "0008", "0010", "0028", "7FE0"}
TagsToKeep = {}

-- entries that are modified to remove dates and patientIDs
TagsToSanitise = {
"SeriesDescription", "StudyDescription"}

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

-- get suggested patient ID, stage for staged operation and suggested patient name
local c = split(command_line or '', '|')
if string.find(command_line or '', '|')==1 then table.insert(c, 1, '') end
local newid = c[1] or (CRC32(Data.PatientID)..'.'..CRC32(Data.PatientBirthDate or ''))
local newname = ''
local stage = c[2] or ''
local newname = c[3] or ('PAT'..CRC32(Data.PatientID))
local dateoffset = c[4] or ''

-- the changes in patient ID etc can be picked up from private tag 0x9999,0x1234 (which is then deleted from the anomymized data)
if Data["9999,1234"] then
  newid = Data["9999,1234"]
  newname = Data["9999,1234"]
  Data["9999,1234"]=nil
end

if Data["9999,1235"] then
  dateoffset = Data["9999,1235"]
  Data["9999,1235"]=nil
end

-- default config
local config = {}
config.reversible  = reversible
config.MaintainAge = MaintainAge
config.MaintainSex = MaintainSex
config.logroot     = logroot
config.logname     = logname
config.logoriginal = logoriginal
config.logmodified = logmodified
config.lognames    = lognames
config.TagsToEmpty = TagsToEmpty
config.TagsToKeep  = TagsToKeep
config.TagsToRemove= TagsToRemove
config.TagsToModify= TagsToModify
config.TagsToPrint = TagsToPrint
config.TagsToSanitise= TagsToSanitise

-- optionally read *stage*.cfg to change anonymisation settings
local g=io.open(string.gsub(Global.BaseDir..stage..'.cfg', '#', ''), 'r')
if g then
  g:close()
  config = dofile(Global.BaseDir..string.gsub(stage, '#', '')..'.cfg')
  print("Read configuration file for stage: ", stage, "\n") 
else
  print("Anomymisation stage              : ", stage, "\n") 
end

anonymize(config, newid, newname, stage, dateoffset)
