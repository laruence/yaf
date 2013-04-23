--TEST--
Bug #61493 (Can't remove item when using unset() with a Yaf_Config_Simple instance)
--CREDITS--
littlemiaor at gmail dot com
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php

$config = new Yaf_Config_Simple(array(
    'foo' => 'bar',
), false);

unset($config['foo']);
print_r($config);
?>
--EXPECTF--
Yaf_Config_Simple Object
(
    [_config:protected] => Array
        (
        )

    [_readonly:protected] => 
)
