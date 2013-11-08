--TEST--
Check for 
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=1
yaf.lowcase_path=0
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
$loader = Yaf_Loader::getInstance();

$dir = $app->getAppDirectory() . "/tmp";
mkdir($dir);
$app->setAppDirectory($dir);

$loader->autoload("MyModel");
$loader->autoload("MyService");
$loader->autoload("MyDao");
?>
--CLEAN--
<?php
rmdir(dirname(__FILE__) . "/tmp");
?>
--EXPECTF--
PHP Warning:  Yaf_Loader::autoload(): Failed opening script %stmp%cservices%cMy.php: No such file or directory in %s070.php on line %d
PHP Warning:  Yaf_Loader::autoload(): Failed opening script %stmp%cdaos%sMy.php: No such file or directory in %s070.php on line %d