--TEST--
Issue #420 (bug in yaf_dispatcher_get_call_parameters)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=1
yaf.lowcase_path=0
yaf.throw_exception=0
yaf.catch_exception=1
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

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
		public function funcAction(\$a = NULL, \$b = NULL) {
			var_dump(\$a, \$b);
			return FALSE;
		}
   }
PHP
);

$app = new Yaf_Application($config);
$req = new Yaf_Request_Simple();
$req->setControllerName('Index');
$req->setActionName('func');
$req->setParam(array('b' => 'the second param'));
$app->getDispatcher()->dispatch($req);
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECT--
NULL
NULL
