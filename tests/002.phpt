--TEST--
Check for Yaf_Request_Simple
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php 
$request  = new Yaf_Request_Simple("CLI", "index", "dummy", NULL, array());
print_r($request);
var_dump((bool)$request->setParam("name", "Laruence"));
var_dump($request->isCli());
var_dump($request->isXmlHttpRequest());
var_dump($request->isPost());
var_dump($request->getParam("name"));
var_dump($request->getParam("notexists"));

$app =  new Yaf_Application(array("application" => array(
            "directory" => dirname(__FILE__),
            )));

try {
    $app->getDispatcher()->dispatch($request);
} catch (Yaf_Exception_LoadFailed_Controller $e) {
    var_dump($e->getMessage());
}

var_dump($request->get("xxx"));
var_dump($request->getQuery("xxx"));
var_dump($request->getServer("xxx"));
var_dump($request->getPost("xxx"));
var_dump($request->getCookie("xxx"));
var_dump($request->getEnv("xxx"));
echo "------default value-------\n";
var_dump($request->get("xxx", "123"));
print_r($request->getQuery("xxx", new stdClass()));
print_r($request->getServer("xxx", array()));
var_dump($request->getPost("xxx", NULL));
var_dump($request->getCookie("xxx", false));
var_dump($request->getEnv("xxx", "2.13232"));
echo "------params-------\n";
var_dump($request->setParam("xxxx"));
var_dump($request->getParam("xxxx"));
var_dump($request->getParams());

?>
--EXPECTF--
Yaf_Request_Simple Object
(
    [module] => index
    [controller] => dummy
    [action] => index
    [method] => CLI
    [params:protected] => Array
        (
        )

    [language:protected] => 
    [_exception:protected] => 
    [_base_uri:protected] => 
    [uri:protected] => 
    [dispatched:protected] => 
    [routed:protected] => 1
)
bool(true)
bool(true)
bool(false)
bool(false)
string(8) "Laruence"
NULL
string(%d) "Failed opening controller script %scontrollers%cDummy.php: No such file or directory"
NULL
NULL
NULL
NULL
NULL
NULL
------default value-------
string(3) "123"
stdClass Object
(
)
Array
(
)
NULL
bool(false)
string(7) "2.13232"
------params-------

Warning: Yaf_Request_Abstract::setParam() expects parameter 1 to be array, string given in %s002.php on line %d
NULL
NULL
array(1) {
  ["name"]=>
  string(8) "Laruence"
}
