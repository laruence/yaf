--TEST--
Check for Yaf_Loader and open_basedir
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
open_basedir=.
yaf.lowcase_path=0
--FILE--
<?php
$loader = Yaf_Loader::getInstance("/tmp");
$loader->import('/tmp/no_exists_037.php');
$loader->autoload("Foo_Bar307");
?>
--EXPECTF--
Warning: Yaf_Loader::autoload(): Failed opening script %sBar307.php: No such file or directory in %s037.php on line %d
