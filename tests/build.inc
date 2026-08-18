<?php
define('DIR_ROOT', dirname(__FILE__));

/* Tests may run in parallel, so each test runs in its own application root
   tests/application_<name>, where <name> is the test's own file name:
   startup()/shutdown() take the caller's __FILE__ and keep the part before
   the first dot, so the FILE section (tests/<name>.php) and the CLEAN
   section (tests/<name>.clean.php) resolve to the same root.

   Anything a test writes outside of APPLICATION_PATH must live in a
   file/directory suffixed with the test's own name, so parallel workers
   never touch the same path. */

function startup($file) {
    $root = DIR_ROOT . DIRECTORY_SEPARATOR . 'application_' . explode('.', basename($file))[0];
    if (!defined('APPLICATION_PATH')) {
        define('APPLICATION_PATH', $root);
    }
    foreach (array('library', 'controllers', 'actions', 'plugins', 'models', 'views', 'views/index', 'tpls') as $dir) {
        $dir = $root . DIRECTORY_SEPARATOR . $dir;
        if (!is_dir($dir) && !@mkdir($dir, 0755, TRUE) && !is_dir($dir)) {
            throw new RuntimeException("yaf tests: unable to create directory '$dir'");
        }
    }
}

function shutdown($file) {
    yaf_rmtree(DIR_ROOT . DIRECTORY_SEPARATOR . 'application_' . explode('.', basename($file))[0]);
}

function yaf_rmtree($dir) {
    if (!is_dir($dir)) {
        return;
    }
    $dp = opendir($dir);
    while (($entry = readdir($dp)) !== FALSE) {
        if ($entry !== '.' && $entry !== '..') {
            $path = $dir . DIRECTORY_SEPARATOR . $entry;
            if (is_dir($path)) {
                yaf_rmtree($path);
            } else {
                unlink($path);
            }
        }
    }
    closedir($dp);
    rmdir($dir);
}
?>
