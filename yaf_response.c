/*
  +----------------------------------------------------------------------+
  | Yet Another Framework                                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Xinchen Hui  <laruence@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id: yaf_response.c 329197 2013-01-18 05:55:37Z laruence $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "main/SAPI.h" /* for sapi_header_line */
#include "ext/standard/php_string.h" /* for php_implode */
#include "Zend/zend_interfaces.h"

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_response.h"
#include "yaf_exception.h"

#include "responses/yaf_response_http.h"
#include "responses/yaf_response_cli.h"

zend_class_entry *yaf_response_ce;

/** {{{ ARG_INFO
 *  */
ZEND_BEGIN_ARG_INFO_EX(yaf_response_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_get_body_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_set_redirect_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, url)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_set_body_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, body)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_clear_body_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_set_header_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, rep)
	ZEND_ARG_INFO(0, response_code)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_clear_headers_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_get_header_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_set_all_headers_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, headers)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ yaf_response_t * yaf_response_instance(yaf_response_t *this_ptr, char *sapi_name TSRMLS_DC)
 */
yaf_response_t * yaf_response_instance(yaf_response_t *this_ptr, char *sapi_name TSRMLS_DC) {
	zval 			*header, *body;
	zend_class_entry 	*ce;
	yaf_response_t 		*instance;

	if (strncasecmp(sapi_name, "cli", 3)) {
		ce = yaf_response_http_ce;
	} else {
		ce = yaf_response_cli_ce;
	}

	if (this_ptr) {
		instance = this_ptr;
	} else {
		MAKE_STD_ZVAL(instance);
		object_init_ex(instance, ce);
	}

	MAKE_STD_ZVAL(header);
	array_init(header);
	zend_update_property(ce, instance, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADER), header TSRMLS_CC);
	zval_ptr_dtor(&header);

	MAKE_STD_ZVAL(body);
	array_init(body);
	zend_update_property(ce, instance, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), body TSRMLS_CC);
	zval_ptr_dtor(&body);

	return instance;
}
/* }}} */

/** {{{ static int yaf_response_set_body(yaf_response_t *response, char *name, int name_len, char *body, long body_len TSRMLS_DC)
 */
#if 0
static int yaf_response_set_body(yaf_response_t *response, char *name, int name_len, char *body, long body_len TSRMLS_DC) {
	zval *zbody;
	zend_class_entry *response_ce;

	if (!body_len) {
		return 1;
	}

	response_ce = Z_OBJCE_P(response);

	zbody = zend_read_property(response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1 TSRMLS_CC);

	zval_ptr_dtor(&zbody);

	MAKE_STD_ZVAL(zbody);
	ZVAL_STRINGL(zbody, body, body_len, 1);

	zend_update_property(response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), zbody TSRMLS_CC);

	return 1;
}
#endif
/* }}} */

/** {{{ int yaf_response_alter_body(yaf_response_t *response, char *name, int name_len, char *body, long body_len, int flag TSRMLS_DC)
 */
int yaf_response_alter_body(yaf_response_t *response, char *name, int name_len, char *body, long body_len, int flag TSRMLS_DC) {
	zval *zbody, **ppzval;
	char *obody;

	if (!body_len) {
		return 1;
	}

	zbody = zend_read_property(yaf_response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1 TSRMLS_CC);

	if (!name) {
		name = YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODY;
		name_len = sizeof(YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODY) - 1;
	}

	if (zend_hash_find(Z_ARRVAL_P(zbody), name, name_len + 1, (void **)&ppzval) == FAILURE) {
		zval *body;
		MAKE_STD_ZVAL(body);
		ZVAL_EMPTY_STRING(body);
		zend_hash_update(Z_ARRVAL_P(zbody), name, name_len +1, (void **)&body, sizeof(zval *), (void **)&ppzval);
	}

	obody = Z_STRVAL_PP(ppzval);

	switch (flag) {
		case YAF_RESPONSE_PREPEND:
			Z_STRLEN_PP(ppzval) = spprintf(&Z_STRVAL_PP(ppzval), 0, "%s%s", body, obody);
			break;
		case YAF_RESPONSE_APPEND:
			Z_STRLEN_PP(ppzval) = spprintf(&Z_STRVAL_PP(ppzval), 0, "%s%s", obody, body);
			break;
		case YAF_RESPONSE_REPLACE:
		default:
			ZVAL_STRINGL(*ppzval, body, body_len, 1);
			break;
	}

	efree(obody);

	return 1;
}
/* }}} */

