--TEST--
Check for Yaf_View_Simple error message outputing
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.library="/php/global/dir"
log_errors=0
display_errors=1
--FILE--
<?php
$view = new Yaf_View_Simple(dirname(__FILE__));

$view->assign("name", "laruence");
$tpl = dirname(__FILE__) . '/foo.phtml';
function cleartpl() {
	global $tpl;
	@unlink($tpl);
}
register_shutdown_function("cleartpl");

    file_put_contents($tpl, <<<HTML
<?php 
   if ((x) { //syntax errors
   } 
?>
HTML
);

echo $view->render($tpl);
?>
--EXPECTF--
Parse error: syntax error, unexpected '{' in %sfoo.phtml on line %d
