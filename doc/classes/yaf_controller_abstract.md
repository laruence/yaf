# Yaf_Controller_Abstract

**Yaf_Controller_Abstract** 是 Yaf MVC 体系的核心部分。MVC（Model-View-Controller）是一种分离应用逻辑与表现逻辑的设计模式。用户通过继承该抽象类实现自己的控制器，控制器中定义的一个个动作（`xxxAction` 方法）是真正的执行体；动作也可以分离到独立的 `Yaf_Action_Abstract` 派生类中，通过控制器的 `$actions` 属性声明映射。与一般框架不同，Yaf 的动作可以定义参数，参数值来自路由结果中的同名参数（如 URL `/index/index/name/a` 对应 `indexAction($name)`）。注意这些参数来自用户请求，使用前必须做安全过滤；建议为参数定义默认值以避免缺参警告。控制器不可序列化、不可克隆。开启 `yaf.use_namespace` 时也可用 `Yaf\Controller_Abstract`。

```php
abstract class Yaf_Controller_Abstract
```

## 方法

### `Yaf_Controller_Abstract::__construct`

控制器构造函数，由 Yaf 在派发时调用。构造前必须已初始化 `Yaf_Application`，否则抛出异常。构造时会从 Dispatcher 关联 request/response/view，并从请求中取出控制器名与模块名，若子类定义了 `init()` 方法则随后调用它（相当于控制器级的初始化钩子）。

```php
public function __construct(): mixed
```

**返回值**: 无返回值（构造函数）

**示例**

```php
class IndexController extends Yaf_Controller_Abstract {
    public function init() {
        // 构造完成后由框架自动调用，可做控制器级初始化
        $this->getView()->assign('base', '/static');
    }
}
```

### `Yaf_Controller_Abstract::render`

渲染动作对应的视图模板并返回渲染结果字符串（不输出）。模板路径由动作名拼成 `<控制器名>/<动作名>.<视图后缀>`（控制器名小写，下划线转换为目录分隔符），在当前视图目录下查找。这是对 `Yaf_View_Interface::render` 的包装。

```php
protected function render(string $tpl, ?array $parameters = NULL): string|bool|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$tpl` | 要渲染的动作名（非模板文件名） |
| `$parameters` | 传递给视图引擎的模板变量，也可事先用 `Yaf_View_Interface::assign` 分配 |

**返回值**: 成功返回渲染结果字符串，失败返回 FALSE

**示例**

```php
class IndexController extends Yaf_Controller_Abstract {
    public function indexAction() {
        Yaf_Dispatcher::getInstance()->disableView(); // 关闭自动渲染
        echo $this->render('index', ['name' => 'value']);
        return false; // 阻止后续自动渲染
    }
}
```

### `Yaf_Controller_Abstract::display`

渲染动作对应的视图模板并直接输出结果（不返回内容）。模板路径的确定方式与 `render` 相同。这是对 `Yaf_View_Interface::display` 的包装。

```php
protected function display(string $tpl, ?array $parameters = NULL): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$tpl` | 要渲染的动作名（非模板文件名） |
| `$parameters` | 传递给视图引擎的模板变量，也可事先用 `Yaf_View_Interface::assign` 分配 |

**返回值**: 成功返回 TRUE，失败返回 FALSE

**示例**

```php
class IndexController extends Yaf_Controller_Abstract {
    public function indexAction() {
        Yaf_Dispatcher::getInstance()->disableView();
        $this->display('index', ['name' => 'value']);
        return false;
    }
}
```

### `Yaf_Controller_Abstract::getRequest`

获取当前请求实例。

```php
public function getRequest(): ?object
```

**返回值**: 当前的 `Yaf_Request_Abstract` 实例；在 Application 未初始化等特殊情况下返回 NULL

**示例**

```php
class IndexController extends Yaf_Controller_Abstract {
    public function indexAction() {
        $name = $this->getRequest()->getParam('name');
    }
}
```

### `Yaf_Controller_Abstract::getResponse`

获取当前响应实例。

```php
public function getResponse(): ?object
```

**返回值**: 当前的 `Yaf_Response_Abstract` 实例；未初始化时返回 NULL

**示例**

```php
class IndexController extends Yaf_Controller_Abstract {
    public function indexAction() {
        $this->getResponse()->setBody('hello', 'content');
    }
}
```

### `Yaf_Controller_Abstract::getView`

获取当前视图引擎实例。Yaf 采用延迟实例化策略，视图引擎在首次调用 `getView` 或 `initView` 之后才可用。

```php
public function getView(): ?object
```

**返回值**: `Yaf_View_Interface` 实例；未初始化时返回 NULL

**示例**

```php
class IndexController extends Yaf_Controller_Abstract {
    public function indexAction() {
        $view = $this->getView();
        $view->assign('foo', 'bar');
    }
}
```

