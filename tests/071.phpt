--TEST--
Check for Yaf_Route_Static::assemble
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--FILE--
<?php

$router = new Yaf_Router();

$route  = new Yaf_Route_Static();

$router->addRoute("static", $route);

var_dump($router->getRoute('static')->assemble(
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
			)
);
--EXPECTF--
string(59) "/yafmodule/yafcontroller/yafaction?tkey1=tval1&tkey2=tval2&"
