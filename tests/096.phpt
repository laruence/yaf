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
			$request->setModuleName("moDule");
			$request->setControllerName("index");
			$request->action = "ActioN";
		}
		return $switch;
	}

	public function assemble(array $info, array $verify = NULL) {
	}
}

$router->addRoute("custom", new myRoute);

$request = new Yaf_Request_Http("/foo/dummy");
$router->route($request);
print_r($request);
var_dump($router->getCurrentRoute());

$switch = false;
$request = new Yaf_Request_Http("/foo/dummy");
$router->route($request);
print_r($request);
var_dump($router->getCurrentRoute());
?>
--EXPECT--
Yaf_Request_Http Object
(
    [method] => CLI
    [module] => Module
    [controller] => Index
    [action] => action
    [uri:protected] => /foo/dummy
    [base_uri:protected] => 
    [dispatched:protected] => 
    [routed:protected] => 1
    [language:protected] => 
    [params:protected] => Array
        (
        )

)
string(6) "custom"
Yaf_Request_Http Object
(
    [method] => CLI
    [module] => 
    [controller] => Foo
    [action] => dummy
    [uri:protected] => /foo/dummy
    [base_uri:protected] => 
    [dispatched:protected] => 
    [routed:protected] => 1
    [language:protected] => 
    [params:protected] => Array
        (
        )

)
string(8) "_default"
