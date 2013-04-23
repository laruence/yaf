--TEST--
Check for throw exception in Yaf_Controller::init
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php 
$config = array(
	"application" => array(
		"directory" => realpath(dirname(__FILE__)),
		"dispatcher" => array(
			"catchException" => 0,
			"throwException" => 1,
		),
        "modules" => "module",
	),
);

class ControllerController extends Yaf_Controller_Abstract {
    public function init() {
        throw new Yaf_Exception("exception");
    }

    public function indexAction() {
        echo "okey";
        return FALSE;
    }
}


$app = new Yaf_Application($config);
$request = new Yaf_Request_Http("/module/controller/index");

try {
  $app->getDispatcher()->returnResponse(false)->dispatch($request);
} catch (Yaf_Exception $e) {
  echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
exception
