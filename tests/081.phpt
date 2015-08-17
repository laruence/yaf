--TEST--
PHP7 Yaf_Response leak memory
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php 
$response = new Yaf_Response_Cli();
$string = "Jason is a good boy";
$response->setBody($string);
$response->setBody($string);
var_dump($response->getBody());
?>
--EXPECTF--
string(19) "Jason is a good boy"
