# Yaf_Request_Abstract

请求对象的抽象基类，封装了请求方法、请求 URI、路由结果（module/controller/action/路由参数）以及对 $_GET/$_POST/$_SERVER 等超全局变量的访问。实际使用中由 Yaf 框架根据 SAPI 自动创建具体子类：HTTP 环境为 Yaf_Request_Http，CLI/测试场景可用 Yaf_Request_Simple。该类不可序列化、不可克隆。

```php
abstract class Yaf_Request_Abstract
```

## 方法

### `Yaf_Request_Abstract::isGet`

判断当前请求是否为 GET 请求。内部将请求方法与 "GET" 做不区分大小写的比较。

```php
public function isGet(): bool
```

**返回值**: 是 GET 请求返回 TRUE，否则返回 FALSE

**示例**

```php
$request = Yaf_Dispatcher::getInstance()->getRequest();
if ($request->isGet()) {
    echo "running in Get mode";
}
```

### `Yaf_Request_Abstract::isPost`

判断当前请求是否为 POST 请求。

```php
public function isPost(): bool
```

**返回值**: 是 POST 请求返回 TRUE，否则返回 FALSE

**示例**

```php
if ($this->getRequest()->isPost()) {
    // 处理表单提交
}
```

### `Yaf_Request_Abstract::isDelete`

判断当前请求是否为 DELETE 请求。

```php
public function isDelete(): bool
```

**返回值**: 是 DELETE 请求返回 TRUE，否则返回 FALSE

### `Yaf_Request_Abstract::isPatch`

判断当前请求是否为 PATCH 请求。

```php
public function isPatch(): bool
```

**返回值**: 是 PATCH 请求返回 TRUE，否则返回 FALSE

### `Yaf_Request_Abstract::isPut`

判断当前请求是否为 PUT 请求。

```php
public function isPut(): bool
```

**返回值**: 是 PUT 请求返回 TRUE，否则返回 FALSE

### `Yaf_Request_Abstract::isHead`

判断当前请求是否为 HEAD 请求。

```php
public function isHead(): bool
```

**返回值**: 是 HEAD 请求返回 TRUE，否则返回 FALSE

### `Yaf_Request_Abstract::isOptions`

判断当前请求是否为 OPTIONS 请求。

```php
public function isOptions(): bool
```

**返回值**: 是 OPTIONS 请求返回 TRUE，否则返回 FALSE

### `Yaf_Request_Abstract::isCli`

判断当前请求是否为 CLI 请求。

```php
public function isCli(): bool
```

**返回值**: 是 CLI 请求返回 TRUE，否则返回 FALSE

**示例**

```php
if ($this->getRequest()->isCli()) {
    echo "running in Cli mode";
}
```

### `Yaf_Request_Abstract::isXmlHttpRequest`

判断是否为 Ajax 请求：检查 $_SERVER["HTTP_X_REQUESTED_WITH"] 是否为 "XMLHttpRequest"（值比较不区分大小写）。Yaf_Request_Simple 覆写了该方法，恒返回 FALSE。

```php
public function isXmlHttpRequest(): bool
```

**返回值**: 是 Ajax 请求返回 TRUE，否则返回 FALSE

**示例**

```php
if ($this->getRequest()->isXmlHttpRequest()) {
    $this->getResponse()->setBody(json_encode($data));
}
```

### `Yaf_Request_Abstract::getQuery`

读取 $_GET 中的值。不传参数时返回整个 $_GET 数组；name 键查找区分大小写。

