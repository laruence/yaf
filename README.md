# Yaf - Yet Another Framework

[![Build status](https://ci.appveyor.com/api/projects/status/awii6wf2ocmy202p/branch/master?svg=true)](https://ci.appveyor.com/project/laruence/yaf/branch/master) [![Build Status](https://github.com/laruence/yaf/workflows/integrate/badge.svg)](https://github.com/laruence/yaf/actions?query=workflow%3Aintegrate)

Yaf is a PHP framework with high performance. It is written in C and built as a PHP extension.

## Requirement

- PHP 7.0+ (master branch)
- PHP 5.2+ ([php5 branch](https://github.com/laruence/yaf/tree/php5))

## Install

### Install via PECL

Yaf is a PECL extension, which means you can simply install it by:

```bash
$ pecl install yaf
```

### Compile from source

```bash
$ /path/to/phpize
$ ./configure --with-php-config=/path/to/php-config
$ make && make install
```

## Runtime Configuration

| INI Setting | Default | Description |
|---|---|---|
| `yaf.environ` | `"product"` | Default environment name. This maps to the INI section loaded from `application.ini` |
| `yaf.library` | `""` | Global library directory, searched after the application's local library |
| `yaf.forward_limit` | `5` | Maximum number of `forward()` calls allowed in a single request. Prevents infinite loops |
| `yaf.name_suffix` | `1` | When `1`, classes use suffix naming (`IndexController`). When `0`, uses prefix naming (`Controller_Index`) |
| `yaf.name_separator` | `""` | In multi-module setups, replaces the underscore between module and class name. E.g. with `"/"`: `Admin/IndexController` instead of `Admin_IndexController` |
| `yaf.use_namespace` | `0` | When `1`, enables namespaced class names (`Yaf\Application`, `Yaf\Controller_Abstract`, etc.) |
| `yaf.action_prefer` | `0` | When `1`, actions are resolved as standalone `Yaf_Action_Abstract` classes in an `actions/` subdirectory instead of controller methods |
| `yaf.lowcase_path` | `0` | When `1`, all paths are lowercased before class loading (e.g. `controllers/Index.php` → `controllers/index.php`) |
| `yaf.use_spl_autoload` | `0` | When `1`, Yaf registers itself on the SPL autoload stack instead of replacing `__autoload`. Useful when coexisting with other autoloaders |

## Documentation

Yaf manual can be found at: http://www.php.net/manual/en/book.yaf.php

## For IDE

A documented prototype script can be found at: https://github.com/elad-yosifon/php-yaf-doc

## Tutorial

### Application Directory Layout

A classic single-module application directory layout is:

```
- .htaccess  # Rewrite rules
+ public
  | - index.php  # Application entry
  | + css
  | + js
  | + img
+ conf
  | - application.ini  # Configuration
- application/
  - Bootstrap.php  # Bootstrap
  + controllers
     - Index.php  # Default controller
  + views
     |+ index
        - index.phtml  # View template for default controller
  + library  # Libraries
  + models   # Models
  + plugins  # Plugins
```

For multi-module applications, the layout is:

```
+ public/
+ conf/
+ application/
  + modules/
    + Index/       # Default module
      + controllers/
      + views/
    + Admin/       # Another module
      + controllers/
      + views/
  + library/
  + models/
  + plugins/
  - Bootstrap.php
```

### DocumentRoot

Set `DocumentRoot` to `application/public`, so only the public folder is accessible from the web.

### index.php

`index.php` in the public directory is the only way into the application. You should rewrite all requests to it (using `.htaccess` in Apache + mod_php, or the equivalent in your web server).

```php
<?php
define("APPLICATION_PATH", dirname(dirname(__FILE__)));

$app = new Yaf_Application(APPLICATION_PATH . "/conf/application.ini");
$app->bootstrap() // call bootstrap methods defined in Bootstrap.php
    ->run();
```

### Bootstrap

A minimal `Bootstrap.php` looks like this:

```php
<?php
class Bootstrap extends Yaf_Bootstrap_Abstract
{
    public function _initConfig(Yaf_Dispatcher $dispatcher)
    {
        // put your init logic here
    }
}
```

**Bootstrap auto-calling**: Any method in `Bootstrap` whose name starts with `_init` is automatically called in definition order by `Yaf_Application::bootstrap()`. Each method receives the `Yaf_Dispatcher` instance as its first argument.

```php
<?php
class Bootstrap extends Yaf_Bootstrap_Abstract
{
    public function _initConfig(Yaf_Dispatcher $dispatcher)
    {
        // called first
    }

    public function _initPlugin(Yaf_Dispatcher $dispatcher)
    {
        // called second
    }

    public function _initRoute(Yaf_Dispatcher $dispatcher)
    {
        // called third
    }

    // This is NOT auto-called — doesn't start with _init
    public function helperMethod()
    {
    }
}
```

### Rewrite Rules

#### Apache

```apache
#.htaccess
RewriteEngine On
RewriteCond %{REQUEST_FILENAME} !-f
RewriteRule .* index.php
```

#### Nginx

```nginx
server {
    listen      80;
    server_name domain.com;
    root        /path/to/document/root;
    index       index.php index.html index.htm;

    location / {
        try_files $uri $uri/ /index.php?$args;
    }
}
```

#### Lighttpd

```
$HTTP["host"] =~ "(www.)?domain.com$" {
    url.rewrite = (
        "^/(.+)/?$" => "/index.php/$1",
    )
}
```

### application.ini

`application.ini` is the application config file:

```ini
[product]
; Constants defined in index.php are supported
application.directory = APPLICATION_PATH "/application/"
```

Alternatively, you can use a PHP array instead:

```php
<?php
$config = [
    "application" => [
        "directory" => APPLICATION_PATH . "/application/",
    ],
];

$app = new Yaf_Application($config);
```

### Default Controller

In Yaf, the default controller is named `IndexController`:

```php
<?php
class IndexController extends Yaf_Controller_Abstract
{
    // default action name
    public function indexAction()
    {
        $this->getView()->content = "Hello World";
    }
}
```

### View Script

The view script for the default controller and default action is `application/views/index/index.phtml`. Yaf provides a simple view engine called `Yaf_View_Simple`, which supports view templates written in PHP:

```php
<html>
  <head>
    <title>Hello World</title>
  </head>
  <body>
    <?php echo $content; ?>
  </body>
</html>
```

### Run the Application

Point your browser to your configured domain (e.g. `http://www.example.com`) and you should see "Hello World".

### Code Generator

You can generate the example above using Yaf Code Generator:

```bash
$ ./yaf_cg -d output_directory [-a application_name] [--namespace]
```

The code generator is located at: https://github.com/laruence/yaf/tree/master/tools/cg


## Core Concepts

### Routing

Yaf supports 6 built-in route types, all implementing `Yaf_Route_Interface`:

| Route | Description |
|---|---|
| `Yaf_Route_Static` | Default route — `/module/controller/action` |
| `Yaf_Route_Simple` | Maps query-string parameters to module/controller/action |
| `Yaf_Route_Supervar` | Uses a single GET/POST variable for routing path (e.g. `?r=/module/controller/action`) |
| `Yaf_Route_Rewrite` | Pattern-matching rewrite with named capture groups |
| `Yaf_Route_Regex` | Full regex matching with capture-to-variable mapping |
| `Yaf_Route_Map` | Maps the first URI segment to either controller or action |

Routes can be registered in Bootstrap:

```php
<?php
class Bootstrap extends Yaf_Bootstrap_Abstract
{
    public function _initRoute(Yaf_Dispatcher $dispatcher)
    {
        $router = $dispatcher->getRouter();

        // Pattern: /user/123 → controller=user, action=index, id=123
        $router->addRoute("user", new Yaf_Route_Rewrite(
            "/user/:id",
            ["controller" => "user", "action" => "index"]
        ));

        // Regex:  /item/123.html → controller=item, action=view, id=123
        $router->addRoute("item", new Yaf_Route_Regex(
            "#^/item/(\d+)\.html$#",
            ["controller" => "item", "action" => "view"],
            [1 => "id"]
        ));
    }
}
```

Routes can also be loaded from INI config:

```php
$router->addConfig(new Yaf_Config_Ini("routes.ini"));
```

### Configuration

Yaf provides two configuration parsers:

#### Yaf_Config_Ini

Parses standard INI files. Supports section inheritance:

```ini
[common]
db.host = "localhost"
db.port = 3306

[product : common]
db.user = "app_user"
db.pass = "secret"
```

The `[product : common]` syntax inherits all keys from `[common]` then applies its own overrides.

#### Yaf_Config_Simple

Parses PHP arrays or INI files into a mutable configuration object. Values can be modified at runtime:

```php
<?php
$config = new Yaf_Config_Simple([
    "db" => ["host" => "localhost"],
]);
$config->db->host = "10.0.0.1"; // editable
```

In contrast, `Yaf_Config_Ini` is always read-only.

> **Performance tip**: If you have a large amount of static configuration, consider using [Yaconf](https://github.com/laruence/yaconf) — a persistent configuration container that keeps configs in shared memory across the entire PHP lifecycle, providing significantly faster access than parsing INI files on every request.

### Plugins

Yaf supports a plugin hook system via `Yaf_Plugin_Abstract`. Override any of 7 hooks:

```php
<?php
class UserPlugin extends Yaf_Plugin_Abstract
{
    public function routerStartup(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {}
    public function routerShutdown(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {}
    public function dispatchLoopStartup(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {}
    public function preDispatch(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {}
    public function postDispatch(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {}
    public function dispatchLoopShutdown(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {}
    public function preResponse(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {}
}
```

Register in Bootstrap:

```php
$dispatcher->registerPlugin(new UserPlugin());
```

### Exceptions

Yaf defines a structured exception hierarchy:

```
Exception
└── Yaf_Exception
    ├── Yaf_Exception_StartupError     — Application startup failure
    ├── Yaf_Exception_RouterError      — Route matching failure
    ├── Yaf_Exception_DispatchError    — Dispatch failure
    ├── Yaf_Exception_LoadFailed       — Class/Method/View load failure
    └── Yaf_Exception_TypeError         — Type mismatch
```

You can control whether Yaf throws or silently swallows exceptions during dispatch:

```php
$dispatcher->throwException(true);  // throw to user
$dispatcher->catchException(true);  // catch and store in request
```

### Core Classes Quick Reference

| Class | Purpose |
|---|---|
| `Yaf_Application` | Application bootstrap and lifecycle |
| `Yaf_Dispatcher` | Request dispatch pipeline |
| `Yaf_Controller_Abstract` | Base controller — use `forward()`, `redirect()`, `render()`, `display()` |
| `Yaf_Action_Abstract` | Standalone action class (requires `yaf.action_prefer = 1`) |
| `Yaf_Bootstrap_Abstract` | Auto-called `_init*` methods |
| `Yaf_Loader` | PSR-0 style class autoloading |
| `Yaf_Router` | Route registration and matching |
| `Yaf_Registry` | Static key-value store (`get/set/has/del`) |
| `Yaf_Session` | Namespaced session wrapper with `ArrayAccess` |
| `Yaf_Config_Ini` | Read-only INI config parser |
| `Yaf_Config_Simple` | Mutable config from INI or PHP array |
| `Yaf_Request_Http` | HTTP request abstraction |
| `Yaf_Request_Simple` | Synthetic request (CLI / testing) |
| `Yaf_Response_Http` | HTTP response with header management |
| `Yaf_Response_Cli` | CLI response |
| `Yaf_View_Simple` | PHP-template view engine (`assign`, `render`, `display`, `eval`, `assignRef`, `clear`) |
| `Yaf_Plugin_Abstract` | 7-hook plugin base class |


## More

More info can be found at the Yaf Manual: http://www.php.net/manual/en/book.yaf.php

## License

[PHP-3.01](https://www.php.net/license/3_01.txt)
