--TEST--
Check for Yaf_Route_Static routing
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php 
$request_uri = "/prefix/controller/action/name/laruence/age/28";
$base_uri	 = "/prefix/";

$request = new Yaf_Request_Http($request_uri, $base_uri);

unset($base_uri);
unset($request_uri);

$route = new Yaf_Route_Static();

var_dump($route->route($request));

print_r($request);
?>
--EXPECTF--
bool(true)
Yaf_Request_Http Object
(
    [method] => CLI
    [module] => 
    [controller] => Controller
    [action] => action
    [uri:protected] => /prefix/controller/action/name/laruence/age/28
    [base_uri:protected] => /prefix
    [dispatched:protected] => 
    [routed:protected] => 
    [language:protected] => 
    [params:protected] => Array
        (
            [name] => laruence
            [age] => 28
        )

)
