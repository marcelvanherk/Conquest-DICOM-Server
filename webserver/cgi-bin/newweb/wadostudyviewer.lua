-- 20151026   mvh   Created; based on nh_queryseries for table colors etc
-- 20151027   mvh   Firefox compat
-- 20151206   mvh   fix 1.4.17d compatibility
-- 20160124   mvh   Added anonymize option; passed to all WADO text display and heading of table
-- 20160319   mvh   Small fix in slope default
-- 20160717   mvh   Fix in level and window intercept; use central slice to get info
-- 20170305   mvh   Added hint to save, fix zoom
-- 20170430   mvh   Zoom no longer checks on version; assumes 1.4.19 plus
-- 20180204   mvh   Removed 'getting image' print
-- 20181223   mvh   Use remotequery (1.4.19d)
-- 20180112   mvh   Fix to allow : in patientID
-- 20200113   mvh   Allow 0 records
-- 20220830   mvh   Renamed dropdown to slicelister; sync its position with slice
-- 20230625   mvh   Made all links relative

-- defaults to allow debugging in zbs
study2 = study2 or 'EG2005:'
version = version or ''
size = size or 560
 
-- split query information
local patid = string.gsub(study2, ':[^:]-$', '')
local studyuid = string.gsub(study2, '^.*:', '')

------------------------------------------------------------------------
-- support functions 
------------------------------------------------------------------------

-- split string into pieces, return as table
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

-- report error string on a otherwise empty reddish page
function errorpage(s)
  HTML('Content-type: text/html\n\n');
  print(
  [[
  <HEAD><TITLE>Version ]].. (version or '-')..[[ WADO viewer</TITLE></HEAD>
  <BODY BGCOLOR='FFDFCF'>
  <H3>Conquest ]].. (version or '-') ..[[ WADO viewer</H3>
  <H3>]].. s .. [[</H3>
  </BODY>
  ]])
end

function remotequery(ae, level, q)
  local remotecode =
[[
  local ae=']]..ae..[[';
  local level=']]..level..[[';
  local q=]]..q:Serialize()..[[;
  local q2=DicomObject:new(); for k,v in pairs(q) do q2[k]=v end;
  local r = dicomquery(ae, level, q2):Serialize();
  local s=tempfile('txt') local f=io.open(s, "wb") f:write(r) returnfile=s f:close();
]]
  local f = loadstring('return '..servercommand('lua:'..remotecode));
  if f then return f() end
end

-- get list of series
function queryseries(source)
  local patis, b, i, j, k, l;
 
  b=newdicomobject();
  b.QueryRetrieveLevel='SERIES'
  b.PatientName = ''
  b.PatientID        = patid
  b.StudyDate        = ''; 
  b.StudyInstanceUID = studyuid;
  b.StudyDescription = '';
  b.SeriesDescription= '';
  b.SeriesInstanceUID= '';
  b.SeriesDate= '';
  b.Modality         = '';
  b.SeriesTime       = '';
  b.NumberOfSeriesRelatedInstances = '';
  series=remotequery(source, 'SERIES', b);
  if series==nil then
   errorpage('Wado viewer error, cannot query server at series level - check ACRNEMA.MAP for: ' .. source)
   return
  end

  -- convert returned DDO (userdata) to table; needed to allow table.sort
  --seriest={}
  --for k1=0,#series-1 do
  --  seriest[k1+1]={}
  --  seriest[k1+1].PatientName      = series[k1].PatientName or ''
  --  seriest[k1+1].StudyDate        = series[k1].StudyDate or ''
  --  seriest[k1+1].PatientID        = series[k1].PatientID or ''
  --  seriest[k1+1].StudyDate        = series[k1].StudyDate or ''
  --  seriest[k1+1].SeriesTime       = series[k1].SeriesTime or ''
  --  seriest[k1+1].SeriesDate       = series[k1].SeriesDate or ''
  --  seriest[k1+1].StudyInstanceUID = series[k1].StudyInstanceUID or ''
  --  seriest[k1+1].SeriesDescription= series[k1].SeriesDescription or ''
  --  seriest[k1+1].StudyDescription = series[k1].StudyDescription or ''
  --  seriest[k1+1].SeriesInstanceUID= series[k1].SeriesInstanceUID or ''
  --  seriest[k1+1].Modality         = series[k1].Modality or ''
  --  seriest[k1+1].NumberOfSeriesRelatedInstances = series[k1].NumberOfSeriesRelatedInstances or 0
  --end
  seriest = series
  return seriest
