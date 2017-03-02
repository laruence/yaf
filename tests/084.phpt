--TEST--
Check request methods
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php 

function checkMethod($method) {
	$request = new Yaf_Request_Simple($method, "index", "dummy", NULL, array());
	echo "checking {$method}...\n";
	var_dump($request->isGet());
	var_dump($request->isPost());
	var_dump($request->isHead());
	var_dump($request->isDelete());
	var_dump($request->isPut());
	var_dump($request->isOptions());
	var_dump($request->isPatch());
	return;
}

checkMethod("GET");
checkMethod("POST");
checkMethod("HEAD");
checkMethod("DELETE");
checkMethod("PUT");
checkMethod("OPTIONS");
checkMethod("PATCH");
?>
--EXPECTF--
checking GET...
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
checking POST...
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
checking HEAD...
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
checking DELETE...
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
checking PUT...
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
checking OPTIONS...
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
checking PATCH...
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
