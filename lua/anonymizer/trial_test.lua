-- for testing only: clear the database
q = DicomObject:new()
dicomdelete(q)
servercommand('initializetables:')

local a=os.clock()
dofile('anonymizer\\trial_bulk_get.lua')

require('csv')

TrialID = 'trial1'
dofile('anonymizer\\trial_collect.lua')
TrialID = 'trial2'
dofile('anonymizer\\trial_collect.lua')
print(os.clock()-a)

os.execute('copy trial1\\*.zip data\\incoming')
os.execute('copy trial2\\*.zip data\\incoming')

--TrialID = 'trial1'
--dofile('anonymizer\\trial_delete.lua')
--TrialID = 'trial1'
--dofile('anonymizer\\trial_restore.lua')
--os.execute('copy trial1\\trial1_1.zip save.zip')
--TrialID = 'trial1'
--dofile('anonymizer\\trial_collect.lua')
