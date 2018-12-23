require('csv')

TrialID = TrialID or 'Trial1'
local TrialFolder = 'C:\\dicomanonymizer\\'..TrialID

function DeleteData(InternalPatientID, StudyUID, SeriesUID, PatientID, SubjectKey, TrialID)
  print('Deleting data for ', TrialID, PatientID, InternalPatientID, SubjectKey)
  local q = DicomObject:new()
  q.PatientID = InternalPatientID
  q.StudyInstanceUID = StudyUID
  q.SeriesInstanceUID = SeriesUID
  dicomdelete(q)
end

subjects = readCSV('keytables/'..TrialID..'.csv')
for k, v in ipairs(subjects) do
  local InternalID = changeuid(v.PatientID)
  DeleteData(InternalID, nil, nil, v.PatientID, v.SubjectKey, v.TrialID)
end
