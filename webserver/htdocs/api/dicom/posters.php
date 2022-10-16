<?php
/**
 * Parse arbitrary multipart/form-data content
 * Note: null result or null values for headers or value means error
 * @return array|null [{"headers":array|null,"value":string|null}]
 * @param string|null $boundary
 * @param string|null $content
 */
function parse_multipart_content(?string $content, ?string $boundary): ?array {
  if(empty($content) || empty($boundary)) return null;
  $sections = array_map("trim", explode("--$boundary", $content));
  $parts = [];
  foreach($sections as $section) {
    if($section === "" || $section === "--") continue;
    $fields = explode("\r\n\r\n", $section);
    if(preg_match_all("/([a-z0-9-_]+)\s*:\s*([^\r\n]+)/iu", $fields[0] ?? "", $matches, PREG_SET_ORDER) === 2) {
      $headers = [];
      foreach($matches as $match) $headers[$match[1]] = $match[2];
    } else $headers = null;
    $parts[] = ["headers" => $headers, "value"   => $fields[1] ?? null];
  }
  return empty($parts) ? null : $parts;
}

// post one or more instances belonging to study; takes PatientID and PatientName from existing study
function poststow() {
  include 'config.php';
  $d=file_get_contents("php://input");
  $t = getallheaders();
  if (!array_key_exists("Content-Type", $t)) {
    echo 'Does not contain content type';
    return;
  }
  preg_match_all("/boundary=\"([^;= ]+)\"/", $t["Content-Type"], $r);
  $boundary= $r[1][0];

  $d = parse_multipart_content($d, $boundary);
  for ($i=0; $i<count($d); $i++) {
    $file = tmpfile();
    fwrite($file, $d[$i]["value"]);
    $path = stream_get_meta_data($file)['uri'];
    ob_start();
    passthru($exe . ' "--dolua:dofile([[posters.lua]]);poststow([['.$path.']])"');
    $var = $var . ob_get_contents();
    ob_end_clean();
    fclose($file);
  }

  header('Access-Control-Allow-Origin: *');
  header('Content-Type: application/json');
  echo $var;
}

// post arbitrary file to conquest attachfile: command
// script is in importConverter format; e.g. add lua: if needed
// scripts in the form-data take preference over query string
function attachfile($script) {
  include 'config.php';
  $file = null;
  $ext = '.dcm';
  
  if (array_key_exists('script', $_POST)) {
    $script = $_POST['script'];
  }

  if(count($_FILES) > 0){
    $first_file_index = $_FILES[array_keys($_FILES)[0]];
    $path = $first_file_index['tmp_name'];
    $ext = '.' . pathinfo($first_file_index['name'], PATHINFO_EXTENSION);
  }
  else {
    $d=file_get_contents("php://input");
    $t = getallheaders();
    if (!array_key_exists("Content-Type", $t)) {
      echo 'Does not contain content type';
      return;
    }
    $file = tmpfile();
    $typ=$t["Content-Type"];
    
    $ext = '.dcm';
    if (strpos($typ, 'application/zip')!==false) $ext='.zip';
    
    if (strpos($typ, 'multipart/related')!==false) {
      preg_match_all("/boundary=\"([^;= ]+)\"/", $typ, $r);
      $boundary= $r[1][0];
    
      $d = parse_multipart_content($d, $boundary);
      fwrite($file, $d[0]["value"]);
    }
    else {
      fwrite($file, $d);
    }
    $path = stream_get_meta_data($file)['uri'];
  }

  ob_start();
  passthru($exe . ' "--dolua:dofile([[posters.lua]]);attachfile([['.$path.']], [['. str_replace('"', $quote, $script).']], [['. $ext .']])"');
  $var = ob_get_contents();
  ob_end_clean();
  if ($file!=null)
    fclose($file);

  header('Access-Control-Allow-Origin: *');
  header('Content-Type: application/json');
  echo $var;
}

// post DICOM file, lua script determines what happens with it
// scripts in the form-data take preference over query string
function attachdicomfile($script) {
  include 'config.php';
  $file = null;

  if (array_key_exists('script', $_POST)) {
    $script = $_POST['script'];
  }

  if(count($_FILES) > 0){
    $first_file_index = $_FILES[array_keys($_FILES)[0]];
    $path = $first_file_index['tmp_name'];
  }
  else { 
    $d=file_get_contents("php://input");
    $t = getallheaders();
    if (!array_key_exists("Content-Type", $t)) {
      echo 'Does not contain content type';
      return;
    }
    $file = tmpfile();
    $typ=$t["Content-Type"];

    if (strpos($typ, 'multipart/related')!==false) {
      preg_match_all("/boundary=\"([^;= ]+)\"/", $typ, $r);
      $boundary= $r[1][0];

      $d = parse_multipart_content($d, $boundary);
      fwrite($file, $d[0]["value"]);
    }
    else {
      fwrite($file, $d);
    }
    $path = stream_get_meta_data($file)['uri'];
  }

  ob_start();
  passthru($exe . ' "--dolua:dofile([[posters.lua]]);attachdicomfile([['.$path.']], [['. str_replace('"', $quote, $script).']])"');
  $var = ob_get_contents();
  ob_end_clean();
  if ($file!=null)
    fclose($file);
  header('Access-Control-Allow-Origin: *');
  header('Content-Type: application/json');
  echo $var;
}

// post script to run immediately
function runscript() {
  include 'config.php';
  $script = $_POST['script'];

  $scr = tmpfile();
  fwrite($scr, $script);
  $scrpath = stream_get_meta_data($scr)['uri'];
  
  ob_start();
  passthru($exe . ' "--dolua:dofile([[posters.lua]]);runscript([['. $scrpath . ']])"');
  $var = ob_get_contents();
  ob_end_clean();
  fclose($scr);
  header('Access-Control-Allow-Origin: *');
  header('Content-Type: application/json');
  echo $var;
}

// post script to run in background; returns uid
function startscript() {
  include 'config.php';
  $script = $_POST['script'];

  $scr = tmpfile();
  fwrite($scr, $script);
  $scrpath = stream_get_meta_data($scr)['uri'];
  
  ob_start();
  passthru($exe . ' "--dolua:dofile([[posters.lua]]);startscript([['. $scrpath . ']])"');
  $var = ob_get_contents();
  ob_end_clean();
  fclose($scr);
  header('Access-Control-Allow-Origin: *');
  header('Content-Type: application/json');
  echo $var;
}

// read progress of script with given uid
function readprogress($uid) {
  include 'config.php';
  ob_start();
  passthru($exe . ' "--dolua:dofile([[posters.lua]]);readprogress([['. $uid . ']])"');
  $var = ob_get_contents();
  ob_end_clean();
  header('Access-Control-Allow-Origin: *');
  header('Content-Type: application/json');
  echo $var;
}

// write progress of script with given uid
function writeprogress($uid, $val) {
  include 'config.php';
  ob_start();
  passthru($exe . ' "--dolua:dofile([[posters.lua]]);writeprogress([['. $uid . ']], ' . $val . ')"');
  $var = ob_get_contents();
  ob_end_clean();
  header('Access-Control-Allow-Origin: *');
  header('Content-Type: application/json');
  echo $var;
}
