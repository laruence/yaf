--TEST--
Check for Yaf_Dispatcher::setRespone
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

class my_response extends Yaf_Response_Abstract {
	public $bodys;

	public function appendBody($body, $name = NULL) {
		$this->bodys[] = $body;
	}
	public function response() {
		var_dump(json_encode($this->bodys));
	}
}

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         public function indexAction() {
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/views/index/index.phtml", "custom response");

$app = new Yaf_Application($config);
$app->getDispatcher()->setResponse(new my_response());
$app->run();
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECT--
string(19) "["custom response"]"
