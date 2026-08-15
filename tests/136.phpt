--TEST--
Supervar route with a non-string supervar must not crash
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--GET--
r[a]=b
--FILE--
<?php
$config = array(
	"application" => array(
		"directory" => realpath(dirname(__FILE__)),
		"dispatcher" => array(
			"defaultRoute" => array("type" => "supervar", "varname" => "r"),
		),
	),
);

$app = new Yaf_Application($config);
$request = new Yaf_Request_Http("/foo/bar");
try {
	$app->getDispatcher()->dispatch($request);
	echo "no error\n";
} catch (Yaf_Exception $e) {
	echo get_class($e), "\n";
}
?>
--EXPECTF--
Yaf_Exception_RouterFailed
