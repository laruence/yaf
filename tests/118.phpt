--TEST--
Yaf_Route_Regex should not leave map/reverse uninitialized on failed construction
--SKIPIF--
<?php
if (!extension_loaded("yaf")) print "skip";
if (version_compare(PHP_VERSION, '7.2.0', '<')) die("skip requires throwing parameter parse");
?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$s = "/r/" . str_repeat("y", 8);
$r = new Yaf_Route_Regex("#^/x#", array("controller" => "c"), NULL, NULL, $s);
unset($r);

try {
    /* missing required $route argument, constructor bails before init */
    new Yaf_Route_Regex("#x#");
} catch (Throwable $e) {
}

/* the freed object block is reused; a sane allocator must not have
   released $s through a stale reverse pointer */
$x = str_repeat("z", 8);
echo strlen($s), ":", $s === $x ? "CORRUPT" : "ok", "\n";
?>
--EXPECT--
11:ok
