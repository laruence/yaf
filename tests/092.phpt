--TEST--
Check for base uri detecting
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
yaf.use_namespace=0
--FILE--
<?php 

$_SERVER["REQUEST_URI"] = "/yaf/c/a";
$_SERVER["SCRIPT_FILENAME"] = "/usr/local/www/yaf/index.php";
$_SERVER["SCRIPT_NAME"] = "/yaf/index.php";
$req = new Yaf_Request_Http();
var_dump($req->getBaseuri());
var_dump($req->getRequestUri());

unset($_SERVER["SCRIPT_NAME"]);
$_SERVER["PHP_SELF"] = "/yaf/index.php";
$req = new Yaf_Request_Http();
var_dump($req->getBaseuri());
var_dump($req->getRequestUri());


unset($_SERVER["REQUEST_URI"]);
unset($_SERVER["PHP_SELF"]);
$_SERVER["PATH_INFO"] = "/c/a";
$req = new Yaf_Request_Http();
var_dump($req->getBaseuri());
var_dump($req->getRequestUri());
?>
--EXPECT--
string(4) "/yaf"
string(8) "/yaf/c/a"
string(4) "/yaf"
string(8) "/yaf/c/a"
string(0) ""
string(4) "/c/a"
