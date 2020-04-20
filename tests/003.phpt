--TEST--
Check for Yaf_Loader local names
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
yaf.use_namespace=0
--FILE--
<?php 
ini_set("ap.lowcase_path", FALSE);
$loader = Yaf_Loader::getInstance(dirname(__FILE__), dirname(__FILE__) . "/global");
$loader->registerLocalNamespace("Baidu");
$loader->registerLocalNamespace("Sina");
$loader->registerLocalNamespace(array("Wb", "Inf", NULL, array(), "123"));
var_dump($loader->getLocalNamespace());
var_dump($loader->isLocalName("Baidu_Name"));
var_dump($loader->isLocalName("Inf\\Name"));

try {
	var_dump($loader->autoload("Baidu_Name"));
} catch (Yaf_Exception_LoadFailed $e) {
	var_dump($e->getMessage());
} 
try {
	var_dump($loader->autoload("Global_Name"));
} catch (Yaf_Exception_LoadFailed $e) {
	var_dump($e->getMessage());
} 

?>
--EXPECTF--
array(5) {
  [0]=>
  string(5) "Baidu"
  [1]=>
  string(4) "Sina"
  [2]=>
  string(2) "Wb"
  [3]=>
  string(3) "Inf"
  [4]=>
  string(3) "123"
}
bool(true)
bool(true)

Warning: Yaf_Loader::autoload(): Failed opening script %sBaidu%cName.php: No such file or directory in %s
bool(true)

Warning: Yaf_Loader::autoload(): Failed opening script %sglobal%cGlobal%cName.php: No such file or directory in %s
bool(true)
