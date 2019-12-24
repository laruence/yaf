--TEST--
Check for Yaf_Response_HTTP::setRedirect in CLI
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$response = new Yaf_Response_HTTP(); 
var_dump($response->setRedirect('https://yourdomain.com'));
?>
--EXPECT--
bool(false)
