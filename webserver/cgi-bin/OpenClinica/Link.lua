-- Show list of studies / series that match query on passed date, modality, and patientID
-- allows: select, unselect, collect, submit at study or series level
-- allows: view at series level
-- test version 20120826

require 'ecrf/split'

local source = gpps('OpenClinica', 'Source', '(local)');

function OCDateToDICOMDate(datein)
  if datein=='' then
    return '';
  end;

  local year, month, day;
  day = string.sub(datein, 1, 2);
  month = string.sub(datein, 4, 6);
  year = string.sub(datein, 8, 11);
  if month=='Jan' then month = '01' end
  if month=='Feb' then month = '02' end
  if month=='Mar' then month = '03' end
  if month=='Apr' then month = '04' end
  if month=='May' then month = '05' end
  if month=='Jun' then month = '06' end
  if month=='Jul' then month = '07' end
  if month=='Aug' then month = '08' end
  if month=='Oct' then month = '09' end
  if month=='Sep' then month = '10' end
  if month=='Nov' then month = '11' end
  if month=='Dec' then month = '12' end
  return year..month..day
end;

function queryallseries()
  local series, seriest, b, s;

  if source=='(local)' then
    s = servercommand('get_param:MyACRNema')
  else
    s = source;
  end

  b=newdicomobject();
  b.PatientID        = CGI('patientId');
  b.StudyDate        = OCDateToDICOMDate(CGI('date'));
  b.StudyInstanceUID = '';
  b.StudyDescription = '';
  b.SeriesDescription= '';
  b.SeriesInstanceUID= '';
  b.Modality         = CGI('modality');
  b.SeriesTime       = '';

  series=dicomquery(s, 'SERIES', b);

  -- convert returned DDO (userdata) to table; needed to allow table.sort
  seriest={}
  for k1=0,#series-1 do
    seriest[k1+1]={}
    seriest[k1+1].StudyDate        = series[k1].StudyDate
    seriest[k1+1].PatientID        = series[k1].PatientID
    seriest[k1+1].StudyDate        = series[k1].StudyDate
    seriest[k1+1].SeriesTime       = series[k1].SeriesTime
    seriest[k1+1].StudyInstanceUID = series[k1].StudyInstanceUID
    seriest[k1+1].SeriesDescription= series[k1].SeriesDescription
    seriest[k1+1].StudyDescription = series[k1].StudyDescription
    seriest[k1+1].SeriesInstanceUID= series[k1].SeriesInstanceUID
    seriest[k1+1].Modality         = series[k1].Modality
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
body { font:10pt Verdana; }
a { color:blue; }
#content { background-color:#dddddd; width:200px; margin-top:2px; }
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
</script>
]]

print
[[
<SCRIPT LANGUAGE="JavaScript">
function popUp(URL)
{ day = new Date();
  id = day.getTime();
  eval("page" + id + " = window.open(URL, '" + id + "', 'toolbar=0,scrollbars=0,location=0,statusbar=0, menubar=0,resizable=1,width=600,height=500,left = 0,top = 0');");
}
</script>
]]

print
[[
<script>
function PopupCenter(pageURL, title,w,h)
{ var left = (screen.width/2)-(w/2);
  var top = (screen.height/2)-(h/2);
  var targetWin = window.open (pageURL, title, 'toolbar=no, location=no, directories=no, status=no, menubar=no, scrollbars=no, resizable=no, copyhistory=no, width='+w+', height='+h+', top='+top+', left='+left);
}
</script>
]]

local eventid = CGI('eventCRFId');
if eventid=='' then eventid = CGI('studyEventId'); end;

local key = 
  'subjectKey='  ..CGI('subjectKey')..';'..
  'eventCRFId='  ..eventid..';'..
  'item='        ..CGI('item');
local query = "key='".. key .."'"

local series=queryallseries()
table.sort(series, function(a,b) return a.StudyInstanceUID<b.StudyInstanceUID end)

print('<H3>Scans for patient: '..CGI('patientId')..'</H3>')

