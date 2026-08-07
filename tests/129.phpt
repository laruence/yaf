--TEST--
Yaf_View_Simple must handle more than eight template variables
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$view = new Yaf_View_Simple(realpath(dirname(__FILE__)));

$vars = array();
for ($i = 0; $i < 20; $i++) {
	$vars["v" . $i] = $i;
}

echo $view->eval("<?php echo \$v0 + \$v19; ?>", $vars), "\n";
echo $view->eval("<?php echo \$v7 . \$v8; ?>", array("v7" => "a", "v8" => "b")), "\n";
?>
--EXPECT--
19
ab
