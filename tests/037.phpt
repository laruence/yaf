--TEST--
Check for Yaf_Loader and open_basedir
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
open_basedir=.
yaf.lowcase_path=0
yaf.library=/tmp/
--FILE--
<?php
$loader = Yaf_Loader::getInstance("/tmp");
$loader->import("/tmp/1.php");
$loader->autoload("Foo_Bar");
?>
--EXPECTF--
Warning: Yaf_Loader::import(): open_basedir restriction in effect. File(/tmp/1.php) is not within the allowed path(s): (.) in %s037.php on line %d

Warning: Yaf_Loader::autoload(): open_basedir restriction in effect. File(/tmp/Foo/Bar.php) is not within the allowed path(s): (.) in %s037.php on line %d

Warning: Yaf_Loader::autoload(): Could not find script /tmp/Foo/Bar.php in %s037.php on line %d
