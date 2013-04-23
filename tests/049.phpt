--TEST--
Check for Sample application with exception
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
        "library" => array(
        ),
	),
);

file_put_contents(APPLICATION_PATH . "/controllers/Error.php", <<<PHP
<?php
   class ErrorController extends Yaf_Controller_Abstract {
         public function errorAction(\$exception) {
            \$this->_view->msg = \$exception->getMessage();
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/Bootstrap.php", <<<PHP
<?php
   class Bootstrap extends Yaf_Bootstrap_Abstract {
        public function _initConfig(Yaf_Dispatcher \$dispatcher) {
            Yaf_Registry::set("config", Yaf_Application::app()->getConfig());
        }
        public function _initPlugin(Yaf_Dispatcher \$dispatcher) {
            \$dispatcher->registerPlugin(new TestPlugin());
        }
        public function _initReturn(Yaf_Dispatcher \$dispatcher) {
            \$dispatcher->returnResponse(true);
        }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/plugins/Test.php", <<<PHP
<?php
   class TestPlugin extends Yaf_Plugin_Abstract {
        public function routerStartup(Yaf_Request_Abstract \$request, Yaf_Response_Abstract \$response) {
            var_dump("routerStartup");
        }
        public function routerShutdown(Yaf_Request_Abstract \$request, Yaf_Response_Abstract \$response) {
            var_dump("routerShutdown");
        }
        public function dispatchLoopStartup(Yaf_Request_Abstract \$request, Yaf_Response_Abstract \$response) {
            var_dump("dispatchLoopStartup");
        }
        public function preDispatch(Yaf_Request_Abstract \$request, Yaf_Response_Abstract \$response) {
            var_dump("preDispatch");
        }
        public function postDispatch(Yaf_Request_Abstract \$request, Yaf_Response_Abstract \$response) {
            var_dump("postDispatch");
        }
        public function dispatchLoopShutdown(Yaf_Request_Abstract \$request, Yaf_Response_Abstract \$response) {
            var_dump("dispatchLoopShutdown");
        }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         public function init() {
            var_dump("init");
         }
         public function indexAction() {
            var_dump("action");
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/views/index/index.phtml",
                "<?php throw new Exception('view exception'); ?>");

mkdir(APPLICATION_PATH . "/views/error/");
file_put_contents(APPLICATION_PATH . "/views/error/error.phtml",
                "catched: <?=\$msg?>");

$app = new Yaf_Application($config);
$app->bootstrap()->run();
?>
--CLEAN--
<?php
/* unlink foo2.phtml permission denied */
require "build.inc"; 
shutdown();
?>
--EXPECTF--
string(13) "routerStartup"
string(14) "routerShutdown"
string(19) "dispatchLoopStartup"
string(11) "preDispatch"
string(4) "init"
string(6) "action"
catched: view exception
