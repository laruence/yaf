--TEST--
Bug (mem leak and crash in Yaf_Config_Ini)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php 
$config = new Yaf_Config_Ini;

var_dump($config->get("\0"));
?>
--EXPECTF--
Warning: Yaf_Config_Ini::__construct() expects at least 1 parameter, 0 given in %s on line %d
NULL
