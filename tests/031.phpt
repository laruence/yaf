--TEST--
Check for application.dispatcher.defaultRoute
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.library="/php/global/dir"
yaf.use_namespace=0
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
var_export($app->getDispatcher()->getRouter()->getRoutes());
?>
--EXPECTF--
Array
(
    [_default] => Yaf_Route_Map Object
        (
            [ctl_prefer:protected] => 1
            [delimiter:protected] => ##
        )

)
array (
  '_default' => 
  Yaf_Route_Map::__set_state(array(
     'ctl_prefer:protected' => true,
     'delimiter:protected' => '##',
  )),
)
