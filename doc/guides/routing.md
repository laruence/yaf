# 路由机制

Yaf 的路由由 [Yaf_Router](../classes/yaf_router.md) 驱动：路由器维护一张「名字 => 路由协议」表，初始化时注册名为 `_default` 的默认路由（[Yaf_Route_Static](../classes/yaf_route_static.md)，可用配置 `application.dispatcher.defaultRoute` 替换）。分发请求时，路由器按**注册的逆序**逐个调用协议的 `route($request)`，第一个返回真值的协议生效：它负责填充 request 的模块/控制器/动作和请求参数，其名字记为当前路由（`getCurrentRoute()` 可取），请求标记为已路由。全部协议都未匹配时路由失败（见[错误与异常处理](error-handling.md)）。

所有协议都实现 [Yaf_Route_Interface](../classes/yaf_route_interface.md) 的两个方法：`route()`（匹配并写回 request）和 `assemble()`（反向组装 URL）。

## 默认路由：Yaf_Route_Static

Static 路由解析去除 base_uri 后的 request_uri（`routes/yaf_route_static.c` 的 `yaf_route_pathinfo_route`）：

1. 按 `/` 分段（连续分隔符跳过），最多取三段，依次尝试解释为模块、控制器、动作；
2. 取到三段时，第一段必须是合法模块名（与 `Yaf_Application::$modules` 做大小写不敏感比较），否则整体左移：第一段当控制器、第二段当动作、第三段起并入参数串；
3. 只有两段时，固定解释为控制器/动作（源码中两段的模块判断已注释掉）；
4. 只有一段时，由 `yaf.action_prefer` 决定：值为真解释为**动作**，否则为控制器；
5. 动作之后剩余的段按 `name/value` 成对解析为请求参数，落单的名其值为 NULL。

大小写规则：模块和控制器名经驼峰规整（`yaf_build_camel_name`：首字母大写、`foo_bar` → `FooBar`），动作名统一小写。

```
// 请求 /ap/foo/bar/dummy/1，base_uri=/ap，未配置名为 Foo 的模块：
//   controller=Foo, action=bar, params[dummy]=1
// 配置 application.modules="Index,Foo" 后：
//   module=Foo, controller=Bar, action=dummy, params[1]=NULL
```

Static 没有匹配模式，`route()` 恒成功（`match()` 恒返回 TRUE）。由于路由器按注册逆序尝试，先注册的 `_default` 总是最后兜底——这正是它该处的位置。

## 内建路由选择矩阵

| 协议 | 匹配源 | 匹配失败条件 | 适用场景 |
| --- | --- | --- | --- |
| [Static](../classes/yaf_route_static.md) | request_uri 路径段 | 恒成功 | 默认约定式 URL，兜底 |
| [Simple](../classes/yaf_route_simple.md) | `$_GET` 中三个指定变量 | 三个变量都不存在 | 兼容 `?m=&c=&a=` 形式 |
| [Supervar](../classes/yaf_route_supervar.md) | `$_GET` 中一个变量（整个 pathinfo） | 变量不存在或非字符串 | `?r=/a/b/c` 形式，PATH_INFO 不可用时的替代 |
| [Rewrite](../classes/yaf_route_rewrite.md) | URI 对 `:占位/*` 模式匹配 | 模式不匹配 | 语义化 URL，最常用 |
| [Regex](../classes/yaf_route_regex.md) | URI 对完整 PCRE 匹配 | 正则不匹配 | 需要约束段格式（数字、枚举等） |
| [Map](../classes/yaf_route_map.md) | URI 段拼接成一个名字 | 恒成功 | REST 风格，URL 段即控制器/动作层级 |

### Simple / Supervar

Simple 构造时给出 module/controller/action 在 `$_GET` 中的变量名；变量值经 `yaf_request_set_*` 写入 request（同样驼峰/小写规整），module 值还须是合法模块名才被接受。Supervar 则把 `$_GET[varname]` 的整个字符串当 pathinfo，走与 Static 相同的分段逻辑。

