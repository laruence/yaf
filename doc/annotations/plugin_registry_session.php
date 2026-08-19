<?php
/**
 * Annotations: Yaf_Plugin_Abstract, Yaf_Registry, Yaf_Session.
 *
 * Verified against yaf_plugin.c/h, yaf_registry.c/h, yaf_session.c/h,
 * yaf_dispatcher.c (YAF_PLUGIN_HANDLE call sites) and php_yaf.h (hook names).
 *
 * Plugin hook order in yaf_dispatcher_dispatch():
 *   routerStartup -> route -> routerShutdown -> dispatchLoopStartup
 *   -> [preDispatch -> action -> postDispatch]* -> dispatchLoopShutdown
 * preResponse is defined but never invoked (no YAF_HOOK_PRERESPONSE entry).
 */

return [
    'Yaf_Plugin_Abstract' => [
        '@class' => "Yaf 插件基类，所有插件必须继承它。插件通过 `Yaf_Dispatcher::registerPlugin()` 注册，在请求生命周期的固定时机由分发器自动回调（基类空实现会被跳过，只有子类重写的方法才会被调用），每个钩子都收到当前的 `\$request` 和 `\$response` 对象。\n\n钩子触发顺序（见 `yaf_dispatcher_dispatch()`）：\n1. `routerStartup` — 路由之前；\n2. `routerShutdown` — 路由成功之后；\n3. `dispatchLoopStartup` — 分发循环开始之前；\n4. `preDispatch` — 每次分发 controller action 之前（forward 导致循环多轮时，每轮都会触发）；\n5. `postDispatch` — 每次分发 action 之后；\n6. `dispatchLoopShutdown` — 分发循环结束之后；\n7. `preResponse` — 预留钩子，当前版本不会被自动调用。\n\n插件可以部署在应用的 `plugins/` 目录下（靠类名后缀被自动加载器识别，受 `ap.name_suffix`/`ap.name_separator` 影响），也可以作为普通类库加载。钩子内抛出的异常走框架异常处理流程。",
        '::routerStartup' => [
            'desc' => '路由前钩子。在分发器对请求做路由之前调用，此时请求尚未路由，可用于改写请求 URI/参数、鉴权跳转等。基类实现为空，返回 TRUE；子类重写后才会被调用。',
            'params' => ['request' => '当前请求对象', 'response' => '当前响应对象'],
            'returns' => '返回值被忽略（基类返回 TRUE）',
            'example' => "class AuthPlugin extends Yaf_Plugin_Abstract {\n    public function routerStartup(Yaf_Request_Abstract \$request, Yaf_Response_Abstract \$response) {\n        // 路由前检查登录态，必要时改写请求\n        if (empty(\$_COOKIE['sid'])) {\n            \$request->setControllerName('Login');\n        }\n    }\n}",
        ],
        '::routerShutdown' => [
            'desc' => '路由后钩子。路由成功完成后调用，此时 request 上已设置好 module/controller/action，可用于基于路由结果做权限检查或参数修正。',
            'params' => ['request' => '当前请求对象（已完成路由）', 'response' => '当前响应对象'],
            'returns' => '返回值被忽略（基类返回 TRUE）',
            'example' => '',
        ],
        '::dispatchLoopStartup' => [
            'desc' => '分发循环启动钩子。路由完成后、进入分发循环（开始实例化 controller 并执行 action）之前调用，整个请求只触发一次。',
            'params' => ['request' => '当前请求对象', 'response' => '当前响应对象'],
            'returns' => '返回值被忽略（基类返回 TRUE）',
            'example' => '',
        ],
        '::dispatchLoopShutdown' => [
            'desc' => '分发循环结束钩子。所有分发轮次结束（request 已被标记 dispatched，或达到 forward 上限）后调用，整个请求只触发一次，适合做收尾清理、日志记录。',
            'params' => ['request' => '当前请求对象', 'response' => '当前响应对象'],
            'returns' => '返回值被忽略（基类返回 TRUE）',
            'example' => '',
        ],
        '::preDispatch' => [
            'desc' => '分发前钩子。分发循环中每次执行 controller action 之前调用；若 action 内发生 forward，循环会再次经过此钩子，因此可能触发多次。',
            'params' => ['request' => '当前请求对象', 'response' => '当前响应对象'],
            'returns' => '返回值被忽略（基类返回 TRUE）',
            'example' => '',
        ],
        '::postDispatch' => [
            'desc' => '分发后钩子。每次 controller action 执行完毕后调用，与 preDispatch 配对，forward 场景下可能触发多次。',
            'params' => ['request' => '当前请求对象', 'response' => '当前响应对象'],
            'returns' => '返回值被忽略（基类返回 TRUE）',
            'example' => '',
        ],
        '::preResponse' => [
            'desc' => '响应前钩子（预留）。定义在基类中，但当前版本的分发器不会在任何时机自动调用它；仅当手动调用时才会执行。',
            'params' => ['request' => '当前请求对象', 'response' => '当前响应对象'],
            'returns' => '返回值被忽略（基类返回 TRUE）',
            'example' => '',
        ],
    ],
    'Yaf_Registry' => [
        '@class' => "对象注册表（全局容器），用于在整个请求生命周期内跨组件共享对象和值。final 类，构造器私有、不可克隆、不可序列化；内部以单例保存条目，`get/has/set/del` 均为静态方法，直接操作同一个请求级实例。\n\n注册表实例在首次访问时惰性创建，存活于当前请求（存储在 Yaf 模块全局量中），不跨请求持久化。典型用法是在 Bootstrap 中 `set()` 共享配置/依赖，在 Controller、Plugin 中 `get()` 取用。",
        '::__construct' => [
            'desc' => '私有构造器，禁止直接 new。只能通过静态方法或 getInstance 使用单例。',
            'returns' => '无',
            'example' => '',
        ],
        '::get' => [
            'desc' => '读取注册表中指定名称的条目。',
            'params' => ['name' => '条目名称'],
            'returns' => '条目存在时返回存储的值（可为任意类型）；不存在时返回 NULL。',
            'example' => "// Bootstrap 中存入\nYaf_Registry::set('config', \$app->getConfig());\n// 任意位置取出\n\$config = Yaf_Registry::get('config');\nvar_dump(Yaf_Registry::get('missing')); // NULL",
        ],
        '::has' => [
            'desc' => '检查注册表中是否存在指定名称的条目。',
            'params' => ['name' => '条目名称'],
            'returns' => '存在返回 TRUE，否则返回 FALSE。',
            'example' => "if (Yaf_Registry::has('config')) {\n    \$config = Yaf_Registry::get('config');\n}",
        ],
        '::set' => [
            'desc' => '写入或覆盖注册表条目。同名条目重复 set 会直接覆盖旧值。',
            'params' => ['name' => '条目名称', 'value' => '要存储的值，任意类型'],
            'returns' => '写入成功返回 TRUE，失败返回 FALSE。',
            'example' => "Yaf_Registry::set('db', new PDO(\$dsn, \$user, \$pass));",
        ],
        '::del' => [
            'desc' => '删除注册表中的指定条目。条目不存在时也不会报错。',
            'params' => ['name' => '条目名称'],
            'returns' => '始终返回 TRUE。',
            'example' => "Yaf_Registry::set('tmp', 1);\nYaf_Registry::del('tmp');\nvar_dump(Yaf_Registry::has('tmp')); // false",
        ],
    ],
    'Yaf_Session' => [
        '@class' => "Yaf 对 PHP session 的包装，abstract final 类（构造器私有，不可直接实例化，通过 getInstance() 获取单例），实现了 `Iterator`、`ArrayAccess`、`Countable`，因此除了 `get/set/has/del` 之外，还支持数组下标、属性访问和 foreach 遍历。\n\n所有读写都直接作用于 `\$_SESSION`：实现上每次访问都从符号表实时解析 `\$_SESSION` 数组，不缓存指针。因此一旦用户代码 `unset(\$_SESSION)`（或 session 被销毁），已持有的实例会降级为不可用状态：`get()` 返回 NULL、`has()` 返回 FALSE、`set()/del()` 返回 FALSE、`count()` 返回 NULL、`clear()` 返回 FALSE、foreach 无内容。\n\n`getInstance()` 首次创建实例时会自动调用 `session_start()`；若启动失败（`\$_SESSION` 不可用）会产生一条 E_WARNING。",
        '::__construct' => [
            'desc' => '私有构造器，禁止直接 new，只能通过 getInstance 获取单例。',
            'returns' => '无',
            'example' => '',
        ],
        '::getInstance' => [
            'desc' => '获取 Session 单例。首次调用时创建实例并自动启动 session（等价于 session_start）；同一请求内返回同一对象。注意：若之后 $_SESSION 被 unset，该实例不会重新初始化，所有操作将返回失败值。',
            'returns' => '返回 Yaf_Session 实例。',
            'example' => "\$session = Yaf_Session::getInstance();\n\$session->set('name', 'Laruence');\necho \$_SESSION['name']; // 与 \$_SESSION 完全互通",
        ],
        '::start' => [
            'desc' => '启动 session。getInstance 首次创建实例时已自动启动，此方法是幂等的（已启动则直接跳过），保留是为了兼容旧用法和链式调用。',
            'returns' => '返回 $this，支持链式调用。',
            'example' => "\$session = Yaf_Session::getInstance()->start();",
        ],
        '::get' => [
            'desc' => '读取 session 变量。name 可省略：不传参数时返回整个 $_SESSION 数组。',
            'params' => ['name' => '变量名；省略时返回全部 session 数据'],
            'returns' => '变量存在返回其值；不存在或 $_SESSION 不可用返回 NULL；不传 name 返回整个 session 数组。',
            'example' => "\$session = Yaf_Session::getInstance();\n\$session->set('age', 28);\nvar_dump(\$session->get('age')); // 28\nvar_dump(count(\$session->get())); // 全部条目",
        ],
        '::has' => [
            'desc' => '检查 session 变量是否存在。',
            'params' => ['name' => '变量名'],
            'returns' => '存在返回 TRUE；不存在或 $_SESSION 不可用返回 FALSE。',
            'example' => "if (Yaf_Session::getInstance()->has('uid')) {\n    // 已登录\n}",
        ],
        '::set' => [
            'desc' => '写入 session 变量，同名变量直接覆盖。',
            'params' => ['name' => '变量名', 'value' => '变量值，任意类型'],
            'returns' => '写入成功返回 TRUE；$_SESSION 不可用返回 FALSE。',
            'example' => "\$session = Yaf_Session::getInstance();\n\$session->set('uid', 1024);",
        ],
        '::del' => [
            'desc' => '删除指定 session 变量。',
            'params' => ['name' => '变量名'],
            'returns' => '删除成功返回 TRUE；变量不存在或 $_SESSION 不可用返回 FALSE。',
            'example' => "\$session = Yaf_Session::getInstance();\n\$session->set('captcha', 'abcd');\n\$session->del('captcha'); // 验证后删除",
        ],
        '::count' => [
            'desc' => '返回 session 变量的个数（Countable 接口实现，可直接 count($session)）。',
            'returns' => 'session 变量个数；$_SESSION 不可用时返回 NULL。',
            'example' => "\$session = Yaf_Session::getInstance();\n\$session->set('a', 1)->set('b', 2);\nvar_dump(count(\$session)); // int(2)",
        ],
        '::clear' => [
            'desc' => '清空所有 session 变量（只清空 $_SESSION 内容，不销毁 session 本身）。',
            'returns' => '成功返回 $this 支持链式；$_SESSION 不可用返回 FALSE。',
            'example' => "Yaf_Session::getInstance()->clear();",
        ],
        '::offsetGet' => [
            'desc' => 'ArrayAccess 接口实现，等价于 get()，支撑 $session["name"] 读语法。',
            'params' => ['name' => '变量名'],
            'returns' => '同 get()：存在返回值，不存在或 $_SESSION 不可用返回 NULL。',
            'example' => "\$session = Yaf_Session::getInstance();\n\$session['company'] = 'Realsee';\necho \$session['company'];",
        ],
        '::offsetSet' => [
            'desc' => 'ArrayAccess 接口实现，等价于 set()，支撑 $session["name"] = $value 写语法。',
            'params' => ['name' => '变量名', 'value' => '变量值'],
            'returns' => '同 set()：成功 TRUE，$_SESSION 不可用 FALSE。',
            'example' => '',
        ],
        '::offsetExists' => [
            'desc' => 'ArrayAccess 接口实现，等价于 has()，支撑 isset($session["name"])。',
            'params' => ['name' => '变量名'],
            'returns' => '同 has()。',
            'example' => '',
        ],
        '::offsetUnSet' => [
            'desc' => 'ArrayAccess 接口实现，等价于 del()，支撑 unset($session["name"])。',
            'params' => ['name' => '变量名'],
            'returns' => '同 del()：删除成功 TRUE，否则 FALSE。',
            'example' => '',
        ],
        '::__get' => [
            'desc' => '魔术方法，等价于 get()，支撑 $session->name 属性读语法。',
            'params' => ['name' => '变量名'],
            'returns' => '同 get()。',
            'example' => "\$session = Yaf_Session::getInstance();\n\$session->age = 28;\necho \$session->age; // 28",
        ],
        '::__isset' => [
            'desc' => '魔术方法，等价于 has()，支撑 isset($session->name)。',
            'params' => ['name' => '变量名'],
            'returns' => '同 has()。',
            'example' => '',
        ],
        '::__set' => [
            'desc' => '魔术方法，等价于 set()，支撑 $session->name = $value 属性写语法。',
            'params' => ['name' => '变量名', 'value' => '变量值'],
            'returns' => '同 set()。',
            'example' => '',
        ],
        '::__unset' => [
            'desc' => '魔术方法，等价于 del()，支撑 unset($session->name)。',
            'params' => ['name' => '变量名'],
            'returns' => '同 del()：删除成功 TRUE，否则 FALSE。',
            'example' => '',
        ],
    ],
];
