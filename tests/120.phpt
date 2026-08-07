--TEST--
Yaf_Controller_Abstract construction should not crash when dispatcher request is unset
--SKIPIF--
<?php
if (!extension_loaded("yaf")) print "skip";
?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$config = array(
    "application" => array(
        "directory" => realpath(dirname(__FILE__)),
    ),
);
$app = new Yaf_Application($config);

class Ctrl extends Yaf_Controller_Abstract {
}

/* no request set on the dispatcher yet */
$ctl = new Ctrl();
var_dump($ctl->getName());
var_dump($ctl->getModuleName());
unset($ctl);

/* with a request set, name/module must still be picked up */
$request = new Yaf_Request_Simple("CLI", "Index", "Dummy", "act");
$app->getDispatcher()->setRequest($request);
$ctl = new Ctrl();
var_dump($ctl->getName());
var_dump($ctl->getModuleName());
?>
--EXPECT--
NULL
NULL
string(5) "Dummy"
string(5) "Index"
