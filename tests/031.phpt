--TEST--
Check for application.dispatcher.defaultRoute
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.library="/php/global/dir"
--FILE--
<?php 
$config = array(
	"application" => array(
		"directory" => realpath(dirname(__FILE__)),
        "dispatcher" => array(
            "defaultRoute" => array(
               "type" => "map",
               "delimiter" => '##',
               "controllerPrefer" => 1,
            ),
         ),
	),
);

$app = new Yaf_Application($config);
print_r($app->getDispatcher()->getRouter()->getRoutes());
?>
--EXPECTF--
Array
(
    [_default] => Yaf_Route_Map Object
        (
            [_ctl_router:protected] => 1
            [_delimeter:protected] => ##
        )

)
