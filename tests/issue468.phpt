--TEST--
ISSUE #468 Check for same name variables assignment
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
	),
);


file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         protected function init() {
            \$this->getView()->assign("ref", "first-source");
		 }

         public function indexAction() {
            \$this->getView()->assign("ref", "second-source");
            \$this->getView()->display("index/index.phtml", ["ref" => "thrid-source"]);
            return false;
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/views/index/index.phtml", "<?php
   var_dump(\$ref);
?>");

$app = new Yaf_Application($config);
$app->run();

?>
--CLEAN--
<?php
require "build.inc";
shutdown();
?>
--EXPECTF--
string(12) "thrid-source"
