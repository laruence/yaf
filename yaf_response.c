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

#if PHP_MAJOR_VERSION > 7
#include "yaf_response_arginfo.h"
#else
#include "yaf_response_legacy_arginfo.h"
#endif

#include "responses/yaf_response_http.h"
#include "responses/yaf_response_cli.h"

zend_class_entry *yaf_response_ce;
static zend_object_handlers yaf_response_obj_handlers;

static HashTable *yaf_response_get_properties(yaf_object *obj) /* {{{ */ {
	zval rv;
	HashTable *ht;
	yaf_response_object *response = php_yaf_response_fetch_object(yaf_strip_obj(obj));

	if (!response->properties) {
		ALLOC_HASHTABLE(response->properties);
		zend_hash_init(response->properties, 4, NULL, ZVAL_PTR_DTOR, 0);
		YAF_ALLOW_VIOLATION(response->properties);
	}

	ht = response->properties;
	ZVAL_LONG(&rv, response->code);
	zend_hash_str_update(ht, "response_code:protected", sizeof("response_code:protected") - 1, &rv);

	ZVAL_BOOL(&rv, response->flags & YAF_RESPONSE_HEADER_SENT);
	zend_hash_str_update(ht, "header_sent:protected", sizeof("header_sent:protected") - 1, &rv);

	if (response->std.ce == yaf_response_http_ce) {
		if (response->header) {
			ZVAL_ARR(&rv, response->header);
			GC_ADDREF(response->header);
		} else {
			array_init(&rv);
		}
		zend_hash_str_update(ht, "header:protected", sizeof("header:protected") - 1, &rv);

		if (response->body) {
			ZVAL_ARR(&rv, response->body);
			GC_ADDREF(response->body);
		} else {
			array_init(&rv);
		}
		zend_hash_str_update(ht, "body:protected", sizeof("body:protected") - 1, &rv);
	}

	return ht;
}
/* }}} */

static zval *yaf_response_read_property(yaf_object *obj, void *name, int type, void **cache_slot, zval *rv) /* {{{ */ {
	zend_string *member;
	yaf_response_object *response = php_yaf_response_fetch_object(yaf_strip_obj(obj));

#if PHP_VERSION_ID < 80000
	if (UNEXPECTED(Z_TYPE_P((zval*)name) != IS_STRING)) {
		return &EG(uninitialized_zval);
	}
	member = Z_STR_P((zval*)name);
#else
	member = (zend_string*)name;
#endif

	if (UNEXPECTED(type == BP_VAR_W || type == BP_VAR_RW)) {
		php_error_docref(NULL, E_WARNING,
				"Indirect modification of Yaf_Response internal property '%s' is not allowed", ZSTR_VAL(member));
		return &EG(error_zval);
	}
	
	if (zend_string_equals_literal(member, "response_code")) {
		ZVAL_LONG(rv, response->code);
		return rv;
	}

	return std_object_handlers.read_property(obj, name, type, cache_slot, rv);
}
/* }}} */

static YAF_WRITE_HANDLER yaf_response_write_property(yaf_object *obj, void *name, zval *value, void **cache_slot) /* {{{ */ {
	zend_string *member;
	yaf_response_object *response = php_yaf_response_fetch_object(yaf_strip_obj(obj));

#if PHP_VERSION_ID < 80000
	if (UNEXPECTED(Z_TYPE_P((zval*)name) != IS_STRING)) {
		YAF_WHANDLER_RET(value);
	}
	member = Z_STR_P((zval*)name);
#else
	member = (zend_string*)name;
#endif
	
	if (zend_string_equals_literal(member, "response_code")) {
		if (Z_TYPE_P(value) != IS_LONG) {
			YAF_WHANDLER_RET(value);
		}
		response->code = Z_LVAL_P(value);
		YAF_WHANDLER_RET(value);
	}

	if (zend_string_equals_literal(member, "body") ||
		zend_string_equals_literal(member, "header") ||
		zend_string_equals_literal(member, "header_sent")) {
		php_error_docref(NULL, E_WARNING,
				"Modification of Yaf_Reponse internal property '%s' is not allowed", ZSTR_VAL(member));
		YAF_WHANDLER_RET(value);
	}

	return std_object_handlers.write_property(obj, name, value, cache_slot);
}
/* }}} */

