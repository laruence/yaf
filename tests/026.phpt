--TEST--
Check for Yaf_Response::setBody/prependBody/appendBody
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.library="/php/global/dir"
--FILE--
<?php 
$response = new Yaf_Response_Http();
$response->setBody("ell")->appendBody("o")->prependBody("H");
echo $response;
?>
--EXPECTF--
Hello
