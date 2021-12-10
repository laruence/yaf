/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c265ad91ca592d72f56f0603c262bcb90f0a1737 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Controller_Abstract___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Controller_Abstract_getRequest, 0, 0, IS_OBJECT, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Controller_Abstract_getResponse arginfo_class_Yaf_Controller_Abstract_getRequest

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Controller_Abstract_getModuleName, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Controller_Abstract_getView arginfo_class_Yaf_Controller_Abstract_getRequest

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Controller_Abstract_initView, 0, 0, IS_OBJECT, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Controller_Abstract_setViewpath, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, view_directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Controller_Abstract_getViewpath arginfo_class_Yaf_Controller_Abstract_getModuleName

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Controller_Abstract_forward, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, args1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, args2, IS_MIXED, 0, "NULL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, args3, IS_MIXED, 0, "NULL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, args4, IS_MIXED, 0, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Controller_Abstract_redirect, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, url, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Controller_Abstract_getInvokeArgs, 0, 0, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Controller_Abstract_getInvokeArg, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Controller_Abstract_getName arginfo_class_Yaf_Controller_Abstract_getModuleName

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Controller_Abstract_render, 0, 1, MAY_BE_STRING|MAY_BE_NULL|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, tpl, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, parameters, IS_ARRAY, 0, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Controller_Abstract_display, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, tpl, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, parameters, IS_ARRAY, 0, "NULL")
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Action_Abstract_execute arginfo_class_Yaf_Controller_Abstract___construct

#define arginfo_class_Yaf_Action_Abstract_getController arginfo_class_Yaf_Controller_Abstract_getRequest

#define arginfo_class_Yaf_Action_Abstract_getControllerName arginfo_class_Yaf_Controller_Abstract_getModuleName
