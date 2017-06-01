<?php
ini_set("display_errors", "On");
error_reporting(E_ALL);
define("APPLICATION_PATH",  dirname(dirname(__FILE__)));

$app  = new Yaf_Application(APPLICATION_PATH . "/conf/application.ini");
$app->bootstrap() //call bootstrap methods defined in Bootstrap.php
    ->run();
?>