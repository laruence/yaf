/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 6d9544072e642f53747bc52f01c4a4a52aa0e3e5 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Response_Abstract___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Response_Abstract_setBody, 0, 0, 1)
	ZEND_ARG_INFO(0, body)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, name, "NULL")
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Response_Abstract_appendBody arginfo_class_Yaf_Response_Abstract_setBody

#define arginfo_class_Yaf_Response_Abstract_prependBody arginfo_class_Yaf_Response_Abstract_setBody

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Yaf_Response_Abstract_clearBody, 0, 0, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, name, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Response_Abstract_getBody, 0, 0, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_STRING, 0, "NULL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Response_Abstract_setRedirect, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, url, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Response_Abstract_response arginfo_class_Yaf_Response_Abstract___construct

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Response_Abstract___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Response_Http_setHeader, 0, 2, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, replace, _IS_BOOL, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, response_code, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Response_Http_setAllHeaders, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, headers, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Response_Http_getHeader arginfo_class_Yaf_Response_Abstract_getBody

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Yaf_Response_Http_clearHeaders, 0, 0, MAY_BE_OBJECT|MAY_BE_NULL|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_class_Yaf_Response_Http_setRedirect arginfo_class_Yaf_Response_Abstract_setRedirect

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Yaf_Response_Http_response, 0, 0, _IS_BOOL, 1)
ZEND_END_ARG_INFO()
