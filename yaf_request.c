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
#include "main/SAPI.h"
#include "standard/php_string.h" /* for php_basename */
#include "Zend/zend_exceptions.h" /* for zend_exception_get_default */
#include "Zend/zend_interfaces.h" /* for zend_class_serialize_deny */
#include "Zend/zend_smart_str.h"

#include "php_yaf.h"
#include "yaf_application.h"
#include "yaf_request.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_loader.h"
#include "yaf_router.h"

#include "requests/yaf_request_simple.h"
#include "requests/yaf_request_http.h"

zend_class_entry *yaf_request_ce;
static zend_object_handlers yaf_request_obj_handlers;

/** {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(yaf_request_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_set_routed_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_set_module_name_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, module)
	ZEND_ARG_INFO(0, format_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_set_controller_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, controller)
	ZEND_ARG_INFO(0, format_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_set_action_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, action)
	ZEND_ARG_INFO(0, format_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_set_baseuri_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_set_request_uri_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_set_param_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_get_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_set_dispatched_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, dispatched)
ZEND_END_ARG_INFO()
	/* }}} */

static zend_object *yaf_request_new(zend_class_entry *ce) /* {{{ */ {
	yaf_request_object *req = emalloc(sizeof(yaf_request_object) + zend_object_properties_size(ce));

	memset(req, 0, XtOffsetOf(yaf_request_object, std));
	zend_object_std_init(&req->std, ce);
	if (ce->default_properties_count) {
		object_properties_init(&req->std, ce);
	}
	req->std.handlers = &yaf_request_obj_handlers;
	req->uri = ZSTR_EMPTY_ALLOC();

	return &req->std;
}
/* }}} */

static void yaf_request_object_free(zend_object *object) /* {{{ */ {
	yaf_request_object *req = php_yaf_request_fetch_object(object);

	if (req->method) {
		zend_string_release(req->method);
	}
	if (req->module) {
		zend_string_release(req->module);
	}
	if (req->controller) {
		zend_string_release(req->controller);
	}
	if (req->action) {
		zend_string_release(req->action);
	}
	if (req->uri) {
		zend_string_release(req->uri);
	}
	if (req->base_uri) {
		zend_string_release(req->base_uri);
	}
	if (req->language) {
		zend_string_release(req->language);
	}

	if (req->params) {
		if (GC_DELREF(req->params) == 0) {
			GC_REMOVE_FROM_BUFFER(req->params);
			zend_array_destroy(req->params);
		}
	}

	if (req->properties) {
		if (GC_DELREF(req->properties) == 0) {
			GC_REMOVE_FROM_BUFFER(req->properties);
			zend_array_destroy(req->properties);
		}
	}

	zend_object_std_dtor(object);
}
/* }}} */

static HashTable *yaf_request_get_gc(zval *object, zval **table, int *n) /* {{{ */
{
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(object);

	*table = NULL;
	*n = 0;

	return request->params;
}
/* }}} */

