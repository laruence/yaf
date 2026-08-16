<?php
define('DIR_ROOT', dirname(__FILE__));

/* Tests may run in parallel and every one of them shares this helper, so each
   test gets its own application directory.  run-tests.php executes the FILE
   section as tests/<name>.php and the CLEAN section as tests/<name>.clean.php
   (both inside DIR_ROOT), so deriving the key from the running script name
   makes the two sections resolve to the same directory. */
function yaf_test_app_name() {
    $script = '';
    if (isset($_SERVER['SCRIPT_FILENAME'])) {
        $script = $_SERVER['SCRIPT_FILENAME'];
    } elseif (isset($_SERVER['argv'][0])) {
        $script = $_SERVER['argv'][0];
    }
    $name = preg_replace('/(\.?clean|\.?skip|\.?preload)?\.php$/i', '', basename($script));
    $name = rtrim($name, '.');
    if ($name === '') {
        /* not run through run-tests.php: still keep concurrent runs apart */
        $name = 'manual-' . getmypid();
    }
    return $name;
}

define("APPLICATION_PATH", DIR_ROOT . DIRECTORY_SEPARATOR . 'application' . DIRECTORY_SEPARATOR . yaf_test_app_name());

function shutdown($root = APPLICATION_PATH) {
    if (!is_dir($root)) {
        return;
    }
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
    closedir($dp);
    rmdir($root);

    /* also drop the shared "application" parent once this test's own
       directory is gone (best effort: other tests may still use it) */
    if ($root === APPLICATION_PATH) {
        @rmdir(dirname($root));
    }
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
