<?php 

///////////////////////////////////////////////////////////////////////////
// mvh 20201021: emulated dgate CGI application with example login system
// starts with user admin only, admin_password and admin_email taken 
// from dicom.ini; user admin is allowed to register other users
//
// This PHP script calls dgate to emulate a CGI interface
// place anywhere in a web server to create web interface
// To configure, modify the next 3 lines for your newweb location; this can
// be in the dicomserver itself e.g c:\dicomserver\webserver\cgi-bin\newweb
//
// Make sure post_max_size=0 or a reasonable limit in php.ini
//
// 202001023  mvh  Added SCRIPT_FILENAME variable
// 202001024  mvh  Working on passing POST (problem: popen does not work in binary mode, now translate to equivalent GET)
// 202001025  mvh  Make sure variables in called lua code are define local and do not clash with globals
// 202001025  mvh  Fixed SCRIPT_NAME

$folder = "c:\\dicomserver\\webserver\\cgi-bin\\newweb";
$exe = "dgate.exe";
$userlogin = true;

chdir($folder); 

// nasty: patch dicom.ini to use dgate.php
$ini=file_get_contents("dicom.ini");
$n = strpos($ini, "WebScriptAddress         = http");
if ($n>0) {
  $m = strpos(substr($ini, $n), "\n");
  $t = substr($ini, 0, $n) . "WebScriptAddress = dgate.php" . substr($ini, $n+$m);
  file_put_contents("dicom.ini", $t);
}

// this script offers login dialog etc; it stores user.db in $folder
if ($userlogin) {
  include('singlefilelogin.php');
  if (!$application->getUserLoginStatus())
    die();
}

// redirect page to clear just posted login data (username and password)
if (!empty($_POST['user_password'])) {
  unset($_POST);
  header("Location: ".$_SERVER[REQUEST_URI]);
  exit;
}

$t = parse_url($_SERVER["REQUEST_URI"]);
if (array_key_exists("query", $t))
  parse_str($t["query"], $output);
else
  $output = ["mode" => "top"];

// translate selected posts into a get (cannot pass binary data to popen)
if(!empty($_SERVER["CONTENT_LENGTH"])) {
  $fn = $_FILES['filetoupload']['tmp_name'];
  if ($fn=='')
    die("file failed to upload; check size limit 'post_max_size' in php.ini");
  
  $output = $_POST;
  $output['_passfile_'] = $fn;
  
  if($output['mode']=='addlocalfile') {
    $output['mode']='start';
    $output['parameter']='uploadfile';
    $output['script']='print(filename);local x=DicomObject:new();x:Read(filename);x:AddImage()';
  }
  else if($output['mode']=='attachfile') {
    $output['mode']='start';
    $output['parameter']='uploadfile';
    $output['script']='local pscript=[['.$_POST['script'].']];local x=DicomObject:new();x:Read(filename);x:Script(pscript);x:AddImage()';
  }
}

// pass parameters to cgi application
putenv("SCRIPT_NAME=".$_SERVER["SCRIPT_NAME"]);
putenv("SCRIPT_FILENAME=".$_SERVER["SCRIPT_FILENAME"]);
putenv("QUERY_STRING=".http_build_query($output));

// Run the cgi executable
header_remove();
ob_start();
passthru($exe);
$var = ob_get_contents();
ob_end_clean();

// Make sure headers are passed allright
$m = strpos($var, "\n\n");
$t = explode("\n", substr($var, 0, $m));
foreach ($t as $value)
  header($value);
echo substr($var, $m+2);

// Add display of logged in user and logout function to selected pages
if ($userlogin) {
  // do not touch WADO pages
  if (isset($_GET["requestType"]) 
    && $_GET["requestType"] == "WADO") 
     return;

  // do not touch POST messages
  if(!empty($_SERVER["CONTENT_LENGTH"]))
    return;
  
  // hands-off binary output and internal data
  if (isset($_GET["parameter"])) return;

  // these are the selected pages
  if (isset($_GET["mode"]) && 
    !in_array($_GET["mode"], 
       array("start", "listpatients", "liststudies", "listseries",
             "wadostudyviewer", "listimages", "listimageswiththumbs")
            )
      )
    return;
  
  // here is the nice floating username/logout link
  echo '<div style="left:90%;top:8px;width:50px;height:12px;position:fixed;z-index:1; background-color:rgba(250,250,250,0.3);padding:10px">';
  echo '<a style="float:right" onmouseover=this.innerHTML="Logout" onmouseout=this.innerHTML="'.$_SESSION['user_name'].'" href="' . $_SERVER['SCRIPT_NAME'] . '?action=logout">'.$_SESSION['user_name'].'</a>';
  echo '</div>';
  
  // and a link to create new users for the admin
  if ($_SESSION['user_name']=='admin')
     echo '<a href="' . $_SERVER['SCRIPT_NAME'] . '?action=register">Register new account</a>';
}
?>