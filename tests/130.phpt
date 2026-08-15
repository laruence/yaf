--TEST--
Dispatching a protected action should warn instead of corrupting the heap
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$config = array(
	"application" => array(
		"directory" => realpath(dirname(__FILE__)),
	),
);

class ProtectedController extends Yaf_Controller_Abstract {
	protected function indexAction() {
	}
}

$app = new Yaf_Application($config);
$request = new Yaf_Request_Http("/protected/index");
$app->getDispatcher()->disableView();

try {
	$app->getDispatcher()->dispatch($request);
} catch (Exception $e) {
	echo "caught: ", get_class($e), "\n";
}
echo "DONE\n";
?>
--EXPECTF--
Warning: %scannot call protected method ProtectedController::indexAction() in %s on line %d
DONE
