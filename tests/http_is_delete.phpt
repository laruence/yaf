--TEST--
Check isDelete
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--

--FILE--
<?php 
include 'php_cli_server.inc';


define('APPLICATION_PATH',  __DIR__);

file_put_contents(APPLICATION_PATH . "/Bootstrap.php", <<<PHP
<?php
   class Bootstrap extends Yaf_Bootstrap_Abstract {};
PHP
);
$controllerDir = APPLICATION_PATH . '/controllers';
if (!is_dir($controllerDir)) {
  mkdir($controllerDir, 0777);
} 
file_put_contents(APPLICATION_PATH . "/controllers/Index.php", <<<PHP
<?php
   class IndexController extends Yaf_Controller_Abstract {
         public function indexAction() {
             var_dump(\$this->getRequest()->isDelete());
             return false;
         }
   }
PHP
);

file_put_contents(APPLICATION_PATH . '/index.php', <<<PHP
<?php
define('APPLICATION_PATH',  __DIR__);
\$config = array(
	"application" => array(
		"directory" => APPLICATION_PATH,
	),
);
\$app = new Yaf_Application(\$config);
\$app->bootstrap()->run();

PHP
);


$handle = php_cli_server_start(null, "index.php", "-c php.ini");

list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
$port = intval($port)? :80;
$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}
$returnStr = '';
if(fwrite($fp, <<<HEADER
DELETE / HTTP/1.1
Host: {$host}


HEADER
)) {
	while (!feof($fp)) {
		$returnStr .= fgets($fp);
	}
}

fclose($fp);
list($header, $body) = explode("\r\n\r\n", $returnStr);
echo $body;

--CLEAN--
<?php
unlink(__DIR__ . "index.php");
unlink(__DIR__ . "controllers/Index.php");
unlink(__DIR__ . 'Bootstrap.php');
rmdir(__DIR__ . 'controllers);
php_cli_server_stop($handle);

--EXPECTF--
bool(true)

