--TEST--
getLanguage should handle unacceptable or malformed Accept-Language without crash
--SKIPIF--
<?php
if (!extension_loaded("yaf")) print "skip";
?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$_SERVER['HTTP_ACCEPT_LANGUAGE'] = 'en;q=0';
$request = new Yaf_Request_Http('/user/view');
var_dump($request->getLanguage());
var_dump($request->language);

$_SERVER['HTTP_ACCEPT_LANGUAGE'] = 'en;q=0, zh-CN;q=0.0';
$request = new Yaf_Request_Http('/user/view');
var_dump($request->getLanguage());

$_SERVER['HTTP_ACCEPT_LANGUAGE'] = 'fr;q=0, en;q=0.7, de;q=0.9';
$request = new Yaf_Request_Http('/user/view');
var_dump($request->getLanguage());

$_SERVER['HTTP_ACCEPT_LANGUAGE'] = 'q=0.8';
$request = new Yaf_Request_Http('/user/view');
var_dump($request->getLanguage());

$_SERVER['HTTP_ACCEPT_LANGUAGE'] = ';q=0.9';
$request = new Yaf_Request_Http('/user/view');
var_dump($request->getLanguage());

$_SERVER['HTTP_ACCEPT_LANGUAGE'] = 'q=0.9, en;q=0.8';
$request = new Yaf_Request_Http('/user/view');
var_dump($request->getLanguage());
?>
--EXPECT--
NULL
NULL
NULL
string(2) "de"
NULL
NULL
string(2) "en"
