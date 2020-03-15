<?php

$path =  getcwd() . '\dgate.exe' . ' "' . $data->study . '" "' . $data->caseNo. '" "' . $data->session . '" "' . $data->observer . '" "' . $references . '" "' . $data->comment . '" -log'; 
exec($path);

$size = filesize($yourfile);
header("Content-type: application/zip"); 
header("Content-Disposition: attachment; filename=$archive_file_name");
header("Content-length: " . $size);
header("Pragma: no-cache"); 
header("Expires: 0"); 
readfile("$yourfile");
