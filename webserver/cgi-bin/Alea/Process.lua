-- provides collect, submit at study or series level
-- Alea version 20130926
-- Use submit2 (points to curl), without password 20130905

function split(str, pat)
   local t = {}
   local fpat = "(.-)" .. pat
   local last_end = 1
   local s, e, cap = str:find(fpat, 1)
   while s do
      if s ~= 1 or cap ~= "" then
	 table.insert(t,cap)
      end
      last_end = e+1
      s, e, cap = str:find(fpat, last_end)
   end
   if last_end <= #str then
      cap = str:sub(last_end)
      table.insert(t, cap)
   end
   return t
end

function findpattern(text, pattern, start)
  if string.find(text, pattern, start) then
    return string.sub(text, string.find(text, pattern, start))
  else
    return ""
  end
end

HTML('Content-type: text/html\n\n');

print([[
<div font="12px Tahoma">
<H3>Conquest DICOM server version: ]]..version..[[</H3>
<p>&nbsp</p>

<p><b><font color = #FF0000"></p>
<div id = "pbar"></div></b></font>

<script type = "text/javascript">
var percent = 0; // adjust starting value to suit
var timePeriod = 100; // adjust milliseconds to suit
var status='';

function getBar()
{ var retBar = '';
  for (i = 0; i < percent/2; i++) {retBar += "|";}
  return retBar;
}

function progressBar()
{ if (percent >= 0)
  { document.getElementById("pbar").innerHTML = "&nbsp &nbsp &nbsp &nbsp Loading : " + percent + " %" + " " + getBar();
    setTimeout ("progressBar()", timePeriod);
  }
  else
  { document.getElementById("pbar").innerHTML = "";
    document.body.style.display = "";
  }
}

function Status()
{ document.getElementById("myframe").src = "]]..script_name..[[?mode=Alea/SubmitStatus";
  setTimeout ("Status()", timePeriod);
}
</script>
]])

local key, source;

key = CGI('key')
source = gpps('Alea', 'Source', '(local)');

if CGI('option')=='submitstudy' or CGI('option')=='submitseries' then
  print([[
  <script>
  progressBar();
  </script>
  ]])

  if source=='(local)' then
    s = servercommand('get_param:MyACRNema')
  else
    s = source;
  end
  b=newdicomobject();
  b.PatientID = CGI('patientid');
  b.StudyInstanceUID = CGI('studyuid');
  b.SeriesInstanceUID = CGI('seriesuid', '')
  b.QueryRetrieveLevel = 'SERIES'

  HTML('<H3>Collecting data set</H3>')
  dicommove(s, servercommand('get_param:MyACRNema'), b,
    [[--print(Global.StatusString);
      --print'<br />'
      print('<script>percent=')
      print(math.floor(100*Command.NumberOfCompletedSuboperations/
	(Command.NumberOfCompletedSuboperations+Command.NumberOfRemainingSuboperations)))
      print('</script>')
    ]]
    );
  HTML('Done collecting data')
  print([[<script>percent = -1;</script>]])
  HTML('<br />')
  HTML('<br />')
  
  local targetserver = gpps('Alea', 'TargetServer', 'user@server.domain:');
  local password = gpps('Alea', 'Password', 'xxxxxxx');

  HTML('<H3>Submitting data to ECRF</H3>');
  HTML('<br />');
  HTML('<b>Alea SubjectKey</b>');
  HTML('<br />' .. CGI('subjectKey'));
  HTML('<br />');
  
  HTML('<b>Patient ID</b>');
  HTML('<br />' .. CGI('patientid'));
  HTML('<br />');
  
  HTML('<b>StudyUID</b>');
  HTML('<br />' .. CGI('studyuid'));
  HTML('<br />');
  
  HTML('<b>SeriesUID</b>');
  HTML('<br />' ..CGI('seriesuid', ''));

  servercommand(				
  'submit2:'..
  CGI('patientid')..','..
  CGI('studyuid')..','..
  CGI('seriesuid', '')..',,,'..
  targetserver..','..
  'call anonymize_script.cq; set PatientID to "'..CGI('subjectKey')..'"'
  )		
end
HTML('</div>');
