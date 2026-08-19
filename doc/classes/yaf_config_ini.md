# Yaf_Config_Ini

INI 文件配置适配器，`final` 类，**恒为只读**。特性：

- **section 继承**：`[child : parent]` 语法，子节深合并（递归合并嵌套数组）父节配置，子节同名键覆盖；支持多父节 `[a : b : c]`，自右向左合并，左侧父节优先，最后是节自身定义的键。
- **点号键自动嵌套**：节内 `a.b.c=1` 解析为嵌套数组 `['a']['b']['c']`。
- **变量插值**：值中 `${CONST}` 引用 PHP 常量、`${ini.directive}` 引用 php.ini 指令。

读取数组值返回只读的子 `Yaf_Config_Ini` 对象。迭代、计数、数组访问能力继承自 `Yaf_Config_Abstract`。

```php
final class Yaf_Config_Ini extends Yaf_Config_Abstract implements Countable, ArrayAccess, Iterator
```

继承: `Yaf_Config_Abstract` ← `Yaf_Config_Ini`

## 方法

### `Yaf_Config_Ini::__construct`

解析 INI 配置文件。文件不存在、不是常规文件或解析失败时抛出 `Yaf_Exception`。指定 `section` 时只加载该节（节支持 `:` 继承语法，父节配置会被深合并进来），节不存在则抛出 `Yaf_Exception`（"There is no section ..."）；省略时整个文件的所有节作为顶层键加载。

```php
public function __construct(array|string $config_file, ?string $section = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$config_file` | INI 文件路径。 |
| `$section` | 可选。要加载的节名；省略或传 `NULL` 时加载全部节。 |

**返回值**: 构造 `Yaf_Config_Ini` 实例，失败抛异常。

**示例**

```php
// app.ini:
//   [common]
//   db.host = "127.0.0.1"
//   [product : common]
//   name = "demo"
$config = new Yaf_Config_Ini('app.ini', 'product');
echo $config->get('name');     // demo
echo $config->get('db.host');  // 127.0.0.1（自 common 继承）
```

### `Yaf_Config_Ini::get`

读取配置项，**支持点号路径**如 `routes.rewrite.match`：按 `.` 逐层深入，中间层不是数组时直接返回该中间值；最终键不存在返回 `NULL`。值为数组时包装为只读的子 `Yaf_Config_Ini` 对象。省略参数返回对象自身。

```php
public function get(?string $name = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 配置键名，可含 `.` 分隔的层级路径。可省略，此时返回对象自身。 |

**返回值**: 配置值；数组返回子配置对象；不存在返回 `NULL`。

**示例**

```php
$config = new Yaf_Config_Ini('app.ini', 'product');
echo $config->get('routes.rewrite.match'); // 点号路径
echo $config['routes.rewrite.match'];      // offsetGet 是 get 的别名
$routes = $config->get('routes');          // 数组 → 只读子配置对象
var_dump($config->get('not.exists'));      // NULL
```

### `Yaf_Config_Ini::set`

INI 配置恒为只读，写入永远失败：产生 `E_WARNING`（config is readonly）。

```php
public function set(string $name, $value): bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 键名。 |
| `$value` | 要写入的值。 |

**返回值**: `FALSE`。

**示例**

```php
$config = new Yaf_Config_Ini('app.ini');
var_dump($config->set('name', 'new')); // E_WARNING + bool(false)
```

### `Yaf_Config_Ini::readonly`

INI 配置恒为只读，本方法恒返回 `TRUE`。

```php
public function readonly(): bool
```

**返回值**: `TRUE`。

### `Yaf_Config_Ini::__set`

INI 配置恒为只读，属性式赋值 `$config->name = ...` 被忽略并产生 `E_WARNING`（config is readonly）。

```php
public function __set($name, $value): void
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 键名。 |
| `$value` | 要写入的值。 |

**返回值**: 无返回值。

### `Yaf_Config_Ini::offsetGet`

`get()` 的别名，数组式访问 `$config['name']`，同样支持点号路径。

```php
public function offsetGet($name): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 配置键名，可含 `.` 分隔的层级路径。 |

**返回值**: 与 `get()` 相同。

### `Yaf_Config_Ini::offsetSet`

`set()` 的别名。INI 配置只读，`$config['name'] = ...` 永远失败并产生 `E_WARNING`。

```php
public function offsetSet($name, $value): void
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 键名。 |
| `$value` | 要写入的值。 |

**返回值**: `FALSE`。

## 继承的方法

- [Yaf_Config_Abstract](yaf_config_abstract.md): `count`, `toArray`, `offsetUnset`, `rewind`, `current`, `key`, `next`, `valid`, `__isset`, `__get`, `offsetExists`

