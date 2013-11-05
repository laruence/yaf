--TEST--
Check for multi inheritance of section
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php 
$config = new Yaf_Config_Ini(dirname(__FILE__) . "/multi-section.ini");
print_r($config->toArray());

?>
--EXPECTF--
Array
(
    [base] => Array
        (
            [base] => Array
                (
                    [a] => 1
                    [b] => 2
                )

        )

    [extra] => Array
        (
            [base] => Array
                (
                    [c] => 3
                )

        )

    [foo] => Array
        (
            [base] => Array
                (
                    [d] => 4
                )

        )

    [multi] => Array
        (
            [base] => Array
                (
                    [d] => 4
                    [a] => 1
                    [b] => 2
                    [c] => 3
                )

        )

    [] => Array
        (
            [base] => Array
                (
                    [d] => 4
                )

        )

)
