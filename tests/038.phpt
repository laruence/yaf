--TEST--
Check for Yaf_View_Simple error message outputing
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.library="/php/global/dir"
log_errors=0
display_errors=1
yaf.use_namespace=0
--FILE--
<?php
require "build.inc";
startup();

$view = new Yaf_View_Simple(dirname(__FILE__));

$view->assign("name", "laruence");
$tpl  =  APPLICATION_PATH . '/tpls/foo.phtml';

file_put_contents($tpl, <<<HTML
<?php 
   if ((x) { //syntax errors
   } 
?>
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
Parse error: syntax error, unexpected '{' in %sfoo.phtml on line %d
