--TEST--
Check for yaf.forward_limit
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
yaf.forward_limit = 10;
--FILE--
<?php 
$config = array(
	"application" => array(
		"directory" => realpath(dirname(__FILE__)),
        "modules" => "module",
	),
);

class ControllerController extends Yaf_Controller_Abstract {
    public function actionAction($from = NULL) {
		if (!$from) {
			print "starting\n";
		} else {
			var_dump($from);
		}
		$this->forward("module", "controller", "index", array("from" => __METHOD__));
		return false;
    }

    public function indexAction($from) {
		var_dump($from);
        $this->forward("dummy", array("from" => __METHOD__));
		return false;
    }

    public function dummyAction($from) {
		var_dump($from);
		$this->forward("controller", "action", array("from" => __METHOD__));
		return false;
    }
}


$app = new Yaf_Application($config);
$request = new Yaf_Request_Http("/module/controller/action");

try {
  $app->getDispatcher()->returnResponse(false)->dispatch($request);
} catch (Yaf_Exception $e) {
  echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
starting
string(34) "ControllerController::actionAction"
string(33) "ControllerController::indexAction"
string(33) "ControllerController::dummyAction"
string(34) "ControllerController::actionAction"
string(33) "ControllerController::indexAction"
string(33) "ControllerController::dummyAction"
string(34) "ControllerController::actionAction"
string(33) "ControllerController::indexAction"
string(33) "ControllerController::dummyAction"
The maximum dispatching count 10 is reached