```php
$router = Yaf_Dispatcher::getInstance()->getRouter();
$router->addRoute('simple', new Yaf_Route_Simple('m', 'c', 'a')); // ?c=index&a=test
$router->addRoute('super',  new Yaf_Route_Supervar('r'));         // ?r=/foo/bar
```

### Rewrite

`match` 模式中 `:name` 是一个段占位（编译为 `[^/]+` 命名子组），`*` 之后的所有段按 `name/value` 成对解析为参数。路由内部把模式编译成以 `#` 为定界符、带 `^` 锚定和 `i` 修饰的正则。`route` 数组给出模块/控制器/动作：值是字面量直接使用，以 `:` 开头则从匹配结果取同名占位的值。构造函数可选的 `verify` 参数当前实现只保存、不参与匹配校验。

```php
$route = new Yaf_Route_Rewrite(
    'product/:ident/*',
    ['controller' => 'products', 'action' => 'view']
);
$router->addRoute('product', $route);
// /product/chocolate-bar/test/value1 → controller=Products, action=view,
//   params: ident=chocolate-bar, test=value1
```

### Regex

`match` 是完整 PCRE（自带定界符，如 `#^list/(\d+)#`）。数字子组通过 `map` 数组映射成参数名（未映射的只能按数字下标取）；命名子组直接以组名为参数名。`route` 数组规则与 Rewrite 相同（字面量或 `:占位` 引用）。`reverse` 字符串供 `assemble()` 反向生成 URL。

```php
$route = new Yaf_Route_Regex(
    '#^list/([^/]*)/([^/]*)#',
    ['controller' => 'Index', 'action' => 'action'],
    [1 => 'name', 2 => 'value']
);
$router->addRoute('list', $route);
// /list/foo/bar → params: name=foo, value=bar
```

### Map

把 URI（去 base_uri 后）各段用 `_` 拼成一个名字：`controller_prefer` 为 TRUE 时作为控制器名（首字母大写，`/foo/bar` → `Foo_Bar`），默认 FALSE 作为动作名（`foo_bar`）。若设置了 `delimiter`，URI 中「`/` + 分隔符」之前参与路由，之后按 `name/value` 成对解析为参数（`/foo/bar/!/page/2` 配 delimiter `!`）。Map 恒匹配成功，适合做最后一条路由。

```php
$router->addRoute('rest', new Yaf_Route_Map(true, '!'));
// /foo/bar/!/page/2 → controller=Foo_Bar, params[page]=2
```

## 自定义路由

实现 `Yaf_Route_Interface` 即可：`route($request)` 返回真值表示匹配成功（路由器停止后续匹配），返回假值则交给下一条协议；`assemble()` 负责反向组装 URL。在 Bootstrap 中注册：

```php
class Bootstrap extends Yaf_Bootstrap_Abstract {
    public function _initRoute(Yaf_Dispatcher $dispatcher) {
        $dispatcher->getRouter()->addRoute('dummy', new DummyRoute());
    }
}

class DummyRoute implements Yaf_Route_Interface {
    public function route($request) {
        if ($request->getRequestUri() === '/dummy') {
            $request->setControllerName('Dummy');
            $request->setActionName('index');
            return true;
        }
        return false;
    }
    public function assemble(array $info, array $query = NULL) { return '/dummy'; }
}
```

后注册的路由先被尝试；同名重复注册会覆盖。

## addConfig 批量注册

`Yaf_Router::addConfig()` 接受 [Yaf_Config_Abstract](../classes/yaf_config_abstract.md) 实例或普通数组，每项是「路由名 => 路由描述数组」，描述数组必须含 `type` 键（rewrite/regex/map/simple/supervar，大小写不敏感；**不支持 static**），其余键由 `routes/yaf_route_interface.c` 的 `yaf_route_instance` 决定：

