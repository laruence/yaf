--TEST--
Check for Yaf_Config_Ini with env
--SKIPIF--
<?php 
if (!extension_loaded("yaf")) print "skip"; 
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip seems windows doesnt support this');
}
?>
--INI--
yaf.directory=/foo/bar
yaf.use_namespace=0
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
