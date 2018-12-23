-- 20120302        mvh        Created

function getstudyuid()
  local a,b,s;
  if CGI('source')=='(local)' then
    s = 'SRTPACSGW'
  else
    s = CGI('source')
  end
  b=newdicomobject();
  b.PatientID = CGI('patientidmatch');
  b.SeriesInstanceUID = CGI('seriesUID');
  b.StudyInstanceUID = '';
  b.Modality = '';
  a=dicomquery(s, 'SERIES', b);
  return a[0].StudyInstanceUID, a[0].Modality
end

function queryimages()
  local images,imaget,b,s;
  if CGI('source')=='(local)' then
    s = 'SRTPACSGW'
  else
    s = CGI('source')
  end

  b=newdicomobject();
  b.PatientID        = CGI('patientidmatch')
  b.SeriesInstanceUID= CGI('seriesUID');
  b.SOPInstanceUID   = '';

  images=dicomquery(s, 'IMAGE', b);

  imaget={}
  for k=0,#images-1 do
    imaget[k+1]={}
    imaget[k+1].SOPInstanceUID=images[k].SOPInstanceUID
  end
  table.sort(imaget, function(a,b) return a.SOPInstanceUID<b.SOPInstanceUID end)

  return imaget
end

HTML('Content-type: text/html\n\n');
HTML('<BODY>')
print(CGI('source'))
    s = servercommand('get_param:MyACRNema')
print(s)
local studyuid, modality = getstudyuid()
local i, images;
images = queryimages();
if #images > 0 then
  print('yes')
else
  print('no')
end
HTML('</BODY>')
