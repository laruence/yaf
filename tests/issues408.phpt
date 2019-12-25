--TEST--
ISSUE #408 ( 支持控制器文件名称多字母驼峰命名风格 )
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
yaf.use_namespace=1
--FILE--
<?php
require "build.inc";
startup();

$config = array(
	"application" => array(
		"directory" => APPLICATION_PATH,
	),
);

file_put_contents(APPLICATION_PATH . "/controllers/GetUserInfo.php", <<<PHP
<?php
   class GetUserInfoController extends Yaf\Controller_Abstract {
         public function indexAction() {
            var_dump("GetUserInfoController");
			return false;
         }
   }
PHP
);
$app = new Yaf\Application($config);
$module = "index";
$controller = "get-user-info";
$method = "index";
$app->getDispatcher()->dispatch(new Yaf\Request\Simple("", $module, $controller,  $method,  []));
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECTF--
string(21) "GetUserInfoController"
