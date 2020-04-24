--TEST--
Check for Yaf_Request read/write property
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.use_namespace=0
--FILE--
<?php

function ref_change(&$val) {
	$val = "bad";
}

$request = new Yaf_Request_Simple();

foreach (array("method", "module", "controller", "action", "uri", "base_uri", "language", 123) as $k) {
	print "Testing $k:\n";
	var_dump($request->{$k});
	$request->{$k} = true;
	var_dump($request->{$k});
	$request->{$k} = "inDex";
	ref_change($request->{$k});
	var_dump($request->{$k});
	echo "\n";
}

var_dump($request);
?>
--EXPECTF--
Testing method:
string(3) "CLI"
string(3) "CLI"
string(5) "inDex"

Testing module:
NULL
NULL
string(5) "Index"

Testing controller:
NULL
NULL
string(5) "Index"

Testing action:
NULL
NULL
string(5) "index"

Testing uri:
string(0) ""

Warning: main(): Modification of Yaf_Request internal property 'uri' is not allowed in %s094.php on line %d
string(0) ""

Warning: main(): Modification of Yaf_Request internal property 'uri' is not allowed in %s094.php on line %d
string(0) ""

Testing base_uri:
NULL

Warning: main(): Modification of Yaf_Request internal property 'base_uri' is not allowed in %s094.php on line %d
NULL

Warning: main(): Modification of Yaf_Request internal property 'base_uri' is not allowed in %s094.php on line %d
NULL

Testing language:
NULL

Warning: main(): Modification of Yaf_Request internal property 'language' is not allowed in %s094.php on line %d
NULL

Warning: main(): Modification of Yaf_Request internal property 'language' is not allowed in %s094.php on line %d
NULL

Testing 123:
NULL
NULL
NULL

object(Yaf_Request_Simple)#1 (10) {
  ["method"]=>
  string(5) "inDex"
  ["module"]=>
  string(5) "Index"
  ["controller"]=>
  string(5) "Index"
  ["action"]=>
  string(5) "index"
  ["uri:protected"]=>
  string(0) ""
  ["base_uri:protected"]=>
  NULL
  ["dispatched:protected"]=>
  bool(false)
  ["routed:protected"]=>
  bool(false)
  ["language:protected"]=>
  string(0) ""
  ["params:protected"]=>
  array(0) {
  }
}
