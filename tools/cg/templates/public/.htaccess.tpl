# {&$APP_NAME&} — Apache rewrite rules.
# Forward every request that does not match a real file to index.php,
# the single application entry point. Place this file in public/.
RewriteEngine On
RewriteCond %{REQUEST_FILENAME} !-f
RewriteRule .* index.php
