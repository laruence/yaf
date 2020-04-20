--TEST--
Check for Yaf_Router basic usages
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php

$router = new Yaf_Router();

$route  = new Yaf_Route_Simple('m', 'c', 'a');
$sroute = new Yaf_Route_Supervar('r');

$router->addRoute("simple", $route)->addRoute("super", $sroute);
print_r($router);
var_dump($router->getCurrentRoute());
print_r($router->getRoutes());
print_r($router->getRoute("simple"));
var_dump($router->getRoute("noexists"));
?>
--EXPECTF--
Yaf_Router Object
(
    [routes:protected] => Array
        (
            [_default] => Yaf_Route_Static Object
                (
                )

            [simple] => Yaf_Route_Simple Object
                (
                    [module:protected] => m
                    [controller:protected] => c
                    [action:protected] => a
                )

            [super] => Yaf_Route_Supervar Object
                (
                    [varname:protected] => r
                )

        )

    [current:protected] => 
)
NULL
Array
(
    [_default] => Yaf_Route_Static Object
        (
        )

    [simple] => Yaf_Route_Simple Object
        (
            [module:protected] => m
            [controller:protected] => c
            [action:protected] => a
        )

    [super] => Yaf_Route_Supervar Object
        (
            [varname:protected] => r
        )

)
Yaf_Route_Simple Object
(
    [module:protected] => m
    [controller:protected] => c
    [action:protected] => a
)
NULL
