--TEST--
Check for Yaf_Request::set*Name 's second argument
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
yaf.forward_limit = 10;
--FILE--
<?php
$request = new Yaf_Request_Simple();

$request->setModuleName("foo", true);
var_dump($request->getModuleName());
$request->setModuleName("foo", false);
var_dump($request->getModuleName());
$request->setModuleName("foo_bAR", true);
var_dump($request->getModuleName());
$request->setModuleName("foo_bAR", false);
var_dump($request->getModuleName());


$request->setControllerName("foo", true);
var_dump($request->getControllerName());
$request->setControllerName("foo", false);
var_dump($request->getControllerName());
$request->setControllerName("foo_bAR", true);
var_dump($request->getControllerName());
$request->setControllerName("foo_bAR", false);
var_dump($request->getControllerName());

$request->setActionName("Foo", true);
var_dump($request->getActionName());
$request->setActionName("Foo", false);
var_dump($request->getActionName());
$request->setActionName("foo_bAR", true);
var_dump($request->getActionName());
$request->setActionName("foo_bAR", false);
var_dump($request->getActionName());
?>
--EXPECT--
string(3) "Foo"
string(3) "foo"
string(7) "Foo_Bar"
string(7) "foo_bAR"
string(3) "Foo"
string(3) "foo"
string(7) "Foo_Bar"
string(7) "foo_bAR"
string(3) "foo"
string(3) "Foo"
string(7) "foo_bar"
string(7) "foo_bAR"
