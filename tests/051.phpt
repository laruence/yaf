--TEST--
Check for Yaf_Route_Simple::assemble
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--FILE--
<?php

$router = new Yaf_Router();

$route  = new Yaf_Route_Simple('m', 'c', 'a');

$router->addRoute("simple", $route);

var_dump($router->getRoute('simple')->assemble(
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
string(64) "?m=yafmodule&c=yafcontroller&a=yafaction&tkey1=tval1&tkey2=tval2"