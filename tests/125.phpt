--TEST--
Sending multiple headers must not leak the formatted header lines
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
/* warm up the SAPI header state and allocator */
$r = new Yaf_Response_Http();
$r->setHeader("X-Warmup", "warm");
$r->response();

$m1 = memory_get_usage();
for ($i = 0; $i < 100; $i++) {
	$response = new Yaf_Response_Http();
	for ($n = 0; $n < 50; $n++) {
		$response->setHeader("X-Header-$n", "value-$n");
	}
	$response->response();
}
$m2 = memory_get_usage();

/* yaf_response_http_send() formats every header line with spprintf();
   sapi_header_op() copies the line, so the formatted buffer has to be
   freed on each iteration. With 50 headers per response, an unfixed
   build leaks 49 lines per response (~1.2MB total) */
var_dump($m2 - $m1 < 50000);
?>
--EXPECT--
bool(true)
