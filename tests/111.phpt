--TEST--
Missing required action argument should raise ArgumentCountError
--SKIPIF--
<?php
if (!extension_loaded("yaf")) print "skip";
if (PHP_VERSION_ID < 70100) print "skip ArgumentCountError is not available before PHP 7.1";
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
			"throwException" => 1,
		),
	),
);

class UserController extends Yaf_Controller_Abstract {
	public function viewAction($id) {
		echo "id=", $id, "\n";
		return FALSE;
	}

	public function listAction($page, $size) {
		echo "page=", $page, " size=", $size, "\n";
		return FALSE;
	}

	public function optAction($id, $format = 'html') {
		echo "id=", $id, " format=", $format, "\n";
		return FALSE;
	}
}

$app = new Yaf_Application($config);
$dispatcher = $app->getDispatcher()->returnResponse(false);

/* required argument provided */
$dispatcher->dispatch(new Yaf_Request_Http("/user/view/id/123"));

/* no parameters at all */
try {
	$dispatcher->dispatch(new Yaf_Request_Http("/user/view"));
	echo "unexpected: dispatched\n";
} catch (ArgumentCountError $e) {
	echo $e->getMessage(), "\n";
}

/* required argument missing while later ones are provided,
   parameters must not be shifted positionally */
try {
	$dispatcher->dispatch(new Yaf_Request_Http("/user/list/size/10"));
	echo "unexpected: dispatched\n";
} catch (ArgumentCountError $e) {
	echo $e->getMessage(), "\n";
}

/* all required arguments provided */
$dispatcher->dispatch(new Yaf_Request_Http("/user/list/page/2/size/10"));

/* required argument provided, optional one falls back to its default */
$dispatcher->dispatch(new Yaf_Request_Http("/user/opt/id/5"));
?>
--EXPECT--
id=123
Too few arguments to function UserController::viewAction(), 0 passed and exactly 1 expected
Too few arguments to function UserController::listAction(), 0 passed and exactly 2 expected
page=2 size=10
id=5 format=html
