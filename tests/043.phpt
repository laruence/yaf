--TEST--
Check for yaf.system settings
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.library="/tmp"
yaf.use_spl_autoload=1
yaf.name_suffix=1
yaf.name_separator=""
yaf.lowcase_path=1
yaf.environ="product"
yaf.use_namespace=0
--FILE--
<?php
define("APPLICATION_PATH", dirname(__FILE__));

$app = new Yaf_Application(APPLICATION_PATH . '/system.ini');
var_dump(ini_get("yaf.library") == APPLICATION_PATH);
var_dump(ini_get("yaf.open_basedir") == "");
var_dump(ini_get("yaf.use_spl_autoload") == "0");
//yaf.cache_config is INI_SYSTEM, can not be overridden
var_dump(ini_get("yaf.cache_config") == "0");
var_dump(ini_get("yaf.name_suffix") == "1");
var_dump(ini_get("yaf.name_separator") == "__");

new Plugin__ABC_EFG();
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)

Warning: Yaf_Loader::autoload(): Failed opening script %sapplcation%cplugins%cABC%cEFG.php: No such file or directory in %s043.php on line %d

Fatal error: Uncaught Error: Class 'Plugin__ABC_EFG' not found in %s043.php:%d
%a
