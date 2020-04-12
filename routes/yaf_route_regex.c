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
#include "ext/pcre/php_pcre.h"
#include "Zend/zend_smart_str.h" /* for smart_str */
#include "Zend/zend_interfaces.h" /* for zend_class_serialize_deny */

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_request.h"

#include "yaf_router.h"
#include "routes/yaf_route_interface.h"
#include "routes/yaf_route_regex.h"
#include "ext/standard/php_string.h"

zend_class_entry *yaf_route_regex_ce;
static zend_object_handlers yaf_route_regex_obj_handlers;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_route_regex_construct_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, match)
	ZEND_ARG_ARRAY_INFO(0, route, 0)
	ZEND_ARG_ARRAY_INFO(0, map, 1)
	ZEND_ARG_ARRAY_INFO(0, verify, 1)
	ZEND_ARG_INFO(0, reverse)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_route_regex_match_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()
/* }}} */

static HashTable *yaf_route_regex_get_properties(zval *object) /* {{{ */ {
	zval rv;
	HashTable *ht;
	yaf_route_regex_object *regex = Z_YAFROUTEREGEXOBJ_P(object);

	if (!regex->properties) {
		ALLOC_HASHTABLE(regex->properties);
		zend_hash_init(regex->properties, 8, NULL, ZVAL_PTR_DTOR, 0);

		ht = regex->properties;
		ZVAL_STR_COPY(&rv, regex->match);
		zend_hash_str_add(ht, "match:protected", sizeof("match:protected") - 1, &rv);

		ZVAL_ARR(&rv, regex->router);
		GC_ADDREF(regex->router);
		zend_hash_str_add(ht, "route:protected", sizeof("route:protected") - 1, &rv);

		if (regex->map) {
			ZVAL_ARR(&rv, regex->map);
			GC_ADDREF(regex->map);
		} else {
			ZVAL_NULL(&rv);
		}
		zend_hash_str_add(ht, "map:protected", sizeof("map:protected") - 1, &rv);

		if (regex->verify) {
			ZVAL_ARR(&rv, regex->verify);
			GC_ADDREF(regex->verify);
		} else {
			ZVAL_NULL(&rv);
		}
		zend_hash_str_add(ht, "verify:protected", sizeof("verify:protected") - 1, &rv);

		if (regex->reverse) {
			ZVAL_STR_COPY(&rv, regex->reverse);
		} else {
			ZVAL_NULL(&rv);
		}
		zend_hash_str_add(ht, "reverse:protected", sizeof("reverse:protected") - 1, &rv);
	}

	return regex->properties;
}
/* }}} */

static zend_object *yaf_route_regex_new(zend_class_entry *ce) /* {{{ */ {
	yaf_route_regex_object *regex = emalloc(sizeof(yaf_route_regex_object));

	zend_object_std_init(&regex->std, ce);

	regex->std.handlers = &yaf_route_regex_obj_handlers;

	regex->match = NULL;
	regex->router = NULL;
	regex->router = NULL;
	regex->verify = NULL;
	regex->properties = NULL;

	return &regex->std;
}
/* }}} */

static void yaf_route_regex_object_free(zend_object *object) /* {{{ */ {
	yaf_route_regex_object *regex = (yaf_route_regex_object*)object;

	if (regex->match) {
		zend_string_release(regex->match);
	}

	if (regex->reverse) {
		zend_string_release(regex->reverse);
	}

	if (regex->router) {
		if (!(GC_FLAGS(regex->router) & IS_ARRAY_IMMUTABLE) && GC_DELREF(regex->router) == 0) {
			GC_REMOVE_FROM_BUFFER(regex->router);
			zend_array_destroy(regex->router);
		}
	}

	if (regex->map) {
		if (!(GC_FLAGS(regex->map) & IS_ARRAY_IMMUTABLE) && GC_DELREF(regex->map) == 0) {
			GC_REMOVE_FROM_BUFFER(regex->map);
			zend_array_destroy(regex->map);
		}
	}

	if (regex->verify) {
		if (!(GC_FLAGS(regex->verify) & IS_ARRAY_IMMUTABLE) && GC_DELREF(regex->verify) == 0) {
			GC_REMOVE_FROM_BUFFER(regex->verify);
			zend_array_destroy(regex->verify);
		}
	}

	if (regex->properties) {
		if (GC_DELREF(regex->properties) == 0) {
			GC_REMOVE_FROM_BUFFER(regex->properties);
			zend_array_destroy(regex->properties);
		}
	}

	zend_object_std_dtor(&regex->std);
}
/* }}} */

