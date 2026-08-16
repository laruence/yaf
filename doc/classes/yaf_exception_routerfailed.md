# Yaf_Exception_RouterFailed

路由阶段错误，对应错误码 `YAF_ERR_ROUTE_FAILED` (513)。典型场景：自定义路由协议实现不符合接口要求。

```php
class Yaf_Exception_RouterFailed extends Yaf_Exception implements Stringable, Throwable
```

继承: `Exception` ← `Yaf_Exception` ← `Yaf_Exception_RouterFailed`

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

