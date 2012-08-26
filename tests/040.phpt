--TEST--
Fixed bug that segv in Yaf_View_Simple::render if the tpl is not a string
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.library="/php/global/dir"
--FILE--
<?php
$view = new Yaf_View_Simple(dirname(__FILE__));
$view->render(NULL);
$view->render(0);
$view->render(true);
echo "okey";
?>
--EXPECT--
okey