| type | 必需键 | 可选键 |
| --- | --- | --- |
| rewrite | `match`(string), `route`(array) | `verify` |
| regex | `match`(string), `route`(array) | `map`, `verify`, `reverse` |
| simple | `module`, `controller`, `action`（均为 $_GET 变量名） | — |
| supervar | `varname` | — |
| map | — | `controllerPrefer`, `delimiter` |

单个条目非法时触发 E_WARNING 并跳过，不影响其余条目；成功返回 `$this` 支持链式调用。

```php
$router->addConfig([
    'rewrite' => ['type' => 'rewrite', 'match' => '/product/:name/:value',
                  'route' => ['controller' => 'product', 'action' => 'info']],
    'simple'  => ['type' => 'simple', 'module' => 'm', 'controller' => 'c', 'action' => 'a'],
]);
```

> 注意：原手册 addConfig 页的 INI 示例（`routes.regex.route="#^...#"`、`routes.rewrite.default.controller=...`）与本实现不符——源码要求匹配模式放在 `match` 键、目标放在 `route` 数组的 `controller`/`action`/`module` 键。

## URL 组装（assemble）

`assemble(array $info, array $query = NULL)` 是路由的逆操作，`$info` 用 `:m`/`:c`/`:a` 键指定模块/控制器/动作，`$query` 以 `?k=v&...` 附加：

| 协议 | 组装规则 | 示例 |
| --- | --- | --- |
| Static | `[/模块]/控制器/动作`（缺 `:c` 或 `:a` 报 TypeError） | `/foo/bar?a=b` |
| Simple | `?m键=&c键=&a键=` 用构造时的变量名 | `?m=&c=index&a=test` |
| Supervar | `?var=/模块/控制器/动作` | `?r=/foo/bar` |
| Rewrite | 把 `match` 模式中的 `:占位` 用 `$info` 同名键（含冒号，如 `':ident'`）替换，`*` 由剩余键按 k/v 段填充 | `/product/x` |
| Regex | 用构造时的 `reverse` 串替换 `:m/:c/:a`；未提供 `reverse` 返回 NULL | `/list/x` |
| Map | 取 `:c`（controller_prefer 时取 `:a`）按 `_` 拆成路径段；query 有 delimiter 时以 `/delim/k/v/` 附加，否则 `?k=v` | `/foo/bar` |

```php
echo $router->getRoute('_default')->assemble(
    [':c' => 'foo', ':a' => 'bar'], ['wd' => 'php']);   // /foo/bar?wd=php
```

## 路由参数传递

协议匹配出的参数经 `yaf_request_set_params_multi` 合并进 request 的参数表，业务代码用 `$request->getParam('name')` / `getParams()` 读取（Rewrite/Regex 中 route 数组用 `:占位` 引用的同名匹配值也在其中）。

分发阶段 `yaf_dispatcher_get_call_parameters`（yaf_dispatcher.c）把参数**按形参名**注入动作方法：

```php
class ProductController extends Yaf_Controller_Abstract {
    // /product/chocolate-bar 匹配出 ident=chocolate-bar
    public function viewAction($ident) {   // 自动注入
        echo $ident;
    }
}
```

绑定规则：逐个遍历动作方法的声明形参，在参数表中按形参名查找；找到则传入；未找到且形参可选则使用其默认值（无默认值的可选参数传 NULL）；必填形参缺失时交给 PHP 引擎抛出标准参数错误。动作方法的查找名是「动作名 + `action`」后缀（如 `view` → `viewAction`）。

## See Also

- [Yaf_Router](../classes/yaf_router.md)、[Yaf_Route_Interface](../classes/yaf_route_interface.md)、[Yaf_Request_Abstract](../classes/yaf_request_abstract.md)
- [错误与异常处理](error-handling.md) — 路由失败（错误码 513）的处理
