# Yaf_Response_Http

HTTP 环境下的响应对象，继承自 Yaf_Response_Abstract，额外管理 HTTP 响应头、状态码（protected 属性 response_code）和重定向。通常由 Yaf 框架自动创建；设置好的头和状态码不会立即发送，而是在 response() 调用时统一发送。

```php
class Yaf_Response_Http extends Yaf_Response_Abstract implements Stringable
```

继承: `Yaf_Response_Abstract` ← `Yaf_Response_Http`

## 方法

### `Yaf_Response_Http::setHeader`

设置 HTTP 响应头，仅记录，不立即发送（response() 时发送）。replace 默认 TRUE 表示替换同名头；为 FALSE 时新值以 ", " 连接到已有值之后。response_code 非 0 时同时设置响应状态码。

```php
public function setHeader(string $name, string $value, bool $replace = 0, int $response_code = 0): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 响应头名称，如 "Content-Type" |
| `$value` | 响应头取值 |
| `$replace` | 同名头已存在时是否替换，默认 TRUE；FALSE 表示追加合并 |
| `$response_code` | 同时设置的 HTTP 状态码，0 表示不修改状态码 |

**返回值**: 成功返回 TRUE，失败返回 FALSE

**示例**

```php
$this->getResponse()->setHeader("Content-Type", "application/json");
```

### `Yaf_Response_Http::setAllHeaders`

批量设置 HTTP 响应头，数组的 key 为头名、value 为头值，同名头总是被替换。

```php
public function setAllHeaders(array $headers): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$headers` | 头名 => 头值 的数组 |

**返回值**: 返回 TRUE

### `Yaf_Response_Http::getHeader`

获取已设置的响应头。传入 name 返回该头的值；不传参数返回所有已设置的响应头数组。

```php
public function getHeader(?string $name = NULL): mixed
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$name` | 响应头名称；省略时返回全部 |

**返回值**: 传 name 时返回对应头值，不存在返回 NULL；不传时返回全部响应头数组（未设置过头时为空数组）

### `Yaf_Response_Http::clearHeaders`

清除所有已设置的响应头。

```php
public function clearHeaders(): object|false|null
```

**返回值**: 返回 Yaf_Response_Http 实例自身（可链式调用）

### `Yaf_Response_Http::setRedirect`

重定向请求到指定 URL：发送 Location 头并设置 302 状态码（注意与 Yaf_Controller_Abstract::forward 的内部转发不同）。成功后会标记响应头已发送，response() 将不再重复发送已设置的头。url 为空，或运行在 CLI/phpdbg 环境下时不执行。

```php
public function setRedirect(string $url): ?bool
```

**参数**

| 参数 | 说明 |
| --- | --- |
| `$url` | 要重定向到的 URL |

**返回值**: 成功返回 TRUE；url 为空、CLI/phpdbg 环境或发送失败时返回 FALSE

**示例**

```php
$this->getResponse()->setRedirect("http://domain.com/");
```

### `Yaf_Response_Http::response`

发送 HTTP 响应：若响应头尚未发送，先发送状态码和所有已设置的响应头并做标记，然后按设置顺序输出所有 body 段。重复调用不会重复发送头。

```php
public function response(): ?bool
```

**返回值**: 成功返回 TRUE，失败返回 FALSE

**示例**

```php
$this->getResponse()->response();
```

## 继承的方法

- [Yaf_Response_Abstract](yaf_response_abstract.md): `__construct`, `__toString`, `setBody`, `appendBody`, `prependBody`, `clearBody`, `getBody`

