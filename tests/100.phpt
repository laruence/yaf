--TEST--
Check for error conditions of Yaf_Application::constructor
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
yaf.use_namespace=0
--FILE--
<?php
require "build.inc";
startup();

try {
	$app = new Yaf_Application(fopen(__FILE__, "r"));
} catch (Exception $e) {
	var_dump($e->getMessage());	
}
var_dump(isset($app));

try {
	$app = new Yaf_Application(array(), "r");
} catch (Exception $e) {
	var_dump($e->getMessage());	
}
var_dump(isset($app));

try {
	$app = new Yaf_Application(array("yaf" => 1), "r");
} catch (Exception $e) {
	var_dump($e->getMessage());	
}
var_dump(isset($app));

try {
	$app = new Yaf_Application(array("yaf" => array()), "r");
} catch (Exception $e) {
	var_dump($e->getMessage());	
}
var_dump(isset($app));

try {
	$app = new Yaf_Application(array("yaf" => array("directory" => __DIR__)), fopen(__FILE__, 'r'));
} catch (Error $e) {
	var_dump($e->getMessage());	
}
var_dump(isset($app));

$app = new Yaf_Application(array("yaf" => array("directory" => APPLICATION_PATH)));

try {
	$double = new Yaf_Application(array("yaf" => array("directory" => __DIR__)));
} catch (Exception $e) {
	var_dump($e->getMessage());	
}
var_dump(isset($double));

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         public function init() {
			Yaf_dispatcher::getInstance()->getApplication()->run();
         }
   }
PHP
);

try {
	$app->run();
} catch (Exception $e) {
    var_dump($e->getMessage());
}
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECT--
string(43) "Initialization of application config failed"
bool(false)
string(46) "Expected an array of application configuration"
bool(false)
string(46) "Expected an array of application configuration"
bool(false)
string(55) "Expected 'directory' entry in application configuration"
bool(false)
string(79) "Yaf_Application::__construct() expects parameter 2 to be string, resource given"
bool(false)
string(39) "Only one application can be initialized"
bool(false)
string(30) "Application is already started"
