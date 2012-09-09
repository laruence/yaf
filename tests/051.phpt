--TEST--
Fixed bug that segfault while a abnormal object set to Yaf_Route*::route
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
--FILE--
<?php
error_reporting(E_ALL & ~E_WARNING);
$x = new Yaf_Route_Map(true);
$x->route($x);

$x = new Yaf_Route_Static();
$x->route($x);

$x = new Yaf_Route_Rewrite("#^/test#", array("controller" => "info"), array());
$x->route($x);

$x = new Yaf_Route_Supervar("r");
$x->route($x);

$x = new Yaf_Route_Regex("#^/test#", array("controller" => "info"), array());
$x->route($x);

echo "okey";
?>
--EXPECTF--
okey
