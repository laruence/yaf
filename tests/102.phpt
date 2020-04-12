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
    public function actionAction() {
		var_dump(__METHOD__);
		$this->forward("module", "controller", "index");
		return false;
    }

    public function indexAction() {
		var_dump(__METHOD__);
        $this->forward("dummy");
		return false;
    }

    public function dummyAction() {
		var_dump(__METHOD__);
		$this->forward("controller", "action");
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
string(34) "ControllerController::actionAction"
string(33) "ControllerController::indexAction"
string(33) "ControllerController::dummyAction"
string(34) "ControllerController::actionAction"
string(33) "ControllerController::indexAction"
string(33) "ControllerController::dummyAction"
string(34) "ControllerController::actionAction"
string(33) "ControllerController::indexAction"
string(33) "ControllerController::dummyAction"
string(34) "ControllerController::actionAction"
The maximum dispatching count 10 is reached
