--TEST--
Check for Yaf_Loader::getInstace() paramters
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.library="/php/global/dir"
yaf.use_namespace=0
--FILE--
<?php 
$loader = Yaf_Loader::getInstance('/foo', '/bar');

var_dump($loader->getLibraryPath());
var_dump($loader->getLibraryPath(TRUE));
$config = array(
	"application" => array(
		"directory" => realpath(dirname(__FILE__)),
	),
);

$app = new Yaf_Application($config);
var_dump($loader->getLibraryPath());
var_dump($loader->getLibraryPath(TRUE));
?>
--EXPECTF--
string(4) "/foo"
string(4) "/bar"
string(%d) "%slibrary"
string(15) "/php/global/dir"
