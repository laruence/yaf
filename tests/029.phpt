--TEST--
Check for Yaf_View_Simple::get and clear
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.library="/php/global/dir"
yaf.use_namespace=0
--FILE--
<?php
$view = new Yaf_View_Simple(dirname(__FILE__));
$view->assign("a", "b");
$view->assign("b", "a");
print_r($view->get("a"));
print_r($view->get());
$view->clear("b");
print_r($view->get());
var_dump(empty($view->a));
var_dump(isset($view->a));
$view->clear();
print_r($view->get());
var_dump(empty($view->a));
var_dump(isset($view->a));
?>
--EXPECTF--
bArray
(
    [a] => b
    [b] => a
)
Array
(
    [a] => b
)
bool(false)
bool(true)
Array
(
)
bool(true)
bool(false)
