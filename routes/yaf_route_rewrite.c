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
#include "ext/pcre/php_pcre.h" /* for pcre */
#include "Zend/zend_smart_str.h" /* for smart_str */
#include "Zend/zend_interfaces.h" /* for zend_class_serialize_deny */

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_request.h"

#include "yaf_router.h"
#include "routes/yaf_route_interface.h"
#include "routes/yaf_route_rewrite.h"

#include "ext/standard/php_string.h"

zend_class_entry *yaf_route_rewrite_ce;
static zend_object_handlers yaf_route_rewrite_obj_handlers;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_route_rewrite_construct_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, match)
	ZEND_ARG_ARRAY_INFO(0, route, 0)
	ZEND_ARG_ARRAY_INFO(0, verify, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_route_rewrite_match_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()
/* }}} */

static HashTable *yaf_route_rewrite_get_properties(zval *object) /* {{{ */ {
	zval rv;
	HashTable *ht;
	yaf_route_rewrite_object *rewrite = Z_YAFROUTEREWRITEOBJ_P(object);

	if (!rewrite->properties) {
		ALLOC_HASHTABLE(rewrite->properties);
		zend_hash_init(rewrite->properties, 4, NULL, ZVAL_PTR_DTOR, 0);

		ht = rewrite->properties;
		ZVAL_STR_COPY(&rv, rewrite->match);
		zend_hash_str_add(ht, "match:protected", sizeof("match:protected") - 1, &rv);

		ZVAL_ARR(&rv, rewrite->router);
		Z_TRY_ADDREF(rv);
		zend_hash_str_add(ht, "route:protected", sizeof("route:protected") - 1, &rv);

		if (rewrite->verify) {
			ZVAL_ARR(&rv, rewrite->verify);
			Z_TRY_ADDREF(rv);
		} else {
			ZVAL_NULL(&rv);
		}
		zend_hash_str_add(ht, "verify:protected", sizeof("verify:protected") - 1, &rv);
	}

	return rewrite->properties;
}
/* }}} */

static zend_object *yaf_route_rewrite_new(zend_class_entry *ce) /* {{{ */ {
	yaf_route_rewrite_object *rewrite = emalloc(sizeof(yaf_route_rewrite_object));

	zend_object_std_init(&rewrite->std, ce);
	rewrite->std.handlers = &yaf_route_rewrite_obj_handlers;

	rewrite->match = NULL;
	rewrite->router = NULL;
	rewrite->verify = NULL;
	rewrite->properties = NULL;

	return &rewrite->std;
}
/* }}} */

static void yaf_route_rewrite_object_free(zend_object *object) /* {{{ */ {
	yaf_route_rewrite_object *rewrite = (yaf_route_rewrite_object*)object;

	if (rewrite->match) {
		zend_string_release(rewrite->match);
	}

	if (rewrite->router) {
		if ((GC_DELREF(rewrite->router) == 0)) {
			GC_REMOVE_FROM_BUFFER(rewrite->router);
			zend_array_destroy(rewrite->router);
		}
	}

	if (rewrite->verify) {
		if ((GC_DELREF(rewrite->verify) == 0)) {
			GC_REMOVE_FROM_BUFFER(rewrite->verify);
			zend_array_destroy(rewrite->verify);
		}
	}

	if (rewrite->properties) {
		if ((GC_DELREF(rewrite->properties) == 0)) {
			GC_REMOVE_FROM_BUFFER(rewrite->properties);
			zend_array_destroy(rewrite->properties);
		}
	}

	zend_object_std_dtor(&rewrite->std);
}
/* }}} */

static void yaf_route_rewrite_init(yaf_route_rewrite_object *rewrite, zend_string *match, zval *router, zval *verify) /* {{{ */ {
	rewrite->match = zend_string_copy(match);

	if (router) {
		rewrite->router = zend_array_dup(Z_ARRVAL_P(router));
	} else {
		rewrite->router = NULL;
	}

	if (verify) {
		rewrite->verify = zend_array_dup(Z_ARRVAL_P(verify));
	} else {
		rewrite->verify = NULL;
	}
}
/* }}} */

void yaf_route_rewrite_instance(yaf_route_t *route, zend_string *match, zval *router, zval *verify) /* {{{ */ {
	zend_object *rewrite = yaf_route_rewrite_new(yaf_route_rewrite_ce);

	yaf_route_rewrite_init((yaf_route_rewrite_object*)rewrite, match, router, verify);
	
	ZVAL_OBJ(route, rewrite);
}
/* }}} */

static int yaf_route_rewrite_match(yaf_route_rewrite_object *rewrite, const char *uri, size_t len, zval *ret) /* {{{ */ {
	char *pos, *m;
	uint32_t l;
	pcre_cache_entry *pce_regexp;
	smart_str pattern = {0};


	ZEND_ASSERT(rewrite->match);

	smart_str_appendc(&pattern, YAF_ROUTE_REGEX_DILIMITER);
	smart_str_appendc(&pattern, '^');

	m = ZSTR_VAL(rewrite->match);
	l = ZSTR_LEN(rewrite->match);
	while (l) {
		if (*m == '*') {
			smart_str_appendl(&pattern, "(?P<__yaf_route_rest>.*)", sizeof("(?P<__yaf_route_rest>.*)") -1);
			break;
		} else {
			uint32_t len;
			pos = memchr(m, YAF_ROUTER_URL_DELIMIETER, l);
			if (pos) {
				len = pos - m;
				l -= len;
			} else {
				len = l;
				l = 0;
			}
			if (*m == ':') {
				smart_str_appendl(&pattern, "(?P<", sizeof("(?P<") -1 );
				smart_str_appendl(&pattern, m + 1, len - 1);
				smart_str_appendl(&pattern, ">[^", sizeof(">[^") - 1);
				smart_str_appendc(&pattern, YAF_ROUTER_URL_DELIMIETER);
				smart_str_appendl(&pattern, "]+)", sizeof("]+)") - 1);
			} else {
				smart_str_appendl(&pattern, m, len);
			}
			if (pos) {
				smart_str_appendc(&pattern, YAF_ROUTER_URL_DELIMIETER);
				smart_str_appendc(&pattern, '+');
				m = ++pos;
				l--;
			}
		}
	}

	smart_str_appendc(&pattern, YAF_ROUTE_REGEX_DILIMITER);
	smart_str_appendc(&pattern, 'i');
	smart_str_0(&pattern);
	pce_regexp = pcre_get_compiled_regex_cache(pattern.s);
	smart_str_free(&pattern);

	if (pce_regexp) {
		zval matches, subparts;

		smart_str_free(&pattern);

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
			zval *pzval;
			zend_string *key;
			HashTable *ht;

			array_init(ret);

			ht = Z_ARRVAL(subparts);
			ZEND_HASH_FOREACH_STR_KEY_VAL(ht, key, pzval) {
				if (key) {
					if (zend_string_equals_literal(key, "__yaf_route_rest")) {
						zval params;
						yaf_router_parse_parameters(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval), &params);
						zend_hash_copy(Z_ARRVAL_P(ret), Z_ARRVAL(params), (copy_ctor_func_t) zval_add_ref);
						zval_ptr_dtor(&params);
					} else {
						Z_ADDREF_P(pzval);
						zend_hash_update(Z_ARRVAL_P(ret), key, pzval);
					}
				}
			} ZEND_HASH_FOREACH_END();

			zval_ptr_dtor(&subparts);
			return 1;
		}
	}

	return 0;
}
/* }}} */

