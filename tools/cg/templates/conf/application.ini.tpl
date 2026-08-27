; {&$APP_NAME&} — application configuration
;
; This file is read by Yaf_Application. The section whose name matches
; the yaf.environ INI directive (php.ini, default "product") is loaded;
; sections can inherit from one another: [product : common].
;
; PHP constants defined in public/index.php (APPLICATION_PATH) can be
; referenced directly in values.

[common]
; Where Bootstrap.php, controllers/, views/ and friends live.
application.directory = APPLICATION_PATH "/application"

; Catch uncaught exceptions during dispatch and hand them to
; ErrorController::errorAction instead of failing with a fatal error.
application.dispatcher.catchException = TRUE

; Custom routes (optional). By default the built-in Yaf_Route_Static
; handles all requests. Yaf_Router::addConfig() understands this
; layout: "type" picks the route class, "match" is the pattern, and
; the keys under "route" are the dispatch parameters. Uncomment to
; enable a rewrite route that maps /user/:id to IndexController::userAction
; (see application/controllers/Index.php), and load it in Bootstrap:
;   $dispatcher->getRouter()->addConfig($config->routes);
;routes.rewrite_user.type = rewrite
;routes.rewrite_user.match = "/user/:id"
;routes.rewrite_user.route.controller = Index
;routes.rewrite_user.route.action = user

; Your own configuration values start here — any key can be fetched at
; runtime via Yaf_Application::app()->getConfig()->key

[product : common]
; Values for the production environment (yaf.environ = product)

[develop : common]
; Values for the development environment (yaf.environ = develop)
