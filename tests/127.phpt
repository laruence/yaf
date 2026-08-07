--TEST--
Yaf_Request_Abstract::setParam(array) must return the request object on success
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$request = new Yaf_Request_Http("/index");

/* array form: yaf_request_set_params_multi() */
var_dump($request->setParam(array("a" => 1, "b" => 2)) === $request);
var_dump($request->getParam("a"));
var_dump($request->getParam("b"));

/* scalar form for comparison: yaf_request_set_params_single() */
var_dump($request->setParam("c", 3) === $request);
var_dump($request->getParam("c"));
?>
--EXPECT--
bool(true)
int(1)
int(2)
bool(true)
int(3)
