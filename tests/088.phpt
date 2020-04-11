--TEST--
Check for Yaf_Route_Rwrite with arbitrary urls
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$url = array(
	"/", "/foo", "//foo/", "/foo/bar", "/foo///",
	"/foo/cont/act", "/foo/cont/act/", "/foocont/act", 
	"/foo/cont//act", "/foo//cont///act//",
    "/foo/cont/act//var/value/", "/foo/cont/act//var/value/age",
	"/foo/cont/act//var/value/age/12//",
);

$route = new Yaf_Route_Rewrite(
	"/foo/:name/:subname/*",
	array(
		"controller" => ":name",
		"action" => ":subname"
	)
);

foreach ($url as $u) {
   $req = new Yaf_Request_Http($u);
   $route->route($req);
   echo $u, " : ",  "m=>", $req->getModuleName(), " c=>", $req->getControllerName(), " a=>",  $req->getActionName();
   if (($args = $req->getParams())) {
       echo " args=>";
       foreach ($args as $k => $v) {
          echo $k , "->", $v , ",";
       }
   }
   echo "\n";
}
?>
--EXPECT--
/ : m=> c=> a=>
/foo : m=> c=> a=>
//foo/ : m=> c=> a=>
/foo/bar : m=> c=> a=>
/foo/// : m=> c=> a=>
/foo/cont/act : m=> c=> a=>
/foo/cont/act/ : m=> c=>Cont a=>act args=>name->cont,subname->act,
/foocont/act : m=> c=> a=>
/foo/cont//act : m=> c=> a=>
/foo//cont///act// : m=> c=>Cont a=>act args=>name->cont,subname->act,
/foo/cont/act//var/value/ : m=> c=>Cont a=>act args=>name->cont,subname->act,var->value,
/foo/cont/act//var/value/age : m=> c=>Cont a=>act args=>name->cont,subname->act,var->value,age->,
/foo/cont/act//var/value/age/12// : m=> c=>Cont a=>act args=>name->cont,subname->act,var->value,age->12,
