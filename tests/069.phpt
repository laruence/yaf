--TEST--
Check for Yaf_Dispatcher move mian module into modules directory when it has multi modules
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$config = array(
    "application" => array(
        "directory" => realpath(dirname(__FILE__)),
        "dispatcher" => array(
            "catchException"    => 0,
            "throwException"    => 0,
            "defaultModule"     => "index",
            "defaultController" => "index",
            "defaultAction"     => "index"
        ),
        "modules" => "Index,Admin"
    ),
);

$app = new Yaf_Application($config);
var_dump($app->getAppDirectory());
$dir = $app->getAppDirectory() . "/tmp";
mkdir($dir);
$app->setAppDirectory($dir);
var_dump($app->getAppDirectory());
$app->run();
?>
--CLEAN--
<?php
rmdir(dirname(__FILE__) . "/tmp");
?>
--EXPECTF--
string(%d) "%stests"
string(%d) "%stmp"

Catchable fatal error: Yaf_Application::run(): Failed opening controller script %stmp%cmodules%cIndex%ccontrollers%cIndex.php: No such file or directory in %s069.php on line %d