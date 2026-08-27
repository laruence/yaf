# Yaf_Exception_DispatchFailed

分发阶段错误，对应错误码 `YAF_ERR_DISPATCH_FAILED` (514)。典型场景：action 方法不存在或不可调用。

```php
class Yaf_Exception_DispatchFailed extends Yaf_Exception implements Stringable, Throwable
```

继承: `Exception` ← `Yaf_Exception` ← `Yaf_Exception_DispatchFailed`

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

