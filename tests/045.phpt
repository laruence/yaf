--TEST--
Check for segfault while use closure as error handler
--SKIPIF--
<?php
if (!extension_loaded("yaf")) print "skip";
if (!version_compare(PHP_VERSION, "5.3", "ge")) print "skip only for 5.3+";
?>
--INI--
yaf.use_namespace=0
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

class Bootstrap extends Yaf_Bootstrap_Abstract {
    public function _initErrorHandler(Yaf_Dispatcher $dispatcher) {
		var_dump($dispatcher->getDefaultModule());
		var_dump($dispatcher->getDefaultController());
		$dispatcher->setDefaultAction("dummy");
		var_dump($dispatcher->getDefaultAction());
        $dispatcher->setErrorHandler(function($errorCode, $errorMessage, $file, $line) {
            throw new ErrorException($errorMessage, 0, $errorCode, $file, $line);
        });
    }
}

class IndexController extends Yaf_Controller_Abstract {
    public function dummyAction() {
        echo  $undefined_var;
        return FALSE;
    }
}

$app = new Yaf_Application($config);
try {
    $app->bootstrap()->run();
} catch (Exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECTF--
string(5) "Index"
string(5) "Index"
string(5) "dummy"
string(33) "Undefined variable: undefined_var"
