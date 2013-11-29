--TEST--
Check for Yaf_Application::environ()
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.environ="product"
yaf.use_namespace=0
--FILE--
<?php 
define("APPLICATION_PATH", dirname(__FILE__));
$app = new Yaf_Application(
    dirname(__FILE__) . "/simple.ini",
    'nocatch');
var_dump($app->environ());
?>
--EXPECTF--
string(7) "nocatch"