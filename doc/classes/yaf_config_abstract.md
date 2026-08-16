# Yaf_Config_Abstract

配置适配器基类，为应用提供对象化的配置数据访问。实现了 `Iterator`、`ArrayAccess`、`Countable`，因此可以直接 `foreach`、`count()`、按数组下标访问。有两个实现：`Yaf_Config_Ini`（INI 文件，只读）和 `Yaf_Config_Simple`（PHP 数组，默认可写）。读取到的数组值会被自动包装为同类型的子配置对象（继承父对象的只读状态）。抽象类不可实例化，且不可序列化、不可克隆。

```php
abstract class Yaf_Config_Abstract implements Iterator, ArrayAccess, Countable
```

## 方法

### `Yaf_Config_Abstract::get`

按键名读取配置项。基类实现只做顶层键的精确匹配，**不解析点号路径**（`Yaf_Config_Ini` 重写了本方法才支持 `a.b.c`）。值为数组时自动包装为子配置对象；键不存在返回 `NULL`。

```php
public function get(?string $name = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 配置键名。可省略（或传 `NULL`），此时返回配置对象自身。 |

**返回值**: 配置值；数组值返回子配置对象；键不存在返回 `NULL`；省略参数返回 `$this`。

**示例**

```php
$config = new Yaf_Config_Simple(['db' => ['host' => '127.0.0.1'], 'name' => 'demo']);
var_dump($config->get('name')); // string(4) "demo"
$db = $config->get('db');       // 数组 → 子配置对象
var_dump($db instanceof Yaf_Config_Simple); // true
```

### `Yaf_Config_Abstract::count`

返回顶层配置项数量。实现了 `Countable`，可直接对配置对象使用 `count()`。

```php
public function count(): int
```

**返回值**: 顶层配置项个数（int）。

**示例**

```php
$config = new Yaf_Config_Simple(['a' => 1, 'b' => 2]);
var_dump(count($config)); // int(2)
```

### `Yaf_Config_Abstract::toArray`

导出配置数据为原生 PHP 数组。返回的是原始数据，嵌套数组不会包装成配置对象。

```php
public function toArray(): array
```

**返回值**: 当前（顶层）配置的数组表示。

**示例**

```php
$config = new Yaf_Config_Ini('app.ini', 'product');
$arr = $config->toArray();
var_dump($arr['name']); // 原生数组，值不再是配置对象
```

### `Yaf_Config_Abstract::offsetUnset`

基类实现一律拒绝删除：产生 `E_WARNING`（config is readonly）并返回 `FALSE`。`Yaf_Config_Simple` 重写后可真正删除。

```php
public function offsetUnset($name): void
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 要删除的键（被忽略，不做实际操作）。 |

**返回值**: `FALSE`。

**示例**

```php
$config = new Yaf_Config_Ini('app.ini');
unset($config['foo']); // E_WARNING: config is readonly
```

### `Yaf_Config_Abstract::rewind`

将内部迭代指针重置回第一个元素（`Iterator` 实现）。`foreach` 开始遍历时会自动调用。

```php
public function rewind(): void
```

**返回值**: 无返回值。

**示例**

```php
$config = new Yaf_Config_Ini('app.ini', 'product');
foreach ($config as $key => $value) {
    // 遍历结束时指针停在末尾
}
$config->rewind(); // 重置后可再次手动遍历
```

### `Yaf_Config_Abstract::current`

返回当前迭代指针指向的值（`Iterator` 实现）。值为数组时包装为同类型子配置对象；指针越界或配置为空返回 `FALSE`。

```php
public function current(): mixed
```

**返回值**: 当前配置值（标量或子配置对象），无元素时 `FALSE`。

### `Yaf_Config_Abstract::key`

返回当前迭代指针指向的键（`Iterator` 实现）。

```php
public function key(): string|int|bool|null
```

**返回值**: 字符串键或整数索引；指针越界时返回 `FALSE`。

### `Yaf_Config_Abstract::next`

将内部迭代指针前移一位（`Iterator` 实现）。

```php
public function next(): void
```

**返回值**: 无返回值。

### `Yaf_Config_Abstract::valid`

检查当前指针位置是否仍有元素（`Iterator` 实现），供 `foreach` 判断是否继续。

```php
public function valid(): bool
```

**返回值**: 还有元素返回 `TRUE`，否则 `FALSE`。

### `Yaf_Config_Abstract::__isset`

检查顶层键是否存在，支持 `isset($config->name)`。只做顶层精确匹配，不支持点号路径。

```php
public function __isset(string $name): bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 要检查的键名。 |

**返回值**: 存在返回 `TRUE`，否则 `FALSE`。

**示例**

```php
$config = new Yaf_Config_Simple(['name' => 'demo']);
var_dump(isset($config->name));    // true
var_dump(isset($config->dummy));   // false
```

### `Yaf_Config_Abstract::__get`

`get()` 的别名，支持属性式读取 `$config->name`。

```php
public function __get(string $name): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 配置键名。 |

**返回值**: 与 `get()` 相同。

**示例**

```php
$config = new Yaf_Config_Simple(['name' => 'demo']);
echo $config->name; // 等价于 $config->get('name')
```

### `Yaf_Config_Abstract::offsetGet`

`get()` 的别名，支持数组式读取 `$config['name']`。在 `Yaf_Config_Ini` 上同样支持点号路径。

```php
public function offsetGet($name): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 配置键名。 |

**返回值**: 与 `get()` 相同。

### `Yaf_Config_Abstract::offsetExists`

`__isset()` 的别名，支持 `isset($config['name'])`。

```php
public function offsetExists($name): bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 要检查的键名。 |

**返回值**: 存在返回 `TRUE`，否则 `FALSE`。

### `Yaf_Config_Abstract::offsetSet`

抽象方法，由子类实现写入。`Yaf_Config_Ini` 永远失败并告警；`Yaf_Config_Simple` 在非只读时可写。

```php
public abstract function offsetSet($name, $value): void
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 键名。 |
| `$value` | 要写入的值。 |

**返回值**: 由子类决定。

### `Yaf_Config_Abstract::set`

抽象方法，由子类实现写入配置项。写入契约见各子类。

```php
public abstract function set(string $name, $value): bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 键名。 |
| `$value` | 要写入的值。 |

**返回值**: 由子类决定。

### `Yaf_Config_Abstract::readonly`

抽象方法，返回配置是否只读。`Yaf_Config_Ini` 恒为 `TRUE`；`Yaf_Config_Simple` 取决于构造参数。

```php
public abstract function readonly(): bool
```

**返回值**: 由子类决定。

