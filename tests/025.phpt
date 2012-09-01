--TEST--
Check for Yaf_Loader with namespace configuration
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.library="/php/global/dir"
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
    [_library:protected] => /tmp
    [_global_library:protected] => /php/global/dir
)
bool(true)
