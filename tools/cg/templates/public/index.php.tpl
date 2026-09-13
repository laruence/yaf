<?php
/**
 * {&$APP_NAME&} — the only entry point of the application
 *
 * @author {&$AUTHOR&}
 *
 * Every request is forwarded to this file by the rewrite rules
 * (public/.htaccess for Apache, or the nginx server block shipped in
 * nginx.conf at the top level of this skeleton).
 */

/*
 * Define APPLICATION_PATH here so it can also be referenced in
 * conf/application.ini (Yaf_Config_Ini resolves PHP constants while
 * parsing). The entry script lives in public/, so the application
 * root is one directory up.
 */
define('APPLICATION_PATH', dirname(__DIR__));

/*
 * Yaf_Application — the heart of a Yaf application.
 * The constructor reads conf/application.ini (a plain PHP config
 * array would work as well), builds the configuration object and
 * prepares the dispatcher.
 */
$application = new Yaf_Application(APPLICATION_PATH . '/conf/application.ini');

/*
 * bootstrap(): runs every method named _init* in
 * application/Bootstrap.php, in declaration order — that is where
 * configuration, plugins, custom routes and view engines are set up.
 *
 * run(): starts the dispatch loop — route the request, call the
 * matched controller action, render the view.
 */
$application->bootstrap()->run();
