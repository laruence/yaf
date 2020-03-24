--TEST--
PR #468 Check for sysboml table bug
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.use_namespace=0

--FILE--
<?php
require "build.inc";
startup();

$config = array(
	"application" => array(
		"directory" => APPLICATION_PATH,
        "dispatcher" => array (
           "catchException" => true,
        ),
        "library" => array(
        ),
	),
);

file_put_contents(APPLICATION_PATH . "/Bootstrap.php", <<<PHP
<?php
   class Bootstrap extends Yaf_Bootstrap_Abstract {
        public function _initConfig(Yaf_Dispatcher \$dispatcher) {
            Yaf_Registry::set("config", Yaf_Application::app()->getConfig());
        }
   }
PHP
);


$value = NULL;

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         public function init() {}

         public function indexAction() {
            \$this->getView()->assign("ref", "ref-source");
            \$this->getView()->display("index/index.phtml", ["ref" => "ref-changed"]);
            return false;
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/views/index/index.phtml", "<?php
   var_dump(\$ref);
?>");

$app = new Yaf_Application($config);
$app->bootstrap()->run();

?>
--CLEAN--
<?php
require "build.inc";
shutdown();
?>
--EXPECTF--
string(11) "ref-changed"

