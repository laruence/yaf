--TEST--
Check for Yaf_Application::bootstrap errors
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

$app = new Yaf_Application($config);

$app->bootstrap();
file_put_contents(APPLICATION_PATH . "/Bootstrap.php", <<<PHP
PHP
);
$app->bootstrap();
file_put_contents(APPLICATION_PATH . "/Bootstrap.php", <<<PHP
<?php
class Bootstrap {}
PHP
);
try {
	$app->bootstrap();
} catch (Exception $e) {
	var_dump($e->getMessage());
}
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECTF--
Warning: Yaf_Application::bootstrap(): Couldn't find bootstrap file %sBootstrap.php in %s105.php on line %d

Warning: Yaf_Application::bootstrap(): Couldn't find class Bootstrap in %sBootstrap.php in %s105.php on line %d
string(%d) "'Bootstrap' is not a subclass of Yaf_Bootstrap_Abstract"
