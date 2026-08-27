Deploy and run this application as follows:

1. Make sure the Yaf extension is installed and loaded on
   {&$DEV_PC&} (php -m | grep yaf).
2. Copy the {&$APP_NAME&} directory to your web server, and point the
   DocumentRoot (or nginx root) at its public/ subdirectory, so that
   only public assets are reachable from the web.
3. Forward every request to public/index.php:
     - Apache: public/.htaccess already contains the rewrite rules
       (mod_rewrite required);
     - nginx: see nginx.conf at the top level of this skeleton.
4. In php.ini, set
       yaf.environ="product"
   so that the [product] section of conf/application.ini is loaded.
5. Restart the web server.
6. Browse http://yourhost/ — a styled page with the Yaf crab logo and
   a record table loaded through SampleModel means it works. Try
   http://yourhost/index/index/user/id/2 for the model demo (any id works;
   unknown ids show a "not found" row). Otherwise check the PHP error
   log.

Quick try without a web server (PHP's built-in server):

  php -S 127.0.0.1:8000 -t public router.php

then browse http://127.0.0.1:8000/.