int yaf_route_rewrite_route(yaf_route_t *route, yaf_request_t *req) /* {{{ */ {
	zval args;
	const char *req_uri;
	size_t req_uri_len;
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(req);
	yaf_route_rewrite_object *rewrite = Z_YAFROUTEREWRITEOBJ_P(route);

	if (request->base_uri) {
		req_uri = yaf_request_strip_base_uri(request->uri, request->base_uri, &req_uri_len);
	} else {
		req_uri = ZSTR_VAL(request->uri);
		req_uri_len = ZSTR_LEN(request->uri);
	}

	if (UNEXPECTED(req_uri_len == 0)) {
		return 0;
	}

	if (EXPECTED(yaf_route_rewrite_match(rewrite, req_uri, req_uri_len, &args))) {
		zval *module, *controller, *action;

		ZEND_ASSERT(rewrite->router);
		if ((module = zend_hash_str_find(rewrite->router, ZEND_STRL("module"))) != NULL && IS_STRING == Z_TYPE_P(module)) {
			if (Z_STRVAL_P(module)[0] != ':') {
				yaf_request_set_module(request, Z_STR_P(module));
			} else {
				zval *m;
				if ((m = zend_hash_str_find(Z_ARRVAL(args), Z_STRVAL_P(module) + 1, Z_STRLEN_P(module) - 1)) && IS_STRING == Z_TYPE_P(m)) {
					yaf_request_set_module(request, Z_STR_P(m));
				}
			}
		}

		if ((controller = zend_hash_str_find(rewrite->router, ZEND_STRL("controller"))) && IS_STRING == Z_TYPE_P(controller)) {
			if (Z_STRVAL_P(controller)[0] != ':') {
				yaf_request_set_controller(request, Z_STR_P(controller));
			} else {
				zval *c;
				if ((c = zend_hash_str_find(Z_ARRVAL(args), Z_STRVAL_P(controller) + 1, Z_STRLEN_P(controller) - 1)) && IS_STRING == Z_TYPE_P(c)) {
					yaf_request_set_controller(request, Z_STR_P(c));
				}
			}
		}

		if ((action = zend_hash_str_find(rewrite->router, ZEND_STRL("action"))) && IS_STRING == Z_TYPE_P(action)) {
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

		return 1;
	}

	return 0;
}
/* }}} */

zend_string *yaf_route_rewrite_assemble(yaf_route_rewrite_object *rewrite, zval *info, zval *query) /* {{{ */ {
	zval pidents, *zv;
	char *seg, *pmatch, *ptrptr;
	zend_string *key, *inter, *uri, *val;
	size_t seg_len;
	smart_str query_str = {0};
	smart_str wildcard = {0};
	char token[2] = {YAF_ROUTER_URL_DELIMIETER, 0};

	array_init(&pidents);

	uri = zend_string_copy(rewrite->match);
	pmatch = estrndup(ZSTR_VAL(rewrite->match), ZSTR_LEN(rewrite->match));
	zend_hash_copy(Z_ARRVAL(pidents), Z_ARRVAL_P(info), (copy_ctor_func_t) zval_add_ref);

	seg = php_strtok_r(pmatch, token, &ptrptr);
	while (seg) {
		seg_len = strlen(seg);
		if (seg_len) {
			if (*(seg) == '*') {
				ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL(pidents), key, zv) {
					if (key) {
						if (IS_STRING == Z_TYPE_P(zv)) {
							smart_str_appendl(&wildcard, ZSTR_VAL(key) + 1, ZSTR_LEN(key) - 1);
							smart_str_appendc(&wildcard, YAF_ROUTER_URL_DELIMIETER);
							smart_str_appendl(&wildcard, Z_STRVAL_P(zv), Z_STRLEN_P(zv));
							smart_str_appendc(&wildcard, YAF_ROUTER_URL_DELIMIETER);
						}
					}
				} ZEND_HASH_FOREACH_END();
				smart_str_0(&wildcard);
				inter = php_str_to_str(ZSTR_VAL(uri), ZSTR_LEN(uri),
						"*", 1, ZSTR_VAL(wildcard.s), ZSTR_LEN(wildcard.s));
				zend_string_release(uri);
				uri = inter;
				break;
			}

			if (*(seg) == ':') {
				if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), seg, seg_len)) != NULL) {
					val = zval_get_string(zv);
					inter = php_str_to_str(ZSTR_VAL(uri),
							ZSTR_LEN(uri), seg, seg_len, ZSTR_VAL(val), ZSTR_LEN(val));
					zend_string_release(val);
					zend_string_release(uri);
					zend_hash_str_del(Z_ARRVAL(pidents), seg, seg_len);
					uri = inter;
				}
			}
		}
		seg = php_strtok_r(NULL, token, &ptrptr);
	}

	smart_str_free(&wildcard);
	efree(pmatch);
	zval_ptr_dtor(&pidents);

	if (query && IS_ARRAY == Z_TYPE_P(query)) {
		HashTable *ht = Z_ARRVAL_P(query);

		smart_str_appendc(&query_str, '?');
		ZEND_HASH_FOREACH_STR_KEY_VAL(ht, key, zv) {
			if (key) {
				val = zval_get_string(zv);
				smart_str_appendl(&query_str, ZSTR_VAL(key), ZSTR_LEN(key));
				smart_str_appendc(&query_str, '=');
				smart_str_appendl(&query_str, ZSTR_VAL(val), ZSTR_LEN(val));
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
/* }}} */

/** {{{ proto public Yaf_Route_Rewrite::route(Yaf_Request_Abstract $request)
 */
PHP_METHOD(yaf_route_rewrite, route) {
	yaf_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &request, yaf_request_ce) == FAILURE) {
		return;
	}

	RETURN_BOOL(yaf_route_rewrite_route(getThis(), request));
}
/** }}} */

/** {{{ proto public Yaf_Route_Rewrite::match(string $uri)
 */
PHP_METHOD(yaf_route_rewrite, match) {
	zend_string *uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &uri) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(uri) == 0) {
		RETURN_FALSE;
	}

	if (!yaf_route_rewrite_match(Z_YAFROUTEREWRITEOBJ_P(getThis()), ZSTR_VAL(uri), ZSTR_LEN(uri), return_value)) {
		RETURN_FALSE;
	}
}
/** }}} */

