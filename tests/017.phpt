--TEST--
Bug (mem leak and crash in Yaf_Config_Ini)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip";
if (version_compare(PHP_VERSION, '7.1.0') <= 0) {
   print "skip PHP 7.0 strict_types __construct() output warning";
}
?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
declare(strict_types=1);
$config = new Yaf_Config_Ini;

var_dump($config->get("\0"));
?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: Yaf_Config_Ini::__construct() expects at least 1%s, 0 given in %s017.php:%d
Stack trace:
#0 %s017.php(%d): Yaf_Config_Ini->__construct()
#1 {main}
  thrown in %s017.php on line %d