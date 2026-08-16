# Yaf_Route_Map

**Yaf_Route_Map** 把 URI（去除 base_uri 后）按 `/` 分段并合并成一个下划线连接的名字，作为控制器或动作的路由结果：构造参数 `controller_prefer` 为 TRUE 时结果作为控制器名（如 `/foo/bar` → 控制器 `Foo_Bar`），为 FALSE（默认）时作为动作名（如 `/foo/bar` → 动作 `foo_bar`）。第二个参数 `delimiter` 指定分隔符：URI 中「`/` + 分隔符」之前的部分参与路由，之后的部分按 `name/value` 成对解析为请求参数（如 `/foo/bar/!/page/2` 配合分隔符 `!`）。Map 路由总是匹配成功，适合作为最后一条兜底路由。

```php
final class Yaf_Route_Map implements Yaf_Route_Interface
```

## 方法

### `Yaf_Route_Map::__construct`

创建 Map 路由，可选指定控制器优先标志和参数分隔符。

```php
public function __construct(bool $controller_prefer = 1, string $delimiter = ''): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$controller_prefer` | 可选，TRUE 时 URI 合并结果作为控制器名，FALSE（默认）时作为动作名 |
| `$delimiter` | 可选，URI 中路由部分与请求参数部分的分隔符字符串（如 `#!`）；不设置时整个 URI 都参与路由 |

**返回值**: 无返回值（构造函数）

**示例**

```php
$router->addRoute('map', new Yaf_Route_Map());          // 结果作为动作
$router->addRoute('map2', new Yaf_Route_Map(true, '#!'));
// 控制器优先，且 /xxx/!/k/v 中 ! 之后作为请求参数
```

### `Yaf_Route_Map::route`

把 URI 分段合并为下划线连接的名字写入 request：控制器优先时设置为控制器名（首字母及分隔处字母大写，`/foo/bar` → `Foo_Bar`），否则设置为动作名（全小写，`/foo/bar` → `foo_bar`）。设置了分隔符时，分隔符之后的部分按 `name/value` 成对解析为请求参数。

```php
public function route($request): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$request` | 待路由的 `Yaf_Request_Abstract` 实例 |

**返回值**: 恒返回 TRUE（Map 路由总是匹配成功）

**示例**

```php
// 请求 URI "/foo/bar"：
// new Yaf_Route_Map()        → action=foo_bar
// new Yaf_Route_Map(true)    → controller=Foo_Bar
// 带分隔符 "!" 时 "/foo/bar/!/page/2" → params[page]=2
```

### `Yaf_Route_Map::assemble`

按 Map 规则反向组装 URL：控制器优先时从 `info` 的 `:a`（动作名）取值，否则从 `:c`（控制器名）取值，按 `_` 拆分为路径段。设置了分隔符时 `query` 以 `/分隔符/k/v/k/v` 形式附加，否则以 `?k=v&k=v` 附加。

```php
public function assemble(array $info, ?array $query = NULL): string|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$info` | 路由信息数组；控制器优先时须含 `:a`，否则须含 `:c` |
| `$query` | 附加参数数组 |

**返回值**: 成功返回组装的 URL；缺少所需键时触发 `Yaf_Exception_TypeError` 并返回 NULL

**示例**

```php
$route = new Yaf_Route_Map();
echo $route->assemble([':c' => 'foo_bar'], ['tkey' => 'tval']);
// /foo/bar?tkey=tval
$route = new Yaf_Route_Map(true, '_');
echo $route->assemble([':a' => 'foo_bar'], ['tkey' => 'tval']);
// /foo/bar/_/tkey/tval
```

