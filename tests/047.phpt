--TEST--
Check for Yaf_Loader with spl_autoload
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
--FILE--
<?php 
$config = array(
	"application" => array(
		"directory" => realpath(dirname(__FILE__)),
        "library" => array(
            "directory" => "/tmp",
            "namespace" => "Foo",
        ),
	),
);

function pre_load($name) {
    var_dump($name);
}

function load($name) {
    eval("class " . $name . " {} ");
    return TRUE;
}

spl_autoload_register("pre_load");

$app = new Yaf_Application($config);

spl_autoload_register("load");

$a = new Foo();

print_r($a);
?>
--EXPECTF--
string(3) "Foo"

Warning: Yaf_Loader::autoload(): Failed opening script %stmp%cFoo.php: No such file or directory in %s047.php on line %d
Foo Object
(
)