end

-- get list of images (called for each series)
function queryimages(source, patid, seriesuid)
  local images,imaget,b;
  
  b=newdicomobject();
  b.PatientID        = patid
  b.SeriesInstanceUID= seriesuid
  b.InstanceNumber   = ''
  b.SOPInstanceUID   = ''
  b.SliceLocation   = ''
  b.ImageComments   = ''
  b.NumberOfFrames = ''

  images=remotequery(source, 'IMAGE', b);
  if images==nil then
   errorpage('Wado viewer error, cannot query server at image level: ' .. source)
   return
  end

  imaget={}
  for k=1,#images do
    if (images[k].InstanceNumber or '') == '' then
      images[k].InstanceNumber = '0'
    end
    imaget[k]={}
    imaget[k].SOPInstanceUID=images[k].SOPInstanceUID
    imaget[k].SliceLocation=images[k].SliceLocation or ''
    imaget[k].ImageComments=images[k].ImageComments or ''
    imaget[k].NumberOfFrames=images[k].NumberOfFrames or 1
    imaget[k].InstanceNumber= tonumber(images[k].InstanceNumber or '0')
  end
  table.sort(imaget, function(a,b) return a.InstanceNumber<b.InstanceNumber end)

  return imaget
end

------------------------------------------------------------------------
-- Start main code
------------------------------------------------------------------------

-- get server version, displayed and used to block buggy zoom function
local serverversion = servercommand('display_status:');
if serverversion==nil then
  errorpage('the server is not running')
  return
end
serverversion = string.match(serverversion, 'version ([%d%.%a]*)');
local viewer=gpps('webdefaults', 'viewer', '');
local source = servercommand('get_param:MyACRNema')

anonymizer = ''
if CGI('anonymize')~='' then
  anonymizer = '&anonymize='..CGI('anonymize')
end

---------------------------------------------------------------------------------
-- generate HTML page
---------------------------------------------------------------------------------

HTML("Content-type: text/html\nCache-Control: no-cache\n");

print(
[[
<html>
<head>
<title>Wado study viewer - version]]..version..[[</title>]]..
[[<style type="text/css">
body { font:10pt Verdana; }
a { color:blue; }
#content { background-color:#dddddd; width:200px; margin-top:2px; }
</style>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
]]
)

print([[
<script>
var size = ]]..size..[[;
var serverversion = ']]..serverversion..[[';
var anonymizer = ']]..(anonymizer or '')..[[';
]])

