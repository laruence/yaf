# Yaf_Route_Rewrite

**Yaf_Route_Rewrite** 基于匹配模式对 URI 做重写路由，是功能最灵活的内建路由。构造参数 `match` 是匹配模式，支持两种标记：`:变量名` 匹配一个 URL 段并把值记为同名参数（如 `/product/:ident`）；`*` 通配其后所有段，按 `name/value` 成对解析为参数。段间以 `/` 分隔，匹配区分大小写由内部生成的正则决定（带 `i` 修饰符，大小写不敏感），且自 URI 头部锚定。`route` 参数给出路由目标数组（`module`/`controller`/`action`），其值若以 `:` 开头则引用匹配出的同名变量，否则为固定值。`verify` 参数被接受但当前版本未参与匹配逻辑。匹配成功后所有捕获变量（含 `*` 展开的参数）写入请求参数。

```php
final class Yaf_Route_Rewrite implements Yaf_Route_Interface
```

## 方法

### `Yaf_Route_Rewrite::__construct`

创建 Rewrite 路由：给出匹配模式、路由目标映射，以及可选的 verify。

```php
public function __construct(string $match, array $route, ?array $verify = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$match` | 匹配模式，支持 `:变量名` 与 `*` 通配符，如 `/product/:ident/*` |
| `$route` | 路由目标数组，可含 `module`/`controller`/`action`；值以 `:` 开头时引用匹配出的同名变量，否则为固定值 |
| `$verify` | 可选，数组（当前版本保留，未参与匹配逻辑） |

**返回值**: 无返回值（构造函数）；`match` 非字符串或 `route` 非数组时抛出 `ArgumentCountError`/`TypeError`

**示例**

```php
$route = new Yaf_Route_Rewrite(
    '/product/:ident',
    ['controller' => 'products', 'action' => 'view']
);
$router->addRoute('product', $route);
// /product/chocolate-bar → products/view, params[ident]=chocolate-bar
```

### `Yaf_Route_Rewrite::match`

用构造时的匹配模式测试一个 URI，不修改任何 request 状态。模式内部的 `:变量名` 编译为命名捕获组，`*` 编译为捕获剩余部分的通配组（其内容按 `name/value` 成对展开为参数）。

```php
public function match(string $uri): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$uri` | 待测试的 URI 字符串 |

**返回值**: 匹配成功返回捕获结果数组（变量名 => 值，含 `*` 展开的参数）；URI 为空或不匹配返回 FALSE

**示例**

```php
$route = new Yaf_Route_Rewrite('/product/:ident/*',
    ['controller' => 'products', 'action' => 'view']);
var_dump($route->match('/product/ipod/page/2'));
// ["ident"]=>"ipod", ["page"]=>"2"
```

### `Yaf_Route_Rewrite::route`

对 request 的 URI（去除 base_uri 后）执行模式匹配；成功后按 `route` 映射设置模块/控制器/动作（映射值以 `:` 开头时取捕获的同名变量），并把全部捕获变量写入请求参数。

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
$route = new Yaf_Route_Rewrite('/product/:ident',
    ['controller' => 'products', 'action' => 'view']);
$request = new Yaf_Request_Http('/product/ipod');
var_dump($route->route($request)); // bool(true)
echo $request->getParam('ident');   // ipod
```

### `Yaf_Route_Rewrite::assemble`

按构造时的匹配模式反向组装 URL：`info` 中提供与模式中 `:变量名` 同名的键进行替换（含 `:m`/`:c`/`:a` 形式），`*` 通配段由 `info` 中其余未消费的键按 `key/value/key/value` 展开填充；`query` 以 `?k=v&k=v` 附加。

```php
public function assemble(array $info, ?array $query = NULL): string|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$info` | 路由信息数组，键需与模式中的 `:变量名` 对应 |
| `$query` | 附加查询参数数组 |

**返回值**: 返回组装的 URL 字符串；缺少占位符对应的键时占位符保持原样

**示例**

```php
$route = new Yaf_Route_Rewrite('/product/:ident/*',
    ['controller' => 'products', 'action' => 'view']);
echo $route->assemble(
    [':ident' => 'ipod', ':sort' => 'price'],
    ['page' => 2]
); // /product/ipod/sort/price/?page=2
```

