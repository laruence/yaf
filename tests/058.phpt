--TEST--
check for Yaf_Dispatcher::flushInstantly
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
--FILE--
<?php
define("APPLICATION_PATH", dirname(__FILE__));
$app = new Yaf_Application(APPLICATION_PATH . '/simple.ini');
Yaf_Dispatcher::getInstance()->flushInstantly(true);
var_dump(Yaf_Dispatcher::getInstance()->flushInstantly());
Yaf_Dispatcher::getInstance()->flushInstantly(false);
var_dump(Yaf_Dispatcher::getInstance()->flushInstantly());
?>
--EXPECTF--
bool(true)
bool(false)
