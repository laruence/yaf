--TEST--
return type in Yaf_Simple_Config::valid() should be boolean
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php

$new = new Yaf_Config_Simple(array());
var_dump($new->valid());
?>
--CLEAN--
--EXPECTF--
bool(false)
