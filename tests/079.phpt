--TEST--
Invalid read in autoloading
--SKIPIF--
<?php
if (!extension_loaded("yaf")) die("skip"); 
?>
--INI--
yaf.use_namespace=0
yaf.library=
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
         public function indexAction() {
			exit(Test_Tool::get("a"));
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/library/Test/Tool.php", <<<PHP
<?php
   class Test_Tool {
         public static function get(\$name) {
		 	\$a = Test_Dict::dummy();
			/* tigger this_var is generated */
			return \$\$name;
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/library/Test/Dict.php", <<<PHP
<?php
   class Test_Dict {
         public static function dummy() {
		 	return "okey";
         }
   }
PHP
);

$app = new Yaf_Application($config);
$request = new Yaf_Request_Simple();
$app->getDispatcher()->dispatch($request);
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECT--
okey
