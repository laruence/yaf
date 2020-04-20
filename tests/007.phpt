--TEST--
Check for Yaf_Config_Simple APIs
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php 
$config = array(
	'section1' => array(
		'name' => 'value',
		'dummy' =>  'foo',
	),
	'section2' => "laruence",
);

$config1 = new Yaf_Config_Simple($config, 1);
print_r($config1);
$config2 = new Yaf_Config_Simple($config, 1);
var_dump($config2->readonly());
$config2->new = "value";
var_dump(isset($config->new));
$config3 = new Yaf_Config_Simple($config);
unset($config);

echo "Isset config3 section:";
var_dump(isset($config3["section2"]));
$config3->new = "value";
echo "Config3 readonly:";
var_dump($config3->readonly());

foreach($config3 as $key => $val) {
}

foreach($config3 as $key => $val) {
	print_r($key);
	print_r("=>");
	print_r($val);
	print_r("\n");
}

print_r($config3->toArray());

$sick = @new Yaf_Config_Simple();

var_dump($sick->__isset(1));
var_dump($sick->__get(2));
$sick->total = 1;
var_dump(count($sick));
var_dump($sick->total);
?>
--EXPECTF--
Yaf_Config_Simple Object
(
    [readonly:protected] => 1
    [config:protected] => Array
        (
            [section1] => Array
                (
                    [name] => value
                    [dummy] => foo
                )

            [section2] => laruence
        )

)
bool(true)
bool(false)
Isset config3 section:bool(true)
Config3 readonly:bool(false)
section1=>Yaf_Config_Simple Object
(
    [readonly:protected] => 
    [config:protected] => Array
        (
            [name] => value
            [dummy] => foo
        )

)

section2=>laruence
new=>value
Array
(
    [section1] => Array
        (
            [name] => value
            [dummy] => foo
        )

    [section2] => laruence
    [new] => value
)
bool(false)
NULL
int(0)
NULL
