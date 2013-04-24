--TEST--
Check for Yaf_Response::setBody/prependBody/appendBody
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.library="/php/global/dir"
yaf.use_namespace=0
--FILE--
<?php 
$response = new Yaf_Response_Http();
$response->setBody("ell")->appendBody("o")->setBody(" W", "footer")->prependBody("H")->appendBody("orld", "footer");
print_r($response);
echo $response;
?>
--EXPECTF--
Yaf_Response_Http Object
(
    [_header:protected] => Array
        (
        )

    [_body:protected] => Array
        (
            [content] => Hello
            [footer] =>  World
        )

    [_sendheader:protected] => 1
    [_response_code:protected] => 200
)
Hello World
