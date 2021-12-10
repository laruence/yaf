/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 3280e5bbb814089b48961f0bcc08eeaf16ed5319 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Loader___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Yaf_Loader_getInstance, 0, 0, Yaf_Loader, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, local_path, IS_STRING, 0, "NULL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, global_path, IS_STRING, 0, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Loader_import, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Loader_autoload, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, class_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Yaf_Loader_registerLocalNamespace, 0, 1, Yaf_Loader, MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, name_prefix, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, path, IS_STRING, 0, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Loader_getLocalNamespace, 0, 0, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Loader_clearLocalNamespace, 0, 0, _IS_BOOL, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Loader_isLocalName, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, class_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Yaf_Loader_setLibraryPath, 0, 1, Yaf_Loader, 1)
	ZEND_ARG_TYPE_INFO(0, library_path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, is_global, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Loader_getLibraryPath, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, is_global, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()
