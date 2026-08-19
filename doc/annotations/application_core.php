<?php
// Annotation group: application_core
// Yaf_Application / Yaf_Bootstrap_Abstract / Yaf_Dispatcher / Yaf_Loader
// 依据 yaf 3.3.8-dev 源码 (yaf_application.c / yaf_bootstrap.c / yaf_dispatcher.c / yaf_loader.c)
// 及 tests/*.phpt 校验, 与旧手册有出入的行为已在 desc 中点明。

return [
    'Yaf_Application' => [
        '@class' => 'Yaf 应用的入口与运行环境容器。final 类, 全进程单例: 构造时解析配置、初始化 `Yaf_Dispatcher` 与请求对象, 并把自身注册为全局实例 (可用 `Yaf_Application::app()` 取回)。它持有应用目录、环境名、模块列表、配置对象等, `run()` 驱动整个分发流程。一个进程只能初始化一次, 重复初始化会抛出异常。',
        '::__construct' => [
            'desc' => '初始化 Yaf_Application。从配置中读取 `application.directory` (必需) 及 bootstrap、library、modules、dispatcher 等可选项, 创建分发器和请求对象。同一进程只允许初始化一次, 重复初始化或配置缺少 directory 时抛出异常。',
            'params' => [
                'config' => '关联数组、ini 配置文件路径字符串, 或 Yaf_Config_Abstract 实例。ini 文件时按 $environ 指定的配置节读取。',
                'environ' => '环境名 (配置节名), 缺省使用 php.ini 的 `yaf.environ` (默认 "product")。',
            ],
            'returns' => '无返回值; 初始化失败时抛出异常。',
            'example' => "\$config = array(\n    \"application\" => array(\n        \"directory\" => realpath(__DIR__) . \"/application\",\n    ),\n);\n\n\$app = new Yaf_Application(\$config);",
        ],
        '::run' => [
            'desc' => '运行应用: 驱动分发器完成路由、插件钩子和 Controller/Action 分发循环。同一实例重复调用会触发错误并返回 FALSE。分发循环的最大迭代次数由 `yaf.forward_limit` 控制 (默认 5); 注意 3.3.8 起非正值 (含 0) 会被钳制回默认值 5, 旧版把 0 当作"不限制"会导致死循环, 该语义已废弃。',
            'returns' => '成功返回 Yaf_Response_Abstract 实例 (默认响应已自动输出); 分发失败返回 FALSE。',
            'example' => "\$app = new Yaf_Application(\"conf/application.ini\");\n\$app->bootstrap()->run();",
        ],
        '::execute' => [
            'desc' => '在应用的环境下执行一个回调, 主要用于 CLI 脚本: 借用 Yaf 的自动加载、配置等外围环境, 而不走路由分发。回调之后的额外实参会原样透传。',
            'params' => [
                'callback' => '任意合法的 PHP 回调 (函数名、array($obj, "method")、闭包等), 之后可跟任意多个传给回调的参数。',
            ],
            'returns' => '返回回调的返回值; 回调执行失败或未产生返回值时返回 FALSE。',
            'example' => "\$app = new Yaf_Application(\"conf/application.ini\");\n\$app->execute(\"main\", __FILE__);\n\nfunction main(\$argc) {\n    echo \"running in cli with {\$argc}\\n\";\n}",
        ],
        '::app' => [
            'desc' => '获取当前进程中已初始化的 Yaf_Application 单例。`Yaf_Application::getInstance()` 是它的别名。',
            'returns' => '返回全局 Yaf_Application 实例; 尚未初始化时返回 NULL。',
            'example' => "\$app = new Yaf_Application(\$config);\nassert(\$app === Yaf_Application::app());",
        ],
        '::environ' => [
            'desc' => '获取当前应用使用的配置环境名, 即构造时传入 (或取自 `yaf.environ`) 的配置节名。',
            'returns' => '环境名字符串, 默认 "product"。',
            'example' => "\$app = new Yaf_Application(\"conf/application.ini\", \"develop\");\necho \$app->environ(); // develop",
        ],
        '::bootstrap' => [
            'desc' => '加载 Bootstrap 类 (默认 `application.directory/Bootstrap.php`, 可用配置项 `application.bootstrap` 覆盖路径) 并按定义顺序依次调用其中所有以 `_init` 开头的方法, 每个方法接收 `Yaf_Dispatcher` 作为参数。通常在 `run()` 之前调用, 用于注册插件、路由、视图引擎等定制。',
            'returns' => '成功返回自身实例 (支持链式调用, 如 `$app->bootstrap()->run()`); 找不到 Bootstrap 类或文件时返回 FALSE 并触发错误。',
            'example' => "class Bootstrap extends Yaf_Bootstrap_Abstract {\n    public function _initPlugin(Yaf_Dispatcher \$dispatcher) {\n        \$dispatcher->registerPlugin(new UserPlugin());\n    }\n}\n\n\$app = new Yaf_Application(\$config);\n\$app->bootstrap()->run();",
        ],
        '::getConfig' => [
            'desc' => '获取应用启动时加载的配置对象。',
            'returns' => 'Yaf_Config_Abstract 实例 (数组配置为 Yaf_Config_Simple, ini 文件为 Yaf_Config_Ini); 应用未成功初始化时返回 NULL。',
            'example' => "\$app = new Yaf_Application(\"conf/application.ini\");\nprint_r(\$app->getConfig()->get(\"application.directory\"));",
        ],
        '::getModules' => [
            'desc' => '获取配置中 `application.modules` 声明的模块列表。',
            'returns' => '模块名数组; 配置中未声明 modules 时返回 NULL。',
            'example' => "\$config = array(\"application\" => array(\n    \"directory\" => \"/path/to/app\",\n    \"modules\"   => \"Index,Admin\",\n));\n\$app = new Yaf_Application(\$config);\nprint_r(\$app->getModules()); // [\"Index\", \"Admin\"]",
        ],
        '::getDispatcher' => [
            'desc' => '获取应用持有的分发器实例, 与 `Yaf_Dispatcher::getInstance()` 返回同一对象。',
            'returns' => 'Yaf_Dispatcher 实例; 应用未成功初始化时返回 NULL。',
            'example' => "\$app = new Yaf_Application(\$config);\n\$app->getDispatcher()->registerPlugin(new UserPlugin());",
        ],
        '::setAppDirectory' => [
            'desc' => '修改应用目录 (APPLICATION_PATH), 之后的控制器、视图都从新目录加载, 但自动加载的类库路径不变。3.3.8 起 `Yaf_Dispatcher::setAppDirectory` 已移除, 请改用本方法。',
            'params' => [
                'directory' => '新的应用目录, 必须是绝对路径。',
            ],
            'returns' => '成功返回自身实例; 路径为空或非绝对路径时返回 FALSE。',
            'example' => "\$app->setAppDirectory(\"/usr/local/new/application\")->run();",
        ],
        '::getAppDirectory' => [
            'desc' => '获取当前的应用目录。',
            'returns' => '应用目录字符串, 即配置中 `application.directory` 的值。',
            'example' => "\$app = new Yaf_Application(\$config);\necho \$app->getAppDirectory();",
        ],
        '::getLastErrorNo' => [
            'desc' => '获取应用最近一次错误的错误码 (对应 YAF_ERR_* 常量)。',
            'returns' => '错误码整数, 无错误时为 0。',
            'example' => "\$app->run();\nif (\$app->getLastErrorNo()) {\n    error_log(\$app->getLastErrorMsg());\n}",
        ],
        '::getLastErrorMsg' => [
            'desc' => '获取应用最近一次错误的错误信息。',
            'returns' => '错误信息字符串, 无错误时为空字符串。',
            'example' => "if (\$app->getLastErrorNo() !== 0) {\n    trigger_error(\$app->getLastErrorMsg());\n}",
        ],
        '::clearLastError' => [
            'desc' => '清空最近一次的错误码和错误信息。',
            'returns' => '返回自身实例, 支持链式调用。',
            'example' => "\$app->clearLastError();\nassert(\$app->getLastErrorNo() === 0);",
        ],
        '::getInstance' => [
            'desc' => '`Yaf_Application::app()` 的别名, 获取全局应用单例。',
            'returns' => '返回全局 Yaf_Application 实例; 尚未初始化时返回 NULL。',
            'example' => "\$app = Yaf_Application::getInstance();\nif (\$app !== NULL) {\n    var_dump(\$app->environ());\n}",
        ],
    ],
    'Yaf_Bootstrap_Abstract' => [
        '@class' => '应用引导类的基类, 提供在分发请求之前定制应用的最早时机。它本身没有定义任何方法: 任何继承它的类 (按约定命名为 `Bootstrap`, 放在应用目录下) 中所有以 `_init` 开头的方法, 都会在 `Yaf_Application::bootstrap()` 时按定义顺序被依次调用, 每个方法接收一个 `Yaf_Dispatcher` 参数。典型用途是注册插件、自定义路由、替换视图引擎等。',
    ],
    'Yaf_Dispatcher' => [
        '@class' => '分发器, final 单例, 由 `Yaf_Application` 在初始化时创建, 负责路由请求、触发插件钩子、加载并执行 Controller/Action、渲染视图并组织响应。也可通过 `Yaf_Dispatcher::getInstance()` 获取 (3.3.8 起必须先初始化 Yaf_Application, 否则抛异常)。内部持有路由器、请求、响应、视图引擎和插件列表; 默认模块/控制器/动作可通过 setDefault* 修改。',
        '::__construct' => [
            'desc' => '私有构造方法, 禁止外部实例化。分发器是单例: 由 Yaf_Application 初始化时创建, 请通过 `Yaf_Dispatcher::getInstance()` 获取。',
            'returns' => '不可用。',
            'example' => "\$app = new Yaf_Application(\$config); // 必须先初始化应用\n\$dispatcher = Yaf_Dispatcher::getInstance();",
        ],
        '::enableView' => [
            'desc' => '开启自动渲染 (默认开启)。开启时 Action 执行完成后会自动渲染对应的视图模板。',
            'returns' => '返回自身实例, 支持链式调用。',
            'example' => "class IndexController extends Yaf_Controller_Abstract {\n    public function init() {\n        if (!\$this->getRequest()->isXmlHttpRequest()) {\n            Yaf_Dispatcher::getInstance()->enableView();\n        }\n    }\n}",
        ],
        '::disableView' => [
            'desc' => '关闭自动渲染, 常用于 Ajax/JSON 接口: Action 执行完不再渲染视图模板。',
            'returns' => '返回自身实例, 支持链式调用。',
            'example' => "class IndexController extends Yaf_Controller_Abstract {\n    public function init() {\n        if (\$this->getRequest()->isXmlHttpRequest()) {\n            Yaf_Dispatcher::getInstance()->disableView();\n        }\n    }\n}",
        ],
        '::initView' => [
            'desc' => '初始化视图引擎。Yaf 采用延迟实例化策略, 首次分发时才创建默认的 Yaf_View_Simple; 调用本方法可提前创建并指定模板目录。若已通过 `setView()` 设置了自定义视图引擎, 则直接返回已有实例, 不会重建。',
            'params' => [
                'templates_dir' => '视图模板目录的绝对路径。',
                'options' => '传给视图引擎构造函数的可选参数 (对 Yaf_View_Simple 无意义)。',
            ],
            'returns' => '返回 Yaf_View_Interface 实例; 初始化失败返回 FALSE。',
            'example' => "class Bootstrap extends Yaf_Bootstrap_Abstract {\n    public function _initView(Yaf_Dispatcher \$dispatcher) {\n        \$dispatcher->initView(APPLICATION_PATH . \"/views/\")->assign(\"site\", \"yaf\");\n    }\n}",
        ],
        '::setView' => [
            'desc' => '设置自定义视图引擎, 替代默认的 Yaf_View_Simple。通常在 Bootstrap 中调用。',
            'params' => [
                'view' => '实现了 Yaf_View_Interface 的视图引擎实例 (如 Smarty 适配器)。',
            ],
            'returns' => '返回自身实例, 支持链式调用。',
            'example' => "class Bootstrap extends Yaf_Bootstrap_Abstract {\n    public function _initSmarty(Yaf_Dispatcher \$dispatcher) {\n        \$smarty = new Smarty_Adapter(null, Yaf_Registry::get(\"config\")->get(\"smarty\"));\n        \$dispatcher->setView(\$smarty);\n    }\n}",
        ],
        '::setRequest' => [
            'desc' => '设置分发器使用的请求对象, 常见于 CLI 下手工构造请求再分发。',
            'params' => [
                'request' => 'Yaf_Request_Abstract 的实例。',
            ],
            'returns' => '返回自身实例, 支持链式调用。',
            'example' => "\$request = new Yaf_Request_Simple(\"CLI\", \"Index\", \"Index\", \"index\");\nYaf_Dispatcher::getInstance()->setRequest(\$request)->dispatch(\$request);",
        ],
        '::setResponse' => [
            'desc' => '设置分发器使用的响应对象。',
            'params' => [
                'response' => 'Yaf_Response_Abstract 的实例。',
            ],
            'returns' => '返回自身实例, 支持链式调用。',
            'example' => "\$response = new Yaf_Response_Http();\nYaf_Dispatcher::getInstance()->setResponse(\$response);",
        ],
        '::getApplication' => [
            'desc' => '获取当前 Yaf_Application 实例, 等价于 `Yaf_Application::app()`, 便于从 Dispatcher 链式回到 Application (如 `->getApplication()->bootstrap()->run()`)。',
            'returns' => '全局 Yaf_Application 实例; 尚未初始化时返回 NULL。',
            'example' => "\$app = Yaf_Dispatcher::getInstance()->getApplication();\n// 更推荐:\n\$app = Yaf_Application::app();",
        ],
        '::getRouter' => [
            'desc' => '获取路由器, 用于在 Bootstrap 中注册自定义路由。',
            'returns' => 'Yaf_Router 实例。',
            'example' => "class Bootstrap extends Yaf_Bootstrap_Abstract {\n    public function _initRoute(Yaf_Dispatcher \$dispatcher) {\n        \$router = \$dispatcher->getRouter();\n        \$router->addRoute(\"product\", new Yaf_Route_Rewrite(\n            \"/product/list/:id/\",\n            array(\"controller\" => \"product\", \"action\" => \"info\")\n        ));\n    }\n}",
        ],
        '::getResponse' => [
            'desc' => '获取当前的响应对象。响应对象在首次分发时才创建。',
            'returns' => 'Yaf_Response_Abstract 实例; 尚未发生过分发时返回 NULL。',
            'example' => "\$response = Yaf_Dispatcher::getInstance()->getResponse();\nif (\$response !== NULL) {\n    echo \$response->getBody();\n}",
        ],
        '::getRequest' => [
            'desc' => '获取当前的请求对象。',
            'returns' => 'Yaf_Request_Abstract 实例。',
            'example' => "\$request = Yaf_Dispatcher::getInstance()->getRequest();\nvar_dump(\$request->getModuleName());",
        ],
        '::getDefaultModule' => [
            'desc' => '获取默认模块名, 即配置中 `application.dispatcher.defaultModule` 的值。',
            'returns' => '默认模块名字符串, 未配置时为 "Index"。',
            'example' => "\$app = new Yaf_Application(\$config);\necho Yaf_Dispatcher::getInstance()->getDefaultModule(); // Index",
        ],
        '::getDefaultController' => [
            'desc' => '获取默认控制器名, 即配置中 `application.dispatcher.defaultController` 的值。',
            'returns' => '默认控制器名字符串, 未配置时为 "Index"。',
            'example' => "\$app = new Yaf_Application(\$config);\necho Yaf_Dispatcher::getInstance()->getDefaultController(); // Index",
        ],
        '::getDefaultAction' => [
            'desc' => '获取默认动作名, 即配置中 `application.dispatcher.defaultAction` 的值。',
            'returns' => '默认动作名字符串, 未配置时为 "index" (小写)。',
            'example' => "\$app = new Yaf_Application(\$config);\necho Yaf_Dispatcher::getInstance()->getDefaultAction(); // index",
        ],
        '::setErrorHandler' => [
            'desc' => '设置错误处理函数, 本质是对 `set_error_handler()` 的封装。在 `throwException` 关闭时, Yaf 出错会以触发错误的方式交给该回调处理, 常用于把 404 等错误映射到自定义页面。一般放在 Bootstrap 中调用。',
            'params' => [
                'callback' => '错误处理回调, 至少接受错误码 ($errno) 和错误信息 ($errstr) 两个参数, 可选错误文件、行号等。',
                'error_types' => '要捕获的错误类型掩码, 缺省为 `E_ALL | E_STRICT`。',
            ],
            'returns' => '成功返回自身实例; 调用 set_error_handler 失败时返回 FALSE。',
            'example' => "class Bootstrap extends Yaf_Bootstrap_Abstract {\n    public function _initError(Yaf_Dispatcher \$dispatcher) {\n        \$dispatcher->throwException(false);\n        \$dispatcher->setErrorHandler(function (\$errno, \$errstr) {\n            if (\$errno == YAF_ERR_NOTFOUND_CONTROLLER) {\n                header(\"HTTP/1.1 404 Not Found\");\n            }\n        }, E_ALL);\n    }\n}",
        ],
        '::setDefaultModule' => [
            'desc' => '修改默认模块。路由结果中不含模块信息时, 使用该值作为目标模块。',
            'params' => [
                'module' => '模块名, 必须是 `application.modules` 中已声明的模块, 否则触发错误。首字母会被规范化为大写。',
            ],
            'returns' => '成功返回自身实例; 应用未初始化或模块不存在时返回 FALSE。',
            'example' => "class Bootstrap extends Yaf_Bootstrap_Abstract {\n    public function _initDefaultName(Yaf_Dispatcher \$dispatcher) {\n        \$dispatcher->setDefaultModule(\"Index\")\n            ->setDefaultController(\"Index\")\n            ->setDefaultAction(\"index\");\n    }\n}",
        ],
        '::setDefaultController' => [
            'desc' => '修改默认控制器。路由结果中不含控制器信息时使用。',
            'params' => [
                'controller' => '控制器名, 首字母会被规范化为大写。',
            ],
            'returns' => '成功返回自身实例; 应用未初始化时返回 FALSE。',
            'example' => "Yaf_Dispatcher::getInstance()->setDefaultController(\"Home\");",
        ],
        '::setDefaultAction' => [
            'desc' => '修改默认动作。路由结果中不含动作信息时使用。',
            'params' => [
                'action' => '动作名, 会被规范化为小写。',
            ],
            'returns' => '成功返回自身实例; 应用未初始化时返回 FALSE。',
            'example' => "Yaf_Dispatcher::getInstance()->setDefaultAction(\"main\");",
        ],
        '::returnResponse' => [
            'desc' => '开关/查询"返回响应对象"模式。开启后分发完成不再自动输出响应, 而是把 Yaf_Response_Abstract 交给程序员自行输出; 默认关闭 (自动输出)。',
            'params' => [
                'flag' => '传入 TRUE/FALSE 进行设置; 不传参数则只查询当前状态。',
            ],
            'returns' => '带参数时返回自身实例 (支持链式); 不带参数时返回当前开关状态 bool。',
            'example' => "\$app = new Yaf_Application(\"config.ini\");\n\$response = \$app->getDispatcher()\n    ->returnResponse(true)\n    ->getApplication()->run();\n\$response->response(); // 手工输出",
        ],
        '::autoRender' => [
            'desc' => '开关/查询自动渲染。开启 (默认) 时, Action 执行完成后自动调用视图引擎渲染对应模板; 控制器属性 `$yafAutoRender` 的优先级高于此全局开关。',
            'params' => [
                'flag' => '传入 TRUE/FALSE 进行设置; 不传参数则只查询当前状态。',
            ],
            'returns' => '带参数时返回自身实例 (支持链式); 不带参数时返回当前开关状态 bool。',
            'example' => "class IndexController extends Yaf_Controller_Abstract {\n    public function init() {\n        if (\$this->getRequest()->isXmlHttpRequest()) {\n            // Ajax 请求关闭自动渲染, 手工输出 JSON\n            Yaf_Dispatcher::getInstance()->autoRender(false);\n        }\n    }\n}",
        ],
        '::flushInstantly' => [
            'desc' => '开关/查询"即时输出"。开启且自动渲染有效时, 渲染结果直接输出给请求端 (走 display), 而不是先追加到响应体再统一输出。默认关闭。',
            'params' => [
                'flag' => '传入 TRUE/FALSE 进行设置; 不传参数则只查询当前状态。',
            ],
            'returns' => '带参数时返回自身实例 (支持链式); 不带参数时返回当前开关状态 bool。',
            'example' => "\$app = new Yaf_Application(\"config.ini\");\nYaf_Dispatcher::getInstance()->flushInstantly(true);\n\$app->run();",
        ],
        '::getInstance' => [
            'desc' => '获取分发器单例。3.3.8 起要求 Yaf_Application 必须先初始化 (分发器由应用构造时创建), 否则抛出异常。',
            'returns' => 'Yaf_Dispatcher 实例。',
            'example' => "\$app = new Yaf_Application(\$config); // 必须先初始化应用\n\$dispatcher = Yaf_Dispatcher::getInstance();\nassert(\$dispatcher === \$app->getDispatcher());",
        ],
        '::dispatch' => [
            'desc' => '处理一个请求: 路由 (含插件 routerStartup/routerShutdown 钩子)、分发循环 (含 preDispatch/postDispatch 钩子)、渲染视图并组织响应。`Yaf_Application::run()` 内部调用的就是它; CLI 下需要手工构造请求时才直接使用。分发循环最多迭代 `yaf.forward_limit` 次 (默认 5, Controller::forward 会触发新一轮迭代); 注意 3.3.8 起该值为 0 或非正数时会被钳制回默认值 5, 不再表示"不限制"。超过上限抛出 Yaf_Exception_DispatchFailed。',
            'params' => [
                'request' => '要分发的 Yaf_Request_Abstract 实例, 会替换分发器当前的请求对象。',
            ],
            'returns' => '成功返回 Yaf_Response_Abstract 实例 (除非开启 returnResponse, 响应会自动输出); 失败返回 FALSE 或抛出异常。',
            'example' => "\$app = new Yaf_Application(array(\"application\" => array(\n    \"directory\" => realpath(dirname(__FILE__)),\n)));\n\n\$request = new Yaf_Request_Http(\"/index/index\");\ntry {\n    \$response = \$app->getDispatcher()->dispatch(\$request);\n} catch (Yaf_Exception \$e) {\n    echo \$e->getMessage();\n}",
        ],
        '::throwException' => [
            'desc' => '开关/查询 Yaf 出错时的报错方式。开启 (默认) 时抛出 Yaf_Exception; 关闭时触发 PHP 错误 (可配合 setErrorHandler 处理)。也可在配置中用 `application.dispatcher.throwException` 设置。',
            'params' => [
                'flag' => '传入 TRUE/FALSE 进行设置; 不传参数则只查询当前状态。',
            ],
            'returns' => '带参数时返回自身实例 (支持链式); 不带参数时返回当前开关状态 bool。',
            'example' => "\$app = new Yaf_Application(\"conf.ini\");\n// 关闭抛异常, 改为触发错误\nYaf_Dispatcher::getInstance()->throwException(false);",
        ],
        '::catchException' => [
            'desc' => '开关/查询默认异常捕获机制。开启后, 未捕获的异常会被转交给 Error 控制器的 errorAction 处理 (异常对象可通过 `$request->getException()` 获取)。默认关闭 (与旧手册"默认开启"的说法不符); 也可在配置中用 `application.dispatcher.catchException` 设置。',
            'params' => [
                'flag' => '传入 TRUE/FALSE 进行设置; 不传参数则只查询当前状态。',
            ],
            'returns' => '带参数时返回自身实例 (支持链式); 不带参数时返回当前开关状态 bool。',
            'example' => "\$app = new Yaf_Application(\"conf.ini\");\nYaf_Dispatcher::getInstance()->catchException(true);",
        ],
        '::registerPlugin' => [
            'desc' => '注册一个插件。插件按注册顺序在分发的各个钩子 (routerStartup、routerShutdown、preDispatch、postDispatch 等) 被回调。通常在 Bootstrap 的 `_init` 方法中注册, 插件类放在 `application.directory/plugins` 下。',
            'params' => [
                'plugin' => 'Yaf_Plugin_Abstract 派生类的实例。',
            ],
            'returns' => '返回自身实例, 支持链式调用。',
            'example' => "class Bootstrap extends Yaf_Bootstrap_Abstract {\n    public function _initPlugin(Yaf_Dispatcher \$dispatcher) {\n        \$user = new UserPlugin();\n        \$dispatcher->registerPlugin(\$user);\n    }\n}\n\nclass UserPlugin extends Yaf_Plugin_Abstract {\n    public function routerStartup(Yaf_Request_Abstract \$request, Yaf_Response_Abstract \$response) {\n    }\n}",
        ],
    ],
    'Yaf_Loader' => [
        '@class' => 'Yaf 的自动加载器, final 单例。它根据类名中的路径信息 (下划线/反斜线作目录分隔) 定位并加载类文件, 同时替代 require_once (不支持 require, 略快)。加载顺序: 以 Controller/Model/Plugin 结尾 (或开头, 取决于 `yaf.name_suffix`) 的类从应用目录的 controllers/models/plugins 下加载; 其余类先查已注册的本地命名空间, 命中则用本地类库目录 (默认 `application.directory/library`), 未命中则用全局类库目录 (`yaf.library`)。PHP 5.3+ 且开启 `yaf.use_namespace` 时也可写作 `Yaf\\Loader`。',
        '::__construct' => [
            'desc' => '私有构造方法, 禁止外部实例化。加载器是单例, 由扩展在首次需要时自动创建并注册为 SPL 自动加载函数, 请通过 `Yaf_Loader::getInstance()` 获取。',
            'returns' => '不可用。',
            'example' => "\$loader = Yaf_Loader::getInstance();",
        ],
        '::autoload' => [
            'desc' => '加载一个类, 是注册给 SPL 的自动加载回调, 也可手动调用。类名中的 `_` 和 `\\` 被视为目录分隔符; `yaf.lowcase_path` 开启时路径部分转小写。注意: 在默认的 `yaf.use_spl_autoload=0` 下, 即使类没找到也返回 TRUE (只触发警告), 以此截断后续自动加载器。',
            'params' => [
                'class_name' => '要加载的类名, 例如 `Foo_Bar_Dummy` 对应类库目录下的 `Foo/Bar/Dummy.php`。',
            ],
            'returns' => '默认 (use_spl_autoload=0) 总是返回 TRUE; 开启 `yaf.use_spl_autoload` 时, 加载成功返回 TRUE, 失败返回 FALSE 交由后续加载器处理。',
            'example' => "\$loader = Yaf_Loader::getInstance(\"/app/library\", \"/global/library\");\n// 本地类: /app/library/Foo/Bar.php\n\$loader->autoload(\"Foo_Bar\");\n// 未注册的类名走全局库: /global/library/Dummy.php\n\$loader->autoload(\"Dummy\");",
        ],
        '::getInstance' => [
            'desc' => '获取加载器单例。注意: 传入路径参数并不会创建新实例, 而是修改已有单例的本地/全局类库路径。',
            'params' => [
                'local_library_path' => '本地类库目录; 缺省时保持当前值 (应用初始化后默认为 `application.directory/library`)。',
                'global_library_path' => '全局类库目录; 缺省时保持当前值 (来自 php.ini 的 `yaf.library`)。',
            ],
            'returns' => 'Yaf_Loader 单例。',
            'example' => "\$loader = Yaf_Loader::getInstance();\nvar_dump(\$loader->getLibraryPath());\n\n// 显式指定本地和全局类库目录\n\$loader = Yaf_Loader::getInstance(\"/app/library\", \"/global/library\");",
        ],
        '::registerLocalNamespace' => [
            'desc' => '注册本地类名前缀 (命名空间), 使这些前缀的类从本地类库目录加载, 而不是全局类库目录。支持多级前缀 (如 `Foo_Bar`)。',
            'params' => [
                'namespace' => '字符串 (单个前缀) 或数组 (多个前缀; 数组带字符串键时, 键为前缀、值为该前缀专属的加载路径)。',
                'path' => '可选。为该前缀单独指定加载路径, 优先于本地类库目录。',
            ],
            'returns' => '成功返回自身实例; 参数既不是字符串也不是数组时返回 FALSE。',
            'example' => "\$loader = Yaf_Loader::getInstance();\n\$loader->registerLocalNamespace(\"Baidu\");\n\$loader->registerLocalNamespace(array(\"Sina\", \"Weibo\"));\n// 带专属路径: Vendor\\Foo 从 /path/to/vendor 加载\n\$loader->registerLocalNamespace(\"Vendor\", \"/path/to/vendor\");",
        ],
        '::getLocalNamespace' => [
            'desc' => '获取已注册的本地类前缀列表。',
            'returns' => '数组: 只注册了前缀的以数字下标列出, 指定了专属路径的以 "前缀 => 路径" 形式列出。',
            'example' => "\$loader = Yaf_Loader::getInstance();\n\$loader->registerLocalNamespace(array(\"Foo\", \"Bar\"));\nprint_r(\$loader->getLocalNamespace()); // [0 => \"Foo\", 1 => \"Bar\"]",
        ],
        '::clearLocalNamespace' => [
            'desc' => '清除所有已注册的本地类前缀。',
            'returns' => '成功返回 TRUE。',
            'example' => "\$loader = Yaf_Loader::getInstance();\n\$loader->registerLocalNamespace(\"Foo\");\n\$loader->clearLocalNamespace();\nvar_dump(\$loader->isLocalName(\"Foo_Bar\")); // false",
        ],
        '::isLocalName' => [
            'desc' => '判断一个类名是否属于已注册的本地前缀。按 `_` 或 `\\` 逐段匹配已注册的前缀 (支持多级)。',
            'params' => [
                'class_name' => '待判断的类名。',
            ],
            'returns' => '是本地类返回 TRUE, 否则返回 FALSE。',
            'example' => "\$loader = Yaf_Loader::getInstance();\n\$loader->registerLocalNamespace(\"Foo\");\nvar_dump(\$loader->isLocalName(\"Foo_Bar\"));  // true\nvar_dump(\$loader->isLocalName(\"FooBar\"));   // false",
        ],
        '::getNamespacePath' => [
            'desc' => '解析加载某个类时将使用的目录。',
            'params' => [
                'class_name' => '类名。',
            ],
            'returns' => '命中带专属路径的前缀时返回该路径; 命中普通本地前缀时返回本地类库目录; 未命中时返回全局类库目录 (未设置全局目录则回退到本地类库目录)。',
            'example' => "\$loader = Yaf_Loader::getInstance(\"/app/library\", \"/global/library\");\n\$loader->registerLocalNamespace(\"Vendor\", \"/path/to/vendor\");\necho \$loader->getNamespacePath(\"Vendor_Foo\"); // /path/to/vendor\necho \$loader->getNamespacePath(\"Local_Foo\");  // /app/library\necho \$loader->getNamespacePath(\"Other_Foo\");  // /global/library",
        ],
        '::import' => [
            'desc' => '静态方法, 加载一个 PHP 文件, 是 require_once 的高性能替代 (不做重复包含检查以外的事情)。文件已包含过时直接返回 TRUE, 不会重复执行。',
            'params' => [
                'file' => '文件路径。绝对路径直接使用; 相对路径以本地类库目录为基目录 (此时要求加载器已初始化, 即先创建过 Yaf_Application 或调用过 getInstance)。',
            ],
            'returns' => '加载成功 (或已包含过) 返回 TRUE; 文件不存在、路径非法或加载器未初始化时返回 FALSE。',
            'example' => "// 绝对路径\nYaf_Loader::import(\"/usr/local/foo.php\");\n// 相对路径, 在本地类库目录 (默认 application.directory/library) 下加载\nYaf_Loader::import(\"functions.php\");",
        ],
        '::setLibraryPath' => [
            'desc' => '设置类库目录。',
            'params' => [
                'library_path' => '类库目录路径。',
                'is_global' => 'TRUE 时设置全局类库目录, FALSE (默认) 时设置本地类库目录。',
            ],
            'returns' => '返回自身实例, 支持链式调用。',
            'example' => "\$loader = Yaf_Loader::getInstance();\n\$loader->setLibraryPath(\"/app/library\")\n       ->setLibraryPath(\"/global/library\", true);",
        ],
        '::getLibraryPath' => [
            'desc' => '获取类库目录。',
            'params' => [
                'is_global' => 'TRUE 时返回全局类库目录, FALSE (默认) 时返回本地类库目录。',
            ],
            'returns' => '目录字符串; 全局目录未设置且查询全局时返回空字符串。',
            'example' => "\$loader = Yaf_Loader::getInstance();\necho \$loader->getLibraryPath();       // 本地类库目录\necho \$loader->getLibraryPath(true);   // 全局类库目录 (yaf.library)",
        ],
        '::registerNamespace' => [
            'desc' => '`registerLocalNamespace()` 的别名。',
            'params' => [
                'namespace' => '字符串 (单个前缀) 或数组 (多个前缀; 数组带字符串键时, 键为前缀、值为该前缀专属的加载路径)。',
                'path' => '可选。为该前缀单独指定加载路径。',
            ],
            'returns' => '成功返回自身实例; 参数类型非法时返回 FALSE。',
            'example' => "\$loader = Yaf_Loader::getInstance();\n\$loader->registerNamespace(\"Foo\")->registerNamespace(array(\"Bar\", \"Baz\"));",
        ],
        '::getNamespaces' => [
            'desc' => '`getLocalNamespace()` 的别名。',
            'returns' => '已注册前缀的数组。',
            'example' => "print_r(Yaf_Loader::getInstance()->getNamespaces());",
        ],
    ],
];