### `Yaf_Controller_Abstract::getName`

获取当前控制器名（来自请求路由结果，保持原始大小写）。

```php
public function getName(): ?string
```

**返回值**: 控制器名字符串；未设置时返回 NULL

### `Yaf_Controller_Abstract::getModuleName`

获取当前控制器所属的模块名。

```php
public function getModuleName(): ?string
```

**返回值**: 模块名字符串；未设置时返回 NULL

**示例**

```php
class IndexController extends Yaf_Controller_Abstract {
    public function init() {
        if ($this->getModuleName() !== 'Index') {
            // 按模块做差异化初始化
        }
    }
}
```

### `Yaf_Controller_Abstract::initView`

初始化并返回视图引擎。由于 Yaf 延迟实例化视图引擎，在通过属性方式访问视图（如 `$this->_view`）前应先调用此方法确保其已实例化。注意：3.3.8 中该方法实际只是返回 Dispatcher 已持有的视图实例，`options` 参数被接受但不使用。

```php
public function initView(?array $options = NULL): ?object
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$options` | 视图引擎选项（当前版本未使用） |

**返回值**: `Yaf_View_Interface` 实例；视图未初始化时返回 NULL

**示例**

```php
class IndexController extends Yaf_Controller_Abstract {
    public function init() {
        $view = $this->initView();
        $view->assign('webroot', 'http://domain.com/');
    }
}
```

### `Yaf_Controller_Abstract::setViewpath`

更改当前控制器的视图模板目录，之后 `render`/`display` 将在新目录下查找模板。内部委托给视图引擎的 `setScriptPath`。

```php
public function setViewpath(string $view_directory): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$view_directory` | 视图模板目录，应为绝对路径 |

**返回值**: 成功返回 TRUE；视图引擎未初始化时返回 FALSE

**示例**

```php
class IndexController extends Yaf_Controller_Abstract {
    public function init() {
        $this->initView();
        $this->setViewpath('/usr/local/www/tpl/');
    }
}
```

### `Yaf_Controller_Abstract::getViewpath`

获取当前控制器的视图模板目录。

```php
public function getViewpath(): ?string
```

**返回值**: 模板目录字符串；视图引擎未初始化或未设置时返回空字符串

### `Yaf_Controller_Abstract::forward`

把当前请求转交给另一个动作处理。该方法只登记转发目的地（设置请求的 module/controller/action 及参数并重置 dispatched 标记），不会立即跳转；当前动作执行完毕后 Yaf 才会发起新一轮 dispatch。支持多种参数形式：`forward($action)`；`forward($controller, $action)` 或 `forward($action, array $invoke_args)`；`forward($module, $controller, $action)` 或 `forward($controller, $action, array $invoke_args)`；以及完整的四参数形式。

```php
public function forward($args1, $args2 = NULL, $args3 = NULL, $args4 = NULL): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$args1` | 1 参形式：目标动作名；2/3/4 参形式：目标模块名 |
| `$args2` | 目标控制器名，或 2 参形式下的调用参数数组 |
| `$args3` | 目标动作名，或 3 参形式下的调用参数数组 |
| `$args4` | 调用参数数组，可通过 `Yaf_Request_Abstract::getParam` 获取 |

**返回值**: 成功返回 TRUE，失败返回 FALSE

**示例**

```php
class IndexController extends Yaf_Controller_Abstract {
    public function indexAction() {
        if (!$user_logged_in) {
            $this->forward('login');           // 转发到当前控制器的 loginAction
            // $this->forward('User', 'login', ['from' => 'index']);
        }
    }
}
```

### `Yaf_Controller_Abstract::redirect`

重定向请求到新的 URL。内部通过响应对象发送 Location 头实现。

```php
public function redirect(string $url): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$url` | 要定向到的目标 URL |

**返回值**: 始终返回 TRUE

**示例**

```php
class IndexController extends Yaf_Controller_Abstract {
    public function indexAction() {
        if (!$user_logged_in) {
            $this->redirect('/login/');
        }
    }
}
```

### `Yaf_Controller_Abstract::getInvokeArgs`

获取通过 `forward` 传递来的全部调用参数。

```php
public function getInvokeArgs(): ?array
```

**返回值**: 调用参数关联数组的副本；无参数时返回 NULL

### `Yaf_Controller_Abstract::getInvokeArg`

获取通过 `forward` 传递来的单个调用参数。

```php
public function getInvokeArg(string $name): ?string
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 参数名 |

**返回值**: 参数值；不存在时返回 NULL

**示例**

```php
class IndexController extends Yaf_Controller_Abstract {
    public function dummyAction() {
        $from = $this->getInvokeArg('from'); // 来自 forward 时附加的参数
    }
}
```