-- here is all the js code defined; long string is rendered line by line below
local js_code=
[[
// variable block
var modality = '';
var seriesuid = '';
var nseries= 1;
var defwindowcenter;
var defwindowwidth;
var description = '';
var studyuid = '';
var seriesno=1;
var bridge = '';
var nframes = 1;
var region = '';
var zoom = 1;
var panx=0.5;
var pany=0.5;
var slice=0;
var nslices=0;

var windowcenter = defwindowcenter;
var windowwidth = defwindowwidth;
var frame = 0;
var inter;
var inputactive=0;

function altRows(id){
	if(document.getElementsByTagName){  
		
		var table = document.getElementById(id);  
		var rows = table.getElementsByTagName("tr"); 
		 
		for(i = 0; i < rows.length; i++){          
			if(i % 2 == 0){
				rows[i].className = "evenrowcolor";
			}else{
				rows[i].className = "oddrowcolor";
			}      
		}
	}
}
window.onload=function(){
	altRows('alternatecolor');
}

// loads a series and makes relevant slicing form visible
function loadseries()
{ for (var i=1; i<=nseries; i++)
    document.getElementById("form"+i).style.display = "none";
  for (var i=1; i<=nseries; i++)
    document.getElementById("A"+i).innerHTML = "View";

  setInterval(savesettings, 500);
  nframes = Number(document.getElementById("form"+seriesno).slicelister.value.split("|")[1]);
  nslices = document.getElementById("form"+seriesno).slicelister.length;
  document.getElementById("form"+seriesno).style.display = "block";
  document.getElementById("A"+seriesno).innerHTML = "---->";
  slice = Math.floor(nslices/2);
  defwindowcenter = Number(document.getElementById("defwindowcenter"+seriesno).value);
  defwindowwidth = Number(document.getElementById("defwindowwidth"+seriesno).value);
  windowcenter = defwindowcenter;
  windowwidth = defwindowwidth;
  loadsettings();
}

// generic cookie functions
function getCookie(c_name)
{ var i,x,y,ARRcookies=document.cookie.split(";");
  for (i=0;i<ARRcookies.length;i++)
  { x=ARRcookies[i].substr(0,ARRcookies[i].indexOf("="));
    y=ARRcookies[i].substr(ARRcookies[i].indexOf("=")+1);
    x=x.replace(/^\s+|\s+$/g,"");
    if (x==c_name)
    { return unescape(y);
    }
  }
}

function setCookie(c_name,value,exdays)
{ var exdate=new Date();
  exdate.setDate(exdate.getDate() + exdays);
  var c_value=escape(value) + ((exdays==null) ? "" : "; expires="+exdate.toUTCString());
  document.cookie=c_name + "=" + c_value;
}

// load and save settings in a cookie
function savesettings()
{ if (document.getElementById("autosave").checked)
    setCookie("wadoseriesviewer_autosave", "true");
  else  
    setCookie("wadoseriesviewer_autosave", "false");
  if (document.getElementById("autosave").checked==false) return;
  setCookie('wadoseriesviewer_'+seriesuid,zoom+'|'+panx+'|'+pany+'|'+windowcenter+'|'+
            windowwidth+'|'+frame+'|'+slice);
}

function loadsettings()
{ var v = getCookie('wadoseriesviewer_'+seriesuid);
  if (v!=null && document.getElementById("autosave").checked)
  { zoom = Number(v.split("|")[0]);
    panx = Number(v.split("|")[1]);
    pany = Number(v.split("|")[2]);
    windowcenter = Number(v.split("|")[3]);
    windowwidth = Number(v.split("|")[4]);
    frame = Number(v.split("|")[5]);
    slice = Number(v.split("|")[6]);
  }
  leveler(0,1);
  framer(0);
  slicer(0);
  zoomer(1,0,0);
  load();
}

// load the DICOM object as image or text
function load()
{ if (Number(document.getElementById("form"+seriesno).slicelister.value.split("|")[1])!=nframes)
  { nframes = Number(document.getElementById("form"+seriesno).slicelister.value.split("|")[1]);
    frame = 0;
  }
  
  var showText = (modality.substring(0, 2)=='RT' && modality!='RTDOSE' && modality!='RTIMAGE') || modality=='SR';
  if (!showText)
  { document.images[0].src = '?requestType=WADO&contentType=image/jpeg'+
    bridge +
    '&studyUID='+studyuid +
    '&seriesUID='+seriesuid +
    '&windowCenter='+windowcenter +
    '&windowWidth='+windowwidth +
    '&frameNumber='+frame +
    '&region='+region +
    '&objectUID=' + document.getElementById("form"+seriesno).slicelister.value.split("|")[0];
    document.addEventListener("keydown", myKeyFunction, false);
    document.getElementById("myframe").style.display='none';
    document.images[0].style.display='block';
  }
  else  
  { document.getElementById("myframe").src   = '?requestType=WADO&contentType=text/plain'+
    bridge+
    '&studyUID='+studyuid +
    '&seriesUID='+seriesuid +
    anonymizer +
    '&objectUID=' + document.getElementById("form"+seriesno).slicelister.value.split("|")[0];
    document.getElementById("myframe").style.display='block';
    document.images[0].style.display='none';
  } 
}

// browser popup (used to display header info)
function PopupCenter(pageURL, title,w,h)
{ var left = (screen.width/2)-(w/2);
  var top = (screen.height/2)-(h/2);
  var targetWin = 
  window.open (pageURL, title, 'toolbar=no, location=no, directories=no, status=no, menubar=no, scrollbars=yes, resizable=yes, copyhistory=no, width='+w+', height='+h+', top='+top+', left='+left);
}

// level window change/display
function leveler(a, b)
{ if (b==0 && a==1)
  { windowwidth = defwindowwidth;
    windowcenter = defwindowcenter;
  }
  else if (b==0 && a==2)
  { windowwidth = 0;
    windowcenter = 0;
  }
  else if (a!=0 || b!=1)
  { if (windowwidth==0)
    { windowwidth = defwindowwidth;
      windowcenter = defwindowcenter;
    }
    windowcenter = windowcenter + a*defwindowwidth;
    windowwidth = windowwidth * b;
  }
  if (inputactive!=2)
    document.getElementById("level").innerHTML = "level: "+Math.round(windowcenter*100)/100;
  else
    document.getElementById("level").innerHTML = "<b>level: "+Math.round(windowcenter*100)/100+"</b>";
  if (inputactive!=3)
    document.getElementById("window").innerHTML = "window: "+Math.round(windowwidth*100)/100;
  else
    document.getElementById("window").innerHTML = "<b>window: "+Math.round(windowwidth*100)/100+"</b>";
  if (a!=0 || b!=1)
    load();
}

// frame change/display
function framer(a)
{ frame = frame + a;
  if (frame>=nframes) frame = nframes-1;
  if (frame<0)        frame = 0;
  if (nframes>1)
  { if (inputactive!=5)
      document.getElementById("frame").innerHTML = "frame: "+frame+" of "+nframes;
    else
      document.getElementById("frame").innerHTML = "<b>frame: "+frame+" of "+nframes+"</b>";
  }
  else
    document.getElementById("frame").innerHTML = "";
  if (a!=0)
    load();
}

// zoom / pan change/display
function zoomer(z, px, py)
{ // if (serverversion!='1.4.19' && serverversion<'1.4.19b') return;
  zoom = z*zoom;
  panx = panx + px/zoom;
  pany = pany + py/zoom;
  if (zoom<=1)
  { zoom=1;
    panx=0.5;
    pany=0.5;
    region="";
  }
  else
  { var xl, xh, yl, yh;
    if (zoom>50) zoom = 50;
    xl = panx - 0.5/zoom; if(xl<0)      {xl=0;     panx=0.5/zoom; }
    xh = panx + 0.5/zoom; if(xh>0.999)  {xh=0.999; panx=xh-0.5/zoom; }
    yl = pany - 0.5/zoom; if(yl<0)      {yl=0;     pany=0.5/zoom; }
    yh = pany + 0.5/zoom; if(yh>=0.999) {yh=0.999; pany=1-0.5/zoom; }
    region = String(xl)+","+String(xh)+","+String(yl)+","+String(yh);
  }
  if (z!=1 || px!=0 || py!=0)
    load();
  if (inputactive!=1)
    document.getElementById("zoom").innerHTML = "zoom: "+Math.round(zoom*100)/100;
  else
    document.getElementById("zoom").innerHTML = "<b>zoom: "+Math.round(zoom*100)/100+"</b>";
}

// keyboard event
function myKeyFunction(a) 
{ a = window.event || a;
  var ch = String.fromCharCode(a.keyCode || a.charCode)
  if      (ch=='B') leveler(-0.05, 1);
  else if (ch=='D') leveler(+0.05, 1);
  else if (ch=='L') leveler(0, 1.2);
  else if (ch=='H') leveler(0, 1/1.2);
  else if (ch=='P') leveler(1, 0);
  else if (ch=='A') leveler(2, 0);
  else if (ch=='N') leveler(0, -1);

  else if (ch=='I') zoomer(1.2, 0, 0);
  else if (ch=='O') zoomer(1/1.2, 0, 0);
  else if (ch=='F') zoomer(0, 0, 0);
  else if (ch=='U') framer(1);
  else if (ch=='V') framer(-1);
  else if (a.keyCode==37) zoomer(1, 0.03, 0);
  else if (a.keyCode==39) zoomer(1, -0.03, 0);
  else if (a.keyCode==38) zoomer(1, 0, 0.03);
  else if (a.keyCode==40) zoomer(1, 0, -0.03);

  else if (a.keyCode==33) slicer(1);
  else if (a.keyCode==34) slicer(-1);
  else if (ch=='Q')
    PopupCenter('?requestType=WADO'+bridge+'&contentType=text/plain&studyUID='+studyuid+'&seriesUID='
    +seriesuid+anonymizer+'&objectUID=' + document.getElementById("form"+seriesno).slicelister.value.split("|")[0], 'hoi', 700, 512);
}

// step through slices
function slicer(delt)
{ if (nslices==1)
  { document.getElementById("slice").innerHTML = "";
    framer(delt);
    return;
  }
  if (delt>0)
  { if (slice >= nslices-delt) 
      slice = nslices-1; 
    else 
      slice = slice + delt;
  }
  else  
  { if (slice < -delt) 
      slice = 0; 
    else 
      slice = slice + delt;
  }
  document.getElementById("form"+seriesno).slicelister.selectedIndex = slice;
  if (delt!=0)
    load()
  if (inputactive!=4 && inputactive!=6 && inputactive!=8)
    document.getElementById("slice").innerHTML = "slice "+(slice+1);
  else
    document.getElementById("slice").innerHTML = "<b>slice "+(slice+1)+"</b>";
}

// highlight labels when mouse over and set their events
function active(n)
{ var hint='';
  inputactive=n;
  leveler(0,1);framer(0);slicer(0);zoomer(1,0,0);
  document.getElementById("slice").addEventListener("mousewheel", wheeler, false);
  document.getElementById("slice").addEventListener("DOMMouseScroll", wheeler, false);
  document.getElementById("zoom").addEventListener("mousewheel", wheeler, false);
  document.getElementById("zoom").addEventListener("DOMMouseScroll", wheeler, false);
  document.getElementById("level").addEventListener("mousewheel", wheeler, false);
  document.getElementById("level").addEventListener("DOMMouseScroll", wheeler, false);
  document.getElementById("window").addEventListener("mousewheel", wheeler, false);
  document.getElementById("window").addEventListener("DOMMouseScroll", wheeler, false);
  document.getElementById("frame").addEventListener("mousewheel", wheeler, false);
  document.getElementById("frame").addEventListener("DOMMouseScroll", wheeler, false);
  document.getElementById("image").addEventListener("mousewheel", wheeler, false);
  document.getElementById("image").addEventListener("DOMMouseScroll", wheeler, false);
  document.getElementById("slice").addEventListener("mouseup", clicker);
  document.getElementById("zoom").addEventListener("mouseup", clicker);
  document.getElementById("level").addEventListener("mouseup", clicker);
  document.getElementById("window").addEventListener("mouseup", clicker);
  document.getElementById("frame").addEventListener("mouseup", clicker);
  if (inputactive==1) hint = 'Wheel=zoom; Key I/O=zoom In/Out; Arrows=pan; F=Fit';
  if (inputactive==2) hint = 'Wheel=brightness; Key B/D=Brigher/Darker; H/L=Higher/Lower contrast';
  if (inputactive==3) hint = 'Wheel=contrast; Key A=Auto level/window; P=Preset level/window';
  if (inputactive==4) hint = 'Wheel=slice(default); Key page up/page down = slice';
  if (inputactive==5) hint = 'Wheel=frame; Key U/V=frame Up/down';
  if (inputactive==8) hint = 'Wheel=slice(default); Key page up/page down = slice';
  if (inputactive==9) hint = 'If selected, view settings are stored in cookies';
  if (hint!='') hint = "Hint: "+hint;
  document.getElementById("hint").innerHTML = hint;
}

// onclick sets numerical value for items
function clicker()
{ var a;
  if (inputactive==4) { a=Number(prompt("Slice number:",slice+1)); if (a) slice=a-1; slicer(0); load(); }
  if (inputactive==3) { a=Number(prompt("Window width:",windowwidth)); if (a) windowwidth=a; leveler(0, 1); load(); }
  if (inputactive==2) { a=Number(prompt("Window center:",windowcenter)); if (a) windowcenter=a; leveler(0, 1); load(); }
  if (inputactive==1) { a=Number(prompt("Zoom:",zoom)); if (a) zoom=a; zoomer(1,0,0); load(); }
  if (inputactive==5) { a=Number(prompt("Frame number:",frame)); if (a) frame=a; framer(0); load(); }
}

// wheel change of items
function wheeler(a)
{ a = window.event || a;
  var delta = Math.max(-1, Math.min(1, (a.wheelDelta || -a.detail)));
  if (inputactive==1) { if (delta>0) zoomer(1.1, 0, 0); else zoomer(1/1.1, 0, 0); return false;}
  if (inputactive==2) { if (delta>0) leveler(0.01, 1); else leveler(-0.01, 1); return false;}
  if (inputactive==3) { if (delta>0) leveler(0, 1.05); else leveler(0, 1/1.05); return false;}
  if (inputactive==4) { if (delta>0) slicer(1); else slicer(-1); return false;}
  if (inputactive==5) { if (delta>0) framer(1); else framer(-1); return false;}
  if (inputactive==6) { if (delta>0) slicer(1); else slicer(-1); return false;}
  if (inputactive==8) { if (delta>0) slicer(1); else slicer(-1); return false;}
}
</script>
]]
js_code = split(js_code, '\n')
for k,v in ipairs(js_code) do print(v) end

