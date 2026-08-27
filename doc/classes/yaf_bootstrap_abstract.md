# Yaf_Bootstrap_Abstract

应用引导类的基类, 提供在分发请求之前定制应用的最早时机。它本身没有定义任何方法: 任何继承它的类 (按约定命名为 `Bootstrap`, 放在应用目录下) 中所有以 `_init` 开头的方法, 都会在 `Yaf_Application::bootstrap()` 时按定义顺序被依次调用, 每个方法接收一个 `Yaf_Dispatcher` 参数。典型用途是注册插件、自定义路由、替换视图引擎等。

```php
abstract class Yaf_Bootstrap_Abstract
```

