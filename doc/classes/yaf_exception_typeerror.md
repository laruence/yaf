# Yaf_Exception_TypeError

类型错误，对应错误码 `YAF_ERR_TYPE_ERROR` (521)。典型场景：配置项或 API 参数类型不符合要求。

```php
class Yaf_Exception_TypeError extends Yaf_Exception implements Stringable, Throwable
```

继承: `Exception` ← `Yaf_Exception` ← `Yaf_Exception_TypeError`

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

