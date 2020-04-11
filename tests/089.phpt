--TEST--
Check for Yaf_Config_Ini gets
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php 
$file = dirname(__FILE__) . "/simple.ini";

define("APPLICATION_PATH", __DIR__);
$config = new Yaf_Config_Ini($file);
var_dump(count($config->base));
var_dump(gettype($config->base->application->directory));

var_dump($config->base->routes->rewrite->route->controller);
var_dump($config->base["routes"]["rewrite"]["route"]["controller"]);
var_dump($config->get("base.routes.rewrite.route.controller"));

var_dump($config->base->get("routes.rewrite.match"));
var_dump($config["base.routes.rewrite.route.controllers"]);

$config = new Yaf_Config_Ini($file, "nocatch");
var_dump(count($config));
var_dump($config->get("routes.rewrite.match"));
var_dump($config->application->dispatcher->throwException);
var_dump($config->application["dispatcher"]["catchException"]);
?>
--EXPECTF--
int(5)
string(6) "string"
string(5) "Index"
string(5) "Index"
string(5) "Index"
string(17) "/yaf/:name/:value"
NULL
int(6)
string(15) "/yaf/:name/:age"
string(0) ""
string(1) "1"
