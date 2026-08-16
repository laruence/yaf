# Yaf_View_Simple

**Yaf_View_Simple** 是 Yaf 内置的视图引擎，追求性能，不提供 Smarty 那样丰富的功能与复杂语法。模板就是普通 PHP 脚本：通过 `assign` 分配的模板变量在模板中可直接以同名变量（`$name`）使用；模板内 `$this` 指向视图引擎自身。开启 `yaf.use_namespace` 时也可用 `Yaf\View\Simple`。

```php
class Yaf_View_Simple implements Yaf_View_Interface
```

## 方法

### `Yaf_View_Simple::__construct`

构造 `Yaf_View_Simple` 实例。模板目录必须是绝对路径，否则触发 Yaf 类型错误。注意：源码 stub 中第一个参数名拼写为 `$tempalte_dir`（template 的笔误），属源码现状。`options` 参数当前未被使用。

```php
public final function __construct(string $tempalte_dir, ?array $options = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$tempalte_dir` | 模板基目录，必须为绝对路径 |
| `$options` | 选项数组（当前版本未使用） |

**返回值**: 无返回值（构造函数）

**示例**

```php
$view = new Yaf_View_Simple('/usr/local/www/views/');
```

### `Yaf_View_Simple::get`

获取已分配的模板变量值。不传参数或传空时返回全部模板变量。`__get` 是它的别名（`$view->name` 等价于 `$view->get("name")`）。

```php
public function get(?string $name = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 模板变量名；省略或传 NULL/空字符串时返回全部变量 |

**返回值**: 指定变量的值，不存在返回 NULL；不传变量名时返回全部模板变量的数组副本

**示例**

```php
$view->assign('name', 'value');
echo $view->get('name');     // value
print_r($view->get());       // 全部模板变量
```

### `Yaf_View_Simple::assign`

为视图引擎分配模板变量，在模板中可直接以同名变量使用。支持两种形式：`assign($name, $value)` 分配单个变量；`assign(array $vars)` 批量分配。变量名若不是合法 PHP 变量名（如整数、含特殊字符），模板中不能直接以 `$name` 访问，需通过 `$this->_tpl_vars` 访问。

```php
public function assign($name = NULL, $default = NULL): object|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 变量名字符串，或变量名=>值的关联数组（此时第二个参数须省略） |
| `$default` | 变量值；第一个参数为数组时须省略 |

**返回值**: 返回视图引擎自身（`Yaf_View_Simple`），支持链式调用；参数错误时返回 FALSE

**示例**

```php
$view->assign(['name' => 'value'])
     ->assign('foo', 'bar');
```

### `Yaf_View_Simple::render`

渲染模板文件并返回结果字符串（不输出）。模板路径为相对路径时与 `setScriptPath` 设置的基目录拼接；绝对路径则直接使用。模板文件不存在时触发 Yaf 错误。`tpl_vars` 中与已分配变量同名的会覆盖后者。

```php
public function render(string $tpl, ?array $tpl_vars = NULL): string|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$tpl` | 模板文件路径（相对模板基目录或绝对路径） |
| `$tpl_vars` | 本次渲染附加的模板变量，同名覆盖已 assign 的变量 |

**返回值**: 成功返回渲染结果字符串，失败返回 FALSE

**示例**

```php
$view = new Yaf_View_Simple('/usr/local/www/views/');
$view->assign('name', 'yaf');
echo $view->render('index/index.phtml');
```

### `Yaf_View_Simple::eval`

渲染模板字符串（而非模板文件）并返回结果，模板变量用法与 `render` 相同。空字符串返回 NULL。

```php
public function eval(string $tpl_str, ?array $vars = NULL): string|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$tpl_str` | 模板内容字符串（PHP 代码，无需包裹在 PHP 标签中） |
| `$vars` | 本次渲染附加的模板变量 |

**返回值**: 成功返回渲染结果字符串；模板字符串为空时返回 NULL

**示例**

```php
$view = new Yaf_View_Simple('/usr/local/www/views/');
echo $view->eval('<p>Hello, <?php echo $name; ?></p>', ['name' => 'yaf']);
// 输出: <p>Hello, yaf</p>
```

### `Yaf_View_Simple::display`

渲染模板文件并直接输出结果（不返回内容）。路径解析规则与 `render` 相同。

```php
public function display(string $tpl, ?array $tpl_vars = NULL): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$tpl` | 模板文件路径（相对模板基目录或绝对路径） |
| `$tpl_vars` | 本次渲染附加的模板变量 |

**返回值**: 成功返回 TRUE，失败返回 FALSE

**示例**

```php
$view = new Yaf_View_Simple('/usr/local/www/views/');
$view->display('index/index.phtml', ['name' => 'yaf']);
```

### `Yaf_View_Simple::assignRef`

分配模板变量，接口上与 `assign` 等价（当前源码实现为值拷贝，并无真正的引用绑定语义），返回视图引擎自身以支持链式调用。

```php
public function assignRef(?string $name, &$value): ?object
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 变量名 |
| `$value` | 变量值 |

**返回值**: 返回视图引擎自身（`Yaf_View_Simple`）

### `Yaf_View_Simple::clear`

清除已分配的模板变量。传入变量名则只清除该变量，不传则清除全部。

```php
public function clear(?string $name = NULL): ?object
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 要清除的变量名；省略或传 NULL 时清除全部模板变量 |

**返回值**: 返回视图引擎自身（`Yaf_View_Simple`）

**示例**

```php
$view->assign(['a' => 1, 'b' => 2]);
$view->clear('a'); // 只清除 a
$view->clear();    // 清除全部
```

### `Yaf_View_Simple::setScriptPath`

设置模板基目录，之后 `render`/`display` 相对路径模板都在此目录下查找。默认为 `APPLICATION_PATH . "/views"`，由 Dispatcher 设置。目录必须为绝对路径。

```php
public function setScriptPath(string $template_dir): object|bool|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$template_dir` | 模板基目录，必须为绝对路径 |

**返回值**: 成功返回视图引擎自身（`Yaf_View_Simple`，支持链式调用）；非绝对路径时返回 FALSE

**示例**

```php
$this->getView()->setScriptPath('/tmp/views/');
```

### `Yaf_View_Simple::getScriptPath`

获取当前模板基目录。

```php
public function getScriptPath(): ?string
```

**返回值**: 模板基目录字符串；未设置时返回 NULL

### `Yaf_View_Simple::__isset`

检测模板变量是否已分配，支撑对视图对象的 `isset()`/`empty()` 用法。

```php
public function __isset(string $name): bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 模板变量名 |

**返回值**: 已分配返回 TRUE，否则 FALSE

**示例**

```php
if (isset($view->name)) {
    echo $view->name;
}
```

### `Yaf_View_Simple::__set`

通过属性赋值的方式分配模板变量，等价于 `assign`，在模板中可直接以同名变量使用。

```php
public function __set(string $name, $value): void
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 变量名 |
| `$value` | 变量值 |

**返回值**: 无返回值

**示例**

```php
$view = $this->getView();
$view->name = 'value'; // 模板中可直接用 $name
```

### `Yaf_View_Simple::__get`

通过属性访问的方式获取模板变量值，是 `get` 的实现别名（`$view->name` 等价于 `$view->get("name")`）。

```php
public function __get(string $name): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 模板变量名 |

**返回值**: 变量值；未分配时返回 NULL

**示例**

```php
$this->initView();
echo $this->_view->name;
```

