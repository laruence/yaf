# Yaf_Request_Simple

简单请求对象，继承自 Yaf_Request_Abstract，主要用于 CLI 脚本和单元测试。可以直接指定请求方法、模块、控制器、动作和路由参数来构造一个请求；未提供 MVC 信息时，会从 $_SERVER["argv"] 中查找形如 request_uri=xxx 的参数作为请求 URI 交给路由器解析。

```php
class Yaf_Request_Simple extends Yaf_Request_Abstract
```

继承: `Yaf_Request_Abstract` ← `Yaf_Request_Simple`

## 方法

### `Yaf_Request_Simple::__construct`

构造简单请求对象。两种模式：传入 module/controller/action 中任意一个时，请求直接标记为已路由（routed），缺省部分用默认值补齐（模块/控制器默认 Index、动作默认 index），名字会被格式化（模块/控制器转 CamelCase、动作转小写）；全部省略时扫描 $_SERVER["argv"] 中的 request_uri= 字符串作为请求 URI，等待路由。

```php
public function __construct(?string $method = NULL, ?string $module = NULL, ?string $controller = NULL, ?string $action = NULL, ?array $params = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$method` | 请求方法，如 "CLI"、"GET"；省略时取自 SAPI |
| `$module` | 模块名 |
| `$controller` | 控制器名 |
| `$action` | 动作名 |
| `$params` | 路由参数数组 |

**返回值**: 无返回值（构造方法）

**示例**

```php
// CLI 下模拟对 DummyController::indexAction 的一次请求
$request = new Yaf_Request_Simple("CLI", "index", "dummy", null, ["foo" => "bar"]);
var_dump($request->isRouted());        // true
var_dump($request->getControllerName()); // Dummy
```

### `Yaf_Request_Simple::isXmlHttpRequest`

覆写了父类实现。Simple 请求面向 CLI/测试场景，不可能是 Ajax 请求。

```php
public function isXmlHttpRequest(): bool
```

**返回值**: 恒返回 FALSE

## 继承的方法

- [Yaf_Request_Abstract](yaf_request_abstract.md): `isGet`, `isPost`, `isDelete`, `isPatch`, `isPut`, `isHead`, `isOptions`, `isCli`, `getQuery`, `getRequest`, `getPost`, `getCookie`, `getRaw`, `getFiles`, `get`, `getServer`, `getEnv`, `setParam`, `getParam`, `getParams`, `clearParams`, `getException`, `getModuleName`, `getControllerName`, `getActionName`, `setModuleName`, `setControllerName`, `setActionName`, `getMethod`, `getLanguage`, `setBaseUri`, `getBaseUri`, `getRequestUri`, `setRequestUri`, `isDispatched`, `setDispatched`, `isRouted`, `setRouted`

