--TEST--
Check for Yaf_Loader and open_basedir
--SKIPIF--
<?php 
if (!extension_loaded("yaf")) {
   die("skip");
}
if (version_compare(PHP_VERSION, "5.3", "lt")) {
   die("skip open_basedir only can be tightened as of 5.3.0");
}
?>
--INI--
yaf.lowcase_path=0
yaf.use_spl_autoload=0
yaf.use_namespace=0
--FILE--
<?php
$dir = __DIR__;
$odir = $dir . "/foo";
file_put_contents($dir . "/Dummy.php", "");

ini_set("open_basedir",  $odir);
$loader = Yaf_Loader::getInstance($dir);
$loader->import($dir . "/Dummy.php");
$loader->autoload("Dummy");
?>
--CLEAN--
<?php
unlink(__DIR__ . "/Dummy.php");
?>
--EXPECTF--
Warning: Yaf_Loader::import(): open_basedir restriction in effect. File(%sDummy.php) is not within the allowed path(s): (%sfoo) in %s037.php on line %d

Warning: Yaf_Loader::import(%sDummy.php): failed to open stream: Operation not permitted in %s037.php on line %d

Warning: Yaf_Loader::import(): Failed opening '%sDummy.php' for inclusion (include_path='%s') in %s037.php on line %d

Warning: Yaf_Loader::autoload(): open_basedir restriction in effect. File(%sDummy.php) is not within the allowed path(s): (%sfoo) in %s037.php on line %d

Warning: Yaf_Loader::autoload(%sDummy.php): failed to open stream: Operation not permitted in %s037.php on line %d

Warning: Yaf_Loader::autoload(): Failed opening '%sDummy.php' for inclusion (include_path='%s') in %s037.php on line %d

Warning: Yaf_Loader::autoload(): Failed opening script %sDummy.php: Operation not permitted in %s037.php on line %d
