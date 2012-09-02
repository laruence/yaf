--TEST--
Check for segfault while use closure as error handler
--SKIPIF--
<?php
if (!extension_loaded("yaf")) print "skip";
if (!version_compare(PHP_VERSION, "5.3", "ge")) print "skip only for 5.3+";
?>
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
    protected function _initErrorHandler(Yaf_Dispatcher $dispatcher) {
        $dispatcher->setErrorHandler(function($errorCode, $errorMessage, $file, $line) {
            throw new ErrorException($errorMessage, 0, $errorCode, $file, $line);
        });
    }
}

class IndexController extends Yaf_Controller_Abstract {
    public function indexAction() {
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
string(33) "Undefined variable: undefined_var"
