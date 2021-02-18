--TEST--
ISSUE #535 (Segsev while throw exception in action)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
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
				"index" => "actions/index.php",
			);
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/actions/index.php", <<<PHP
<?php
   class IndexAction extends Yaf_Action_Abstract {
         public function execute() {
			\$x  = new X();
			\$this->getView()->assign("result", \$x->test());
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/library/X.php", <<<PHP
<?php
	class X extends Y {
		public static function test() {
			return "okey";
        }
    }
PHP
);

file_put_contents(APPLICATION_PATH . "/library/Y.php", <<<PHP
<?php
	class Y {
    }
PHP
);

file_put_contents(APPLICATION_PATH . "/views/index/index.phtml", "<?=\$result?>" . PHP_EOL);

$app = new Yaf_Application($config);
$app->run();
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECT--
okey
