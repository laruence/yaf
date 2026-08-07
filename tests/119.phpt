--TEST--
Controller properties hashtable should be destroyed when the object is freed
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
/* controller construction copies module/controller from the dispatcher request */
$app->getDispatcher()->setRequest(new Yaf_Request_Simple("CLI", "Index", "Dummy", "act"));

class Ctrl extends Yaf_Controller_Abstract {
}

$m1 = memory_get_usage();
for ($i = 0; $i < 100; $i++) {
    $ctl = new Ctrl();
    /* lazy-create the properties hashtable */
    get_object_vars($ctl);
    unset($ctl);
}
$m2 = memory_get_usage();

/* inverted GC_DELREF leaves 100 hash tables leaked */
var_dump($m2 - $m1 < 10000);
?>
--EXPECT--
bool(true)
