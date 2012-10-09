--TEST--
Bug empty template file interrupts forward chain
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=1
yaf.lowcase_path=0
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
       public function indexAction() {
           \$this->forward("dummy");
       }
       public function dummyAction() {
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
file_put_contents(APPLICATION_PATH . "/views/index/dummy.phtml", "Dummy");

$app = new Yaf_Application($config);
$response = $app->run();
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECTF--
Dummy
