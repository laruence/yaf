--TEST--
check for Yaf_Dispatcher::catchException
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
yaf.environ="product"
yaf.use_namespace=0
--FILE--
<?php
define("APPLICATION_PATH", dirname(__FILE__));
$app = new Yaf_Application(APPLICATION_PATH . '/simple.ini');
Yaf_Dispatcher::getInstance()->catchException(true);
var_dump(Yaf_Dispatcher::getInstance()->catchException());
Yaf_Dispatcher::getInstance()->catchException(false);
var_dump(Yaf_Dispatcher::getInstance()->catchException());
?>
--EXPECTF--
bool(true)
bool(false)
