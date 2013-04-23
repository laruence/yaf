--TEST--
Bug segfault while call exit in a view template
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.library="/php/global/dir"
yaf.use_namespace=0
--FILE--
<?php
require "build.inc";
startup();

$view = new Yaf_View_Simple(dirname(__FILE__));

$view->assign("name", "laruence");
$tpl  =  APPLICATION_PATH . '/tpls/foo.phtml';

file_put_contents($tpl, <<<HTML
okey
<?php exit; ?>
HTML
);
echo $view->render($tpl);
?>
--CLEAN--
<?php
/* unlink foo2.phtml permission denied */
require "build.inc"; 
shutdown();
?>
--EXPECTF--
okey
