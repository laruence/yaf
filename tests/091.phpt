--TEST--
Check for Yaf_Request_getXXX
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
yaf.use_namespace=0
--GET--
name=get
--POST--
name=raw
--FILE--
<?php
$_SERVER["name"] = "server";
$_COOKIE["name"] = "cookie";
$_POST["name"] = "post";
$_ENV["name"] = "env";
$_REQUEST["name"] = "request";

$request = new Yaf_Request_Http("/");

var_dump($request->getServer("name"));
var_dump($request->getQuery("name"));
var_dump($request->getCookie("name"));
var_dump($request->getPost("name"));
var_dump($request->getEnv("name"));
var_dump($request->getRequest("name"));
var_dump($request->getRaw());

var_dump(count($request->getServer()));
var_dump(count($request->getQuery()));
var_dump(count($request->getCookie()));
var_dump(count($request->getPost()));
var_dump(count($request->getEnv()));
var_dump(count($request->getRequest()));
?>
--EXPECTF--
string(6) "server"
string(3) "get"
string(6) "cookie"
string(4) "post"
string(3) "env"
string(7) "request"
string(8) "name=raw"
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
