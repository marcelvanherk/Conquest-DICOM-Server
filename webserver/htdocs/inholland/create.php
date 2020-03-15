<?php

// 20191019 mvh For inholland

if ($table==$uri[1].'_studies') {
  $link->exec('create table if not exists '.$table.
  ' (id integer PRIMARY KEY,
  studyID varchar(64),
  description varchar(64),
  modified DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
  );'
  );
}

if ($table==$uri[1].'_students') {
  $link->exec('create table if not exists '.$table.
  ' (id integer PRIMARY KEY,
  studentID varchar(64),
  groupID varchar(64),
  period varchar(64),
  modified DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
  );'
  );
}

if ($table==$uri[1].'_patients') {
  $link->exec('create table if not exists '.$table.
  ' (id integer PRIMARY KEY,
  patientID varchar(64),
  studyID varchar(64),
  templateID varchar(64),
  goldenStandardIDs varchar(640),
  modified DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
  );'
  );
}

if ($table==$uri[1].'_sessions') {
  $link->exec('create table if not exists '.$table.
  ' (id integer PRIMARY KEY,
  caseID varchar(64),
  modified DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
  );'
  );
}

if ($table==$uri[1].'_delineations') {
  $link->exec('create table if not exists '.$table.
  ' (id integer PRIMARY KEY,
  delineationID varchar(64), // 12 12_1
  studentID varchar(64),
  groupID varchar(64),
  filename varchar(64),
  modified DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
  );'
  );
}
