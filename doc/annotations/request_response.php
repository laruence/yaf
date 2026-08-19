<?php
// Annotations for group: request_response. Generated from C source (yaf_request.c/h, yaf_response.c/h) + manual.
return [
    'Yaf_Request_Abstract' => [
        '@class' => '请求对象的抽象基类，封装了请求方法、请求 URI、路由结果（module/controller/action/路由参数）以及对 $_GET/$_POST/$_SERVER 等超全局变量的访问。实际使用中由 Yaf 框架根据 SAPI 自动创建具体子类：HTTP 环境为 Yaf_Request_Http，CLI/测试场景可用 Yaf_Request_Simple。该类不可序列化、不可克隆。',
        '::isGet' => [
            'desc' => '判断当前请求是否为 GET 请求。内部将请求方法与 "GET" 做不区分大小写的比较。',
            'returns' => '是 GET 请求返回 TRUE，否则返回 FALSE',
            'example' => '$request = Yaf_Dispatcher::getInstance()->getRequest();
if ($request->isGet()) {
    echo "running in Get mode";
}',
        ],
        '::isPost' => [
            'desc' => '判断当前请求是否为 POST 请求。',
            'returns' => '是 POST 请求返回 TRUE，否则返回 FALSE',
            'example' => 'if ($this->getRequest()->isPost()) {
    // 处理表单提交
}',
        ],
        '::isDelete' => [
            'desc' => '判断当前请求是否为 DELETE 请求。',
            'returns' => '是 DELETE 请求返回 TRUE，否则返回 FALSE',
        ],
        '::isPatch' => [
            'desc' => '判断当前请求是否为 PATCH 请求。',
            'returns' => '是 PATCH 请求返回 TRUE，否则返回 FALSE',
        ],
        '::isPut' => [
            'desc' => '判断当前请求是否为 PUT 请求。',
            'returns' => '是 PUT 请求返回 TRUE，否则返回 FALSE',
        ],
        '::isHead' => [
            'desc' => '判断当前请求是否为 HEAD 请求。',
            'returns' => '是 HEAD 请求返回 TRUE，否则返回 FALSE',
        ],
        '::isOptions' => [
            'desc' => '判断当前请求是否为 OPTIONS 请求。',
            'returns' => '是 OPTIONS 请求返回 TRUE，否则返回 FALSE',
        ],
        '::isCli' => [
            'desc' => '判断当前请求是否为 CLI 请求。',
            'returns' => '是 CLI 请求返回 TRUE，否则返回 FALSE',
            'example' => 'if ($this->getRequest()->isCli()) {
    echo "running in Cli mode";
}',
        ],
        '::isXmlHttpRequest' => [
            'desc' => '判断是否为 Ajax 请求：检查 $_SERVER["HTTP_X_REQUESTED_WITH"] 是否为 "XMLHttpRequest"（值比较不区分大小写）。Yaf_Request_Simple 覆写了该方法，恒返回 FALSE。',
            'returns' => '是 Ajax 请求返回 TRUE，否则返回 FALSE',
            'example' => 'if ($this->getRequest()->isXmlHttpRequest()) {
    $this->getResponse()->setBody(json_encode($data));
}',
        ],
        '::getQuery' => [
            'desc' => '读取 $_GET 中的值。不传参数时返回整个 $_GET 数组；name 键查找区分大小写。',
            'params' => [
                'name' => '要获取的键名；不传则返回整个 $_GET 数组',
                'default' => '键不存在时返回的默认值，未设置则返回 NULL',
            ],
            'returns' => '找到返回对应值；未找到且设置了 default 返回 default；否则返回 NULL',
            'example' => '$page = $this->getRequest()->getQuery("page", 1);',
        ],
        '::getRequest' => [
            'desc' => '读取 $_REQUEST 中的值。不传参数时返回整个 $_REQUEST 数组；name 键查找区分大小写。',
            'params' => [
                'name' => '要获取的键名；不传则返回整个 $_REQUEST 数组',
                'default' => '键不存在时返回的默认值，未设置则返回 NULL',
            ],
            'returns' => '找到返回对应值；未找到且设置了 default 返回 default；否则返回 NULL',
        ],
        '::getPost' => [
            'desc' => '读取 $_POST 中的值。不传参数时返回整个 $_POST 数组；name 键查找区分大小写。',
            'params' => [
                'name' => '要获取的键名；不传则返回整个 $_POST 数组',
                'default' => '键不存在时返回的默认值，未设置则返回 NULL',
            ],
            'returns' => '找到返回对应值；未找到且设置了 default 返回 default；否则返回 NULL',
            'example' => '$title = $this->getRequest()->getPost("title", "");',
        ],
        '::getCookie' => [
            'desc' => '读取 $_COOKIE 中的值。不传参数时返回整个 $_COOKIE 数组；name 键查找区分大小写。',
            'params' => [
                'name' => '要获取的键名；不传则返回整个 $_COOKIE 数组',
                'default' => '键不存在时返回的默认值，未设置则返回 NULL',
            ],
            'returns' => '找到返回对应值；未找到且设置了 default 返回 default；否则返回 NULL',
            'example' => '$uid = $this->getRequest()->getCookie("uid", 0);',
        ],
        '::getRaw' => [
            'desc' => '读取原始请求体（raw body），常用于 JSON API 等场景。内部对请求体流做 rewind 后整体读取。',
            'returns' => '返回原始请求体字符串；请求体为空或无法读取时返回 FALSE',
            'example' => '$payload = json_decode($this->getRequest()->getRaw(), true);',
        ],
        '::getFiles' => [
            'desc' => '读取 $_FILES 中的值。不传参数时返回整个 $_FILES 数组；name 键查找区分大小写。',
            'params' => [
                'name' => '要获取的键名；不传则返回整个 $_FILES 数组',
                'default' => '键不存在时返回的默认值，未设置则返回 NULL',
            ],
            'returns' => '找到返回对应值；未找到且设置了 default 返回 default；否则返回 NULL',
        ],
        '::get' => [
            'desc' => '按固定顺序查找一个键：路由参数 → $_POST → $_GET → $_COOKIE → $_SERVER，命中即返回（不查 $_ENV）。',
            'params' => [
                'name' => '要获取的键名',
                'default' => '全部来源都未找到时返回的默认值，未设置则返回 NULL',
            ],
            'returns' => '按上述顺序返回第一个命中的值；都未命中返回 default 或 NULL',
            'example' => '// 一次调用即可兼容路由参数、POST、GET、COOKIE、SERVER 多种来源
$value = $this->getRequest()->get("token", "anonymous");',
        ],
        '::getServer' => [
            'desc' => '读取 $_SERVER 中的值。不传参数时返回整个 $_SERVER 数组；name 键查找区分大小写，HTTP 头需使用 HTTP_XXX 大写形式（如 HTTP_HOST）。',
            'params' => [
                'name' => '要获取的键名；不传则返回整个 $_SERVER 数组',
                'default' => '键不存在时返回的默认值，未设置则返回 NULL',
            ],
            'returns' => '找到返回对应值；未找到且设置了 default 返回 default；否则返回 NULL',
            'example' => '$host = $this->getRequest()->getServer("HTTP_HOST");',
        ],
        '::getEnv' => [
            'desc' => '读取 $_ENV 中的值。不传参数时返回整个 $_ENV 数组；name 键查找区分大小写。',
            'params' => [
                'name' => '要获取的键名；不传则返回整个 $_ENV 数组',
                'default' => '键不存在时返回的默认值，未设置则返回 NULL',
            ],
            'returns' => '找到返回对应值；未找到且设置了 default 返回 default；否则返回 NULL',
        ],
        '::setParam' => [
            'desc' => '设置路由参数。支持两种调用方式：setParam($name, $value) 设置单个；setParam($array) 批量设置（只处理字符串键）。路由参数不同于 $_GET/$_POST，它是路由协议从 Request URI 中解析出来的参数，也可由代码手动设置。',
            'params' => [
                'name' => '路由参数名（字符串）；或传入数组表示批量设置',
                'value' => '路由参数值；批量模式下省略',
            ],
            'returns' => '成功返回 Yaf_Request_Abstract 实例自身（可链式调用），失败返回 FALSE',
            'example' => '$this->getRequest()->setParam("userid", 0);
$this->getRequest()->setParam(["age" => 28, "gender" => "man"]);',
        ],
        '::getParam' => [
            'desc' => '获取当前请求中的路由参数。路由参数不是 $_GET 或 $_POST，而是路由协议根据 Request URI 分析出的请求参数。比如默认路由 Yaf_Route_Static 路由 http://www.domain.com/module/controller/action/name1/value1/ 后，会得到路由参数 name1，值为 value1。路由参数来自用户输入，使用前需做安全过滤。',
            'params' => [
                'name' => '要获取的路由参数名',
                'default' => '如果没有找到该路由参数，则返回此默认值；未设置则返回 NULL',
            ],
            'returns' => '找到返回对应的路由参数值；未找到且设置了 default 返回 default；否则返回 NULL',
            'example' => 'class IndexController extends Yaf_Controller_Abstract {
    public function indexAction() {
        echo "user id:" . $this->getRequest()->getParam("userid", 0);
    }
}',
        ],
        '::getParams' => [
            'desc' => '获取当前请求中的所有路由参数。路由参数是路由协议根据 Request URI 分析出的参数，不是 $_GET/$_POST，来自用户输入，使用前需做安全过滤。',
            'returns' => '当前所有路由参数组成的数组；没有任何路由参数时返回空数组',
            'example' => 'var_dump($this->getRequest()->getParams());',
        ],
        '::clearParams' => [
            'desc' => '清空当前请求的所有路由参数。',
            'returns' => '返回 Yaf_Request_Abstract 实例自身（可链式调用）',
        ],
        '::getException' => [
            'desc' => '在异常捕获模式（application.dispatcher.catchException 开启）下，当流程进入 ErrorController 的 error 动作时，用本方法获取当前发生的异常对象。实现上是读取名为 "exception" 的路由参数。',
            'returns' => '有异常时返回当前异常对象，没有异常时返回 NULL',
            'example' => 'class ErrorController extends Yaf_Controller_Abstract {
    public function errorAction() {
        $exception = $this->getRequest()->getException();
    }
}',
        ],
        '::getModuleName' => [
            'desc' => '获取当前请求被路由到的模块名。',
            'returns' => '路由成功后返回当前处理该请求的模块名；路由之前返回 NULL',
            'example' => 'echo "current Module:" . $this->getRequest()->getModuleName();',
        ],
        '::getControllerName' => [
            'desc' => '获取当前请求被路由到的控制器名。',
            'returns' => '路由成功后返回当前处理该请求的控制器名（CamelCase 风格）；路由之前返回 NULL',
            'example' => 'echo "current Controller:" . $this->getRequest()->getControllerName();',
        ],
        '::getActionName' => [
            'desc' => '获取当前请求被路由到的动作（Action）名。',
            'returns' => '路由成功后返回当前处理该请求的动作名（小写）；路由之前返回 NULL',
            'example' => 'echo "current Action:" . $this->getRequest()->getActionName();',
        ],
        '::setModuleName' => [
            'desc' => '设置请求将被分发到的模块名。format_name 为 TRUE 时按 CamelCase 格式化后保存；为 FALSE 时原样保存。传入空字符串不做任何修改。',
            'params' => [
                'module' => '模块名',
                'format_name' => '是否格式化模块名，默认 TRUE',
            ],
            'returns' => '返回 Yaf_Request_Abstract 实例自身（可链式调用）',
        ],
        '::setControllerName' => [
            'desc' => '设置请求将被分发到的控制器名。format_name 为 TRUE 时按 CamelCase 格式化后保存；为 FALSE 时原样保存。传入空字符串不做任何修改。',
            'params' => [
                'controller' => '控制器名',
                'format_name' => '是否格式化控制器名，默认 TRUE',
            ],
            'returns' => '返回 Yaf_Request_Abstract 实例自身（可链式调用）',
        ],
        '::setActionName' => [
            'desc' => '设置请求将被分发到的动作名。format_name 为 TRUE 时转为小写后保存；为 FALSE 时原样保存。传入空字符串不做任何修改。',
            'params' => [
                'action' => '动作名',
                'format_name' => '是否格式化动作名，默认 TRUE',
            ],
            'returns' => '返回 Yaf_Request_Abstract 实例自身（可链式调用）',
        ],
        '::getMethod' => [
            'desc' => '获取当前请求的方法类型，可能的返回值如 GET、POST、HEAD、PUT、CLI 等。HTTP 环境取自 SAPI 请求方法，CLI 环境下为 "CLI"。',
            'returns' => '当前请求的方法字符串（原样返回，不做大小写转换）',
            'example' => 'if ($this->getRequest()->getMethod() == "CLI") {
    echo "running in cli mode";
}',
        ],
        '::getLanguage' => [
            'desc' => '获取客户端首选语言：解析 $_SERVER["HTTP_ACCEPT_LANGUAGE"]，按 q 值取权重最高的语言标签；首次解析后结果被缓存。',
            'returns' => '首选语言字符串（如 zh-CN）；无法获取时返回 NULL',
        ],
        '::setBaseUri' => [
            'desc' => '设置基础 URI。路由时该前缀会从请求 URI 中被剥离（剥离比较不区分大小写）。传入的 URI 尾部斜杠会被去除，空字符串会被拒绝。',
            'params' => [
                'uir' => '基础 URI，如 "/app"',
            ],
            'returns' => '成功返回 Yaf_Request_Abstract 实例自身；URI 为空时返回 FALSE',
            'example' => '$request->setBaseUri("/app")->setRequestUri("/app/user/list");',
        ],
        '::getBaseUri' => [
            'desc' => '获取之前设置（或构造时推断）的基础 URI。',
            'returns' => '基础 URI 字符串；未设置时返回空字符串',
        ],
        '::getRequestUri' => [
            'desc' => '获取当前请求的 URI，路由即基于该值进行。HTTP 环境下通常由 Yaf 在构造时自动设置。',
            'returns' => '请求 URI 字符串；未设置时返回空字符串',
        ],
        '::setRequestUri' => [
            'desc' => '设置请求 URI，路由将基于该值进行。常用于改写路由前的 URI。',
            'params' => [
                'uir' => '请求 URI',
            ],
            'returns' => '返回 Yaf_Request_Abstract 实例自身（可链式调用）',
        ],
        '::isDispatched' => [
            'desc' => '判断请求是否已被分发（final 方法）。分发循环中若该标记为 TRUE，Yaf_Dispatcher 会终止循环。',
            'returns' => '已分发返回 TRUE，否则返回 FALSE',
        ],
        '::setDispatched' => [
            'desc' => '设置请求的分发状态（final 方法）。在插件中将其置为 TRUE 可以阻止 Yaf_Dispatcher 继续分发。',
            'params' => [
                'flag' => '分发状态，默认 TRUE',
            ],
            'returns' => '返回 Yaf_Request_Abstract 实例自身（可链式调用）',
        ],
        '::isRouted' => [
            'desc' => '判断请求是否已完成路由（final 方法）。',
            'returns' => '已路由返回 TRUE，否则返回 FALSE',
        ],
        '::setRouted' => [
            'desc' => '设置请求的路由状态（final 方法）。自定义路由协议匹配成功后应调用本方法，Yaf_Dispatcher 将跳过后续路由流程。',
            'params' => [
                'flag' => '路由状态，默认 TRUE',
            ],
            'returns' => '返回 Yaf_Request_Abstract 实例自身（可链式调用）',
            'example' => '// 在自定义 Route 的 route() 方法中
$request->setControllerName("product")
        ->setActionName("detail")
        ->setRouted();',
        ],
    ],
    'Yaf_Request_Http' => [
        '@class' => 'HTTP 环境下的请求对象，继承自 Yaf_Request_Abstract。Web 请求时由 Yaf 框架自动创建，也可手动构造（常用于测试）。构造时自动确定请求方法和请求 URI；另定义了 SCHEME_HTTP、SCHEME_HTTPS 两个常量。',
        '::__construct' => [
            'desc' => '构造 HTTP 请求对象。requestUri 省略时，Yaf 依次尝试从 $_SERVER 的 PATH_INFO、REQUEST_URI、ORIG_PATH_INFO 中探测请求 URI；baseUri 省略时会根据 SCRIPT_NAME/PHP_SELF/ORIG_SCRIPT_NAME 自动推断，传入的 baseUri 尾部斜杠会被去除。请求方法取自 SAPI，无法获取时为 "CLI"。',
            'params' => [
                'requestUri' => '请求 URI，如 "/user/list"；省略时自动探测',
                'baseUri' => '基础 URI，如 "/app"；省略时自动推断',
            ],
            'returns' => '无返回值（构造方法）',
            'example' => '$request = new Yaf_Request_Http("/user/list", "/app");
echo $request->getRequestUri(); // /user/list
echo $request->getBaseUri();    // /app',
        ],
    ],
    'Yaf_Request_Simple' => [
        '@class' => '简单请求对象，继承自 Yaf_Request_Abstract，主要用于 CLI 脚本和单元测试。可以直接指定请求方法、模块、控制器、动作和路由参数来构造一个请求；未提供 MVC 信息时，会从 $_SERVER["argv"] 中查找形如 request_uri=xxx 的参数作为请求 URI 交给路由器解析。',
        '::__construct' => [
            'desc' => '构造简单请求对象。两种模式：传入 module/controller/action 中任意一个时，请求直接标记为已路由（routed），缺省部分用默认值补齐（模块/控制器默认 Index、动作默认 index），名字会被格式化（模块/控制器转 CamelCase、动作转小写）；全部省略时扫描 $_SERVER["argv"] 中的 request_uri= 字符串作为请求 URI，等待路由。',
            'params' => [
                'method' => '请求方法，如 "CLI"、"GET"；省略时取自 SAPI',
                'module' => '模块名',
                'controller' => '控制器名',
                'action' => '动作名',
                'params' => '路由参数数组',
            ],
            'returns' => '无返回值（构造方法）',
            'example' => '// CLI 下模拟对 DummyController::indexAction 的一次请求
$request = new Yaf_Request_Simple("CLI", "index", "dummy", null, ["foo" => "bar"]);
var_dump($request->isRouted());        // true
var_dump($request->getControllerName()); // Dummy',
        ],
        '::isXmlHttpRequest' => [
            'desc' => '覆写了父类实现。Simple 请求面向 CLI/测试场景，不可能是 Ajax 请求。',
            'returns' => '恒返回 FALSE',
        ],
    ],
    'Yaf_Response_Abstract' => [
        '@class' => '响应对象的抽象基类，负责管理响应 body 和输出。body 以命名段（key）累积保存，默认段名为 "content"（常量 DEFAULT_BODY 的值）。Yaf 框架根据 SAPI 自动选择具体子类：HTTP 环境为 Yaf_Response_Http（额外支持响应头、状态码、重定向），CLI 环境为 Yaf_Response_Cli。定义了 setHeader/setAllHeaders/getHeader/clearHeaders 的占位实现，实际功能由 Yaf_Response_Http 提供。',
        '::__construct' => [
            'desc' => '构造方法，无实际操作。响应对象通常由 Yaf 框架自动创建，无需手动实例化。',
            'returns' => '无返回值（构造方法）',
        ],
        '::__toString' => [
            'desc' => '将所有 body 段按设置的先后顺序拼接后返回，不输出任何内容。',
            'returns' => '拼接后的响应内容字符串；没有任何 body 时返回空字符串',
            'example' => '$response->setBody("Hello World");
echo $response; // Hello World',
        ],
        '::setBody' => [
            'desc' => '设置指定 body 段的内容，已存在的同名内容会被覆盖。name 省略时使用默认段 "content"。可通过不同 name 保存多段内容，全部输出时按设置顺序发送，可用于最后拼装 layout。',
            'params' => [
                'body' => '要响应的字符串，一般是一段 HTML 或 JSON',
                'name' => 'body 段名称，省略时为 "content"',
            ],
            'returns' => '成功返回 Yaf_Response_Abstract 实例自身，失败返回 FALSE',
            'example' => '$this->getResponse()->setBody("Hello World");',
        ],
        '::appendBody' => [
            'desc' => '向指定 body 段已有内容的末尾追加新内容；该段尚不存在时效果等同 setBody。name 省略时使用默认段 "content"。',
            'params' => [
                'body' => '要追加的字符串',
                'name' => 'body 段名称，省略时为 "content"',
            ],
            'returns' => '成功返回 Yaf_Response_Abstract 实例自身，失败返回 FALSE',
            'example' => '$response->setBody("Hello");
$response->appendBody(" World");
echo $response->getBody(); // Hello World',
        ],
        '::prependBody' => [
            'desc' => '向指定 body 段已有内容的前面插入新内容；该段尚不存在时效果等同 setBody。name 省略时使用默认段 "content"。',
            'params' => [
                'body' => '要插入的字符串',
                'name' => 'body 段名称，省略时为 "content"',
            ],
            'returns' => '成功返回 Yaf_Response_Abstract 实例自身，失败返回 FALSE',
            'example' => '$response->setBody("World");
$response->prependBody("Hello ");
echo $response->getBody(); // Hello World',
        ],
        '::clearBody' => [
            'desc' => '清除已设置的响应 body。传入 name 时只清除该段；省略或传 NULL 时清除全部 body 段。',
            'params' => [
                'name' => '要清除的 body 段名称，省略或 NULL 表示清除全部',
            ],
            'returns' => '返回 Yaf_Response_Abstract 实例自身',
        ],
        '::getBody' => [
            'desc' => '获取已设置的响应 body。不传参数时返回默认段 "content"；显式传 NULL 时返回包含所有 body 段的数组；传指定 name 时返回该段内容。',
            'params' => [
                'name' => 'body 段名称；省略取默认段 "content"，显式 NULL 取全部段组成的数组',
            ],
            'returns' => '对应的 body 内容字符串（传 NULL 时为数组）；相应内容不存在时返回空字符串（或空数组）',
            'example' => '$response->appendBody("Hello");
var_dump($response->getBody());       // string(5) "Hello"
print_r($response->getBody(null));    // ["content" => "Hello"]',
        ],
        '::response' => [
            'desc' => '发送响应给请求端。抽象类实现按设置顺序将所有 body 段直接输出，不处理 HTTP 头；实际使用中该方法由子类 Yaf_Response_Http / Yaf_Response_Cli 覆写。',
            'returns' => '成功返回 TRUE，失败返回 FALSE',
        ],
    ],
    'Yaf_Response_Http' => [
        '@class' => 'HTTP 环境下的响应对象，继承自 Yaf_Response_Abstract，额外管理 HTTP 响应头、状态码（protected 属性 response_code）和重定向。通常由 Yaf 框架自动创建；设置好的头和状态码不会立即发送，而是在 response() 调用时统一发送。',
        '::setHeader' => [
            'desc' => '设置 HTTP 响应头，仅记录，不立即发送（response() 时发送）。replace 默认 TRUE 表示替换同名头；为 FALSE 时新值以 ", " 连接到已有值之后。response_code 非 0 时同时设置响应状态码。',
            'params' => [
                'name' => '响应头名称，如 "Content-Type"',
                'value' => '响应头取值',
                'replace' => '同名头已存在时是否替换，默认 TRUE；FALSE 表示追加合并',
                'response_code' => '同时设置的 HTTP 状态码，0 表示不修改状态码',
            ],
            'returns' => '成功返回 TRUE，失败返回 FALSE',
            'example' => '$this->getResponse()->setHeader("Content-Type", "application/json");',
        ],
        '::setAllHeaders' => [
            'desc' => '批量设置 HTTP 响应头，数组的 key 为头名、value 为头值，同名头总是被替换。',
            'params' => [
                'headers' => '头名 => 头值 的数组',
            ],
            'returns' => '返回 TRUE',
        ],
        '::getHeader' => [
            'desc' => '获取已设置的响应头。传入 name 返回该头的值；不传参数返回所有已设置的响应头数组。',
            'params' => [
                'name' => '响应头名称；省略时返回全部',
            ],
            'returns' => '传 name 时返回对应头值，不存在返回 NULL；不传时返回全部响应头数组（未设置过头时为空数组）',
        ],
        '::clearHeaders' => [
            'desc' => '清除所有已设置的响应头。',
            'returns' => '返回 Yaf_Response_Http 实例自身（可链式调用）',
        ],
        '::setRedirect' => [
            'desc' => '重定向请求到指定 URL：发送 Location 头并设置 302 状态码（注意与 Yaf_Controller_Abstract::forward 的内部转发不同）。成功后会标记响应头已发送，response() 将不再重复发送已设置的头。url 为空，或运行在 CLI/phpdbg 环境下时不执行。',
            'params' => [
                'url' => '要重定向到的 URL',
            ],
            'returns' => '成功返回 TRUE；url 为空、CLI/phpdbg 环境或发送失败时返回 FALSE',
            'example' => '$this->getResponse()->setRedirect("http://domain.com/");',
        ],
        '::response' => [
            'desc' => '发送 HTTP 响应：若响应头尚未发送，先发送状态码和所有已设置的响应头并做标记，然后按设置顺序输出所有 body 段。重复调用不会重复发送头。',
            'returns' => '成功返回 TRUE，失败返回 FALSE',
            'example' => '$this->getResponse()->response();',
        ],
    ],
    'Yaf_Response_Cli' => [
        '@class' => 'CLI 环境下的响应对象，继承自 Yaf_Response_Abstract，没有自己的方法。Yaf 框架在 cli/phpdbg SAPI 下自动创建本类实例。response() 只按顺序输出 body 内容，不发送任何 HTTP 头；setRedirect 在 CLI 环境下恒返回 FALSE。',
    ],
];
