--TEST--
Check for auto response with ErrorController
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
        "dispatcher" => array (
           "catchException" => true,
        ), 
	),
);

file_put_contents(APPLICATION_PATH . "/controllers/Error.php", <<<PHP
<?php
   class ErrorController extends Yaf_Controller_Abstract {
         public function errorAction(\$exception) {
			\$response = \$this->getResponse(); 	
			\$response->setBody(json_encode(array(
										"code" => \$exception->getCode(),
										"msg" => \$exception->getMessage(),
									)
								));
			\$response->response();
			return false;
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         public function init() {
			Yaf_Dispatcher::getInstance()->returnResponse(true);
         }
   }
PHP
);

$app = new Yaf_Application($config);
$response = $app->run();
?>
--CLEAN--
<?php
/* unlink foo2.phtml permission denied */
require "build.inc"; 
shutdown();
?>
--EXPECTF--
{"code":%d,"msg":"There is no method indexAction in IndexController"}