/** {{{ int yaf_response_clear_body(yaf_response_t *response, char *name, uint name_len TSRMLS_DC)
 */
int yaf_response_clear_body(yaf_response_t *response, char *name, uint name_len TSRMLS_DC) {
	zval *zbody;
	zbody = zend_read_property(yaf_response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1 TSRMLS_CC);

	if (name) {
		zend_hash_del(Z_ARRVAL_P(zbody), name, name_len + 1);
	} else {
		zend_hash_clean(Z_ARRVAL_P(zbody));
	}
	return 1;
}
/* }}} */

/** {{{ int yaf_response_set_redirect(yaf_response_t *response, char *url, int len TSRMLS_DC)
 */
int yaf_response_set_redirect(yaf_response_t *response, char *url, int len TSRMLS_DC) {
	sapi_header_line ctr = {0};

	ctr.line_len 		= spprintf(&(ctr.line), 0, "%s %s", "Location:", url);
	ctr.response_code 	= 0;
	if (sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC) == SUCCESS) {
		efree(ctr.line);
		return 1;
	}
	efree(ctr.line);
	return 0;
}
/* }}} */

/** {{{ zval * yaf_response_get_body(yaf_response_t *response, char *name, uint name_len TSRMLS_DC)
 */
zval * yaf_response_get_body(yaf_response_t *response, char *name, uint name_len TSRMLS_DC) {
	zval **ppzval;
	zval *zbody = zend_read_property(yaf_response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1 TSRMLS_CC);

	if (!name) {
		return zbody;
	}

	if (zend_hash_find(Z_ARRVAL_P(zbody), name, name_len + 1, (void **)&ppzval) == FAILURE) {
		return NULL;
	}

	return *ppzval;
}
/* }}} */

/** {{{ int yaf_response_send(yaf_response_t *response TSRMLS_DC)
 */
int yaf_response_send(yaf_response_t *response TSRMLS_DC) {
	zval **val;

	zval *zbody = zend_read_property(yaf_response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1 TSRMLS_CC);

	zend_hash_internal_pointer_reset(Z_ARRVAL_P(zbody));
	while (SUCCESS == zend_hash_get_current_data(Z_ARRVAL_P(zbody), (void**)&val)) {
		convert_to_string_ex(val);
		php_write(Z_STRVAL_PP(val), Z_STRLEN_PP(val) TSRMLS_CC);
		zend_hash_move_forward(Z_ARRVAL_P(zbody));
	}

	return 1;
}
/* }}} */

