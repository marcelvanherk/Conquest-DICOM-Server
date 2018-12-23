require('csv')

TrialID = TrialID or 'Trial1'
local TrialFolder = 'C:\\dicomanonymizer\\'..TrialID

mkdir(TrialFolder)

function ExportData(InternalPatientID, StudyUID, SeriesUID, filename, PatientID, SubjectKey, TrialID)
  print('Exporting data for ', TrialID, PatientID, InternalPatientID, SubjectKey)
  servercommand('export:'..InternalPatientID..','..(StudyUID or '')..','..(SeriesUID or '')..',,'..
                 filename..',lua/anonymize_script.lua('..SubjectKey..'|'..TrialID..')')
end

subjects = readCSV('keytables/'..TrialID..'.csv')
for k, v in ipairs(subjects) do
  local InternalID = changeuid(v.PatientID)
  local filename = TrialFolder .. '\\' .. v.SubjectKey .. '.zip'
  ExportData(InternalID, nil, nil, filename, v.PatientID, v.SubjectKey, v.TrialID)
end
