<!--
  Exception page rendered by ErrorController::errorAction. The
  dispatcher lands here when it catches an unhandled exception; the
  $exception variable was passed in via assign().
-->
<html>
<head>
<meta charset="utf-8">
<title>500 Error</title>
<link rel="stylesheet" href="/css/style.css">
</head>
<body>

<header class="top">
  <img class="logo" src="/img/yaf-logo.jpg" alt="Yaf logo">
  <div>
    <h1>500 — something went wrong</h1>
    <div class="tagline">Yaf caught an unhandled exception</div>
  </div>
</header>

<main class="content">
  <div class="card">
    <h2>Exception message</h2>
    <p><?php echo $exception->getMessage(); ?></p>
    <p class="muted">
      This page is rendered by ErrorController::errorAction because
      application.dispatcher.catchException is enabled; log the details
      in production and show a generic message instead.
    </p>
  </div>
</main>

<footer class="bottom">
  served by Yaf — the C-implemented PHP framework<br>
  <a href="https://www.php.net/manual/en/book.yaf.php">Yaf manual</a> ·
  <a href="https://github.com/laruence/yaf">Yaf on GitHub</a> ·
  <a href="https://www.laruence.com/">author homepage</a>
</footer>

</body>
</html>
