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
zend_object_handlers yaf_response_obj_handlers;

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

static HashTable *yaf_response_get_debug_info(zval *object, int *is_temp) /* {{{ */ {
	zval rv;
	HashTable *ht;
	yaf_response_object *response = Z_YAFRESPONSEOBJ_P(object);

	*is_temp = 1;
	ALLOC_HASHTABLE(ht);
	zend_hash_init(ht, 4, NULL, ZVAL_PTR_DTOR, 0);

	ZVAL_LONG(&rv, response->code);
	zend_hash_str_add(ht, "response_code:protected", sizeof("response_code:protected") - 1, &rv);

	ZVAL_BOOL(&rv, response->header_sent);
	zend_hash_str_add(ht, "header_sent:protected", sizeof("header_sent:protected") - 1, &rv);

	if (Z_OBJCE_P(object) == yaf_response_http_ce) {
		ZVAL_ARR(&rv, zend_array_dup(&response->header));
		zend_hash_str_add(ht, "header:protected", sizeof("header:protected") - 1, &rv);

		ZVAL_ARR(&rv, zend_array_dup(&response->body));
		zend_hash_str_add(ht, "body:protected", sizeof("body:protected") - 1, &rv);
	}

	return ht;
}
/* }}} */

static zval *yaf_response_read_property(zval *zobj, zval *name, int type, void **cache_slot, zval *rv) /* {{{ */ {
	zend_string *member;
	yaf_response_object *response = Z_YAFRESPONSEOBJ_P(zobj);

	if (UNEXPECTED(Z_TYPE_P(name) != IS_STRING)) {
		return &EG(uninitialized_zval);
	}
	
	if (UNEXPECTED(type == BP_VAR_W || type == BP_VAR_RW)) {
		return &EG(error_zval);
	}

	member = Z_STR_P(name);
	
	if (zend_string_equals_literal(member, "response_code")) {
		ZVAL_LONG(rv, response->code);
		return rv;
	}

	return &EG(uninitialized_zval);
}
/* }}} */

static void yaf_response_write_property(zval *zobj, zval *name, zval *value, void **cache_slot) /* {{{ */ {
	zend_string *member;
	yaf_response_object *response = Z_YAFRESPONSEOBJ_P(zobj);

	if (UNEXPECTED(Z_TYPE_P(name) != IS_STRING)) {
		return;
	}

	member = Z_STR_P(name);
	
	if (zend_string_equals_literal(member, "response_code")) {
		if (Z_TYPE_P(value) != IS_LONG) {
			return;
		}
		response->code = Z_LVAL_P(value);
		return;
	}

	return;
}
/* }}} */

static zend_object *yaf_response_new(zend_class_entry *ce) /* {{{ */ {
	yaf_response_object *response = emalloc(sizeof(yaf_response_object) + zend_object_properties_size(ce));
	
	zend_object_std_init(&response->std, ce);
	response->std.handlers = &yaf_response_obj_handlers;

	zend_hash_init(&response->body, 8, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_init(&response->header, 8, NULL, ZVAL_PTR_DTOR, 0);

	response->code = 0;
	response->header_sent = 0;

	return &response->std;
}
/* }}} */

static void yaf_response_object_free(zend_object *object) /* {{{ */ {
	yaf_response_object *response = php_yaf_response_fetch_object(object);

	zend_object_std_dtor(object);
	
	zend_hash_destroy(&response->body);
	zend_hash_destroy(&response->header);
}
/* }}} */

void yaf_response_instance(yaf_response_t *this_ptr, char *sapi_name) /* {{{ */ {
	zend_class_entry *ce;

	if (strncasecmp(sapi_name, "cli", 4)) {
		ce = yaf_response_http_ce;
	} else {
		ce = yaf_response_cli_ce;
	}
	
	ZVAL_OBJ(this_ptr, yaf_response_new(ce));
}
/* }}} */

int yaf_response_alter_body(yaf_response_object *response, zend_string *name, zend_string *body, int flag) /* {{{ */ {
	zval *zbody, rv;
	zend_string *obody;

	if (ZSTR_LEN(body) == 0) {
		return 1;
	}

	if (!name) {
		zbody = zend_hash_str_find(&response->body, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODY));
	} else {
		zbody = zend_hash_find(&response->body, name);
	}

	if (zbody) {
		obody = Z_STR_P(zbody);
	} else {
		obody = NULL;
	}

	if (obody == NULL) {
		obody = zend_string_copy(body);
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
				obody = result;
				break;
			case YAF_RESPONSE_APPEND:
				result_len = ZSTR_LEN(body) + ZSTR_LEN(obody);
				result = zend_string_realloc(obody, result_len, 0);
				memcpy(ZSTR_VAL(result) + ZSTR_LEN(result) - ZSTR_LEN(body), ZSTR_VAL(body), ZSTR_LEN(body) + 1);
				obody = result;
				break;
			case YAF_RESPONSE_REPLACE:
			default:
				zend_string_release(obody);
				obody = body;
				break;
		}
	}

	if (zbody) {
		ZVAL_STR(zbody, obody);
	} else {
		ZVAL_STR(&rv, obody);
		if (!name) {
			zend_hash_str_update(&response->body, YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODY, sizeof(YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODY) - 1, &rv);;
		} else {
			zend_hash_update(&response->body, name, &rv);
		}
	}

	return 1;
}
/* }}} */

