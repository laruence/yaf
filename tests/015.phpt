--TEST--
Check for Yaf_Exception
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
<?php if (version_compare(PHP_VERSION, "5.3.0", "lt")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php 
$previous = new Yaf_Exception("Previous", 100);
$exception = new Yaf_Exception("Exception", 200, $previous);

var_dump($previous === $exception->getPrevious());
var_dump($exception->getMessage());
var_dump($exception->getPrevious()->getCode());
?>
--EXPECTF--
bool(true)
string(9) "Exception"
int(100)
