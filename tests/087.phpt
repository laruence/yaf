--TEST--
Check for Yaf_Route_Map with arbitrary urls
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$url = array(
  "/", "/foo", "/foo/", "/foo///bar", "//foo//bar", "/foo/bar///",
  "/foo/bar/:", "/foo/bar/:/", "/foo///bar/:/name/laruence/age/32",
  "foo/bar/:dummy/value:/age/37",
);

$route = new Yaf_Route_Map(true);

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

$route = new Yaf_Route_Map(false, ":");

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
--EXPECT--
/ : m=> c=> a=>
/foo : m=> c=>Foo a=>
/foo/ : m=> c=>Foo a=>
/foo///bar : m=> c=>Foo_Bar a=>
//foo//bar : m=> c=>Foo_Bar a=>
/foo/bar/// : m=> c=>Foo_Bar a=>
/foo/bar/: : m=> c=>Foo_Bar_: a=>
/foo/bar/:/ : m=> c=>Foo_Bar_: a=>
/foo///bar/:/name/laruence/age/32 : m=> c=>Foo_Bar_:_Name_Laruence_Age_32 a=>
foo/bar/:dummy/value:/age/37 : m=> c=>Foo_Bar_:dummy_Value:_Age_37 a=>
/ : m=> c=> a=>
/foo : m=> c=> a=>foo
/foo/ : m=> c=> a=>foo
/foo///bar : m=> c=> a=>foo_bar
//foo//bar : m=> c=> a=>foo_bar
/foo/bar/// : m=> c=> a=>foo_bar
/foo/bar/: : m=> c=> a=>foo_bar
/foo/bar/:/ : m=> c=> a=>foo_bar
/foo///bar/:/name/laruence/age/32 : m=> c=> a=>foo_bar args=>name->laruence,age->32,
foo/bar/:dummy/value:/age/37 : m=> c=> a=>foo_bar args=>dummy->value:,age->37,
