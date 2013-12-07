--TEST--
check for Yaf_Response_Http::setHeader() and Yaf_Response_Http::getHeader() 
and Yaf_Response_Http::setAllHeaders() and Yaf_Response_Http::clearHeaders()
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$response = new Yaf_Response_Http();

var_dump($response->setHeader('MyName1', 'Header1'));
var_dump($response->setHeader('MyName2', 'Header2'));
var_dump($response->setHeader('MyName2', 'Header22'));
var_dump($response->setHeader('MyName1', 'Header11', false));
print_r($response);

var_dump($response->setHeader('MyName1', 'Header1'));
var_dump($response->setHeader('MyName3', 'Header31', false, 301));
var_dump($response->setHeader('MyName3', 'Header32', true, 302));
var_dump($response->setHeader('MyName1', 'Header2', false, 302));

var_dump($response->getHeader());
var_dump($response->getHeader('MyName'));
var_dump($response->getHeader('MyName1'));
print_r($response->clearHeaders());

$headers = array(
    'MyName1' => 'Header1x',
    'MyName2' => 'Header2x',
    'MyName3' => 12345
);
var_dump($response->setAllHeaders($headers));
var_dump($response->getHeader());

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
Yaf_Response_Http Object
(
    [_header:protected] => Array
        (
            [MyName1] => Header1, Header11
            [MyName2] => Header22
        )

    [_body:protected] => Array
        (
        )

    [_sendheader:protected] => 1
    [_response_code:protected] => 200
)
bool(true)
bool(true)
bool(true)
bool(true)
array(3) {
  ["MyName1"]=>
  string(16) "Header1, Header2"
  ["MyName2"]=>
  string(8) "Header22"
  ["MyName3"]=>
  string(8) "Header32"
}
NULL
string(16) "Header1, Header2"
Yaf_Response_Http Object
(
    [_header:protected] => Array
        (
        )

    [_body:protected] => Array
        (
        )

    [_sendheader:protected] => 1
    [_response_code:protected] => 302
)
bool(true)
array(3) {
  ["MyName1"]=>
  string(8) "Header1x"
  ["MyName2"]=>
  string(8) "Header2x"
  ["MyName3"]=>
  string(5) "12345"
}
