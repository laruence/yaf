--TEST--
Check for Yaf_Route_Static with arbitrary urls
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_namespace=0
--FILE--
<?php
$url = array(
  "/", "/foo", "/foo/", "/foo///bar", "foo/bar", "/foo/bar/",
  "/foo/bar/dummy", "/foo///bar/dummy/", "foo/bar/dummy/",
  "/my", "/my/", "/my/foo", "/my/foo/", "my/foo/bar", "my/foo/bar/",
  "/m/index/index", "/my/foo/bar/dummy/1", "my/foo/bar/dummy/1/a/2/////",
  "/my/index/index", "my/index", "/foo/index", "index/foo",
);

$config = array( 
    "application" => array(
        "directory" => '/tmp/',
        "modules"   => 'Index,My',
     ),
);

$app = new Yaf_Application($config);

$route = Yaf_Dispatcher::getInstance()->getRouter();

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

ini_set("yaf.action_prefer", 1);
$url = array(
  "/", "/foo", "/foo/",
  "/my", "/my/", "/my/foo", "/my//foo/", 
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
--EXPECTF--
/ : m=> c=> a=>
/foo : m=> c=>Foo a=>
/foo/ : m=> c=>Foo a=>
/foo///bar : m=> c=>Foo a=>bar
foo/bar : m=> c=>Foo a=>bar
/foo/bar/ : m=> c=>Foo a=>bar
/foo/bar/dummy : m=> c=>Foo a=>bar args=>dummy->,
/foo///bar/dummy/ : m=> c=>Foo a=>bar args=>dummy->,
foo/bar/dummy/ : m=> c=>Foo a=>bar args=>dummy->,
/my : m=> c=>My a=>
/my/ : m=> c=>My a=>
/my/foo : m=> c=>My a=>foo
/my/foo/ : m=> c=>My a=>foo
my/foo/bar : m=>My c=>Foo a=>bar
my/foo/bar/ : m=>My c=>Foo a=>bar
/m/index/index : m=> c=>M a=>index args=>index->,
/my/foo/bar/dummy/1 : m=>My c=>Foo a=>bar args=>dummy->1,
my/foo/bar/dummy/1/a/2///// : m=>My c=>Foo a=>bar args=>dummy->1,a->2,
/my/index/index : m=>My c=>Index a=>index
my/index : m=> c=>My a=>index
/foo/index : m=> c=>Foo a=>index
index/foo : m=> c=>Index a=>foo
/ : m=> c=> a=>
/foo : m=> c=> a=>foo
/foo/ : m=> c=> a=>foo
/my : m=> c=> a=>my
/my/ : m=> c=> a=>my
/my/foo : m=> c=>My a=>foo
/my//foo/ : m=> c=>My a=>foo
