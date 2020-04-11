--TEST--
Check for Yaf_Loader with namespace configuration
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.library="/php/global/dir"
yaf.use_namespace=0
--FILE--
<?php 
$config = array(
	"application" => array(
		"directory" => realpath(dirname(__FILE__)),
        "library" => array(
            "directory" => "/tmp",
            "namespace" => "Foo, Bar",
        ),
	),
);

$app = new Yaf_Application($config);
Yaf_Loader::getInstance()->registerLocalNamespace("Dummy");
print_r(Yaf_Loader::getInstance());
var_dump(Yaf_Loader::getInstance()->isLocalName("Bar_Name"));

?>
--EXPECTF--
Yaf_Loader Object
(
    [library:protected] => /tmp
    [global_library:protected] => /php/global/dir
    [namespace:protected] => Array
        (
            [0] => Foo
            [1] => Bar
            [2] => Dummy
        )

    [use_spl_autoload:protected] => 
    [lowercase_path:protected] => 
    [is_name_suffix:protected] => 1
    [has_name_seperator:protected] => 
)
bool(true)
