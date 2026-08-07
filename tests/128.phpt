--TEST--
Superglobal getters must read userland-assigned values
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$_GET["foo"] = "bar";
$_POST["baz"] = "qux";

$request = new Yaf_Request_Simple();

var_dump($request->getQuery("foo"));
var_dump($request->getPost("baz"));
var_dump($request->getQuery("missing", "dflt"));
var_dump(is_array($request->getQuery()));
var_dump($request->getServer("no_such_key", "fallback"));
?>
--EXPECT--
string(3) "bar"
string(3) "qux"
string(4) "dflt"
bool(true)
string(8) "fallback"
