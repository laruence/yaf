# Yaf_Router

**Yaf_Router** 是 Yaf 的路由器，负责把 `Yaf_Request_Abstract` 请求路由到具体的模块/控制器/动作。路由器内部维护一组按名字索引的路由协议（`Yaf_Route_Interface` 实例），初始化时自动注册名为 `_default` 的默认路由（`Yaf_Route_Static`，可通过配置 `application.dispatcher.defaultRoute` 替换）。路由时按注册的**逆序**依次尝试各协议，第一个匹配成功的协议生效，其名字被记为当前路由。除默认静态路由外，内建协议还有 Simple、Supervar、Rewrite、Regex、Map，也可注册自定义路由类。该类 final、不可序列化、不可克隆；开启 `yaf.use_namespace` 时也可用 `Yaf\Router`。

```php
final class Yaf_Router
```

## 方法

### `Yaf_Router::__construct`

构造路由器并注册默认路由协议：若应用配置了 `application.dispatcher.defaultRoute` 则按其构建，否则回退为 `Yaf_Route_Static`（默认路由构建失败时触发 E_WARNING 并同样回退）。

```php
public function __construct(): mixed
```

**返回值**: 无返回值（构造函数）

**示例**

```php
$router = new Yaf_Router();
print_r($router->getRoutes());
// [_default] => Yaf_Route_Static Object
```

### `Yaf_Router::addRoute`

向路由器注册一个路由协议。后注册的路由先被尝试（路由按注册逆序匹配），同名重复注册会覆盖旧协议。

```php
public function addRoute(string $name, object $route): ?object
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 路由协议的名字，之后可用 `getRoute($name)` 取回 |
| `$route` | 实现了 `Yaf_Route_Interface` 的路由协议实例 |

**返回值**: 成功返回 `$this`（Yaf_Router），支持链式调用

**示例**

```php
$router = Yaf_Dispatcher::getInstance()->getRouter();
$route  = new Yaf_Route_Rewrite(
    "/product/list/:id",
    ['controller' => 'product', 'action' => 'info']
);
$router->addRoute('product', $route);
```

### `Yaf_Router::addConfig`

批量注册路由协议。遍历配置，每一项是描述一条路由的数组，必须包含 `type` 键（rewrite/regex/map/simple/supervar，不区分大小写），其余键依类型而定（如 rewrite/regex 需 `match` + `route`，simple 需 `module`/`controller`/`action`，supervar 需 `varname`，map 可选 `controllerPrefer`/`delimiter`）。配置项的键作为路由名；单个条目非法时触发 E_WARNING 并跳过，不影响其余条目。

```php
public function addConfig(object|array $config): object|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$config` | `Yaf_Config_Abstract` 实例或普通数组，内容为「路由名 => 路由配置数组」 |

**返回值**: 成功返回 `$this`（Yaf_Router）；参数既非 Yaf_Config_Abstract 实例也非数组时触发 E_WARNING 并返回 FALSE

**示例**

```php
$config = [
    'rewrite' => ['type' => 'rewrite', 'match' => '/yaf/:name/:value',
                  'route' => ['controller' => 'Index', 'action' => 'action']],
    'simple'  => ['type' => 'simple', 'module' => 'm', 'controller' => 'c', 'action' => 'a'],
];
$router->addConfig($config);
```

### `Yaf_Router::route`

对请求执行路由：按注册的逆序依次调用各路由协议的 `route()`，第一个匹配成功的协议负责填充 request 的模块/控制器/动作及请求参数，其名字被记为当前路由，请求被标记为已路由（`isRouted()` 为真）。内置协议直接走 C 实现；自定义协议调用其 PHP 层 `route()` 方法，返回非真值则继续尝试下一条。通常无需手动调用，`Yaf_Dispatcher::dispatch` 会自动调用。

```php
public function route(object $request): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$request` | 要路由的 `Yaf_Request_Abstract` 实例 |

**返回值**: 有路由协议匹配成功返回 TRUE；全部协议都未匹配返回 FALSE

**示例**

```php
$router  = new Yaf_Router();
$request = new Yaf_Request_Http('/foo/bar');
var_dump($router->route($request));       // bool(true)
var_dump($router->getCurrentRoute());     // string(8) "_default"
```

### `Yaf_Router::getRoute`

按名字获取已注册的路由协议。

```php
public function getRoute(string $name): object|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 路由协议的名字（`addRoute` 时使用的名字；默认静态路由为 `_default`） |

**返回值**: 返回对应的 `Yaf_Route_Interface` 实例；名字为空字符串或路由不存在时返回 FALSE/NULL

**示例**

```php
$router->addRoute('simple', new Yaf_Route_Simple('m', 'c', 'a'));
$route = $router->getRoute('simple');      // Yaf_Route_Simple
var_dump($router->getRoute('noexists'));  // NULL
```

### `Yaf_Router::getRoutes`

获取路由器中已注册的全部路由协议。

```php
public function getRoutes(): ?array
```

**返回值**: 「路由名 => 路由协议实例」的数组副本（含 `_default` 默认路由）

**示例**

```php
$router = new Yaf_Router();
$router->addRoute('super', new Yaf_Route_Supervar('r'));
print_r(array_keys($router->getRoutes())); // ["_default", "super"]
```

### `Yaf_Router::getCurrentRoute`

获取最近一次成功路由所用路由协议的名字。

```php
public function getCurrentRoute(): string|int|null
```

**返回值**: 当前路由协议的名字字符串；尚未执行过路由（或没有路由匹配成功）时返回 NULL

**示例**

```php
$router  = new Yaf_Router();
$request = new Yaf_Request_Http('/foo/bar');
$router->route($request);
var_dump($router->getCurrentRoute()); // string(8) "_default"
```

