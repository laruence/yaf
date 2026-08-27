<!--
  Exception page rendered by ErrorController::errorAction. The
  dispatcher lands here when it catches an unhandled exception; the
  $exception variable was passed in via assign().
-->
<html>
<head>
<title>500 Error</title>
</head>
<body>
<div>
<?php
echo "Yaf Caught exception : ";
?>
</div>
<div>
<?php
echo "Exception Message: ", $exception->getMessage();
?>
</div>
</body>
</html>
