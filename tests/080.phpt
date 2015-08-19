--TEST--
PHP7 didn't display Error.
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
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
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
class IndexController extends Yaf_Controller_Abstract {

    public function indexAction() {
        geoge();//funciton undefined!    
    }    
}
PHP
);

$app = new Yaf_Application($config);
$app->bootstrap()->run();
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function geoge() in %s:5
Stack trace:
#0 [internal function]: IndexController->indexAction()
#1 %s080.php(30): Yaf_Application->run()
#2 {main}
  thrown in %s on line %d
