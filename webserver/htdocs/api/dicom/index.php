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

    // Create a Router
    $router = new \Bramus\Router\Router();
    
    $router->options('/rs/studies/([0-9%.]+)/series/([0-9%.]+)/instances/([0-9%.]+)$', function ($st, $se,$sop) {
       header('Access-Control-Allow-Headers: *');
       header('Access-Control-Allow-Origin: *');
       header('Content-Type: application/dcim');
    });

    $router->options('/.*', function () {
       header('Access-Control-Allow-Origin: *');
    });

    // 404 Handler
    $router->set404(function () {
        header($_SERVER['SERVER_PROTOCOL'] . ' 404 Not Found');
        echo 'Item not found!';
    });

    // Static route: / (homepage)
    $router->get('/$', function () {
        echo 'Conquest DICOM api; supports wado URI and QIDO (partial)';
    });

    // Static route: /wadouri
    $router->get('/wadouri$', function () {
       include 'wado.php';
    });

    $router->get('/rs/studies$', function () {
       include 'qido.php';
       $t = parse_url($_SERVER["REQUEST_URI"]);
       $output = array();
       if (array_key_exists("query", $t))
         parse_str($t["query"], $output);
       studies($output);
    });

    $router->get('/rs/studies/([0-9%.]+)/series$', function ($st) {
       include 'qido.php';
       $t = parse_url($_SERVER["REQUEST_URI"]);
       $output = array();
       if (array_key_exists("query", $t))
         parse_str($t["query"], $output);
       series($st, $output);
    });

    $router->get('/rs/studies/([0-9%.]+)/series/([0-9%.]+)/instances$', function ($st, $se) {
       include 'qido.php';
       $t = parse_url($_SERVER["REQUEST_URI"]);
       $output = array();
       if (array_key_exists("query", $t))
         parse_str($t["query"], $output);
       instances($st, $se, $output);
    });

    $router->get('/rs/studies/([0-9%.]+)/series/([0-9%.]+)/instances/([0-9%.]+)/metadata$', function ($st, $se,$sop) {
       include 'qido.php';
       metadataimage($st,$se,$sop);
    });

    $router->get('/rs/studies/([0-9%.]+)/series/([0-9%.]+)/metadata$', function ($st, $se) {
       include 'qido.php';
       metadataseries($st,$se);
    });

    $router->get('/rs/studies/([0-9%.]+)/metadata', function ($st) {
       include 'qido.php';
       metadatastudy($st);
    });

    $router->get('/rs/studies/([0-9%.]+)/series/([0-9%.]+)/instances/([0-9%.]+)$', function ($st, $se,$sop) {
       include 'qido.php';
       getinstance($st,$se,$sop);
    });

    $router->run();
