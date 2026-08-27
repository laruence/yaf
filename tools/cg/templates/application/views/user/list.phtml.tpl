<!--
  View template for UserController::listAction, auto-rendered from
  views/user/list.phtml. The $id variable was passed in via assign().
-->
<html>
<head>
<title>User #<?=$id?></title>
</head>
<body>
<p>
<?php
// $id comes from the :id placeholder of the rewrite route /user/:id
echo "This is user ", $id;
?>
</p>
</body>
</html>
