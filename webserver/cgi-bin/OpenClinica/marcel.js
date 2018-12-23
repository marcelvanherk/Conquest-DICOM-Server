// Support functions for linking OpenClinica to the Conquest DICOM server
// Marcel van Herk, last updated August 27, 2012
// web link of conquest server and hospital patient IDs are kept in cookies

// How to use: 
// 1: sudo cp Desktop/marcel.js /usr/local/tomcat/webapps/OpenClinica/includes
// 2: put this kind of information in the ECRF excel sheet for openclinica and create CRF as usual

//A9: BASELINECT
//B9: Baseline CT PACS link
//C9: <A HREF=# onclick="javascript:link(8,1,'CT'); return false;">Baseline CT:</A> 
//D9: .
//H9: Diagnostic scans:<script type="text/javascript" language="JavaScript" src=includes/marcel.js></script>
//I9: PACS link: <A HREF=# onclick="javascript:openconfig(); return false;">Configure</A> 
//N9: calculation
//O9: link1
//P9: calculation
//Q9: func: getExternalValue("http://dummy.html",right,600,400)
//R9: ST

//A10: BASELINEPET
//B10: Baseline PET PACS link
//C10: <A HREF=# onclick="javascript:link(9,1,'PET'); return false;">Baseline PET:</A> 
//D10: .
//H10:
//I10:
//N10: calculation
//O10: link2
//P10: calculation
//Q10: func: getExternalValue("http://dummy.html",right,600,400)
//R10: ST

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

// uncomment following line for testing cookie code
// setCookie("conquestlink","",365*5);


// get conquestlink from user or cookie
function conquestlink(retype)
{ var link=getCookie("conquestlink");
  if (link!=null && link!="" && retype==0)
  { return link;
  }
  else
  { if (link==null) link="http://127.0.0.1/cgi-bin/dgate.exe";
    link=prompt("Please enter web adress of conquest web server:",link);
    if (link!=null && link!="")
    { setCookie("conquestlink",link,365*5);
      return link;
    }
  }
  return "http://127.0.0.1/cgi-bin/dgate.exe";
}

// get patient ID from user or cookie
function getpatientid(subjectId)
{ var id=getCookie("patientid"+subjectId);
  if (id==null) id="";
  id=prompt("Please enter hospital patient ID for this subject:",id);
  if (id!=null && id!="")
  { setCookie("patientid"+subjectId,id,365*5);
    return id;
  }
  return null;
}

function getElementsByClassName(className)
{ var hasClassName = new RegExp("(?:^|\\s)" + className + "(?:$|\\s)");
  var allElements = document.getElementsByTagName("*");
  var results = [];

  var element;
  for (var i = 0; (element = allElements[i]) != null; i++) 
  { var elementClass = element.className;
    if (elementClass && elementClass.indexOf(className) != -1 && hasClassName.test(elementClass))
	results.push(element);
  }

  return results;
}

function trim10 (str) 
{ var whitespace = ' \n\r\t\f\x0b\xa0';
  for (var i = 0; i < str.length; i++) 
  { if (whitespace.indexOf(str.charAt(i)) === -1) 
    { str = str.substring(i);
      break;
    }
  }
  for (i = str.length - 1; i >= 0; i--)
  { if (whitespace.indexOf(str.charAt(i)) === -1) 
    { str = str.substring(0, i + 1);
      break;
    }
  }
  return whitespace.indexOf(str.charAt(0)) === -1 ? str : '';
}

// get subjectKey from HTML page (highly specific to OpenClinica version)
function getSubjectKey()
{ return trim10(getElementsByClassName('title_manage')[1].childNodes[0].data);
}

function getECRFName()
{ try
  { return trim10(getElementsByClassName('title_manage')[0].childNodes[0].innerText);
  }
  catch(err)
  { return trim10(getElementsByClassName('title_manage')[0].childNodes[1].childNodes[0].data);
  }
}

function GetOccuranceNumber()
{ return trim10(getElementsByClassName('table_cell_noborder')[3].childNodes[0].data);
}

function GetStudyName()
{ return trim10(getElementsByClassName('table_cell_noborder')[6].childNodes[0].data);
}

function GetDateOfBirth()
{ return trim10(getElementsByClassName('table_cell_noborder')[7].childNodes[0].data);
}