static HashTable *yaf_request_get_properties(zval *object) /* {{{ */ {
	zval rv;
	HashTable *ht;
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(object);

	if (!request->properties) {
		ALLOC_HASHTABLE(request->properties);
		zend_hash_init(request->properties, 16, NULL, ZVAL_PTR_DTOR, 0);
		YAF_ALLOW_VIOLATION(request->properties);
	}

	ht = request->properties;
	ZVAL_STR_COPY(&rv, request->method);
	zend_hash_str_update(ht, "method", sizeof("method") - 1, &rv);
	if (request->module) {
		ZVAL_STR_COPY(&rv, request->module);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_update(ht, "module", sizeof("module") - 1, &rv);
	if (request->controller) {
		ZVAL_STR_COPY(&rv, request->controller);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_update(ht, "controller", sizeof("controller") - 1, &rv);
	if (request->action) {
		ZVAL_STR_COPY(&rv, request->action);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_update(ht, "action", sizeof("action") - 1, &rv);

	if (request->uri) {
		ZVAL_STR_COPY(&rv, request->uri);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_update(ht, "uri:protected", sizeof("uri:protected") - 1, &rv);

	if (request->base_uri) {
		ZVAL_STR_COPY(&rv, request->base_uri);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_update(ht, "base_uri:protected", sizeof("base_uri:protected") - 1, &rv);

	ZVAL_BOOL(&rv, yaf_request_is_dispatched(request));
	zend_hash_str_update(ht, "dispatched:protected", sizeof("dispatched:protected") - 1, &rv);

	ZVAL_BOOL(&rv, yaf_request_is_routed(request));
	zend_hash_str_update(ht, "routed:protected", sizeof("routed:protected") - 1, &rv);

	if (request->language) {
		ZVAL_STR_COPY(&rv, request->language);
	} else {
		ZVAL_EMPTY_STRING(&rv);
	}
	zend_hash_str_update(ht, "language:protected", sizeof("language:protected") - 1, &rv);

	if (request->params) {
		ZVAL_ARR(&rv, request->params);
		GC_ADDREF(request->params);
	} else {
#if PHP_VERSION_ID < 70400
		array_init(&rv);
#else
		ZVAL_EMPTY_ARRAY(&rv);
#endif
	}
	zend_hash_str_update(ht, "params:protected", sizeof("params:protected") - 1, &rv);

	return ht;
}
/* }}} */

static zend_always_inline zend_string *yaf_request_get_method(yaf_request_object *request) /* {{{ */ {
	return zend_string_copy(request->method);
}
/* }}} */

static zend_always_inline zend_string *yaf_request_get_module(yaf_request_object *request) /* {{{ */ {
	if (request->module) {
		return zend_string_copy(request->module);
	}
	return NULL;
}
/* }}} */

static zend_always_inline zend_string *yaf_request_get_controller(yaf_request_object *request) /* {{{ */ {
	if (request->controller) {
		return zend_string_copy(request->controller);
	}
	return NULL;
}
/* }}} */

static zend_always_inline zend_string *yaf_request_get_action(yaf_request_object *request) /* {{{ */ {
	if (request->action) {
		return zend_string_copy(request->action);
	}
	return NULL;
}
/* }}} */

static zend_always_inline zend_string *yaf_request_get_uri(yaf_request_object *request) /* {{{ */ {
	if (request->uri) {
		return zend_string_copy(request->uri);
	}
	return NULL;
}
/* }}} */

static zend_always_inline zend_string *yaf_request_get_base_uri(yaf_request_object *request) /* {{{ */ {
	if (request->base_uri) {
		return zend_string_copy(request->base_uri);
	}
	return NULL;
}
/* }}} */

zend_string *yaf_request_get_language(yaf_request_object *request) /* {{{ */ {
	if (!request->language) {
		zval *accept_langs = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, ZEND_STRL("HTTP_ACCEPT_LANGUAGE"));
		if (!accept_langs) {
			return NULL;
		} else if (UNEXPECTED(IS_STRING != Z_TYPE_P(accept_langs) || !Z_STRLEN_P(accept_langs))) {
			return NULL;
		} else {
			char *seg;
			char *ptrptr = NULL;
			unsigned prefer_len = 0;
			double	max_qvlaue = 0;
			char *prefer = NULL;
			char *langs = estrndup(Z_STRVAL_P(accept_langs), Z_STRLEN_P(accept_langs));

			seg = php_strtok_r(langs, ",", &ptrptr);
			while (seg) {
				char *qvalue;
				while (*(seg) == ' ') {
					seg++;
				}
				/* Accept-Language: da, en-gb;q=0.8, en;q=0.7 */
				if ((qvalue = strstr(seg, "q="))) {
					float qval = strtod(qvalue + 2, NULL);
					if (qval > max_qvlaue) {
						max_qvlaue = qval;
						prefer_len = qvalue - seg - 1;
						prefer 	   = seg;
					}
				} else {
					if (max_qvlaue < 1) {
						max_qvlaue = 1;
						prefer_len = strlen(seg);
						prefer 	   = seg;
					}
				}

				seg = php_strtok_r(NULL, ",", &ptrptr);
			}

			if (prefer) {
				request->language = zend_string_init(prefer, prefer_len, 0);
			}
			efree(langs);
		}
	}

	return zend_string_copy(request->language);
}
/* }}} */

static zval* yaf_request_read_property(zval *zobj, zval *name, int type, void **cache_slot, zval *rv) /* {{{ */ {
	zend_string *member;
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(zobj);

	if (UNEXPECTED(Z_TYPE_P(name) != IS_STRING)) {
		return &EG(uninitialized_zval);
	}

	if (UNEXPECTED(type == BP_VAR_W || type == BP_VAR_RW)) {
		php_error_docref(NULL, E_WARNING,
				"Indirect modification of Yaf_Reqeust internal property '%s' is not allowed", Z_STRVAL_P(name));
		return &EG(error_zval);
	}

	member = Z_STR_P(name);

	switch (ZSTR_LEN(member)) {
		case 3:
			if (yaf_slip_equal(ZSTR_VAL(member), ZEND_STRS("uri"))) {
				zend_string *val = yaf_request_get_uri(request);
				if (val) {
					ZVAL_STR(rv, val);
					return rv;
				}
				return &EG(uninitialized_zval);
			}
			break;
		case 6:
			if (yaf_slip_equal(ZSTR_VAL(member), ZEND_STRL("method"))) {
				zend_string *val = yaf_request_get_method(request);
				if (val) {
					ZVAL_STR(rv, val);
					return rv;
				}
				return &EG(uninitialized_zval);
			}
			if (yaf_slip_equal(ZSTR_VAL(member), ZEND_STRL("module"))) {
				zend_string *val = yaf_request_get_module(request);
				if (val) {
					ZVAL_STR(rv, val);
					return rv;
				}
				return &EG(uninitialized_zval);
			}
			if (yaf_slip_equal(ZSTR_VAL(member), ZEND_STRL("action"))) {
				zend_string *val = yaf_request_get_action(request);
				if (val) {
					ZVAL_STR(rv, val);
					return rv;
				}
				return &EG(uninitialized_zval);
			}
			if (yaf_slip_equal(ZSTR_VAL(member), ZEND_STRL("routed"))) {
				ZVAL_BOOL(rv, yaf_request_is_routed(request));
				return rv;
			}
			break;
		case 8:
			if (yaf_slip_equal(ZSTR_VAL(member), ZEND_STRL("base_uri"))) {
				zend_string *val = yaf_request_get_base_uri(request);
				if (val) {
					ZVAL_STR(rv, val);
					return rv;
				}
				return &EG(uninitialized_zval);
			}
			if (yaf_slip_equal(ZSTR_VAL(member), ZEND_STRL("language"))) {
				zend_string *val = yaf_request_get_language(request);
				if (val) {
					ZVAL_STR(rv, val);
					return rv;
				}
				return &EG(uninitialized_zval);
			}
			break;
		case 10:
			if (yaf_slip_equal(ZSTR_VAL(member), ZEND_STRL("controller"))) {
				zend_string *val = yaf_request_get_controller(request);
				if (val) {
					ZVAL_STR(rv, val);
					return rv;
				}
				return &EG(uninitialized_zval);
			}
			if (yaf_slip_equal(ZSTR_VAL(member), ZEND_STRL("dispatched"))) {
				ZVAL_BOOL(rv, yaf_request_is_dispatched(request));
				return rv;
			}
			default:
			break;
	}

	return std_object_handlers.read_property(zobj, name, type, cache_slot, rv);
}
/* }}} */

static inline void yaf_request_set_uri(yaf_request_object *request, zend_string *uri) /* {{{ */ {
	if (request->uri) {
		zend_string_release(request->uri);
	}
	request->uri = zend_string_copy(uri);
}
/* }}} */

const char *yaf_request_strip_base_uri(zend_string *uri, zend_string *base_uri, size_t *len) /* {{{ */ {
	register const char *p = ZSTR_VAL(uri);
	if (strncasecmp(p, ZSTR_VAL(base_uri), ZSTR_LEN(base_uri)) == 0) {
		p += ZSTR_LEN(base_uri);
		if (*p == '\0' || *p == YAF_ROUTER_URL_DELIMIETER || *(--p) == YAF_ROUTER_URL_DELIMIETER) {
			*len = ZSTR_LEN(uri) - (p - ZSTR_VAL(uri));
			return p;
		}
	}
	*len = ZSTR_LEN(uri);
	return ZSTR_VAL(uri);
}
/* }}} */

int yaf_request_set_base_uri(yaf_request_object *request, zend_string *base_uri, zend_string *request_uri) /* {{{ */ {

	if (UNEXPECTED(request->base_uri)) {
		zend_string_release(request->base_uri);
	}

	if (base_uri == NULL) {
		zend_string *basename = NULL;
		zval *script_filename = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, ZEND_STRL("SCRIPT_FILENAME"));
		do {
			if (script_filename && IS_STRING == Z_TYPE_P(script_filename)) {
				zend_string *file_name;
				char *ext;
				size_t ext_len;
				zval *script_name, *phpself_name, *orig_name;
				yaf_application_object *app;

				if (UNEXPECTED((app = yaf_application_instance()) && app->ext)) {
					ext = ZSTR_VAL(app->ext);
					ext_len = ZSTR_LEN(app->ext);
				} else {
					ext = YAF_DEFAULT_EXT;
					ext_len = sizeof(YAF_DEFAULT_EXT) - 1;
				}

				script_name = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, ZEND_STRL("SCRIPT_NAME"));
				file_name = php_basename(Z_STRVAL_P(script_filename), Z_STRLEN_P(script_filename), ext, ext_len);
				if (script_name && EXPECTED(IS_STRING == Z_TYPE_P(script_name))) {
					zend_string	*script = php_basename(Z_STRVAL_P(script_name), Z_STRLEN_P(script_name), NULL, 0);

					if (memcmp(ZSTR_VAL(file_name), ZSTR_VAL(script), MIN(ZSTR_LEN(file_name), ZSTR_LEN(script))) == 0) {
						basename = zend_string_copy(Z_STR_P(script_name));
						zend_string_release(file_name);
						zend_string_release(script);
						break;
					}
					zend_string_release(script);
				}

				phpself_name = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, ZEND_STRL("PHP_SELF"));
				if (phpself_name && EXPECTED(IS_STRING == Z_TYPE_P(phpself_name))) {
					zend_string *phpself = php_basename(Z_STRVAL_P(phpself_name), Z_STRLEN_P(phpself_name), NULL, 0);
					if (memcmp(ZSTR_VAL(file_name), ZSTR_VAL(phpself), MIN(ZSTR_LEN(file_name), ZSTR_LEN(file_name))) == 0) {
						basename = zend_string_copy(Z_STR_P(phpself_name));
						zend_string_release(file_name);
						zend_string_release(phpself);
						break;
					}
					zend_string_release(phpself);
				}

				orig_name = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, ZEND_STRL("ORIG_SCRIPT_NAME"));
				if (orig_name && IS_STRING == Z_TYPE_P(orig_name)) {
					zend_string *orig = php_basename(Z_STRVAL_P(orig_name), Z_STRLEN_P(orig_name), NULL, 0);
					if (memcmp(ZSTR_VAL(file_name), ZSTR_VAL(orig), MIN(ZSTR_LEN(file_name), ZSTR_LEN(file_name))) == 0) {
						basename = zend_string_copy(Z_STR_P(orig_name));
						zend_string_release(file_name);
						zend_string_release(orig);
						break;
					}
					zend_string_release(orig);
				}
				zend_string_release(file_name);
			}
		} while (0);

		if (basename) {
			if (memcmp(ZSTR_VAL(request_uri), ZSTR_VAL(basename), MIN(ZSTR_LEN(basename), ZSTR_LEN(request_uri))) == 0) {
				if (ZSTR_VAL(basename)[ZSTR_LEN(basename) - 1] == '/') {
					zend_string *sanitized_uri = zend_string_init(ZSTR_VAL(basename), ZSTR_LEN(basename) - 1, 0);
					zend_string_release(basename);
					basename = sanitized_uri;
				}
				request->base_uri = basename;
				return 1;
			} else {
				zend_string *dir = zend_string_init(ZSTR_VAL(basename), ZSTR_LEN(basename), 0); /* php_dirname might alter the string */

				zend_string_release(basename);
				ZSTR_LEN(dir) = php_dirname(ZSTR_VAL(dir), ZSTR_LEN(dir));
				if (*(ZSTR_VAL(dir) + ZSTR_LEN(dir) - 1) == '/') {
					ZSTR_VAL(dir)[ZSTR_LEN(dir) - 1] = '\0';
					ZSTR_LEN(dir)--;
				}
				if (ZSTR_LEN(dir)) {
					if (memcmp(ZSTR_VAL(request_uri), ZSTR_VAL(dir), MIN(ZSTR_LEN(dir), ZSTR_LEN(request_uri))) == 0) {
						request->base_uri = dir;
						return 1;
					}
				}
				zend_string_release(dir);
			}
		}
		request->base_uri = NULL;
	} else {
		zend_string *sanitized_uri = NULL;
		if (UNEXPECTED(ZSTR_VAL(base_uri)[ZSTR_LEN(base_uri) - 1] == '/')) {
			sanitized_uri = zend_string_init(ZSTR_VAL(base_uri), ZSTR_LEN(base_uri) - 1, 0);
			base_uri = sanitized_uri;
		} else {
			zend_string_copy(base_uri);
		}
		request->base_uri = base_uri;
	}
	return 1;
}
/* }}} */

