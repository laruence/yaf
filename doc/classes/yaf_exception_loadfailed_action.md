# Yaf_Exception_LoadFailed_Action

动作未找到，对应错误码 `YAF_ERR_NOTFOUND_ACTION` (517)。

```php
class Yaf_Exception_LoadFailed_Action extends Yaf_Exception_LoadFailed implements Throwable, Stringable
```

继承: `Exception` ← `Yaf_Exception` ← `Yaf_Exception_LoadFailed` ← `Yaf_Exception_LoadFailed_Action`

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

