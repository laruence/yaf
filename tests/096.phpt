--TEST--
Check for Custom route
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$router = new Yaf_Router();

$switch = true;
class myRoute implements Yaf_Route_Interface {
	public function route($request) {
		global $switch;
		if ($switch) {
			print_r($request);
		}
		return $switch;
	}

	public function assemble(array $info, array $verify = NULL) {
	}
}

$router->addRoute("custom", new myRoute);

$router->route(new Yaf_Request_Http("/foo/dummy"));
var_dump($router->getCurrentRoute());

$switch = false;
$router->route(new Yaf_Request_Http("/foo/dummy"));
var_dump($router->getCurrentRoute());
?>
--EXPECT--
Yaf_Request_Http Object
(
    [method] => CLI
    [module] => 
    [controller] => 
    [action] => 
    [uri:protected] => /foo/dummy
    [base_uri:protected] => 
    [dispatched:protected] => 
    [routed:protected] => 
    [language:protected] => 
    [params:protected] => Array
        (
        )

)
string(6) "custom"
string(8) "_default"
