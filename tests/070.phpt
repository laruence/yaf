--TEST--
Check for Yaf_Route_Supervar::assemble
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--FILE--
<?php

$router = new Yaf_Router();

$route  = new Yaf_Route_Supervar('r');

$router->addRoute("supervar", $route);

var_dump($router->getRoute('supervar')->assemble(
		array(
		      'action' => 'yafaction', 
		      'tkey' => 'tval', 
		      'controller' => 'yafcontroller', 
		      'module' => 'yafmodule'
		), 
		array(
		      'tkey1' => 'tval1', 
		      'tkey2' => 'tval2'
		)
));
--EXPECTF--
string(61) "?r=/yafmodule/yafcontroller/yafaction&tkey1=tval1&tkey2=tval2"
