--TEST--
Check for Yaf_View_Simple with short_tag_open
--SKIPIF--
<?php
if (!extension_loaded("yaf")) print "skip"; 
if (version_compare(PHP_VERSION, '5.4.0') >= 0) {
   print "skip PHP 5.4 remove short_open_tag";
} 
?>
--INI--
short_open_tag = 0
--FILE--
<?php 
$view = new Yaf_View_Simple(dirname(__FILE__));
$tpl = dirname(__FILE__) . '/short_tag_test.phtml';

$content = "<?=\$name?>";

file_put_contents($tpl, $content);

echo $view->render($tpl, array('name' => 'template'));
echo "\n";
$view->display($tpl, array('name' => 'template'));
$view = new Yaf_View_Simple(dirname(__FILE__), array("short_tag" => 0));
echo "\n";
var_dump(ini_get("short_open_tag"));
$view->display($tpl, array('name' => 'template'));
echo "\n";
print_r($view);
unlink($tpl);
?>
--EXPECTF--
template
template
string(1) "0"
<?=$name?>
Yaf_View_Simple Object
(
    [_tpl_vars:protected] => Array
        (
        )

    [_tpl_dir:protected] => %s
    [_options:protected] => Array
        (
            [short_tag] => 0
        )

)
