--TEST--
Fixed misleading error message when providing a string in Yaf_Application construction
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php

$config = <<<INI
[product]
;CONSTANTS is supported
application.directory = APP_PATH "/application/"
INI;

try {
	$app = new Yaf_Application($config);
	$app->run();
}
catch (Exception $e){
	print $e->getPrevious()->getMessage();
}

?>
--CLEAN--
--EXPECTF--
Expects a path to *.ini configuration file as parameter
