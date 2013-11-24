--TEST--
Check for Yaf_Response
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php 
$response = new Yaf_Response_Cli();


$body  = <<<HTML
ifjakdsljfklasdjfkljasdkljfkljadsf
HTML;

$string = "laruence";

$response->appendBody($body);
$response->prependBody($string);
$response->appendBody("kfjdaksljfklajdsfkljasdkljfkjasdf");

$body = $response->getBody();
unset($body);

var_dump(Yaf_Response_Abstract::DEFAULT_BODY);
print_r($response->getBody(NULL));
debug_zval_dump($response->getBody(Yaf_Response_Http::DEFAULT_BODY));
unset($string);
debug_zval_dump($response->getBody());
echo $response;
debug_zval_dump($response->getBody());
$response->response();
debug_zval_dump($response->getBody());
?>
--EXPECTF--
string(7) "content"
Array
(
    [content] => laruenceifjakdsljfklasdjfkljasdkljfkljadsfkfjdaksljfklajdsfkljasdkljfkjasdf
)
string(75) "laruenceifjakdsljfklasdjfkljasdkljfkljadsfkfjdaksljfklajdsfkljasdkljfkjasdf" refcount(1)
string(75) "laruenceifjakdsljfklasdjfkljasdkljfkljadsfkfjdaksljfklajdsfkljasdkljfkjasdf" refcount(1)
laruenceifjakdsljfklasdjfkljasdkljfkljadsfkfjdaksljfklajdsfkljasdkljfkjasdfstring(75) "laruenceifjakdsljfklasdjfkljasdkljfkljadsfkfjdaksljfklajdsfkljasdkljfkjasdf" refcount(1)
laruenceifjakdsljfklasdjfkljasdkljfkljadsfkfjdaksljfklajdsfkljasdkljfkjasdfstring(75) "laruenceifjakdsljfklasdjfkljasdkljfkljadsfkfjdaksljfklajdsfkljasdkljfkjasdf" refcount(1)
