# Yaf_Route_Supervar

**Yaf_Route_Supervar** 与 Simple 相似，也从 `$_GET` 取路由信息，不同之处在于它取的是**一个包含完整路由路径的变量**：构造时给出变量名，如 `new Yaf_Route_Supervar("r")` 对应 `?r=/module/controller/action/k/v`。变量值按与默认静态路由（`Yaf_Route_Static`）完全相同的规则解析（模块校验、`name/value` 参数、`yaf.action_prefer` 对单段 URI 的影响也一致）。`$_GET` 中不存在该变量、或该变量的值不是字符串（如 `?r[a]=b` 这样的数组）时路由失败，把路由权交给下一条协议。

```php
final class Yaf_Route_Supervar implements Yaf_Route_Interface
```

## 方法

### `Yaf_Route_Supervar::__construct`

创建 Supervar 路由，指定 `$_GET` 中承载完整路由路径的变量名。

```php
public function __construct(string $supervar_name): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$supervar_name` | `$_GET` 中承载路由路径的变量名 |

**返回值**: 无返回值（构造函数）；参数不是字符串时抛出 `ArgumentCountError`/`TypeError`

**示例**

```php
// 对于 http://domain.com/index.php?r=/a/b/c
$route = new Yaf_Route_Supervar('r');
$router->addRoute('super', $route);
// 路由结果: module=a, controller=b, action=c
```

### `Yaf_Route_Supervar::route`

读取 `$_GET` 中 supervar 变量的值作为路由路径，按静态路由规则（`/` 分段、模块校验、参数成对解析）填充 request。值为数组等非字符串类型时视为未匹配。

```php
public function route($request): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$request` | 待路由的 `Yaf_Request_Abstract` 实例 |

**返回值**: supervar 存在且为字符串返回 TRUE，否则返回 FALSE

**示例**

```php
$route = new Yaf_Route_Supervar('r');
$request = new Yaf_Request_Http('/');
$_GET = ['r' => '/a/b/c'];
var_dump($route->route($request)); // bool(true)
```

### `Yaf_Route_Supervar::assemble`

按 Supervar 规则组装 URL：输出 `?变量名=/模块/控制器/动作` 形式的路径，`info` 中 `:c`、`:a` 必填，`:m` 可选；`query` 以 `&k=v` 追加。

```php
public function assemble(array $info, ?array $query = NULL): string|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$info` | 路由信息数组，必须包含 `:c` 和 `:a`，可选 `:m` |
| `$query` | 附加查询参数数组 |

**返回值**: 成功返回组装的 URL；`info` 缺少 `:c` 或 `:a` 时触发 `Yaf_Exception_TypeError` 并返回 NULL

**示例**

```php
$router = new Yaf_Router();
$router->addRoute('supervar', new Yaf_Route_Supervar('r'));
echo $router->getRoute('supervar')->assemble(
    [':m' => 'yafmodule', ':c' => 'yafcontroller', ':a' => 'yafaction'],
    ['tkey' => 'tval']
); // ?r=/yafmodule/yafcontroller/yafaction&tkey=tval
```

