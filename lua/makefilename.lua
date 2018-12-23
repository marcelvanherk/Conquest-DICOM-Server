-- Use with FileNameSyntax=lua:dofile('makefilename.lua')

local n = Data.PatientName
-- process n here
return n .. Data.PatientSex .. Data.PatientAge .. Data.PatientBirthDate .. Data.SOPInstanceUID .. '.dcm'
