--TEST--
Check for ReturnResponse in cli
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

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         public function indexAction() {}
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/views/index/index.phtml", "okey");

$app = new Yaf_Application($config);
$response = $app->getDispatcher()->returnResponse(true)->dispatch(new Yaf_Request_Simple("CLI", "Index","Index","index"));
var_dump($response->getBody());
?>
--CLEAN--
<?php
/* unlink foo2.phtml permission denied */
require "build.inc"; 
shutdown();
?>
--EXPECTF--
string(4) "okey"
