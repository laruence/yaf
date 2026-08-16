# Yaf_Route_Static

**Yaf_Route_Static** 是 Yaf 的默认路由协议（路由器中以 `_default` 注册），解析去除 base_uri 后的 request_uri：按 `/` 分段，依次尝试解释为模块、控制器、动作。第一段需通过 `Yaf_Application::$modules`（`yaf.application.modules`）校验才算模块，否则整体前移（第一段当控制器、第二段当动作）；剩余段按 `name/value` 成对解析为请求参数，落单的名值为 NULL。当 URI 只有一段时，`yaf.action_prefer=1` 会把它解释为动作而非控制器。控制器/模块名会按驼峰规整，动作名统一小写。该协议总是匹配成功（`match` 恒返回 TRUE），因此注册顺序上应放在其他路由之后。

```php
final class Yaf_Route_Static implements Yaf_Route_Interface
```

## 方法

### `Yaf_Route_Static::match`

静态路由没有匹配模式，对任何 URI 都视为匹配成功，本方法恒返回 TRUE（仅为满足协议形态而存在）。

```php
public function match(string $uri): bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$uri` | 任意 URI 字符串 |

**返回值**: 恒返回 TRUE

### `Yaf_Route_Static::route`

按 `/` 分段解析 request 的 URI（先剥离 base_uri），依次确定模块、控制器、动作；第一段不是合法模块名时整体前移为控制器/动作，剩余段按 `name/value` 成对写入请求参数。控制器/模块名按驼峰规整，动作名转小写。

```php
public function route($request): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$request` | 待路由的 `Yaf_Request_Abstract` 实例 |

**返回值**: 恒返回 TRUE（静态路由总能解析出结果）

**示例**

```php
// 请求 "/ap/foo/bar/dummy/1"，base_uri 为 "/ap"，
// 未配置名为 foo 的模块时路由结果为：
// controller=foo, action=bar, params[dummy]=1
```

### `Yaf_Route_Static::assemble`

按静态路由规则组装 URL：`info` 中 `:c`（控制器）、`:a`（动作）必填，`:m`（模块）可选，输出形如 `/模块/控制器/动作` 的路径；`query` 以 `?k=v&k=v` 形式附加。

```php
public function assemble(array $info, ?array $query = NULL): string|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$info` | 路由信息数组，必须包含 `:c` 和 `:a`，可选 `:m` |
| `$query` | 附加查询参数数组 |

**返回值**: 成功返回组装的 URL 字符串；`info` 缺少 `:c` 或 `:a` 时触发 `Yaf_Exception_TypeError` 并返回 FALSE

**示例**

```php
$router = new Yaf_Router();
echo $router->getRoute('_default')->assemble(
    [':m' => 'module', ':c' => 'controller', ':a' => 'action'],
    ['tkey' => 'tval']
); // /module/controller/action?tkey=tval
```

