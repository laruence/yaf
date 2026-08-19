# Yaf_Exception

Yaf 所有异常的基类，继承自 `RuntimeException`（SPL 不可用时退化为 `Exception`）。

当 `yaf.dispatcher.throwException=1`（或 `Yaf_Dispatcher::throwException(TRUE)`）时，框架错误以异常抛出；否则降级为可恢复错误（`E_RECOVERABLE_ERROR`）。错误码对应 `YAF_ERR_*` 常量，子类与错误码一一对应。

```php
class Yaf_Exception extends Exception implements Throwable, Stringable
```

继承: `Exception` ← `Yaf_Exception`

## 示例

```php
<?php
// ini: yaf.dispatcher.throwException=1
$app = new Yaf_Application('conf/app.ini');
try {
    $app->run();
} catch (Yaf_Exception $e) {
    var_dump($e->getCode(), $e->getMessage());
}
```

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

