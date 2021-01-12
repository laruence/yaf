--TEST--
Segfault while exiting in action
--SKIPIF--
<?php 
	if (!extension_loaded("yaf")) exit("skip"); 
	if (version_compare(PHP_VERSION, "8.0.0", "lt")) exit("skip only PHP8 affected");
?>
--INI--
yaf.use_namespace=0
yaf.use_spl_autoload=0
--FILE--
<?php 
require "build.inc";
startup();

$config = array(
	"application" => array(
		"directory" => APPLICATION_PATH,
	),
);
$app = new Yaf_Application($config);

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
class IndexController extends Yaf_Controller_Abstract {
    public function indexAction() {
		echo 'okey'; exit;
    }    
}
PHP
);

$app->run();
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECT--
okey
