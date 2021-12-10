/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: ef6113aea8b100c9a211a3b81733b4fc4daaced0 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Request_Abstract_isGet, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Request_Abstract_isPost arginfo_class_Yaf_Request_Abstract_isGet

#define arginfo_class_Yaf_Request_Abstract_isPut arginfo_class_Yaf_Request_Abstract_isGet

#define arginfo_class_Yaf_Request_Abstract_isHead arginfo_class_Yaf_Request_Abstract_isGet

#define arginfo_class_Yaf_Request_Abstract_isOptions arginfo_class_Yaf_Request_Abstract_isGet

#define arginfo_class_Yaf_Request_Abstract_isDelete arginfo_class_Yaf_Request_Abstract_isGet

#define arginfo_class_Yaf_Request_Abstract_isPatch arginfo_class_Yaf_Request_Abstract_isGet

#define arginfo_class_Yaf_Request_Abstract_isCli arginfo_class_Yaf_Request_Abstract_isGet

#define arginfo_class_Yaf_Request_Abstract_isXmlHttpRequest arginfo_class_Yaf_Request_Abstract_isGet

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Request_Abstract_getServer, 0, 0, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_STRING, 0, "NULL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_MIXED, 0, "NULL")
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Request_Abstract_getEnv arginfo_class_Yaf_Request_Abstract_getServer

#define arginfo_class_Yaf_Request_Abstract_getQuery arginfo_class_Yaf_Request_Abstract_getServer

#define arginfo_class_Yaf_Request_Abstract_getRequest arginfo_class_Yaf_Request_Abstract_getServer

#define arginfo_class_Yaf_Request_Abstract_getPost arginfo_class_Yaf_Request_Abstract_getServer

#define arginfo_class_Yaf_Request_Abstract_getCookie arginfo_class_Yaf_Request_Abstract_getServer

#define arginfo_class_Yaf_Request_Abstract_getFiles arginfo_class_Yaf_Request_Abstract_getServer

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Request_Abstract_getRaw, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Request_Abstract_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_MIXED, 0, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Request_Abstract_clearParams, 0, 0, IS_OBJECT, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Request_Abstract_setParam, 0, 1, MAY_BE_OBJECT|MAY_BE_BOOL|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, name, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_MIXED, 1, "NULL")
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Request_Abstract_getParam arginfo_class_Yaf_Request_Abstract_get

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Request_Abstract_getParams, 0, 0, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Yaf_Request_Abstract_getException, 0, 0, Exception, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Request_Abstract_getModuleName arginfo_class_Yaf_Request_Abstract_getRaw

#define arginfo_class_Yaf_Request_Abstract_getControllerName arginfo_class_Yaf_Request_Abstract_getRaw

#define arginfo_class_Yaf_Request_Abstract_getActionName arginfo_class_Yaf_Request_Abstract_getRaw

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Request_Abstract_setModuleName, 0, 1, IS_OBJECT, 1)
	ZEND_ARG_TYPE_INFO(0, module, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, format_name, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Request_Abstract_setControllerName, 0, 1, IS_OBJECT, 1)
	ZEND_ARG_TYPE_INFO(0, controller, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, format_name, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Request_Abstract_setActionName, 0, 1, IS_OBJECT, 1)
	ZEND_ARG_TYPE_INFO(0, action, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, format_name, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Request_Abstract_getMethod arginfo_class_Yaf_Request_Abstract_getRaw

#define arginfo_class_Yaf_Request_Abstract_getLanguage arginfo_class_Yaf_Request_Abstract_getRaw

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Request_Abstract_setBaseUri, 0, 1, MAY_BE_OBJECT|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, uir, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Request_Abstract_getBaseUri arginfo_class_Yaf_Request_Abstract_getRaw

#define arginfo_class_Yaf_Request_Abstract_getRequestUri arginfo_class_Yaf_Request_Abstract_getRaw

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Request_Abstract_setRequestUri, 0, 1, IS_OBJECT, 1)
	ZEND_ARG_TYPE_INFO(0, uir, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Request_Abstract_isDispatched arginfo_class_Yaf_Request_Abstract_isGet

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Request_Abstract_setDispatched, 0, 0, IS_OBJECT, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flag, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Request_Abstract_isRouted arginfo_class_Yaf_Request_Abstract_isGet

#define arginfo_class_Yaf_Request_Abstract_setRouted arginfo_class_Yaf_Request_Abstract_setDispatched

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Request_Http___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, requestUri, IS_STRING, 1, "NULL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, baseUri, IS_STRING, 1, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Request_Simple___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, method, IS_STRING, 1, "NULL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, module, IS_STRING, 1, "NULL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, controller, IS_STRING, 1, "NULL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, action, IS_STRING, 1, "NULL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, params, IS_ARRAY, 1, "NULL")
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Request_Simple_isXmlHttpRequest arginfo_class_Yaf_Request_Abstract_isGet
