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
6. Browse http://yourhost/ — "Hello world, I am Yaf." means it works.
   You can also try http://yourhost/user/42 for the custom rewrite
   route. Otherwise check the PHP error log.
