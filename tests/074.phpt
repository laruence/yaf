--TEST--
Check for Yaf_Route_Simple::assemble
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php

$router = new Yaf_Router();

$route  = new Yaf_Route_Simple('m', 'c', 'a');

$router->addRoute("simple", $route);

var_dump($router->getRoute('simple')->assemble(
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
--EXPECTF--
string(64) "?m=yafmodule&c=yafcontroller&a=yafaction&tkey1=tval1&tkey2=tval2"