/** {{{ proto public Yaf_Route_Rewrite::__construct(string $match, array $route, array $verify, string $reverse = NULL)
 */
PHP_METHOD(yaf_route_rewrite, __construct) {
	zend_string *match;
	zval *route, *verify = NULL;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "Sa|a", &match, &route, &verify) ==  FAILURE) {
		return;
	}

	yaf_route_rewrite_init(Z_YAFROUTEREWRITEOBJ_P(getThis()), match, route, verify);
}
/** }}} */

/** {{{ proto public Yaf_Route_rewrite::assemble(array $info[, array $query = NULL])
*/
PHP_METHOD(yaf_route_rewrite, assemble) {
	zend_string *str;
	zval *info, *query = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|a", &info, &query) == FAILURE) {
		return;
	}

	if ((str = yaf_route_rewrite_assemble(Z_YAFROUTEREWRITEOBJ_P(getThis()), info, query))) {
		RETURN_STR(str);
	}
}
/* }}} */

/** {{{ yaf_route_rewrite_methods
 */
zend_function_entry yaf_route_rewrite_methods[] = {
	PHP_ME(yaf_route_rewrite, __construct, yaf_route_rewrite_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(yaf_route_rewrite, match, yaf_route_rewrite_match_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_route_rewrite, route, yaf_route_route_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_route_rewrite, assemble, yaf_route_assemble_arginfo, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
 */
YAF_STARTUP_FUNCTION(route_rewrite) {
	zend_class_entry ce;
	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Route_Rewrite", "Yaf\\Route\\Rewrite", yaf_route_rewrite_methods);
	yaf_route_rewrite_ce = zend_register_internal_class_ex(&ce, NULL);
	yaf_route_rewrite_ce->ce_flags |= ZEND_ACC_FINAL;
	yaf_route_rewrite_ce->create_object = yaf_route_rewrite_new;
	yaf_route_rewrite_ce->serialize = zend_class_serialize_deny;
	yaf_route_rewrite_ce->unserialize = zend_class_unserialize_deny;

	zend_class_implements(yaf_route_rewrite_ce, 1, yaf_route_ce);

	memcpy(&yaf_route_rewrite_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_route_rewrite_obj_handlers.free_obj = yaf_route_rewrite_object_free;
	yaf_route_rewrite_obj_handlers.clone_obj = NULL;
	yaf_route_rewrite_obj_handlers.get_gc = NULL;
	yaf_route_rewrite_obj_handlers.get_properties = yaf_route_rewrite_get_properties;

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

