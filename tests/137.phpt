--TEST--
Yaf_Session must not dangle after $_SESSION is unset
--SKIPIF--
<?php if (!extension_loaded("session")) die("skip session required"); ?>
<?php if (!extension_loaded("yaf")) die("skip"); ?>
--INI--
yaf.use_namespace=0
session.save_handler=files
session.save_path=
--FILE--
<?php
$session = Yaf_Session::getInstance();
for ($i = 0; $i < 8; $i++) {
	$session->set("key$i", "value$i");
}
var_dump($session->has("key0"));
var_dump(count($session));

/* drop the only userland reference to the session array; yaf must not
   keep or dereference a stale pointer afterwards */
unset($_SESSION);

$noise = [];
for ($i = 0; $i < 2000; $i++) {
	$noise[] = str_repeat("z", 64);
}

var_dump($session->has("key0"));
var_dump($session->get("key7"));
var_dump($session->set("new", "value"));
var_dump($session->del("new"));
?>
--EXPECT--
bool(true)
int(8)
bool(false)
NULL
bool(false)
bool(false)
