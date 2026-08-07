--TEST--
Base uri detection should compare PHP_SELF/ORIG_SCRIPT_NAME with their own basename length
--SKIPIF--
<?php
if (!extension_loaded("yaf")) print "skip";
?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
/* file_name is "index" (extension-stripped basename of SCRIPT_FILENAME),
 * PHP_SELF basename "inde" is a strict prefix of it, so it must match */
$_SERVER['SCRIPT_FILENAME'] = '/www/index.php';
$_SERVER['PHP_SELF'] = '/app/inde';
$_SERVER['REQUEST_URI'] = '/app/inde/foo';
$request = new Yaf_Request_Http();
var_dump($request->base_uri);

unset($_SERVER['PHP_SELF']);
$_SERVER['ORIG_SCRIPT_NAME'] = '/cgi/inde';
$_SERVER['REQUEST_URI'] = '/cgi/inde/foo';
$request = new Yaf_Request_Http();
var_dump($request->base_uri);
?>
--EXPECT--
string(9) "/app/inde"
string(9) "/cgi/inde"
