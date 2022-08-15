<?php
  function processData($params, $tags, $query, $level) {
    foreach($tags as $element) {
      $opt = array($element=>"");
      $params = array_merge($opt, $params);
    };
    $params = array_merge($params, $query);
      
    ob_start();
    passthru('servertask "--dolua:dofile([[rquery.lua]]);'.$level.'([[CONQUESTSRV1]],[[ ' . str_replace('"', '""', json_encode($params)). ' ]], true)"');
    $var = ob_get_contents();
    ob_end_clean();
   
    //header_remove();
    //header('Allow-Origin-Header: yes');
    //header('Access-Control-Allow-Headers: Authorization');
    header('Access-Control-Allow-Origin: http://localhost:3000');
    header('Content-Type: application/json');
    echo '{"data":' . $var . '}';
  }    

  // http://127.0.0.1/api/dicom/rs/studies?PatientID=118
  function studies($query) {
     $defaultTags = [
       "00080005", "00080020", "00080030",
       "00080050", "00080054", "00080056",
       "00080061", "00080090", "00081190",
       "00080201", "00100010", "00100020",
       "00100030", "00100040", "0020000D",
       "00200010", "00201206", "00201208"];
    processData($query, $defaultTags, array("99990c00"=>"StudyDate"), "studies");
  }

  // http://127.0.0.1/api/dicom/rs/studies/1.2.840.113704.1.111.5068.1602767444.4/series
  function series($st, $query) {
    $defaultTags = [
      "00080005", "00080054", "00080056",
      "00080060", "0008103E", "00081190",
      "0020000E", "00200011", "00201209",
      "00080201", "00400244", "00400245",
      "00400275", "00400009", "00401001"];
    processData($query, $defaultTags, array("StudyInstanceUID"=>$st, "99990c00"=>"SeriesTime"), "series");
  }

  // http://127.0.0.1/api/dicom/rs/studies/1.2.840.113704.1.111.5068.1602767444.4/series/1.2.840.113704.1.111.7348.1602767982.6/instances
  function instances($st, $se, $query) {
    $defaultTags = [
      "00080016", "00080018", "00080056",
      "00080201", "00081190", "00200013",
      "00280010", "00280011", "00280100",
      "00280008"];
    processData($query, $defaultTags, array("StudyInstanceUID"=>$st, "SeriesInstanceUID"=>$se, "99990c00"=>"ImageNumber"), "images");
  }
