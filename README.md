# Yaf - Yet Another Framework  
[![Build Status](https://secure.travis-ci.org/laruence/php-yaf.png)](https://travis-ci.org/laruence/php-yaf)

PHP framework written in c and built as a PHP extension.

## Requirement
- PHP 5.2 +

## Install
### Install Yaf 
Yaf is an PECL extension, thus you can simply install it by:

```
$pecl install yaf
```
### Compile Yaf in Linux
```
$/path/to/phpize
$./configure --with-php-config=/path/to/php-config
$make && make install
```
### For windows 
Yaf binary dlls could be found at http://code.google.com/p/yafphp/downloads/list

## Document
Yaf manual could be found at: http://www.php.net/manual/en/book.yaf.php

## IRC
efnet.org #php.yaf

## Tutorial

### layout
A classic Application directory layout:

```
- .htaccess // Rewrite rules
+ public
  | - index.php // Application entry
  | + css
  | + js
  | + img
+ conf
  | - application.ini // Configure 
- application/
  - Bootstrap.php   // Bootstrap
  + controllers
     - Index.php // Default controller
  + views    
     |+ index   
        - index.phtml // View template for default controller
  - library
  - models  // Models
  - plugins // Plugins
```
### DocumentRoot
you should set DocumentRoot to application/public, thus only the public folder can be accessed by user

### index.php
index.php in the public directory is the only way in of the application, you should rewrite all request to it(you can use .htaccess in Apache+php mod) 

```php
<?php
define("APPLICATION_PATH",  dirname(dirname(__FILE__)));

$app  = new Yaf_Application(APPLICATION_PATH . "/conf/application.ini");
$app->bootstrap() //call bootstrap methods defined in Bootstrap.php
    ->run();
```
### Rewrite rules

#### Apache

```conf
#.htaccess
RewriteEngine On
RewriteCond %{REQUEST_FILENAME} !-f
RewriteRule .* index.php
```

#### Nginx

```
server {
  listen ****;
  server_name  domain.com;
  root   document_root;
  index  index.php index.html index.htm;
 
  if (!-e $request_filename) {
    rewrite ^/(.*)  /index.php/$1 last;
  }
}
```

#### Lighttpd

```
$HTTP["host"] =~ "(www.)?domain.com$" {
  url.rewrite = (
     "^/(.+)/?$"  => "/index.php/$1",
  )
}
```

### application.ini
application.ini is the application config file
```ini
[product]
;CONSTANTS is supported
application.directory = APP_PATH "/application/" 
```
alternatively, you can use a PHP array instead: 
```php
<?php
$config = array(
   "application" => array(
       "directory" => application_path . "/application/",
    ),
);

$app  = new yaf_application($config);
....
  
```
### default controller
In Yaf, the default controller is named IndexController:

```php
<?php
class IndexController extends Yaf_Controller_Abstract {
   // default action name
   public function indexAction() {  
        $this->getView()->content = "Hello World";
   }
}
?>
```

###view script
The view script for default controller and default action is in the application/views/index/index.phtml, Yaf provides a simple view engineer called "Yaf_View_Simple", which supported the view template written by PHP.

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

## Run the Applicatioin
  http://www.yourhostname.com/

## Alternative
you can generate the example above by using Yaf Code Generator:  https://github.com/laruence/php-yaf/tree/master/tools/cg

## More
More info could be found at Yaf Manual: http://www.php.net/manual/en/book.yaf.php
