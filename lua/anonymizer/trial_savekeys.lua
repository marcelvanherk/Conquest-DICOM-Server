require('csv')
TrialID = TrialID or 'Trial1'

t = dbquery('UIDMODS', 'MODTime, OldUID, MODType, NewUID, Stage', 'Stage="(empty)"')
writeCSV(t, 'keytables/'..TrialID..'_modifiers1.csv') -- {'MODTime', 'OldUID', 'MODType', 'NewUID', 'Stage'}

t = dbquery('UIDMODS', 'MODTime, OldUID, MODType, NewUID, Stage', 'Stage="'..TrialID..'"')
writeCSV(t, 'keytables/'..TrialID..'_modifiers2.csv') -- {'MODTime', 'OldUID', 'MODType', 'NewUID', 'Stage'}

subjects = readCSV('keytables/'..TrialID..'.csv')
for k, v in ipairs(subjects) do
  local InternalID = changeuid(v.PatientID)
end