function GetPersonID()
{ return trim10(getElementsByClassName('table_cell_noborder')[10].childNodes[0].data);
}

// get entry from page URL
function gup( name ) 
{ name = name.replace(/[[]/,"\[").replace(/[]]/,"\]");
  var regexS = "[\?&]"+name+"=([^&#]*)";
  var regex = new RegExp( regexS );
  var results = regex.exec( window.location.href );
  if( results == null ) return "";
  else                  return results[1]; 
}

function isNumber(n) 
{ return !isNaN(parseFloat(n)) && isFinite(n);
}

// get item by number or literal (cannot be number) from OpenClinica CRF
function getitem(item)
{ var firstitem=parseInt(document.forms[0].formFirstField.value);
  var itemname = 'input'+(firstitem+item-1);
  if (isNumber(item)) 
    return document.forms[0]['input'+(firstitem+item-1)].value;
  else
    return item;
}

// get item name by number from OpenClinica CRF
function getitemname(item)
{ var firstitem=parseInt(document.forms[0].formFirstField.value);
  return 'input'+(firstitem+item-1);
}

// These functions are to be called from HTML code generated from the CRF excel file

//<script type="text/javascript" language="JavaScript src=http://127.0.0.1/marcel.js><script>
//<a href="#" onclick="javascript:openconfig(); return false;">Configure</a>

//<A HREF=# onclick="javascript:link(8,1,'CT'); return false;">Baseline CT:</A></td>
//func: getExternalValue("http://127.0.0.1/cgi-bin/dgate/mode=OpenClinicaSimpleLink&Modality=CT&",right,600,400)

// open conquest configuration page
function openconfig()
{ window.open(conquestlink(1)+
   '?mode=OpenClinica/Config',
   '_Blank','width=800,height=600,scrollbars=1');
}

// request link from conquest, patient ID from cookie, rest from url & page, result into item
function link(item, dateitem, modalityitem)
{ var date     = getitem(dateitem);
  var modality = getitem(modalityitem); // can also be literal or empty
  var link     = getitem(item);
  var itemname = 'mainForm.' + getitemname(item);

  var id = getpatientid(getSubjectKey());
  if (id==null) return;

  window.open(conquestlink(0)+'?mode=OpenClinica/Link'+
    // result needs to be posted here
    '&item='         + itemname +

    // PACS stuff on the page (non-anonymized)
    '&modality='     + modality +
    '&date='         + date + 
    '&patientId='    + id +

    // OC keys
    '&eventDefinitionCRFId=' + gup('eventDefinitionCRFId') +
    '&subjectId='    + gup('subjectId') +
    '&studyEventId=' + gup('studyEventId') +
    '&eventCRFId='   + document.forms[0].eventCRFId.value +

    // title info
    '&subjectKey='   + getSubjectKey() +
    '&ECRFName='     + getECRFName() +

    // crf header info
    '&OccuranceNumber=' + GetOccuranceNumber() +
    '&StudyName='    + GetStudyName() +
    '&DateOfBirth='  + GetDateOfBirth() +
    '&PersonID='     + GetPersonID() +

    '&link='         + link
    ,'_Blank','width=600,height=400,scrollbars=1');

  // for test: postItToOC(itemname, "test");
  // for test: postItToOC(itemname, shortenlink("id:long1:long2:long3"), 10);
}

// This function posts value to a given item
function postItToOC (itemname, value) 
{ var mes = itemname + ":" + value;
  postMessage (mes, "*");        
}

// This function clears the item; to be used from web page called from func: getExternalValue
function clearitem () 
{ var mes = gup('item') + ":";
  postMessage (mes, "*");        
}

// This function shortens a link to fit in maxlen characters by leaving out the one but last UID
// i.e, id:long1:long2:long3 --> id:long1::long3; works for patientid:studyuid, 
// patientid:studyuid:seriesuid, or patientid:studyuid:seriesuid:sopuid
function shortenlink(link, maxlen)
{ if (link.length<maxlen) return link;
  var temp = new Array();
  temp = link.split(':');
  link = '';
  for (var i=0; i<temp.length; i++)
  { if (i != temp.length-2)
      link = link+temp[i];
    if (i != temp.length-1)
      link = link + ':';
  }
  return link;
}

