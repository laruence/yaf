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

$app = new Yaf_Application(array(
	"yaf" => array(
		"directory" => __DIR__ ,
	),
));

foreach (array(
	"environ", "running", "config", "library",
    "directory", "ext", "view_ext", "err_no", "err_msg",
   "bootstrap", "dispatcher", 123, "noexists", ) as $k) {
	print "Testing $k:\n";
	var_dump($app->{$k});
	$app->{$k} = true;
	var_dump($app->{$k});
	$app->{$k} = "inDex";
	ref_change($app->{$k});
	var_dump($app->{$k});
	echo "\n";
}

var_dump($app);
?>
--EXPECTF--
Testing environ:
string(7) "product"
string(7) "product"
string(7) "product"

Testing running:
bool(false)
bool(false)
bool(false)

Testing config:
object(Yaf_Config_Simple)#2 (2) {
%A
}
object(Yaf_Config_Simple)#2 (2) {
%A
}
object(Yaf_Config_Simple)#2 (2) {
%A
}

Testing library:
NULL
NULL
string(5) "inDex"

Testing directory:
string(%d) "%stests"
string(%d) "%stests"
string(5) "inDex"

Testing ext:
string(3) "php"
string(3) "php"
string(5) "inDex"

Testing view_ext:
string(5) "phtml"
string(5) "phtml"
string(5) "inDex"

Testing err_no:
bool(false)
bool(false)
bool(false)

Testing err_msg:
NULL
NULL
NULL

Testing bootstrap:
NULL
NULL
string(5) "inDex"

Testing dispatcher:
object(Yaf_Dispatcher)#4 (0) {
}
object(Yaf_Dispatcher)#4 (0) {
}
object(Yaf_Dispatcher)#4 (0) {
}

Testing 123:
NULL
NULL
NULL

Testing noexists:
NULL
NULL
NULL

object(Yaf_Application)#1 (13) {
  ["directory"]=>
  string(5) "inDex"
  ["library"]=>
  string(5) "inDex"
  ["bootstrap"]=>
  string(5) "inDex"
  ["ext"]=>
  string(5) "inDex"
  ["view_ext"]=>
  string(5) "inDex"
  ["environ:protected"]=>
  string(7) "product"
  ["running:protected"]=>
  bool(false)
  ["err_msg:protected"]=>
  NULL
  ["err_no:protected"]=>
  bool(false)
  ["config:protected"]=>
  object(Yaf_Config_Simple)#2 (2) {
%A
  }
  ["dispatcher:protected"]=>
  object(Yaf_Dispatcher)#4 (0) {
  }
  ["modules:protected"]=>
  array(1) {
    [0]=>
    string(5) "Index"
  }
  ["default_route:protected"]=>
  NULL
}