static YAF_WRITE_HANDLER yaf_request_write_property(zval *zobj, zval *name, zval *value, void **cache_slot) /* {{{ */ {
	zend_string *member;
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(zobj);

	if (UNEXPECTED(Z_TYPE_P(name) != IS_STRING)) {
		YAF_WHANDLER_RET(value);
	}

	member = Z_STR_P(name);

	if (zend_string_equals_literal(member, "method")) {
		if (UNEXPECTED(Z_TYPE_P(value) != IS_STRING || Z_STRLEN_P(value) == 0)) {
			YAF_WHANDLER_RET(value);
		}
		zend_string_release(request->method);
		request->method = zend_string_copy(Z_STR_P(value));
		YAF_WHANDLER_RET(value);
	}

	if (zend_string_equals_literal(member, "module")) {
		if (UNEXPECTED(Z_TYPE_P(value) != IS_STRING || Z_STRLEN_P(value) == 0)) {
			YAF_WHANDLER_RET(value);
		}
		yaf_request_set_module(request, Z_STR_P(value));
		YAF_WHANDLER_RET(value);
	}

	if (zend_string_equals_literal(member, "controller")) {
		if (UNEXPECTED(Z_TYPE_P(value) != IS_STRING || Z_STRLEN_P(value) == 0)) {
			YAF_WHANDLER_RET(value);
		}
		yaf_request_set_controller(request, Z_STR_P(value));
		YAF_WHANDLER_RET(value);
	}

	if (zend_string_equals_literal(member, "action")) {
		if (UNEXPECTED(Z_TYPE_P(value) != IS_STRING || Z_STRLEN_P(value) == 0)) {
			YAF_WHANDLER_RET(value);
		}
		yaf_request_set_action(request, Z_STR_P(value));
		YAF_WHANDLER_RET(value);
	}

	if (zend_string_equals_literal(member, "uri") ||
		zend_string_equals_literal(member, "base_uri") ||
		zend_string_equals_literal(member, "dispatched") ||
		zend_string_equals_literal(member, "language") ||
		zend_string_equals_literal(member, "routed") ||
		zend_string_equals_literal(member, "params")) {
		php_error_docref(NULL, E_WARNING,
				"Modification of Yaf_Request internal property '%s' is not allowed", Z_STRVAL_P(name));
		YAF_WHANDLER_RET(value);
	}

	return std_object_handlers.write_property(zobj, name, value, cache_slot);
}
/* }}} */

