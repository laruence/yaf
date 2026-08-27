# Yaf_Dispatcher

分发器, final 单例, 由 `Yaf_Application` 在初始化时创建, 负责路由请求、触发插件钩子、加载并执行 Controller/Action、渲染视图并组织响应。也可通过 `Yaf_Dispatcher::getInstance()` 获取 (3.3.8 起必须先初始化 Yaf_Application, 否则抛异常)。内部持有路由器、请求、响应、视图引擎和插件列表; 默认模块/控制器/动作可通过 setDefault* 修改。

```php
final class Yaf_Dispatcher
```

## 方法

### `Yaf_Dispatcher::__construct`

私有构造方法, 禁止外部实例化。分发器是单例: 由 Yaf_Application 初始化时创建, 请通过 `Yaf_Dispatcher::getInstance()` 获取。

```php
private function __construct(): mixed
```

**返回值**: 不可用。

**示例**

```php
$app = new Yaf_Application($config); // 必须先初始化应用
$dispatcher = Yaf_Dispatcher::getInstance();
```

### `Yaf_Dispatcher::enableView`

开启自动渲染 (默认开启)。开启时 Action 执行完成后会自动渲染对应的视图模板。

```php
public function enableView(): ?object
```

**返回值**: 返回自身实例, 支持链式调用。

**示例**

```php
class IndexController extends Yaf_Controller_Abstract {
    public function init() {
        if (!$this->getRequest()->isXmlHttpRequest()) {
            Yaf_Dispatcher::getInstance()->enableView();
        }
    }
}
```

### `Yaf_Dispatcher::disableView`

关闭自动渲染, 常用于 Ajax/JSON 接口: Action 执行完不再渲染视图模板。

```php
public function disableView(): ?object
```

**返回值**: 返回自身实例, 支持链式调用。

**示例**

```php
class IndexController extends Yaf_Controller_Abstract {
    public function init() {
        if ($this->getRequest()->isXmlHttpRequest()) {
            Yaf_Dispatcher::getInstance()->disableView();
        }
    }
}
```

### `Yaf_Dispatcher::initView`

初始化视图引擎。Yaf 采用延迟实例化策略, 首次分发时才创建默认的 Yaf_View_Simple; 调用本方法可提前创建并指定模板目录。若已通过 `setView()` 设置了自定义视图引擎, 则直接返回已有实例, 不会重建。

```php
public function initView(string $templates_dir, ?array $options = NULL): object|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$templates_dir` | 视图模板目录的绝对路径。 |
| `$options` | 传给视图引擎构造函数的可选参数 (对 Yaf_View_Simple 无意义)。 |

**返回值**: 返回 Yaf_View_Interface 实例; 初始化失败返回 FALSE。

**示例**

```php
class Bootstrap extends Yaf_Bootstrap_Abstract {
    public function _initView(Yaf_Dispatcher $dispatcher) {
        $dispatcher->initView(APPLICATION_PATH . "/views/")->assign("site", "yaf");
    }
}
```

### `Yaf_Dispatcher::setView`

设置自定义视图引擎, 替代默认的 Yaf_View_Simple。通常在 Bootstrap 中调用。

```php
public function setView(object $view): ?object
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$view` | 实现了 Yaf_View_Interface 的视图引擎实例 (如 Smarty 适配器)。 |

**返回值**: 返回自身实例, 支持链式调用。

**示例**

```php
class Bootstrap extends Yaf_Bootstrap_Abstract {
    public function _initSmarty(Yaf_Dispatcher $dispatcher) {
        $smarty = new Smarty_Adapter(null, Yaf_Registry::get("config")->get("smarty"));
        $dispatcher->setView($smarty);
    }
}
```

### `Yaf_Dispatcher::setRequest`

设置分发器使用的请求对象, 常见于 CLI 下手工构造请求再分发。

```php
public function setRequest(object $request): ?object
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$request` | Yaf_Request_Abstract 的实例。 |

**返回值**: 返回自身实例, 支持链式调用。

**示例**

```php
$request = new Yaf_Request_Simple("CLI", "Index", "Index", "index");
Yaf_Dispatcher::getInstance()->setRequest($request)->dispatch($request);
```

