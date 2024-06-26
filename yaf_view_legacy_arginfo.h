/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 241571a08dce1a4f00fe4e4ae5874c211fc0918d */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_View_Interface_assign, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_View_Interface_render, 0, 0, 1)
	ZEND_ARG_INFO(0, tpl)
	ZEND_ARG_INFO(0, tpl_vars)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_View_Interface_display arginfo_class_Yaf_View_Interface_render

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_View_Interface_setScriptPath, 0, 0, 1)
	ZEND_ARG_INFO(0, template_dir)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_View_Interface_getScriptPath, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_View_Simple___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, tempalte_dir)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_View_Simple___isset, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_View_Simple_get, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_View_Simple_assign, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_View_Simple_render arginfo_class_Yaf_View_Interface_render

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_View_Simple_eval, 0, 0, 1)
	ZEND_ARG_INFO(0, tpl_str)
	ZEND_ARG_INFO(0, vars)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_View_Simple_display arginfo_class_Yaf_View_Interface_render

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_View_Simple_assignRef, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(1, value)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_View_Simple_clear arginfo_class_Yaf_View_Simple_get

#define arginfo_class_Yaf_View_Simple_setScriptPath arginfo_class_Yaf_View_Interface_setScriptPath

#define arginfo_class_Yaf_View_Simple_getScriptPath arginfo_class_Yaf_View_Interface_getScriptPath

#define arginfo_class_Yaf_View_Simple___get arginfo_class_Yaf_View_Simple___isset

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_View_Simple___set, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

