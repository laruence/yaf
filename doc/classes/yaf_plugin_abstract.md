# Yaf_Plugin_Abstract

Yaf 插件基类，所有插件必须继承它。插件通过 `Yaf_Dispatcher::registerPlugin()` 注册，在请求生命周期的固定时机由分发器自动回调（基类空实现会被跳过，只有子类重写的方法才会被调用），每个钩子都收到当前的 `$request` 和 `$response` 对象。

钩子触发顺序（见 `yaf_dispatcher_dispatch()`）：
1. `routerStartup` — 路由之前；
2. `routerShutdown` — 路由成功之后；
3. `dispatchLoopStartup` — 分发循环开始之前；
4. `preDispatch` — 每次分发 controller action 之前（forward 导致循环多轮时，每轮都会触发）；
5. `postDispatch` — 每次分发 action 之后；
6. `dispatchLoopShutdown` — 分发循环结束之后；
7. `preResponse` — 预留钩子，当前版本不会被自动调用。

插件可以部署在应用的 `plugins/` 目录下（靠类名后缀被自动加载器识别，受 `ap.name_suffix`/`ap.name_separator` 影响），也可以作为普通类库加载。钩子内抛出的异常走框架异常处理流程。

```php
abstract class Yaf_Plugin_Abstract
```

## 方法

### `Yaf_Plugin_Abstract::routerStartup`

路由前钩子。在分发器对请求做路由之前调用，此时请求尚未路由，可用于改写请求 URI/参数、鉴权跳转等。基类实现为空，返回 TRUE；子类重写后才会被调用。

```php
public function routerStartup(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$request` | 当前请求对象 |
| `$response` | 当前响应对象 |

**返回值**: 返回值被忽略（基类返回 TRUE）

**示例**

```php
class AuthPlugin extends Yaf_Plugin_Abstract {
    public function routerStartup(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {
        // 路由前检查登录态，必要时改写请求
        if (empty($_COOKIE['sid'])) {
            $request->setControllerName('Login');
        }
    }
}
```

### `Yaf_Plugin_Abstract::routerShutdown`

路由后钩子。路由成功完成后调用，此时 request 上已设置好 module/controller/action，可用于基于路由结果做权限检查或参数修正。

```php
public function routerShutdown(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$request` | 当前请求对象（已完成路由） |
| `$response` | 当前响应对象 |

**返回值**: 返回值被忽略（基类返回 TRUE）

### `Yaf_Plugin_Abstract::dispatchLoopStartup`

分发循环启动钩子。路由完成后、进入分发循环（开始实例化 controller 并执行 action）之前调用，整个请求只触发一次。

```php
public function dispatchLoopStartup(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$request` | 当前请求对象 |
| `$response` | 当前响应对象 |

**返回值**: 返回值被忽略（基类返回 TRUE）

### `Yaf_Plugin_Abstract::dispatchLoopShutdown`

分发循环结束钩子。所有分发轮次结束（request 已被标记 dispatched，或达到 forward 上限）后调用，整个请求只触发一次，适合做收尾清理、日志记录。

```php
public function dispatchLoopShutdown(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$request` | 当前请求对象 |
| `$response` | 当前响应对象 |

**返回值**: 返回值被忽略（基类返回 TRUE）

### `Yaf_Plugin_Abstract::preDispatch`

分发前钩子。分发循环中每次执行 controller action 之前调用；若 action 内发生 forward，循环会再次经过此钩子，因此可能触发多次。

```php
public function preDispatch(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$request` | 当前请求对象 |
| `$response` | 当前响应对象 |

**返回值**: 返回值被忽略（基类返回 TRUE）

### `Yaf_Plugin_Abstract::postDispatch`

分发后钩子。每次 controller action 执行完毕后调用，与 preDispatch 配对，forward 场景下可能触发多次。

```php
public function postDispatch(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$request` | 当前请求对象 |
| `$response` | 当前响应对象 |

**返回值**: 返回值被忽略（基类返回 TRUE）

### `Yaf_Plugin_Abstract::preResponse`

响应前钩子（预留）。定义在基类中，但当前版本的分发器不会在任何时机自动调用它；仅当手动调用时才会执行。

```php
public function preResponse(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$request` | 当前请求对象 |
| `$response` | 当前响应对象 |

**返回值**: 返回值被忽略（基类返回 TRUE）