### `Yaf_Dispatcher::setResponse`

设置分发器使用的响应对象。

```php
public function setResponse(object $response): ?object
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$response` | Yaf_Response_Abstract 的实例。 |

**返回值**: 返回自身实例, 支持链式调用。

**示例**

```php
$response = new Yaf_Response_Http();
Yaf_Dispatcher::getInstance()->setResponse($response);
```

### `Yaf_Dispatcher::getApplication`

获取当前 Yaf_Application 实例, 等价于 `Yaf_Application::app()`, 便于从 Dispatcher 链式回到 Application (如 `->getApplication()->bootstrap()->run()`)。

```php
public function getApplication(): ?object
```

**返回值**: 全局 Yaf_Application 实例; 尚未初始化时返回 NULL。

**示例**

```php
$app = Yaf_Dispatcher::getInstance()->getApplication();
// 更推荐:
$app = Yaf_Application::app();
```

### `Yaf_Dispatcher::getRouter`

获取路由器, 用于在 Bootstrap 中注册自定义路由。

```php
public function getRouter(): ?object
```

**返回值**: Yaf_Router 实例。

**示例**

```php
class Bootstrap extends Yaf_Bootstrap_Abstract {
    public function _initRoute(Yaf_Dispatcher $dispatcher) {
        $router = $dispatcher->getRouter();
        $router->addRoute("product", new Yaf_Route_Rewrite(
            "/product/list/:id/",
            array("controller" => "product", "action" => "info")
        ));
    }
}
```

### `Yaf_Dispatcher::getResponse`

获取当前的响应对象。响应对象在首次分发时才创建。

```php
public function getResponse(): ?object
```

**返回值**: Yaf_Response_Abstract 实例; 尚未发生过分发时返回 NULL。

**示例**

```php
$response = Yaf_Dispatcher::getInstance()->getResponse();
if ($response !== NULL) {
    echo $response->getBody();
}
```

### `Yaf_Dispatcher::getRequest`

获取当前的请求对象。

```php
public function getRequest(): ?object
```

**返回值**: Yaf_Request_Abstract 实例。

**示例**

```php
$request = Yaf_Dispatcher::getInstance()->getRequest();
var_dump($request->getModuleName());
```

### `Yaf_Dispatcher::getDefaultModule`

获取默认模块名, 即配置中 `application.dispatcher.defaultModule` 的值。

```php
public function getDefaultModule(): ?string
```

**返回值**: 默认模块名字符串, 未配置时为 "Index"。

**示例**

```php
$app = new Yaf_Application($config);
echo Yaf_Dispatcher::getInstance()->getDefaultModule(); // Index
```

### `Yaf_Dispatcher::getDefaultController`

获取默认控制器名, 即配置中 `application.dispatcher.defaultController` 的值。

```php
public function getDefaultController(): ?string
```

**返回值**: 默认控制器名字符串, 未配置时为 "Index"。

**示例**

```php
$app = new Yaf_Application($config);
echo Yaf_Dispatcher::getInstance()->getDefaultController(); // Index
```

### `Yaf_Dispatcher::getDefaultAction`

获取默认动作名, 即配置中 `application.dispatcher.defaultAction` 的值。

```php
public function getDefaultAction(): ?string
```

**返回值**: 默认动作名字符串, 未配置时为 "index" (小写)。

**示例**

```php
$app = new Yaf_Application($config);
echo Yaf_Dispatcher::getInstance()->getDefaultAction(); // index
```

### `Yaf_Dispatcher::setErrorHandler`

设置错误处理函数, 本质是对 `set_error_handler()` 的封装。在 `throwException` 关闭时, Yaf 出错会以触发错误的方式交给该回调处理, 常用于把 404 等错误映射到自定义页面。一般放在 Bootstrap 中调用。

```php
public function setErrorHandler($callback, int $error_types = 0): object|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$callback` | 错误处理回调, 至少接受错误码 ($errno) 和错误信息 ($errstr) 两个参数, 可选错误文件、行号等。 |
| `$error_types` | 要捕获的错误类型掩码, 缺省为 `E_ALL | E_STRICT`。 |

**返回值**: 成功返回自身实例; 调用 set_error_handler 失败时返回 FALSE。

