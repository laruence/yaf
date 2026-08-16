# Yaf_Loader

Yaf 的自动加载器, final 单例。它根据类名中的路径信息 (下划线/反斜线作目录分隔) 定位并加载类文件, 同时替代 require_once (不支持 require, 略快)。加载顺序: 以 Controller/Model/Plugin 结尾 (或开头, 取决于 `yaf.name_suffix`) 的类从应用目录的 controllers/models/plugins 下加载; 其余类先查已注册的本地命名空间, 命中则用本地类库目录 (默认 `application.directory/library`), 未命中则用全局类库目录 (`yaf.library`)。PHP 5.3+ 且开启 `yaf.use_namespace` 时也可写作 `Yaf\Loader`。

```php
final class Yaf_Loader
```

## 方法

### `Yaf_Loader::__construct`

私有构造方法, 禁止外部实例化。加载器是单例, 由扩展在首次需要时自动创建并注册为 SPL 自动加载函数, 请通过 `Yaf_Loader::getInstance()` 获取。

```php
private function __construct(): mixed
```

**返回值**: 不可用。

**示例**

```php
$loader = Yaf_Loader::getInstance();
```

### `Yaf_Loader::autoload`

加载一个类, 是注册给 SPL 的自动加载回调, 也可手动调用。类名中的 `_` 和 `\` 被视为目录分隔符; `yaf.lowcase_path` 开启时路径部分转小写。注意: 在默认的 `yaf.use_spl_autoload=0` 下, 即使类没找到也返回 TRUE (只触发警告), 以此截断后续自动加载器。

```php
public function autoload($class_name): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$class_name` | 要加载的类名, 例如 `Foo_Bar_Dummy` 对应类库目录下的 `Foo/Bar/Dummy.php`。 |

**返回值**: 默认 (use_spl_autoload=0) 总是返回 TRUE; 开启 `yaf.use_spl_autoload` 时, 加载成功返回 TRUE, 失败返回 FALSE 交由后续加载器处理。

**示例**

```php
$loader = Yaf_Loader::getInstance("/app/library", "/global/library");
// 本地类: /app/library/Foo/Bar.php
$loader->autoload("Foo_Bar");
// 未注册的类名走全局库: /global/library/Dummy.php
$loader->autoload("Dummy");
```

### `Yaf_Loader::getInstance`

获取加载器单例。注意: 传入路径参数并不会创建新实例, 而是修改已有单例的本地/全局类库路径。

```php
public static function getInstance($local_library_path, $global_library_path): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$local_library_path` | 本地类库目录; 缺省时保持当前值 (应用初始化后默认为 `application.directory/library`)。 |
| `$global_library_path` | 全局类库目录; 缺省时保持当前值 (来自 php.ini 的 `yaf.library`)。 |

**返回值**: Yaf_Loader 单例。

**示例**

```php
$loader = Yaf_Loader::getInstance();
var_dump($loader->getLibraryPath());

