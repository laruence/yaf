--TEST--
Custom view setScriptPath return value must not leak when template dir is set
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
yaf.use_namespace=0
--FILE--
<?php
require "build.inc";
startup(__FILE__);

$config = array(
	"application" => array(
		"directory" => APPLICATION_PATH,
	),
);

class LeakView implements Yaf_View_Interface {
	public function assign($name, $value = NULL) {}
	/* always NULL so the dispatcher resets the template dir on every dispatch */
	public function getScriptPath($request = NULL) { return NULL; }
	/* like Yaf_View_Simple, return $this */
	public function setScriptPath($path) { return $this; }
	public function render($script, $value = NULL) { return ""; }
	public function display($script, $value = NULL) { return TRUE; }
}

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         public function indexAction() {
            return FALSE;
         }
   }
PHP
);

function refcount_of($obj) {
	ob_start();
	debug_zval_dump($obj);
	$out = ob_get_clean();
	preg_match('/refcount\((\d+)/', $out, $m);
	return (int)$m[1];
}

$app = new Yaf_Application($config);
$dispatcher = Yaf_Dispatcher::getInstance();
$view = new LeakView();
$dispatcher->setView($view);
$dispatcher->returnResponse(true);

$before = refcount_of($view);
for ($i = 0; $i < 10; $i++) {
	$dispatcher->dispatch(new Yaf_Request_Http("/index/index"));
}
$after = refcount_of($view);

/* setScriptPath returns $this on every dispatch; the dispatcher must drop
   that reference after the call */
var_dump($after - $before);
?>
--CLEAN--
<?php
require "build.inc";
shutdown(__FILE__);
?>
--EXPECT--
int(0)