print([[
<!-- CSS goes in the document HEAD or added to your external stylesheet -->
<style type="text/css">
table.altrowstable {
	font-family: verdana,arial,sans-serif;
	font-size:16px;
	color:#333333;
	border-width: 1px;
	border-color: #a9c6c9;
	border-collapse: collapse;
}
table.altrowstable th {
	border-width: 1px;
	padding: 8px;
	border-style: solid;
	border-color: #a9c6c9;
}
table.altrowstable td {
	border-width: 1px;
	padding: 8px;
	min-width: 20px;
	max-width: 180px;
	border-style: solid;
	border-color: #a9c6c9;
}
.oddrowcolor{
	background-color:#d4e3e5;
}
.evenrowcolor{
	background-color:#c3dde0;
}
table.altrowstable tr:first-child {
	font-weight:bold;
}
table.altrowstable Caption {
    font-weight:bold;
    color: yellow;
    background: green;
}
</style>
</head> 
<body BGCOLOR='CFDFCF'>
]])

print(string.format("<H2>Conquest WADO study viewer - version %s</H2>", version))

-- render table with available series

local pats=queryseries(source) 

if anonymizer~='' then
  pats[1].PatientID   = string.sub(anonymizer, 12, 99)
  pats[1].PatientName = ''
end

print("<div style='position: absolute; left: 10px; width: 550px'>")
print("<table class='altrowstable' id='alternatecolor' RULES=ALL BORDER=1>");
print(string.format("<Caption>List of series on local server for ID=%s, name=%s</caption>", (pats[1] or {PatientID=''}).PatientID, (pats[1] or {PatientName=''}).PatientName));
print("<TR><TD>Date<TD>Time<TD>Description<TD>Modality</TR>");
	
