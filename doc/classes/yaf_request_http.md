# Yaf_Request_Http

HTTP 环境下的请求对象，继承自 Yaf_Request_Abstract。Web 请求时由 Yaf 框架自动创建，也可手动构造（常用于测试）。构造时自动确定请求方法和请求 URI；另定义了 SCHEME_HTTP、SCHEME_HTTPS 两个常量。

```php
class Yaf_Request_Http extends Yaf_Request_Abstract
```

继承: `Yaf_Request_Abstract` ← `Yaf_Request_Http`

## 方法

### `Yaf_Request_Http::__construct`

构造 HTTP 请求对象。requestUri 省略时，Yaf 依次尝试从 $_SERVER 的 PATH_INFO、REQUEST_URI、ORIG_PATH_INFO 中探测请求 URI；baseUri 省略时会根据 SCRIPT_NAME/PHP_SELF/ORIG_SCRIPT_NAME 自动推断，传入的 baseUri 尾部斜杠会被去除。请求方法取自 SAPI，无法获取时为 "CLI"。

```php
public function __construct(?string $requestUri = NULL, ?string $baseUri = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$requestUri` | 请求 URI，如 "/user/list"；省略时自动探测 |
| `$baseUri` | 基础 URI，如 "/app"；省略时自动推断 |

**返回值**: 无返回值（构造方法）

**示例**

```php
$request = new Yaf_Request_Http("/user/list", "/app");
echo $request->getRequestUri(); // /user/list
echo $request->getBaseUri();    // /app
```

## 继承的方法

- [Yaf_Request_Abstract](yaf_request_abstract.md): `isGet`, `isPost`, `isDelete`, `isPatch`, `isPut`, `isHead`, `isOptions`, `isCli`, `isXmlHttpRequest`, `getQuery`, `getRequest`, `getPost`, `getCookie`, `getRaw`, `getFiles`, `get`, `getServer`, `getEnv`, `setParam`, `getParam`, `getParams`, `clearParams`, `getException`, `getModuleName`, `getControllerName`, `getActionName`, `setModuleName`, `setControllerName`, `setActionName`, `getMethod`, `getLanguage`, `setBaseUri`, `getBaseUri`, `getRequestUri`, `setRequestUri`, `isDispatched`, `setDispatched`, `isRouted`, `setRouted`

