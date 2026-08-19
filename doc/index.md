# Yaf API 手册

版本 `3.3.8-dev` · 生成于 2026-08-16 · 类/方法签名来自运行中的 yaf 扩展 (Reflection), 与源码一致。

## 指南

- [快速上手: 从零跑起一个最小应用](guides/quick-start.md)
- [请求生命周期: 一次请求在 yaf 内部的完整路径](guides/lifecycle.md)
- [目录结构与类名约定: 类如何映射到文件](guides/directory-conventions.md)
- [路由机制: 6 种内建路由与自定义路由](guides/routing.md)
- [错误与异常处理: 两条错误通道的开关组合](guides/error-handling.md)

## 参考

- [常量](constants.md)
- [配置项 (php.ini)](ini.md)

### Application

- [Yaf_Application](classes/yaf_application.md) — final, 15 个自有方法 — Yaf 应用的入口与运行环境容器。
- [Yaf_Bootstrap_Abstract](classes/yaf_bootstrap_abstract.md) — abstract, 0 个自有方法 — 应用引导类的基类, 提供在分发请求之前定制应用的最早时机。
- [Yaf_Dispatcher](classes/yaf_dispatcher.md) — final, 26 个自有方法 — 分发器, final 单例, 由 Yaf_Application 在初始化时创建, 负责路由请求、触发插件钩子、加载并执…
- [Yaf_Loader](classes/yaf_loader.md) — final, 13 个自有方法 — Yaf 的自动加载器, final 单例。
- [Yaf_Registry](classes/yaf_registry.md) — final, 5 个自有方法 — 对象注册表（全局容器），用于在整个请求生命周期内跨组件共享对象和值。
- [Yaf_Session](classes/yaf_session.md) — abstract, 17 个自有方法 — Yaf 对 PHP session 的包装，abstract final 类（构造器私有，不可直接实例化，通过 getI…

### MVC

- [Yaf_Controller_Abstract](classes/yaf_controller_abstract.md) — abstract, 15 个自有方法 — Yaf_Controller_Abstract 是 Yaf MVC 体系的核心部分。
- [Yaf_Action_Abstract](classes/yaf_action_abstract.md) — abstract, 3 个自有方法 — Yaf_Action_Abstract 是 MVC 中控制器动作的基类，继承自 Yaf_Controller_Abstr…
- [Yaf_View_Interface](classes/yaf_view_interface.md) — interface, 5 个自有方法 — Yaf_View_Interface 是 Yaf 的视图引擎接口，定义了接入 Yaf 的视图引擎需要实现的方法与功能。
- [Yaf_View_Simple](classes/yaf_view_simple.md) — class, 13 个自有方法 — Yaf_View_Simple 是 Yaf 内置的视图引擎，追求性能，不提供 Smarty 那样丰富的功能与复杂语法。

### Request / Response

- [Yaf_Request_Abstract](classes/yaf_request_abstract.md) — abstract, 39 个自有方法 — 请求对象的抽象基类，封装了请求方法、请求 URI、路由结果（module/controller/action/路由参数）…
- [Yaf_Request_Http](classes/yaf_request_http.md) — class, 1 个自有方法 — HTTP 环境下的请求对象，继承自 Yaf_Request_Abstract。
- [Yaf_Request_Simple](classes/yaf_request_simple.md) — class, 2 个自有方法 — 简单请求对象，继承自 Yaf_Request_Abstract，主要用于 CLI 脚本和单元测试。
- [Yaf_Response_Abstract](classes/yaf_response_abstract.md) — abstract, 8 个自有方法 — 响应对象的抽象基类，负责管理响应 body 和输出。
- [Yaf_Response_Http](classes/yaf_response_http.md) — class, 6 个自有方法 — HTTP 环境下的响应对象，继承自 Yaf_Response_Abstract，额外管理 HTTP 响应头、状态码（pr…
- [Yaf_Response_Cli](classes/yaf_response_cli.md) — class, 0 个自有方法 — CLI 环境下的响应对象，继承自 Yaf_Response_Abstract，没有自己的方法。

### Config

- [Yaf_Config_Abstract](classes/yaf_config_abstract.md) — abstract, 16 个自有方法 — 配置适配器基类，为应用提供对象化的配置数据访问。
- [Yaf_Config_Ini](classes/yaf_config_ini.md) — final, 7 个自有方法 — INI 文件配置适配器，final 类，恒为只读。
- [Yaf_Config_Simple](classes/yaf_config_simple.md) — final, 6 个自有方法 — PHP 数组配置适配器，final 类。

