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
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_response_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_get_body_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_set_body_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, body)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_clear_body_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ yaf_response_t * yaf_response_instance(yaf_response_t *this_ptr, char *sapi_name)
 */
yaf_response_t * yaf_response_instance(yaf_response_t *this_ptr, char *sapi_name) {
	zval 			header, body;
	zend_class_entry 	*ce;
	yaf_response_t 		*instance;

	if (strncasecmp(sapi_name, "cli", 4)) {
		ce = yaf_response_http_ce;
	} else {
		ce = yaf_response_cli_ce;
	}

    instance = this_ptr;
    if (ZVAL_IS_NULL(this_ptr)) {
        object_init_ex(instance, ce);
    }

	array_init(&header);
	zend_update_property(ce, instance, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADER), &header);
	zval_ptr_dtor(&header);

	array_init(&body);
	zend_update_property(ce, instance, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), &body);
	zval_ptr_dtor(&body);

	return instance;
}
/* }}} */

/** {{{ static int yaf_response_set_body(yaf_response_t *response, char *name, int name_len, char *body, long body_len)
 */
#if 0
static int yaf_response_set_body(yaf_response_t *response, char *name, int name_len, char *body, long body_len) {
	zval *zbody;
	zend_class_entry *response_ce;

	if (!body_len) {
		return 1;
	}

	response_ce = Z_OBJCE_P(response);

	zbody = zend_read_property(response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1);

	zval_ptr_dtor(&zbody);

	MAKE_STD_ZVAL(zbody);
	ZVAL_STRINGL(zbody, body, body_len, 1);

	zend_update_property(response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), zbody);

	return 1;
}
#endif
/* }}} */

/** {{{ int yaf_response_alter_body(yaf_response_t *response, zend_string *name, zend_string *body, int flag)
 */
int yaf_response_alter_body(yaf_response_t *response, zend_string *name, zend_string *body, int flag) {
	zval *zbody, *pzval;
	uint  free_name = 0;
	zend_string *obody;

	if (ZSTR_LEN(body) == 0) {
		return 1;
	}

	zbody = zend_read_property(yaf_response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1, NULL);
	if (!name) {
		name = zend_string_init(ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODY), 0);
		free_name = 1;
	}

	if ((pzval = zend_hash_find(Z_ARRVAL_P(zbody), name)) == NULL) {
		obody = NULL;
		pzval = zend_hash_update(Z_ARRVAL_P(zbody), name, &EG(uninitialized_zval));
	} else {
		obody = Z_STR_P(pzval);
	}

	if (obody == NULL) {
		ZVAL_STR_COPY(pzval, body);
	} else {
		zend_string *result;
		size_t result_len;

		switch (flag) {
			case YAF_RESPONSE_PREPEND:
				result_len = ZSTR_LEN(body) + ZSTR_LEN(obody);
				result = zend_string_alloc(result_len, 0);
				memcpy(ZSTR_VAL(result), ZSTR_VAL(body), ZSTR_LEN(body));
				memcpy(ZSTR_VAL(result) + ZSTR_LEN(body), ZSTR_VAL(obody), ZSTR_LEN(obody) + 1);
				zend_string_release(obody);
				ZVAL_NEW_STR(pzval, result);
				break;
			case YAF_RESPONSE_APPEND:
				result_len = ZSTR_LEN(body) + ZSTR_LEN(obody);
				result = zend_string_realloc(obody, result_len, 0);
				memcpy(ZSTR_VAL(result) + ZSTR_LEN(result) - ZSTR_LEN(body), ZSTR_VAL(body), ZSTR_LEN(body) + 1);
				ZVAL_NEW_STR(pzval, result);
				break;
			case YAF_RESPONSE_REPLACE:
			default:
				zend_string_release(obody);
				ZVAL_STR_COPY(pzval, body);
				break;
		}
	}

	if (free_name) {
		zend_string_release(name);
	}

	return 1;
}
/* }}} */

/** {{{ int yaf_response_clear_body(yaf_response_t *response, zend_string *name)
 */
int yaf_response_clear_body(yaf_response_t *response, zend_string *name) {
	zval *zbody;
	zbody = zend_read_property(yaf_response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1, NULL);

	if (name) {
		zend_hash_del(Z_ARRVAL_P(zbody), name);
	} else {
		zend_hash_clean(Z_ARRVAL_P(zbody));
	}
	return 1;
}
/* }}} */

/** {{{ zval * yaf_response_get_body(yaf_response_t *response, zend_string *name)
 */
zval * yaf_response_get_body(yaf_response_t *response, zend_string *name) {
	zval *zbody = zend_read_property(yaf_response_ce,
			response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1, NULL);

	if (!name) {
		return zbody;
	}

	return zend_hash_find(Z_ARRVAL_P(zbody), name);
}
/* }}} */

/** {{{ zval * yaf_response_get_body_str(yaf_response_t *response, char *name, size_t len)
 */