**示例**

```php
class Bootstrap extends Yaf_Bootstrap_Abstract {
    public function _initError(Yaf_Dispatcher $dispatcher) {
        $dispatcher->throwException(false);
        $dispatcher->setErrorHandler(function ($errno, $errstr) {
            if ($errno == YAF_ERR_NOTFOUND_CONTROLLER) {
                header("HTTP/1.1 404 Not Found");
            }
        }, E_ALL);
    }
}
```

### `Yaf_Dispatcher::setDefaultModule`

修改默认模块。路由结果中不含模块信息时, 使用该值作为目标模块。

```php
public function setDefaultModule(string $module): object|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$module` | 模块名, 必须是 `application.modules` 中已声明的模块, 否则触发错误。首字母会被规范化为大写。 |

**返回值**: 成功返回自身实例; 应用未初始化或模块不存在时返回 FALSE。

**示例**

```php
class Bootstrap extends Yaf_Bootstrap_Abstract {
    public function _initDefaultName(Yaf_Dispatcher $dispatcher) {
        $dispatcher->setDefaultModule("Index")
            ->setDefaultController("Index")
            ->setDefaultAction("index");
    }
}
```

### `Yaf_Dispatcher::setDefaultController`

修改默认控制器。路由结果中不含控制器信息时使用。

```php
public function setDefaultController(string $controller): object|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$controller` | 控制器名, 首字母会被规范化为大写。 |

**返回值**: 成功返回自身实例; 应用未初始化时返回 FALSE。

**示例**

```php
Yaf_Dispatcher::getInstance()->setDefaultController("Home");
```

### `Yaf_Dispatcher::setDefaultAction`

修改默认动作。路由结果中不含动作信息时使用。

```php
public function setDefaultAction(string $action): object|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$action` | 动作名, 会被规范化为小写。 |

**返回值**: 成功返回自身实例; 应用未初始化时返回 FALSE。

**示例**

```php
Yaf_Dispatcher::getInstance()->setDefaultAction("main");
```

### `Yaf_Dispatcher::returnResponse`

开关/查询"返回响应对象"模式。开启后分发完成不再自动输出响应, 而是把 Yaf_Response_Abstract 交给程序员自行输出; 默认关闭 (自动输出)。

```php
public function returnResponse(bool $flag = false): object|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$flag` | 传入 TRUE/FALSE 进行设置; 不传参数则只查询当前状态。 |

**返回值**: 带参数时返回自身实例 (支持链式); 不带参数时返回当前开关状态 bool。

**示例**

```php
$app = new Yaf_Application("config.ini");
$response = $app->getDispatcher()
    ->returnResponse(true)
    ->getApplication()->run();
$response->response(); // 手工输出
```

### `Yaf_Dispatcher::autoRender`

开关/查询自动渲染。开启 (默认) 时, Action 执行完成后自动调用视图引擎渲染对应模板; 控制器属性 `$yafAutoRender` 的优先级高于此全局开关。

```php
public function autoRender(?bool $flag = NULL): object|bool|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$flag` | 传入 TRUE/FALSE 进行设置; 不传参数则只查询当前状态。 |

**返回值**: 带参数时返回自身实例 (支持链式); 不带参数时返回当前开关状态 bool。

**示例**

```php
class IndexController extends Yaf_Controller_Abstract {
    public function init() {
        if ($this->getRequest()->isXmlHttpRequest()) {
            // Ajax 请求关闭自动渲染, 手工输出 JSON
            Yaf_Dispatcher::getInstance()->autoRender(false);
        }
    }
}
```

### `Yaf_Dispatcher::flushInstantly`

开关/查询"即时输出"。开启且自动渲染有效时, 渲染结果直接输出给请求端 (走 display), 而不是先追加到响应体再统一输出。默认关闭。

```php
public function flushInstantly(?bool $flag = NULL): object|bool|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$flag` | 传入 TRUE/FALSE 进行设置; 不传参数则只查询当前状态。 |

**返回值**: 带参数时返回自身实例 (支持链式); 不带参数时返回当前开关状态 bool。

**示例**

```php
$app = new Yaf_Application("config.ini");
Yaf_Dispatcher::getInstance()->flushInstantly(true);
$app->run();
```

