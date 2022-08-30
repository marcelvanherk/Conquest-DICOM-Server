-- 20120302   mvh   Created
-- 20151015   mvh   WIP mouse wheel and more interface
-- 20151018   mvh   Cleanup, added default level and window; added error handling
-- 20151018   mvh   Note: print has a hardcoded limit at 2000 chars, avoid really long strings!
-- 20151019   mvh   Added keyboard interface
-- 20151020   mvh   Zoom and pan working
-- 20151021   mvh   Cleanup zoom and pan; show level/zoom etc info in table, frame control
-- 20151023   mvh   Some fixes, introduce slicer(0) and use xxxer functions to display info 
-- 20151024   mvh   make zoom..frame labels mousewheel active and highlighting, layout, help
-- 20151024   mvh   Added hints; click labels to set numerical value; added autosave function
--            mvh   Note: uses cookie with seriesuid and wadoseriesviewer_autosave
-- 20151025   mvh   Unified js code with wadostudyviewer; note loadseries() not used
-- 20151027   mvh   Use series2 to enable processing of special characters
-- 20151027   mvh   Firefox compat
-- 20151206   mvh   fix 1.4.17d compatibility
-- 20160319   mvh   Small fix in slope default
-- 20160717   mvh   Fix in level and window intercept; use central slice to get info
-- 20170305   mvh   Added hint to save, fix zoom
-- 20170430   mvh   Zoom no longer checks on version; assumes 1.4.19 plus
-- 20180204   mvh   Removed 'getting image' print
-- 20181128   mvh   Added anonymizer also to series viewer
-- 20181223   mvh   Use remotequery (1.4.19d)
-- 20180112   mvh   Fix to allow : in patientID
-- 20220830   mvh   Renamed dropdown to slicelister; sync its position with slice
--                  Disable altrows; no table!

-- default series information - this is here to allow debugging of the code usign ZeroBrane Studio, running from the server
series2 = series2 or '0009703828:1.3.46.670589.5.2.10.2156913941.892665339.860724'
-- series2 = series2 or '1082100772_3727835028:1.2.826.0.1.3680043.2.135.733552.44047153.7.1252971725.189.61'

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

-- get some extra information at series and study level
function getseriesinfo(source, patid, seriesuid)
  local b;
  b=newdicomobject();
  b.PatientID = patid
  b.SeriesInstanceUID = seriesuid
  b.StudyInstanceUID = '';
  b.Modality = '';
  b.SeriesDescription = '';
  b.SeriesDate = '';
  b.SeriesTime = '';
  a=remotequery(source, 'SERIES', b);
  if a==nil then
   errorpage('Wado viewer error, cannot query server at series level - check ACRNEMA.MAP for: ' .. source)
   return
  end
  
  b=newdicomobject();
  b.PatientID = patid
  b.StudyInstanceUID = a[1].StudyInstanceUID;
  b.PatientName = '';
  b.PatientSex = '';
  b.PatientBirthDate = '';
  c=remotequery(source, 'STUDY', b);
  if c==nil then
   errorpage('Wado viewer error, cannot query server at image level: ' .. source)
   return
  end
  
  return a[1], c[1]
end

-- get list of images (called once)
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

-- get local server AE title from dicom.ini at server side
local source
if CGI('source', '(local)')=='(local)' then
  source = servercommand('get_param:MyACRNema')
  if source==nil then 
    errorpage('server not running: '.. CGI('source')) 
    return
  end
else
  source = CGI('source')
end

local bridge=''
if CGI('source', '(local)')~='(local)' then
	bridge='&bridge='..CGI('source')
end

-- get server version, displayed and used to block buggy zoom function
local serverversion = servercommand('display_status:');
if serverversion==nil then
  errorpage('the server is not running')
  return
end
serverversion = string.match(serverversion, 'version ([%d%.%a]*)');

anonymizer = ''
if CGI('anonymize')~='' then
  anonymizer = '&anonymize='..CGI('anonymize')
end
  
  -- collect all DICOM information needed
local patid = string.gsub(series2, ':[^:]-$', '')
local seriesuid = string.gsub(series2, '^.*:', '')
local seriesinfo, patinfo = getseriesinfo(source, patid, seriesuid)
local studyuid = seriesinfo.StudyInstanceUID
local modality = seriesinfo.Modality
local description = seriesinfo.SeriesDescription
local date = seriesinfo.SeriesDate
local time = seriesinfo.SeriesTime
local images = queryimages(source, patid, seriesuid);

-- size of image display - comes from cgi-bin/dicom.ini
local size = 0+CGI('size', '512')

-- some error handling
if studyuid==nil then
  errorpage('wado viewer error: unknown series passed')
  return
end

if #images==0 then
  errorpage('wado viewer error: no images found')
  return
end

local a = newdicomobject()
a.InstanceNumber = 'test'
if a.InstanceNumber ~= 'test' then
  errorpage('wado viewer error: old dgate.dic used')
  return
end

---------------------------------------------------------------------------------
-- get some information out of an arbitrary slice at the server side
-- running a short lua script on the server
-- this is needed because not all information can be queried
-- and the CGI interface has no direct access to DICOM slices
---------------------------------------------------------------------------------

