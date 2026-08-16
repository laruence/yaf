# Yaf_Route_Simple

**Yaf_Route_Simple** 基于 `$_GET` 中的三个变量做路由：构造时给出三个键名，分别代表模块、控制器、动作在 query string 中的变量名（例如 `new Yaf_Route_Simple("m", "c", "a")` 对应 `?m=xxx&c=xxx&a=xxx`）。路由时从 `$_GET` 按这三个键取值：模块值还须是合法模块名（`Yaf_Application::$modules`）才会被设置；三个键在 `$_GET` 中全都不存在时路由失败，把路由权交给下一条协议。注意只要有一个键存在即视为匹配成功，即使个别值不是字符串。

```php
final class Yaf_Route_Simple implements Yaf_Route_Interface
```

## 方法

### `Yaf_Route_Simple::__construct`

创建 Simple 路由。三个参数不是路由结果本身，而是 `$_GET` 中承载模块、控制器、动作的变量名（键名）。

```php
public function __construct(string $module_name, string $controller_name, string $action_name): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$module_name` | `$_GET` 中承载模块名的键名 |
| `$controller_name` | `$_GET` 中承载控制器名的键名 |
| `$action_name` | `$_GET` 中承载动作名的键名 |

**返回值**: 无返回值（构造函数）；参数不是字符串时抛出 `ArgumentCountError`/`TypeError`

**示例**

```php
// 对于 http://domain.com/index.php?c=index&a=test
$route = new Yaf_Route_Simple('m', 'c', 'a');
$router->addRoute('simple', $route);
// 路由结果: controller=index, action=test
```

### `Yaf_Route_Simple::route`

从 `$_GET` 按构造时指定的三个键读取模块/控制器/动作并写入 request。模块值须是合法模块名才会被采用；三个键全都不存在时视为未匹配。

```php
public function route($request): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$request` | 待路由的 `Yaf_Request_Abstract` 实例 |

**返回值**: `$_GET` 中存在三个键之一即返回 TRUE，全不存在返回 FALSE

**示例**

```php
$route = new Yaf_Route_Simple('m', 'c', 'a');
$request = new Yaf_Request_Http('/');
$_GET = ['c' => 'index', 'a' => 'test'];
var_dump($route->route($request)); // bool(true)
```

### `Yaf_Route_Simple::assemble`

按 Simple 路由规则组装 URL：用构造时的三个键名输出 query string，`info` 中 `:c`、`:a` 必填，`:m` 可选，形如 `?m=模块&c=控制器&a=动作`；`query` 以 `&k=v` 追加。

```php
public function assemble(array $info, ?array $query = NULL): string|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$info` | 路由信息数组，必须包含 `:c` 和 `:a`，可选 `:m` |
| `$query` | 附加查询参数数组 |

**返回值**: 成功返回组装的 query string；`info` 缺少 `:c` 或 `:a` 时触发 `Yaf_Exception_TypeError` 并返回 NULL

**示例**

```php
$router = new Yaf_Router();
$router->addRoute('simple', new Yaf_Route_Simple('m', 'c', 'a'));
echo $router->getRoute('simple')->assemble(
    [':m' => 'yafmodule', ':c' => 'yafcontroller', ':a' => 'yafaction'],
    ['tkey' => 'tval']
); // ?m=yafmodule&c=yafcontroller&a=yafaction&tkey=tval
```

