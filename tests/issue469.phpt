--TEST--
Issue #469 (treat autocontroller as Contorller mistakenly)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
yaf.throw_exception=0
yaf.catch_exception=1
yaf.use_namespace=0
yaf.name_separator=""
--FILE--
<?php 
Yaf_Loader::getInstance("/tmp/library");

set_error_handler(function($level, $msg) {
	global $is_mvc;
	if (strpos($msg, "Couldn't load a MVC class unless") !== false && !$is_mvc) {
		var_dump($msg);
	} else if (strpos($msg, "Couldn't load a MVC class unless") === false && $is_mvc) {
		var_dump($msg);
	}
});

function try_load_class($name) {
	Yaf_Loader::getInstance()->autoload($name);
}

ini_set("yaf.name_suffix", 1);
$is_mvc = false;
try_load_class("automodel");
try_load_class("autoplugin");
try_load_class("autocontroller");
$is_mvc = true;
try_load_class("autoModel");
try_load_class("autoPlugin");
try_load_class("autoController");

ini_set("yaf.name_suffix", 0);
$is_mvc = false;
try_load_class("modelauto");
try_load_class("pluginauto");
try_load_class("controllerauto");
$is_mvc = true;
try_load_class("Modelauto");
try_load_class("Pluginauto");
try_load_class("Controllerauto");

ini_set("yaf.name_suffix", 1);
ini_set("yaf.name_separator", "_");
$is_mvc = false;
try_load_class("auto_model");
try_load_class("auto_plugin");
try_load_class("auto_controller");
$is_mvc = true;
try_load_class("auto_Model");
try_load_class("auto_Plugin");
try_load_class("auto_Controller");

ini_set("yaf.name_suffix", 0);
$is_mvc = false;
try_load_class("model_auto");
try_load_class("plugin_auto");
try_load_class("controller_auto");
try_load_class("Modelauto");
try_load_class("Pluginauto");
try_load_class("Controllerauto");

$is_mvc = true;
try_load_class("Model_auto");
try_load_class("Plugin_auto");
try_load_class("Controller_auto");

ini_set("yaf.name_separator", "\\");
try_load_class("Model\auto");
try_load_class("\Plugin\auto");
try_load_class("\Controller\auto");
?>
okey
--EXPECT--
okey
