--TEST--
forward() in init() must go through the dispatch loop, not unbounded recursion
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

class GateController extends Yaf_Controller_Abstract {
	public function init() {
		$this->forward("Target", "index");
	}
	public function indexAction() {
		echo "should not reach here\n";
		return FALSE;
	}
}

class TargetController extends Yaf_Controller_Abstract {
	public function indexAction() {
		echo "target\n";
		return FALSE;
	}
}

class RecurseController extends Yaf_Controller_Abstract {
	public function init() {
		$this->forward("Recurse", "index");
	}
	public function indexAction() {
		return FALSE;
	}
}

$app = new Yaf_Application($config);
$app->getDispatcher()->disableView();

$request = new Yaf_Request_Http("/gate/index");
$app->getDispatcher()->dispatch($request);

$request = new Yaf_Request_Http("/recurse/index");
try {
	$app->getDispatcher()->dispatch($request);
	echo "no error\n";
} catch (Yaf_Exception $e) {
	echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
target
The maximum dispatching count 5 is reached
