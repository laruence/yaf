--TEST--
check for Yaf_Dispatcher::autoRender
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
Yaf_Dispatcher::getInstance()->autoRender(true);
var_dump(Yaf_Dispatcher::getInstance()->autoRender());
Yaf_Dispatcher::getInstance()->autoRender(false);
var_dump(Yaf_Dispatcher::getInstance()->autoRender());
?>
--EXPECTF--
bool(true)
bool(false)
