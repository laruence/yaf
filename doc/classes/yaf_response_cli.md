# Yaf_Response_Cli

CLI 环境下的响应对象，继承自 Yaf_Response_Abstract，没有自己的方法。Yaf 框架在 cli/phpdbg SAPI 下自动创建本类实例。response() 只按顺序输出 body 内容，不发送任何 HTTP 头；setRedirect 在 CLI 环境下恒返回 FALSE。

```php
class Yaf_Response_Cli extends Yaf_Response_Abstract implements Stringable
```

继承: `Yaf_Response_Abstract` ← `Yaf_Response_Cli`

## 继承的方法

- [Yaf_Response_Abstract](yaf_response_abstract.md): `__construct`, `__toString`, `setBody`, `appendBody`, `prependBody`, `clearBody`, `getBody`, `response`

