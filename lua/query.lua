-- query a dicom server

a=DicomObject:new()
a.QueryRetrieveLevel='STUDY'
a.StudyInstanceUID=''
a.StudyDate='20010101-20130404'
a["9999,0c00"]="StudyDate"
--a["9999,0c01"]="5"
--a["9999,0c02"]=""
b=dicomquery('CONQUESTSRV1', 'STUDY', a)
for i=0, #b-1 do
  print(b[i].StudyDate, b[i].StudyInstanceUID)
end
