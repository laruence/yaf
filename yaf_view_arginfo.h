/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 45071bca670049bd8ac3f1a2ea5611c022466ade */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_View_Interface_assign, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_MIXED, 0, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_View_Interface_render, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, tpl, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, tpl_vars, IS_ARRAY, 1, "NULL")
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_View_Interface_display arginfo_class_Yaf_View_Interface_render

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_View_Interface_setScriptPath, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, template_dir, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_View_Interface_getScriptPath, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_View_Simple___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, tempalte_dir, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_View_Simple___isset, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_View_Simple_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_View_Simple_assign, 0, 0, MAY_BE_OBJECT|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_MIXED, 0, "NULL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_MIXED, 0, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_View_Simple_render, 0, 1, MAY_BE_NULL|MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, tpl, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, tpl_vars, IS_ARRAY, 1, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_View_Simple_eval, 0, 1, MAY_BE_NULL|MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, tpl_str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, vars, IS_ARRAY, 1, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_View_Simple_display, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, tpl, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, tpl_vars, IS_ARRAY, 1, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_View_Simple_assignRef, 0, 2, IS_OBJECT, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_STRING, 1, "NULL")
	ZEND_ARG_TYPE_INFO(1, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_View_Simple_clear, 0, 0, IS_OBJECT, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_STRING, 1, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_View_Simple_setScriptPath, 0, 1, MAY_BE_OBJECT|MAY_BE_NULL|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, template_dir, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_View_Simple_getScriptPath, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_View_Simple___get arginfo_class_Yaf_View_Simple_get

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_View_Simple___set, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