void yaf_request_instance(yaf_request_t *this_ptr, zend_string *request_uri) /* {{{ */ {
	zend_object *req = yaf_request_new(yaf_request_http_ce);

	yaf_request_http_init(php_yaf_request_fetch_object(req), NULL, request_uri);

	ZVAL_OBJ(this_ptr, req);
}
/* }}} */

void yaf_request_set_mvc(yaf_request_object *request, zend_string *module, zend_string *controller, zend_string *action, zend_array *params) /* {{{ */ {
	if (module) {
		yaf_request_set_module(request, module);
	}
	if (controller) {
		yaf_request_set_controller(request, controller);
	}
	if (action) {
		yaf_request_set_action(request, action);
	}
	if (params) {
		if (!request->params) {
			ALLOC_HASHTABLE(request->params);
			zend_hash_init(request->params, zend_hash_num_elements(params), NULL, ZVAL_PTR_DTOR, 0);
			YAF_ALLOW_VIOLATION(request->params);
		}
		zend_hash_copy(request->params, params, (copy_ctor_func_t) zval_add_ref);
	}
}
/* }}} */

void yaf_request_clean_params(yaf_request_object *request) /* {{{ */ {
	if (request->params) {
		zend_hash_clean(request->params);
	}
}
/* }}} */

static inline zval* yaf_request_fetch_container(unsigned type) /* {{{ */ {
	zval *container;
	zend_bool jit_initialization = PG(auto_globals_jit);

	switch (type) {
		case YAF_GLOBAL_VARS_POST:
			container = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_POST"));
			break;
		case YAF_GLOBAL_VARS_GET:
			container = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_GET"));
			break;
		case YAF_GLOBAL_VARS_COOKIE:
			container = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_COOKIE"));
			break;
		case YAF_GLOBAL_VARS_FILES:
			container = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_FILES"));
			break;
		case YAF_GLOBAL_VARS_SERVER:
			if (jit_initialization) {
				zend_is_auto_global_str(ZEND_STRL("_SERVER"));
			}
			container = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_SERVER"));
			break;
		case YAF_GLOBAL_VARS_REQUEST:
			if (jit_initialization) {
				zend_is_auto_global_str(ZEND_STRL("_REQUEST"));
			}
			container = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_REQUEST"));
			break;
		case YAF_GLOBAL_VARS_ENV:
			if (jit_initialization) {
				zend_is_auto_global_str(ZEND_STRL("_ENV"));
			}
			container = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_ENV"));
			break;
		default:
			return NULL;
	}

	if (UNEXPECTED(Z_TYPE_P(container) != IS_ARRAY)) {
		if (Z_TYPE_P(container) != IS_REFERENCE || Z_TYPE_P(Z_REFVAL_P(container)) != IS_ARRAY) {
			return NULL;
		}
		return Z_REFVAL_P(container);
	}
	return container;
}
/* }}} */

