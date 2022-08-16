<?php
    if ($_SERVER['REQUEST_METHOD'] == 'OPTIONS') {
        header('Access-Control-Allow-Origin: *');
        exit(0);
    }

    require_once 'Router.php';

    // Create a Router
    $router = new \Bramus\Router\Router();

    // 404 Handler
    $router->set404(function () {
        header($_SERVER['SERVER_PROTOCOL'] . ' 404 Not Found');
        echo 'Item not found!';
    });

    // Static route: / (homepage)
    $router->get('/', function () {
        echo 'Conquest DICOM api; supports wado URI and QIDO (partial)';
    });

    // Static route: /wadouri
    $router->get('/wadouri', function () {
       include 'wado.php';
    });

    $router->get('/rs/studies', function () {
       include 'qido.php';
       $t = parse_url($_SERVER["REQUEST_URI"]);
       $output = array();
       if (array_key_exists("query", $t))
         parse_str($t["query"], $output);
       studies($output);
    });

    $router->get('/rs/studies/([0-9%.]+)/series', function ($st) {
       include 'qido.php';
       $t = parse_url($_SERVER["REQUEST_URI"]);
       $output = array();
       if (array_key_exists("query", $t))
         parse_str($t["query"], $output);
       series($st, $output);
    });

    $router->get('/rs/studies/([0-9%.]+)/metadata', function ($st) {
       include 'qido.php';
       metadata($st);
    });

    $router->get('/rs/studies/([0-9%.]+)/series/([0-9%.]+)/instances', function ($st, $se) {
       include 'qido.php';
       $t = parse_url($_SERVER["REQUEST_URI"]);
       $output = array();
       if (array_key_exists("query", $t))
         parse_str($t["query"], $output);
       instances($st, $se, $output);
    });

    $router->run();
