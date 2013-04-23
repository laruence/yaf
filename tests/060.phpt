--TEST--
Check for working with other autoloaders
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=1
yaf.lowcase_path=0
yaf.use_namespace=0
--FILE--
<?php 
require "build.inc";
startup();

function my_autoload($class) {
     eval("class $class {}");
     return TRUE;
}

$config = array(
	"application" => array(
		"directory" => APPLICATION_PATH,
	),
);

file_put_contents(APPLICATION_PATH . "/Bootstrap.php", <<<PHP
<?php
   class Bootstrap extends Yaf_Bootstrap_Abstract {
        public function _initAutoload(Yaf_Dispatcher \$dispatcher) {
             spl_autoload_register("my_autoload");
        }

        public static function errorHandler(\$error, \$errstr) {
              var_dump(\$errstr);
        }

        public function _initErrorHandler(Yaf_Dispatcher \$dispatcher) {
              \$dispatcher->setErrorHandler(array("Bootstrap", "errorHandler"));
        }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         public function indexAction() {
            \$obj = new Dummy();
            \$this->_view->obj = \$obj;
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/views/index/index.phtml", "<?php echo get_class(\$obj);?>");

$app = new Yaf_Application($config);
$response = $app->bootstrap()->run();
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECTF--
Dummy
