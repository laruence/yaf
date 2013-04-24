--TEST--
Check actions map with defined action class
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
		public \$actions = array(
				"sub" => "actions/sub.php",
			);
         public function indexAction() {
             var_dump(\$this->forward('sub'));
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/actions/sub.php", <<<PHP
<?php
   class SubAction extends Yaf_Action_Abstract {
         public function execute() {
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/views/index/index.phtml", "indexAction" . PHP_EOL);
file_put_contents(APPLICATION_PATH . "/views/index/sub.phtml", "subAction" . PHP_EOL);

$app = new Yaf_Application($config);
$request = new Yaf_Request_Simple();
$app->getDispatcher()->dispatch($request);
$new_request = new Yaf_Request_Simple();
$new_request->setActionName('sub');
$app->getDispatcher()->dispatch($new_request);
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECTF--
bool(true)
indexAction
subAction
subAction
