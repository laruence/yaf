<?php
define('DIR_ROOT', dirname(__FILE__));

define("APPLICATION_PATH", DIR_ROOT . '/application');

function shutdown($root = APPLICATION_PATH) {
    $dp = opendir($root);
    while (($dir = readdir($dp))) {
        if (in_array($dir, array('.', '..'))) {
            continue;
        }
        $path = $root . DIRECTORY_SEPARATOR . $dir;

        if (is_dir($path)) {
            shutdown($path);
        } else if (is_file($path)) {
            unlink($path);
        }
    }
    rmdir($root);
}

function startup($root = APPLICATION_PATH) {
    $dirs = array();
    $dirs[] = 'library';
    $dirs[] = 'controllers';
	$dirs[] = 'actions';
    $dirs[] = 'plugins';
    $dirs[] = 'models';
    $dirs[] = 'views';
    $dirs[] = 'views/index';
    $dirs[] = 'tpls';

    foreach($dirs as $dir){
        $dir = $root . DIRECTORY_SEPARATOR . $dir;
        if (!file_exists($dir)) {
            mkdir($dir, 0755, true);
            chmod($dir, 0755);
        }
    }
}

?>
