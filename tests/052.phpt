--TEST--
Check for Yaf_Request APis
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.use_namespace=0
--FILE--
<?php
$request = new Yaf_Request_Http(new stdClass(), "xxxx", false);
var_dump($request);
$request = new Yaf_Request_Http("xxxxxxxxxxxxxxxxxxxxxxxxxxx");
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
var_dump($request->getCookie("xxx"), false);
var_dump($request->getEnv("xxx"), "2.13232");
echo "------params-------\n";
var_dump($request->setParam("xxxx"));
var_dump($request->getParam("xxxx"));
var_dump($request->getParams());
echo "-----others-----\n";
var_dump($request->isXmlHttpRequest());
var_dump($request->isCli());
var_dump($request->isPost());
?>
--EXPECTF--
Warning: Yaf_Request_Http::__construct() expects at most 2 parameters, 3 given in %s052.php on line %d
bool(false)
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
NULL
bool(false)
NULL
string(7) "2.13232"
------params-------

Warning: Yaf_Request_Abstract::setParam() expects parameter 1 to be array, string given in %s052.php on line %d
NULL
NULL
array(0) {
}
-----others-----
bool(false)
bool(true)
bool(false)
