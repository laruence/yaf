--TEST--
Bug #76217 (Memory leaks with Yaf_Dispatcher::setDefault*)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
require "build.inc";
startup();

file_put_contents(APPLICATION_PATH . "/Bootstrap.php", <<<PHP
<?php
class Bootstrap extends Yaf_Bootstrap_Abstract
{
    public function _initView(Yaf_Dispatcher \$dispatcher)
    {
        \$dispatcher->setDefaultModule("index")
                    ->setDefaultController("admin")
                    ->setDefaultAction("login");
    }
}
PHP
);

file_put_contents(APPLICATION_PATH . "/controllers/Admin.php", <<<PHP
<?php
class AdminController extends Yaf_Controller_Abstract
{
     public function loginAction()
     {
         var_dump("index/admin/login");
         return false;
     }
}
PHP
);

$config = array(
    "application" => array(
        "directory" => APPLICATION_PATH,
        "modules" => "Index,User,Admin",
    ),
);

$app = new Yaf_Application($config);
$app->bootstrap()->run();
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECTF--
string(%d) "%s"
