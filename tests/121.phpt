--TEST--
Exception handler must not clobber other dispatcher flags when clearing IN_EXCEPTION
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
		"dispatcher" => array(
			"catchException" => true,
		),
	),
);

file_put_contents(APPLICATION_PATH . "/controllers/Error.php", <<<PHP
<?php
   class ErrorController extends Yaf_Controller_Abstract {
         public function errorAction(\$exception) {
            \$this->forward("second");
            return FALSE;
         }
         public function secondAction() {
            \$this->getResponse()->appendBody("second-done");
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         public function indexAction() {
            throw new Exception("boom");
         }
   }
PHP
);

mkdir(APPLICATION_PATH . "/views/error/");
file_put_contents(APPLICATION_PATH . "/views/error/second.phtml", "SECOND-VIEW");

$app = new Yaf_Application($config);
$dispatcher = Yaf_Dispatcher::getInstance();
/* auto rendering is explicitly disabled before dispatch */
$dispatcher->autoRender(false);

$request = new Yaf_Request_Http("/index/index");
/* first dispatch: the exception is caught, errorAction forwards and the
   dispatcher returns NULL because the exception handler ran */
var_dump($dispatcher->dispatch($request));

/* second dispatch of the same request runs the forwarded action; clearing
   IN_EXCEPTION must not have turned AUTO_RENDER/INSTANT_FLUSH back on,
   otherwise second.phtml gets rendered and flushed instantly */
$dispatcher->returnResponse(true);
$response = $dispatcher->dispatch($request);
var_dump($response->getBody());
?>
--CLEAN--
<?php
require "build.inc";
shutdown();
?>
--EXPECT--
bool(false)
string(11) "second-done"
