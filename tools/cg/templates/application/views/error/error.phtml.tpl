<!--
  Exception page rendered by ErrorController::errorAction. The
  dispatcher lands here when it catches an unhandled exception; the
  $exception variable was passed in via assign().
-->
<html>
<head>
<meta charset="utf-8">
<title><?=$code?> Error</title>
<link rel="stylesheet" href="/css/style.css">
</head>
<body>

<header class="top">
  <img class="logo" src="/img/yaf-logo.jpg" alt="Yaf logo">
  <div>
    <h1><?=$code?> — something went wrong</h1>
    <div class="tagline"><?php if ($code == 404): ?>Yaf could not find the requested page<?php else: ?>Yaf caught an unhandled exception<?php endif; ?></div>
  </div>
</header>

<nav class="top">
  <a href="/">Home</a>
</nav>

<main class="content">
  <div class="card">
    <h2>Exception message</h2>
    <p><?php echo $exception->getMessage(); ?></p>
    <p class="muted">
      This page is rendered by ErrorController::errorAction because
      application.dispatcher.catchException is enabled;
      <?php if ($code == 404): ?>the request matched no controller or action — check the URL.<?php else: ?>log the details in production and show a generic message instead.<?php endif; ?>
    </p>
  </div>
</main>

<footer class="bottom">
  served by Yaf — the C-implemented PHP framework<br>
  <a href="https://www.php.net/manual/en/book.yaf.php">Yaf manual</a> ·
  <a href="https://github.com/laruence/yaf">Yaf on GitHub</a> ·
  <a href="https://www.laruence.com/">Author homepage</a>
</footer>

</body>
</html>
