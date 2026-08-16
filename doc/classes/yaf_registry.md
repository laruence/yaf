# Yaf_Registry

对象注册表（全局容器），用于在整个请求生命周期内跨组件共享对象和值。final 类，构造器私有、不可克隆、不可序列化；内部以单例保存条目，`get/has/set/del` 均为静态方法，直接操作同一个请求级实例。

注册表实例在首次访问时惰性创建，存活于当前请求（存储在 Yaf 模块全局量中），不跨请求持久化。典型用法是在 Bootstrap 中 `set()` 共享配置/依赖，在 Controller、Plugin 中 `get()` 取用。

```php
final class Yaf_Registry
```

## 方法

### `Yaf_Registry::__construct`

私有构造器，禁止直接 new。只能通过静态方法或 getInstance 使用单例。

```php
private function __construct(): mixed
```

**返回值**: 无

### `Yaf_Registry::get`

读取注册表中指定名称的条目。

```php
public static function get(string $name): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 条目名称 |

**返回值**: 条目存在时返回存储的值（可为任意类型）；不存在时返回 NULL。

**示例**

```php
// Bootstrap 中存入
Yaf_Registry::set('config', $app->getConfig());
// 任意位置取出
$config = Yaf_Registry::get('config');
var_dump(Yaf_Registry::get('missing')); // NULL
```

### `Yaf_Registry::has`

检查注册表中是否存在指定名称的条目。

```php
public static function has(string $name): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 条目名称 |

**返回值**: 存在返回 TRUE，否则返回 FALSE。

**示例**

```php
if (Yaf_Registry::has('config')) {
    $config = Yaf_Registry::get('config');
}
```

### `Yaf_Registry::set`

写入或覆盖注册表条目。同名条目重复 set 会直接覆盖旧值。

```php
public static function set(string $name, $value): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 条目名称 |
| `$value` | 要存储的值，任意类型 |

**返回值**: 写入成功返回 TRUE，失败返回 FALSE。

**示例**

```php
Yaf_Registry::set('db', new PDO($dsn, $user, $pass));
```

### `Yaf_Registry::del`

删除注册表中的指定条目。条目不存在时也不会报错。

```php
public static function del(string $name): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 条目名称 |

**返回值**: 始终返回 TRUE。

**示例**

```php
Yaf_Registry::set('tmp', 1);
Yaf_Registry::del('tmp');
var_dump(Yaf_Registry::has('tmp')); // false
```

