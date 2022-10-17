<?php
  function processData($params, $tags, $query, $level) {
    include 'config.php';
    foreach($tags as $element) {
      $opt = array($element=>"");
      $params = array_merge($opt, $params);
    };
    foreach ($params as $key => $value) {
      if ($key=='limit')  $params["99990C01"]=$value;
      if ($key=='offset') $params["99990C02"]=$value;
    };
    $params = array_merge($params, $query);
    $t = str_replace('"', $quote, json_encode($params));    
 
    ob_start();
    passthru($exe . ' "--dolua:dofile([[rquery.lua]]);rquery(nil, [[ ' . $t . ' ]], [[' . $level . ']])"');
    $var = ob_get_contents();
    ob_end_clean();
   
    header('Access-Control-Allow-Origin: *');
    header('Content-Type: application/json');
    echo $var;
  }    

  // http://127.0.0.1/api/dicom/rs/studies?PatientID=118
  function querystudies($query) {
     $defaultTags = [
       "00080005", "00080020", "00080030",
       "00080050", "00080054", "00080056",
       "00080061", "00080090", "00081190",
       "00080201", "00100010", "00100020",
       "00100030", "00100040", "0020000D",
       "00200010", "00201206", "00201208"];
    processData($query, $defaultTags, array("99990C00"=>"PatientName"), "STUDY");
  }

  // http://127.0.0.1/api/dicom/rs/studies/1.2.840.113704.1.111.5068.1602767444.4/series
  function queryseries($st, $query) {
    $defaultTags = [
      "00080005", "00080054", "00080056",
      "00080060", "0008103E", "00081190",
      "0020000E", "00200011", "00201209",
      "00080201", "00400244", "00400245",
      "00400275", "00400009", "00401001"];
    processData($query, $defaultTags, array("StudyInstanceUID"=>$st, "99990C00"=>"SeriesTime"), "SERIES");
  }

  // http://127.0.0.1/api/dicom/rs/studies/1.2.840.113704.1.111.5068.1602767444.4/series/1.2.840.113704.1.111.7348.1602767982.6/instances
  function queryinstances($st, $se, $query) {
    $defaultTags = [
      "00080016", "00080018", "00080056",
      "00080201", "00081190", "00200013",
      "00280010", "00280011", "00280100",
      "00280008"];
    processData($query, $defaultTags, array("StudyInstanceUID"=>$st, "SeriesInstanceUID"=>$se, "99990C00"=>"ImageNumber"), "IMAGE");
  }

  function getmetadata($st,$se,$sop) {
    include 'config.php';
    ob_start();
    passthru($exe.' "--dolua:dofile([[rquery.lua]]);getmetadata(nil,[['.$st.']],[['.$se.']],[['.$sop.']])"');
    $var = ob_get_contents();
    ob_end_clean();
    header('Access-Control-Allow-Origin: *');
    header('Content-Type: application/json');
    echo $var;
  }

  function generateRandomString($length) {
    return substr(str_shuffle(str_repeat($x='0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ', ceil($length/strlen($x)))),1,$length);
  }

  function getinstances($st,$se,$sop) {
    include 'config.php';
    $bd = generateRandomString(32);
    ob_start();
    passthru($exe.' "--dolua:dofile([[rquery.lua]]);getinstances(nil,[['.$bd.']],[['.$st.']],[['.$se.']],[['.$sop.']])"');
    $var = ob_get_contents();
    ob_end_clean();
    header('Access-Control-Allow-Headers: *');
    header('Access-Control-Allow-Origin: *');
    header('Content-Type: multipart/related; boundary='.$bd);
    echo $var;
  }
  
  function getframe($st,$se,$sop,$fr) {
    include 'config.php';
    $bd = generateRandomString(32);
    ob_start();
    passthru($exe.' "--dolua:dofile([[rquery.lua]]);getframe(nil,[['.$st.']],[['.$se.']],[['.$sop.']],'.$fr.')"');
    $var = ob_get_contents();
    ob_end_clean();
    header('Access-Control-Allow-Headers: *');
    header('Access-Control-Allow-Origin: *');
    header('Content-Type: multipart/related; boundary='.$bd);

    echo("--".$bd."\r\n");
    echo("Content-Type: application/dicom\r\n");
    header('Access-Control-Allow-Origin: *');
    echo("Content-Transfer-Encoding: binary\r\n\r\n");
    echo $var;
    echo("--".$bd."--\r\n\r\n");
  }

  function thumbnail($st,$se,$sop,$fr,$sz) {
    include 'config.php';
    ob_start();
    passthru($exe.' "--dolua:dofile([[rquery.lua]]);getthumbnail(nil,[['.$st.']],[['.$se.']],[['.$sop.']],'.$fr.','.$sz.')"');
    $var = ob_get_contents();
    ob_end_clean();
    header('Access-Control-Allow-Headers: *');
    header('Access-Control-Allow-Origin: *');
    header('Content-Type: image/jpeg');
    echo $var;
  }
  
  function modalities() {
    include 'config.php';
    ob_start();
    passthru($exe.' "--dolua:dofile([[rquery.lua]]);remotemodalities()"');
    $var = ob_get_contents();
    ob_end_clean();
    header('Access-Control-Allow-Headers: *');
    header('Access-Control-Allow-Origin: *');
    header('Content-Type: application/json');
    echo $var;
  }

  function dicomecho($ae) {
    include 'config.php';
    ob_start();
    passthru($exe.' "--dolua:dofile([[rquery.lua]]);remoteecho([['.$ae.']])"');
    $var = ob_get_contents();
    ob_end_clean();
    header('Access-Control-Allow-Headers: *');
    header('Access-Control-Allow-Origin: *');
    header('Content-Type: application/json');
    echo $var;
  }

  function zip($st,$se,$sop,$script) {
    include 'config.php';
    ob_start();
    passthru($exe.' "--dolua:dofile([[rquery.lua]]);remotezip(nil,[['.$st.']],[['.$se.']],[['.$sop.']],[['.$script.']])"');
    $var = ob_get_contents();
    ob_end_clean();
    header('Access-Control-Allow-Headers: *');
    header('Access-Control-Allow-Origin: *');
    header('Content-Type: application/zip');
    echo $var;
  }

  function move($src,$dest,$st,$se,$sop,$script) {
    include 'config.php';
    ob_start();
    $params = array("StudyInstanceUID"=>$st, "SeriesInstanceUID"=>$se, "SOPInstanceUID"=>$sop, "99990900"=>$script);
    $t = str_replace('"', $quote, json_encode($params));    
    passthru($exe.' "--dolua:dofile([[rquery.lua]]);remotemove([['.$src.']],[['.$dest.']],[['.$t.']])"');
    $var = ob_get_contents();
    ob_end_clean();
    header('Access-Control-Allow-Headers: *');
    header('Access-Control-Allow-Origin: *');
    header('Content-Type: application/json');
    echo $var;
  }
  