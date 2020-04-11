--TEST--
Check for various  cycle references
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
yaf.use_namespace=0
--FILE--
<?php 
$foo = new Stdclass();
$foo->request = new Yaf_Request_Simple();
$foo->request->setParam("foo", $foo);
unset($foo);

$foo = new Stdclass();
$foo->config = new Yaf_Config_Simple(array(), 0);
$foo->config->foo = $foo;
unset($foo);

$foo = new Stdclass();
$foo->request = new Yaf_Request_Simple();
$app = new Yaf_Application(["yaf" => ["directory" => __DIR__]]);
$app->getDispatcher()->setRequest($foo->request);
unset($foo);
?>
okey
--EXPECT--
okey
