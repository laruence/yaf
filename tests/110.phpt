--TEST--
Bug (open file not destroy or crash in Yaf_Config_Ini, PHP 8.1 or later)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
yaf.use_namespace=0
--FILE--
<?php
$file = dirname(__FILE__) . "/simple.ini";

// ulimit -n, default 256
// open files will be 257
for ($i = 0; $i < 257; $i++) {
    $a = new Yaf_Config_Ini($file);
    unset($a);
}

var_dump("Done")
?>
--EXPECTF--
string(4) "Done"