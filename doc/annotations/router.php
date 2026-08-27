<?php
// Annotations for group: router.
return [
    'Yaf_Router' => [
        '@class' => '**Yaf_Router** 是 Yaf 的路由器，负责把 `Yaf_Request_Abstract` 请求路由到具体的模块/控制器/动作。路由器内部维护一组按名字索引的路由协议（`Yaf_Route_Interface` 实例），初始化时自动注册名为 `_default` 的默认路由（`Yaf_Route_Static`，可通过配置 `application.dispatcher.defaultRoute` 替换）。路由时按注册的**逆序**依次尝试各协议，第一个匹配成功的协议生效，其名字被记为当前路由。除默认静态路由外，内建协议还有 Simple、Supervar、Rewrite、Regex、Map，也可注册自定义路由类。该类 final、不可序列化、不可克隆；开启 `yaf.use_namespace` 时也可用 `Yaf\Router`。',
        '::__construct' => [
            'desc' => '构造路由器并注册默认路由协议：若应用配置了 `application.dispatcher.defaultRoute` 则按其构建，否则回退为 `Yaf_Route_Static`（默认路由构建失败时触发 E_WARNING 并同样回退）。',
            'returns' => '无返回值（构造函数）',
            'example' => "\$router = new Yaf_Router();\nprint_r(\$router->getRoutes());\n// [_default] => Yaf_Route_Static Object",
        ],
        '::addRoute' => [
            'desc' => '向路由器注册一个路由协议。后注册的路由先被尝试（路由按注册逆序匹配），同名重复注册会覆盖旧协议。',
            'params' => [
                'name' => '路由协议的名字，之后可用 `getRoute($name)` 取回',
                'route' => '实现了 `Yaf_Route_Interface` 的路由协议实例',
            ],
            'returns' => '成功返回 `$this`（Yaf_Router），支持链式调用',
            'example' => "\$router = Yaf_Dispatcher::getInstance()->getRouter();\n\$route  = new Yaf_Route_Rewrite(\n    \"/product/list/:id\",\n    ['controller' => 'product', 'action' => 'info']\n);\n\$router->addRoute('product', \$route);",
        ],
        '::addConfig' => [
            'desc' => '批量注册路由协议。遍历配置，每一项是描述一条路由的数组，必须包含 `type` 键（rewrite/regex/map/simple/supervar，不区分大小写），其余键依类型而定（如 rewrite/regex 需 `match` + `route`，simple 需 `module`/`controller`/`action`，supervar 需 `varname`，map 可选 `controllerPrefer`/`delimiter`）。配置项的键作为路由名；单个条目非法时触发 E_WARNING 并跳过，不影响其余条目。',
            'params' => [
                'config' => '`Yaf_Config_Abstract` 实例或普通数组，内容为「路由名 => 路由配置数组」',
            ],
            'returns' => '成功返回 `$this`（Yaf_Router）；参数既非 Yaf_Config_Abstract 实例也非数组时触发 E_WARNING 并返回 FALSE',
            'example' => "\$config = [\n    'rewrite' => ['type' => 'rewrite', 'match' => '/yaf/:name/:value',\n                  'route' => ['controller' => 'Index', 'action' => 'action']],\n    'simple'  => ['type' => 'simple', 'module' => 'm', 'controller' => 'c', 'action' => 'a'],\n];\n\$router->addConfig(\$config);",
        ],
        '::route' => [
            'desc' => '对请求执行路由：按注册的逆序依次调用各路由协议的 `route()`，第一个匹配成功的协议负责填充 request 的模块/控制器/动作及请求参数，其名字被记为当前路由，请求被标记为已路由（`isRouted()` 为真）。内置协议直接走 C 实现；自定义协议调用其 PHP 层 `route()` 方法，返回非真值则继续尝试下一条。通常无需手动调用，`Yaf_Dispatcher::dispatch` 会自动调用。',
            'params' => [
                'request' => '要路由的 `Yaf_Request_Abstract` 实例',
            ],
            'returns' => '有路由协议匹配成功返回 TRUE；全部协议都未匹配返回 FALSE',
            'example' => "\$router  = new Yaf_Router();\n\$request = new Yaf_Request_Http('/foo/bar');\nvar_dump(\$router->route(\$request));       // bool(true)\nvar_dump(\$router->getCurrentRoute());     // string(8) \"_default\"",
        ],
        '::getRoute' => [
            'desc' => '按名字获取已注册的路由协议。',
            'params' => [
                'name' => '路由协议的名字（`addRoute` 时使用的名字；默认静态路由为 `_default`）',
            ],
            'returns' => '返回对应的 `Yaf_Route_Interface` 实例；名字为空字符串或路由不存在时返回 FALSE/NULL',
            'example' => "\$router->addRoute('simple', new Yaf_Route_Simple('m', 'c', 'a'));\n\$route = \$router->getRoute('simple');      // Yaf_Route_Simple\nvar_dump(\$router->getRoute('noexists'));  // NULL",
        ],
        '::getRoutes' => [
            'desc' => '获取路由器中已注册的全部路由协议。',
            'returns' => '「路由名 => 路由协议实例」的数组副本（含 `_default` 默认路由）',
            'example' => "\$router = new Yaf_Router();\n\$router->addRoute('super', new Yaf_Route_Supervar('r'));\nprint_r(array_keys(\$router->getRoutes())); // [\"_default\", \"super\"]",
        ],
        '::getCurrentRoute' => [
            'desc' => '获取最近一次成功路由所用路由协议的名字。',
            'returns' => '当前路由协议的名字字符串；尚未执行过路由（或没有路由匹配成功）时返回 NULL',
            'example' => "\$router  = new Yaf_Router();\n\$request = new Yaf_Request_Http('/foo/bar');\n\$router->route(\$request);\nvar_dump(\$router->getCurrentRoute()); // string(8) \"_default\"",
        ],
    ],
    'Yaf_Route_Interface' => [
        '@class' => '**Yaf_Route_Interface** 是 Yaf 路由协议的标准接口，所有内建路由（Static/Simple/Supervar/Rewrite/Regex/Map）都实现该接口；实现该接口即可编写自定义路由协议，并通过 `Yaf_Router::addRoute` 注册。自定义协议的 `route()` 返回真值表示匹配成功（路由器据此停止后续匹配），返回假值则把路由权交给下一条路由。开启 `yaf.use_namespace` 时也可用 `Yaf\Route_Interface`。',
        '::route' => [
            'desc' => '路由请求：解析 request 中的 URI 或查询信息，将模块/控制器/动作及请求参数写回 request。由 `Yaf_Router::route` 调用，无需手动执行。',
            'params' => [
                'request' => '待路由的 `Yaf_Request_Abstract` 实例',
            ],
            'returns' => '匹配成功返回 TRUE（路由器记录当前路由并停止匹配），失败返回 FALSE（继续尝试下一条路由协议）',
            'example' => "class MyRoute implements Yaf_Route_Interface {\n    public function route(\$request) {\n        \$request->setControllerName('Dummy');\n        \$request->setActionName('index');\n        return true;\n    }\n    public function assemble(\$info, \$query = null) { return ''; }\n}",
        ],
        '::assemble' => [
            'desc' => '反向组装 URL：根据给定的模块/控制器/动作信息，按本路由协议的规则生成对应的 URL 字符串。`info` 数组用 `:m`、`:c`、`:a` 三个键分别表示模块、控制器、动作。',
            'params' => [
                'info' => '路由信息数组，支持 `:m`（模块）、`:c`（控制器）、`:a`（动作）键',
                'query' => '附加的查询参数数组，会被拼入 URL（query string 或协议自身的参数形式）',
            ],
            'returns' => '成功返回组装好的 URL 字符串；失败（如缺少必要键、协议不支持反向组装）返回 NULL/FALSE',
            'example' => "\$router = Yaf_Dispatcher::getInstance()->getRouter();\necho \$router->getRoute('_default')->assemble(\n    [':c' => 'product', ':a' => 'view'],\n    ['id' => 1]\n); // /product/view?id=1",
        ],
    ],
    'Yaf_Route_Static' => [
        '@class' => '**Yaf_Route_Static** 是 Yaf 的默认路由协议（路由器中以 `_default` 注册），解析去除 base_uri 后的 request_uri：按 `/` 分段，依次尝试解释为模块、控制器、动作。第一段需通过 `Yaf_Application::$modules`（`yaf.application.modules`）校验才算模块，否则整体前移（第一段当控制器、第二段当动作）；剩余段按 `name/value` 成对解析为请求参数，落单的名值为 NULL。当 URI 只有一段时，`yaf.action_prefer=1` 会把它解释为动作而非控制器。控制器/模块名会按驼峰规整，动作名统一小写。该协议总是匹配成功（`match` 恒返回 TRUE），因此注册顺序上应放在其他路由之后。',
        '::match' => [
            'desc' => '静态路由没有匹配模式，对任何 URI 都视为匹配成功，本方法恒返回 TRUE（仅为满足协议形态而存在）。',
            'params' => [
                'uri' => '任意 URI 字符串',
            ],
            'returns' => '恒返回 TRUE',
        ],
        '::route' => [
            'desc' => '按 `/` 分段解析 request 的 URI（先剥离 base_uri），依次确定模块、控制器、动作；第一段不是合法模块名时整体前移为控制器/动作，剩余段按 `name/value` 成对写入请求参数。控制器/模块名按驼峰规整，动作名转小写。',
            'params' => [
                'request' => '待路由的 `Yaf_Request_Abstract` 实例',
            ],
            'returns' => '恒返回 TRUE（静态路由总能解析出结果）',
            'example' => "// 请求 \"/ap/foo/bar/dummy/1\"，base_uri 为 \"/ap\"，\n// 未配置名为 foo 的模块时路由结果为：\n// controller=foo, action=bar, params[dummy]=1",
        ],
        '::assemble' => [
            'desc' => '按静态路由规则组装 URL：`info` 中 `:c`（控制器）、`:a`（动作）必填，`:m`（模块）可选，输出形如 `/模块/控制器/动作` 的路径；`query` 以 `?k=v&k=v` 形式附加。',
            'params' => [
                'info' => '路由信息数组，必须包含 `:c` 和 `:a`，可选 `:m`',
                'query' => '附加查询参数数组',
            ],
            'returns' => '成功返回组装的 URL 字符串；`info` 缺少 `:c` 或 `:a` 时触发 `Yaf_Exception_TypeError` 并返回 FALSE',
            'example' => "\$router = new Yaf_Router();\necho \$router->getRoute('_default')->assemble(\n    [':m' => 'module', ':c' => 'controller', ':a' => 'action'],\n    ['tkey' => 'tval']\n); // /module/controller/action?tkey=tval",
        ],
    ],
    'Yaf_Route_Simple' => [
        '@class' => '**Yaf_Route_Simple** 基于 `$_GET` 中的三个变量做路由：构造时给出三个键名，分别代表模块、控制器、动作在 query string 中的变量名（例如 `new Yaf_Route_Simple("m", "c", "a")` 对应 `?m=xxx&c=xxx&a=xxx`）。路由时从 `$_GET` 按这三个键取值：模块值还须是合法模块名（`Yaf_Application::$modules`）才会被设置；三个键在 `$_GET` 中全都不存在时路由失败，把路由权交给下一条协议。注意只要有一个键存在即视为匹配成功，即使个别值不是字符串。',
        '::__construct' => [
            'desc' => '创建 Simple 路由。三个参数不是路由结果本身，而是 `$_GET` 中承载模块、控制器、动作的变量名（键名）。',
            'params' => [
                'module_name' => '`$_GET` 中承载模块名的键名',
                'controller_name' => '`$_GET` 中承载控制器名的键名',
                'action_name' => '`$_GET` 中承载动作名的键名',
            ],
            'returns' => '无返回值（构造函数）；参数不是字符串时抛出 `ArgumentCountError`/`TypeError`',
            'example' => "// 对于 http://domain.com/index.php?c=index&a=test\n\$route = new Yaf_Route_Simple('m', 'c', 'a');\n\$router->addRoute('simple', \$route);\n// 路由结果: controller=index, action=test",
        ],
        '::route' => [
            'desc' => '从 `$_GET` 按构造时指定的三个键读取模块/控制器/动作并写入 request。模块值须是合法模块名才会被采用；三个键全都不存在时视为未匹配。',
            'params' => [
                'request' => '待路由的 `Yaf_Request_Abstract` 实例',
            ],
            'returns' => '`$_GET` 中存在三个键之一即返回 TRUE，全不存在返回 FALSE',
            'example' => "\$route = new Yaf_Route_Simple('m', 'c', 'a');\n\$request = new Yaf_Request_Http('/');\n\$_GET = ['c' => 'index', 'a' => 'test'];\nvar_dump(\$route->route(\$request)); // bool(true)",
        ],
        '::assemble' => [
            'desc' => '按 Simple 路由规则组装 URL：用构造时的三个键名输出 query string，`info` 中 `:c`、`:a` 必填，`:m` 可选，形如 `?m=模块&c=控制器&a=动作`；`query` 以 `&k=v` 追加。',
            'params' => [
                'info' => '路由信息数组，必须包含 `:c` 和 `:a`，可选 `:m`',
                'query' => '附加查询参数数组',
            ],
            'returns' => '成功返回组装的 query string；`info` 缺少 `:c` 或 `:a` 时触发 `Yaf_Exception_TypeError` 并返回 NULL',
            'example' => "\$router = new Yaf_Router();\n\$router->addRoute('simple', new Yaf_Route_Simple('m', 'c', 'a'));\necho \$router->getRoute('simple')->assemble(\n    [':m' => 'yafmodule', ':c' => 'yafcontroller', ':a' => 'yafaction'],\n    ['tkey' => 'tval']\n); // ?m=yafmodule&c=yafcontroller&a=yafaction&tkey=tval",
        ],
    ],
    'Yaf_Route_Supervar' => [
        '@class' => '**Yaf_Route_Supervar** 与 Simple 相似，也从 `$_GET` 取路由信息，不同之处在于它取的是**一个包含完整路由路径的变量**：构造时给出变量名，如 `new Yaf_Route_Supervar("r")` 对应 `?r=/module/controller/action/k/v`。变量值按与默认静态路由（`Yaf_Route_Static`）完全相同的规则解析（模块校验、`name/value` 参数、`yaf.action_prefer` 对单段 URI 的影响也一致）。`$_GET` 中不存在该变量、或该变量的值不是字符串（如 `?r[a]=b` 这样的数组）时路由失败，把路由权交给下一条协议。',
        '::__construct' => [
            'desc' => '创建 Supervar 路由，指定 `$_GET` 中承载完整路由路径的变量名。',
            'params' => [
                'supervar_name' => '`$_GET` 中承载路由路径的变量名',
            ],
            'returns' => '无返回值（构造函数）；参数不是字符串时抛出 `ArgumentCountError`/`TypeError`',
            'example' => "// 对于 http://domain.com/index.php?r=/a/b/c\n\$route = new Yaf_Route_Supervar('r');\n\$router->addRoute('super', \$route);\n// 路由结果: module=a, controller=b, action=c",
        ],
        '::route' => [
            'desc' => '读取 `$_GET` 中 supervar 变量的值作为路由路径，按静态路由规则（`/` 分段、模块校验、参数成对解析）填充 request。值为数组等非字符串类型时视为未匹配。',
            'params' => [
                'request' => '待路由的 `Yaf_Request_Abstract` 实例',
            ],
            'returns' => 'supervar 存在且为字符串返回 TRUE，否则返回 FALSE',
            'example' => "\$route = new Yaf_Route_Supervar('r');\n\$request = new Yaf_Request_Http('/');\n\$_GET = ['r' => '/a/b/c'];\nvar_dump(\$route->route(\$request)); // bool(true)",
        ],
        '::assemble' => [
            'desc' => '按 Supervar 规则组装 URL：输出 `?变量名=/模块/控制器/动作` 形式的路径，`info` 中 `:c`、`:a` 必填，`:m` 可选；`query` 以 `&k=v` 追加。',
            'params' => [
                'info' => '路由信息数组，必须包含 `:c` 和 `:a`，可选 `:m`',
                'query' => '附加查询参数数组',
            ],
            'returns' => '成功返回组装的 URL；`info` 缺少 `:c` 或 `:a` 时触发 `Yaf_Exception_TypeError` 并返回 NULL',
            'example' => "\$router = new Yaf_Router();\n\$router->addRoute('supervar', new Yaf_Route_Supervar('r'));\necho \$router->getRoute('supervar')->assemble(\n    [':m' => 'yafmodule', ':c' => 'yafcontroller', ':a' => 'yafaction'],\n    ['tkey' => 'tval']\n); // ?r=/yafmodule/yafcontroller/yafaction&tkey=tval",
        ],
    ],
    'Yaf_Route_Rewrite' => [
        '@class' => '**Yaf_Route_Rewrite** 基于匹配模式对 URI 做重写路由，是功能最灵活的内建路由。构造参数 `match` 是匹配模式，支持两种标记：`:变量名` 匹配一个 URL 段并把值记为同名参数（如 `/product/:ident`）；`*` 通配其后所有段，按 `name/value` 成对解析为参数。段间以 `/` 分隔，匹配区分大小写由内部生成的正则决定（带 `i` 修饰符，大小写不敏感），且自 URI 头部锚定。`route` 参数给出路由目标数组（`module`/`controller`/`action`），其值若以 `:` 开头则引用匹配出的同名变量，否则为固定值。`verify` 参数被接受但当前版本未参与匹配逻辑。匹配成功后所有捕获变量（含 `*` 展开的参数）写入请求参数。',
        '::__construct' => [
            'desc' => '创建 Rewrite 路由：给出匹配模式、路由目标映射，以及可选的 verify。',
            'params' => [
                'match' => '匹配模式，支持 `:变量名` 与 `*` 通配符，如 `/product/:ident/*`',
                'route' => '路由目标数组，可含 `module`/`controller`/`action`；值以 `:` 开头时引用匹配出的同名变量，否则为固定值',
                'verify' => '可选，数组（当前版本保留，未参与匹配逻辑）',
            ],
            'returns' => '无返回值（构造函数）；`match` 非字符串或 `route` 非数组时抛出 `ArgumentCountError`/`TypeError`',
            'example' => "\$route = new Yaf_Route_Rewrite(\n    '/product/:ident',\n    ['controller' => 'products', 'action' => 'view']\n);\n\$router->addRoute('product', \$route);\n// /product/chocolate-bar → products/view, params[ident]=chocolate-bar",
        ],
        '::match' => [
            'desc' => '用构造时的匹配模式测试一个 URI，不修改任何 request 状态。模式内部的 `:变量名` 编译为命名捕获组，`*` 编译为捕获剩余部分的通配组（其内容按 `name/value` 成对展开为参数）。',
            'params' => [
                'uri' => '待测试的 URI 字符串',
            ],
            'returns' => '匹配成功返回捕获结果数组（变量名 => 值，含 `*` 展开的参数）；URI 为空或不匹配返回 FALSE',
            'example' => "\$route = new Yaf_Route_Rewrite('/product/:ident/*',\n    ['controller' => 'products', 'action' => 'view']);\nvar_dump(\$route->match('/product/ipod/page/2'));\n// [\"ident\"]=>\"ipod\", [\"page\"]=>\"2\"",
        ],
        '::route' => [
            'desc' => '对 request 的 URI（去除 base_uri 后）执行模式匹配；成功后按 `route` 映射设置模块/控制器/动作（映射值以 `:` 开头时取捕获的同名变量），并把全部捕获变量写入请求参数。',
            'params' => [
                'request' => '待路由的 `Yaf_Request_Abstract` 实例',
            ],
            'returns' => '匹配成功返回 TRUE，失败返回 FALSE',
            'example' => "\$route = new Yaf_Route_Rewrite('/product/:ident',\n    ['controller' => 'products', 'action' => 'view']);\n\$request = new Yaf_Request_Http('/product/ipod');\nvar_dump(\$route->route(\$request)); // bool(true)\necho \$request->getParam('ident');   // ipod",
        ],
        '::assemble' => [
            'desc' => '按构造时的匹配模式反向组装 URL：`info` 中提供与模式中 `:变量名` 同名的键进行替换（含 `:m`/`:c`/`:a` 形式），`*` 通配段由 `info` 中其余未消费的键按 `key/value/key/value` 展开填充；`query` 以 `?k=v&k=v` 附加。',
            'params' => [
                'info' => '路由信息数组，键需与模式中的 `:变量名` 对应',
                'query' => '附加查询参数数组',
            ],
            'returns' => '返回组装的 URL 字符串；缺少占位符对应的键时占位符保持原样',
            'example' => "\$route = new Yaf_Route_Rewrite('/product/:ident/*',\n    ['controller' => 'products', 'action' => 'view']);\necho \$route->assemble(\n    [':ident' => 'ipod', ':sort' => 'price'],\n    ['page' => 2]\n); // /product/ipod/sort/price/?page=2",
        ],
    ],
    'Yaf_Route_Regex' => [
        '@class' => '**Yaf_Route_Regex** 用完整 PCRE 正则匹配 URI，是灵活度最高的内建路由（正则需自带定界符，如 `#^/product/([^/]+)#`）。构造参数 `match` 为正则；`route` 为路由目标数组，值以 `:` 开头时引用捕获的同名变量，否则为固定值；`map` 是「捕获组序号 => 参数名」的映射，把数字分组（`$1`、`$2`…）转成有意义的参数名，正则中的命名捕获组也可直接使用；`reverse` 是反向组装用的 URL 模板（含 `:m`/`:c`/`:a` 占位符），供 `assemble` 使用；`verify` 被接受但当前版本未参与匹配。匹配成功后，所有命名/映射出的捕获变量写入请求参数。',
        '::__construct' => [
            'desc' => '创建 Regex 路由：给出正则、路由目标映射，可选给出捕获组映射、verify 和反向组装模板。',
            'params' => [
                'match' => 'PCRE 正则（含定界符），用于匹配 URI',
                'route' => '路由目标数组，可含 `module`/`controller`/`action`；值以 `:` 开头时引用捕获的同名变量，否则为固定值',
                'map' => '可选，「捕获组序号 => 参数名」映射，如 `[1 => \'ident\']`',
                'verify' => '可选，数组（当前版本保留，未参与匹配逻辑）',
                'reverse' => '可选，`assemble` 反向组装时使用的 URL 模板，如 `/product/:ident`',
            ],
            'returns' => '无返回值（构造函数）；`match` 非字符串或 `route` 非数组时抛出 `ArgumentCountError`/`TypeError`',
            'example' => "\$route = new Yaf_Route_Regex(\n    '#^/product/([a-zA-Z0-9_-]+)#',\n    ['controller' => 'products', 'action' => 'view'],\n    [1 => 'ident']\n);\n\$router->addRoute('product', \$route);",
        ],
        '::match' => [
            'desc' => '用构造时的正则测试一个 URI，不修改任何 request 状态。命名捕获组按组名入结果；数字分组通过 `map` 映射成参数名入结果（无 map 时数字分组被忽略）。',
            'params' => [
                'uri' => '待测试的 URI 字符串',
            ],
            'returns' => '匹配成功返回捕获结果数组（参数名 => 值）；URI 为空、正则非法或不匹配返回 FALSE',
            'example' => "\$route = new Yaf_Route_Regex('#^/product/([^/]+)#',\n    ['controller' => 'products', 'action' => 'view'],\n    [1 => 'ident']);\nvar_dump(\$route->match('/product/ipod'));\n// [\"ident\"]=>\"ipod\"",
        ],
        '::route' => [
            'desc' => '对 request 的 URI（去除 base_uri 后）执行正则匹配；成功后按 `route` 映射设置模块/控制器/动作（映射值以 `:` 开头时取捕获的同名变量），并把全部捕获变量写入请求参数。',
            'params' => [
                'request' => '待路由的 `Yaf_Request_Abstract` 实例',
            ],
            'returns' => '匹配成功返回 TRUE，失败返回 FALSE',
            'example' => "\$route = new Yaf_Route_Regex('#^/product/([^/]+)#',\n    ['controller' => 'products', 'action' => 'view'],\n    [1 => 'ident']);\n\$request = new Yaf_Request_Http('/product/ipod');\nvar_dump(\$route->route(\$request)); // bool(true)\necho \$request->getParam('ident');   // ipod",
        ],
        '::assemble' => [
            'desc' => '按构造时给出的 `reverse` 模板反向组装 URL：把 `info` 中的 `:m`/`:c`/`:a` 替换进模板，`query` 以 `?k=v&k=v` 附加。',
            'params' => [
                'info' => '路由信息数组，支持 `:m`、`:c`、`:a` 键',
                'query' => '附加查询参数数组',
            ],
            'returns' => '成功返回组装的 URL；构造时未提供 `reverse` 时返回 NULL',
            'example' => "\$route = new Yaf_Route_Regex('#^/product/([^/]+)#',\n    ['controller' => 'products'], [], [],\n    '/:m/:c/:a');\necho \$route->assemble(\n    [':m' => 'module', ':c' => 'controller', ':a' => 'action'],\n    ['tkey' => 'tval']\n); // /module/controller/action?tkey=tval",
        ],
    ],
    'Yaf_Route_Map' => [
        '@class' => '**Yaf_Route_Map** 把 URI（去除 base_uri 后）按 `/` 分段并合并成一个下划线连接的名字，作为控制器或动作的路由结果：构造参数 `controller_prefer` 为 TRUE 时结果作为控制器名（如 `/foo/bar` → 控制器 `Foo_Bar`），为 FALSE（默认）时作为动作名（如 `/foo/bar` → 动作 `foo_bar`）。第二个参数 `delimiter` 指定分隔符：URI 中「`/` + 分隔符」之前的部分参与路由，之后的部分按 `name/value` 成对解析为请求参数（如 `/foo/bar/!/page/2` 配合分隔符 `!`）。Map 路由总是匹配成功，适合作为最后一条兜底路由。',
        '::__construct' => [
            'desc' => '创建 Map 路由，可选指定控制器优先标志和参数分隔符。',
            'params' => [
                'controller_prefer' => '可选，TRUE 时 URI 合并结果作为控制器名，FALSE（默认）时作为动作名',
                'delimiter' => '可选，URI 中路由部分与请求参数部分的分隔符字符串（如 `#!`）；不设置时整个 URI 都参与路由',
            ],
            'returns' => '无返回值（构造函数）',
            'example' => "\$router->addRoute('map', new Yaf_Route_Map());          // 结果作为动作\n\$router->addRoute('map2', new Yaf_Route_Map(true, '#!'));\n// 控制器优先，且 /xxx/!/k/v 中 ! 之后作为请求参数",
        ],
        '::route' => [
            'desc' => '把 URI 分段合并为下划线连接的名字写入 request：控制器优先时设置为控制器名（首字母及分隔处字母大写，`/foo/bar` → `Foo_Bar`），否则设置为动作名（全小写，`/foo/bar` → `foo_bar`）。设置了分隔符时，分隔符之后的部分按 `name/value` 成对解析为请求参数。',
            'params' => [
                'request' => '待路由的 `Yaf_Request_Abstract` 实例',
            ],
            'returns' => '恒返回 TRUE（Map 路由总是匹配成功）',
            'example' => "// 请求 URI \"/foo/bar\"：\n// new Yaf_Route_Map()        → action=foo_bar\n// new Yaf_Route_Map(true)    → controller=Foo_Bar\n// 带分隔符 \"!\" 时 \"/foo/bar/!/page/2\" → params[page]=2",
        ],
        '::assemble' => [
            'desc' => '按 Map 规则反向组装 URL：控制器优先时从 `info` 的 `:a`（动作名）取值，否则从 `:c`（控制器名）取值，按 `_` 拆分为路径段。设置了分隔符时 `query` 以 `/分隔符/k/v/k/v` 形式附加，否则以 `?k=v&k=v` 附加。',
            'params' => [
                'info' => '路由信息数组；控制器优先时须含 `:a`，否则须含 `:c`',
                'query' => '附加参数数组',
            ],
            'returns' => '成功返回组装的 URL；缺少所需键时触发 `Yaf_Exception_TypeError` 并返回 NULL',
            'example' => "\$route = new Yaf_Route_Map();\necho \$route->assemble([':c' => 'foo_bar'], ['tkey' => 'tval']);\n// /foo/bar?tkey=tval\n\$route = new Yaf_Route_Map(true, '_');\necho \$route->assemble([':a' => 'foo_bar'], ['tkey' => 'tval']);\n// /foo/bar/_/tkey/tval",
        ],
    ],
];
