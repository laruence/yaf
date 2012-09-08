--TEST--
Memleaks in Yaf_Dispatcher::getInstance()
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
define("APPLICATION_PATH", dirname(__FILE__));
$app = new Yaf_Application(APPLICATION_PATH . '/simple.ini');
Yaf_Dispatcher::getInstance();
$a = Yaf_Dispatcher::getInstance();
unset($a);
Yaf_Dispatcher::getInstance();
$b = Yaf_Dispatcher::getInstance();
var_dump(get_class($b));
?>
--EXPECTF--
string(14) "Yaf_Dispatcher"
