<?php
// Annotations for group: mvc.
return [
    'Yaf_Controller_Abstract' => [
        '@class' => '**Yaf_Controller_Abstract** 是 Yaf MVC 体系的核心部分。MVC（Model-View-Controller）是一种分离应用逻辑与表现逻辑的设计模式。用户通过继承该抽象类实现自己的控制器，控制器中定义的一个个动作（`xxxAction` 方法）是真正的执行体；动作也可以分离到独立的 `Yaf_Action_Abstract` 派生类中，通过控制器的 `$actions` 属性声明映射。与一般框架不同，Yaf 的动作可以定义参数，参数值来自路由结果中的同名参数（如 URL `/index/index/name/a` 对应 `indexAction($name)`）。注意这些参数来自用户请求，使用前必须做安全过滤；建议为参数定义默认值以避免缺参警告。控制器不可序列化、不可克隆。开启 `yaf.use_namespace` 时也可用 `Yaf\Controller_Abstract`。',
        '::__construct' => [
            'desc' => '控制器构造函数，由 Yaf 在派发时调用。构造前必须已初始化 `Yaf_Application`，否则抛出异常。构造时会从 Dispatcher 关联 request/response/view，并从请求中取出控制器名与模块名，若子类定义了 `init()` 方法则随后调用它（相当于控制器级的初始化钩子）。',
            'returns' => '无返回值（构造函数）',
            'example' => "class IndexController extends Yaf_Controller_Abstract {\n    public function init() {\n        // 构造完成后由框架自动调用，可做控制器级初始化\n        \$this->getView()->assign('base', '/static');\n    }\n}",
        ],
        '::render' => [
            'desc' => '渲染动作对应的视图模板并返回渲染结果字符串（不输出）。模板路径由动作名拼成 `<控制器名>/<动作名>.<视图后缀>`（控制器名小写，下划线转换为目录分隔符），在当前视图目录下查找。这是对 `Yaf_View_Interface::render` 的包装。',
            'params' => [
                'tpl' => '要渲染的动作名（非模板文件名）',
                'parameters' => '传递给视图引擎的模板变量，也可事先用 `Yaf_View_Interface::assign` 分配',
            ],
            'returns' => '成功返回渲染结果字符串，失败返回 FALSE',
            'example' => "class IndexController extends Yaf_Controller_Abstract {\n    public function indexAction() {\n        Yaf_Dispatcher::getInstance()->disableView(); // 关闭自动渲染\n        echo \$this->render('index', ['name' => 'value']);\n        return false; // 阻止后续自动渲染\n    }\n}",
        ],
        '::display' => [
            'desc' => '渲染动作对应的视图模板并直接输出结果（不返回内容）。模板路径的确定方式与 `render` 相同。这是对 `Yaf_View_Interface::display` 的包装。',
            'params' => [
                'tpl' => '要渲染的动作名（非模板文件名）',
                'parameters' => '传递给视图引擎的模板变量，也可事先用 `Yaf_View_Interface::assign` 分配',
            ],
            'returns' => '成功返回 TRUE，失败返回 FALSE',
            'example' => "class IndexController extends Yaf_Controller_Abstract {\n    public function indexAction() {\n        Yaf_Dispatcher::getInstance()->disableView();\n        \$this->display('index', ['name' => 'value']);\n        return false;\n    }\n}",
        ],
        '::getRequest' => [
            'desc' => '获取当前请求实例。',
            'returns' => '当前的 `Yaf_Request_Abstract` 实例；在 Application 未初始化等特殊情况下返回 NULL',
            'example' => "class IndexController extends Yaf_Controller_Abstract {\n    public function indexAction() {\n        \$name = \$this->getRequest()->getParam('name');\n    }\n}",
        ],
        '::getResponse' => [
            'desc' => '获取当前响应实例。',
            'returns' => '当前的 `Yaf_Response_Abstract` 实例；未初始化时返回 NULL',
            'example' => "class IndexController extends Yaf_Controller_Abstract {\n    public function indexAction() {\n        \$this->getResponse()->setBody('hello', 'content');\n    }\n}",
        ],
        '::getView' => [
            'desc' => '获取当前视图引擎实例。Yaf 采用延迟实例化策略，视图引擎在首次调用 `getView` 或 `initView` 之后才可用。',
            'returns' => '`Yaf_View_Interface` 实例；未初始化时返回 NULL',
            'example' => "class IndexController extends Yaf_Controller_Abstract {\n    public function indexAction() {\n        \$view = \$this->getView();\n        \$view->assign('foo', 'bar');\n    }\n}",
        ],
        '::getName' => [
            'desc' => '获取当前控制器名（来自请求路由结果，保持原始大小写）。',
            'returns' => '控制器名字符串；未设置时返回 NULL',
        ],
        '::getModuleName' => [
            'desc' => '获取当前控制器所属的模块名。',
            'returns' => '模块名字符串；未设置时返回 NULL',
            'example' => "class IndexController extends Yaf_Controller_Abstract {\n    public function init() {\n        if (\$this->getModuleName() !== 'Index') {\n            // 按模块做差异化初始化\n        }\n    }\n}",
        ],
        '::initView' => [
            'desc' => '初始化并返回视图引擎。由于 Yaf 延迟实例化视图引擎，在通过属性方式访问视图（如 `$this->_view`）前应先调用此方法确保其已实例化。注意：3.3.8 中该方法实际只是返回 Dispatcher 已持有的视图实例，`options` 参数被接受但不使用。',
            'params' => [
                'options' => '视图引擎选项（当前版本未使用）',
            ],
            'returns' => '`Yaf_View_Interface` 实例；视图未初始化时返回 NULL',
            'example' => "class IndexController extends Yaf_Controller_Abstract {\n    public function init() {\n        \$view = \$this->initView();\n        \$view->assign('webroot', 'http://domain.com/');\n    }\n}",
        ],
        '::setViewpath' => [
            'desc' => '更改当前控制器的视图模板目录，之后 `render`/`display` 将在新目录下查找模板。内部委托给视图引擎的 `setScriptPath`。',
            'params' => [
                'view_directory' => '视图模板目录，应为绝对路径',
            ],
            'returns' => '成功返回 TRUE；视图引擎未初始化时返回 FALSE',
            'example' => "class IndexController extends Yaf_Controller_Abstract {\n    public function init() {\n        \$this->initView();\n        \$this->setViewpath('/usr/local/www/tpl/');\n    }\n}",
        ],
        '::getViewpath' => [
            'desc' => '获取当前控制器的视图模板目录。',
            'returns' => '模板目录字符串；视图引擎未初始化或未设置时返回空字符串',
        ],
        '::forward' => [
            'desc' => '把当前请求转交给另一个动作处理。该方法只登记转发目的地（设置请求的 module/controller/action 及参数并重置 dispatched 标记），不会立即跳转；当前动作执行完毕后 Yaf 才会发起新一轮 dispatch。支持多种参数形式：`forward($action)`；`forward($controller, $action)` 或 `forward($action, array $invoke_args)`；`forward($module, $controller, $action)` 或 `forward($controller, $action, array $invoke_args)`；以及完整的四参数形式。',
            'params' => [
                'args1' => '1 参形式：目标动作名；2/3/4 参形式：目标模块名',
                'args2' => '目标控制器名，或 2 参形式下的调用参数数组',
                'args3' => '目标动作名，或 3 参形式下的调用参数数组',
                'args4' => '调用参数数组，可通过 `Yaf_Request_Abstract::getParam` 获取',
            ],
            'returns' => '成功返回 TRUE，失败返回 FALSE',
            'example' => "class IndexController extends Yaf_Controller_Abstract {\n    public function indexAction() {\n        if (!\$user_logged_in) {\n            \$this->forward('login');           // 转发到当前控制器的 loginAction\n            // \$this->forward('User', 'login', ['from' => 'index']);\n        }\n    }\n}",
        ],
        '::redirect' => [
            'desc' => '重定向请求到新的 URL。内部通过响应对象发送 Location 头实现。',
            'params' => [
                'url' => '要定向到的目标 URL',
            ],
            'returns' => '始终返回 TRUE',
            'example' => "class IndexController extends Yaf_Controller_Abstract {\n    public function indexAction() {\n        if (!\$user_logged_in) {\n            \$this->redirect('/login/');\n        }\n    }\n}",
        ],
        '::getInvokeArgs' => [
            'desc' => '获取通过 `forward` 传递来的全部调用参数。',
            'returns' => '调用参数关联数组的副本；无参数时返回 NULL',
        ],
        '::getInvokeArg' => [
            'desc' => '获取通过 `forward` 传递来的单个调用参数。',
            'params' => [
                'name' => '参数名',
            ],
            'returns' => '参数值；不存在时返回 NULL',
            'example' => "class IndexController extends Yaf_Controller_Abstract {\n    public function dummyAction() {\n        \$from = \$this->getInvokeArg('from'); // 来自 forward 时附加的参数\n    }\n}",
        ],
    ],
    'Yaf_Action_Abstract' => [
        '@class' => '**Yaf_Action_Abstract** 是 MVC 中控制器动作的基类，继承自 `Yaf_Controller_Abstract`。一般而言动作定义在控制器类中，但为了拆分较大的控制器、使代码更清晰，也可以把动作分离成独立的 `Yaf_Action_Abstract` 派生类，并在控制器的 `$actions` 属性中声明映射（如 `public $actions = ["index" => "actions/Index.php"]`），Yaf 会在路由到该动作时加载对应文件并调用其 `execute` 方法。开启 `yaf.use_namespace` 时也可用 `Yaf\Action_Abstract`。',
        '::execute' => [
            'desc' => '抽象方法，动作的执行体，由用户继承实现。Yaf 路由到该动作时调用。可定义参数以接收路由结果中的同名参数。',
            'returns' => '返回 FALSE 可阻止自动渲染（由实现决定）',
            'example' => "class IndexAction extends Yaf_Action_Abstract {\n    public function execute(\$name = null) {\n        echo \$this->getControllerName(); // Index\n        return false; // 关闭本动作的自动渲染\n    }\n}",
        ],
        '::getController' => [
            'desc' => '获取该动作所属的控制器实例。',
            'returns' => '`Yaf_Controller_Abstract` 实例；未关联控制器时返回 NULL',
            'example' => "class IndexAction extends Yaf_Action_Abstract {\n    public function execute() {\n        \$this->getController()->getView()->assign('foo', 'bar');\n    }\n}",
        ],
        '::getControllerName' => [
            'desc' => '获取该动作所属的控制器名。',
            'returns' => '控制器名字符串；未关联时返回 NULL',
        ],
    ],
    'Yaf_View_Interface' => [
        '@class' => '**Yaf_View_Interface** 是 Yaf 的视图引擎接口，定义了接入 Yaf 的视图引擎需要实现的方法与功能。实现该接口即可接入自定义模板引擎（如 Smarty），Yaf 内置的实现是 `Yaf_View_Simple`。开启 `yaf.use_namespace` 时也可用 `Yaf\View_Interface`。',
        '::assign' => [
            'desc' => '视图引擎契约：分配模板变量。分配的变量在模板渲染时可用。',
            'params' => [
                'name' => '变量名',
                'value' => '变量值',
            ],
            'returns' => '实现自定，约定成功返回视图引擎自身（便于链式调用），失败返回 FALSE',
        ],
        '::display' => [
            'desc' => '视图引擎契约：渲染模板脚本并直接输出结果。',
            'params' => [
                'tpl' => '模板脚本路径',
                'tpl_vars' => '本次渲染附加的模板变量',
            ],
            'returns' => '实现自定，约定成功返回 TRUE，失败返回 FALSE',
        ],
        '::render' => [
            'desc' => '视图引擎契约：渲染模板脚本并返回结果字符串，不输出。Yaf 的自动渲染机制依赖此方法返回渲染内容。',
            'params' => [
                'tpl' => '模板脚本路径',
                'tpl_vars' => '本次渲染附加的模板变量',
            ],
            'returns' => '实现自定，约定成功返回渲染结果字符串，失败返回 FALSE',
        ],
        '::setScriptPath' => [
            'desc' => '视图引擎契约：设置模板脚本的基目录。Yaf 渲染相对路径模板时会以此目录为基准拼接。',
            'params' => [
                'template_dir' => '模板基目录，应为绝对路径',
            ],
            'returns' => '实现自定，约定成功返回 TRUE（`Yaf_View_Simple` 返回自身），失败返回 FALSE',
        ],
        '::getScriptPath' => [
            'desc' => '视图引擎契约：获取当前模板脚本的基目录。',
            'returns' => '模板基目录字符串',
        ],
    ],
    'Yaf_View_Simple' => [
        '@class' => '**Yaf_View_Simple** 是 Yaf 内置的视图引擎，追求性能，不提供 Smarty 那样丰富的功能与复杂语法。模板就是普通 PHP 脚本：通过 `assign` 分配的模板变量在模板中可直接以同名变量（`$name`）使用；模板内 `$this` 指向视图引擎自身。开启 `yaf.use_namespace` 时也可用 `Yaf\View\Simple`。',
        '::__construct' => [
            'desc' => '构造 `Yaf_View_Simple` 实例。模板目录必须是绝对路径，否则触发 Yaf 类型错误。注意：源码 stub 中第一个参数名拼写为 `$tempalte_dir`（template 的笔误），属源码现状。`options` 参数当前未被使用。',
            'params' => [
                'tempalte_dir' => '模板基目录，必须为绝对路径',
                'options' => '选项数组（当前版本未使用）',
            ],
            'returns' => '无返回值（构造函数）',
            'example' => "\$view = new Yaf_View_Simple('/usr/local/www/views/');",
        ],
        '::get' => [
            'desc' => '获取已分配的模板变量值。不传参数或传空时返回全部模板变量。`__get` 是它的别名（`$view->name` 等价于 `$view->get("name")`）。',
            'params' => [
                'name' => '模板变量名；省略或传 NULL/空字符串时返回全部变量',
            ],
            'returns' => '指定变量的值，不存在返回 NULL；不传变量名时返回全部模板变量的数组副本',
            'example' => "\$view->assign('name', 'value');\necho \$view->get('name');     // value\nprint_r(\$view->get());       // 全部模板变量",
        ],
        '::assign' => [
            'desc' => '为视图引擎分配模板变量，在模板中可直接以同名变量使用。支持两种形式：`assign($name, $value)` 分配单个变量；`assign(array $vars)` 批量分配。变量名若不是合法 PHP 变量名（如整数、含特殊字符），模板中不能直接以 `$name` 访问，需通过 `$this->_tpl_vars` 访问。',
            'params' => [
                'name' => '变量名字符串，或变量名=>值的关联数组（此时第二个参数须省略）',
                'default' => '变量值；第一个参数为数组时须省略',
            ],
            'returns' => '返回视图引擎自身（`Yaf_View_Simple`），支持链式调用；参数错误时返回 FALSE',
            'example' => "\$view->assign(['name' => 'value'])\n     ->assign('foo', 'bar');",
        ],
        '::render' => [
            'desc' => '渲染模板文件并返回结果字符串（不输出）。模板路径为相对路径时与 `setScriptPath` 设置的基目录拼接；绝对路径则直接使用。模板文件不存在时触发 Yaf 错误。`tpl_vars` 中与已分配变量同名的会覆盖后者。',
            'params' => [
                'tpl' => '模板文件路径（相对模板基目录或绝对路径）',
                'tpl_vars' => '本次渲染附加的模板变量，同名覆盖已 assign 的变量',
            ],
            'returns' => '成功返回渲染结果字符串，失败返回 FALSE',
            'example' => "\$view = new Yaf_View_Simple('/usr/local/www/views/');\n\$view->assign('name', 'yaf');\necho \$view->render('index/index.phtml');",
        ],
        '::eval' => [
            'desc' => '渲染模板字符串（而非模板文件）并返回结果，模板变量用法与 `render` 相同。空字符串返回 NULL。',
            'params' => [
                'tpl_str' => '模板内容字符串（PHP 代码，无需包裹在 PHP 标签中）',
                'vars' => '本次渲染附加的模板变量',
            ],
            'returns' => '成功返回渲染结果字符串；模板字符串为空时返回 NULL',
            'example' => "\$view = new Yaf_View_Simple('/usr/local/www/views/');\necho \$view->eval('<p>Hello, <?php echo \$name; ?></p>', ['name' => 'yaf']);\n// 输出: <p>Hello, yaf</p>",
        ],
        '::display' => [
            'desc' => '渲染模板文件并直接输出结果（不返回内容）。路径解析规则与 `render` 相同。',
            'params' => [
                'tpl' => '模板文件路径（相对模板基目录或绝对路径）',
                'tpl_vars' => '本次渲染附加的模板变量',
            ],
            'returns' => '成功返回 TRUE，失败返回 FALSE',
            'example' => "\$view = new Yaf_View_Simple('/usr/local/www/views/');\n\$view->display('index/index.phtml', ['name' => 'yaf']);",
        ],
        '::assignRef' => [
            'desc' => '分配模板变量，接口上与 `assign` 等价（当前源码实现为值拷贝，并无真正的引用绑定语义），返回视图引擎自身以支持链式调用。',
            'params' => [
                'name' => '变量名',
                'value' => '变量值',
            ],
            'returns' => '返回视图引擎自身（`Yaf_View_Simple`）',
        ],
        '::clear' => [
            'desc' => '清除已分配的模板变量。传入变量名则只清除该变量，不传则清除全部。',
            'params' => [
                'name' => '要清除的变量名；省略或传 NULL 时清除全部模板变量',
            ],
            'returns' => '返回视图引擎自身（`Yaf_View_Simple`）',
            'example' => "\$view->assign(['a' => 1, 'b' => 2]);\n\$view->clear('a'); // 只清除 a\n\$view->clear();    // 清除全部",
        ],
        '::setScriptPath' => [
            'desc' => '设置模板基目录，之后 `render`/`display` 相对路径模板都在此目录下查找。默认为 `APPLICATION_PATH . "/views"`，由 Dispatcher 设置。目录必须为绝对路径。',
            'params' => [
                'template_dir' => '模板基目录，必须为绝对路径',
            ],
            'returns' => '成功返回视图引擎自身（`Yaf_View_Simple`，支持链式调用）；非绝对路径时返回 FALSE',
            'example' => "\$this->getView()->setScriptPath('/tmp/views/');",
        ],
        '::getScriptPath' => [
            'desc' => '获取当前模板基目录。',
            'returns' => '模板基目录字符串；未设置时返回 NULL',
        ],
        '::__isset' => [
            'desc' => '检测模板变量是否已分配，支撑对视图对象的 `isset()`/`empty()` 用法。',
            'params' => [
                'name' => '模板变量名',
            ],
            'returns' => '已分配返回 TRUE，否则 FALSE',
            'example' => "if (isset(\$view->name)) {\n    echo \$view->name;\n}",
        ],
        '::__set' => [
            'desc' => '通过属性赋值的方式分配模板变量，等价于 `assign`，在模板中可直接以同名变量使用。',
            'params' => [
                'name' => '变量名',
                'value' => '变量值',
            ],
            'returns' => '无返回值',
            'example' => "\$view = \$this->getView();\n\$view->name = 'value'; // 模板中可直接用 \$name",
        ],
        '::__get' => [
            'desc' => '通过属性访问的方式获取模板变量值，是 `get` 的实现别名（`$view->name` 等价于 `$view->get("name")`）。',
            'params' => [
                'name' => '模板变量名',
            ],
            'returns' => '变量值；未分配时返回 NULL',
            'example' => "\$this->initView();\necho \$this->_view->name;",
        ],
    ],
];