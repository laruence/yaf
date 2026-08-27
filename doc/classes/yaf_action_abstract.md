# Yaf_Action_Abstract

**Yaf_Action_Abstract** 是 MVC 中控制器动作的基类，继承自 `Yaf_Controller_Abstract`。一般而言动作定义在控制器类中，但为了拆分较大的控制器、使代码更清晰，也可以把动作分离成独立的 `Yaf_Action_Abstract` 派生类，并在控制器的 `$actions` 属性中声明映射（如 `public $actions = ["index" => "actions/Index.php"]`），Yaf 会在路由到该动作时加载对应文件并调用其 `execute` 方法。开启 `yaf.use_namespace` 时也可用 `Yaf\Action_Abstract`。

```php
abstract class Yaf_Action_Abstract extends Yaf_Controller_Abstract
```

继承: `Yaf_Controller_Abstract` ← `Yaf_Action_Abstract`

## 属性

```php
protected mixed $_controller;
```

## 方法

### `Yaf_Action_Abstract::execute`

抽象方法，动作的执行体，由用户继承实现。Yaf 路由到该动作时调用。可定义参数以接收路由结果中的同名参数。

```php
public abstract function execute(): mixed
```

**返回值**: 返回 FALSE 可阻止自动渲染（由实现决定）

**示例**

```php
class IndexAction extends Yaf_Action_Abstract {
    public function execute($name = null) {
        echo $this->getControllerName(); // Index
        return false; // 关闭本动作的自动渲染
    }
}
```

### `Yaf_Action_Abstract::getController`

获取该动作所属的控制器实例。

```php
public function getController(): ?object
```

**返回值**: `Yaf_Controller_Abstract` 实例；未关联控制器时返回 NULL

**示例**

```php
class IndexAction extends Yaf_Action_Abstract {
    public function execute() {
        $this->getController()->getView()->assign('foo', 'bar');
    }
}
```

### `Yaf_Action_Abstract::getControllerName`

获取该动作所属的控制器名。

```php
public function getControllerName(): ?string
```

**返回值**: 控制器名字符串；未关联时返回 NULL

## 继承的方法

- [Yaf_Controller_Abstract](yaf_controller_abstract.md): `__construct`, `render`, `display`, `getRequest`, `getResponse`, `getView`, `getName`, `getModuleName`, `initView`, `setViewpath`, `getViewpath`, `forward`, `redirect`, `getInvokeArgs`, `getInvokeArg`

