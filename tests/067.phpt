--TEST--
Check for Yaf_Route_Rewrite::assemble
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--FILE--
<?php

$router = new Yaf_Router();

$route  = new Yaf_Route_Rewrite(
                "/product/:name/:id/*",
                array(
                        'controller' => "product",
                ),
                '/%m/%c/%a'
);

$router->addRoute("rewrite", $route);

var_dump($router->getRoute('rewrite')->assemble(
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
