<?php

// mvh 20180303 Added method to transfer binary data next to database
// filename="t.jpg",binary="base64string"; default filename = key
// mvh 20180304 prepend table name with subdir from e.g. server/subdir/db.php
// mvh 20180304 create.php creates new tables, allow drop table; renamed base64 binary folder to images
// mvh 20180305 fix return of index in post (broken due to default filename change)
// mvh 20180328 for big brother; no delete
// mvh 20180617 Keep older versions of same uploaded file

// get the HTTP method, path, uri and body of the request
$method = $_SERVER['REQUEST_METHOD'];
$request = explode('/', trim($_SERVER['PATH_INFO'],'/'));
$uri = explode('/', $_SERVER['REQUEST_URI']);
$input = json_decode(file_get_contents('php://input'),true);
if (!$input) $input = array();

// connect to the mysql database
$link = new PDO('sqlite:../db/bb.sqlite3');

// retrieve the table and key from the path, prepending table with first part of uri
$table = preg_replace('/[^a-z0-9_]+/i','',$uri[1].'_'.array_shift($request));
$key = array_shift($request)+0;

// create new table, e.g. bb_slides if it does not exists
include 'create.php';

// escape the columns and values from the input object
$columns = preg_replace('/[^a-z0-9_]+/i','',array_keys($input));
$values = array_map(function ($value) use ($link) {
  if ($value===null) return null;
  return $link->quote((string)$value);
},array_values($input));

// build the SET part of the SQL command, skipping binary data and capturing filename
$set = '';
$bin = '';
$name = '';
$rows = '';
$vals = '';
for ($i=0;$i<count($columns);$i++) {
  if ($columns[$i]=='binary') {
    $bin = $values[$i];
  }
  else if ($columns[$i]=='filename') {
    $name = str_replace("'","", $values[$i]);
    $set.=($i>0?',':'').$columns[$i].'=';
    $set.=($values[$i]===null?'NULL':$values[$i]);
    $rows.=($i>0?',':'').$columns[$i];
    $vals.=($i>0?',':'').$values[$i];
  }
  else {
    $set.=($i>0?',':'').''.$columns[$i].'=';
    $set.=($values[$i]===null?'NULL':$values[$i]);
    $rows.=($i>0?',':'').$columns[$i];
    $vals.=($i>0?',':'').$values[$i];
  }
}

if ($vals=='') {
  $vals = 'NULL';
  $rows = 'id';
}

// create SQL based on HTTP method
switch ($method) {
  case 'GET':
    $sql = "select * from $table".($key?" WHERE id=$key":''); break;
  case 'PUT':
    $sql = "update $table set $set where id=$key"; break;
  case 'POST':
    $sql = "insert into $table ($rows) values ($vals)"; break;
  case 'DELETE':
    if (!$key) { 
       $sql = "drop table $table";
    }
    else {
      $sql = "delete from $table where id=$key";
    }      
    break;
}

// execute SQL statement
if ($method == 'GET') {
  $result = $link->query($sql);
}
else {
  $result = $link->exec($sql);
}

// die if SQL statement failed
if (!$result) {
  http_response_code(404);
  die('sqlite error: '.$link->errorInfo()[2].$sql);
}

// print results, insert id or affected row count
if ($method == 'GET') {
  $i=0;
  if (!$key) echo '[';
  foreach($result as $row) {
    echo ($i>0?',':'').json_encode($row);
    $i=$i+1;
  }
  if (!$key) echo ']';
} elseif ($method == 'POST') {
  // post generates new id, update default filename if needed
  echo $link->lastInsertId();
} else {
  echo 1;
}

// logic to delete or write the binary file to images folder
$file = __DIR__."/images/".$name;
// if ($method == 'DELETE') {
  // unlink($file);
// }
//else 
if (($method != 'GET') and ($bin != '')) {
  if (file_exists($file)) {
    for ($i=1;$i<1000;$i++) {
      if (!file_exists($file.'.'.$i)) {
        rename($file, $file.'.'.$i);
	break;
      }
    }
  }
  file_put_contents($file, base64_decode($bin), LOCK_EX);
}

// close mysql connection
$link=null;
