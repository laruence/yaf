--TEST--
Check for numeric keys in view assign
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
	),
);

file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         public function init() {}

         public function indexAction() {
            \$this->getView()->assign("var", "str key");
            \$this->getView()->assign(1, "number key");
            \$this->getView()->assign(2.3, "float key");
            \$this->getView()->assign(array(), "array key");
			\$this->getView()->assign(array(1 => "number key", 3.4 => "float key"));
            \$this->getView()->display("index/index.phtml", [-1 => "number key", "0x2342" => "hex key"]);
            return false;
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . "/views/index/index.phtml", "<?php
var_dump(get_defined_vars());
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
Warning: Yaf_View_Simple::assign() expects parameter 1 to be string, array given in %sIndex.php on line %d
array(1) {
  ["var"]=>
  string(7) "str key"
}