/** {{{ proto private Yaf_Response_Abstract::__construct()
*/
PHP_METHOD(yaf_response, __construct) {
	(void)yaf_response_instance(getThis(), sapi_module.name TSRMLS_CC);
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::__desctruct(void)
*/
PHP_METHOD(yaf_response, __destruct) {
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::appenBody($body, $name = NULL)
*/
PHP_METHOD(yaf_response, appendBody) {
	char		*body, *name = NULL;
	uint		body_len, name_len = 0;
	yaf_response_t 	*self;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &body, &body_len, &name, &name_len) == FAILURE) {
		return;
	}

	self = getThis();

	if (yaf_response_alter_body(self, name, name_len, body, body_len, YAF_RESPONSE_APPEND TSRMLS_CC)) {
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::prependBody($body, $name = NULL)
*/
PHP_METHOD(yaf_response, prependBody) {
	char		*body, *name = NULL;
	uint		body_len, name_len = 0;
	yaf_response_t 	*self;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &body, &body_len, &name, &name_len) == FAILURE) {
		return;
	}

	self = getThis();

	if (yaf_response_alter_body(self, name, name_len, body, body_len, YAF_RESPONSE_PREPEND TSRMLS_CC)) {
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::setHeader($name, $value, $replace = 0)
*/
PHP_METHOD(yaf_response, setHeader) {
	char 		*name, *value;
	uint 		name_len, value_len, response_code;
	zend_bool   rep = 1;
	sapi_header_line ctr={0};

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|bl", &name, &name_len, &value, &value_len, &rep, &response_code) == FAILURE) {
		return;
	}

	ctr.line_len = spprintf(&(ctr.line), 0, "%s: %s", name, value);
	ctr.response_code = response_code;

	if (sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC) == SUCCESS) {
		efree(ctr.line);
		RETURN_TRUE;
	}
	efree(ctr.line);
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto protected Yaf_Response_Abstract::setAllHeaders(void)
*/
PHP_METHOD(yaf_response, setAllHeaders) {
	zval 	*headers;
	zval 	**entry;
	char 	*header_name;
	uint 	header_name_len;
	ulong 	num_key;
	HashPosition pos;
	sapi_header_line ctr={0};

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &headers) == FAILURE) {
		return;
	}

	if (IS_ARRAY != Z_TYPE_P(headers)) {
		RETURN_FALSE;
	}

	for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(headers), &pos);
			zend_hash_get_current_data_ex(Z_ARRVAL_P(headers), (void **)&entry, &pos) == SUCCESS;
			zend_hash_move_forward_ex(Z_ARRVAL_P(headers), &pos)) {
		if (zend_hash_get_current_key_ex(Z_ARRVAL_P(headers), &header_name, &header_name_len, &num_key, 0, &pos) != HASH_KEY_IS_STRING) {
				continue;
			}

		convert_to_string_ex(entry);
		ctr.line_len = spprintf(&(ctr.line), 0, "%s: %s", header_name, Z_STRVAL_PP(entry));

		if (sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC) == SUCCESS) {
			efree(ctr.line);
			continue;
		}
		efree(ctr.line);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ php_head_apply_header_list_to_hash
   Turn an llist of sapi_header_struct headers into a numerically indexed zval hash */
static void php_head_apply_header_list_to_hash(void *data, void *arg TSRMLS_DC)
{
        sapi_header_struct *sapi_header = (sapi_header_struct *)data;

        if (arg && sapi_header) {
                add_next_index_string((zval *)arg, (char *)(sapi_header->header), 1);
        }
}

/* {{{ php_heade_apply_find_header_name 
	through header name find header to list */
