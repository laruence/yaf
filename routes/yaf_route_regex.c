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

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_request.h"

#include "yaf_router.h"
#include "routes/yaf_route_interface.h"
#include "routes/yaf_route_regex.h"
#include "ext/standard/php_string.h"
#include "zend_smart_str.h" /* for smart_str */

zend_class_entry *yaf_route_regex_ce;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_route_regex_construct_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, match)
	ZEND_ARG_ARRAY_INFO(0, route, 0)
	ZEND_ARG_ARRAY_INFO(0, map, 1)
	ZEND_ARG_ARRAY_INFO(0, verify, 1)
	ZEND_ARG_INFO(0, reverse)
ZEND_END_ARG_INFO()
/* }}} */

yaf_route_t * yaf_route_regex_instance(yaf_route_t *this_ptr, zval *route, zval *def, zval *map, zval *verify, zval *reverse) /* {{{ */ {
	if (Z_ISUNDEF_P(this_ptr)) {
		object_init_ex(this_ptr, yaf_route_regex_ce);
	} 

	zend_update_property(yaf_route_regex_ce, this_ptr, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_MATCH), route);
	zend_update_property(yaf_route_regex_ce, this_ptr, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_ROUTE), def);

	if (map) {
		zend_update_property(yaf_route_regex_ce, this_ptr, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_MAP), map);
	}

	if (!verify) {
		zend_update_property_null(yaf_route_regex_ce, this_ptr, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_VERIFY));
	} else {
		zend_update_property(yaf_route_regex_ce, this_ptr, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_VERIFY), verify);
	}
	

	if (!reverse || IS_STRING != Z_TYPE_P(reverse)) {
		zend_update_property_null(yaf_route_regex_ce, this_ptr, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_REVERSE));
	} else {
		zend_update_property(yaf_route_regex_ce, this_ptr, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_REVERSE), reverse);
	}

	return this_ptr;
}
/* }}} */

/** {{{ static int yaf_route_regex_match(yaf_route_t *router, zend_string *uri, zval *ret)
 */
static int yaf_route_regex_match(yaf_route_t *route, zend_string *uri, zval *ret) {
	zval *match;
	pcre_cache_entry *pce_regexp;

	if (ZSTR_LEN(uri) == 0) {
		return 0;
	}

	match = zend_read_property(yaf_route_regex_ce, route, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_MATCH), 1, NULL);

	if ((pce_regexp = pcre_get_compiled_regex_cache(Z_STR_P(match))) == NULL) {
		return 0;
	} else {
		zval matches, subparts, *map;

		ZVAL_NULL(&subparts);

		map = zend_read_property(yaf_route_regex_ce, route, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_MAP), 1, NULL);

		php_pcre_match_impl(pce_regexp, ZSTR_VAL(uri), ZSTR_LEN(uri), &matches, &subparts /* subpats */,
				0/* global */, 0/* ZEND_NUM_ARGS() >= 4 */, 0/*flags PREG_OFFSET_CAPTURE*/, 0/* start_offset */);

		if (!zend_hash_num_elements(Z_ARRVAL(subparts))) {
			zval_ptr_dtor(&subparts);
			return 0;
		} else {
			zval *name, *pzval;
			zend_string	*key = NULL;
			ulong idx = 0;
			HashTable *ht;

			array_init(ret);

			ht = Z_ARRVAL(subparts);
			ZEND_HASH_FOREACH_KEY_VAL(ht, idx, key, pzval) {
				if (key) {
					Z_TRY_ADDREF_P(pzval);
					zend_hash_update(Z_ARRVAL_P(ret), key, pzval);
				} else {	
					if (Z_TYPE_P(map) == IS_ARRAY &&
						(name = zend_hash_index_find(Z_ARRVAL_P(map), idx)) != NULL && Z_TYPE_P(name) == IS_STRING) {
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

/** {{{ zend_string * yaf_route_regex_assemble(yaf_route_t *this_ptr, zval *info, zval *query)
 */
zend_string * yaf_route_regex_assemble(yaf_route_t *this_ptr, zval *info, zval *query) {
	zval *reverse;
	zval *zv;
	zend_string *uri, *inter, *val;
	smart_str query_str = {0};

	reverse = zend_read_property(yaf_route_regex_ce, this_ptr, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_REVERSE), 1, NULL);

	if (Z_TYPE_P(reverse) != IS_STRING) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "%s", "Reverse property is not a valid string");
		return NULL;
	}

	uri = zend_string_copy(Z_STR_P(reverse));

	if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_MOUDLE_FORMAT))) != NULL) {
		val = zval_get_string(zv);
		inter = php_str_to_str(ZSTR_VAL(uri), ZSTR_LEN(uri),
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

/** {{{ int yaf_route_regex_route(yaf_route_t *router, yaf_request_t *request)
 */
int yaf_route_regex_route(yaf_route_t *router, yaf_request_t *request) {
	zend_string *request_uri;
	zval args, *base_uri, *zuri;

	zuri = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_URI), 1, NULL);
	base_uri = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_BASE), 1, NULL);

	if (base_uri && IS_STRING == Z_TYPE_P(base_uri)
			&& !strncasecmp(Z_STRVAL_P(zuri), Z_STRVAL_P(base_uri), Z_STRLEN_P(base_uri))) {
		request_uri = zend_string_init(Z_STRVAL_P(zuri) + Z_STRLEN_P(base_uri), Z_STRLEN_P(zuri) - Z_STRLEN_P(base_uri), 0);
	} else {
		request_uri = zend_string_copy(Z_STR_P(zuri));
	}

	if (!yaf_route_regex_match(router, request_uri, &args)) {
		zend_string_release(request_uri);
		return 0;
	} else {
		zval *module, *controller, *action, *routes;

		routes = zend_read_property(yaf_route_regex_ce, router, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_ROUTE), 1, NULL);
		if ((module = zend_hash_str_find(Z_ARRVAL_P(routes), ZEND_STRL("module"))) != NULL
				&& IS_STRING == Z_TYPE_P(module)) {
			if (Z_STRVAL_P(module)[0] != ':') {
				zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), module);
			} else {
				zval *m;
				if ((m = zend_hash_str_find(Z_ARRVAL(args), Z_STRVAL_P(module) + 1, Z_STRLEN_P(module) - 1)) != NULL
						&& IS_STRING == Z_TYPE_P(m)) {
					zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), m);
				}
			}
		}

		if ((controller = zend_hash_str_find(Z_ARRVAL_P(routes), ZEND_STRL("controller"))) != NULL
				&& IS_STRING == Z_TYPE_P(controller)) {
			if (Z_STRVAL_P(controller)[0] != ':') {
				zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), controller);
			} else {
				zval *c;
				if ((c = zend_hash_str_find(Z_ARRVAL(args), Z_STRVAL_P(controller) + 1, Z_STRLEN_P(controller) - 1)) != NULL && IS_STRING == Z_TYPE_P(c)) {
					zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), c);
				}
			}
		}

		if ((action = zend_hash_str_find(Z_ARRVAL_P(routes), ZEND_STRL("action"))) != NULL
				&& IS_STRING == Z_TYPE_P(action)) {
			if (Z_STRVAL_P(action)[0] != ':') {
				zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), action);
			} else {
				zval *a;
				if ((a = zend_hash_str_find(Z_ARRVAL(args), Z_STRVAL_P(action) + 1, Z_STRLEN_P(action) - 1)) != NULL && IS_STRING == Z_TYPE_P(a)) {
					zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), a);
				}
			}
		}

		(void)yaf_request_set_params_multi(request, &args);
		zval_ptr_dtor(&args);
		zend_string_release(request_uri);
	}

	return 1;
}
/* }}} */