for i=1,#pats do
  u = string.format([[<A id=A]]..i..[[ onclick="seriesno=%d;seriesuid='%s';studyuid='%s';modality='%s';loadseries();">View</A>]], 
      i, pats[i].SeriesInstanceUID, pats[i].StudyInstanceUID, pats[i].Modality); 
  s = string.format("<TR><TD>%s<TD>%s<TD>%s<TD>%s<br>(%d)<TD>%s</TR>",pats[i].SeriesDate, pats[i].SeriesTime,
  pats[i].SeriesDescription,pats[i].Modality, 0+pats[i].NumberOfSeriesRelatedInstances,u);
  print(s)
end 

print('</table>')
print("</div>");

-- generate window for displaying image and interaction controls
print[[

<div style='position: absolute; top: 40px; left: 620px' id=lowerpage>
<table style="width: 560px"><tr>
<td onmouseover="active(1)" id=zoom></td>
<td onmouseover="active(2)" id=level></td>
<td onmouseover="active(3)" id=window></td>
<td onmouseover="active(4)" id=slice></td>
<td onmouseover="active(5)" id=frame></td>
<td onmouseover="active(9)"><INPUT TYPE=CHECKBOX VALUE=0 id=autosave onclick="loadsettings();savesettings();">Auto save</td>
</tr></table>

<IMG onmouseover="active(6)" id=image BORDER=1 HEIGHT=560>
<iframe onmouseover="active(7)" id=myframe BORDER=0 WIDTH=560 HEIGHT=560 style='display: none'>
<p>Your browser does not support iframes.</p>
</iframe>
]]

