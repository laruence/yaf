--TEST--
ISSUE #134 (Segfault while calling assemble)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$router = new Yaf_Router();
$route  = new Yaf_Route_Rewrite(
	"/detail/:id",
	array(
		'controller' => 'index',
		'action' => 'detail',
		'module' => 'kfc'
	)
);
$router->addRoute("kfc/index/detail", $route);

print_r($router->getRoute('kfc/index/detail')->assemble(
	array(
		':id' => '1',
	)
));
--EXPECTF--
/detail/1
