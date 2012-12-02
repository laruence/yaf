--TEST--
Check for Yaf_Route_Static with arbitrary urls
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
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
/foo : m=> c=>foo a=>
/foo/ : m=> c=>foo a=>
/foo///bar : m=> c=>foo a=>bar
foo/bar : m=> c=>foo a=>bar
/foo/bar/ : m=> c=>foo a=>bar
/foo/bar/dummy : m=> c=>foo a=>bar args=>dummy->,
/foo///bar/dummy/ : m=> c=>foo a=>bar args=>dummy->,
foo/bar/dummy/ : m=> c=>foo a=>bar args=>dummy->,
/my : m=> c=>my a=>
/my/ : m=> c=>my a=>
/my/foo : m=> c=>my a=>foo
/my/foo/ : m=> c=>my a=>foo
my/foo/bar : m=>my c=>foo a=>bar
my/foo/bar/ : m=>my c=>foo a=>bar
/m/index/index : m=> c=>m a=>index args=>index->,
/my/foo/bar/dummy/1 : m=>my c=>foo a=>bar args=>dummy->1,
my/foo/bar/dummy/1/a/2///// : m=>my c=>foo a=>bar args=>dummy->1,a->2,
/my/index/index : m=>my c=>index a=>index
my/index : m=> c=>my a=>index
/foo/index : m=> c=>foo a=>index
index/foo : m=> c=>index a=>foo
/ : m=> c=> a=>
/foo : m=> c=> a=>foo
/foo/ : m=> c=> a=>foo
/my : m=> c=>my a=>
/my/ : m=> c=>my a=>
/my/foo : m=> c=>my a=>foo
/my//foo/ : m=> c=>my a=>foo
