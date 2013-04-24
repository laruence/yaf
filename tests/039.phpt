--TEST--
Check for Yaf_View_Simple recursive render error message outputing
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
$tpl2  =  APPLICATION_PATH . '/tpls/foo2.phtml';

file_put_contents($tpl, <<<HTML
<?php
   echo \$this->render(\$tpl);
?>
HTML
);

file_put_contents($tpl2, <<<HTML
<?php
   if ((1) { //syntax error
   }
?>
HTML
);

echo $view->render($tpl, array('tpl' => $tpl2));
?>
--CLEAN--
<?php
/* unlink foo2.phtml permission denied */
require "build.inc"; 
shutdown();
?>
--EXPECTF--
Parse error: syntax error, unexpected '{' in %sfoo2.phtml on line %d
