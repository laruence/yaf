--TEST--
Yaf_Router::getCurrent with number key
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
    "map" => array(
        1 => 'version',
    )
  ) 
 )
)->route($request);

var_dump($router->getCurrentRoute());
var_dump($request->getParam("version"));

?>
--EXPECT--
int(0)
string(3) "1.2"
