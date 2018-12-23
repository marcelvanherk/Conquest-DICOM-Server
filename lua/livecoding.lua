-- 1.4.17d
a=DicomObject:new()
a.QueryRetrieveLevel='STUDY'
a.StudyInstanceUID=''
a.StudyDate='19980101-20010404'
b=dicomquery('CONQUESTSRV1', 'STUDY', a)
for i=0, #b-1 do
  print(b[i].StudyInstanceUID)
end
