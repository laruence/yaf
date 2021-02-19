--TEST--
ISSUE #535 (Segsev while throw exception in action)
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

$config = array(
	"application" => array(
		"directory" => APPLICATION_PATH,
	),
);

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
		public \$actions = array(
				"test_seprate" => "actions/test/seprate.php",
			);
   }
PHP
);

@mkdir(APPLICATION_PATH . "/actions/test/");
file_put_contents(APPLICATION_PATH . "/actions/test/seprate.php", <<<PHP
<?php
   class Test_SeprateAction extends Yaf_Action_Abstract {
         public function execute() {
			\$x  = new XX();
			\$yy  = new YY();
			echo "okey";
			return false;
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/library/XX.php", <<<PHP
<?php
	class XX extends ZZ {
		public static function test() {
        }
    }
PHP
);

file_put_contents(APPLICATION_PATH . "/library/YY.php", <<<PHP
<?php
	class YY extends ZZ {
		public static function test() {
        }
    }
PHP
);

file_put_contents(APPLICATION_PATH . "/library/ZZ.php", <<<PHP
<?php
	class ZZ { }
PHP
);

$app = new Yaf_Application($config);
$request = new Yaf_Request_Simple("GET", "Index", "Index", "test_seprate");
$app->getDispatcher()->dispatch($request);
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECT--
okey
