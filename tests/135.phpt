--TEST--
Unknown route type in dispatcher.defaultRoute must fall back to static route
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
			"defaultRoute" => array("type" => "no-such-type"),
		),
	),
);

class IndexController extends Yaf_Controller_Abstract {
	public function indexAction() {
		echo "ok\n";
		return FALSE;
	}
}

$app = new Yaf_Application($config);
$app->getDispatcher()->disableView();
$request = new Yaf_Request_Http("/index/index");
$app->getDispatcher()->dispatch($request);
?>
--EXPECTF--
Warning: %sUnable to initialize default route, use Yaf_Route_Static instead in %s on line %d
ok
