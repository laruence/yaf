# Yaf_Session

Yaf 对 PHP session 的包装，abstract final 类（构造器私有，不可直接实例化，通过 getInstance() 获取单例），实现了 `Iterator`、`ArrayAccess`、`Countable`，因此除了 `get/set/has/del` 之外，还支持数组下标、属性访问和 foreach 遍历。

所有读写都直接作用于 `$_SESSION`：实现上每次访问都从符号表实时解析 `$_SESSION` 数组，不缓存指针。因此一旦用户代码 `unset($_SESSION)`（或 session 被销毁），已持有的实例会降级为不可用状态：`get()` 返回 NULL、`has()` 返回 FALSE、`set()/del()` 返回 FALSE、`count()` 返回 NULL、`clear()` 返回 FALSE、foreach 无内容。

`getInstance()` 首次创建实例时会自动调用 `session_start()`；若启动失败（`$_SESSION` 不可用）会产生一条 E_WARNING。

```php
abstract class Yaf_Session implements Iterator, ArrayAccess, Countable
```

## 方法

### `Yaf_Session::__construct`

私有构造器，禁止直接 new，只能通过 getInstance 获取单例。

```php
private function __construct(): mixed
```

**返回值**: 无

### `Yaf_Session::getInstance`

获取 Session 单例。首次调用时创建实例并自动启动 session（等价于 session_start）；同一请求内返回同一对象。注意：若之后 $_SESSION 被 unset，该实例不会重新初始化，所有操作将返回失败值。

```php
public static function getInstance(): ?object
```

**返回值**: 返回 Yaf_Session 实例。

**示例**

```php
$session = Yaf_Session::getInstance();
$session->set('name', 'Laruence');
echo $_SESSION['name']; // 与 $_SESSION 完全互通
```

### `Yaf_Session::start`

启动 session。getInstance 首次创建实例时已自动启动，此方法是幂等的（已启动则直接跳过），保留是为了兼容旧用法和链式调用。

```php
public function start(): object
```

**返回值**: 返回 $this，支持链式调用。

**示例**

```php
$session = Yaf_Session::getInstance()->start();
```

### `Yaf_Session::get`

读取 session 变量。name 可省略：不传参数时返回整个 $_SESSION 数组。

```php
public function get(?string $name = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 变量名；省略时返回全部 session 数据 |

**返回值**: 变量存在返回其值；不存在或 $_SESSION 不可用返回 NULL；不传 name 返回整个 session 数组。

**示例**

```php
$session = Yaf_Session::getInstance();
$session->set('age', 28);
var_dump($session->get('age')); // 28
var_dump(count($session->get())); // 全部条目
```

### `Yaf_Session::has`

检查 session 变量是否存在。

```php
public function has(string $name): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 变量名 |

**返回值**: 存在返回 TRUE；不存在或 $_SESSION 不可用返回 FALSE。

**示例**

```php
if (Yaf_Session::getInstance()->has('uid')) {
    // 已登录
}
```

### `Yaf_Session::set`

写入 session 变量，同名变量直接覆盖。

```php
public function set(string $name, $value): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 变量名 |
| `$value` | 变量值，任意类型 |

**返回值**: 写入成功返回 TRUE；$_SESSION 不可用返回 FALSE。

**示例**

```php
$session = Yaf_Session::getInstance();
$session->set('uid', 1024);
```

### `Yaf_Session::del`

删除指定 session 变量。

```php
public function del(string $name): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 变量名 |

**返回值**: 删除成功返回 TRUE；变量不存在或 $_SESSION 不可用返回 FALSE。

**示例**

```php
$session = Yaf_Session::getInstance();
$session->set('captcha', 'abcd');
$session->del('captcha'); // 验证后删除
```

### `Yaf_Session::count`

返回 session 变量的个数（Countable 接口实现，可直接 count($session)）。

```php
public function count(): int
```

**返回值**: session 变量个数；$_SESSION 不可用时返回 NULL。

**示例**

```php
$session = Yaf_Session::getInstance();
$session->set('a', 1)->set('b', 2);
var_dump(count($session)); // int(2)
```

### `Yaf_Session::clear`

清空所有 session 变量（只清空 $_SESSION 内容，不销毁 session 本身）。

```php
public function clear(): object|false|null
```

**返回值**: 成功返回 $this 支持链式；$_SESSION 不可用返回 FALSE。

**示例**

```php
Yaf_Session::getInstance()->clear();
```

### `Yaf_Session::offsetGet`

ArrayAccess 接口实现，等价于 get()，支撑 $session["name"] 读语法。

```php
public function offsetGet($name): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 变量名 |

**返回值**: 同 get()：存在返回值，不存在或 $_SESSION 不可用返回 NULL。

**示例**

```php
$session = Yaf_Session::getInstance();
$session['company'] = 'Realsee';
echo $session['company'];
```

### `Yaf_Session::offsetSet`

ArrayAccess 接口实现，等价于 set()，支撑 $session["name"] = $value 写语法。

```php
public function offsetSet($name, $value): void
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 变量名 |
| `$value` | 变量值 |

**返回值**: 同 set()：成功 TRUE，$_SESSION 不可用 FALSE。

### `Yaf_Session::offsetExists`

ArrayAccess 接口实现，等价于 has()，支撑 isset($session["name"])。

```php
public function offsetExists($name): bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 变量名 |

**返回值**: 同 has()。

### `Yaf_Session::offsetUnSet`

ArrayAccess 接口实现，等价于 del()，支撑 unset($session["name"])。

```php
public function offsetUnSet($name): void
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 变量名 |

**返回值**: 同 del()：删除成功 TRUE，否则 FALSE。

### `Yaf_Session::__get`

魔术方法，等价于 get()，支撑 $session->name 属性读语法。

```php
public function __get(string $name): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 变量名 |

**返回值**: 同 get()。

**示例**

```php
$session = Yaf_Session::getInstance();
$session->age = 28;
echo $session->age; // 28
```

### `Yaf_Session::__isset`

魔术方法，等价于 has()，支撑 isset($session->name)。

```php
public function __isset(string $name): bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 变量名 |

**返回值**: 同 has()。

### `Yaf_Session::__set`

魔术方法，等价于 set()，支撑 $session->name = $value 属性写语法。

```php
public function __set(string $name, $value): void
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 变量名 |
| `$value` | 变量值 |

**返回值**: 同 set()。

### `Yaf_Session::__unset`

魔术方法，等价于 del()，支撑 unset($session->name)。

```php
public function __unset(string $name): void
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 变量名 |

**返回值**: 同 del()：删除成功 TRUE，否则 FALSE。

## 继承的方法

- `Iterator`: `current`, `next`, `key`, `valid`, `rewind`

