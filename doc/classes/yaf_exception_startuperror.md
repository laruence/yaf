# Yaf_Exception_StartupError

启动阶段错误，对应错误码 `YAF_ERR_STARTUP_FAILED` (512)。典型场景：重复实例化 `Yaf_Application`、应用初始化配置错误。

```php
class Yaf_Exception_StartupError extends Yaf_Exception implements Stringable, Throwable
```

继承: `Exception` ← `Yaf_Exception` ← `Yaf_Exception_StartupError`

## 属性

```php
protected string $file;
protected int $line;
protected mixed $message;
protected mixed $code;
protected mixed $previous;
```

## 继承的方法

- `Exception`: `__construct`, `__wakeup`, `getMessage`, `getCode`, `getFile`, `getLine`, `getTrace`, `getPrevious`, `getTraceAsString`, `__toString`

