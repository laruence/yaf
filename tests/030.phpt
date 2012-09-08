--TEST--
Check for Yaf_Config_Ini::__construct with section
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.library="/php/global/dir"
--FILE--
<?php
$file = dirname(__FILE__) . "/simple.ini";

try {
  $config = new Yaf_Config_Ini($file, "ex");
} catch (Exception $e) { 
  print_r($e->getMessage());
}

?>
--EXPECTF--
There is no section 'ex' in '%ssimple.ini'
