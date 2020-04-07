--TEST--
ISSUE #231 (php-fpm worker core dump BUG)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
variables_order=EPS
yaf.lowcase_path=0
yaf.throw_exception=0
yaf.catch_exception=1
yaf.use_namespace=0
--POST--
SCRIPT_FILENAME=issue231.php
--FILE--
<?php 
$request = new Yaf_Request_Http(dirname(__FILE__));
print_r($request);
$request = new Yaf_Request_Http(dirname(__FILE__), dirname(dirname(__FILE__)));
print_r($request);
?>
--EXPECTF--
Yaf_Request_Http Object
(
    [method] => POST
    [module] => 
    [controller] => 
    [action] => 
    [uri:protected] => %syaf%ctests
    [base_uri:protected] => 
    [dispatched:protected] => 
    [routed:protected] => 
    [language:protected] => 
    [params:protected] => Array
        (
        )

)
Yaf_Request_Http Object
(
    [method] => POST
    [module] => 
    [controller] => 
    [action] => 
    [uri:protected] => %syaf%ctests
    [base_uri:protected] => %syaf
    [dispatched:protected] => 
    [routed:protected] => 
    [language:protected] => 
    [params:protected] => Array
        (
        )

)
