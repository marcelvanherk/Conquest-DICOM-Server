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
-- 20250911     mvh     Added dateoffset to anonymizer; write anonymization profile sequence; config is not optional
-- 20260107	mvh	Moved reading of alternative configuration to wrapper
-- 20260109	mvh	Added TagsToSanitise, version to 1.5
-- 20260114	mvh	Also remove dates as dd-mm-yyyy, yyyy-mm-dd, dd/mm/yyyy, yyyy/mm/dd
-- 20260119	mvh	Escaped - in dd-mm-yyyy, yyyy-mm-dd sanitiser
-- 20260806	mvh	Allow element by group and number, but only in TagsToKeep, e.g. "ffff" vs "ffffcafe"

local scriptversion = "1.5; date 20260806"

function CRC32(val)
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

function anonymize(config, newid, newname, stage, dateoffset)
  newid = newid or ''
  newname = newname or ''
  stage = stage or ''
  dateoffset = dateoffset or ''

  local DirSep      = '/'
  if string.find(Global.BaseDir, '\\') then DirSep = '\\' end

  if string.find(stage, '#') then config.reversible=false end -- uid generation by MD5 is not reversible
  
  -- Log file handling (trailing directory separator required for mkdir)
  -- remove characters that are not allowed in a filename (used as folder for the log file)
  local pid = string.gsub(Data.PatientID or 'unknown', '[\\/:*?"<>|]', '_')
  local logdir = config.logroot..pid..DirSep
  local logfile = config.logname(Data, stage, pid)
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
  for _, val2 in ipairs({config.TagsToModify, config.TagsToPrint, config.TagsToEmpty, config.TagsToRemove, config.TagsToKeep, config.TagsToSanitise}) do
    for _, val in ipairs(val2) do
      if tonumber(val, 16)==nil and dictionary(val)==nil and string.sub(val,1,1)~='0' then
        f:write("*** Error: '", val, "' not in dictionary - object will not be processed\n")
        print("*** Error anonymize_script: configured tag '", val, "' is not in dictionary - object not processed\n")
        destroy() -- do not process image at all
        return
      end
    end
  end
  
  -- Log data in original object (optionally suppress all names for privacy)
  if config.logoriginal then
    f:write("===== ORIGINAL DICOM DATA =====\n")
    for _, val2 in ipairs({config.TagsToModify, config.TagsToPrint, config.TagsToEmpty, config.TagsToRemove, config.TagsToKeep}) do
      for _, val in ipairs(val2) do
        if tonumber(val, 16)~=nil then
          -- nothing
        elseif string.find(val, 'Name') then
          if config.lognames then
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
  if config.logmodified then f:write("===== MODIFIED DICOM DATA =====\n") end
  if stage~='' then
    script('newuids stage ' .. stage);
    if config.logmodified then f:write("Generated new UIDs stage "..stage.."\n") end
  else
    script('newuids')
    if config.logmodified then f:write("Generated new UIDs\n") end
  end
  if dateoffset~='' then
    script('offset dates ' .. dateoffset);
    if config.logmodified then f:write("Offset dates by "..dateoffset.."days\n") end
  end
  
  local oldID = Data.PatientID
  if Data.PatientID == Data.PatientName then Data.PatientName = "PAT_" .. Data.PatientName end
  
  if Data.PatientID~='' then
    if config.reversible==true then
      Data.PatientID = changeuid(Data.PatientID, newid, stage, 'PatientID')
    else
      Data.PatientID = newid
    end
    if config.logmodified then
      f:write('Anonymized PatientID to: ', Data.PatientID, "\n")
    end
  end

  local oldName = Data.PatientName
  if Data.PatientName~='' then
    if config.reversible==true then
      Data.PatientName = changeuid(Data.PatientName, newname, stage, 'PatientName')
    else
      Data.PatientName = newname
    end
    if config.logmodified then
      f:write('Anonymized PatientName to: ', Data.PatientName, "\n")
    end
  end

  if Data.PatientBirthDate and Data.PatientBirthDate~='' then
    local org = Data.PatientBirthDate
    if config.MaintainAge==true then
      Data.PatientBirthDate = string.sub(org, 1, 4)..'0101'
    else
      Data.PatientBirthDate = ''
    end
    if config.reversible==true then
      changeuid(oldID..'.bd.'..org, newid..'.bd.'..Data.PatientBirthDate, stage, 'PatientBirthDate')
    end
    if config.logmodified then
      f:write('Changed patient birthdate to: ', tostring(Data.PatientBirthDate), "\n")
    end
  end
  if (config.MaintainSex==false) and (Data.PatientSex~='') then
    local org = Data.PatientSex or 'UN'
    Data.PatientSex = ''
    if config.logmodified then
      f:write('Made patient sex empty: ', tostring(Data.PatientSex), "\n")
    end
    if config.reversible==true then
      changeuid(oldID..'.ps.'..org, newid..'.ps.'..Data.PatientSex, stage, 'PatientSex')
    end
  end
  
  -- log modified UIDs
  if config.logmodified then
    for i=1, 4 do
      f:write(config.TagsToPrint[i], ': ', tostring(Data[config.TagsToPrint[i]]), "\n")
    end
  end
  
  -- process TagsToSanitise, get rid of ID, name, and optional dates
  if config.TagsToSanitise[1] then
    local sans = {
    oldID, '_ID_', 
    oldName, '_Name_', 
    '19%d%d%d%d%d%d', '_Date_', 
    '20%d%d%d%d%d%d', '_Date_', 
    '21%d%d%d%d%d%d', '_Date_',
    '%d%d%-%d%d%-19%d%d', '_Date_', 
    '%d%d%-%d%d%-20%d%d', '_Date_', 
    '%d%d%-%d%d%-21%d%d', '_Date_', 
    '19%d%d%-%d%d%-%d%d', '_Date_', 
    '20%d%d%-%d%d%-%d%d', '_Date_', 
    '21%d%d%-%d%d%-%d%d', '_Date_',
    '%d%d/%d%d/19%d%d', '_Date_', 
    '%d%d/%d%d/20%d%d', '_Date_', 
    '%d%d/%d%d/21%d%d', '_Date_', 
    '19%d%d/%d%d/%d%d', '_Date_', 
    '20%d%d/%d%d/%d%d', '_Date_', 
    '21%d%d/%d%d/%d%d', '_Date_',
    }
    for _, val in ipairs(config.TagsToSanitise) do
      local s=Data[val]
      if s then
        for i=1, #sans, 2 do
          s=string.gsub(s, sans[i], sans[i+1])
        end
        Data[val]=s
        f:write('Sanitised: ' .. val, "\n")
      end
    end
  end
  
  -- list items in object for enumeration of entire groups
  local names,types,groups,elements = Data:ListItems()
  names = split(names, '|')
  types = split(types, '|')
  groups = split(groups, '|')
  elements = split(elements, '|')
  
  -- keep tags (empties TagsToEmpty and TagsToRemove tables above)
  if config.TagsToKeep[1] then
    local Data2 = Data:Copy()
    Data:Reset() -- requires update > 20231021
    for _, val in ipairs(config.TagsToKeep) do
      if tonumber(val, 16) then
        if tonumber(val, 16)<65536 and config.logmodified then
          f:write('Keep group ', val, "\n")
        end
        if tonumber(val, 16)>=65536 and config.logmodified then
          f:write('Keep element ', val, "\n")
        end
	if tonumber(val, 16) < 65536 then
          for i=1, #groups do
            if tonumber(groups[i])==tonumber(val, 16) and tonumber(elements[i])~=0 then
              Data:SetVR(groups[i], elements[i], Data2:GetVR(groups[i], elements[i]))
  	  end
	else
          local g, e = math.floor(val/65536), val%65536
          Data:SetVR(g, e, Data2:GetVR(g, e))
        end
      else
        local g, e = dictionary(val)
        Data:SetVR(g, e, Data2:GetVR(g, e))
        if config.logmodified then
          f:write('Kept ', val .. ': ', tostring(Data[val]), "\n")
        end
      end
    end
  end
    
  -- empty tags
  for _, val in ipairs(config.TagsToEmpty) do
    if tonumber(val, 16) then
      if config.logmodified then
        f:write('Made group ', val, " empty\n")
      end
      for i=1, #groups do
        if tonumber(groups[i])==tonumber(val, 16) and tonumber(elements[i])~=0 then
          Data[names[i]]=''
        end
      end
    else
      Data[val]=''
      if config.logmodified then
        f:write('Made ', val, ' empty: ', tostring(Data[val]), "\n")
      end
    end
  end
  
  -- remove tags
  for _, val in ipairs(config.TagsToRemove) do
    if tonumber(val, 16) then
      if config.logmodified then
        f:write('Remove group ', val, "\n")
      end
      for i=1, #groups do
        if tonumber(groups[i])==tonumber(val, 16) and tonumber(elements[i])~=0 then
          Data[names[i]]=nil
        end
      end
    else
      Data[val]=nil
      if config.logmodified then
        f:write('Removed ', val, ': ', tostring(Data[val]), "\n")
      end
    end
  end
  
  Data.PatientIdentityRemoved='YES'
  Data.DeidentificationMethod='Conquest:' ..Global.DGATEVERSION..'; anonymizer:'.. scriptversion..' '..stage

  Data.DeidentificationMethodCodeSequence=nil
  Data.DeidentificationMethodCodeSequence={}
  local n=0
  Data.DeidentificationMethodCodeSequence[n].CodeValue = '113100'
  Data.DeidentificationMethodCodeSequence[n].CodingSchemeDesignator = 'DCM'
  Data.DeidentificationMethodCodeSequence[n].CodeMeaning = 'Basic Application Confidentiality Profile'
  n=n+1
  Data.DeidentificationMethodCodeSequence[n].CodeValue = '113105'
  Data.DeidentificationMethodCodeSequence[n].CodingSchemeDesignator = 'DCM'
  Data.DeidentificationMethodCodeSequence[n].CodeMeaning = 'Clean Descriptors Option'
  n=n+1
  if dateoffset~='' then
    Data.DeidentificationMethodCodeSequence[n].CodeValue = '113107'
    Data.DeidentificationMethodCodeSequence[n].CodingSchemeDesignator = 'DCM'
    Data.DeidentificationMethodCodeSequence[n].CodeMeaning = 'Retain Longitudinal Temporal Information Modified Dates Option'
    n=n+1
    Data.LongitudinalTemporalInformationModified = 'MODIFIED'
  else
    Data.DeidentificationMethodCodeSequence[n].CodeValue = '113106'
    Data.DeidentificationMethodCodeSequence[n].CodingSchemeDesignator = 'DCM'
    Data.DeidentificationMethodCodeSequence[n].CodeMeaning = 'Retain Longitudinal Temporal Information'
    n=n+1
    Data.LongitudinalTemporalInformationModified = 'UNMODIFIED'
  end
  Data.DeidentificationMethodCodeSequence[n].CodeValue = '113109'
  Data.DeidentificationMethodCodeSequence[n].CodingSchemeDesignator = 'DCM'
  Data.DeidentificationMethodCodeSequence[n].CodeMeaning = 'Retain Device Identity Option'
  n=n+1
  
  f:write("\n\n")
  f:close();
end
