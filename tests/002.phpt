--TEST--
Check for Yaf_Request_Simple
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
report_memleaks=0
--FILE--
<?php 
$request  = new Yaf_Request_Simple("CLI", "index", "dummy", "index");
print_r($request);
print_r($request->setParam("name", "Laruence"));
var_dump($request->isCli());
var_dump($request->getParam("name"));
var_dump($request->getParam("notexists"));

$app =  new Yaf_Application(array("application" => array(
            "directory" => dirname(__FILE__),
            )));

try {
$app->getDispatcher()->dispatch($request);
} catch (Yaf_Exception_LoadFailed_Controller $e) {
   print $e->getMessage();
}

?>
--EXPECTF--
Yaf_Request_Simple Object
(
    [module] => index
    [controller] => dummy
    [action] => index
    [method] => CLI
    [params:protected] => Array
        (
        )

    [language:protected] => 
    [_exception:protected] => 
    [_base_uri:protected] => 
    [uri:protected] => 
    [dispatched:protected] => 
    [routed:protected] => 1
)
Yaf_Request_Simple Object
(
    [module] => index
    [controller] => dummy
    [action] => index
    [method] => CLI
    [params:protected] => Array
        (
            [name] => Laruence
        )

    [language:protected] => 
    [_exception:protected] => 
    [_base_uri:protected] => 
    [uri:protected] => 
    [dispatched:protected] => 
    [routed:protected] => 1
)
bool(true)
string(8) "Laruence"
NULL
Could not find controller script %scontrollers%sDummy.php
