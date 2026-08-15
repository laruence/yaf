--TEST--
yaf.forward_limit=0 must not cause an infinite dispatch loop
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
yaf.forward_limit=0
--FILE--
<?php
$config = array(
	"application" => array(
		"directory" => realpath(dirname(__FILE__)),
		"dispatcher" => array(
			"catchException" => 0,
			"throwException" => 1,
		),
	),
);

class ForwardController extends Yaf_Controller_Abstract {
	public function indexAction() {
		$this->forward("index");
		return FALSE;
	}
}

$app = new Yaf_Application($config);
$request = new Yaf_Request_Http("/forward/index");

try {
	$app->getDispatcher()->dispatch($request);
	echo "no error\n";
} catch (Yaf_Exception $e) {
	echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
The maximum dispatching count 5 is reached