/** {{{ proto public Yaf_Route_Regex::route(string $uri)
 */
PHP_METHOD(yaf_route_regex, route) {
	yaf_route_t	*route;
	yaf_request_t *request;

	route = getThis();

	RETVAL_FALSE;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &request, yaf_request_ce) == FAILURE) {
		return;
	}

	if (!request || IS_OBJECT != Z_TYPE_P(request)
			|| !instanceof_function(Z_OBJCE_P(request), yaf_request_ce)) {
		php_error_docref(NULL, E_WARNING, "Expects a %s instance", ZSTR_VAL(yaf_request_ce->name));
		RETURN_FALSE;
	}

	RETURN_BOOL(yaf_route_regex_route(route, request));
}
/** }}} */

/** {{{ proto public Yaf_Route_Regex::__construct(string $match, array $route, array $map = NULL, array $verify = NULL, string reverse = NULL)
 */
PHP_METHOD(yaf_route_regex, __construct) {
	zval 		*match, *route, *map = NULL, *verify = NULL, *reverse = NULL;
	yaf_route_t	rself, *self = getThis();

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "za|aaz", &match, &route, &map, &verify, &reverse) ==  FAILURE) {
		return;
	}

	if (IS_STRING != Z_TYPE_P(match) || !Z_STRLEN_P(match)) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Expects a valid string as the first parameter");
		RETURN_FALSE;
	}

	if (verify && IS_ARRAY != Z_TYPE_P(verify)) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Expects an array as third parameter");
		RETURN_FALSE;
	}

	if (reverse && IS_STRING != Z_TYPE_P(reverse)) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Expects a valid string reverse as fourth parameter");
		RETURN_FALSE;
	}

	if (!self) {
		ZVAL_NULL(&rself);
		self = &rself;
	}

	yaf_route_regex_instance(self, match, route, map, verify, reverse);

	if (self) {
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/** }}} */

/** {{{ proto public Yaf_Route_regex::assemble(array $info[, array $query = NULL])
 */
PHP_METHOD(yaf_route_regex, assemble) {
	zval *info, *query = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|a", &info, &query) == FAILURE) {
		return;
	} else {
		zend_string *str;
        if ((str = yaf_route_regex_assemble(getThis(), info, query)) != NULL) {
			RETURN_STR(str);
		}
		RETURN_NULL();
	}
}
/* }}} */

/** {{{ yaf_route_regex_methods
 */
zend_function_entry yaf_route_regex_methods[] = {
	PHP_ME(yaf_route_regex, __construct, yaf_route_regex_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
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
	zend_class_implements(yaf_route_regex_ce, 1, yaf_route_ce);
	yaf_route_regex_ce->ce_flags |= ZEND_ACC_FINAL;

	zend_declare_property_null(yaf_route_regex_ce, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_MATCH),  ZEND_ACC_PROTECTED);
	zend_declare_property_null(yaf_route_regex_ce, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_ROUTE),  ZEND_ACC_PROTECTED);
	zend_declare_property_null(yaf_route_regex_ce, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_MAP),    ZEND_ACC_PROTECTED);
	zend_declare_property_null(yaf_route_regex_ce, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_VERIFY), ZEND_ACC_PROTECTED);
	zend_declare_property_null(yaf_route_regex_ce, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_REVERSE), ZEND_ACC_PROTECTED);

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

