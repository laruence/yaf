/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c0fe3d13422654adb5be158a281a4a16d861f00d */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Dispatcher___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Dispatcher_enableView, 0, 0, IS_OBJECT, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Dispatcher_disableView arginfo_class_Yaf_Dispatcher_enableView

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Dispatcher_initView, 0, 1, MAY_BE_OBJECT|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, templates_dir, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Dispatcher_setView, 0, 1, IS_OBJECT, 1)
	ZEND_ARG_TYPE_INFO(0, view, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Dispatcher_setRequest, 0, 1, IS_OBJECT, 1)
	ZEND_ARG_TYPE_INFO(0, request, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Dispatcher_setResponse, 0, 1, IS_OBJECT, 1)
	ZEND_ARG_TYPE_INFO(0, response, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Dispatcher_getApplication arginfo_class_Yaf_Dispatcher_enableView

#define arginfo_class_Yaf_Dispatcher_getRouter arginfo_class_Yaf_Dispatcher_enableView

#define arginfo_class_Yaf_Dispatcher_getRequest arginfo_class_Yaf_Dispatcher_enableView

#define arginfo_class_Yaf_Dispatcher_getResponse arginfo_class_Yaf_Dispatcher_enableView

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Dispatcher_setErrorHandler, 0, 1, MAY_BE_OBJECT|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, callback, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, error_types, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Dispatcher_setDefaultModule, 0, 1, MAY_BE_OBJECT|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, module, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Dispatcher_setDefaultController, 0, 1, MAY_BE_OBJECT|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, controller, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Dispatcher_setDefaultAction, 0, 1, MAY_BE_OBJECT|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, action, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Dispatcher_returnResponse, 0, 0, MAY_BE_OBJECT|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flag, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Dispatcher_autoRender, 0, 0, MAY_BE_OBJECT|MAY_BE_NULL|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flag, _IS_BOOL, 1, "NULL")
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Dispatcher_flushInstantly arginfo_class_Yaf_Dispatcher_autoRender

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Dispatcher_dispatch, 0, 1, MAY_BE_OBJECT|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, request, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Dispatcher_throwException arginfo_class_Yaf_Dispatcher_autoRender

#define arginfo_class_Yaf_Dispatcher_catchException arginfo_class_Yaf_Dispatcher_autoRender

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Dispatcher_registerPlugin, 0, 1, MAY_BE_OBJECT|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, plugin, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Dispatcher_getDefaultModule, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Dispatcher_getDefaultController arginfo_class_Yaf_Dispatcher_getDefaultModule

#define arginfo_class_Yaf_Dispatcher_getDefaultAction arginfo_class_Yaf_Dispatcher_getDefaultModule

#define arginfo_class_Yaf_Dispatcher_getInstance arginfo_class_Yaf_Dispatcher_enableView
