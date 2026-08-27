# 错误与异常处理

Yaf 有两条独立的错误通道：**框架错误**（路由失败、类加载失败等 C 层错误）和 **PHP 异常/错误**（应用代码抛出的）。理解这两条通道的开关组合，是写好错误处理的前提。

## 通道一：框架错误 → throwException 决定形态

所有 C 层错误都经过 `yaf_trigger_error()`（yaf_exception.c），行为取决于 `yaf.dispatcher.throwException`：

| throwException | 行为 |
| --- | --- |
| `0`（默认） | 触发 `E_RECOVERABLE_ERROR`，同时把错误码/消息存入 Application（`getLastErrorNo()` / `getLastErrorMsg()` 可取回，`clearLastError()` 清除） |
| `1` | 抛出对应的 `Yaf_Exception_*` 子类 |

错误码与异常类的对应关系（yaf_throw_exception 按 `YAF_ERR_*` 码选择异常类）：

| 错误码 | 常量 | 异常类 |
| --- | --- | --- |
| 512 | `YAF_ERR_STARTUP_FAILED` | [Yaf_Exception_StartupError](../classes/yaf_exception_startuperror.md) |
| 513 | `YAF_ERR_ROUTE_FAILED` | [Yaf_Exception_RouterFailed](../classes/yaf_exception_routerfailed.md) |
| 514 | `YAF_ERR_DISPATCH_FAILED` | [Yaf_Exception_DispatchFailed](../classes/yaf_exception_dispatchfailed.md) |
| 520 | `YAF_ERR_AUTOLOAD_FAILED` | [Yaf_Exception_LoadFailed](../classes/yaf_exception_loadfailed.md) |
| 515–518 | `YAF_ERR_NOTFOUND_*` | LoadFailed 的 Module/Controller/Action/View 子类 |
| 521 | `YAF_ERR_TYPE_ERROR` | [Yaf_Exception_TypeError](../classes/yaf_exception_typeerror.md) |

开启异常模式：

```php
// php.ini: yaf.dispatcher.throwException=1
// 或运行时:
Yaf_Dispatcher::getInstance()->throwException(TRUE);

try {
    $app->run();
} catch (Yaf_Exception_LoadFailed_Controller $e) {
    // 404: 控制器不存在
} catch (Yaf_Exception $e) {
    // 其余框架错误
}
```

非异常模式下查询最近错误：

```php
$app->run();
if ($app->getLastErrorNo()) {
    var_dump($app->getLastErrorNo(), $app->getLastErrorMsg());
    $app->clearLastError();
}
```

## 通道二：应用异常 → catchException 决定去向

分发过程中应用代码抛出的异常（action 里 throw、视图渲染报错等），当 `Yaf_Dispatcher::catchException(TRUE)` 开启时，由内建处理器接管（yaf_dispatcher.c `yaf_dispatcher_exception_handler`）：

1. 取出当前异常，清空 PHP 的异常状态
2. 把请求转发到 **Error 控制器的 errorAction**（当前模块下）
3. 异常对象以路由参数 `exception` 传入，action 里用 `$this->getRequest()->getException()` 取回

```php
// Bootstrap 或入口文件开启
Yaf_Dispatcher::getInstance()->catchException(TRUE);
```

```php
class ErrorController extends Yaf_Controller_Abstract
{
    public function errorAction($exception)
    {
        Yaf_Dispatcher::getInstance()->disableView();
        switch (true) {
            case $exception instanceof Yaf_Exception_LoadFailed_Action:
            case $exception instanceof Yaf_Exception_LoadFailed_Controller:
                $this->getResponse()->setHeader('HTTP/1.0 404 Not Found');
                break;
            default:
                error_log($exception->getTraceAsString());
        }
        $this->getResponse()->setBody('<h1>出错了</h1>');
        return FALSE; // 阻止自动渲染
    }
}
```

注意三个边界（均来自源码）：

- **防递归**：errorAction 内部再抛的异常不会被同一次分发捕获（`YAF_DISPATCHER_IN_EXCEPTION` 标志），只会影响下一次 `dispatch()` 调用
- **时机**：处理器只在分发循环的检查点生效；`routerStartup` 钩子里抛异常时模块名可能还没解析，会回退到默认模块
- **exit**：`zend_unwind_exit`（如 action 里 `exit`）不被当作异常处理

## PHP 错误 → setErrorHandler

[Yaf_Dispatcher::setErrorHandler](../classes/yaf_dispatcher.md) 只是 PHP 原生 `set_error_handler()` 的包装，用于把 `E_WARNING`/`E_NOTICE` 等转交自定义回调（常见做法是在回调里 throw 成 ErrorException）：

```php
Yaf_Dispatcher::getInstance()->setErrorHandler(function ($errno, $errstr) {
    if (!(error_reporting() & $errno)) {
        return;
    }
    throw new ErrorException($errstr, 0, $errno);
});
```

## 组合速查

| throwException | catchException | 效果 |
| --- | --- | --- |
| 0 | 0 | 框架错误走 E_RECOVERABLE_ERROR，应用异常直接冒泡给 PHP |
| 1 | 0 | 框架错误和应用异常都可被入口 try/catch 捕获（自己控制错误页） |
| 1 | 1 | 框架错误可 try/catch；应用异常自动转发到 ErrorController |
| 0 | 1 | 框架错误仍是 E_RECOVERABLE_ERROR，应用异常转发到 ErrorController |

一般推荐最后一列的 `1|1` 组合：框架错误在入口 catch，应用异常交给 ErrorController 统一渲染错误页。

## See Also

- [常量](../constants.md) — 全部 `YAF_ERR_*` 错误码
- [请求生命周期](lifecycle.md) — 异常处理发生在哪个阶段
