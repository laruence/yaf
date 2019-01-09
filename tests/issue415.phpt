--TEST--
Issue #415 ($actions changed to be reference)
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
		public \$actions = [
			'index' => 'need init',
		];

		public function init() {
			// foreach (\$this->actions as \$action => \$path) {
			// 	\$this->actions[\$action] = 'actions/IndexAction.php';
			// }
			array_walk(\$this->actions, function(\$path, \$action) {
				\$this->actions[\$action] = 'actions/IndexAction.php';
			});
		}
   }
PHP
);

@mkdir(APPLICATION_PATH . "/actions/");
file_put_contents(APPLICATION_PATH . "/actions/IndexAction.php", <<<PHP
<?php
   class IndexAction extends Yaf_Action_Abstract {
			public function execute() {
				echo "Okey";
				return FALSE;
			}
   }
PHP
);

$app = new Yaf_Application($config);
$response = $app->run();
?>
--CLEAN--
<?php
require "build.inc"; 
shutdown();
?>
--EXPECT--
Okey
