--TEST--
Check for Custom view engine
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
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

class SimpleView extends Yaf_View_Simple {
      public function __constrct() {
      }

      public function assign($name, $value = NULL) {
             $this->_tpls_vars[$name] = $value;
      }
}

$tpl_dir = APPLICATION_PATH . "/application/views";
file_put_contents(APPLICATION_PATH . "/application/Bootstrap.php", <<<PHP
<?php
   class Bootstrap extends Yaf_Bootstrap_Abstract {
        public function _initView(Yaf_Dispatcher \$dispatcher) {
            \$dispatcher->setView(new SimpleView("{$tpl_dir}"));
        }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/application/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         public function indexAction() {
            \$this->_view->name = "custom view";
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/application/views/index/index.phtml", "<?=\$name?>");

$app = new Yaf_Application($config);
$response = $app->bootstrap()->run();
echo $response;
?>
--EXPECTF--
custom view