ZEND_HOT zval *yaf_request_query_str(unsigned type, const char *name, size_t len) /* {{{ */ {
	zval *container = yaf_request_fetch_container(type);

	if (UNEXPECTED(!container)) {
		return NULL;
	}

	if (UNEXPECTED(name == NULL)) {
		return container;
	}

	return zend_hash_str_find(Z_ARRVAL_P(container), name, len);
}
/* }}} */

ZEND_HOT zval *yaf_request_query(unsigned type, zend_string *name) /* {{{ */ {
	zval *container = yaf_request_fetch_container(type);

	if (UNEXPECTED(!container)) {
		return NULL;
	}

	if (UNEXPECTED(name == NULL)) {
		return container;
	}

	return zend_hash_find(Z_ARRVAL_P(container), name);
}
/* }}} */

int yaf_request_del_param(yaf_request_object *request, zend_string *key) /* {{{ */ {
	if (request->params) {
		return zend_hash_del(request->params, key);
	}
	return 0;
}
/* }}} */

int yaf_request_set_params_single(yaf_request_object *request, zend_string *key, zval *value) /* {{{ */ {
	if (!request->params) {
		ALLOC_HASHTABLE(request->params);
		zend_hash_init(request->params, 8, NULL, ZVAL_PTR_DTOR, 0);
		YAF_ALLOW_VIOLATION(request->params);
	}
	if ((zend_hash_update(request->params, key, value)) != NULL) {
		Z_TRY_ADDREF_P(value);
		return 1;
	}
	return 0;
}
/* }}} */