static void php_heade_apply_find_header_name_to_hash(zend_llist *l, char *name, int name_len, zval *arg TSRMLS_DC) {
	zend_llist_element *element;
	sapi_header_struct *sapi_header;	

	for (element = l->head; element; element = element->next) {

		sapi_header = (sapi_header_struct *)element->data;
		
		if (!strncasecmp(sapi_header->header, name, name_len)) {
			add_next_index_string(arg, (sapi_header->header), 1);
		}

	}
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::getHeader(void)
*/
PHP_METHOD(yaf_response, getHeader) {
	char 	*name, *name_head;
	uint 	name_len, name_head_len;
	sapi_header_line ctr = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
		return;
	}

	if (!&SG(sapi_headers).headers) {
		RETURN_FALSE;
	}

	array_init(return_value);

	if (!ZEND_NUM_ARGS()) {
		zend_llist_apply_with_argument(&SG(sapi_headers).headers, php_head_apply_header_list_to_hash, return_value TSRMLS_CC);
	} else {
		name_head_len = spprintf(&name_head, 0, "%s:", name);
		php_heade_apply_find_header_name_to_hash(&SG(sapi_headers).headers, name_head, name_head_len, return_value TSRMLS_CC);
	}
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::clearHeaders(string $header = null)
*/
PHP_METHOD(yaf_response, clearHeaders) {
	char 	*name;
	uint 	name_len;
	sapi_header_line ctr = {0};

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
    	return;
    }

    if (ZEND_NUM_ARGS()) {
		ctr.line 	 = name;
    	ctr.line_len = name_len;
    }

    RETURN_BOOL(!sapi_header_op(SAPI_HEADER_DELETE_ALL, &ctr TSRMLS_CC));    	
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::setRedirect(string $url)
*/
PHP_METHOD(yaf_response, setRedirect) {
	char 	*url;
	uint 	url_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &url, &url_len) == FAILURE) {
		return;
	}

	if (!url_len) {
		RETURN_FALSE;
	}

	RETURN_BOOL(yaf_response_set_redirect(getThis(), url, url_len TSRMLS_CC));
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::setBody($body, $name = NULL)
*/
PHP_METHOD(yaf_response, setBody) {
	char		*body, *name = NULL;
	uint		body_len, name_len = 0;
	yaf_response_t 	*self;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &body, &body_len, &name, &name_len) == FAILURE) {
		return;
	}

	self = getThis();

	if (yaf_response_alter_body(self, name, name_len, body, body_len, YAF_RESPONSE_REPLACE TSRMLS_CC)) {
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::clearBody(string $name = NULL)
*/
PHP_METHOD(yaf_response, clearBody) {
	char *name = NULL;
	uint name_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
		return;
	}
	if (yaf_response_clear_body(getThis(), name, name_len TSRMLS_CC)) {
		RETURN_ZVAL(getThis(), 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::getBody(string $name = NULL)
 */
PHP_METHOD(yaf_response, getBody) {
	zval *body = NULL;
	zval *name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &name) == FAILURE) {
		return;
	}

	if (!name) {
		body = yaf_response_get_body(getThis(), YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODY, sizeof(YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODY) - 1 TSRMLS_CC);
	} else {
		if (ZVAL_IS_NULL(name)) {
			body = yaf_response_get_body(getThis(), NULL, 0 TSRMLS_CC);
		} else {
			convert_to_string_ex(&name);
			body = yaf_response_get_body(getThis(), Z_STRVAL_P(name), Z_STRLEN_P(name) TSRMLS_CC);
		}
	}

	if (body) {
		RETURN_ZVAL(body, 1, 0);
	}

	RETURN_EMPTY_STRING();
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::response(void)
 */
PHP_METHOD(yaf_response, response) {
	RETURN_BOOL(yaf_response_send(getThis() TSRMLS_CC));
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::__toString(void)
 */
PHP_METHOD(yaf_response, __toString) {
	zval delim;
	zval *zbody = zend_read_property(yaf_response_ce, getThis(), ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1 TSRMLS_CC);

	ZVAL_EMPTY_STRING(&delim);
	php_implode(&delim, zbody, return_value TSRMLS_CC);
	zval_dtor(&delim);
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::__clone(void)
*/
PHP_METHOD(yaf_response, __clone) {
}
/* }}} */

/** {{{ yaf_response_methods
*/
zend_function_entry yaf_response_methods[] = {
	PHP_ME(yaf_response, __construct, 	yaf_response_void_arginfo, 		ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(yaf_response, __destruct,  	yaf_response_void_arginfo, 		ZEND_ACC_PUBLIC|ZEND_ACC_DTOR)
	PHP_ME(yaf_response, __clone,		NULL, 					ZEND_ACC_PRIVATE)
	PHP_ME(yaf_response, __toString,	NULL, 					ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, setBody,		yaf_response_set_body_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, appendBody,	yaf_response_set_body_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, prependBody,	yaf_response_set_body_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, clearBody,		yaf_response_clear_body_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, getBody,		yaf_response_get_body_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, setHeader,		yaf_response_set_header_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, setAllHeaders,	yaf_response_set_all_headers_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, getHeader,		yaf_response_get_header_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, clearHeaders, 	yaf_response_clear_headers_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, setRedirect,	yaf_response_set_redirect_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, response,		yaf_response_void_arginfo, 		ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(response) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Response_Abstract", "Yaf\\Response_Abstract", yaf_response_methods);

	yaf_response_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
	yaf_response_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	zend_declare_property_null(yaf_response_ce, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADER), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_response_ce, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_bool(yaf_response_ce, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADEREXCEPTION), 0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_class_constant_stringl(yaf_response_ce, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODYNAME), ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODY) TSRMLS_CC);

	YAF_STARTUP(response_http);
	YAF_STARTUP(response_cli);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