int yaf_response_clear_body(yaf_response_object *response, zend_string *name) /* {{{ */ {
	if (name) {
		zend_hash_del(&response->body, name);
	} else {
		zend_hash_clean(&response->body);
	}
	return 1;
}
/* }}} */

zval *yaf_response_get_body(yaf_response_object *response, zend_string *name) /* {{{ */{
	return zend_hash_find(&response->body, name);
}
/* }}} */

zval* yaf_response_get_body_str(yaf_response_object *response, char *name, size_t len) /* {{{ */ {
	return zend_hash_str_find(&response->body, name, len);
}
/* }}} */

int yaf_response_send(yaf_response_object *response) /* {{{ */ {
	zval *val;

	ZEND_HASH_FOREACH_VAL(&response->body, val) {
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &body, &name) == FAILURE) {
		return;
	}

	if (yaf_response_alter_body(Z_YAFRESPONSEOBJ_P(getThis()), name, body, YAF_RESPONSE_APPEND)) {
		RETURN_ZVAL(getThis(), 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::prependBody($body, $name = NULL)
*/
PHP_METHOD(yaf_response, prependBody) {
	zend_string	*name = NULL;
	zend_string *body;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &body, &name) == FAILURE) {
		return;
	}

	if (yaf_response_alter_body(Z_YAFRESPONSEOBJ_P(getThis()), name, body, YAF_RESPONSE_PREPEND)) {
		RETURN_ZVAL(getThis(), 1, 0);
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
	zend_string *url;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &url) == FAILURE) {
		return;
	}

	RETURN_BOOL(yaf_response_set_redirect(Z_YAFRESPONSEOBJ_P(getThis()), url));
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::setBody($body, $name = NULL)
*/
PHP_METHOD(yaf_response, setBody) {
	zend_string	*name = NULL;
	zend_string *body;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &body, &name) == FAILURE) {
		return;
	}

	if (yaf_response_alter_body(Z_YAFRESPONSEOBJ_P(getThis()), name, body, YAF_RESPONSE_REPLACE)) {
		RETURN_ZVAL(getThis(), 1, 0);
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
	if (yaf_response_clear_body(Z_YAFRESPONSEOBJ_P(getThis()), name)) {
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
	yaf_response_object *response = Z_YAFRESPONSEOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|z", &name) == FAILURE) {
		return;
	}

	if (!name) {
		body = yaf_response_get_body_str(response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODY));
	} else {
		if (ZVAL_IS_NULL(name)) {
			RETVAL_ARR(&response->body);
			Z_ADDREF_P(return_value);
			return;
		} else {
			zend_string *t = zval_get_string(name);
			body = yaf_response_get_body(response, t);
			zend_string_release(t);
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
	RETURN_BOOL(yaf_response_send(Z_YAFRESPONSEOBJ_P(getThis())));
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::__toString(void)
 */
PHP_METHOD(yaf_response, __toString) {
	zend_string *delim;
	zval rv;

	delim = ZSTR_EMPTY_ALLOC();

	ZVAL_ARR(&rv, &Z_YAFRESPONSEOBJ_P(getThis())->body);
	php_implode(delim, &rv, return_value);
}
/* }}} */

/** {{{ yaf_response_methods
*/
zend_function_entry yaf_response_methods[] = {
	PHP_ME(yaf_response, __construct, yaf_response_void_arginfo,       ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(yaf_response, __toString,  NULL,                            ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, setBody,     yaf_response_set_body_arginfo,   ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, appendBody,  yaf_response_set_body_arginfo,   ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, prependBody, yaf_response_set_body_arginfo,   ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, clearBody,   yaf_response_clear_body_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, getBody,     yaf_response_get_body_arginfo,   ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, response,    yaf_response_void_arginfo,       ZEND_ACC_PUBLIC)
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

	yaf_response_ce->create_object = yaf_response_new;
	yaf_response_ce->serialize = zend_class_serialize_deny;
	yaf_response_ce->unserialize = zend_class_unserialize_deny;

	memcpy(&yaf_response_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_response_obj_handlers.offset = XtOffsetOf(yaf_response_object, std);
	yaf_response_obj_handlers.free_obj = yaf_response_object_free;
	yaf_response_obj_handlers.clone_obj = NULL;
	yaf_response_obj_handlers.get_debug_info = yaf_response_get_debug_info;
	yaf_response_obj_handlers.read_property = yaf_response_read_property;
	yaf_response_obj_handlers.write_property = yaf_response_write_property;

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
