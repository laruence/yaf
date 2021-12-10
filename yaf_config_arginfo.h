/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2a33bede1d9790ff5cb67c0e86f42e688930fa98 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Config_Abstract_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Config_Abstract_rewind, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Config_Abstract_current, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Config_Abstract_next arginfo_class_Yaf_Config_Abstract_rewind

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Config_Abstract_valid, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Config_Abstract_key, 0, 0, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_NULL|MAY_BE_BOOL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Config_Abstract___isset, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Config_Abstract___get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Config_Abstract_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Config_Abstract_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Config_Abstract_offsetUnSet, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Config_Abstract_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Config_Abstract_get, 0, 0, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_STRING, 1, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Config_Abstract_set, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Config_Abstract_readonly arginfo_class_Yaf_Config_Abstract_valid

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Config_Abstract_toArray, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Config_Simple___construct, 0, 0, 1)
	ZEND_ARG_TYPE_MASK(0, config_file, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, section, IS_STRING, 1, "NULL")
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Config_Simple___isset arginfo_class_Yaf_Config_Abstract___isset

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Config_Simple___set, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Config_Simple_get arginfo_class_Yaf_Config_Abstract_get

#define arginfo_class_Yaf_Config_Simple_set arginfo_class_Yaf_Config_Abstract_set

#define arginfo_class_Yaf_Config_Simple_readonly arginfo_class_Yaf_Config_Abstract_valid

#define arginfo_class_Yaf_Config_Simple_offsetGet arginfo_class_Yaf_Config_Abstract_offsetGet

#define arginfo_class_Yaf_Config_Simple_offsetSet arginfo_class_Yaf_Config_Abstract_offsetSet

#define arginfo_class_Yaf_Config_Simple_offsetUnSet arginfo_class_Yaf_Config_Abstract_offsetUnSet

#define arginfo_class_Yaf_Config_Ini___construct arginfo_class_Yaf_Config_Simple___construct

#define arginfo_class_Yaf_Config_Ini___isset arginfo_class_Yaf_Config_Abstract___isset

#define arginfo_class_Yaf_Config_Ini_get arginfo_class_Yaf_Config_Abstract_get

#define arginfo_class_Yaf_Config_Ini_set arginfo_class_Yaf_Config_Abstract_set

#define arginfo_class_Yaf_Config_Ini_readonly arginfo_class_Yaf_Config_Abstract_valid

#define arginfo_class_Yaf_Config_Ini_offsetGet arginfo_class_Yaf_Config_Abstract_offsetGet

#define arginfo_class_Yaf_Config_Ini_offsetSet arginfo_class_Yaf_Config_Abstract_offsetSet

#define arginfo_class_Yaf_Config_Ini___set arginfo_class_Yaf_Config_Abstract_offsetSet
