-- updated to pass compress mode, mvh 20140126
-- updated to suppress characters like umlaut Jorg 20160609

print('Content-Type: application/xml\n')

local patid = string.gsub(series2, ':.*$', '')
local seriesuid = string.gsub(series2, '^.*:', '')
local q = DicomObject:new()
q.QueryRetrieveLevel = 'IMAGE'
q.PatientID = patid
q.SeriesInstanceUID = seriesuid
q.SOPInstanceUID = ''
q.PatientBirthDate = ''
q.PatientName = ''
q.StudyInstanceUID = ''
q.StudyDescription = ''
q.StudyDate = ''
q.StudyTime = ''
q.SeriesDescription = ''
q.SeriesNumber = ''
q.Modality = ''
q.ImageNumber = ''
r = dicomquery(servercommand('get_param:MyACRNema'), 'IMAGE', q)

-- ascii compliance insert: remove  non alphanumeric characters 06/2016 
r[0].PatientName =r[0].PatientName:gsub('%W','_')
r[0].StudyDescription =r[0].StudyDescription:gsub('%W','_')
r[0].SeriesDescription = r[0].SeriesDescription:gsub('%W','_')
-- end ascii compliance insert 

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


print([[
<?xml version="1.0" encoding="utf-8" ?>
<wado_query xmlns= "http://www.weasis.org/xsd" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" wadoURL="]]..webscriptadress..[[" requireOnlySOPInstanceUID="false" overrideDicomTagsList="0x00000000" >
  <Patient PatientID="]]..patid..[[" PatientName="]]..r[0].PatientName..[[" PatientBirthDate="]]..r[0].PatientBirthDate..[[" >
    <Study StudyInstanceUID="]]..r[0].StudyInstanceUID..[[" StudyDescription="]]..r[0].StudyDescription..[[" StudyDate="]]..r[0].StudyDate..[[" StudyTime="]]..r[0].StudyTime..[[" >
      <Series SeriesInstanceUID="]]..r[0].SeriesInstanceUID..[[" SeriesDescription="]]..r[0].SeriesDescription..[[" SeriesNumber="]]..r[0].SeriesNumber..[[" Modality="]]..r[0].Modality..[["]]..wt..[[ >
]])

for i=0, #r-1 do
  print([[<Instance SOPInstanceUID="]]..r[i].SOPInstanceUID..[[" InstanceNumber="]]..i..[[" />]])
end

print([[
      </Series>
    </Study>
  </Patient>
</wado_query>
]])
