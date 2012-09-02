--TEST--
Check for Sample application with return response
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
report_memleaks=0
--FILE--
<?php 
require "build.inc";
define("APPLICATION_PATH", dirname(__FILE__));
startup(APPLICATION_PATH . '/application');
$config = array(
	"application" => array(
		"directory" => APPLICATION_PATH . "/application/",
	),
);

file_put_contents(APPLICATION_PATH . "/application/Bootstrap.php", <<<PHP
<?php
   class Bootstrap extends Yaf_Bootstrap_Abstract {
        public function _initReturn(Yaf_Dispatcher \$dispatcher) {
            \$dispatcher->returnResponse(true);
        }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/application/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         public function indexAction() {
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/application/views/index/index.phtml", "view");

$app = new Yaf_Application($config);
$response = $app->bootstrap()->run();
var_dump("-------");
echo $response;
--EXPECTF--
string(7) "-------"
view
