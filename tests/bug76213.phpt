--TEST--
Bug #76213 (Memory leaks with yaf_dispatcher_exception_handler)
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
            "catchException" => 1
        )
    ),
);

$app = new Yaf_Application($config);

try {
    $request = new Yaf_Request_Http("/index/admin/login1");
    $app->getDispatcher()->dispatch($request);
} catch(Yaf_Exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECTF--
string(%d) "%s"
