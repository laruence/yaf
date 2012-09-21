--TEST--
Check for Yaf_Loader::set/get(library_path)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
--FILE--
<?php 
$loader = Yaf_Loader::getInstance('/foo', '/bar');
$loader->registerLocalNamespace(array("Foo"));

$loader->autoload("Foo_Bar");
$loader->autoload("Bar_Foo");

$loader->setLibraryPath("/foobar", FALSE);
$loader->autoload("Foo_Bar");
$loader->autoload("Bar_Foo");

$loader->setLibraryPath("/foobar", TRUE);
$loader->autoload("Foo_Bar");
$loader->autoload("Bar_Foo");

$loader->autoload("Bar_Model");
?>
--EXPECTF--
Warning: Yaf_Loader::autoload(): Failed opening script /foo%cFoo%cBar.php: No such file or directory in %s023.php on line %d

Warning: Yaf_Loader::autoload(): Failed opening script /bar%cBar%cFoo.php: No such file or directory in %s023.php on line %d

Warning: Yaf_Loader::autoload(): Failed opening script /foobar%cFoo%cBar.php: No such file or directory in %s023.php on line %d

Warning: Yaf_Loader::autoload(): Failed opening script /bar%cBar%cFoo.php: No such file or directory in %s023.php on line %d

Warning: Yaf_Loader::autoload(): Failed opening script /foobar%cFoo%cBar.php: No such file or directory in %s023.php on line %d

Warning: Yaf_Loader::autoload(): Failed opening script %cfoobar%cBar%cFoo.php: No such file or directory in %s023.php on line %d

Warning: Yaf_Loader::autoload(): Couldn't load a framework MVC class without an Yaf_Application initializing in %s023.php on line %d
