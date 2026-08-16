# Yaf_Response_Abstract

响应对象的抽象基类，负责管理响应 body 和输出。body 以命名段（key）累积保存，默认段名为 "content"（常量 DEFAULT_BODY 的值）。Yaf 框架根据 SAPI 自动选择具体子类：HTTP 环境为 Yaf_Response_Http（额外支持响应头、状态码、重定向），CLI 环境为 Yaf_Response_Cli。定义了 setHeader/setAllHeaders/getHeader/clearHeaders 的占位实现，实际功能由 Yaf_Response_Http 提供。

```php
abstract class Yaf_Response_Abstract implements Stringable
```

## 方法

### `Yaf_Response_Abstract::__construct`

构造方法，无实际操作。响应对象通常由 Yaf 框架自动创建，无需手动实例化。

```php
public function __construct(): mixed
```

**返回值**: 无返回值（构造方法）

### `Yaf_Response_Abstract::__toString`

将所有 body 段按设置的先后顺序拼接后返回，不输出任何内容。

```php
public function __toString(): string
```

**返回值**: 拼接后的响应内容字符串；没有任何 body 时返回空字符串

**示例**

```php
$response->setBody("Hello World");
echo $response; // Hello World
```

### `Yaf_Response_Abstract::setBody`

设置指定 body 段的内容，已存在的同名内容会被覆盖。name 省略时使用默认段 "content"。可通过不同 name 保存多段内容，全部输出时按设置顺序发送，可用于最后拼装 layout。

```php
public function setBody($body, $name = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$body` | 要响应的字符串，一般是一段 HTML 或 JSON |
| `$name` | body 段名称，省略时为 "content" |

**返回值**: 成功返回 Yaf_Response_Abstract 实例自身，失败返回 FALSE

**示例**

```php
$this->getResponse()->setBody("Hello World");
```

### `Yaf_Response_Abstract::appendBody`

向指定 body 段已有内容的末尾追加新内容；该段尚不存在时效果等同 setBody。name 省略时使用默认段 "content"。

```php
public function appendBody($body, $name = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$body` | 要追加的字符串 |
| `$name` | body 段名称，省略时为 "content" |

**返回值**: 成功返回 Yaf_Response_Abstract 实例自身，失败返回 FALSE

**示例**

```php
$response->setBody("Hello");
$response->appendBody(" World");
echo $response->getBody(); // Hello World
```

### `Yaf_Response_Abstract::prependBody`

向指定 body 段已有内容的前面插入新内容；该段尚不存在时效果等同 setBody。name 省略时使用默认段 "content"。

```php
public function prependBody($body, $name = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$body` | 要插入的字符串 |
| `$name` | body 段名称，省略时为 "content" |

**返回值**: 成功返回 Yaf_Response_Abstract 实例自身，失败返回 FALSE

**示例**

```php
$response->setBody("World");
$response->prependBody("Hello ");
echo $response->getBody(); // Hello World
```

### `Yaf_Response_Abstract::clearBody`

清除已设置的响应 body。传入 name 时只清除该段；省略或传 NULL 时清除全部 body 段。

```php
public function clearBody($name = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 要清除的 body 段名称，省略或 NULL 表示清除全部 |

**返回值**: 返回 Yaf_Response_Abstract 实例自身

### `Yaf_Response_Abstract::getBody`

获取已设置的响应 body。不传参数时返回默认段 "content"；显式传 NULL 时返回包含所有 body 段的数组；传指定 name 时返回该段内容。

```php
public function getBody(?string $name = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | body 段名称；省略取默认段 "content"，显式 NULL 取全部段组成的数组 |

**返回值**: 对应的 body 内容字符串（传 NULL 时为数组）；相应内容不存在时返回空字符串（或空数组）

**示例**

```php
$response->appendBody("Hello");
var_dump($response->getBody());       // string(5) "Hello"
print_r($response->getBody(null));    // ["content" => "Hello"]
```

### `Yaf_Response_Abstract::response`

发送响应给请求端。抽象类实现按设置顺序将所有 body 段直接输出，不处理 HTTP 头；实际使用中该方法由子类 Yaf_Response_Http / Yaf_Response_Cli 覆写。

```php
public function response(): mixed
```

**返回值**: 成功返回 TRUE，失败返回 FALSE

