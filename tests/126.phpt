--TEST--
Yaf_Config get_properties must expose NULL config when init failed
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$config = array(
	"application" => array(
		"directory" => realpath(dirname(__FILE__)),
		"dispatcher" => array(
			"throwException" => 0,
		),
	),
);

$app = new Yaf_Application($config);

set_error_handler(function () {
	/* swallow the recoverable error raised for the missing ini file */
	return true;
});

/* yaf_config_ini_init() fails but the constructor completes,
   leaving conf->config as NULL */
$ini = new Yaf_Config_Ini("/no/such/config.ini");
var_dump($ini);
?>
--EXPECTF--
object(Yaf_Config_Ini)#%d (2) {
  ["readonly:protected"]=>
  bool(true)
  ["config:protected"]=>
  NULL
}