static zend_object *yaf_response_new(zend_class_entry *ce) /* {{{ */ {
	yaf_response_object *response = emalloc(sizeof(yaf_response_object) + zend_object_properties_size(ce));
	
	memset(response, 0, XtOffsetOf(yaf_response_object, std));
	zend_object_std_init(&response->std, ce);
	response->std.handlers = &yaf_response_obj_handlers;
	if (UNEXPECTED(ce->default_properties_count)) {
		object_properties_init(&response->std, ce);
	}

	return &response->std;
}
/* }}} */

static void yaf_response_object_free(zend_object *object) /* {{{ */ {
	yaf_response_object *response = php_yaf_response_fetch_object(object);

	if (response->header) {
		if (GC_DELREF(response->header) == 0) {
			GC_REMOVE_FROM_BUFFER(response->header);
			zend_array_destroy(response->header);
		}
	}
	if (response->body) {
		if (GC_DELREF(response->body) == 0) {
			GC_REMOVE_FROM_BUFFER(response->body);
			zend_array_destroy(response->body);
		}
	}
	if (response->properties) {
		if (GC_DELREF(response->properties) == 0) {
			GC_REMOVE_FROM_BUFFER(response->properties);
			zend_array_destroy(response->properties);
		}
	}
	zend_object_std_dtor(object);
}
/* }}} */

void yaf_response_instance(yaf_response_t *this_ptr, char *sapi_name) /* {{{ */ {
	zend_class_entry *ce;

	if (!yaf_slip_equal(sapi_name, "cli", 4)) {
		ce = yaf_response_http_ce;
	} else {
		ce = yaf_response_cli_ce;
	}
	
	ZVAL_OBJ(this_ptr, yaf_response_new(ce));
}
/* }}} */

static int yaf_response_alter_body_ex(yaf_response_object *response, zend_string *name, zend_string *body, int flag) /* {{{ */ {
	zval rv;

	if (!response->body) {
		ALLOC_HASHTABLE(response->body);
		zend_hash_init(response->body, 8, NULL, ZVAL_PTR_DTOR, 0);
		YAF_ALLOW_VIOLATION(response->body);
update:
		ZVAL_STR_COPY(&rv, body);
		if (EXPECTED(name == NULL)) {
			return zend_hash_update(response->body, YAF_KNOWN_STR(YAF_CONTENT), &rv) != NULL;
		} else {
			return zend_hash_update(response->body, name, &rv) != NULL;
		}
	} else {
		zval *pzval;
		zend_string *obody;

		if (EXPECTED(name == NULL)) {
			pzval = zend_hash_find(response->body, YAF_KNOWN_STR(YAF_CONTENT));
		} else {
			pzval = zend_hash_find(response->body, name);
		}
		if (EXPECTED(pzval == NULL) || flag == YAF_RESPONSE_REPLACE) {
			goto update;
		} else if (Z_TYPE_P(pzval) == IS_STRING) {
			zend_string *result;
			obody = Z_STR_P(pzval);
			result = zend_string_alloc(ZSTR_LEN(obody) + ZSTR_LEN(body), 0);
			if (flag == YAF_RESPONSE_APPEND) {
				memcpy(ZSTR_VAL(result), ZSTR_VAL(obody), ZSTR_LEN(obody));
				memcpy(ZSTR_VAL(result) + ZSTR_LEN(obody), ZSTR_VAL(body), ZSTR_LEN(body) + 1);
			} else {
				memcpy(ZSTR_VAL(result), ZSTR_VAL(body), ZSTR_LEN(body));
				memcpy(ZSTR_VAL(result) + ZSTR_LEN(body), ZSTR_VAL(obody), ZSTR_LEN(obody) + 1);
			}
			zend_string_release(obody);
			ZVAL_STR(pzval, result);
			return 1;
		}
	}
	return 0;
}
/* }}} */

