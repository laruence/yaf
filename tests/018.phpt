--TEST--
Bug Yaf_Config_Ini crash due to inaccurate refcount
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--FILE--
<?php 
$file = dirname(__FILE__) . "/simple.ini";
$config = new Yaf_Config_Ini($file, 'base');
print_r($config);
?>
--EXPECTF--
Yaf_Config_Ini Object
(
    [_config:protected] => Array
        (
            [application] => Array
                (
                    [directory] => APPLICATION_PATH/applcation
                )

            [name] => base
            [array] => Array
                (
                    [1] => 1
                    [name] => name
                )

            [5] => 5
            [routes] => Array
                (
                    [regex] => Array
                        (
                            [type] => regex
                            [match] => ^/ap/(.*)
                            [route] => Array
                                (
                                    [controller] => Index
                                    [action] => action
                                )

                            [map] => Array
                                (
                                    [0] => name
                                    [1] => name
                                    [2] => value
                                )

                        )

                    [simple] => Array
                        (
                            [type] => simple
                            [controller] => c
                            [module] => m
                            [action] => a
                        )

                    [supervar] => Array
                        (
                            [type] => supervar
                            [varname] => c
                        )

                    [rewrite] => Array
                        (
                            [type] => rewrite
                            [match] => /yaf/:name/:value
                            [route] => Array
                                (
                                    [controller] => Index
                                    [action] => action
                                )

                        )

                )

        )

    [_readonly:protected] => 1
)
