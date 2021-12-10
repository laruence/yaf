/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: edc476409aa71476eecd8cef4235a0a8d64f47b3 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Route_Interface_route, 0, 0, 1)
	ZEND_ARG_INFO(0, request)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Route_Interface_assemble, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, info, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, query, IS_ARRAY, 0, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Route_Static_match, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Route_Static_route arginfo_class_Yaf_Route_Interface_route

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Route_Static_assemble, 0, 1, MAY_BE_STRING|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, info, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, query, IS_ARRAY, 0, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Route_Supervar___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, supervar_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Route_Supervar_route arginfo_class_Yaf_Route_Interface_route

#define arginfo_class_Yaf_Route_Supervar_assemble arginfo_class_Yaf_Route_Static_assemble

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Route_Simple___construct, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, module_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, controller_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, action_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Route_Simple_route arginfo_class_Yaf_Route_Interface_route

#define arginfo_class_Yaf_Route_Simple_assemble arginfo_class_Yaf_Route_Static_assemble

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Route_Rewrite___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, match, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, route, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, verify, IS_ARRAY, 0, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Route_Rewrite_match, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Route_Rewrite_route arginfo_class_Yaf_Route_Interface_route

#define arginfo_class_Yaf_Route_Rewrite_assemble arginfo_class_Yaf_Route_Static_assemble

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Route_Regex___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, match, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, route, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, map, IS_ARRAY, 0, "NULL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, verify, IS_ARRAY, 0, "NULL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, reverse, IS_STRING, 0, "NULL")
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Route_Regex_match arginfo_class_Yaf_Route_Rewrite_match

#define arginfo_class_Yaf_Route_Regex_route arginfo_class_Yaf_Route_Interface_route

#define arginfo_class_Yaf_Route_Regex_assemble arginfo_class_Yaf_Route_Static_assemble

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Route_Map___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, controller_prefer, _IS_BOOL, 0, "1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delimiter, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Route_Map_route arginfo_class_Yaf_Route_Interface_route

#define arginfo_class_Yaf_Route_Map_assemble arginfo_class_Yaf_Route_Static_assemble
