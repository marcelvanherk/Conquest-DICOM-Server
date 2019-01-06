-- Test routines for server commands (dgate --xx:.... or servercommand('xx:....')
-- 1.5.0alpha

dofile('/software/dicomserver/Conquest-Dicom-Server/lua/test_utilities.lua')

print('To detect memory leaks, compare output on subsequent runs')
collectgarbage("collect"); print(heapinfo())

print('----- Testing database options -------')
print('Testing query command')
print(servercommand('query:DICOMPatients|PatientID||%s*'))
print('Testing query2 command')
print(servercommand('query2:DICOMPatients|PatientID||%s*|2'))
print('Testing patientfinder command')
print(servercommand('patientfinder:CONQUESTSRV1|00097'))
print('Testing studyfinder command')
print(servercommand('studyfinder:CONQUESTSRV1|00097'))
print('Testing seriesfinder command')
print(servercommand('seriesfinder:CONQUESTSRV1|00097'))
print('Testing imagefinder command')
print(servercommand('imagefinder:CONQUESTSRV1|00097'))
print('Testing serieslister command')
print(servercommand('serieslister:CONQUESTSRV1|0009703828'))
print('Testing imagelister command')
print(servercommand('imagelister:local|0009703828'))

print('Testing dicomstore and dicomget commands')
DeletePatient('testseries')

local series = MakeSeries(10, 'testseries')
AddArray(series)
CountImagesInPatient('testseries', 10)
DeletePatient('testseries')
dicomstore('CONQUESTSRV1', series)
CountImagesInPatient('testseries', 10)
local q=DicomObject:new()
q.PatientID='testseries'
series = dicomget('CONQUESTSRV1', 'IMAGE', q)
DeletePatient('testseries')
CountImagesInPatient('testseries', 0)
dicomstore('CONQUESTSRV1', series)
CountImagesInPatient('testseries', 10)

print('Testing movepatient, moveseries, deleteseries, imagelister and delete object commands')
DeletePatient('testseries')

local series = MakeSeries(10, 'testseries')
AddArray(series)
servercommand('movepatient:CONQUESTSRV1,CONQUESTSRV1,testseries')
servercommand('moveseries:CONQUESTSRV1,CONQUESTSRV1,testseries:'..series[0].SeriesInstanceUID)
CountImagesInPatient('testseries', 10)
servercommand('deleteimage:testseries:'..series[0].SOPInstanceUID)
servercommand('deleteimage:'..series[1].SOPInstanceUID)
CountImagesInPatient('testseries', 8)
servercommand('deletesopfromdb:testseries,'..series[0].StudyInstanceUID..','..series[0].SeriesInstanceUID..','..series[2].SOPInstanceUID)
CountImagesInPatient('testseries', 7)

local files = split(servercommand('imagelister:local|testseries'), '*')
servercommand('deleteimagefile:'..files[1])
CountImagesInPatient('testseries', 6)
servercommand('deleteimagefromdb:'..files[2])
CountImagesInPatient('testseries', 5)

--servercommand('deleteseries:testseries:'..series[0].SeriesInstanceUID)
servercommand('deleteseries:'..series[0].SeriesInstanceUID)
CountImagesInPatient('testseries', 0)

for k,v in ipairs(files) do
  os.remove(v)
end

series=nil
files=nil

print('Testing movestudy:, move: and deletestudy: commands')
DeletePatient('teststudy')
local study = MakeStudy(10, 'teststudy')
AddArray(study)
servercommand('movestudy:CONQUESTSRV1,CONQUESTSRV1,teststudy:'..study[0].StudyInstanceUID)
CountImagesInPatient('teststudy', 100)
servercommand('deletestudy:teststudy:'..study[0].StudyInstanceUID)
--servercommand('deletestudy:'..study[0].StudyInstanceUID)
CountImagesInPatient('teststudy', 0)

DeletePatient('testpatient')
study = MakeStudy(5, 'testpatient')
AddArray(study)
study = MakeStudy(5, 'testpatient')
AddArray(study)
study = MakeStudy(5, 'testpatient')
AddArray(study)
CountImagesInPatient('testpatient', 3*5*5)

servercommand('move:CONQUESTSRV1,CONQUESTSRV1,testpatient')
servercommand('move:CONQUESTSRV1,CONQUESTSRV1,testpatient,'..study[0].StudyInstanceUID)
servercommand('move:CONQUESTSRV1,CONQUESTSRV1,testpatient,'..study[0].StudyInstanceUID..','..study[0].SeriesInstanceUID)
servercommand('move:CONQUESTSRV1,CONQUESTSRV1,testpatient,'..study[0].StudyInstanceUID..','..study[0].SeriesInstanceUID..','..study[0].SOPInstanceUID)

servercommand('deletepatient:testpatient') 

CountImagesInPatient('testpatient', 0)
study=nil

print('Testing deletestudies: command')
DeletePatient('testpatient*')

study = MakeStudy(5, 'testpatient1', '19800101')
AddArray(study)
study = MakeStudy(5, 'testpatient2', '19800507')
AddArray(study)
study = MakeStudy(5, 'testpatient3', '19801130')
AddArray(study)
study = MakeStudy(5, 'testpatient3', '19801230')
AddArray(study)
CountImagesInPatient('testpatient*', 4*5*5)
servercommand('deletestudies:19801230')
CountImagesInPatient('testpatient*', 3*5*5)
servercommand('deletestudies:19800101-19800507')
CountImagesInPatient('testpatient*', 1*5*5)
servercommand('deletestudies:19800101-19801231')
CountImagesInPatient('testpatient*', 0)
study=nil

print('Testing dicommove() command')
DeletePatient('testpatient')
study = MakeStudy(5, 'testpatient')
AddArray(study)
local a = DicomObject:new()
a.QueryRetrieveLevel='STUDY'
a.StudyInstanceUID=''
a.PatientID='testpatient'
dicommove('CONQUESTSRV1', 'CONQUESTSRV1', a)

study=nil
a=nil
collectgarbage("collect"); print(heapinfo())
if true then return end

print('Testing dicomprint() command')
a = DicomObject:new()
a:Read('0009703828:1.3.46.670589.5.2.10.2156913941.892665339.718742')
dicomprint(a, 'CONQUESTSRV1')

b = DicomObject:newarray()
b:Add(a)
b:Add(a)
b:Add(a)
b:Add(a)
b:Add(a)
b:Add(a)
dicomprint(b, 'CONQUESTSRV1', 'test', 'ImageDisplayFormat=STANDARD\\2,3')

if true then return end