int yaf_request_set_params_multi(yaf_request_object *request, zval *values) /* {{{ */ {
	zval *entry;
	zend_string *key;
	ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(values), key, entry) {
		if (UNEXPECTED(key == NULL)) {
			continue;
		}
		yaf_request_set_params_single(request, key, entry);
	} ZEND_HASH_FOREACH_END();
	return 0;
}
/* }}} */

zval *yaf_request_get_param(yaf_request_object *request, zend_string *key) /* {{{ */ {
	if (request->params) {
		return zend_hash_find(request->params, key);
	}
	return NULL;
}
/* }}} */

zval *yaf_request_get_param_str(yaf_request_object *request, const char *key, size_t len) /* {{{ */ {
	if (request->params) {
		return zend_hash_str_find(request->params, key, len);
	}
	return NULL;
}
/* }}} */

const char *yaf_request_get_request_method(void) /* {{{ */ {
	if (SG(request_info).request_method) {
		return SG(request_info).request_method;
	}
#if 0
	else if (yaf_slip_equal(sapi_module.name, "cli", 4)) {
		return "CLI";
	} else {
		return "UNKNOWN";
	}
#endif
	return "CLI";
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isGet(void)
*/
YAF_REQUEST_IS_METHOD(Get);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isPost(void)
*/
YAF_REQUEST_IS_METHOD(Post);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isPut(void)
*/
YAF_REQUEST_IS_METHOD(Put);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isDelete(void)
*/
YAF_REQUEST_IS_METHOD(Delete);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isPatch(void)
*/
YAF_REQUEST_IS_METHOD(Patch);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isHead(void)
*/
YAF_REQUEST_IS_METHOD(Head);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isOptions(void)
*/
YAF_REQUEST_IS_METHOD(Options);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isCli(void)
*/
YAF_REQUEST_IS_METHOD(Cli);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isXmlHttpRequest()
*/
PHP_METHOD(yaf_request, isXmlHttpRequest) {
	zend_string *name;
	zval * header;
	name = zend_string_init("HTTP_X_REQUESTED_WITH", sizeof("HTTP_X_REQUESTED_WITH") - 1, 0);
	header = yaf_request_query(YAF_GLOBAL_VARS_SERVER, name);
	zend_string_release(name);
	if (header && Z_TYPE_P(header) == IS_STRING
			&& strncasecmp("XMLHttpRequest", Z_STRVAL_P(header), Z_STRLEN_P(header)) == 0) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getQuery(mixed $name, mixed $default = NULL)
*/
YAF_REQUEST_METHOD(yaf_request, Query, 	YAF_GLOBAL_VARS_GET);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getPost(mixed $name, mixed $default = NULL)
*/
YAF_REQUEST_METHOD(yaf_request, Post,  	YAF_GLOBAL_VARS_POST);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getRequet(mixed $name, mixed $default = NULL)
*/
YAF_REQUEST_METHOD(yaf_request, Request, YAF_GLOBAL_VARS_REQUEST);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getFiles(mixed $name, mixed $default = NULL)
*/
YAF_REQUEST_METHOD(yaf_request, Files, 	YAF_GLOBAL_VARS_FILES);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getCookie(mixed $name, mixed $default = NULL)
*/
YAF_REQUEST_METHOD(yaf_request, Cookie, 	YAF_GLOBAL_VARS_COOKIE);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getRaw()
*/
PHP_METHOD(yaf_request, getRaw) {
	php_stream *s;
	smart_str raw_data = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	s = SG(request_info).request_body;
	if (!s || FAILURE == php_stream_rewind(s)) {
		RETURN_FALSE;
	}

	while (!php_stream_eof(s)) {
		char buf[512];
		size_t len = php_stream_read(s, buf, sizeof(buf));

		if (len && len != (size_t) -1) {
			smart_str_appendl(&raw_data, buf, len);
		}
	}

	if (raw_data.s) {
		smart_str_0(&raw_data);
		RETURN_STR(raw_data.s);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::get(mixed $name, mixed $default)
 * params -> post -> get -> cookie -> server
 */
PHP_METHOD(yaf_request, get) {
	zend_string	*name;
	zval *def = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &name, &def) == FAILURE) {
		return;
	} else {
		zval *value = yaf_request_get_param(Z_YAFREQUESTOBJ_P(getThis()), name);
		if (value) {
			RETURN_ZVAL(value, 1, 0);
		} else {
			zval *params = NULL;
			zval *pzval	= NULL;

			YAF_GLOBAL_VARS_TYPE methods[4] = {
				YAF_GLOBAL_VARS_POST,
				YAF_GLOBAL_VARS_GET,
				YAF_GLOBAL_VARS_COOKIE,
				YAF_GLOBAL_VARS_SERVER
			};

			{
				int i = 0;
				for (;i < 4; i++) {
					params = &PG(http_globals)[methods[i]];
					if (params && Z_TYPE_P(params) == IS_ARRAY) {
						if ((pzval = zend_hash_find(Z_ARRVAL_P(params), name)) != NULL ){
							RETURN_ZVAL(pzval, 1, 0);
						}
					}
				}

			}
			if (def) {
				RETURN_ZVAL(def, 1, 0);
			}
		}
	}
	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getEnv(mixed $name, mixed $default = NULL)
*/
YAF_REQUEST_METHOD(yaf_request, Env, 	YAF_GLOBAL_VARS_ENV);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getServer(mixed $name, mixed $default = NULL)
*/
YAF_REQUEST_METHOD(yaf_request, Server, YAF_GLOBAL_VARS_SERVER);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getModuleName(void)
*/
PHP_METHOD(yaf_request, getModuleName) {
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(getThis());
	zend_string *module = yaf_request_get_module(request);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (UNEXPECTED(module == NULL)) {
		RETURN_NULL();
	}

	RETURN_STR(module);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getControllerName(void)
*/
PHP_METHOD(yaf_request, getControllerName) {
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(getThis());
	zend_string *controller = yaf_request_get_controller(request);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (UNEXPECTED(controller == NULL)) {
		RETURN_NULL();
	}

	RETURN_STR(controller);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getActionName(void)
*/
PHP_METHOD(yaf_request, getActionName) {
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(getThis());
	zend_string *action = yaf_request_get_action(request);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (UNEXPECTED(action == NULL)) {
		RETURN_NULL();
	}

	RETURN_STR(action);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::setModuleName(string $module, bool $format_name = true)
*/
PHP_METHOD(yaf_request, setModuleName) {
	zend_string *module;
	zend_bool format_name = 1;
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|b", &module, &format_name) == FAILURE) {
		return;
	}

	if (EXPECTED(ZSTR_LEN(module))) {
		if (format_name) {
			yaf_request_set_module(request, module);
		} else {
			if (request->module) {
				zend_string_release(request->module);
				request->module = zend_string_copy(module);
			}
		}
	}

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::setControllerName(string $controller, bool $format_name = true)
*/
PHP_METHOD(yaf_request, setControllerName) {
	zend_string *controller;
	zend_bool format_name = 1;
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|b", &controller, &format_name) == FAILURE) {
		return;
	}

	if (EXPECTED(ZSTR_LEN(controller))) {
		if (format_name) {
			yaf_request_set_controller(request, controller);
		} else {
			if (request->controller) {
				zend_string_release(request->controller);
				request->controller = zend_string_copy(controller);
			}
		}
	}

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::setActionName(string $action, bool $format_name = true)
*/
PHP_METHOD(yaf_request, setActionName) {
	zend_string *action;
	zend_bool format_name = 1;
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|b", &action, &format_name) == FAILURE) {
		return;
	}

	if (EXPECTED(ZSTR_LEN(action))) {
		if (format_name) {
			yaf_request_set_action(request, action);
		} else {
			if (request->action) {
				zend_string_release(request->action);
				request->action = zend_string_copy(action);
			}
		}
	}

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::setParam(mixed $value)
*/
PHP_METHOD(yaf_request, setParam) {
	unsigned argc;

	argc = ZEND_NUM_ARGS();

	if (1 == argc) {
		zval *value ;
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &value) == FAILURE) {
			return;
		}
		if (yaf_request_set_params_multi(Z_YAFREQUESTOBJ_P(getThis()), value)) {
			RETURN_ZVAL(getThis(), 1, 0);
		}
	} else if (2 == argc) {
		zval *value;
		zend_string *name;
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
			return;
		}

		if (yaf_request_set_params_single(Z_YAFREQUESTOBJ_P(getThis()), name, value)) {
			RETURN_ZVAL(getThis(), 1, 0);
		}
	} else {
		WRONG_PARAM_COUNT;
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getParam(string $name, $mixed $default = NULL)
*/
PHP_METHOD(yaf_request, getParam) {
	zend_string *name;
	zval *def = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &name, &def) == FAILURE) {
		return;
	} else {
		zval *value = yaf_request_get_param(Z_YAFREQUESTOBJ_P(getThis()), name);
		if (value) {
			RETURN_ZVAL(value, 1, 0);
		}
		if (def) {
			RETURN_ZVAL(def, 1, 0);
		}
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getException(void)
*/
PHP_METHOD(yaf_request, getException) {
	zval *exception;
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if ((exception = yaf_request_get_param_str(request, ZEND_STRL("exception")))) {
		RETURN_ZVAL(exception, 1, 0);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getParams(void)
*/
PHP_METHOD(yaf_request, getParams) {
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (request->params) {
		GC_ADDREF(request->params);
		RETURN_ARR(request->params);
	}
#if PHP_VERSION_ID < 70400
	array_init(return_value);
#else
	RETURN_EMPTY_ARRAY();
#endif
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::clearParams(void)
*/
PHP_METHOD(yaf_request, clearParams) {
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	yaf_request_clean_params(request);
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getLanguage(void)
*/
PHP_METHOD(yaf_request, getLanguage) {
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(getThis());
	zend_string *language = yaf_request_get_language(request);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (UNEXPECTED(language == NULL)) {
		RETURN_NULL();
	}

	RETURN_STR(language);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getMethod(void)
*/
PHP_METHOD(yaf_request, getMethod) {
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_STR(yaf_request_get_method(Z_YAFREQUESTOBJ_P(getThis())));
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isDispatched(void)
*/
PHP_METHOD(yaf_request, isDispatched) {
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_BOOL(yaf_request_is_dispatched(Z_YAFREQUESTOBJ_P(getThis())));
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::setDispatched(bool $dispatched = true)
*/
PHP_METHOD(yaf_request, setDispatched) {
	zend_bool state = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &state) == FAILURE){
		return;
	}

	yaf_request_set_dispatched(Z_YAFREQUESTOBJ_P(getThis()), state);

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::setBaseUri(string $name)
*/
PHP_METHOD(yaf_request, setBaseUri) {
	zend_string *uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &uri) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(uri) == 0) {
		RETURN_FALSE;
	}

	if (yaf_request_set_base_uri(Z_YAFREQUESTOBJ_P(getThis()), uri, NULL)) {
		RETURN_ZVAL(getThis(), 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getBaseUri(void)
*/
PHP_METHOD(yaf_request, getBaseUri) {
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(getThis());
	zend_string *base_uri = yaf_request_get_base_uri(request);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (UNEXPECTED(base_uri == NULL)) {
		RETURN_EMPTY_STRING();
	}

	RETURN_STR(base_uri);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getRequestUri(void)
*/
PHP_METHOD(yaf_request, getRequestUri) {
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(getThis());
	zend_string *uri = yaf_request_get_uri(request);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (UNEXPECTED(uri == NULL)) {
		RETURN_EMPTY_STRING();
	}

	RETURN_STR(uri);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::setRequestUri(string $name)
*/
PHP_METHOD(yaf_request, setRequestUri) {
	zend_string *uri;
	yaf_request_object *req = Z_YAFREQUESTOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &uri) == FAILURE) {
		return;
	}

	yaf_request_set_uri(req, uri);

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isRouted(void)
*/
PHP_METHOD(yaf_request, isRouted) {
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_BOOL(yaf_request_is_routed(Z_YAFREQUESTOBJ_P(getThis())));
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::setRouted(bool $routed = true)
*/
PHP_METHOD(yaf_request, setRouted) {
	zend_bool state = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &state) == FAILURE){
		return;
	}

	yaf_request_set_routed(Z_YAFREQUESTOBJ_P(getThis()), state);
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ yaf_request_methods
*/
zend_function_entry yaf_request_methods[] = {
	PHP_ME(yaf_request, isGet, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isPost, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isDelete, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isPatch, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isPut, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isHead, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isOptions, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isCli, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isXmlHttpRequest, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getQuery, yaf_request_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getRequest, yaf_request_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getPost, yaf_request_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getCookie, yaf_request_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getRaw, yaf_request_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getFiles, yaf_request_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, get, yaf_request_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getServer, yaf_request_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getEnv, yaf_request_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, setParam, yaf_request_set_param_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getParam, yaf_request_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getParams, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, clearParams, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getException, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getModuleName, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getControllerName, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getActionName, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, setModuleName, yaf_request_set_module_name_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, setControllerName, yaf_request_set_controller_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, setActionName, yaf_request_set_action_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getMethod, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getLanguage, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, setBaseUri, yaf_request_set_baseuri_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getBaseUri, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getRequestUri, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, setRequestUri, yaf_request_set_request_uri_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isDispatched, yaf_request_void_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	PHP_ME(yaf_request, setDispatched, yaf_request_set_dispatched_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	PHP_ME(yaf_request, isRouted, yaf_request_void_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	PHP_ME(yaf_request, setRouted, yaf_request_set_routed_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(request){
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Request_Abstract", "Yaf\\Request_Abstract", yaf_request_methods);
	yaf_request_ce	= zend_register_internal_class_ex(&ce, NULL);
	yaf_request_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	yaf_request_ce->create_object = yaf_request_new;
	yaf_request_ce->serialize = zend_class_serialize_deny;
	yaf_request_ce->unserialize = zend_class_unserialize_deny;

	memcpy(&yaf_request_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_request_obj_handlers.offset = XtOffsetOf(yaf_request_object, std);
	yaf_request_obj_handlers.free_obj = yaf_request_object_free;
	yaf_request_obj_handlers.get_properties = yaf_request_get_properties;;
	yaf_request_obj_handlers.read_property = yaf_request_read_property;
	yaf_request_obj_handlers.write_property = yaf_request_write_property;
	yaf_request_obj_handlers.get_gc = yaf_request_get_gc;
	yaf_request_obj_handlers.clone_obj = NULL;

	YAF_STARTUP(request_http);
	YAF_STARTUP(request_simple);

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
