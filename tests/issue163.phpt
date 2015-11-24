--TEST--
Issue #163 (forward from init controller)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=1
yaf.lowcase_path=0
yaf.throw_exception=0
yaf.catch_exception=1
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
		public function init() {
			\$this->forward("Index", "Second", "okey");
		}
		public function indexAction() {
			echo "bad";
		}
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/controllers/Second.php", <<<PHP
<?php
   class SecondController extends Yaf_Controller_Abstract {
			public function okeyAction() {
				echo "Okey";
				return FALSE;
			}
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/views/index/index.phtml", "Okey");

$app = new Yaf_Application($config);
$response = $app->run();
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECT--
Okey
