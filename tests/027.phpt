--TEST--
Check for Yaf autoload controller
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.library="/php/global/dir"
--FILE--
<?php 
$config = array(
		"application" => array(
			"directory" => realpath(dirname(__FILE__)),
			"dispatcher" => array(
				"catchException" => 0,
				"throwException" => 0,
				),
			),
		);

function main() {
    $dummy = new NoExistsController();
}

$app = new Yaf_Application($config);
$app->execute("main");
?>
--EXPECTF--
Warning: Yaf_Loader::autoload(): Could not find script %s/controllers/NoExists.php in %s027.php on line %d

Fatal error: Class 'NoExistsController' not found in %s027.php on line %d
