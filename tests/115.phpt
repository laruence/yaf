--TEST--
Yaf_Request_Http should not crash on unparseable absolute-form REQUEST_URI
--SKIPIF--
<?php
if (!extension_loaded("yaf")) print "skip";
?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
/* absolute-form REQUEST_URI that php_url_parse rejects (empty host / bad port) */
$_SERVER['REQUEST_URI'] = 'HTTP:///foo';
$request = new Yaf_Request_Http();
var_dump($request->uri);

$_SERVER['REQUEST_URI'] = 'http://host:99999999/foo';
$request = new Yaf_Request_Http();
var_dump($request->uri);

/* valid absolute-form should extract the path, case-insensitively */
$_SERVER['REQUEST_URI'] = 'HTTP://example.com/foo/bar?a=b';
$request = new Yaf_Request_Http();
var_dump($request->uri);

$_SERVER['REQUEST_URI'] = 'http://example.com/foo/bar?a=b';
$request = new Yaf_Request_Http();
var_dump($request->uri);

$_SERVER['REQUEST_URI'] = 'HtTp://example.com/baz';
$request = new Yaf_Request_Http();
var_dump($request->uri);
?>
--EXPECT--
string(0) ""
string(0) ""
string(8) "/foo/bar"
string(8) "/foo/bar"
string(4) "/baz"
