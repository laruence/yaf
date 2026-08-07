--TEST--
Routing with an empty base_uri should not read out of bounds
--SKIPIF--
<?php
if (!extension_loaded("yaf")) print "skip";
?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
/* base_uri '/' gets sanitized to an empty string, uri has no leading slash */
$request = new Yaf_Request_Http('foo/bar', '/');
var_dump($request->base_uri);

$route = new Yaf_Route_Map(true);
var_dump($route->route($request));
var_dump($request->getControllerName());
?>
--EXPECT--
string(0) ""
bool(true)
string(7) "Foo_Bar"
