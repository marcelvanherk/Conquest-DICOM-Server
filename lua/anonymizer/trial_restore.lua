require('csv')
local PACS = 'CONQUESTSRV1'

TrialID = TrialID or 'Trial1'
local TrialFolder = 'C:\\dicomanonymizer\\'..TrialID

function RestoreData(InternalPatientID, StudyUID, SeriesUID, PatientID, SubjectKey, TrialID)
  print('Restoring data for ', TrialID, PatientID, InternalPatientID, SubjectKey)
  local q = DicomObject:new()
  q.PatientID = PatientID
  q.StudyInstanceUID = changeuidback(StudyUID or '', TrialID)
  q.SeriesInstanceUID = changeuidback(SeriesUID or '', TrialID)
  dicommove(PACS, 'ANONYMIZER', q)
end

subjects = readCSV('keytables/'..TrialID..'.csv')
for k, v in ipairs(subjects) do
  local InternalID = changeuid(v.PatientID)
  RestoreData(InternalID, nil, nil, v.PatientID, v.SubjectKey, v.TrialID)
end