-- this notation is accepted in the server and will also access virtual servers
local imagelocation = studyuid..'\\\\'..seriesuid..'\\\\'..images[math.ceil(#images/2)].SOPInstanceUID

windowcenter, windowwidth, slope, intercept = 
  unpack(split(servercommand('lua:'..
  [[
  local a=newdicomobject(); 
  a:Read("]]..imagelocation..[[");
  local c = (a.WindowCenter or 1000)  if c=='' then c=1000 end
  local w = (a.WindowWidth or 1000)   if w=='' then w=1000 end
  local s = (a.RescaleSlope or 1)     if s=='' then s=1 end
  local i = (a.RescaleIntercept or 0) if i=='' then i=0 end
  --print('getting image for wado viewer', c, w)
  return c.."|"..w.."|"..s.."|"..i
  ]]
  ) or '0|0|1|0s', '|'))

-- process level and window information: may be multiple
if string.find(windowcenter, '%\\') then
  windowcenter = unpack(split(windowcenter, '\\'))
end

if string.find(windowwidth, '%\\') then
  windowwidth = unpack(split(windowwidth, '\\'))
end

-- todo: fully scale level and window according to slope and intercept
if modality=='CT' then
  windowcenter = windowcenter+0 -- default for CT in conquest
end

---------------------------------------------------------------------------------
-- generate HTML page
---------------------------------------------------------------------------------

HTML('Content-type: text/html\n\n');
print([[
<HEAD><TITLE>Version ]].. (version or '-')..[[ WADO viewer</TITLE></HEAD>
<script>
]])

print([[
var bridge = ']]..bridge..[[';
var size = ]]..size..[[;
var serverversion = ']]..serverversion..[[';
var script_name = ']]..(script_name or '')..[[';
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
	//altRows('alternatecolor');
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
  { document.images[0].src = script_name+'?requestType=WADO&contentType=image/jpeg'+
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
  { document.getElementById("myframe").src   =  script_name+'?requestType=WADO&contentType=text/plain'+
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
    PopupCenter(script_name+'?requestType=WADO'+bridge+'&contentType=text/plain&studyUID='+studyuid+'&seriesUID='
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

-- bridge Lua to javascript with this block of variables 
print([[
<script>
defwindowcenter = ]]..windowcenter..[[;
defwindowwidth = ]]..windowwidth..[[;
windowcenter = defwindowcenter;
windowwidth = defwindowwidth;
modality = ']]..modality..[[';
description = ']]..description..[[';
studyuid = ']]..studyuid..[[';
seriesuid = ']]..seriesuid..[[';
</script>
]])

print([[
</script>
<BODY BGCOLOR='CFDFCF'>
<i onmouseover="active(0)">Conquest ]].. (version or '-') ..'/' .. serverversion ..[[ WADO viewer</i>
]])

-- generate text string with patient and series information
if time~='' then
  time = math.floor(time+0.5);
end
print([[
<br><b onmouseover="active(0)">]].. string.format('%s (ID %s; gender %s; born %s) %s %s', patinfo.PatientName, patinfo.PatientID, patinfo.PatientSex, patinfo.PatientBirthDate,  modality, description) ..[[</b>
]])

-- display placeholders for info items
print('<table style="width=560"><tr>')
print(string.format('<td>date %s</td><td>time %s</td>', date, time))
print('<td onmouseover="active(1)" id=zoom></td>')
print('<td onmouseover="active(2)" id=level></td>')
print('<td onmouseover="active(3)" id=window></td>')
print('<td onmouseover="active(4)" id=slice></td>')
print('<td onmouseover="active(5)" id=frame></td>')
print('</tr></table>')

-- generate elements to display information: image (IMG) and text (iframe)
print([[
<iframe onmouseover="active(7)" id="myframe" style="display: none" BORDER=0 WIDTH=]]..size..[[ HEIGHT=]]..size..[[>
<p>Your browser does not support iframes.</p>
</iframe>
]])
print([[
<IMG onmouseover="active(6)" id=image BORDER=1 HEIGHT=]]..size..[[>
]])

-- generate form with slicing controls
print([[
<FORM onmouseover="active(8)" id=form1>
Slice:
  <INPUT TYPE=BUTTON VALUE='<' onclick=slicer(-1) onmousedown="inter=setInterval('slicer(-1)', 100)" onmouseup=clearInterval(inter) onmouseout=clearInterval(inter)>
  <INPUT TYPE=HIDDEN VALUE=]]..windowcenter..[[ id=defwindowcenter1>
  <INPUT TYPE=HIDDEN VALUE=]]..windowwidth..[[ id=defwindowwidth1>
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
  <INPUT TYPE=CHECKBOX VALUE=0 id=autosave onmouseover="active(9)" onclick="loadsettings();savesettings();">Auto save

  <a href=# onclick="javascript:PopupCenter(script_name+'?requestType=WADO'+bridge+'&contentType=text/plain&studyUID='+studyuid+'&seriesUID='+seriesuid+'&objectUID=' + document.forms[0].slicelister.value.split('|')[0], 'hoi', 700, 512)">[show header]</a>
  <a href=# onclick="javascript:PopupCenter(script_name+'?mode=wadoviewerhelp', 'hoi', 700, 512)">[help]</a>
  <br><i id=hint></i>

</FORM>
]])

-- generate page close and autoload of first image; auto save settings
print([[
<SCRIPT language=JavaScript>
  document.getElementById("autosave").checked = getCookie("wadoseriesviewer_autosave")=='true';
  document.onload=setInterval(savesettings, 500);nframes = Number(document.forms[0].slicelister.value.split("|")[1]);
  nslices=]]..#images..[[;slice=Math.floor(nslices/2);loadsettings();
  </SCRIPT>
</BODY>
</html>
]])