### Router / Route

- [Yaf_Router](classes/yaf_router.md) — final, 7 个自有方法 — Yaf_Router 是 Yaf 的路由器，负责把 Yaf_Request_Abstract 请求路由到具体的模块/控制…
- [Yaf_Route_Interface](classes/yaf_route_interface.md) — interface, 2 个自有方法 — Yaf_Route_Interface 是 Yaf 路由协议的标准接口，所有内建路由（Static/Simple/Sup…
- [Yaf_Route_Static](classes/yaf_route_static.md) — final, 3 个自有方法 — Yaf_Route_Static 是 Yaf 的默认路由协议（路由器中以 _default 注册），解析去除 base_…
- [Yaf_Route_Simple](classes/yaf_route_simple.md) — final, 3 个自有方法 — Yaf_Route_Simple 基于 $_GET 中的三个变量做路由：构造时给出三个键名，分别代表模块、控制器、动作在…
- [Yaf_Route_Supervar](classes/yaf_route_supervar.md) — final, 3 个自有方法 — Yaf_Route_Supervar 与 Simple 相似，也从 $_GET 取路由信息，不同之处在于它取的是一个包含…
- [Yaf_Route_Rewrite](classes/yaf_route_rewrite.md) — final, 4 个自有方法 — Yaf_Route_Rewrite 基于匹配模式对 URI 做重写路由，是功能最灵活的内建路由。
- [Yaf_Route_Regex](classes/yaf_route_regex.md) — final, 4 个自有方法 — Yaf_Route_Regex 用完整 PCRE 正则匹配 URI，是灵活度最高的内建路由（正则需自带定界符，如 #^/…
- [Yaf_Route_Map](classes/yaf_route_map.md) — final, 3 个自有方法 — Yaf_Route_Map 把 URI（去除 base_uri 后）按 / 分段并合并成一个下划线连接的名字，作为控制器…

### Plugin

- [Yaf_Plugin_Abstract](classes/yaf_plugin_abstract.md) — abstract, 7 个自有方法 — Yaf 插件基类，所有插件必须继承它。

### Exceptions

- [Yaf_Exception](classes/yaf_exception.md) — class, 0 个自有方法 — Yaf 所有异常的基类，继承自 RuntimeException（SPL 不可用时退化为 Exception）。
- [Yaf_Exception_DispatchFailed](classes/yaf_exception_dispatchfailed.md) — class, 0 个自有方法 — 分发阶段错误，对应错误码 YAF_ERR_DISPATCH_FAILED (514)。
- [Yaf_Exception_LoadFailed](classes/yaf_exception_loadfailed.md) — class, 0 个自有方法 — 自动加载失败基类，对应错误码 YAF_ERR_AUTOLOAD_FAILED (520)。
- [Yaf_Exception_LoadFailed_Action](classes/yaf_exception_loadfailed_action.md) — class, 0 个自有方法 — 动作未找到，对应错误码 YAF_ERR_NOTFOUND_ACTION (517)。
- [Yaf_Exception_LoadFailed_Controller](classes/yaf_exception_loadfailed_controller.md) — class, 0 个自有方法 — 控制器未找到，对应错误码 YAF_ERR_NOTFOUND_CONTROLLER (516)。
- [Yaf_Exception_LoadFailed_Module](classes/yaf_exception_loadfailed_module.md) — class, 0 个自有方法 — 模块未找到，对应错误码 YAF_ERR_NOTFOUND_MODULE (515)。
- [Yaf_Exception_LoadFailed_View](classes/yaf_exception_loadfailed_view.md) — class, 0 个自有方法 — 视图脚本未找到，对应错误码 YAF_ERR_NOTFOUND_VIEW (518)。
- [Yaf_Exception_RouterFailed](classes/yaf_exception_routerfailed.md) — class, 0 个自有方法 — 路由阶段错误，对应错误码 YAF_ERR_ROUTE_FAILED (513)。
- [Yaf_Exception_StartupError](classes/yaf_exception_startuperror.md) — class, 0 个自有方法 — 启动阶段错误，对应错误码 YAF_ERR_STARTUP_FAILED (512)。
- [Yaf_Exception_TypeError](classes/yaf_exception_typeerror.md) — class, 0 个自有方法 — 类型错误，对应错误码 YAF_ERR_TYPE_ERROR (521)。

