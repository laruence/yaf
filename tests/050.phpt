--TEST--
Check for Sample application with return response
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
        public function _initReturn(Yaf_Dispatcher \$dispatcher) {
            \$dispatcher->returnResponse(true);
        }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         public function indexAction() {
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/views/index/index.phtml", "view");

$app = new Yaf_Application($config);
$response = $app->bootstrap()->run();
var_dump("-------");
echo $response;
?>
--CLEAN--
<?php
/* unlink foo2.phtml permission denied */
require "build.inc"; 
shutdown();
?>
--EXPECTF--
string(7) "-------"
view
