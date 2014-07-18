--TEST--
Check for Yaf_Route_Supervar::assemble
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.throw_exception=1
yaf.use_namespace=0
--FILE--
<?php

$router = new Yaf_Router();

$route  = new Yaf_Route_Supervar('r');

$router->addRoute("supervar", $route);

var_dump($router->getRoute('supervar')->assemble(
		array(
		      ':a' => 'yafaction', 
		      'tkey' => 'tval', 
		      ':c' => 'yafcontroller', 
		      ':m' => 'yafmodule'
		), 
		array(
		      'tkey1' => 'tval1', 
		      'tkey2' => 'tval2'
		)
));

try { 
var_dump($router->getRoute('supervar')->assemble(
		array(
		      ':a' => 'yafaction', 
		      'tkey' => 'tval', 
		      ':m' => 'yafmodule'
		), 
		array(
		      'tkey1' => 'tval1', 
		      'tkey2' => 'tval2',
			  1 => array(),
		)
));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
--EXPECTF--
string(%d) "?r=/yafmodule/yafcontroller/yafaction&tkey1=tval1&tkey2=tval2"
string(%d) "You need to specify the controller by ':c'"
