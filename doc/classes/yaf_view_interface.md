# Yaf_View_Interface

**Yaf_View_Interface** 是 Yaf 的视图引擎接口，定义了接入 Yaf 的视图引擎需要实现的方法与功能。实现该接口即可接入自定义模板引擎（如 Smarty），Yaf 内置的实现是 `Yaf_View_Simple`。开启 `yaf.use_namespace` 时也可用 `Yaf\View_Interface`。

```php
interface Yaf_View_Interface
```

## 方法

### `Yaf_View_Interface::assign`

视图引擎契约：分配模板变量。分配的变量在模板渲染时可用。

```php
public abstract function assign(string $name, $value = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 变量名 |
| `$value` | 变量值 |

**返回值**: 实现自定，约定成功返回视图引擎自身（便于链式调用），失败返回 FALSE

### `Yaf_View_Interface::display`

视图引擎契约：渲染模板脚本并直接输出结果。

```php
public abstract function display(string $tpl, ?array $tpl_vars = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$tpl` | 模板脚本路径 |
| `$tpl_vars` | 本次渲染附加的模板变量 |

**返回值**: 实现自定，约定成功返回 TRUE，失败返回 FALSE

### `Yaf_View_Interface::render`

视图引擎契约：渲染模板脚本并返回结果字符串，不输出。Yaf 的自动渲染机制依赖此方法返回渲染内容。

```php
public abstract function render(string $tpl, ?array $tpl_vars = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$tpl` | 模板脚本路径 |
| `$tpl_vars` | 本次渲染附加的模板变量 |

**返回值**: 实现自定，约定成功返回渲染结果字符串，失败返回 FALSE

### `Yaf_View_Interface::setScriptPath`

视图引擎契约：设置模板脚本的基目录。Yaf 渲染相对路径模板时会以此目录为基准拼接。

```php
public abstract function setScriptPath(string $template_dir): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$template_dir` | 模板基目录，应为绝对路径 |

**返回值**: 实现自定，约定成功返回 TRUE（`Yaf_View_Simple` 返回自身），失败返回 FALSE

### `Yaf_View_Interface::getScriptPath`

视图引擎契约：获取当前模板脚本的基目录。

```php
public abstract function getScriptPath(): mixed
```

**返回值**: 模板基目录字符串

