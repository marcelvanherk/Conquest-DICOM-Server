-- mvh 20151206: fix 1.4.17d compatibility
-- mvh 20160124: added anonymizer trial viewer; enabled after any named anonymizing
-- mvh 20160709: fix error message if AE not in acrnema.map
-- mvh 20170211: Protect counttrials against missing UIDmods table
-- mvh 20180204: Some cleanup, report when ladle is used
-- mvh 20181111: Added missing Global.WebCodeBase for conquest icon
-- mvh 20181230: Removed no longer needed acrnema.map test, all access is remote
-- mvh 20191017: Added submit button to upload file

webscriptaddress = webscriptaddress or webscriptadress or 'dgate.exe'
local ex = string.match(webscriptaddress, 'dgate(.*)')

-- report error string on a otherwise empty reddish page
function errorpage(s)
  HTML('Content-type: text/html\n\n');
  print(
  [[
  <HEAD><TITLE>Version ]].. (version or '-')..[[ WEB interface</TITLE></HEAD>
  <BODY BGCOLOR='FFDFCF'>
  <H3>Conquest ]].. (version or '-') ..[[ WEB interface</H3>
  <H3>]].. s .. [[</H3>
  </BODY>
  ]])
end

local s    = servercommand('get_param:MyACRNema')
local port = gpps('sscscp', 'TCPPort', '');
local ip   = gpps('sscscp', 'WebServerFor', '')
local found= false

function counttrials()
  local list = {}
  local count = 
      servercommand('lua:'..
	  [[
	  local a=dbquery('UIDMods', 'DISTINCT Stage');
	  return #(a or {})
	  ]]
                   ) or 0
  return tonumber(count)
end;

if s==nil then
  errorpage('server not running')
  return
end

--[[
for i=0,100 do
  local AE,IP,P,C = get_amap(i)
  if AE==nil then break end
  if s==AE then
    servercommand('put_amap:'..i..','..AE..','..ip..','..port..','..C)
    if ip~=IP then
      errorpage('IP address of DICOM server '..AE..' is misconfigured; in cgi-bin/dicom.ini: '..ip..'; in acrnema.map: '..IP)
      return;
    end
    if port~=P then
      errorpage('IP port of DICOM server '..AE..' is misconfigured; in cgi-bin/dicom.ini: '..port..'; in acrnema.map: '..P)
      return;
    end
    found = true;
  end
end

if found==false then
  errorpage('AE port of DICOM server '..(s)..' is misconfigured; in cgi-bin/dicom.ini: '..s..'; while this AE is not defined in acrnema.map')
  return;
end
]]

local a = newdicomobject()
a.InstanceNumber = 'test'
if a.InstanceNumber ~= 'test' then
  errorpage('Web server error: old dgate.dic used')
  return
end

