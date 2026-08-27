<?php
/**
 * {&$APP_NAME&} — router script for PHP's built-in web server.
 *
 * The built-in server serves real files and 404s everything else; it
 * knows nothing about rewrite rules. This script fills that gap: real
 * files under public/ are served as-is, every other request is handed
 * to public/index.php for Yaf to route:
 *
 *   php -S 127.0.0.1:8000 -t public router.php
 *
 * It is only needed for that quick try. Under a real web server the
 * rewrite rules take over and this file is not used:
 *   - Apache: public/.htaccess
 *   - nginx:  nginx.conf at the top level of this skeleton
 */
$path = parse_url($_SERVER["REQUEST_URI"], PHP_URL_PATH);
if ($path !== "/" && is_file(__DIR__ . "/public" . $path)) {
    return false;
}
require __DIR__ . "/public/index.php";
