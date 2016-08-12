--TEST--
Check for variables out of scope
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

$view = new Yaf_View_Simple(DIR_ROOT);
$view->assign("name", "laruence");
$tpl  =  APPLICATION_PATH . '/tpls/foo.phtml';

file_put_contents($tpl, <<<HTML
<?php
   echo \$name, \$tpl;
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
laruence
Notice: Undefined variable: tpl in %sfoo.phtml on line %d
