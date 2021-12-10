/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: edc476409aa71476eecd8cef4235a0a8d64f47b3 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Route_Interface_route, 0, 0, 1)
	ZEND_ARG_INFO(0, request)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Route_Interface_assemble, 0, 0, 1)
	ZEND_ARG_INFO(0, info)
	ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Route_Static_match, 0, 0, 1)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Route_Static_route arginfo_class_Yaf_Route_Interface_route

#define arginfo_class_Yaf_Route_Static_assemble arginfo_class_Yaf_Route_Interface_assemble

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Route_Supervar___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, supervar_name)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Route_Supervar_route arginfo_class_Yaf_Route_Interface_route

#define arginfo_class_Yaf_Route_Supervar_assemble arginfo_class_Yaf_Route_Interface_assemble

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Route_Simple___construct, 0, 0, 3)
	ZEND_ARG_INFO(0, module_name)
	ZEND_ARG_INFO(0, controller_name)
	ZEND_ARG_INFO(0, action_name)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Route_Simple_route arginfo_class_Yaf_Route_Interface_route

#define arginfo_class_Yaf_Route_Simple_assemble arginfo_class_Yaf_Route_Interface_assemble

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Route_Rewrite___construct, 0, 0, 2)
	ZEND_ARG_INFO(0, match)
	ZEND_ARG_INFO(0, route)
	ZEND_ARG_INFO(0, verify)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Route_Rewrite_match arginfo_class_Yaf_Route_Static_match

#define arginfo_class_Yaf_Route_Rewrite_route arginfo_class_Yaf_Route_Interface_route

#define arginfo_class_Yaf_Route_Rewrite_assemble arginfo_class_Yaf_Route_Interface_assemble

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Route_Regex___construct, 0, 0, 2)
	ZEND_ARG_INFO(0, match)
	ZEND_ARG_INFO(0, route)
	ZEND_ARG_INFO(0, map)
	ZEND_ARG_INFO(0, verify)
	ZEND_ARG_INFO(0, reverse)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Route_Regex_match arginfo_class_Yaf_Route_Static_match

#define arginfo_class_Yaf_Route_Regex_route arginfo_class_Yaf_Route_Interface_route

#define arginfo_class_Yaf_Route_Regex_assemble arginfo_class_Yaf_Route_Interface_assemble

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Route_Map___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, controller_prefer)
	ZEND_ARG_INFO(0, delimiter)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Route_Map_route arginfo_class_Yaf_Route_Interface_route

#define arginfo_class_Yaf_Route_Map_assemble arginfo_class_Yaf_Route_Interface_assemble
