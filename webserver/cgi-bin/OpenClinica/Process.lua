-- provides collect, submit at study or series level
-- test version 20120719

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
{ document.getElementById("myframe").src = "]]..script_name..[[?mode=OpenClinica/SubmitStatus";
  setTimeout ("Status()", timePeriod);
}
</script>
]])

local key, source;

key = CGI('key')
source = gpps('OpenClinica', 'Source', '(local)');

if CGI('option')=='collectstudy' or  CGI('option')=='collectseries' then
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
  -- it is possible to anonymize  here (add private tag; the script should set OID's)
  -- b.['9999,0900']='call anonymize.lua';
  HTML('<H3>Collecting data set</H3>')
  dicommove(s, servercommand('get_param:MyACRNema'), b,
    [[--print(Global.StatusString);
      --print'<br>'
      print('<script>percent=')
      print(math.floor(100*Command.NumberOfCompletedSuboperations/
	(Command.NumberOfCompletedSuboperations+Command.NumberOfRemainingSuboperations)))
      print('</script>')
    ]]
    );
  HTML('<H3>Done collecting data</H3>')
  print([[<script>percent = -1;</script>]])
end

if CGI('option')=='submitstudy' or CGI('option')=='submitseries' then
  print([[
  <iframe id="myframe" BORDER=0 HEIGHT=80>
  <p>Your browser does not support iframes.</p>
  </iframe>
  <script>
  Status()
  </script>
  <script>
  timePeriod=200
  </script>
  ]])

  local targetserver = gpps('OpenClinica', 'TargetServer', 'user@server.domain:');
  local password = gpps('OpenClinica', 'Password', 'xxxxxxx');

  HTML('<H3>Submitting data to ECRF</H3>');
  HTML('Key = ' .. CGI('key'));
  HTML('<br>');
  HTML('PatientID=' .. CGI('patientid'));
  HTML('<br>');
  HTML('StudyUID=' .. CGI('studyuid'));
  HTML('<br>');
  HTML('SeriesUID=' .. CGI('seriesuid', ''));
  -- it is possible to anonymize below here (replace nop by script; the script should set OID's)
  servercommand('submit:'..CGI('patientid')..','..CGI('studyuid')..','..CGI('seriesuid', '')
                ..',,'..targetserver..','..password..',anonymize_script.cq')

  --HTML('submit:'..CGI('patientid')..','..CGI('studyuid')..','..CGI('seriesuid', '')
  --              ..',,'..targetserver..','..password..',nop');

  HTML('<br>')
  HTML('<br>')
  print([[
  <script>
  timePeriod=1000
  </script>
  ]])
end

--print([[
--<a href="#" onclick="history.go(-1)">Return</a>
--]])

