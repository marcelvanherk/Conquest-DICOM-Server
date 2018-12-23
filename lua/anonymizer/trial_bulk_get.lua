local TrialID = TrialID or 'trial1'
local PACS = 'CONQUESTSRV1'

require('csv')

subjects = readCSV('keytables/'..TrialID..'.csv')
for k, v in ipairs(subjects) do
  q = DicomObject:new()
  q.PatientID = v.PatientID
  dicommove(PACS, 'ANONYMIZER', q)
end

