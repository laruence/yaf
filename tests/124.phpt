--TEST--
Bootstrap _init* return values must not leak
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$config = array(
	"application" => array(
		"directory" => realpath(dirname(__FILE__)),
	),
);

class Bootstrap extends Yaf_Bootstrap_Abstract {
	/* returning $this keeps an extra reference per bootstrap() call
	   if the dispatcher does not drop the return value */
	public function _initLeak(Yaf_Dispatcher $dispatcher) {
		return $this;
	}
}

$app = new Yaf_Application($config);
$m1 = memory_get_usage();
for ($i = 0; $i < 500; $i++) {
	$app->bootstrap();
}
$m2 = memory_get_usage();

/* the leaked object is small (~40 bytes, no properties table), so it
   takes 500 iterations (~20KB) to rise clearly above the threshold */
var_dump($m2 - $m1 < 10000);
?>
--EXPECT--
bool(true)
