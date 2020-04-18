--TEST--
Check for Yaf_Dispatcher::dispatch() of errors while loading executor
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
yaf.use_spl_autoload=0
--FILE--
<?php 
require "build.inc";
startup();

$config = array(
	"application" => array(
		"directory" => APPLICATION_PATH,
	),
);
$app = new Yaf_Application($config);
$dispatcher = $app->getDispatcher();

$request = new Yaf_Request_Simple();
set_error_handler(function ($no, $msg) {
	var_dump(substr($msg, 0, 100));
});
$request->setControllerName(str_repeat("Index", 1024 * 1024));

try {
	$dispatcher->dispatch($request);
} catch (Exception $e) {
	var_dump(substr($e->getMessage(), 0, 100));
}
$request->setControllerName("index");

try {
	$dispatcher->dispatch($request);
} catch (Exception $e) {
	var_dump($e->getMessage());
}

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", "xxx");
try {
	$dispatcher->dispatch($request);
} catch (Exception $e) {
	var_dump($e->getMessage());
}

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
class IndexController  {
    public function indexAction() {
    }    
}
PHP
);
try {
	$dispatcher->dispatch($request);
} catch (Exception $e) {
	var_dump($e->getMessage());
}

$request->setControllerName("dummy");
file_put_contents(APPLICATION_PATH . "/controllers/Dummy.php", <<<PHP
<?php
class DummyController extends Yaf_Controller_Abstract {
}
PHP
);
try {
	$dispatcher->dispatch($request);
} catch (Exception $e) {
	var_dump($e->getMessage());
}

$request->setControllerName("dummy1");
file_put_contents(APPLICATION_PATH . "/controllers/Dummy1.php", <<<PHP
<?php
class Dummy1Controller extends Yaf_Controller_Abstract {
	public \$actions;
}
PHP
);
try {
	$dispatcher->dispatch($request);
} catch (Exception $e) {
	var_dump($e->getMessage());
}

$request->setControllerName("dummy2");
file_put_contents(APPLICATION_PATH . "/controllers/Dummy2.php", <<<PHP
<?php
class Dummy2Controller extends Yaf_Controller_Abstract {
	public \$actions = array();
}
PHP
);
try {
	$dispatcher->dispatch($request);
} catch (Exception $e) {
	var_dump($e->getMessage());
}

$request->setControllerName("dummy3");
file_put_contents(APPLICATION_PATH . "/controllers/Dummy3.php", <<<PHP
<?php
class Dummy3Controller extends Yaf_Controller_Abstract {
	public \$actions = array("index" => NULL);
}
PHP
);
try {
	$dispatcher->dispatch($request);
} catch (Exception $e) {
	var_dump($e->getMessage());
}

$request->setControllerName("dummy4");
file_put_contents(APPLICATION_PATH . "/controllers/Dummy4.php", <<<PHP
<?php
class Dummy4Controller extends Yaf_Controller_Abstract {
	public \$actions = array(
		"index" => "actions/index.php",
		"foo" => "actions/foo.php",
	);
}
PHP
);
try {
	$dispatcher->dispatch($request);
} catch (Exception $e) {
	var_dump($e->getMessage());
}

file_put_contents(APPLICATION_PATH . "/actions/index.php", <<<PHP
<?php
class IndexAction {
}
PHP
);
try {
	$dispatcher->dispatch($request);
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	$dispatcher->dispatch($request);
} catch (Exception $e) {
	var_dump($e->getMessage());
}

$request->setActionName("foo");
file_put_contents(APPLICATION_PATH . "/actions/foo.php", <<<PHP
<?php
class FooAction extends Yaf_Action_Abstract {
}
PHP
);
$dispatcher->dispatch($request);
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECTF--
string(100) "path too long while loading 'Indexindexindexindexindexindexindexindexindexindexindexindexindexindexi"
string(%d) "Failed opening controller script %sIndex.php: No such file or directory"
xxxstring(%d) "Could not find class IndexController in controller script %sIndex.php"
string(73) "Controller 'IndexController' is not a subclass of Yaf_Controller_Abstract"
string(49) "There is no method indexAction in DummyController"
string(50) "There is no method indexAction in Dummy1Controller"
string(60) "There is no method indexAction in Dummy2Controller::$actions"
string(70) "Action 'index' in Dummy3Controller::actions does not have a valid path"
string(%s) "Failed opening action script %sindex.php: No such file or directory"
string(61) "Action 'IndexAction' is not a subclass of Yaf_Action_Abstract"
string(61) "Action 'IndexAction' is not a subclass of Yaf_Action_Abstract"

Fatal error: Class FooAction contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (Yaf_Action_Abstract::execute) in %sfoo.php on line %d
