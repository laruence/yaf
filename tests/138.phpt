--TEST--
Reject short configuration filenames safely
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
require "build.inc";
startup(__FILE__);

foreach (array("", "a", "ini") as $config) {
    try {
        new Yaf_Application($config);
    } catch (Throwable $e) {
        echo $e->getMessage(), PHP_EOL;
    }
}

$config = APPLICATION_PATH . "/application.ini";
file_put_contents($config, "[product]
application.directory = \"" . APPLICATION_PATH . "\"
");
new Yaf_Application($config);
echo "valid", PHP_EOL;
?>
--CLEAN--
<?php
require "build.inc";
shutdown(__FILE__);
?>
--EXPECT--
Initialization of application config failed
Initialization of application config failed
Initialization of application config failed
valid
