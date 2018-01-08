--TEST--
Check Yaf_Request_Http::getRaw
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--POST--
a=b&c=d
--FILE--
<?php
$request = new Yaf_Request_Http("/");
var_dump($request->getRaw());
?>
--EXPECT--
string(7) "a=b&c=d"
