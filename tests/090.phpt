--TEST--
Check for view path generating
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

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         public function indexAction() {
         }
         public function foo_bar_indexAction() {
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/views/index/index.phtml", "<?php var_dump('Index'); ?>");

$app = new Yaf_Application($config);


$app->getDispatcher()->getRouter()->AddRoute("map", new Yaf_Route_Map(true));

$request = new Yaf_Request_Http("/");
$app->getDispatcher()->dispatch($request);

/* Foo_Bar_Index controller */
mkdir(APPLICATION_PATH . "/controllers/Foo");
mkdir(APPLICATION_PATH . "/controllers/Foo/Bar");
file_put_contents(APPLICATION_PATH . "/controllers/Foo/Bar/Index.php", <<<PHP
<?php
   class Foo_Bar_IndexController extends Yaf_Controller_Abstract {
         public function indexAction() {
         }
   }
PHP
);

mkdir(APPLICATION_PATH . "/views/foo");
mkdir(APPLICATION_PATH . "/views/foo/bar");
mkdir(APPLICATION_PATH . "/views/foo/bar/index");
file_put_contents(APPLICATION_PATH . "/views/foo/bar/index/index.phtml", "<?php var_dump('CFoo_Bar_Index'); ?>");
$request = new Yaf_Request_Http("/Foo/Bar/Index");
$app->getDispatcher()->dispatch($request);

/* Foo_Bar_Index action */
mkdir(APPLICATION_PATH . "/views/index/foo");
mkdir(APPLICATION_PATH . "/views/index/foo/bar");
file_put_contents(APPLICATION_PATH . "/views/index/foo/bar/index.phtml", "<?php var_dump('AFoo_Bar_Index'); ?>");
$app->getDispatcher()->getRouter()->AddRoute("map", new Yaf_Route_Map(false));

$app->getDispatcher()->getRequest()->setDispatched(false)->setRouted(false)->setControllerName("Index");
$app->getDispatcher()->dispatch($request);

?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECT--
string(5) "Index"
string(14) "CFoo_Bar_Index"
string(14) "AFoo_Bar_Index"
