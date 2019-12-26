--TEST--
ISSUE #311 (Yaf_application::environ should respect $environ)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
yaf.use_namespace=0
yaf.environ=product
--FILE--
<?php
$config = array(
    "application" => array(
        "directory" => dirname(__DIR__),
    ),
);
$app = new Yaf_Application($config,'dev');//使用dev环境
echo ini_get('yaf.environ'),' ',$app->environ();//显示系统配置的yaf环境和当前应用的yaf环境
?>
--EXPECT--
product dev