zval * yaf_response_get_body_str(yaf_response_t *response, char *name, size_t len) {
	zval *ret;
	zend_string *n = zend_string_init(name, len, 0);
	ret = yaf_response_get_body(response, n);
	zend_string_release(n);
	return ret;
}
/* }}} */

/** {{{ int yaf_response_send(yaf_response_t *response)
 */
int yaf_response_send(yaf_response_t *response) {
	zval *zbody;
	zval *val;

	zbody = zend_read_property(yaf_response_ce,
			response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1, NULL);

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zbody), val) {
		if (UNEXPECTED(Z_TYPE_P(val) != IS_STRING)) {
			continue;
		}
		php_write(Z_STRVAL_P(val), Z_STRLEN_P(val));
	} ZEND_HASH_FOREACH_END();
	return 1;
}
/* }}} */

/** {{{ proto private Yaf_Response_Abstract::__construct()
*/
PHP_METHOD(yaf_response, __construct) {
	(void)yaf_response_instance(getThis(), sapi_module.name);
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::__destruct(void)
*/
PHP_METHOD(yaf_response, __destruct) {
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::appendBody($body, $name = NULL)
*/
PHP_METHOD(yaf_response, appendBody) {
	zend_string *name = NULL;
	zend_string *body;
	yaf_response_t *self;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &body, &name) == FAILURE) {
		return;
	}

	self = getThis();

	if (yaf_response_alter_body(self, name, body, YAF_RESPONSE_APPEND)) {
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::prependBody($body, $name = NULL)
*/
PHP_METHOD(yaf_response, prependBody) {
	zend_string	*name = NULL;
	zend_string *body;
	yaf_response_t *self;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &body, &name) == FAILURE) {
		return;
	}

	self = getThis();

	if (yaf_response_alter_body(self, name, body, YAF_RESPONSE_PREPEND)) {
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::setHeader($name, $value, $replace = 0)
*/
PHP_METHOD(yaf_response, setHeader) {
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto protected Yaf_Response_Abstract::setAllHeaders(void)
*/
PHP_METHOD(yaf_response, setAllHeaders) {
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::getHeader(void)
*/
PHP_METHOD(yaf_response, getHeader) {
	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::clearHeaders(void)
*/
PHP_METHOD(yaf_response, clearHeaders) {
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::setRedirect(string $url)
*/
PHP_METHOD(yaf_response, setRedirect) {
	char 	*url;
	size_t 	url_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &url, &url_len) == FAILURE) {
		return;
	}

	if (!url_len) {
		RETURN_FALSE;
	}

	RETURN_BOOL(yaf_response_set_redirect(getThis(), url, url_len));
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::setBody($body, $name = NULL)
*/
PHP_METHOD(yaf_response, setBody) {
	zend_string	*name = NULL;
	zend_string *body;
	yaf_response_t *self;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &body, &name) == FAILURE) {
		return;
	}

	self = getThis();

	if (yaf_response_alter_body(self, name, body, YAF_RESPONSE_REPLACE)) {
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::clearBody(string $name = NULL)
*/
PHP_METHOD(yaf_response, clearBody) {
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &name) == FAILURE) {
		return;
	}
	if (yaf_response_clear_body(getThis(), name)) {
		RETURN_ZVAL(getThis(), 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::getBody(string $name = NULL)
 */
PHP_METHOD(yaf_response, getBody) {
	zval *body;
	zval *name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|z", &name) == FAILURE) {
		return;
	}

	if (!name) {
		body = yaf_response_get_body_str(getThis(),
				YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODY, sizeof(YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODY) - 1);
	} else {
		if (ZVAL_IS_NULL(name)) {
			body = yaf_response_get_body(getThis(), NULL);
		} else {
			convert_to_string_ex(name);
			body = yaf_response_get_body(getThis(), Z_STR_P(name));
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
	RETURN_BOOL(yaf_response_send(getThis()));
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::__toString(void)
 */
PHP_METHOD(yaf_response, __toString) {
	zend_string *delim;
	zval *zbody = zend_read_property(yaf_response_ce,
			getThis(), ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1, NULL);

	delim = ZSTR_EMPTY_ALLOC();
	php_implode(delim, zbody, return_value);
	zend_string_release(delim);
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
	PHP_ME(yaf_response, response,		yaf_response_void_arginfo, 			ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(response) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Response_Abstract", "Yaf\\Response_Abstract", yaf_response_methods);

	yaf_response_ce = zend_register_internal_class_ex(&ce, NULL);
	yaf_response_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	zend_declare_property_null(yaf_response_ce, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADER), ZEND_ACC_PROTECTED);
	zend_declare_property_null(yaf_response_ce, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), ZEND_ACC_PROTECTED);
	zend_declare_property_bool(yaf_response_ce, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADEREXCEPTION), 0, ZEND_ACC_PROTECTED);
	zend_declare_class_constant_stringl(yaf_response_ce, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODYNAME), ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODY));

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
