<?php

function shutdown($root) {
   $dp = opendir($root);
   while (($dir = readdir($dp))) {
       if (in_array($dir, array('.', '..'))) {
           continue;
       }
       if (is_dir($root . '/' . $dir)) {
           shutdown($root . '/' . $dir);
       } else {
           unlink($root . '/' . $dir);
       }
   }
   rmdir($root);
}

function startup($root) {
    mkdir($root);
    mkdir($root . "/controllers");
    mkdir($root . "/plugins");
    mkdir($root . "/models");
    mkdir($root . "/views");
    mkdir($root . "/views/index");
    register_shutdown_function("shutdown", $root);
}

?>
