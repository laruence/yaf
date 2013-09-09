--TEST--
Check for Yaf_View_Simple and application's template directory
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.library="/php/global/dir"
yaf.use_namespace=0
--FILE--
<?php
$view = new Yaf_View_Simple(dirname(__FILE__));
try {
    $view = new Yaf_View_Simple("");
} catch (Yaf_Exception_TypeError $e) {
    var_dump($e->getMessage());
}

$config = array(
   "application" => array(
      "directory" => dirname(__FILE__),
   ),
);
$app = new Yaf_Application($config);

$view = Yaf_Dispatcher::getInstance()->initView(array());
var_dump($view->getScriptPath());
?>
--EXPECTF--
string(%d) "Expects an absolute path for templates directory"
NULL
