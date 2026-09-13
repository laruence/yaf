--TEST--
Reject an oversized relative view script path safely
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
require "build.inc";
startup(__FILE__);

try {
    $view = new Yaf_View_Simple("/" . str_repeat("x", PHP_MAXPATHLEN));
    $view->render("index.phtml");
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
Yaf_Exception_LoadFailed_View: View script path is too long