print'<ul>'
for i=1,#series do
  if series[i].StudyDate=='' then series[i].StudyDate = series[i].SeriesDate end
  if series[i].StudyDate=='' or series[i].StudyDate==nil then series[i].StudyDate = 'Unknown' end

  local split = (i==1) or (series[i-1].StudyInstanceUID ~= series[i].StudyInstanceUID)
  local anonstudyuid = servercommand('changeuid:'..series[i].StudyInstanceUID);

  if split and i~=1 then
    print '    </ul>'
    print '  </li>'
  end

  if split then
    print ('  <li>')
	print ('    Date:')
	print([[    <a href="#" onclick="toggle('node]]..i..[[')">]],  series[i].StudyDate)
	print('    </a>')
	print(     series[i].StudyDescription)

	print([[     <a href=# onclick="javascript:window.opener.postMessage(']]..CGI('item')..[[:]]
             	..CGI('subjectKey')..[[:]]..anonstudyuid..
		[[', '*')">[select]</a>]]
             )

	print([[     <a href=# onclick="javascript:window.opener.postMessage(']]..CGI('item')..[[:]]
             	..
		[[', '*')">[unselect]</a>]]
             )

	print([[   <a href=# onclick="javascript:PopupCenter(
		']]..script_name..[[?mode=OpenClinica/Process&option=collectstudy&patientid=]]..
		series[i].PatientID..'&studyuid='..
		series[i].StudyInstanceUID..[[', 'Scan', 400, 500)">[collect]</a>]]
             )

	print([[   <a href=# onclick="javascript:PopupCenter(
		']]..script_name..[[?mode=OpenClinica/Process&option=submitstudy&patientid=]]..
		series[i].PatientID..'&studyuid='..series[i].StudyInstanceUID..
		[[&key=]]..key..[[', 'Scan', 400, 500)">[submit]</a>]]
	     )

	if #series>10 then
	  print('    <ul id=node'..i..' style="display:none">')
	else
	  print('    <ul id=node'..i..' >')
	end
  end

  print('    <li>')

  print([[     <a href=# onclick="javascript:PopupCenter(']]
                ..script_name..[[?mode=OpenClinica/wadoviewer&patientidmatch=]]
		..series[i].PatientID..[[&seriesUID=]]..series[i].SeriesInstanceUID..[[&source=]]..source
		..[[', 'Scan', 400, 500)">[view]</a>]]
       )

  local anonseriesuid = servercommand('changeuid:'..series[i].SeriesInstanceUID);

  print([[     <a href=# onclick="javascript:window.opener.postMessage(']]..CGI('item')..[[:]]
             	..CGI('subjectKey')..[[:]]..anonstudyuid..[[:]]..anonseriesuid..
		[[', '*')">[select]</a>]]
       )

  print([[     <a href=# onclick="javascript:window.opener.postMessage(']]..CGI('item')..[[:]]
             	..
		[[', '*')">[unselect]</a>]]
       )

  print([[     <a href=# onclick="javascript:PopupCenter(
		']]..script_name..[[?mode=OpenClinica/Process&option=collectseries&patientid=]]
             	..series[i].PatientID..'&studyuid='..series[i].StudyInstanceUID
		..'&seriesuid='..series[i].SeriesInstanceUID..[[&key=]]..key
		..[[', 'Scan', 400, 500)">[collect]</a>]]
       )

  print([[     <a href=# onclick="javascript:PopupCenter(
		']]..script_name..[[?mode=OpenClinica/Process&option=submitseries&patientid=]]
             	..series[i].PatientID..'&studyuid='..series[i].StudyInstanceUID
		..'&seriesuid='..series[i].SeriesInstanceUID..[[&key=]]..key
		..[[', 'Scan', 400, 500)">[submit]</a>]]
       )

  print('      ', series[i].Modality, series[i].SeriesTime, series[i].SeriesDescription)
print('    </li>')
end

print '    </ul>'
print '  </li>'
print'</ul>'

print('<br>Mode', CGI('mode'))
print('<br>item', CGI('item'))
print('<br>modality', CGI('modality'))
print('<br>date', CGI('date'), OCDateToDICOMDate(CGI('date')))
print('<br>subjectId', CGI('subjectId'))
print('<br>eventDefinitionCRFId', CGI('eventDefinitionCRFId'))
print('<br>studyEventId', CGI('studyEventId'))
print('<br>eventCRFId', CGI('eventCRFId'))
-- print('<br>sectionId', CGI('sectionId'))
print('<br>patientId', CGI('patientId'))
print('<br>subjectKey', CGI('subjectKey'))
print('<br>link', CGI('link'))
print('<br>key', key)
print('<br>extra', script_name)

print
[[
</body>
</html>
]]

