--TEST--
Numeric keys in the system section of the config must not crash
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$config = array(
	"application" => array(
		"directory" => realpath(dirname(__FILE__)),
		"system" => array(
			0 => "foo",
			"library" => "bar",
		),
	),
);

$app = new Yaf_Application($config);
echo "OK\n";
?>
--EXPECTF--
OK
