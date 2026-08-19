# Yaf_Config_Simple

PHP 数组配置适配器，`final` 类。直接包装一个 PHP 数组，**默认可写**；构造时传第二个参数 `TRUE` 可设为只读。数组值同样会被包装为子配置对象；`get()` 继承自 `Yaf_Config_Abstract`，只做顶层键精确匹配，不支持点号路径。

```php
final class Yaf_Config_Simple extends Yaf_Config_Abstract implements Countable, ArrayAccess, Iterator
```

继承: `Yaf_Config_Abstract` ← `Yaf_Config_Simple`

## 方法

### `Yaf_Config_Simple::__construct`

用 PHP 数组创建配置。第一个参数必须是数组，否则触发 TypeError。注意：第二个参数名为 `section` 但实际语义是**只读标志**（bool，默认 `FALSE` 可写）——本类没有节的概念。只读模式下直接持有传入的数组（零拷贝）；可写模式下复制一份，后续与原数组互不影响。

```php
public function __construct(array|string $config_file, ?string $section = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$config_file` | 配置数据数组。 |
| `$section` | 实际为只读标志：传 `TRUE` 表示只读，默认 `FALSE` 可写。 |

**返回值**: 构造 `Yaf_Config_Simple` 实例。

**示例**

```php
$config = new Yaf_Config_Simple(['db' => ['host' => '127.0.0.1']]);
var_dump($config->readonly()); // false，默认可写
$ro = new Yaf_Config_Simple(['a' => 1], true);
var_dump($ro->readonly());     // true，只读
```

### `Yaf_Config_Simple::set`

写入**顶层**配置项（已存在则覆盖）。只读模式下静默失败返回 `FALSE`（不产生告警，与 Ini 的行为不同）。不支持点号路径。

```php
public function set(string $name, $value): bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 键名。 |
| `$value` | 要写入的值。 |

**返回值**: 写入成功返回 `TRUE`；只读或写入失败返回 `FALSE`。

**示例**

```php
$config = new Yaf_Config_Simple(['name' => 'demo']);
$config->set('version', '1.0');   // 或 $config->version = '1.0'
print_r($config->toArray());
```

### `Yaf_Config_Simple::readonly`

返回配置是否只读，即构造时传入的只读标志。

```php
public function readonly(): bool
```

**返回值**: 只读返回 `TRUE`，可写返回 `FALSE`。

### `Yaf_Config_Simple::offsetUnset`

删除顶层配置项，支持 `unset($config['key'])`。仅字符串和整数偏移有效。**注意**：即使配置是只读模式也会真正删除，只读时仅额外产生一条 `E_WARNING`。

```php
public function offsetUnset($name): void
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 要删除的键（字符串或整数）。 |

**返回值**: 无返回值。

**示例**

```php
$config = new Yaf_Config_Simple(['foo' => 1, 'bar' => 2]);
unset($config['foo']);
print_r($config->toArray()); // ['bar' => 2]
```

### `Yaf_Config_Simple::__set`

属性式写入 `$config->name = ...`，内部调用 `set()`；只读模式下被忽略。

```php
public function __set(string $name, $value): void
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 键名。 |
| `$value` | 要写入的值。 |

**返回值**: 无返回值。

### `Yaf_Config_Simple::offsetSet`

`set()` 的别名，支持 `$config['key'] = value` 数组式写入；只读模式下静默失败。

```php
public function offsetSet($name, $value): void
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 键名。 |
| `$value` | 要写入的值。 |

**返回值**: 写入成功返回 `TRUE`，只读返回 `FALSE`。

## 继承的方法

- [Yaf_Config_Abstract](yaf_config_abstract.md): `get`, `count`, `toArray`, `rewind`, `current`, `key`, `next`, `valid`, `__isset`, `__get`, `offsetGet`, `offsetExists`

