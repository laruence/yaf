<!--
  View template for IndexController::indexAction.
  Yaf_View_Simple auto-renders views/<controller>/<action>.phtml (both
  names lowercased); variables passed via assign() become local
  variables in the template ($app_name, $name, $records, $count).

  The stylesheet, script and logo below are served straight from
  public/css/, public/js/ and public/img/ by the web server — the
  rewrite rules only forward requests that do not match a real file.
-->
<html>
<head>
<meta charset="utf-8">
<title><?=$app_name?> — powered by Yaf</title>
<link rel="stylesheet" href="/css/style.css">
<script src="/js/app.js" defer></script>
</head>
<body>

<header class="top">
  <img class="logo" src="/img/yaf-logo.jpg" alt="Yaf logo">
  <div>
    <h1><?=$app_name?></h1>
    <div class="tagline">a Yaf skeleton application — C-speed PHP framework</div>
  </div>
</header>

<nav class="top">
  <a href="/">Home</a>
  <a href="/user/3">User #3</a>
</nav>

<main class="content">
  <div class="card">
    <h2>Hello <?=$name?>!</h2>
    <p>
      This page was dispatched by Yaf and rendered from
      <code>views/index/index.phtml</code>. The table below came from
      <code>SampleModel::find()</code> — a mock DAO with an in-memory
      "table", ready to be swapped for a real database driver.
    </p>
    <table class="records">
      <tr><th>ID</th><th>Name</th><th>Role</th></tr>
      <?php foreach ($records as $row): ?>
      <tr>
        <td><?=$row["id"]?></td>
        <td><?=$row["name"]?></td>
        <td><?=$row["role"]?></td>
      </tr>
      <?php endforeach; ?>
    </table>
    <p class="muted"><?=$count?> record(s) loaded.</p>
    <button id="visit">Visit a random user</button>
  </div>
</main>

<footer class="bottom">
  served by Yaf — the C-implemented PHP framework
</footer>

</body>
</html>
