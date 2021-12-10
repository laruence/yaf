/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 3280e5bbb814089b48961f0bcc08eeaf16ed5319 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Loader___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Loader_getInstance, 0, 0, 0)
	ZEND_ARG_INFO(0, local_path)
	ZEND_ARG_INFO(0, global_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Loader_import, 0, 0, 1)
	ZEND_ARG_INFO(0, file)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Loader_autoload, 0, 0, 1)
	ZEND_ARG_INFO(0, class_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Loader_registerLocalNamespace, 0, 0, 1)
	ZEND_ARG_INFO(0, name_prefix)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Loader_getLocalNamespace arginfo_class_Yaf_Loader___construct

#define arginfo_class_Yaf_Loader_clearLocalNamespace arginfo_class_Yaf_Loader___construct

#define arginfo_class_Yaf_Loader_isLocalName arginfo_class_Yaf_Loader_autoload

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Loader_setLibraryPath, 0, 0, 1)
	ZEND_ARG_INFO(0, library_path)
	ZEND_ARG_INFO(0, is_global)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Loader_getLibraryPath, 0, 0, 0)
	ZEND_ARG_INFO(0, is_global)
ZEND_END_ARG_INFO()
