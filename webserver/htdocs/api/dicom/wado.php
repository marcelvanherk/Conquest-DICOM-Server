<?php 
// mvh 20220814: Simple and fast WADO server using conquest1.5.0c wadoparse command and servertask

$exe       = "servertask";					// also ok for linux
$userlogin = false;						// uses single file login system
$wplogin   = false;						// uses wordpress login system

if ($wplogin) {
  if (!defined("DISABLE_WP_CRON")) define( 'DISABLE_WP_CRON', true );
  if (!defined("WP_PLUGIN_DIR"))   define( 'WP_PLUGIN_DIR', 'xxx' );
  require_once('./wp-blog-header.php');
  if ( !is_user_logged_in() ) return;
}

// this script offers login dialog etc; it stores user.db in $folder
if ($userlogin) {
  include('singlefilelogin.php');
  if (!$application->getUserLoginStatus())
    die();

  // redirect page to clear just posted login data (username and password)
  if (!empty($_POST['user_password'])) {
    unset($_POST);
    header("Location: ".$_SERVER['REQUEST_URI']);
    exit;
  }
}

$t = parse_url($_SERVER["REQUEST_URI"]);
if (array_key_exists("query", $t))
  parse_str($t["query"], $output);

// Run the executable
header_remove();
ob_start();
passthru($exe . ' "--wadoparse:' . http_build_query($output) . '"');
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