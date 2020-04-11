--TEST--
Check for Yaf_Bootstrap protected method
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
    protected function _initErrorHandler(Yaf_Dispatcher $dispatcher) {
		echo "Bad";
    }
}

class IndexController extends Yaf_Controller_Abstract {
    public function indexAction() {
		echo "Okey";
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
Warning: %scannot call protected method Bootstrap::_initErrorHandler() in %s098.php on line %d
Okey
