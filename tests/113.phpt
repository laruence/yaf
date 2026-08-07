--TEST--
Yaf_View_Simple::eval with empty or uncompilable template should not crash
--SKIPIF--
<?php
if (!extension_loaded("yaf")) print "skip";
?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$view = new Yaf_View_Simple(__DIR__);

var_dump($view->eval(''));
var_dump($view->eval('plain text'));

$view->assign('name', 'yaf');
var_dump($view->eval('hello <?php echo $name; ?>'));

try {
    $view->eval('<?php if(');
} catch (ParseError $e) {
    echo "ParseError\n";
}
?>
--EXPECT--
NULL
string(10) "plain text"
string(9) "hello yaf"
ParseError
