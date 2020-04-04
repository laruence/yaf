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
    [err_no:protected] => 
    [config:protected] => Yaf_Config_Ini Object
        (
%A
        )

    [dispatcher:protected] => Yaf_Dispatcher Object
        (
        )

    [modules:protected] => Array
        (
            [0] => Index
        )

    [default_route:protected] => 
)