static void yaf_route_regex_init(yaf_route_regex_object *regex, zend_string *match, zval *router, zval *map, zval *verify, zend_string *reverse) /* {{{ */ {
	regex->match = zend_string_copy(match);

	if (router) {
		regex->router = zend_array_dup(Z_ARRVAL_P(router));
	} else {
		regex->router = NULL;
	}

	if (map) {
		regex->map = zend_array_dup(Z_ARRVAL_P(map));
	} else {
		regex->map = NULL;
	}

	if (verify) {
		regex->verify = zend_array_dup(Z_ARRVAL_P(verify));
	} else {
		regex->verify = NULL;
	}

	if (reverse) {
		regex->reverse = zend_string_copy(reverse);
	} else {
		regex->reverse = NULL;
	}
}
/* }}} */

void yaf_route_regex_instance(yaf_route_t *route, zend_string *match, zval *router, zval *map, zval *verify, zend_string *reverse) /* {{{ */ {
	zend_object *regex = yaf_route_regex_new(yaf_route_regex_ce);

	yaf_route_regex_init((yaf_route_regex_object*)regex, match, router, map, verify, reverse);

	ZVAL_OBJ(route, regex);
}
/* }}} */

static int yaf_route_regex_match(yaf_route_regex_object *regex, const char *uri, size_t len, zval *ret) /* {{{ */ {
	pcre_cache_entry *pce_regexp;

	if (UNEXPECTED(len == 0)) {
		return 0;
	}

	ZEND_ASSERT(regex->match);

	if ((pce_regexp = pcre_get_compiled_regex_cache(regex->match)) == NULL) {
		return 0;
	} else {
		zval matches, subparts;

		ZVAL_NULL(&subparts);
#if PHP_VERSION_ID < 70400
		php_pcre_match_impl(pce_regexp, (char*)uri, len, &matches, &subparts /* subpats */,
				0/* global */, 0/* ZEND_NUM_ARGS() >= 4 */, 0/*flags PREG_OFFSET_CAPTURE*/, 0/* start_offset */);
#else
		{
			zend_string *tmp = zend_string_init(uri, len, 0);
			php_pcre_match_impl(pce_regexp, tmp, &matches, &subparts /* subpats */,
					0/* global */, 0/* ZEND_NUM_ARGS() >= 4 */, 0/*flags PREG_OFFSET_CAPTURE*/, 0/* start_offset */);
			zend_string_release(tmp);
		}
#endif

		if (!zend_hash_num_elements(Z_ARRVAL(subparts))) {
			zval_ptr_dtor(&subparts);
			return 0;
		} else {
			zval *name, *pzval;
			zend_string	*key = NULL;
			zend_ulong idx = 0;
			HashTable *ht;

			array_init(ret);

			ht = Z_ARRVAL(subparts);
			ZEND_HASH_FOREACH_KEY_VAL(ht, idx, key, pzval) {
				if (key) {
					Z_TRY_ADDREF_P(pzval);
					zend_hash_update(Z_ARRVAL_P(ret), key, pzval);
				} else {
					if (regex->map && (name = zend_hash_index_find(regex->map, idx)) && Z_TYPE_P(name) == IS_STRING) {
						Z_TRY_ADDREF_P(pzval);
						zend_hash_update(Z_ARRVAL_P(ret), Z_STR_P(name), pzval);
					}
				}
			} ZEND_HASH_FOREACH_END();

			zval_ptr_dtor(&subparts);
			return 1;
		}
	}
}
/* }}} */

