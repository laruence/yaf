--TEST--
Autoloading the classes under library
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

$config = array(
	"application" => array(
		"directory" => APPLICATION_PATH,
	),
);

file_put_contents(APPLICATION_PATH . "/Bootstrap.php", <<<PHP
<?php
   class Bootstrap extends Yaf_Bootstrap_Abstract {
       public function _initTest() {
			Yaf_Loader::getInstance()->registerLocalNamespace("Test");
			Yaf_Registry::set("test", new Test());
       }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/library/Test.php", <<<PHP
<?php
class Test {
	public function __construct() {
		var_dump("okey");
	}
}
PHP
);

$app = new Yaf_Application($config);
$response = $app->bootstrap();
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECTF--
string(4) "okey"
