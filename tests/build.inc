<?php

function shutdown($root) {
    @rmdir($root . "/application/views/index");
    @rmdir($root . "/application/views");
    @rmdir($root . "/application/plugins");
    @rmdir($root . "/application/models");
    @rmdir($root . "/application/controllers");
    @rmdir($root . "/application");
}

function startup($root) {
    @mkdir($root . "/application");
    @mkdir($root . "/application/controllers");
    @mkdir($root . "/application/plugins");
    @mkdir($root . "/application/models");
    @mkdir($root . "/application/views");
    @mkdir($root . "/application/views/index");
    register_shutdown_function("shutdown", $root);
}

?>
