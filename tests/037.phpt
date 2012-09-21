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
Warning: Yaf_Loader::import(): open_basedir restriction in effect. File(%stmp%c1.php) is not within the allowed path(s): (.) in %s037.php on line %d

Warning: Yaf_Loader::autoload(): open_basedir restriction in effect. File(%stmp%cFoo%cBar.php) is not within the allowed path(s): (.) in %s037.php on line %d

Warning: Yaf_Loader::autoload(): Failed opening script %stmp%cFoo%cBar.php: Operation not permitted in %s037.php on line %d
