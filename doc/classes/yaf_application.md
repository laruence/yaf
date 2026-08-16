# Yaf_Application

Yaf 应用的入口与运行环境容器。final 类, 全进程单例: 构造时解析配置、初始化 `Yaf_Dispatcher` 与请求对象, 并把自身注册为全局实例 (可用 `Yaf_Application::app()` 取回)。它持有应用目录、环境名、模块列表、配置对象等, `run()` 驱动整个分发流程。一个进程只能初始化一次, 重复初始化会抛出异常。

```php
final class Yaf_Application
```

## 方法

### `Yaf_Application::__construct`

初始化 Yaf_Application。从配置中读取 `application.directory` (必需) 及 bootstrap、library、modules、dispatcher 等可选项, 创建分发器和请求对象。同一进程只允许初始化一次, 重复初始化或配置缺少 directory 时抛出异常。

```php
public function __construct(array|string $config, ?string $environ = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$config` | 关联数组、ini 配置文件路径字符串, 或 Yaf_Config_Abstract 实例。ini 文件时按 $environ 指定的配置节读取。 |
| `$environ` | 环境名 (配置节名), 缺省使用 php.ini 的 `yaf.environ` (默认 "product")。 |

**返回值**: 无返回值; 初始化失败时抛出异常。

**示例**

```php
$config = array(
    "application" => array(
        "directory" => realpath(__DIR__) . "/application",
    ),
);

$app = new Yaf_Application($config);
```

### `Yaf_Application::run`

运行应用: 驱动分发器完成路由、插件钩子和 Controller/Action 分发循环。同一实例重复调用会触发错误并返回 FALSE。分发循环的最大迭代次数由 `yaf.forward_limit` 控制 (默认 5); 注意 3.3.8 起非正值 (含 0) 会被钳制回默认值 5, 旧版把 0 当作"不限制"会导致死循环, 该语义已废弃。

```php
public function run(): object|false
```

**返回值**: 成功返回 Yaf_Response_Abstract 实例 (默认响应已自动输出); 分发失败返回 FALSE。

**示例**

```php
$app = new Yaf_Application("conf/application.ini");
$app->bootstrap()->run();
```

### `Yaf_Application::execute`

在应用的环境下执行一个回调, 主要用于 CLI 脚本: 借用 Yaf 的自动加载、配置等外围环境, 而不走路由分发。回调之后的额外实参会原样透传。

```php
public function execute($callback): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$callback` | 任意合法的 PHP 回调 (函数名、array($obj, "method")、闭包等), 之后可跟任意多个传给回调的参数。 |

**返回值**: 返回回调的返回值; 回调执行失败或未产生返回值时返回 FALSE。

**示例**

```php
$app = new Yaf_Application("conf/application.ini");
$app->execute("main", __FILE__);

function main($argc) {
    echo "running in cli with {$argc}\n";
}
```

### `Yaf_Application::app`

获取当前进程中已初始化的 Yaf_Application 单例。`Yaf_Application::getInstance()` 是它的别名。

```php
public static function app(): ?object
```

**返回值**: 返回全局 Yaf_Application 实例; 尚未初始化时返回 NULL。

**示例**

```php
$app = new Yaf_Application($config);
assert($app === Yaf_Application::app());
```

### `Yaf_Application::environ`

获取当前应用使用的配置环境名, 即构造时传入 (或取自 `yaf.environ`) 的配置节名。

```php
public function environ(): ?string
```

**返回值**: 环境名字符串, 默认 "product"。

**示例**

```php
$app = new Yaf_Application("conf/application.ini", "develop");
echo $app->environ(); // develop
```

### `Yaf_Application::bootstrap`

加载 Bootstrap 类 (默认 `application.directory/Bootstrap.php`, 可用配置项 `application.bootstrap` 覆盖路径) 并按定义顺序依次调用其中所有以 `_init` 开头的方法, 每个方法接收 `Yaf_Dispatcher` 作为参数。通常在 `run()` 之前调用, 用于注册插件、路由、视图引擎等定制。

```php
public function bootstrap(): object|false|null
```

