--TEST--
Check for Yaf_View_Simple recursive render error message outputing
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
$tpl  = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'foo.phtml';
$tpl2 = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'foo2.phtml';
function cleartpl() {
	global $tpl, $tpl2;
	unlink($tpl);
	unlink($tpl2);
}
register_shutdown_function("cleartpl");

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
--EXPECTF--
Parse error: syntax error, unexpected '{' in %sfoo2.phtml on line %d
