--TEST--
FR #62702 (Make baseuri case-insensitive)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php

$router = new Yaf_Route_Static();
$request = new Yaf_Request_Http("/sample", "/sample");
$router->route($request);
var_dump($request->getControllerName());

$request = new Yaf_Request_Http("/Sample/ABC", "/sample");
$router->route($request);
var_dump($request->getControllerName());


$router = new Yaf_Route_Map(true);
$request = new Yaf_Request_Http("/sample/A/B/C", "/sample");
$router->route($request);
var_dump($request->getControllerName());

$request = new Yaf_Request_Http("/sample", "/sAmplE");
$router->route($request);
var_dump($request->getControllerName());

$router = new Yaf_Route_Regex("#^/test#", array("controller" => "info"), array());
$request = new Yaf_Request_Http("/test/", "/Test");
$router->route($request);
var_dump($request->getControllerName());

$request = new Yaf_Request_Http("/sample/test", "/sAmplE");
$router->route($request);
var_dump($request->getControllerName());

$router = new Yaf_Route_Rewrite("/test", array("controller" => "info"), array());
$request = new Yaf_Request_Http("/test/", "/Test");
$router->route($request);
var_dump($request->getControllerName());

$request = new Yaf_Request_Http("/sample/test", "/sAmplE");
$router->route($request);
var_dump($request->getControllerName());
?>
--EXPECTF--
NULL
string(3) "ABC"
string(5) "A_B_C"
NULL
NULL
string(4) "info"
NULL
string(4) "info"
