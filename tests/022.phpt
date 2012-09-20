--TEST--
Check for Yaf_Application
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
--FILE--
<?php 
$config = array(
	"application" => array(
		"directory" => realpath(dirname(__FILE__)),
		"dispatcher" => array(
			"catchException" => 0,
			"throwException" => 0,
		),
	),
);

$app = new Yaf_Application($config);
var_dump($app->getAppDirectory());
$app->setAppDirectory('/tmp');
var_dump($app->getAppDirectory());
$app->run();
?>
--EXPECTF--
string(%d) "%stests"
string(4) "/tmp"

Catchable fatal error: Yaf_Application::run(): Failed opening controller script /tmp/controllers/Index.php:%s in %s022.php on line %d
