--TEST--
Yaf_Dispatcher::getInstance() before Yaf_Application should throw instead of segfault
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
try {
	Yaf_Dispatcher::getInstance();
	echo "no exception\n";
} catch (Exception $e) {
	echo "caught: ", get_class($e), ": ", $e->getMessage(), "\n";
}

$app = new Yaf_Application(array(
	"application" => array("directory" => realpath(dirname(__FILE__))),
));
var_dump(Yaf_Dispatcher::getInstance() instanceof Yaf_Dispatcher);
?>
--EXPECTF--
caught: Exception: Yaf_Application must be initialized before Yaf_Dispatcher::getInstance()
bool(true)
