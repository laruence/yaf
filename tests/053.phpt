--TEST--
Check for Custom view engine
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

class SimpleView implements Yaf_View_Interface {
	protected $tpl_dir;
	protected $view;
	public function __construct() {
		$this->view = new Yaf_View_Simple(__DIR__);
	}
	public function assign($name, $value = NULL) {
		$this->view->assign($name,  $value . " custom view");
	}
	public function __set($name, $value = NULL) {
		return $this->assign($name, $value);
	}
	public function getScriptPath($request = NULL) {
		return $this->tpl_dir;
	}
	public function setScriptPath($path) {
		$this->view->setScriptPath($path);
		return true;
	}
	public function render($script, $value = NULL) {
		return $this->view->render($script, $value);
	}
	public function display($script, $value = NULL) {
		return 	$this->view->display($script, $value);
	}
}

$tpl_dir = APPLICATION_PATH . "/views";
file_put_contents(APPLICATION_PATH . "/Bootstrap.php", <<<PHP
<?php
   class Bootstrap extends Yaf_Bootstrap_Abstract {
        public function _initView(Yaf_Dispatcher \$dispatcher) {
            \$dispatcher->setView(new SimpleView('{$tpl_dir}'));
        }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         public function indexAction() {
            \$this->_view->assign("name", "name");
            \$this->_view->val = "val";
         }
   }
PHP
);

file_put_contents($tpl_dir . "/index/index.phtml", <<<HTML
<?=\$name?>

<?=\$val?>
HTML
);

$app = new Yaf_Application($config);
$response = $app->bootstrap()->run();
echo $response;
?>
--CLEAN--
<?php
/* unlink foo2.phtml permission denied */
require "build.inc"; 
shutdown();
?>
--EXPECTF--
name custom view
val custom view
