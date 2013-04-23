--TEST--
Check nesting view render
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
         public function init() {
             Yaf_Dispatcher::getInstance()->flushInstantly(true);
         } 
         public function indexAction() {
             var_dump(\$this->_view->getScriptPath());
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/views/index/index.phtml", "<?php print_r(\$this); \$this->display('index/sub.phtml', array('content' => 'dummy'));?>");
file_put_contents(APPLICATION_PATH . "/views/index/sub.phtml", "<?php echo \$content; echo \$this->eval('foobar'); ?>");

$app = new Yaf_Application($config);
$response = $app->bootstrap()->run();
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECTF--
string(%d) "%sapplication%cviews"
Yaf_View_Simple Object
(
    [_tpl_vars:protected] => Array
        (
        )

    [_tpl_dir:protected] => 
    [_options:protected] => 
)
dummyfoobar
