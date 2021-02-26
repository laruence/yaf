--TEST--
Check for Custom MVC name
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.use_namespace=0
--FILE--
<?php
require "build.inc";
startup();

$config = array(
	"application" => array(
		"directory" => APPLICATION_PATH,
        "bootstrap" => APPLICATION_PATH . "/cboot.php",
        "dispatcher" => array (
           "catchException" => true,
        ), 
        "library" => array(
        ),
	),
);

file_put_contents(APPLICATION_PATH . "/controllers/Error.php", <<<PHP
<?php
   class ErrorController extends Yaf_Controller_Abstract {
         public function errorAction(\$exception) {
              var_dump(\$exception->getMessage());
              return FALSE;
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/cboot.php", <<<PHP
<?php
   class Bootstrap extends Yaf_Bootstrap_Abstract {
        public function _initPlugin(Yaf_Dispatcher \$dispatcher) {
            \$dispatcher->registerPlugin(new TestPlugin());
        }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/plugins/Test.php", <<<PHP
<?php
   class TestPlugin extends Yaf_Plugin_Abstract {
        public function routerShutdown(Yaf_Request_Abstract \$request, Yaf_Response_Abstract \$response) {
			\$request->setModuleName("CUSTOM", false);
			\$request->setControllerName("INDEX", false);
			\$request->setActionName("CamelName", false);
			return;
        }
        public function dispatchLoopStartup(Yaf_Request_Abstract \$request, Yaf_Response_Abstract \$response) {
        }
        public function preDispatch(Yaf_Request_Abstract \$request, Yaf_Response_Abstract \$response) {
        }
        public function postDispatch(Yaf_Request_Abstract \$request, Yaf_Response_Abstract \$response) {
			var_dump(\$request->getControllerName());
			var_dump(\$request->getActionName());
        }
   }
PHP
);

$value = NULL;

@mkdir(APPLICATION_PATH . "/modules");
@mkdir(APPLICATION_PATH . "/modules/CUSTOM");
@mkdir(APPLICATION_PATH . "/modules/CUSTOM/controllers");
@mkdir(APPLICATION_PATH . "/modules/CUSTOM/views");
/* Controller name here should be lowercase anyway? */
@mkdir(APPLICATION_PATH . "/modules/CUSTOM/views/index");
file_put_contents(APPLICATION_PATH . "/modules/CUSTOM/controllers/INDEX.php", <<<PHP
<?php
   class INDEXController extends Yaf_Controller_Abstract {
         public function CamelNameAction() {
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/modules/CUSTOM/views/index/CamelName.phtml", "<?php var_dump('ok'); ?>");

$app = new Yaf_Application($config);
$app->bootstrap()->run();

?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECTF--
string(5) "INDEX"
string(9) "CamelName"
string(2) "ok"
