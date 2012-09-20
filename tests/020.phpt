--TEST--
Check for Yaf_Application
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
--FILE--
<?php 
$config = array(
	"application" => array(
		"directory" => realpath(dirname(__FILE__)),
		"dispatcher" => array(
			"catchException" => 0,
			"throwException" => 0,
		),
	),
);

function error_handler($errno, $errstr, $errfile, $errline) {
	var_dump(Yaf_Application::app()->getLastErrorNo());
	var_dump(Yaf_Application::app()->getLastErrorMsg());
	Yaf_Application::app()->clearLastError();
	var_dump(Yaf_Application::app()->getLastErrorNo());
	var_dump(Yaf_Application::app()->getLastErrorMsg());
}

$app = new Yaf_Application($config);
$app->getDispatcher()->setErrorHandler("error_handler", E_RECOVERABLE_ERROR);
$app->run();
?>
--EXPECTF--
int(516)
string(%d) "Failed opening controller script %s: %s"
int(0)
string(0) ""
