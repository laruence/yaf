--TEST--
ISSUE #232 (Segfault with Yaf_Route_Simple)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.lowcase_path=0
yaf.throw_exception=0
yaf.catch_exception=1
yaf.use_namespace=0
--GET--
c=index
--FILE--
<?php 
$router = new Yaf_Route_Simple("m", "c", "a");
$request = new Yaf_Request_Http("/");
$router->route($request);
var_dump($request->getModuleName());
var_dump($request->getControllerName());
var_dump($request->getActionName());
?>
--EXPECTF--
NULL
string(5) "index"
NULL
