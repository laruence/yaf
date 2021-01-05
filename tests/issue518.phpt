--TEST--
Check for Yaf_Response_HTTP::setRedirect in CGI
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--GET--
foo=bar
--FILE--
<?php
$response = new Yaf_Response_HTTP(); 
var_dump($response->setRedirect('https://yourdomain.com'));
?>
--EXPECTHEADERS--
Status: 302 Found
--EXPECT--
bool(true)
