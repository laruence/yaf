--TEST--
Check for Yaf_View_Simple::eval
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
--FILE--
<?php 
$view = new Yaf_View_Simple("/tmp");

$tpl = <<<PHP
<?php
echo \$name, "\n";
foreach(\$entry as \$list) {
   echo "1. ", \$list, "\n";
}
?>
<?=\$name?>
PHP;

$view->assign("entry", array('a', 'b', 'c'));
echo $view->eval($tpl, array('name' => 'template'));

?>
--EXPECTF--
template
1. a
1. b
1. c
template
