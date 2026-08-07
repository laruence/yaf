--TEST--
Yaf_Config_Ini should fail gracefully when the file exists but cannot be opened
--SKIPIF--
<?php
if (!extension_loaded("yaf")) print "skip";
if (substr(PHP_OS, 0, 3) == "WIN") die("skip permission bits not enforced on Windows");
$f = __DIR__ . '/114.tmp';
file_put_contents($f, 'a=b');
chmod($f, 0);
$fp = @fopen($f, 'r');
if ($fp) {
    fclose($fp);
    unlink($f);
    die("skip file permissions not enforced (root?)");
}
unlink($f);
?>
--INI--
yaf.throw_exception=1
yaf.use_namespace=0
--FILE--
<?php
$file = __DIR__ . '/114.tmp';
file_put_contents($file, "foo=bar\n");
chmod($file, 0);

try {
    $config = new Yaf_Config_Ini($file);
    echo "unexpected\n";
} catch (Exception $e) {
    echo strpos($e->getMessage(), 'Unable to open config file') !== false ? "caught\n" : $e->getMessage();
}

chmod($file, 0644);
var_dump(new Yaf_Config_Ini($file) instanceof Yaf_Config_Ini);
unlink($file);
?>
--EXPECT--
caught
bool(true)
--CLEAN--
<?php
@unlink(__DIR__ . '/114.tmp');
?>
