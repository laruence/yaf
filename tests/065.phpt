--TEST--
Yaf_Route_Regex map is optional
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php 

$request = new Yaf_Request_Http("/subdir/ap/1.2/name/value", "/subdir");

$router = new Yaf_Router();

$router->addConfig(
 array(
    array(
    "type" => "regex",
    "match" => "#^/ap/([^/]*)/*#i",
    "route" => array(
        array(
            "action" => 'ap',
            ),
        ),
  ) 
 )
)->route($request);

var_dump($router->getCurrentRoute());
var_dump($request->getActionName());


$router->addRoute("regex", new Yaf_Route_Regex("#^/ap/([^/]*)/*#i", array("action" => "ap")))->route($request);

var_dump($router->getCurrentRoute());
var_dump($request->getActionName());

?>
--EXPECT--
int(0)
NULL
string(5) "regex"
string(2) "ap"
