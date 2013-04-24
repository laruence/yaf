--TEST--
Bug #63381 ($_SERVER['SCRIPT_NAME'] changed by yaf)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=1
yaf.lowcase_path=0
yaf.use_namespace=0
--FILE--
<?php 
require "build.inc";
startup();

$server = $_SERVER;
$cookie = $_COOKIE;
$post   = $_POST;
$get    = $_GET;

$config = array(
	"application" => array(
		"directory" => APPLICATION_PATH,
	),
);

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
       public function indexAction() {
       }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/controllers/Dummy.php", <<<PHP
<?php
   class DummyController extends Yaf_Controller_Abstract {
       public function indexAction() {
       }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/views/index/index.phtml", "");

$app = new Yaf_Application($config);
$response = $app->run();

var_dump($server === $_SERVER);
var_dump($cookie === $_COOKIE);
var_dump($get === $_GET);
var_dump($post === $_POST);

?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
