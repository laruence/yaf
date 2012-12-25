--TEST--
Check for Yaf_Route_Regex::assemble
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--FILE--
<?php

$router = new Yaf_Router();

$route  = new Yaf_Route_Regex(
                "#^/product/([^/]+)/([^/])+#",
                array(
                        'controller' => "product",  //route to product controller,
                ),
                array(),
                '/%m/%c/%a'
);

$router->addRoute("regex", $route);

var_dump($router->getRoute('regex')->assemble(
                        array(
                                'module' => 'module',
                                'controller' => 'controller',
                                'action' => 'action'
                        ),
                        array(
                                'tkey1' => 'tval1',
                                'tkey2' => 'tval2'
                             )
                        )
);
--EXPECTF--
string(49) "/module/controller/action?tkey1=tval1&tkey2=tval2"
