--TEST--
Check for Yaf_Config_Ini with env
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.directory=/foo/bar
--FILE--
<?php 
putenv("FOO=bar");
define("FOO", "Dummy");
print_r(new Yaf_Config_Ini(
	dirname(__FILE__) . "/simple.ini", "envtest"));
?>
--EXPECTF--
Yaf_Config_Ini Object
(
    [_config:protected] => Array
        (
            [env] => bar
            [ini] => /foo/bar
            [const] => Dummy
        )

    [_readonly:protected] => 1
)