int yaf_route_regex_route(yaf_route_t *route, yaf_request_t *req) /* {{{ */ {
	zval args;
	const char *req_uri;
	size_t req_uri_len;
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(req);
	yaf_route_regex_object *regex = Z_YAFROUTEREGEXOBJ_P(route);

	if (request->base_uri) {
		req_uri = yaf_request_strip_base_uri(request->uri, request->base_uri, &req_uri_len);
	} else {
		req_uri = ZSTR_VAL(request->uri);
		req_uri_len = ZSTR_LEN(request->uri);
	}

	if (!yaf_route_regex_match(regex, req_uri, req_uri_len, &args)) {
		return 0;
	} else {
		zval *module, *controller, *action;

		ZEND_ASSERT(regex->router);
		if ((module = zend_hash_str_find(regex->router, ZEND_STRL("module"))) && IS_STRING == Z_TYPE_P(module)) {
			if (Z_STRVAL_P(module)[0] != ':') {
				yaf_request_set_module(request, Z_STR_P(module));
			} else {
				zval *m;
				if ((m = zend_hash_str_find(Z_ARRVAL(args), Z_STRVAL_P(module) + 1, Z_STRLEN_P(module) - 1)) && IS_STRING == Z_TYPE_P(m)) {
					yaf_request_set_module(request, Z_STR_P(m));
				}
			}
		}

		if ((controller = zend_hash_str_find(regex->router, ZEND_STRL("controller"))) && IS_STRING == Z_TYPE_P(controller)) {
			if (Z_STRVAL_P(controller)[0] != ':') {
				yaf_request_set_controller(request, Z_STR_P(controller));
			} else {
				zval *c;
				if ((c = zend_hash_str_find(Z_ARRVAL(args), Z_STRVAL_P(controller) + 1, Z_STRLEN_P(controller) - 1)) && IS_STRING == Z_TYPE_P(c)) {
					yaf_request_set_controller(request, Z_STR_P(c));
				}
			}
		}

		if ((action = zend_hash_str_find(regex->router, ZEND_STRL("action"))) && IS_STRING == Z_TYPE_P(action)) {
			if (Z_STRVAL_P(action)[0] != ':') {
				yaf_request_set_action(request, Z_STR_P(action));
			} else {
				zval *a;
				if ((a = zend_hash_str_find(Z_ARRVAL(args), Z_STRVAL_P(action) + 1, Z_STRLEN_P(action) - 1)) && IS_STRING == Z_TYPE_P(a)) {
					yaf_request_set_action(request, Z_STR_P(a));
				}
			}
		}

		yaf_request_set_params_multi(request, &args);
		zval_ptr_dtor(&args);
	}

	return 1;
}
/* }}} */

zend_string * yaf_route_regex_assemble(yaf_route_regex_object *regex, zval *info, zval *query) /* {{{ */ {
	zval *zv;
	zend_string *val;
	zend_string *uri;
	zend_string *inter;
	smart_str query_str = {0};

	if (UNEXPECTED(regex->reverse == NULL)) {
		return NULL;
	}

	uri = zend_string_copy(regex->reverse);
	if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_MOUDLE_FORMAT))) != NULL) {
		val = zval_get_string(zv);
		inter = php_str_to_str(ZSTR_VAL(regex->reverse), ZSTR_LEN(regex->reverse),
				ZEND_STRL(YAF_ROUTE_ASSEMBLE_MOUDLE_FORMAT), ZSTR_VAL(val), ZSTR_LEN(val));
		zend_string_release(val);
		zend_string_release(uri);
		uri = inter;
	}

	if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_CONTROLLER_FORMAT))) != NULL) {
		val = zval_get_string(zv);
		inter = php_str_to_str(ZSTR_VAL(uri), ZSTR_LEN(uri),
				ZEND_STRL(YAF_ROUTE_ASSEMBLE_CONTROLLER_FORMAT), ZSTR_VAL(val), ZSTR_LEN(val));
		zend_string_release(val);
		zend_string_release(uri);
		uri = inter;
	}

	if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_ACTION_FORMAT))) != NULL) {
		val = zval_get_string(zv);
		inter = php_str_to_str(ZSTR_VAL(uri), ZSTR_LEN(uri),
				ZEND_STRL(YAF_ROUTE_ASSEMBLE_ACTION_FORMAT), Z_STRVAL_P(zv), Z_STRLEN_P(zv));
		zend_string_release(val);
		zend_string_release(uri);
		uri = inter;
	}

	if (query && IS_ARRAY == Z_TYPE_P(query)) {
		zend_string *key;
		HashTable *ht = Z_ARRVAL_P(query);

		smart_str_appendc(&query_str, '?');
		ZEND_HASH_FOREACH_STR_KEY_VAL(ht, key, zv) {
			if (key) {
				val = zval_get_string(zv);
				smart_str_appendl(&query_str, ZSTR_VAL(key), ZSTR_LEN(key));
				smart_str_appendc(&query_str, '=');
				smart_str_appendl(&query_str, Z_STRVAL_P(zv), Z_STRLEN_P(zv));
				smart_str_appendc(&query_str, '&');
				zend_string_release(val);
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (query_str.s) {
		size_t orig_len = ZSTR_LEN(uri);
		ZSTR_LEN(query_str.s)--; /* get rid of the tail & */
		smart_str_0(&query_str);
		uri = zend_string_realloc(uri, ZSTR_LEN(uri) + ZSTR_LEN(query_str.s), 0);
		memcpy(ZSTR_VAL(uri) + orig_len, ZSTR_VAL(query_str.s), ZSTR_LEN(query_str.s));
		ZSTR_VAL(uri)[ZSTR_LEN(uri)] = '\0';
		smart_str_free(&query_str);
	}

	return uri;
}
/** }}} */

/** {{{ proto public Yaf_Route_Regex::route(string $uri)
 */
PHP_METHOD(yaf_route_regex, route) {
	yaf_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &request, yaf_request_ce) == FAILURE) {
		return;
	}

	RETURN_BOOL(yaf_route_regex_route(getThis(), request));
}
/** }}} */

