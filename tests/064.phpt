--TEST--
Check for Yaf_Route_Regex with dynamic mvc
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php 
$request = new Yaf_Request_Http("/subdir/ctl/act/name/value");

$router = new Yaf_Router();

$route  = new Yaf_Route_Regex(
	"#subdir/(?<c>.*?)/(.*?)/.*#",
	array(
        "module" => "m",
        "controller" => ":c",
		"action" => ":a",
	),
    array(
        2 => "a",
    )
);

$router->addRoute("subdir", $route)->addRoute("yaf", new Yaf_Route_Regex(
	"#yaf/(.*?)/.*#",
    array(
		"action" => ':action',
        "controller" => "index",
	),
    array(
        1 => "action",
    )
))->route($request);

var_dump($router->getCurrentRoute());
print_r($request->getParams());
var_dump($request->getActionName());
var_dump($request->getControllerName());
var_dump($request->getModuleName());

$request = new Yaf_Request_Http("/yaf/act/name/value");
$router->route($request);

var_dump($router->getCurrentRoute());
print_r($request->getParams());
var_dump($request->getActionName());
var_dump($request->getControllerName());
var_dump($request->getModuleName());

?>
--EXPECTF--
string(6) "subdir"
Array
(
    [c] => ctl
    [a] => act
)
string(3) "act"
string(3) "ctl"
string(1) "m"
string(3) "yaf"
Array
(
    [action] => act
)
string(3) "act"
string(5) "index"
NULL
