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
                array()
);

$router->addRoute("rewrite", $route);

var_dump($router->getRoute('rewrite')->assemble(
                        array(
                                ':name' => 'foo',
                                ':id' => 'bar',
                                ':tmpkey1' => 'tmpval1'
                        ),
                        array(
                                'tkey1' => 'tval1',
                                'tkey2' => 'tval2'
                             )
                        )
);

--EXPECTF--
string(57) "/product/foo/bar/tmpkey1/tmpval1/?tkey1=tval1&tkey2=tval2"