/** {{{ proto public Yaf_Route_Regex::match(string $uri)
 */
PHP_METHOD(yaf_route_regex, match) {
	zend_string *uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &uri) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(uri) == 0) {
		RETURN_FALSE;
	}

	if (!yaf_route_regex_match(Z_YAFROUTEREGEXOBJ_P(getThis()), ZSTR_VAL(uri), ZSTR_LEN(uri), return_value)) {
		RETURN_FALSE;
	}
}
/** }}} */

/** {{{ proto public Yaf_Route_Regex::__construct(string $match, array $route, array $map = NULL, array $verify = NULL, string reverse = NULL)
 */
PHP_METHOD(yaf_route_regex, __construct) {
	zend_string *match;
	zend_string *reverse = NULL;
	zval *route, *map = NULL, *verify = NULL;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "Sa|a!a!S!", &match, &route, &map, &verify, &reverse) ==  FAILURE) {
		return;
	}

	yaf_route_regex_init(Z_YAFROUTEREGEXOBJ_P(getThis()), match, route, map, verify, reverse);
}
/** }}} */

/** {{{ proto public Yaf_Route_regex::assemble(array $info[, array $query = NULL])
 */
PHP_METHOD(yaf_route_regex, assemble) {
	zval *info, *query = NULL;
	zend_string *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|a", &info, &query) == FAILURE) {
		return;
	}

	if ((str = yaf_route_regex_assemble(Z_YAFROUTEREGEXOBJ_P(getThis()), info, query)) != NULL) {
		RETURN_STR(str);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ yaf_route_regex_methods
 */
zend_function_entry yaf_route_regex_methods[] = {
	PHP_ME(yaf_route_regex, __construct, yaf_route_regex_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(yaf_route_regex, match, yaf_route_regex_match_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_route_regex, route, yaf_route_route_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_route_regex, assemble, yaf_route_assemble_arginfo, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
 */
YAF_STARTUP_FUNCTION(route_regex) {
	zend_class_entry ce;
	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Route_Regex", "Yaf\\Route\\Regex", yaf_route_regex_methods);
	yaf_route_regex_ce = zend_register_internal_class(&ce);
	yaf_route_regex_ce->create_object = yaf_route_regex_new;
	yaf_route_regex_ce->ce_flags |= ZEND_ACC_FINAL;
	yaf_route_regex_ce->serialize = zend_class_serialize_deny;
	yaf_route_regex_ce->unserialize = zend_class_unserialize_deny;

	zend_class_implements(yaf_route_regex_ce, 1, yaf_route_ce);

	memcpy(&yaf_route_regex_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_route_regex_obj_handlers.free_obj = yaf_route_regex_object_free;
	yaf_route_regex_obj_handlers.get_gc = NULL;
	yaf_route_regex_obj_handlers.clone_obj = NULL;
	yaf_route_regex_obj_handlers.get_properties = yaf_route_regex_get_properties;

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

