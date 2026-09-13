--TEST--
Reject an oversized action mapping path safely
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
yaf.use_spl_autoload=0
--FILE--
<?php
require "build.inc";
startup(__FILE__);

$path = str_repeat("x", PHP_MAXPATHLEN);
file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
class IndexController extends Yaf_Controller_Abstract {
    public \$actions = array("foo" => "$path");
}
PHP
);

try {
    $app = new Yaf_Application(array("application" => array("directory" => APPLICATION_PATH)));
    $app->getDispatcher()->disableView();
    $request = new Yaf_Request_Simple();
    $request->setControllerName("index");
    $request->setActionName("foo");
    $app->getDispatcher()->dispatch($request);
} catch (Throwable $e) {
    echo get_class($e), ": ", $e->getMessage(), PHP_EOL;
}
?>
--CLEAN--
<?php
require "build.inc";
shutdown(__FILE__);
?>
--EXPECT--
Yaf_Exception_LoadFailed_Action: path too long while loading action 'foo'
