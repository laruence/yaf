--TEST--
yaf_trigger_error must release previous err_msg before overwriting it
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
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

function error_handler($errno, $errstr) {
	/* swallow the recoverable error so dispatching can go on */
}

$app = new Yaf_Application($config);
$app->getDispatcher()->setErrorHandler("error_handler", E_RECOVERABLE_ERROR);
$app->getDispatcher()->returnResponse(true);

/* first dispatch stores the error message in the application */
$app->getDispatcher()->dispatch(new Yaf_Request_Http("/no/such"));
$m1 = memory_get_usage();
/* every further trigger overwrites app->err_msg; the previous message
   must be released instead of leaked */
for ($i = 0; $i < 50; $i++) {
	$app->getDispatcher()->dispatch(new Yaf_Request_Http("/no/such"));
}
$m2 = memory_get_usage();

var_dump($m2 - $m1 < 1000);
?>
--EXPECT--
bool(true)
