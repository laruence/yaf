--TEST--
Reject an oversized default bootstrap path safely
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
require "build.inc";
startup(__FILE__);

try {
    $app = new Yaf_Application(array(
        "application" => array("directory" => "/" . str_repeat("x", PHP_MAXPATHLEN)),
    ));
    $app->bootstrap();
} catch (Throwable $e) {
    echo get_class($e), ": ", $e->getMessage(), PHP_EOL;
}
?>
--CLEAN--
<?php
require "build.inc";
shutdown(__FILE__);
?>
--EXPECT--
Yaf_Exception_StartupError: Bootstrap path is too long
