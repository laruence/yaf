# Yaf_Exception_LoadFailed

自动加载失败基类，对应错误码 `YAF_ERR_AUTOLOAD_FAILED` (520)。`LoadFailed_Module/Controller/Action/View` 均继承自它，可用一个 catch 统一处理四类加载失败。

```php
class Yaf_Exception_LoadFailed extends Yaf_Exception implements Stringable, Throwable
```

继承: `Exception` ← `Yaf_Exception` ← `Yaf_Exception_LoadFailed`

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

