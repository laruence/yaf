--TEST--
Check for Yaf_Response_Cli APIs
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php 
$response = new Yaf_Response_Cli();


$body  = <<<HTML
yaf
HTML;

$string = "header|";

$response->appendBody($body);
$response->prependBody($string);
$response->appendBody("|footer");

$body = $response->getBody();
unset($body);

var_dump(Yaf_Response_Abstract::DEFAULT_BODY);
print_r($response->getBody(NULL));
var_dump($response->getBody(Yaf_Response_Http::DEFAULT_BODY));
unset($string);
var_dump($response->getBody());
echo($response);
echo "\n";
var_dump($response->getBody());
$response->response();
echo "\n";
var_dump($response->getBody());
?>
--EXPECTF--
string(7) "content"
Array
(
    [content] => header|yaf|footer
)
string(17) "header|yaf|footer"
string(17) "header|yaf|footer"
header|yaf|footer
string(17) "header|yaf|footer"
header|yaf|footer
string(17) "header|yaf|footer"
