-- updated to pass compress mode, mvh 20140126
-- updated to suppress characters like umlaut Jorg 20160609

print('Content-Type: application/xml\n')


local patid = string.gsub(study2, ':.*$', '')
local studyuid = string.gsub(study2, '^.*:', '')
local q = DicomObject:new()
q.QueryRetrieveLevel = 'SERIES'
q.PatientID = patid
q.StudyInstanceUID = studyuid
q.PatientBirthDate = ''
q.PatientName = ''
q.StudyDescription = ''
q.StudyDate = ''
q.StudyTime = ''
q.SeriesInstanceUID = ''
q.SeriesDescription = ''
q.SeriesNumber = ''
q.Modality = ''
r = dicomquery(servercommand('get_param:MyACRNema'), 'SERIES', q)

wt = ""
if compress=='J3' or compress=='j3' then wt = ' WadoTransferSyntaxUID="1.2.840.10008.1.2.4.50"' end
if compress=='J4' or compress=='j4' then wt = ' WadoTransferSyntaxUID="1.2.840.10008.1.2.4.51"' end
if compress=='J5' or compress=='j5' then wt = ' WadoTransferSyntaxUID="1.2.840.10008.1.2.4.53"' end
if compress=='J1' or compress=='j1' then wt = ' WadoTransferSyntaxUID="1.2.840.10008.1.2.4.55"' end
if compress=='J2' or compress=='j2' then wt = ' WadoTransferSyntaxUID="1.2.840.10008.1.2.4.57"' end
if compress=='J1' or compress=='j1' then wt = ' WadoTransferSyntaxUID="1.2.840.10008.1.2.4.70"' end
if compress=='JK' or compress=='jk' then wt = ' WadoTransferSyntaxUID="1.2.840.10008.1.2.4.90"' end
if compress=='JL' or compress=='jl' then wt = ' WadoTransferSyntaxUID="1.2.840.10008.1.2.4.91"' end

local s = DicomObject:new()

-- ascii compliance insert: remove  non alphanumeric characters 06/2016
r[0].PatientName =r[0].PatientName:gsub('%W','_')
r[0].StudyDescription =r[0].StudyDescription:gsub('%W','_')
-- end ascii compliance insert 

print([[
<?xml version="1.0" encoding="utf-8" ?>
<wado_query xmlns= "http://www.weasis.org/xsd" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" wadoURL="]]..webscriptadress..[[" requireOnlySOPInstanceUID="false" overrideDicomTagsList="0x00000000">
  <Patient PatientID="]]..patid..[[" PatientName="]]..r[0].PatientName..[[" PatientBirthDate="]]..r[0].PatientBirthDate..[[" >
    <Study StudyInstanceUID="]]..r[0].StudyInstanceUID..[[" StudyDescription="]]..r[0].StudyDescription..[[" StudyDate="]]..r[0].StudyDate..[[" StudyTime="]]..r[0].StudyTime..[[" >
]])

for i=0, #r-1 do

-- ascii compliance insert: remove  non alphanumeric characters 06/2016
r[i].SeriesDescription = r[i].SeriesDescription:gsub('%W','_')
-- end ascii compliance insert

  print([[
      <Series SeriesInstanceUID="]]..r[i].SeriesInstanceUID..[[" SeriesDescription="]]..r[i].SeriesDescription..[[" SeriesNumber="]]..r[i].SeriesNumber..[[" Modality="]]..r[i].Modality..[["]]..wt..[[ >
  ]])

  s.QueryRetrieveLevel = 'IMAGE'
  s.SOPInstanceUID = ''
  s.ImageNumber = ''
  s.SeriesInstanceUID = r[i].SeriesInstanceUID
  t = dicomquery(servercommand('get_param:MyACRNema'), 'IMAGE', s)
  for j=0, #t-1 do
    print([[<Instance SOPInstanceUID="]]..t[j].SOPInstanceUID..[[" InstanceNumber="]]..j..[[" />]])
  end

  print([[
      </Series>
]])
end

print([[
    </Study>
  </Patient>
</wado_query>
]])