-- generate forms with slicing control for each series; display only one at the time
for i=1,#pats do
  images = queryimages(source, patid, pats[i].SeriesInstanceUID)
  seriesuid = pats[i].SeriesInstanceUID

  -- this notation is accepted in the server and will also access virtual servers
  local imagelocation = studyuid..'\\\\'..seriesuid..'\\\\'..images[math.ceil(#images/2)].SOPInstanceUID

  -- get info about slice from server side script
  windowcenter, windowwidth, slope, intercept, rows, columns = 
  unpack(split(servercommand('lua:'..
  [[
  local a=newdicomobject(); 
  a:Read("]]..imagelocation..[[");
  local c = (a.WindowCenter or 1000)  if c=='' then c=1000 end
  local w = (a.WindowWidth or 1000)   if w=='' then w=1000 end
  local s = (a.RescaleSlope or 1)     if s=='' then s=1 end
  local i = (a.RescaleIntercept or 0) if i=='' then i=0 end
  local x = (a.Columns or 0) if i=='' then i=0 end
  local y = (a.Rows or 0)    if i=='' then i=0 end
  --print('getting image for wado viewer', c, w)
  return c.."|"..w.."|"..s.."|"..i.."|"..x.."|"..y
  ]]
  ) or '0|0|1|0|512|512', '|'))

  -- process level and window information: may be multiple
  if string.find(windowcenter, '%\\') then
    windowcenter = unpack(split(windowcenter, '\\'))
  end

  if string.find(windowwidth, '%\\') then
    windowwidth = unpack(split(windowwidth, '\\'))
  end

  -- todo: fully scale level and window according to slope and intercept
  if pats[i].Modality=='CT' then
    windowcenter = windowcenter+0 -- default for CT in conquest
  end

  -- generate form with slicing controls
  print([[
  <FORM onmouseover="active(8)" style="display: none" id=form]]..i..[[>
  Slice:
  <INPUT TYPE=BUTTON VALUE='<' onclick=slicer(-1) onmousedown="inter=setInterval('slicer(-1)', 100)" onmouseup=clearInterval(inter) onmouseout=clearInterval(inter)>
  <select name=slicelister onchange=slice=document.getElementById("form"+seriesno).slicelister.selectedIndex;slicer(0)>
  ]])

  for i=1, math.min(#images, 2000) do
    print('<option value='..images[i].SOPInstanceUID..'|'..images[i].NumberOfFrames..'>'..string.format('%d (%.1f) %s', i, tonumber(images[i].SliceLocation) or 0, images[i].ImageComments or '') .. '</option>')
  end

  if #images>2000 then
    print('<option value='..images[#images].SOPInstanceUID..'> list truncated </option>')
  end

  print([[
    </select>
    <INPUT TYPE=BUTTON VALUE='>' onclick=slicer(1) onmousedown="inter=setInterval('slicer(1)', 100)" onmouseup=clearInterval(inter) onmouseout=clearInterval(inter)> Total slices: ]].. #images .. [[
    <INPUT TYPE=HIDDEN VALUE=]]..windowcenter..[[ id=defwindowcenter]]..i..[[>
    <INPUT TYPE=HIDDEN VALUE=]]..windowwidth..[[ id=defwindowwidth]]..i..[[>

    <a href=# onclick="javascript:PopupCenter('?requestType=WADO'+bridge+'&contentType=text/plain&studyUID='+studyuid+'&seriesUID='+seriesuid+anonymizer+'&objectUID='+document.getElementById('form'+seriesno).slicelister.value.split('|')[0], 'hoi', 700, 512)">[show header]</a>
    <a href=# onclick="javascript:PopupCenter('?mode=wadoviewerhelp', 'hoi', 700, 512)">[help]</a>
    </FORM>
  ]])
end

-- info to autoload first series for display
local info = string.format("seriesuid='%s';studyuid='%s';modality='%s';nseries=%d;",
  (pats[1] or {SeriesInstanceUID=''}).SeriesInstanceUID, 
  (pats[1] or {StudyInstanceUID=''}).StudyInstanceUID, 
  (pats[1] or {Modality=''}).Modality, #pats)

print([[
<i id=hint></i>
</div>

<SCRIPT language=JavaScript>
  document.getElementById("autosave").checked = getCookie("wadoseriesviewer_autosave")=='true';
  document.onload=setInterval(savesettings, 500);]]..info..[[loadseries();
</SCRIPT>

</body>
</html>
]])