```php
public function getQuery(?string $name = NULL, $default = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 要获取的键名；不传则返回整个 $_GET 数组 |
| `$default` | 键不存在时返回的默认值，未设置则返回 NULL |

**返回值**: 找到返回对应值；未找到且设置了 default 返回 default；否则返回 NULL

**示例**

```php
$page = $this->getRequest()->getQuery("page", 1);
```

### `Yaf_Request_Abstract::getRequest`

读取 $_REQUEST 中的值。不传参数时返回整个 $_REQUEST 数组；name 键查找区分大小写。

```php
public function getRequest(?string $name = NULL, $default = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 要获取的键名；不传则返回整个 $_REQUEST 数组 |
| `$default` | 键不存在时返回的默认值，未设置则返回 NULL |

**返回值**: 找到返回对应值；未找到且设置了 default 返回 default；否则返回 NULL

### `Yaf_Request_Abstract::getPost`

读取 $_POST 中的值。不传参数时返回整个 $_POST 数组；name 键查找区分大小写。

```php
public function getPost(?string $name = NULL, $default = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 要获取的键名；不传则返回整个 $_POST 数组 |
| `$default` | 键不存在时返回的默认值，未设置则返回 NULL |

**返回值**: 找到返回对应值；未找到且设置了 default 返回 default；否则返回 NULL

**示例**

```php
$title = $this->getRequest()->getPost("title", "");
```

### `Yaf_Request_Abstract::getCookie`

读取 $_COOKIE 中的值。不传参数时返回整个 $_COOKIE 数组；name 键查找区分大小写。

```php
public function getCookie(?string $name = NULL, $default = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 要获取的键名；不传则返回整个 $_COOKIE 数组 |
| `$default` | 键不存在时返回的默认值，未设置则返回 NULL |

**返回值**: 找到返回对应值；未找到且设置了 default 返回 default；否则返回 NULL

**示例**

```php
$uid = $this->getRequest()->getCookie("uid", 0);
```

### `Yaf_Request_Abstract::getRaw`

读取原始请求体（raw body），常用于 JSON API 等场景。内部对请求体流做 rewind 后整体读取。

```php
public function getRaw(): ?string
```

**返回值**: 返回原始请求体字符串；请求体为空或无法读取时返回 FALSE

**示例**

```php
$payload = json_decode($this->getRequest()->getRaw(), true);
```

### `Yaf_Request_Abstract::getFiles`

读取 $_FILES 中的值。不传参数时返回整个 $_FILES 数组；name 键查找区分大小写。

```php
public function getFiles(?string $name = NULL, $default = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 要获取的键名；不传则返回整个 $_FILES 数组 |
| `$default` | 键不存在时返回的默认值，未设置则返回 NULL |

**返回值**: 找到返回对应值；未找到且设置了 default 返回 default；否则返回 NULL

### `Yaf_Request_Abstract::get`

按固定顺序查找一个键：路由参数 → $_POST → $_GET → $_COOKIE → $_SERVER，命中即返回（不查 $_ENV）。

```php
public function get(string $name, $default = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 要获取的键名 |
| `$default` | 全部来源都未找到时返回的默认值，未设置则返回 NULL |

**返回值**: 按上述顺序返回第一个命中的值；都未命中返回 default 或 NULL

**示例**

```php
// 一次调用即可兼容路由参数、POST、GET、COOKIE、SERVER 多种来源
$value = $this->getRequest()->get("token", "anonymous");
```

### `Yaf_Request_Abstract::getServer`

读取 $_SERVER 中的值。不传参数时返回整个 $_SERVER 数组；name 键查找区分大小写，HTTP 头需使用 HTTP_XXX 大写形式（如 HTTP_HOST）。

```php
public function getServer(?string $name = NULL, $default = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 要获取的键名；不传则返回整个 $_SERVER 数组 |
| `$default` | 键不存在时返回的默认值，未设置则返回 NULL |

**返回值**: 找到返回对应值；未找到且设置了 default 返回 default；否则返回 NULL

**示例**

```php
$host = $this->getRequest()->getServer("HTTP_HOST");
```

### `Yaf_Request_Abstract::getEnv`

读取 $_ENV 中的值。不传参数时返回整个 $_ENV 数组；name 键查找区分大小写。

```php
public function getEnv(?string $name = NULL, $default = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 要获取的键名；不传则返回整个 $_ENV 数组 |
| `$default` | 键不存在时返回的默认值，未设置则返回 NULL |

**返回值**: 找到返回对应值；未找到且设置了 default 返回 default；否则返回 NULL

### `Yaf_Request_Abstract::setParam`

设置路由参数。支持两种调用方式：setParam($name, $value) 设置单个；setParam($array) 批量设置（只处理字符串键）。路由参数不同于 $_GET/$_POST，它是路由协议从 Request URI 中解析出来的参数，也可由代码手动设置。

```php
public function setParam($name, $value = NULL): object|bool|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 路由参数名（字符串）；或传入数组表示批量设置 |
| `$value` | 路由参数值；批量模式下省略 |

**返回值**: 成功返回 Yaf_Request_Abstract 实例自身（可链式调用），失败返回 FALSE

**示例**

```php
$this->getRequest()->setParam("userid", 0);
$this->getRequest()->setParam(["age" => 28, "gender" => "man"]);
```

### `Yaf_Request_Abstract::getParam`

获取当前请求中的路由参数。路由参数不是 $_GET 或 $_POST，而是路由协议根据 Request URI 分析出的请求参数。比如默认路由 Yaf_Route_Static 路由 http://www.domain.com/module/controller/action/name1/value1/ 后，会得到路由参数 name1，值为 value1。路由参数来自用户输入，使用前需做安全过滤。

```php
public function getParam(string $name, $default = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 要获取的路由参数名 |
| `$default` | 如果没有找到该路由参数，则返回此默认值；未设置则返回 NULL |

**返回值**: 找到返回对应的路由参数值；未找到且设置了 default 返回 default；否则返回 NULL

**示例**

```php
class IndexController extends Yaf_Controller_Abstract {
    public function indexAction() {
        echo "user id:" . $this->getRequest()->getParam("userid", 0);
    }
}
```

### `Yaf_Request_Abstract::getParams`

获取当前请求中的所有路由参数。路由参数是路由协议根据 Request URI 分析出的参数，不是 $_GET/$_POST，来自用户输入，使用前需做安全过滤。

```php
public function getParams(): ?array
```

**返回值**: 当前所有路由参数组成的数组；没有任何路由参数时返回空数组

**示例**

```php
var_dump($this->getRequest()->getParams());
```

### `Yaf_Request_Abstract::clearParams`

清空当前请求的所有路由参数。

```php
public function clearParams(): ?object
```

**返回值**: 返回 Yaf_Request_Abstract 实例自身（可链式调用）

### `Yaf_Request_Abstract::getException`

在异常捕获模式（application.dispatcher.catchException 开启）下，当流程进入 ErrorController 的 error 动作时，用本方法获取当前发生的异常对象。实现上是读取名为 "exception" 的路由参数。

```php
public function getException(): ?Exception
```

**返回值**: 有异常时返回当前异常对象，没有异常时返回 NULL

**示例**

```php
class ErrorController extends Yaf_Controller_Abstract {
    public function errorAction() {
        $exception = $this->getRequest()->getException();
    }
}
```

### `Yaf_Request_Abstract::getModuleName`

获取当前请求被路由到的模块名。

```php
public function getModuleName(): ?string
```

**返回值**: 路由成功后返回当前处理该请求的模块名；路由之前返回 NULL

**示例**

```php
echo "current Module:" . $this->getRequest()->getModuleName();
```

### `Yaf_Request_Abstract::getControllerName`

获取当前请求被路由到的控制器名。

```php
public function getControllerName(): ?string
```

**返回值**: 路由成功后返回当前处理该请求的控制器名（CamelCase 风格）；路由之前返回 NULL

**示例**

```php
echo "current Controller:" . $this->getRequest()->getControllerName();
```

### `Yaf_Request_Abstract::getActionName`

获取当前请求被路由到的动作（Action）名。

```php
public function getActionName(): ?string
```

**返回值**: 路由成功后返回当前处理该请求的动作名（小写）；路由之前返回 NULL

**示例**

```php
echo "current Action:" . $this->getRequest()->getActionName();
```

### `Yaf_Request_Abstract::setModuleName`

设置请求将被分发到的模块名。format_name 为 TRUE 时按 CamelCase 格式化后保存；为 FALSE 时原样保存。传入空字符串不做任何修改。

```php
public function setModuleName(string $module, bool $format_name = true): ?object
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$module` | 模块名 |
| `$format_name` | 是否格式化模块名，默认 TRUE |

**返回值**: 返回 Yaf_Request_Abstract 实例自身（可链式调用）

### `Yaf_Request_Abstract::setControllerName`

设置请求将被分发到的控制器名。format_name 为 TRUE 时按 CamelCase 格式化后保存；为 FALSE 时原样保存。传入空字符串不做任何修改。

```php
public function setControllerName(string $controller, bool $format_name = true): ?object
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$controller` | 控制器名 |
| `$format_name` | 是否格式化控制器名，默认 TRUE |

**返回值**: 返回 Yaf_Request_Abstract 实例自身（可链式调用）

### `Yaf_Request_Abstract::setActionName`

设置请求将被分发到的动作名。format_name 为 TRUE 时转为小写后保存；为 FALSE 时原样保存。传入空字符串不做任何修改。

```php
public function setActionName(string $action, bool $format_name = true): ?object
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$action` | 动作名 |
| `$format_name` | 是否格式化动作名，默认 TRUE |

**返回值**: 返回 Yaf_Request_Abstract 实例自身（可链式调用）

### `Yaf_Request_Abstract::getMethod`

获取当前请求的方法类型，可能的返回值如 GET、POST、HEAD、PUT、CLI 等。HTTP 环境取自 SAPI 请求方法，CLI 环境下为 "CLI"。

```php
public function getMethod(): ?string
```

**返回值**: 当前请求的方法字符串（原样返回，不做大小写转换）

**示例**

```php
if ($this->getRequest()->getMethod() == "CLI") {
    echo "running in cli mode";
}
```

### `Yaf_Request_Abstract::getLanguage`

获取客户端首选语言：解析 $_SERVER["HTTP_ACCEPT_LANGUAGE"]，按 q 值取权重最高的语言标签；首次解析后结果被缓存。

```php
public function getLanguage(): ?string
```

**返回值**: 首选语言字符串（如 zh-CN）；无法获取时返回 NULL

### `Yaf_Request_Abstract::setBaseUri`

设置基础 URI。路由时该前缀会从请求 URI 中被剥离（剥离比较不区分大小写）。传入的 URI 尾部斜杠会被去除，空字符串会被拒绝。

```php
public function setBaseUri(string $uir): object|false
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$uir` | 基础 URI，如 "/app" |

**返回值**: 成功返回 Yaf_Request_Abstract 实例自身；URI 为空时返回 FALSE

**示例**

```php
$request->setBaseUri("/app")->setRequestUri("/app/user/list");
```

### `Yaf_Request_Abstract::getBaseUri`

获取之前设置（或构造时推断）的基础 URI。

```php
public function getBaseUri(): ?string
```

**返回值**: 基础 URI 字符串；未设置时返回空字符串

### `Yaf_Request_Abstract::getRequestUri`

获取当前请求的 URI，路由即基于该值进行。HTTP 环境下通常由 Yaf 在构造时自动设置。

```php
public function getRequestUri(): ?string
```

**返回值**: 请求 URI 字符串；未设置时返回空字符串

### `Yaf_Request_Abstract::setRequestUri`

设置请求 URI，路由将基于该值进行。常用于改写路由前的 URI。

```php
public function setRequestUri(string $uir): ?object
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$uir` | 请求 URI |

**返回值**: 返回 Yaf_Request_Abstract 实例自身（可链式调用）

### `Yaf_Request_Abstract::isDispatched`

判断请求是否已被分发（final 方法）。分发循环中若该标记为 TRUE，Yaf_Dispatcher 会终止循环。

```php
public final function isDispatched(): bool
```

**返回值**: 已分发返回 TRUE，否则返回 FALSE

### `Yaf_Request_Abstract::setDispatched`

设置请求的分发状态（final 方法）。在插件中将其置为 TRUE 可以阻止 Yaf_Dispatcher 继续分发。

```php
public final function setDispatched(bool $flag = true): ?object
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$flag` | 分发状态，默认 TRUE |

**返回值**: 返回 Yaf_Request_Abstract 实例自身（可链式调用）

### `Yaf_Request_Abstract::isRouted`

判断请求是否已完成路由（final 方法）。

```php
public final function isRouted(): bool
```

**返回值**: 已路由返回 TRUE，否则返回 FALSE

### `Yaf_Request_Abstract::setRouted`

设置请求的路由状态（final 方法）。自定义路由协议匹配成功后应调用本方法，Yaf_Dispatcher 将跳过后续路由流程。

```php
public final function setRouted(bool $flag = true): ?object
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$flag` | 路由状态，默认 TRUE |

**返回值**: 返回 Yaf_Request_Abstract 实例自身（可链式调用）

**示例**

```php
// 在自定义 Route 的 route() 方法中
$request->setControllerName("product")
        ->setActionName("detail")
        ->setRouted();
```

