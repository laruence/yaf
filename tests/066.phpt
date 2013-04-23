--TEST--
Check for Yaf_Route_Regex with abnormal map
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php 
$request = new Yaf_Request_Http("/subdir/ap/1.2/xxx/name/value", "/subdir");

$router = new Yaf_Router();

$router->addRoute("ap", new Yaf_Route_Regex(
	"#^/ap/([^/]*)/([^/]*)/*#i",
    array(
		"action" => 'ap',
	),
	array(
		1 => 23432,
        2 => NULL,
	)
))->route($request);

var_dump($router->getCurrentRoute());
var_dump($request->getParam(1));
var_dump($request->getActionName());
var_dump($request->getControllerName());
var_dump($request->getParam('name'));

?>
--EXPECTF--
string(2) "ap"
NULL
string(2) "ap"
NULL
NULL
