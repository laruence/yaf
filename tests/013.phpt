--TEST--
Check for Yaf_Router and Config Routes
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php 
$file = dirname(__FILE__) . "/simple.ini";

$config = new Yaf_Config_Ini($file, 'extra');

$routes = $config->routes;
print_r($routes);

$router = new Yaf_Router();
$router->addConfig($routes);

print_r($router->getRoutes());
?>
--EXPECTF--
Yaf_Config_Ini Object
(
    [readonly:protected] => 1
    [config:protected] => Array
        (
            [regex] => Array
                (
                    [type] => regex
                    [match] => ^/ap/(.*)
                    [route] => Array
                        (
                            [controller] => Index
                            [action] => action
                        )

                    [map] => Array
                        (
                            [0] => name
                            [1] => name
                            [2] => value
                        )

                )

            [simple] => Array
                (
                    [type] => simple
                    [controller] => c
                    [module] => m
                    [action] => a
                )

            [supervar] => Array
                (
                    [type] => supervar
                    [varname] => c
                )

            [rewrite] => Array
                (
                    [type] => rewrite
                    [match] => /yaf/:name/:value
                    [route] => Array
                        (
                            [controller] => Index
                            [action] => action
                        )

                )

        )

)
Array
(
    [_default] => Yaf_Route_Static Object
        (
        )

    [regex] => Yaf_Route_Regex Object
        (
            [match:protected] => ^/ap/(.*)
            [route:protected] => Array
                (
                    [controller] => Index
                    [action] => action
                )

            [map:protected] => Array
                (
                    [0] => name
                    [1] => name
                    [2] => value
                )

            [verify:protected] => 
            [reverse:protected] => 
        )

    [simple] => Yaf_Route_Simple Object
        (
            [module:protected] => a
            [controller:protected] => c
            [action:protected] => a
        )

    [supervar] => Yaf_Route_Supervar Object
        (
            [varname:protected] => c
        )

    [rewrite] => Yaf_Route_Rewrite Object
        (
            [match:protected] => /yaf/:name/:value
            [route:protected] => Array
                (
                    [controller] => Index
                    [action] => action
                )

            [verify:protected] => 
        )

)
