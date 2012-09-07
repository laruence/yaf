--TEST--
Check for Yaf_Application
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
--FILE--
<?php 
define("APPLICATION_PATH", dirname(__FILE__));
$app = new Yaf_Application(
	dirname(__FILE__) . "/simple.ini",
	'nocatch');

function error_handler($errno, $errstr, $errfile, $errline) {
	echo "error occurrd instead of exception threw";
}

$app->getDispatcher()->setErrorHandler("error_handler", E_RECOVERABLE_ERROR);
$app->run();
?>
--EXPECTF--
error occurrd instead of exception threw
