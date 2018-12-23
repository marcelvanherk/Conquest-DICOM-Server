-- Show list of studies / series that match query on passed date, modality, and patientID
-- allows: select, unselect, collect, submit at study or series level
-- allows: view at series level
-- 20130926 RvdB require 'Alea/split'
-- 20141031 mvh+svk: Split hierarchical query into 2 non-hier levels
-- 20141102 mvh+svk: Pass studyuid to viewer; put quotes around header info

-- uit Dicom.ini server config (= Alea/config)
local source = gpps('Alea', 'Source', '(local)');


function DICOMDateToDisplay(datein)
  if datein=='' then
    return '';
  end;
  local year, month, day;
  day = string.sub(datein, 7, 8);
  month = string.sub(datein, 5, 6);
  year = string.sub(datein, 1, 4);
  if month=='01' then month = 'Jan' end
  if month=='02' then month = 'Feb' end
  if month=='03' then month = 'Mar' end
  if month=='04' then month = 'Apr' end
  if month=='05' then month = 'May' end
  if month=='06' then month = 'Jun' end
  if month=='07' then month = 'Jul' end
  if month=='08' then month = 'Aug' end
  if month=='09' then month = 'Sep' end
  if month=='10' then month = 'Oct' end
  if month=='11' then month = 'Nov' end
  if month=='12' then month = 'Dec' end
  return day..' '..month..' '..year
end;

