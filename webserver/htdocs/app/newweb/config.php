<?php
  $folder = '.';				// where are the newweb files
  $exe    = 'dgate -p5678 -q127.0.0.1';		// communication with DICOM server
  $quote  = '""';				// quotes in command line

  if (PHP_OS_FAMILY != 'Windows') {		// On Linux:
    $exe = './' . $exe;				// start as ./servertask
    $quote = '\"';				// quotes in command line
  }

  $userlogin = false;				// uses single file login system
  $wplogin   = false;				// uses wordpress login system
  $cors      = false;				// allow cross site use
