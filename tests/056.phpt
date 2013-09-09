--TEST--
check for Yaf_Dispatcher::throwException
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
Yaf_Dispatcher::getInstance()->throwException(true);
var_dump(Yaf_Dispatcher::getInstance()->throwException());
Yaf_Dispatcher::getInstance()->throwException(false);
var_dump(Yaf_Dispatcher::getInstance()->throwException());
?>
--EXPECTF--
bool(true)
bool(false)