**返回值**: 成功返回自身实例 (支持链式调用, 如 `$app->bootstrap()->run()`); 找不到 Bootstrap 类或文件时返回 FALSE 并触发错误。

**示例**

```php
class Bootstrap extends Yaf_Bootstrap_Abstract {
    public function _initPlugin(Yaf_Dispatcher $dispatcher) {
        $dispatcher->registerPlugin(new UserPlugin());
    }
}

$app = new Yaf_Application($config);
$app->bootstrap()->run();
```

### `Yaf_Application::getConfig`

获取应用启动时加载的配置对象。

```php
public function getConfig(): ?object
```

**返回值**: Yaf_Config_Abstract 实例 (数组配置为 Yaf_Config_Simple, ini 文件为 Yaf_Config_Ini); 应用未成功初始化时返回 NULL。

**示例**

```php
$app = new Yaf_Application("conf/application.ini");
print_r($app->getConfig()->get("application.directory"));
```

### `Yaf_Application::getModules`

获取配置中 `application.modules` 声明的模块列表。

```php
public function getModules(): ?array
```

**返回值**: 模块名数组; 配置中未声明 modules 时返回 NULL。

**示例**

```php
$config = array("application" => array(
    "directory" => "/path/to/app",
    "modules"   => "Index,Admin",
));
$app = new Yaf_Application($config);
print_r($app->getModules()); // ["Index", "Admin"]
```

### `Yaf_Application::getDispatcher`

获取应用持有的分发器实例, 与 `Yaf_Dispatcher::getInstance()` 返回同一对象。

```php
public function getDispatcher(): ?object
```

**返回值**: Yaf_Dispatcher 实例; 应用未成功初始化时返回 NULL。

**示例**

```php
$app = new Yaf_Application($config);
$app->getDispatcher()->registerPlugin(new UserPlugin());
```

### `Yaf_Application::setAppDirectory`

修改应用目录 (APPLICATION_PATH), 之后的控制器、视图都从新目录加载, 但自动加载的类库路径不变。3.3.8 起 `Yaf_Dispatcher::setAppDirectory` 已移除, 请改用本方法。

```php
public function setAppDirectory(string $directory): object|false|null
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$directory` | 新的应用目录, 必须是绝对路径。 |

**返回值**: 成功返回自身实例; 路径为空或非绝对路径时返回 FALSE。

**示例**

```php
$app->setAppDirectory("/usr/local/new/application")->run();
```

### `Yaf_Application::getAppDirectory`

获取当前的应用目录。

```php
public function getAppDirectory(): ?string
```

**返回值**: 应用目录字符串, 即配置中 `application.directory` 的值。

**示例**

```php
$app = new Yaf_Application($config);
echo $app->getAppDirectory();
```

### `Yaf_Application::getLastErrorNo`

获取应用最近一次错误的错误码 (对应 YAF_ERR_* 常量)。

```php
public function getLastErrorNo(): ?int
```

**返回值**: 错误码整数, 无错误时为 0。

**示例**

```php
$app->run();
if ($app->getLastErrorNo()) {
    error_log($app->getLastErrorMsg());
}
```

### `Yaf_Application::getLastErrorMsg`

获取应用最近一次错误的错误信息。

```php
public function getLastErrorMsg(): ?string
```

**返回值**: 错误信息字符串, 无错误时为空字符串。

**示例**

```php
if ($app->getLastErrorNo() !== 0) {
    trigger_error($app->getLastErrorMsg());
}
```

### `Yaf_Application::clearLastError`

清空最近一次的错误码和错误信息。

```php
public function clearLastError(): ?object
```

**返回值**: 返回自身实例, 支持链式调用。

**示例**

```php
$app->clearLastError();
assert($app->getLastErrorNo() === 0);
```

### `Yaf_Application::getInstance`

`Yaf_Application::app()` 的别名, 获取全局应用单例。

```php
public static function getInstance(): ?object
```

**返回值**: 返回全局 Yaf_Application 实例; 尚未初始化时返回 NULL。

**示例**

```php
$app = Yaf_Application::getInstance();
if ($app !== NULL) {
    var_dump($app->environ());
}
```

