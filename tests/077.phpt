--TEST--
Check for Yaf_Route_Static::assemble
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php

$router = new Yaf_Router();

$route  = new Yaf_Route_Static();

$router->addRoute("static", $route);

var_dump($router->getRoute('static')->assemble(
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
		)
);

var_dump($router->getRoute('static')->assemble(
			array(
				':a' => 'yafaction',
				'tkey' => 'tval',
				':c' => 'yafcontroller',
				':m' => 'yafmodule'
	        ),
			array(
				1 => 2,
				array(),
		    )
		)
);
--EXPECTF--
string(%d) "/yafmodule/yafcontroller/yafaction?tkey1=tval1&tkey2=tval2"
string(%d) "/yafmodule/yafcontroller/yafaction"