int yaf_response_alter_body(yaf_response_object *response, zend_string *name, zend_string *body, int flag) /* {{{ */ {
	zend_class_entry *ce = response->std.ce;
	if (EXPECTED(ce == yaf_response_http_ce || ce == yaf_response_cli_ce)) {
		return yaf_response_alter_body_ex(response, name, body, flag);
	} else {
		zval obj, arg, ret;

		ZVAL_OBJ(&obj, &response->std);
		ZVAL_STR(&arg, body);
#if PHP_VERSION_ID < 80000
		zend_call_method_with_1_params(&obj, ce, NULL, "appendbody", &ret, &arg);
#else
        zend_call_method_with_1_params(Z_OBJ(obj), ce, NULL, "appendbody", &ret, &arg);
#endif
		if (UNEXPECTED(EG(exception))) {
			return 0;
		}
		zval_ptr_dtor(&ret);
		return 1;
	}
}
/* }}} */

static void yaf_response_clear_body_ex(yaf_response_object *response, zend_string *name) /* {{{ */ {
	if (response->body) {
		if (name) {
			zend_hash_del(response->body, name);
		} else {
			zend_hash_clean(response->body);
		}
	}
}
/* }}} */

int yaf_response_clear_body(yaf_response_object *response, zend_string *name) /* {{{ */ {
	zend_class_entry *ce = response->std.ce;
	if (EXPECTED(ce == yaf_response_http_ce || ce == yaf_response_cli_ce)) {
		yaf_response_clear_body_ex(response, name);
		return 1;
	} else {
		zval obj, arg, ret;

		ZVAL_OBJ(&obj, &response->std);
		if (name) {
			ZVAL_STR(&arg, name);
#if PHP_VERSION_ID < 80000
            zend_call_method_with_1_params(&obj, ce, NULL, "clearbody", &ret, &arg);
#else
            zend_call_method_with_1_params(Z_OBJ(obj), ce, NULL, "clearbody", &ret, &arg);
#endif
		} else {
#if PHP_VERSION_ID < 80000
			zend_call_method_with_0_params(&obj, ce, NULL, "clearbody", &ret);
#else
            zend_call_method_with_0_params(Z_OBJ(obj), ce, NULL, "clearbody", &ret);
#endif
		}
		if (UNEXPECTED(EG(exception))) {
			return 0;
		}
		zval_ptr_dtor(&ret);
		return 1;
	}
}
/* }}} */

zval *yaf_response_get_body(yaf_response_object *response, zend_string *name) /* {{{ */{
	if (response->body) {
		return zend_hash_find(response->body, name);
	}
	return NULL;
}
/* }}} */

static int yaf_response_send(yaf_response_object *response) /* {{{ */ {
	zval *val;

	if (response->body) {
		ZEND_HASH_FOREACH_VAL(response->body, val) {
			if (UNEXPECTED(Z_TYPE_P(val) != IS_STRING)) {
				continue;
			}
			php_write(Z_STRVAL_P(val), Z_STRLEN_P(val));
		} ZEND_HASH_FOREACH_END();
	}

	return 1;
}
/* }}} */

int yaf_response_response(yaf_response_object *response) /* {{{ */ {
	zend_class_entry *ce = response->std.ce;
	if (EXPECTED(ce == yaf_response_http_ce)) {
		return yaf_response_http_send(response);
	} else if (ce == yaf_response_cli_ce) {
		return yaf_response_send(response);
	} else {
		zval obj, ret;

		ZVAL_OBJ(&obj, &response->std);
#if PHP_VERSION_ID < 80000
		zend_call_method_with_0_params(&obj, ce, NULL, "response", &ret);
#else
        zend_call_method_with_0_params(Z_OBJ(obj), ce, NULL, "response", &ret);
#endif
		if (UNEXPECTED(EG(exception))) {
			return 0;
		}
		zval_ptr_dtor(&ret);
		return 1;
	}
}
/* }}} */