### `Yaf_Dispatcher::getInstance`

获取分发器单例。3.3.8 起要求 Yaf_Application 必须先初始化 (分发器由应用构造时创建), 否则抛出异常。

```php
public static function getInstance(): ?object
```

**返回值**: Yaf_Dispatcher 实例。

**示例**

```php
$app = new Yaf_Application($config); // 必须先初始化应用
$dispatcher = Yaf_Dispatcher::getInstance();
assert($dispatcher === $app->getDispatcher());
```

### `Yaf_Dispatcher::dispatch`

处理一个请求: 路由 (含插件 routerStartup/routerShutdown 钩子)、分发循环 (含 preDispatch/postDispatch 钩子)、渲染视图并组织响应。`Yaf_Application::run()` 内部调用的就是它; CLI 下需要手工构造请求时才直接使用。分发循环最多迭代 `yaf.forward_limit` 次 (默认 5, Controller::forward 会触发新一轮迭代); 注意 3.3.8 起该值为 0 或非正数时会被钳制回默认值 5, 不再表示"不限制"。超过上限抛出 Yaf_Exception_DispatchFailed。

```php
public function dispatch(object $request): object|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$request` | 要分发的 Yaf_Request_Abstract 实例, 会替换分发器当前的请求对象。 |

**返回值**: 成功返回 Yaf_Response_Abstract 实例 (除非开启 returnResponse, 响应会自动输出); 失败返回 FALSE 或抛出异常。

**示例**

```php
$app = new Yaf_Application(array("application" => array(
    "directory" => realpath(dirname(__FILE__)),
)));

$request = new Yaf_Request_Http("/index/index");
try {
    $response = $app->getDispatcher()->dispatch($request);
} catch (Yaf_Exception $e) {
    echo $e->getMessage();
}
```

### `Yaf_Dispatcher::throwException`

开关/查询 Yaf 出错时的报错方式。开启 (默认) 时抛出 Yaf_Exception; 关闭时触发 PHP 错误 (可配合 setErrorHandler 处理)。也可在配置中用 `application.dispatcher.throwException` 设置。

```php
public function throwException(?bool $flag = NULL): object|bool|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$flag` | 传入 TRUE/FALSE 进行设置; 不传参数则只查询当前状态。 |

**返回值**: 带参数时返回自身实例 (支持链式); 不带参数时返回当前开关状态 bool。

**示例**

```php
$app = new Yaf_Application("conf.ini");
// 关闭抛异常, 改为触发错误
Yaf_Dispatcher::getInstance()->throwException(false);
```

### `Yaf_Dispatcher::catchException`

开关/查询默认异常捕获机制。开启后, 未捕获的异常会被转交给 Error 控制器的 errorAction 处理 (异常对象可通过 `$request->getException()` 获取)。默认关闭 (与旧手册"默认开启"的说法不符); 也可在配置中用 `application.dispatcher.catchException` 设置。

```php
public function catchException(?bool $flag = NULL): object|bool|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$flag` | 传入 TRUE/FALSE 进行设置; 不传参数则只查询当前状态。 |

**返回值**: 带参数时返回自身实例 (支持链式); 不带参数时返回当前开关状态 bool。

**示例**

```php
$app = new Yaf_Application("conf.ini");
Yaf_Dispatcher::getInstance()->catchException(true);
```

### `Yaf_Dispatcher::registerPlugin`

注册一个插件。插件按注册顺序在分发的各个钩子 (routerStartup、routerShutdown、preDispatch、postDispatch 等) 被回调。通常在 Bootstrap 的 `_init` 方法中注册, 插件类放在 `application.directory/plugins` 下。

```php
public function registerPlugin(object $plugin): object|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$plugin` | Yaf_Plugin_Abstract 派生类的实例。 |

**返回值**: 返回自身实例, 支持链式调用。

**示例**

```php
class Bootstrap extends Yaf_Bootstrap_Abstract {
    public function _initPlugin(Yaf_Dispatcher $dispatcher) {
        $user = new UserPlugin();
        $dispatcher->registerPlugin($user);
    }
}

class UserPlugin extends Yaf_Plugin_Abstract {
    public function routerStartup(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {
    }
}
```

