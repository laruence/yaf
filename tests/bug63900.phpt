--TEST--
Bug #63900 (Segfault if separated action executes failed)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=1
yaf.lowcase_path=0
yaf.throw_exception=0
yaf.catch_exception=1
yaf.use_namespace=0
--FILE--
<?php 
require "build.inc";
startup();

$config = array(
	"application" => array(
		"directory" => APPLICATION_PATH,
	),
);

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
       public \$actions = array(
          "index" => "controllers/IndexAction.php",
       );
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/controllers/IndexAction.php", <<<PHP
<?php
   class IndexAction extends Yaf_Action_Abstract {
       public function execute() {
            return FALSE;
       }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/views/index/index.phtml", "Okey");

$app = new Yaf_Application($config);
$response = $app->run();
echo "Okey";
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECTF--
Okey