HTML("Content-type: text/html\nCache-Control: no-cache\n");
HTML("<HEAD>")
print [[<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">]]
HTML("<TITLE>Conquest DICOM server - version %s</TITLE>", version)
	
print([[<style type="text/css">
	body { font:10pt Verdana; }
	a { color:blue; }
	#content { background-color:#dddddd; width:200px; margin-top:2px; }
	h1 {background-color:#6495ed;}
	</style>]])

HTML("</HEAD>")
		 		
HTML("<BODY BGCOLOR='CFDFCF'>")


HTML("<H1>Welcome to Conquest DICOM server - version %s</H1>", version)
if write then 
  HTML("<H2>Written by Conquest Project - Running on Ladle</H2>")
else 
  HTML("<H2>Written by Conquest Project</H2>") 
end
HTML("<IMG SRC='%sconquest.jpg' ALT='Written by Conquest Project'>", Global.WebCodeBase)
HTML("<HR>")

HTML("<PRE>")
HTML(servercommand('display_status:'))
HTML("</PRE>")
HTML("<HR>");

HTML("<table>");
key=''  	
HTML("<tr>");
HTML("<FORM ACTION=\"dgate%s\"   onSubmit=\"if (this.patientnamematch.value == '' && this.patientidmatch.value == '') {alert('Please, fill the entry fields');return false;}\">", ex);
HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=listpatients>");
HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", port);
HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", address);
HTML("<INPUT NAME=key     TYPE=HIDDEN VALUE=%s>", key);
HTML("<td>Local Patient List");
HTML("<td>Patient ID: <INPUT NAME=patientidmatch TYPE=Text VALUE=>");
HTML("<td>Name: <INPUT NAME=patientnamematch TYPE=Text style=\"text-transform:uppercase;\" onkeyup=\"javascript:this.value=this.value.toUpperCase();\" VALUE=>");
HTML("<td>");
HTML("<td><INPUT TYPE=SUBMIT VALUE=Submit>");
HTML("</FORM>");
HTML("</tr>");	

HTML("<tr>");
HTML("<FORM ACTION=\"dgate%s\" onSubmit=\"if (this.patientnamematch.value == '' && this.patientidmatch.value == '' && this.studydatematch.value == '') {alert('Please, fill the entry fields');return false;}\">", ex);
HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=liststudies>");
HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", port);
HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", address);
HTML("<INPUT NAME=key     TYPE=HIDDEN VALUE=%s>", key);
HTML("<td>Local Studies List");
HTML("<td>Patient ID: <INPUT NAME=patientidmatch TYPE=Text VALUE=>");
HTML("<td>Name: <INPUT NAME=patientnamematch TYPE=Text style=\"text-transform:uppercase;\" onkeyup=\"javascript:this.value=this.value.toUpperCase();\" VALUE=>");
HTML("<td>Date: <INPUT NAME=studydatematch id=studydatematch TYPE=Text VALUE=>");
HTML("<td><INPUT TYPE=SUBMIT VALUE=Submit>");
HTML("<tr><td><td><td><td>")
HTML([[<INPUT TYPE=BUTTON VALUE=Today onClick="var d=new Date(); document.getElementById('studydatematch').value=(d.getFullYear()).toString()+(d.getMonth()+101).toString().substring(1,3)+(d.getDate()+100).toString().substring(1,3)">]]);
HTML([[<INPUT TYPE=BUTTON VALUE=Yesterday onClick="var d=new Date(); d=new Date(d.valueOf()-864E5); document.getElementById('studydatematch').value=(d.getFullYear()).toString()+(d.getMonth()+101).toString().substring(1,3)+(d.getDate()+100).toString().substring(1,3)">]]);
HTML([[<INPUT TYPE=BUTTON VALUE='Last 7 days' onClick="var d=new Date(); var d1=new Date(d.valueOf()-7*864E5); document.getElementById('studydatematch').value=(d1.getFullYear()).toString()+(d1.getMonth()+101).toString().substring(1,3)+(d1.getDate()+100).toString().substring(1,3)+'-'+(d.getFullYear()).toString()+(d.getMonth()+101).toString().substring(1,3)+(d.getDate()+100).toString().substring(1,3)">]]);
HTML("</FORM>");
HTML("</tr>");	

if counttrials()>1 then
  HTML("<tr>");
  HTML("<FORM ACTION=\"dgate%s\">", ex);
  HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=listtrials>");
  HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", port);
  HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", address);
  HTML("<INPUT NAME=key     TYPE=HIDDEN VALUE=%s>", key);
  HTML("<td>Local Trial List");
  HTML("<td>Trial ID: <INPUT NAME=trialmatch TYPE=Text VALUE=>");
  HTML("<td>");
  HTML("<td>");
  HTML("<td><INPUT TYPE=SUBMIT VALUE=Submit>");
  HTML("</FORM>");
  HTML("</tr>");	
end

HTML("</table>");

HTML("<FORM ACTION=\"dgate%s\" METHOD=POST ENCTYPE=\"multipart/form-data\">", ex);
HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=addlocalfile>");
HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", port);
HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", address);
HTML("Upload file to enter into server (dcm/v2/HL7/zip/7z/gz/tar): <INPUT NAME=filetoupload SIZE=40 TYPE=file VALUE=>");
HTML("<INPUT TYPE=SUBMIT VALUE=Go>");
HTML("</FORM>");

HTML("</BODY>")