/** {{{ proto private Yaf_Response_Abstract::__construct()
*/
PHP_METHOD(yaf_response, __construct) {
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

	if (yaf_response_alter_body_ex(Z_YAFRESPONSEOBJ_P(getThis()), name, body, YAF_RESPONSE_APPEND)) {
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
	zend_string *name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S!", &name) == FAILURE) {
		return;
	}
	yaf_response_clear_body_ex(Z_YAFRESPONSEOBJ_P(getThis()), name);

	RETURN_ZVAL(getThis(), 1, 0);
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
		body = yaf_response_get_body(response, YAF_KNOWN_STR(YAF_CONTENT));
	} else {
		if (ZVAL_IS_NULL(name)) {
			if (response->body) {
				RETVAL_ARR(response->body);
				GC_ADDREF(response->body);
				return;
			}
#if PHP_VERSION_ID < 70400
			array_init(return_value);
			return;
#else
			RETURN_EMPTY_ARRAY();
#endif
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
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_BOOL(yaf_response_send(Z_YAFRESPONSEOBJ_P(getThis())));
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::__toString(void)
 */
PHP_METHOD(yaf_response, __toString) {
	zval rv;
	yaf_response_object *response = Z_YAFRESPONSEOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (response->body) {
		ZVAL_ARR(&rv, response->body);
#if PHP_VERSION_ID < 80000
		php_implode(ZSTR_EMPTY_ALLOC(), &rv, return_value);
#else
        php_implode(ZSTR_EMPTY_ALLOC(), Z_ARR(rv), return_value);
#endif
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/** {{{ yaf_response_methods
*/
zend_function_entry yaf_response_methods[] = {
	PHP_ME(yaf_response, __construct, arginfo_class_Yaf_Response_Abstract___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(yaf_response, __toString, arginfo_class_Yaf_Response_Abstract___toString, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, setBody, arginfo_class_Yaf_Response_Abstract_setBody, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, appendBody, arginfo_class_Yaf_Response_Abstract_appendBody, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, prependBody, arginfo_class_Yaf_Response_Abstract_prependBody, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, clearBody, arginfo_class_Yaf_Response_Abstract_clearBody, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, getBody, arginfo_class_Yaf_Response_Abstract_getBody, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, response, arginfo_class_Yaf_Response_Abstract_response, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(response) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Response_Abstract", "Yaf\\Response_Abstract", yaf_response_methods);

	yaf_response_ce = zend_register_internal_class_ex(&ce, NULL);
	yaf_response_ce->create_object = yaf_response_new;
#if PHP_VERSION_ID < 80100
	yaf_response_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
	yaf_response_ce->serialize = zend_class_serialize_deny;
	yaf_response_ce->unserialize = zend_class_unserialize_deny;
#else
	yaf_response_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS | ZEND_ACC_NOT_SERIALIZABLE;
#endif

	memcpy(&yaf_response_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_response_obj_handlers.offset = XtOffsetOf(yaf_response_object, std);
	yaf_response_obj_handlers.free_obj = yaf_response_object_free;
	yaf_response_obj_handlers.clone_obj = NULL;
	yaf_response_obj_handlers.get_gc = yaf_fake_get_gc;
	yaf_response_obj_handlers.get_properties = yaf_response_get_properties;
	yaf_response_obj_handlers.read_property = (zend_object_read_property_t)yaf_response_read_property;
	yaf_response_obj_handlers.write_property = (zend_object_write_property_t)yaf_response_write_property;

	zend_declare_class_constant_string(yaf_response_ce, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODYNAME), YAF_KNOWN_CHARS(YAF_CONTENT));

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