// 显式指定本地和全局类库目录
$loader = Yaf_Loader::getInstance("/app/library", "/global/library");
```

### `Yaf_Loader::registerLocalNamespace`

注册本地类名前缀 (命名空间), 使这些前缀的类从本地类库目录加载, 而不是全局类库目录。支持多级前缀 (如 `Foo_Bar`)。

```php
public function registerLocalNamespace($namespace, $path): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$namespace` | 字符串 (单个前缀) 或数组 (多个前缀; 数组带字符串键时, 键为前缀、值为该前缀专属的加载路径)。 |
| `$path` | 可选。为该前缀单独指定加载路径, 优先于本地类库目录。 |

**返回值**: 成功返回自身实例; 参数既不是字符串也不是数组时返回 FALSE。

**示例**

```php
$loader = Yaf_Loader::getInstance();
$loader->registerLocalNamespace("Baidu");
$loader->registerLocalNamespace(array("Sina", "Weibo"));
// 带专属路径: Vendor\Foo 从 /path/to/vendor 加载
$loader->registerLocalNamespace("Vendor", "/path/to/vendor");
```

### `Yaf_Loader::getLocalNamespace`

获取已注册的本地类前缀列表。

```php
public function getLocalNamespace(): mixed
```

**返回值**: 数组: 只注册了前缀的以数字下标列出, 指定了专属路径的以 "前缀 => 路径" 形式列出。

**示例**

```php
$loader = Yaf_Loader::getInstance();
$loader->registerLocalNamespace(array("Foo", "Bar"));
print_r($loader->getLocalNamespace()); // [0 => "Foo", 1 => "Bar"]
```

### `Yaf_Loader::clearLocalNamespace`

清除所有已注册的本地类前缀。

```php
public function clearLocalNamespace(): mixed
```

**返回值**: 成功返回 TRUE。

**示例**

```php
$loader = Yaf_Loader::getInstance();
$loader->registerLocalNamespace("Foo");
$loader->clearLocalNamespace();
var_dump($loader->isLocalName("Foo_Bar")); // false
```

### `Yaf_Loader::isLocalName`

判断一个类名是否属于已注册的本地前缀。按 `_` 或 `\` 逐段匹配已注册的前缀 (支持多级)。

```php
public function isLocalName($class_name): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$class_name` | 待判断的类名。 |

**返回值**: 是本地类返回 TRUE, 否则返回 FALSE。

**示例**

```php
$loader = Yaf_Loader::getInstance();
$loader->registerLocalNamespace("Foo");
var_dump($loader->isLocalName("Foo_Bar"));  // true
var_dump($loader->isLocalName("FooBar"));   // false
```

### `Yaf_Loader::getNamespacePath`

解析加载某个类时将使用的目录。

```php
public function getNamespacePath($class_name): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$class_name` | 类名。 |

**返回值**: 命中带专属路径的前缀时返回该路径; 命中普通本地前缀时返回本地类库目录; 未命中时返回全局类库目录 (未设置全局目录则回退到本地类库目录)。

**示例**

```php
$loader = Yaf_Loader::getInstance("/app/library", "/global/library");
$loader->registerLocalNamespace("Vendor", "/path/to/vendor");
echo $loader->getNamespacePath("Vendor_Foo"); // /path/to/vendor
echo $loader->getNamespacePath("Local_Foo");  // /app/library
echo $loader->getNamespacePath("Other_Foo");  // /global/library
```

### `Yaf_Loader::import`

静态方法, 加载一个 PHP 文件, 是 require_once 的高性能替代 (不做重复包含检查以外的事情)。文件已包含过时直接返回 TRUE, 不会重复执行。

```php
public static function import($file): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$file` | 文件路径。绝对路径直接使用; 相对路径以本地类库目录为基目录 (此时要求加载器已初始化, 即先创建过 Yaf_Application 或调用过 getInstance)。 |

**返回值**: 加载成功 (或已包含过) 返回 TRUE; 文件不存在、路径非法或加载器未初始化时返回 FALSE。

**示例**

```php
// 绝对路径
Yaf_Loader::import("/usr/local/foo.php");
// 相对路径, 在本地类库目录 (默认 application.directory/library) 下加载
Yaf_Loader::import("functions.php");
```

### `Yaf_Loader::setLibraryPath`

设置类库目录。

```php
public function setLibraryPath($library_path, $is_global): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$library_path` | 类库目录路径。 |
| `$is_global` | TRUE 时设置全局类库目录, FALSE (默认) 时设置本地类库目录。 |

**返回值**: 返回自身实例, 支持链式调用。

**示例**

```php
$loader = Yaf_Loader::getInstance();
$loader->setLibraryPath("/app/library")
       ->setLibraryPath("/global/library", true);
```

### `Yaf_Loader::getLibraryPath`

获取类库目录。

```php
public function getLibraryPath($is_global): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$is_global` | TRUE 时返回全局类库目录, FALSE (默认) 时返回本地类库目录。 |

**返回值**: 目录字符串; 全局目录未设置且查询全局时返回空字符串。

**示例**

```php
$loader = Yaf_Loader::getInstance();
echo $loader->getLibraryPath();       // 本地类库目录
echo $loader->getLibraryPath(true);   // 全局类库目录 (yaf.library)
```

### `Yaf_Loader::registerNamespace`

`registerLocalNamespace()` 的别名。

```php
public function registerNamespace($namespace, $path): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$namespace` | 字符串 (单个前缀) 或数组 (多个前缀; 数组带字符串键时, 键为前缀、值为该前缀专属的加载路径)。 |
| `$path` | 可选。为该前缀单独指定加载路径。 |

**返回值**: 成功返回自身实例; 参数类型非法时返回 FALSE。

**示例**

```php
$loader = Yaf_Loader::getInstance();
$loader->registerNamespace("Foo")->registerNamespace(array("Bar", "Baz"));
```

### `Yaf_Loader::getNamespaces`

`getLocalNamespace()` 的别名。

```php
public function getNamespaces(): mixed
```

**返回值**: 已注册前缀的数组。

**示例**

```php
print_r(Yaf_Loader::getInstance()->getNamespaces());
```