-- parameters from cgi
function queryallseries()
  if source=='(local)' then
    s = servercommand('get_param:MyACRNema')
  else
    s = source;
  end

  local b1=newdicomobject();
  b1.PatientID        = CGI('patientId');
  b1.StudyDate        = CGI('date');
  b1.StudyInstanceUID = '';
  b1.StudyDescription = '';
  local studies=dicomquery(s, 'STUDY', b1);

  local seriest={}

  for i=0, #studies-1 do
    local b=newdicomobject();
    b.PatientID        = CGI('patientId');
    b.StudyDate        = CGI('date');
    b.Modality         = CGI('modality');
    b.StudyInstanceUID = studies[i].StudyInstanceUID;
    b.SeriesDescription= '';
    b.SeriesInstanceUID= '';
    b.SeriesTime       = '';

    local series=dicomquery(s, 'SERIES', b);

    for k=0,#series-1 do
      seriest[#seriest+1]={}
      seriest[#seriest].StudyDate        = studies[i].StudyDate
      seriest[#seriest].PatientID        = studies[i].PatientID
      seriest[#seriest].StudyInstanceUID = studies[i].StudyInstanceUID   
      seriest[#seriest].StudyDescription = studies[i].StudyDescription

      seriest[#seriest].SeriesDescription= series[k].SeriesDescription
      seriest[#seriest].SeriesTime       = series[k].SeriesTime
      seriest[#seriest].SeriesInstanceUID= series[k].SeriesInstanceUID
      seriest[#seriest].Modality         = series[k].Modality
    end
  end
  
  return seriest
end

HTML('Content-type: text/html\n\n');

print
[[
<html>
<head>
<title>Browse scans</title>
<style type="text/css">
body { 
  font-family:Tahoma;
  font-size:12px;
  font-style:normal;
  font-variant:normal;
  font-weight:normal;
  line-height:normal;
  color:#000000;
}  
a { 
  color:#0d45b7; 
}
#content {
   background-color:#dddddd; 
   width:200px; 
   margin-top:2px; 
}
.date
{
	padding: 4px;
}
.dthdr
{
   border: 1px solid silver;
   background-color: #FFFFD4;	
   margin-bottom: 3px;
}
</style>
</head>
<body>
]]

print
[[
<script type="text/javascript">
function toggle(id)
{ var e = document.getElementById(id);
  if (e.style.display == '')
    e.style.display = 'none';
  else
    e.style.display = '';
}

function sendView( url, title){
	var msg = [];
	var sendMsg = '';
	msg.push('view');
	msg.push(url);
	msg.push(title);
	sendMsg = msg.join('~');
	parent.postMessage(sendMsg,'*');
}
function sendCollect(url, title){
	var msg = [];
	var sendMsg = '';
	msg.push('collect');
	msg.push(url);
	msg.push(title);
	sendMsg = msg.join('~');
	parent.postMessage(sendMsg,'*');
}
function sendSubmit(url,key,StudyDate,SeriesTime,StudyInstanceUID,SeriesDescription,SeriesInstanceUID,Modality,anonseriesuid,anonstudyuid){
	var msg = [];
	var sendMsg = '';
	msg.push('submit');
	
	msg.push(key);
	msg.push(StudyDate);
	msg.push(SeriesTime);
	msg.push(StudyInstanceUID);
	msg.push(SeriesDescription);
	msg.push('');
	msg.push(SeriesInstanceUID);
	msg.push(Modality);
	msg.push(anonseriesuid);
	msg.push(anonstudyuid);
	msg.push(url);
	sendMsg = msg.join('~');
	parent.postMessage(sendMsg,'*');
}
</script>
]]


local eventid = CGI('eventCRFId');
if eventid=='' then eventid = CGI('studyEventId'); end;

local key = 
  'Study='     .. CGI('StudyName') ..';'..
  'ECRF='      .. CGI('ECRFName')  ..';'..
  'Occurance=' .. CGI('OccuranceNumber');

local series=queryallseries()
table.sort(series, function(a,b) return a.StudyInstanceUID<b.StudyInstanceUID end)

print('<H3>'..CGI('modality')..' scans for patient: '..CGI('patientId')..'</H3>')
if CGI('date')~='' then
  print('<div class="dthdr"><b>'..DICOMDateToDisplay(CGI('date'))..'</b></div>')
end

print'<div id="main">'
for i=1,#series do
  if series[i].StudyDate=='' then series[i].StudyDate = series[i].SeriesDate end
  if series[i].StudyDate=='' or series[i].StudyDate==nil then series[i].StudyDate = 'Unknown' end

  local split = (i==1) or (series[i-1].StudyInstanceUID ~= series[i].StudyInstanceUID)
  local anonstudyuid = servercommand('changeuid:'..series[i].StudyInstanceUID);

  if split and i~=1 then
    print '    </div>'
    print '  </div>'
  end

  
  if split then
    print ('  <div class="date"><div class="dthdr">')	
	print ('    Date:')
	print([[    <a href="#" onclick="toggle('node]]..i..[[')">]],  DICOMDateToDisplay(series[i].StudyDate))
	print('    </a>')
	print(     series[i].StudyDescription)
	print('    </div>')
		
	if #series>200 then
	  print('<div id=node'..i..' style="display:none">')
	else
	  print('<div id=node'..i..' >')
	end
  end

  print('<div>')

  local anonseriesuid = servercommand('changeuid:'..series[i].SeriesInstanceUID);
  local header = series[i].Modality..[[ - ]]..series[i].SeriesTime..[[ - ]]..series[i].SeriesDescription		  
  print([[     <a href=# onclick="javascript:sendView(']]
                ..script_name..[[?mode=Alea/wadoviewer&patientidmatch=]]
		..series[i].PatientID..[[&studyUID=]]..series[i].StudyInstanceUID..[[&seriesUID=]]..series[i].SeriesInstanceUID..[[&source=]]..source..[[&header="]]..header..[["]]..
		..[[', 'Scan')">[view]</a>]]
       )



--  print([[     <a href=# onclick="javascript:sendCollect(
--		']]..script_name..[[?mode=Alea/Process&option=collectseries&patientid=]]
--             	..series[i].PatientID..'&studyuid='..series[i].StudyInstanceUID
--		..'&seriesuid='..series[i].SeriesInstanceUID..[[&key=]]..key
--		..[[', 'Scan')">[collect]</a>]]
--       )
		
  print([[     <a href=# onclick="javascript:sendSubmit(
		']]..script_name..[[?mode=Alea/Process&option=submitseries&patientid=]]
             	..series[i].PatientID..'&studyuid='..series[i].StudyInstanceUID
		..'&seriesuid='..series[i].SeriesInstanceUID..[[&key=]]..key..[[&header=]]..header
		..[[', ']]..key
		..[[', ']]..series[i].StudyDate
		..[[', ']]..series[i].SeriesTime
		..[[', ']]..series[i].StudyInstanceUID
		..[[', ']]..series[i].SeriesDescription
		..[[', ']]..series[i].SeriesInstanceUID
		..[[', ']]..series[i].Modality
		..[[', ']]..anonseriesuid
		..[[', ']]..anonstudyuid   		
		..[[')">[submit]</a>]]
       )

  if CGI('link')==CGI('subjectKey')..':'..anonstudyuid..':'..anonseriesuid then
    print('<b>')
  end

  -- print(key2 ..'<br/>')
  print('      ', series[i].Modality, series[i].SeriesTime, series[i].SeriesDescription)

  if CGI('link')==CGI('subjectKey')..':'..anonstudyuid..':'..anonseriesuid then
    print(' (selected)</b>')
  end
print('    </div>')
end

print '    </div>'
print '  </div>'
print'</div>'

if #series==0 then
  print('<H2><B>No '..CGI('modality')..' scans found</B></H2>');
end  

print
[[
</body>
</html>
]]

