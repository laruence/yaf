<!--
  View template for IndexController::indexAction.
  Yaf_View_Simple auto-renders views/<controller>/<action>.phtml (both
  names lowercased); variables passed via assign() become local
  variables in the template ($header, $content, $name).
-->
<html>
<head>
<title><?=$header?></title>
</head>
<body>
<?php
echo $content, ", I am ", $name, ".";
?>

</body>
</html>
