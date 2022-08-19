<?php 
// mvh 20220814: Simple and fast WADO server using conquest1.5.0c wadoparse command and servertask
include 'config.php';

$t = parse_url($_SERVER["REQUEST_URI"]);
if (array_key_exists("query", $t))
  parse_str($t["query"], $output);

// Run the executable
header_remove();
ob_start();
passthru($exe . ' "--wadoparse:' . str_replace('"', $quote, http_build_query($output)) . '"');
$var = ob_get_contents();
ob_end_clean();

// Make sure headers are passed allright
$s = substr($var, 0, 1000);
$m = strpos($s, "\n\n");
$t = explode("\n", substr($s, 0, $m));

foreach ($t as $value)
  header($value);
echo substr($var, $m+2);
?>