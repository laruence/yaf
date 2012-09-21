<?php

function shutdown($root) {
   $dp = opendir($root);
   while (($dir = readdir($dp))) {
       if (in_array($dir, array('.', '..'))) {
           continue;
       }
       if (is_dir($root . DIRECTORY_SEPARATOR  . $dir)) {
           shutdown($root . DIRECTORY_SEPARATOR . $dir);
       } else {
           unlink($root . DIRECTORY_SEPARATOR  . $dir);
       }
   }
   rmdir($root);
}

function startup($root) {
    mkdir($root);
    mkdir($root . DIRECTORY_SEPARATOR . "controllers");
    mkdir($root . DIRECTORY_SEPARATOR . "plugins");
    mkdir($root . DIRECTORY_SEPARATOR . "models");
    mkdir($root . DIRECTORY_SEPARATOR . "views");
    mkdir($root . DIRECTORY_SEPARATOR . "views" . DIRECTORY_SEPARATOR  . "index");
    register_shutdown_function("shutdown", $root);
}

?>
