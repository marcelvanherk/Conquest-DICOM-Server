<?php
    require_once 'Router.php';
    include 'config.php';
    
    if ($wplogin) {
      if (!defined("DISABLE_WP_CRON")) define( 'DISABLE_WP_CRON', true );
      if (!defined("WP_PLUGIN_DIR"))   define( 'WP_PLUGIN_DIR', 'xxx' );
      require_once('./wp-blog-header.php');
      if ( !is_user_logged_in() ) return;
    }
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
    
    function CGI($name, $default) {
       $t = parse_url($_SERVER["REQUEST_URI"]);
       $s = array();
       if (array_key_exists("query", $t))
         parse_str($t["query"], $s);
       if ($name=='') return $s;
       $output = $default;
       if (array_key_exists($name, $s)) {
	 $output = $s[$name];
       }
       return $output;
    }
    
    // Create a Router
    $router = new \Bramus\Router\Router();
    
    // 404 Handler
    $router->set404(function () {
        header($_SERVER['SERVER_PROTOCOL'] . ' 404 Not Found');
        echo 'Item not found!';
    });

    //preflight
    $router->options('/rs/studies/([0-9%.]+)/series/([0-9%.]+)/instances/([0-9%.]+)$', function ($st, $se,$sop) {
       header('Access-Control-Allow-Headers: *');
       header('Access-Control-Allow-Origin: *');
       header('Content-Type: multipart/related');
    });

    $router->options('/.*', function () {
       header('Access-Control-Allow-Headers: *');
       header('Access-Control-Allow-Origin: *');
    });

    $router->options('/$', function () {
       header('Access-Control-Allow-Origin: *');
       header('Access-Control-Allow-Headers: *');
       header('Content-Type: application/json');
       $res = 
<<<EOD
    <resources base="http://api/dicom">  
      <resource path="">  
      </resource>  
      <resource path="wadouri">  
      </resource>  
      <resource path="rs">  
      </resource>  
    </resources>
EOD;
       echo $res;
    });

    // Static route: / (homepage)
    $router->get('/$', function () {
        echo 'Conquest DICOM api; partially supports wado-rs, wado-uri and qido-rs';
    });

    // Static route: /wadouri
    $router->get('/wadouri$', function () {
       include 'wado.php';
    });

    // query all studies
    $router->get('/rs/studies$', function () {
       include 'qido.php';
       $output = CGI('', '');
       querystudies($output);
    });

    // query series of a study
    $router->get('/rs/studies/([0-9%.]+)/series$', function ($st) {
       include 'qido.php';
       $output = CGI('', '');
       queryseries($st, $output);
    });

    // query all series
    $router->get('/rs/series$', function () {
       include 'qido.php';
       $output = CGI('', '');
       queryseries('', $output);
    });

    // query instances of series
    $router->get('/rs/studies/([0-9%.]+)/series/([0-9%.]+)/instances$', function ($st, $se) {
       include 'qido.php';
       $output = CGI('', '');
       queryinstances($st, $se, $output);
    });

    // query all instances
    $router->get('/rs/instances$', function () {
       include 'qido.php';
       $output = CGI('', '');
       queryinstances('', '', $output);
    });

    // metadata of a study
    $router->get('/rs/studies/([0-9%.]+)/metadata', function ($st) {
       include 'qido.php';
       getmetadata($st,'','');
    });

    // metadata of a series
    $router->get('/rs/studies/([0-9%.]+)/series/([0-9%.]+)/metadata$', function ($st,$se) {
       include 'qido.php';
       getmetadata($st,$se,'');
    });

    // metadata of an instance
    $router->get('/rs/studies/([0-9%.]+)/series/([0-9%.]+)/instances/([0-9%.]+)/metadata$', function ($st,$se,$sop) {
       include 'qido.php';
       getmetadata($st,$se,$sop);
    });

    // single frame (binary pixel data)
    $router->get('/rs/studies/([0-9%.]+)/series/([0-9%.]+)/instances/([0-9%.]+)/frames/([0-9%.]+)$', function ($st,$se,$sop,$fr) {
       include 'qido.php';
       getframe($st,$se,$sop,$fr);
    });

    // dicom instance
    $router->get('/rs/studies/([0-9%.]+)/series/([0-9%.]+)/instances/([0-9%.]+)$', function ($st,$se,$sop) {
       include 'qido.php';
       getinstances($st,$se,$sop);
    });

    // dicom instances of series
    $router->get('/rs/studies/([0-9%.]+)/series/([0-9%.]+)$', function ($st,$se) {
       include 'qido.php';
       getinstances($st,$se,'');
    });

    // dicom instances of study
    $router->get('/rs/studies/([0-9%.]+)$', function ($st) {
       include 'qido.php';
       getinstances($st,'','');
    });

    // post instance(s)
    $router->post('/rs/studies$', function () {
       include 'posters.php';
       poststow();
    });

    // attach instance (allow importconverter style script query parameter run for each object in e.g. zip)
    $router->post('/rs/attach$', function () {
       include 'posters.php';
       $script = CGI('script', '');
       attachfile($script);
    });

    // attach instance (allow lua script query parameter, let it return JSON string for response)
    $router->post('/rs/attachdicom$', function () {
       include 'posters.php';
       $script = CGI('script', '');
       attachdicomfile($script);
    });

    // thumbnail of a frame
    $router->get('/rs/studies/([0-9%.]+)/series/([0-9%.]+)/instances/([0-9%.]+)/thumbnail/frames/([0-9]+)$', function ($st,$se,$sop,$fr) {
       include 'qido.php';
       thumbnail($st,$se,$sop,$fr,128);
    });

    // thumbnail of an image
    $router->get('/rs/studies/([0-9%.]+)/series/([0-9%.]+)/instances/([0-9%.]+)/thumbnail$', function ($st,$se,$sop) {
       include 'qido.php';
       thumbnail($st,$se,$sop,0,128);
    });

    // thumbnail for series (middle one)
    $router->get('/rs/studies/([0-9%.]+)/series/([0-9%.]+)/thumbnail$', function ($st,$se) {
       include 'qido.php';
       thumbnail($st,$se,'',0,128);
    });

    // thumbnail for study (middle one)
    $router->get('/rs/studies/([0-9%.]+)/thumbnail$', function ($st) {
       include 'qido.php';
       thumbnail($st,'','',0,128);
    });
    
    // runs a script
    $router->post('/rs/script$', function () {
       include 'posters.php';
       runscript();
    });

    // start a script
    $router->post('/rs/startscript$', function () {
       include 'posters.php';
       startscript();
    });

    // get (or set) a script progress (uri parameter setvalue writes the value)
    $router->get('/rs/startscript/([0-9%.]+)$', function ($uid) {
       include 'posters.php';
       $val = CGI('setvalue', -1);
       if ($val>=0) 
         writeprogress($uid, $val);
       else
         readprogress($uid);
    });

    $router->run();