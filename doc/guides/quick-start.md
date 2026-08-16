# 快速上手

从零搭建并运行一个最小的 Yaf 应用。本篇面向两类读者：照着做的人类用户，以及需要明确规则的 AI。所有目录名、默认值均与 yaf 源码（`yaf_application.c` / `yaf_loader.h` / `yaf_config.c`）核对过。

## 前提

- PHP 已安装，且 yaf 扩展已加载（`php -m | grep yaf` 能看到 `yaf`）。
- Web 服务器（Nginx/Apache）或 PHP 内置服务器用于访问入口文件。

## 目录结构

Yaf 应用的惯例目录结构如下（`application.directory` 指向 `application/`）：

```
yaf-demo/
├── application/
│   ├── controllers/
│   │   └── Index.php            # IndexController
│   ├── views/
│   │   └── index/
│   │       └── index.phtml      # Index/index 的视图模板
│   ├── Bootstrap.php            # 可选，引导类
│   ├── library/                 # 可选，本地类库
│   └── application.ini          # 配置文件
└── public/
    └── index.php                # Web 入口（document root 指向这里）
```

规则（源码依据）：

- **controllers**：控制器目录名硬编码为 `controllers`（`yaf_loader.h`）。默认模块的控制器放在 `application/controllers/`；非默认模块放在 `application/modules/<Module>/controllers/`。
- **views**：视图目录硬编码为 `views`（`yaf_dispatcher.c` 中 `yaf_dispatcher_handle` 拼装 `application/views` 作为模板目录）。
- **Bootstrap.php**：若配置里没指定 `bootstrap` 路径，默认找 `application.directory + "Bootstrap.php"`，类名必须是 `Bootstrap`，继承 [Yaf_Bootstrap_Abstract](../classes/yaf_bootstrap_abstract.md)。
- 控制器类名默认为 `<Name>Controller`（`yaf.name_suffix=1` 默认开启），文件名 `Index.php`，大小写遵循类名。

## 配置文件 application.ini

最小配置只需要一项：`application.directory`（源码 `yaf_application_parse_option()`：找不到 `application`/`yaf` 段的 `directory` 项就初始化失败，报 `Expected 'directory' entry in application configuration`）。

```ini
[product]
; 必需。指向 application 目录，必须为绝对路径
application.directory = /path/to/yaf-demo/application
```

要点：

- **section 名**：[Yaf_Application](../classes/yaf_application.md) 构造时第二个参数是 environ，不传则取 php.ini 的 `yaf.environ`（默认 `product`）。所以默认配置写在 `[product]` 段下。常用做法是写 `product`/`develop` 段，用 `: ` 继承：

  ```ini
  [common]
  application.directory = /path/to/yaf-demo/application

  [product : common]
  ; 生产环境覆盖项

  [develop : common]
  ; 开发环境覆盖项
  ```

  然后 `new Yaf_Application($config, 'develop')` 选择环境。

- 段名前缀 `application` 与 `yaf` 都认（源码先找 `application` 键，找不到再找 `yaf` 键）。
- 配置也可以是 PHP 数组，直接传给构造函数；ini 文件路径同样直接传，扩展内部用 [Yaf_Config_Ini](../classes/yaf_config_ini.md) 解析。
- 其他可选配置（`dispatcher.defaultController`、`modules`、`library` 等）见 [ini 配置项](../ini.md)。

## 入口文件 public/index.php

```php
<?php
define('APPLICATION_PATH', dirname(__DIR__) . '/application');

$app = new Yaf_Application(APPLICATION_PATH . '/application.ini');
$app->bootstrap();   // 可选：执行 Bootstrap.php 中的 _init* 方法
$app->run();         // 路由 + 分发 + 输出响应
```

- `new Yaf_Application($config, $environ = null)`：加载配置、创建单例 [Yaf_Dispatcher](../classes/yaf_dispatcher.md)（内含 [Yaf_Router](../classes/yaf_router.md)）和请求对象（[Yaf_Request_Http](../classes/yaf_request_http.md)）。全局只允许初始化一次。
- `bootstrap()` 会查找并实例化 `Bootstrap` 类，按声明顺序调用所有以 `_init` 开头的方法，参数为 dispatcher。这是注册插件、初始化全局对象的最早时机。
- `run()` 进入分发流程（详见 [请求生命周期](lifecycle.md)）。

## IndexController

`application/controllers/Index.php`：

```php
<?php
class IndexController extends Yaf_Controller_Abstract {
    public function init() {
        // 可选：控制器初始化，每个 controller 实例化后自动调用
    }

    public function indexAction() {
        $this->getView()->assign('name', 'Yaf');
        // 不 return 或 return 非 false → 自动渲染视图
        // return false → 本次不自动渲染
    }
}
```

规则：

- 方法名 = action 名 + `Action` 后缀，如 `indexAction` 对应 action `index`。
- action 返回 `false` 时跳过自动渲染（常用于 JSON 接口，配合 `$this->getResponse()` 输出）。
- URL 路由参数会自动注入到 action 方法形参（按参数名匹配）。

视图模板 `application/views/index/index.phtml`（模板目录 = `views/<控制器名小写>/<action名>.phtml`）：

```php
<h1>Hello, <?php echo $name; ?></h1>
```

视图引擎是 [Yaf_View_Simple](../classes/yaf_view_simple.md)：模板就是 PHP 文件，`$this->getView()->assign()` 赋值的变量在模板中直接可用。

## Web 服务器配置

### Nginx

```nginx
server {
    listen 80;
    server_name demo.local;
    root /path/to/yaf-demo/public;
    index index.php;

    location / {
        try_files $uri $uri/ /index.php?$query_string;
    }

    location ~ \.php$ {
        fastcgi_pass   127.0.0.1:9000;   # 或 unix:/run/php-fpm.sock
        include        fastcgi_params;
        fastcgi_param  SCRIPT_FILENAME $document_root$fastcgi_script_name;
    }
}
```

### Apache（.htaccess，需开启 mod_rewrite）

放在 `public/.htaccess`：

```apache
RewriteEngine On
RewriteCond %{REQUEST_FILENAME} !-f
RewriteRule .* index.php
```

### 不配服务器也能跑

```bash
cd public
php -S 0.0.0.0:8080
```

关键：document root 指向 `public/`，所有非静态文件请求都落到 `index.php`。PATH_INFO 或 `REQUEST_URI` 是 yaf 解析路由的来源。

## 运行效果

访问 `http://localhost:8080/`（即 `/<默认模块>/<默认控制器>/<默认action>` = Index/Index/index）：

```html
<h1>Hello, Yaf</h1>
```

访问不存在的 action（如 `/index/none`）会看到 Yaf 的异常提示（`Yaf_Exception_LoadFailed` 类），生产环境可通过 [Yaf_Dispatcher::catchException()](../classes/yaf_dispatcher.md) 配合 `ErrorController` 统一接管错误页。

## 下一步

- 理解一次请求的完整执行路径：[请求生命周期](lifecycle.md)
- 路由规则与自定义路由：[Yaf_Router](../classes/yaf_router.md)、[Yaf_Route_Rewrite](../classes/yaf_route_rewrite.md)
- 插件钩子：[Yaf_Plugin_Abstract](../classes/yaf_plugin_abstract.md)
- 全部 ini 配置项：[配置项](../ini.md)
