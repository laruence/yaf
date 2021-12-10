/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c1419f58751475cec2cf84b0bab3d759846eea79 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Application___construct, 0, 0, 1)
	ZEND_ARG_TYPE_MASK(0, config, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, environ, IS_STRING, 0, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Application_run, 0, 0, MAY_BE_OBJECT|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Application_execute, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Application_environ, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Application_bootstrap, 0, 0, MAY_BE_OBJECT|MAY_BE_FALSE|MAY_BE_NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Application_getConfig, 0, 0, IS_OBJECT, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Application_getModules, 0, 0, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Application_getDispatcher arginfo_class_Yaf_Application_getConfig

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Application_setAppDirectory, 0, 1, MAY_BE_OBJECT|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Application_getAppDirectory arginfo_class_Yaf_Application_environ

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Application_getLastErrorNo, 0, 0, IS_LONG, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Application_getLastErrorMsg arginfo_class_Yaf_Application_environ

#define arginfo_class_Yaf_Application_clearLastError arginfo_class_Yaf_Application_getConfig

#define arginfo_class_Yaf_Application_app arginfo_class_Yaf_Application_getConfig
