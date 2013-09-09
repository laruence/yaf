--TEST--
check for Various segfault
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
yaf.use_namespace=0
--FILE--
<?php
try {
    $config = new Yaf_Config_Ini(dirname(__FILE__), "test");
} catch (Exception $e) {
    var_dump($e->getMessage());
}

$request = new Yaf_Request_Simple(NULL);
var_dump($request->isOptions());


$config = new Yaf_Config_Simple(array());
$config->key();
echo "okey";
?>
--EXPECTF--
string(%d) "Argument is not a valid ini file '%s'"
bool(false)
okey
