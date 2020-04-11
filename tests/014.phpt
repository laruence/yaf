--TEST--
Check for Yaf_Application
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.environ="product"
yaf.use_namespace=0
--FILE--
<?php 
define("APPLICATION_PATH", dirname(__FILE__));
$app = new Yaf_Application(
	dirname(__FILE__) . "/simple.ini",
	'product');
print_r($app);
?>
--EXPECTF--
Yaf_Application Object
(
    [directory] => %stests%capplcation
    [library] => 
    [bootstrap] => 
    [ext] => php
    [view_ext] => phtml
    [environ:protected] => product
    [running:protected] => 
    [err_msg:protected] => 
    [err_no:protected] => 0
    [config:protected] => Yaf_Config_Ini Object
        (
%A
        )

    [dispatcher:protected] => Yaf_Dispatcher Object
        (
            [auto_render:protected] => 1
            [instant_flush:protected] => 
            [return_response:protected] => 
            [request:protected] => Yaf_Request_Http Object
                (
                    [method] => CLI
                    [module] => 
                    [controller] => 
                    [action] => 
                    [uri:protected] => 
                    [base_uri:protected] => 
                    [dispatched:protected] => 
                    [routed:protected] => 
                    [language:protected] => 
                    [params:protected] => Array
                        (
                        )

                )

            [response:protected] => 
            [router:protected] => Yaf_Router Object
                (
                    [routes:protected] => Array
                        (
                            [_default] => Yaf_Route_Static Object
                                (
                                )

                        )

                    [current:protected] => 
                )

            [view:protected] => 
            [plugins:protected] => Array
                (
                )

        )

    [modules:protected] => Array
        (
            [0] => Index
        )

    [default_route:protected] => 
)
