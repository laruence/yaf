# Yaf_Route_Interface

**Yaf_Route_Interface** 是 Yaf 路由协议的标准接口，所有内建路由（Static/Simple/Supervar/Rewrite/Regex/Map）都实现该接口；实现该接口即可编写自定义路由协议，并通过 `Yaf_Router::addRoute` 注册。自定义协议的 `route()` 返回真值表示匹配成功（路由器据此停止后续匹配），返回假值则把路由权交给下一条路由。开启 `yaf.use_namespace` 时也可用 `Yaf\Route_Interface`。

```php
interface Yaf_Route_Interface
```

## 方法

### `Yaf_Route_Interface::route`

路由请求：解析 request 中的 URI 或查询信息，将模块/控制器/动作及请求参数写回 request。由 `Yaf_Router::route` 调用，无需手动执行。

```php
public abstract function route($request): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$request` | 待路由的 `Yaf_Request_Abstract` 实例 |

**返回值**: 匹配成功返回 TRUE（路由器记录当前路由并停止匹配），失败返回 FALSE（继续尝试下一条路由协议）

**示例**

```php
class MyRoute implements Yaf_Route_Interface {
    public function route($request) {
        $request->setControllerName('Dummy');
        $request->setActionName('index');
        return true;
    }
    public function assemble($info, $query = null) { return ''; }
}
```

### `Yaf_Route_Interface::assemble`

反向组装 URL：根据给定的模块/控制器/动作信息，按本路由协议的规则生成对应的 URL 字符串。`info` 数组用 `:m`、`:c`、`:a` 三个键分别表示模块、控制器、动作。

```php
public abstract function assemble(array $info, ?array $query = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$info` | 路由信息数组，支持 `:m`（模块）、`:c`（控制器）、`:a`（动作）键 |
| `$query` | 附加的查询参数数组，会被拼入 URL（query string 或协议自身的参数形式） |

**返回值**: 成功返回组装好的 URL 字符串；失败（如缺少必要键、协议不支持反向组装）返回 NULL/FALSE

**示例**

```php
$router = Yaf_Dispatcher::getInstance()->getRouter();
echo $router->getRoute('_default')->assemble(
    [':c' => 'product', ':a' => 'view'],
    ['id' => 1]
); // /product/view?id=1
```

