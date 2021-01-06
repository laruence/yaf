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
$response->setHeader("custom", "foobar");
$response->response();
var_dump($response->setRedirect('https://yourdomain.com'));
?>
--EXPECTHEADERS--
Status: 302 Found
Location: https://yourdomain.com
custom: foobar
--EXPECT--
bool(true)
