--TEST--
Check for PSR-4 autoloading 
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
		"library" => array(
			"namespace" => array(
				"\Zend" => "/lib/Zend/foo",
				"\Microsoft\PHP" => "/var/lib/microsoft",
				"\Microsoft\ASP" => "/var/lib/asp",
			)
		),
	),
);
$app = new Yaf_Application($config);
$loader = Yaf_Loader::getInstance();
var_dump($loader->getNamespaces());
var_dump($loader->getNamespacePath("\Zend\Foo\Dummy"));
var_dump($loader->getNamespacePath("\Microsoft\Java"));
var_dump($loader->getNamespacePath("\Microsoft\PHP\Framework"));

$loader->registerNamespace("\Google\Robot", "/library/vendor/lib/robot");
var_dump($loader->getNamespacePath("Google\Robot"));

var_dump(class_exists("\Microsoft\PHP\Framework\Dummy"));
var_dump(class_exists("\Microsoft\ASP\Framework\Dummy"));
var_dump(class_exists("\Zend\Foo\Dummy\Bar"));
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECTF--
array(2) {
  ["Zend"]=>
  string(13) "/lib/Zend/foo"
  ["Microsoft"]=>
  array(2) {
    ["PHP"]=>
    string(18) "/var/lib/microsoft"
    ["ASP"]=>
    string(12) "/var/lib/asp"
  }
}
string(13) "/lib/Zend/foo"
string(%d) "%sapplication%clibrary"
string(18) "/var/lib/microsoft"
string(%d) "%sapplication%clibrary"

Warning: Yaf_Loader::autoload(): Failed opening script /var/lib/microsoft%cFramework%cDummy.php: No such file or directory in %s106.php on line %d
bool(false)

Warning: Yaf_Loader::autoload(): Failed opening script /var/lib/asp%cFramework%cDummy.php: No such file or directory in %s106.php on line %d
bool(false)

Warning: Yaf_Loader::autoload(): Failed opening script /lib/Zend/foo%cFoo%cDummy%cBar.php: No such file or directory in %s106.php on line %d
bool(false)
