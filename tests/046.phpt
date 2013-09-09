--TEST--
Check for Yaf_Loader with single class
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
yaf.use_namespace=0
--FILE--
<?php 
$config = array(
	"application" => array(
		"directory" => realpath(dirname(__FILE__)),
        "library" => array(
            "directory" => "/tmp",
            "namespace" => "Foo",
        ),
	),
);
$app = new Yaf_Application($config);
$loader = Yaf_Loader::getInstance();
var_dump($loader->isLocalName("Foo_Bar"));
var_dump($loader->isLocalName("Foo"));
$loader->clearLocalNamespace();
$loader->registerLocalNamespace("Bar");
var_dump($loader->isLocalName("Foo_Bar"));
var_dump($loader->isLocalName("Bar"));
?>
--EXPECTF--
bool(true)
bool(true)
bool(false)
bool(true)
