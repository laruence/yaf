# Yaf_Route_Regex

**Yaf_Route_Regex** 用完整 PCRE 正则匹配 URI，是灵活度最高的内建路由（正则需自带定界符，如 `#^/product/([^/]+)#`）。构造参数 `match` 为正则；`route` 为路由目标数组，值以 `:` 开头时引用捕获的同名变量，否则为固定值；`map` 是「捕获组序号 => 参数名」的映射，把数字分组（`$1`、`$2`…）转成有意义的参数名，正则中的命名捕获组也可直接使用；`reverse` 是反向组装用的 URL 模板（含 `:m`/`:c`/`:a` 占位符），供 `assemble` 使用；`verify` 被接受但当前版本未参与匹配。匹配成功后，所有命名/映射出的捕获变量写入请求参数。

```php
final class Yaf_Route_Regex implements Yaf_Route_Interface
```

## 方法

### `Yaf_Route_Regex::__construct`

创建 Regex 路由：给出正则、路由目标映射，可选给出捕获组映射、verify 和反向组装模板。

```php
public function __construct(string $match, array $route, ?array $map = NULL, ?array $verify = NULL, ?string $reverse = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$match` | PCRE 正则（含定界符），用于匹配 URI |
| `$route` | 路由目标数组，可含 `module`/`controller`/`action`；值以 `:` 开头时引用捕获的同名变量，否则为固定值 |
| `$map` | 可选，「捕获组序号 => 参数名」映射，如 `[1 => 'ident']` |
| `$verify` | 可选，数组（当前版本保留，未参与匹配逻辑） |
| `$reverse` | 可选，`assemble` 反向组装时使用的 URL 模板，如 `/product/:ident` |

**返回值**: 无返回值（构造函数）；`match` 非字符串或 `route` 非数组时抛出 `ArgumentCountError`/`TypeError`

**示例**

```php
$route = new Yaf_Route_Regex(
    '#^/product/([a-zA-Z0-9_-]+)#',
    ['controller' => 'products', 'action' => 'view'],
    [1 => 'ident']
);
$router->addRoute('product', $route);
```

### `Yaf_Route_Regex::match`

用构造时的正则测试一个 URI，不修改任何 request 状态。命名捕获组按组名入结果；数字分组通过 `map` 映射成参数名入结果（无 map 时数字分组被忽略）。

```php
public function match(string $uri): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$uri` | 待测试的 URI 字符串 |

**返回值**: 匹配成功返回捕获结果数组（参数名 => 值）；URI 为空、正则非法或不匹配返回 FALSE

**示例**

```php
$route = new Yaf_Route_Regex('#^/product/([^/]+)#',
    ['controller' => 'products', 'action' => 'view'],
    [1 => 'ident']);
var_dump($route->match('/product/ipod'));
// ["ident"]=>"ipod"
```

### `Yaf_Route_Regex::route`

对 request 的 URI（去除 base_uri 后）执行正则匹配；成功后按 `route` 映射设置模块/控制器/动作（映射值以 `:` 开头时取捕获的同名变量），并把全部捕获变量写入请求参数。

```php
public function route($request): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$request` | 待路由的 `Yaf_Request_Abstract` 实例 |

**返回值**: 匹配成功返回 TRUE，失败返回 FALSE

**示例**

```php
$route = new Yaf_Route_Regex('#^/product/([^/]+)#',
    ['controller' => 'products', 'action' => 'view'],
    [1 => 'ident']);
$request = new Yaf_Request_Http('/product/ipod');
var_dump($route->route($request)); // bool(true)
echo $request->getParam('ident');   // ipod
```

### `Yaf_Route_Regex::assemble`

按构造时给出的 `reverse` 模板反向组装 URL：把 `info` 中的 `:m`/`:c`/`:a` 替换进模板，`query` 以 `?k=v&k=v` 附加。

```php
public function assemble(array $info, ?array $query = NULL): string|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$info` | 路由信息数组，支持 `:m`、`:c`、`:a` 键 |
| `$query` | 附加查询参数数组 |

**返回值**: 成功返回组装的 URL；构造时未提供 `reverse` 时返回 NULL

**示例**

```php
$route = new Yaf_Route_Regex('#^/product/([^/]+)#',
    ['controller' => 'products'], [], [],
    '/:m/:c/:a');
echo $route->assemble(
    [':m' => 'module', ':c' => 'controller', ':a' => 'action'],
    ['tkey' => 'tval']
); // /module/controller/action?tkey=tval
```

