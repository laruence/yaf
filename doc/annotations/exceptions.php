<?php
/**
 * Annotations: Yaf_Exception family.
 *
 * Exception classes have no own methods; only class-level prose here.
 * Inheritance and error-code mapping verified against yaf_exception.c:
 * each built-in exception corresponds to a YAF_ERR_* code used by
 * yaf_throw_exception().
 */

return [
    'Yaf_Exception' => [
        '@class' => "Yaf 所有异常的基类，继承自 `RuntimeException`（SPL 不可用时退化为 `Exception`）。\n\n当 `yaf.dispatcher.throwException=1`（或 `Yaf_Dispatcher::throwException(TRUE)`）时，框架错误以异常抛出；否则降级为可恢复错误（`E_RECOVERABLE_ERROR`）。错误码对应 `YAF_ERR_*` 常量，子类与错误码一一对应。",
        '@class_example' => "<?php\n// ini: yaf.dispatcher.throwException=1\n\$app = new Yaf_Application('conf/app.ini');\ntry {\n    \$app->run();\n} catch (Yaf_Exception \$e) {\n    var_dump(\$e->getCode(), \$e->getMessage());\n}",
    ],
    'Yaf_Exception_StartupError' => [
        '@class' => "启动阶段错误，对应错误码 `YAF_ERR_STARTUP_FAILED` (512)。典型场景：重复实例化 `Yaf_Application`、应用初始化配置错误。",
    ],
    'Yaf_Exception_RouterFailed' => [
        '@class' => "路由阶段错误，对应错误码 `YAF_ERR_ROUTE_FAILED` (513)。典型场景：自定义路由协议实现不符合接口要求。",
    ],
    'Yaf_Exception_DispatchFailed' => [
        '@class' => "分发阶段错误，对应错误码 `YAF_ERR_DISPATCH_FAILED` (514)。典型场景：action 方法不存在或不可调用。",
    ],
    'Yaf_Exception_LoadFailed' => [
        '@class' => "自动加载失败基类，对应错误码 `YAF_ERR_AUTOLOAD_FAILED` (520)。`LoadFailed_Module/Controller/Action/View` 均继承自它，可用一个 catch 统一处理四类加载失败。",
    ],
    'Yaf_Exception_LoadFailed_Module' => [
        '@class' => "模块未找到，对应错误码 `YAF_ERR_NOTFOUND_MODULE` (515)。",
    ],
    'Yaf_Exception_LoadFailed_Controller' => [
        '@class' => "控制器未找到，对应错误码 `YAF_ERR_NOTFOUND_CONTROLLER` (516)。最常见的 404 类错误。",
    ],
    'Yaf_Exception_LoadFailed_Action' => [
        '@class' => "动作未找到，对应错误码 `YAF_ERR_NOTFOUND_ACTION` (517)。",
    ],
    'Yaf_Exception_LoadFailed_View' => [
        '@class' => "视图脚本未找到，对应错误码 `YAF_ERR_NOTFOUND_VIEW` (518)。",
    ],
    'Yaf_Exception_TypeError' => [
        '@class' => "类型错误，对应错误码 `YAF_ERR_TYPE_ERROR` (521)。典型场景：配置项或 API 参数类型不符合要求。",
    ],
];
