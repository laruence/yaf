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

## Documentation

Yaf manual can be found at: http://www.php.net/manual/en/book.yaf.php

## For IDE

A documented prototype script can be found at: https://github.com/elad-yosifon/php-yaf-doc

## Tutorial

### Application Directory Layout

A classic application directory layout is:

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

## Run the Application

Point your browser to your configured domain (e.g. `http://www.example.com`) and you should see "Hello World".

## Code Generator

You can generate the example above using Yaf Code Generator:

```bash
$ ./yaf_cg -d output_directory [-a application_name] [--namespace]
```

The code generator is located at: https://github.com/laruence/php-yaf/tree/master/tools/cg

## More

More info can be found at the Yaf Manual: http://www.php.net/manual/en/book.yaf.php

## License

[PHP-3.01](https://www.php.net/license/3_01.txt)
