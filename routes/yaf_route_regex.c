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

/* $Id: regex.c 329197 2013-01-18 05:55:37Z laruence $ */

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

/** {{{ yaf_route_t * yaf_route_regex_instance(yaf_route_t *this_ptr, zval *route, zval *def, zval *map, zval *verify, zval reverse TSRMLS_DC)
 */
yaf_route_t * yaf_route_regex_instance(yaf_route_t *this_ptr, zval *route, zval *def, zval *map, zval *verify, zval *reverse TSRMLS_DC) {
	yaf_route_t	*instance;

	instance = this_ptr;
	if (ZVAL_IS_NULL(this_ptr)) {
		object_init_ex(instance, yaf_route_regex_ce);
	} 

	zend_update_property(yaf_route_regex_ce, instance, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_MATCH), route TSRMLS_CC);
	zend_update_property(yaf_route_regex_ce, instance, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_ROUTE), def TSRMLS_CC);

	if (map) {
		zend_update_property(yaf_route_regex_ce, instance, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_MAP), map TSRMLS_CC);
	}

	if (!verify) {
		zend_update_property_null(yaf_route_regex_ce, instance, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_VERIFY) TSRMLS_CC);
	} else {
		zend_update_property(yaf_route_regex_ce, instance, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_VERIFY), verify TSRMLS_CC);
	}
	

	if (!reverse || IS_STRING != Z_TYPE_P(reverse)) {
		zend_update_property_null(yaf_route_regex_ce, instance, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_REVERSE) TSRMLS_CC);
	} else {
		zend_update_property(yaf_route_regex_ce, instance, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_REVERSE), reverse TSRMLS_CC);
	}

	return instance;
}
/* }}} */

/** {{{ static void yaf_route_regex_match(yaf_route_t *router, char *uri, int len, zval *ret TSRMLS_DC)
 */
static void yaf_route_regex_match(yaf_route_t *route, char *uri, int len, zval *ret TSRMLS_DC) {
	zval *match;
	pcre_cache_entry *pce_regexp;

	if (!len) {
		return;
	}

	match = zend_read_property(yaf_route_regex_ce, route, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_MATCH), 1 TSRMLS_CC);

	if ((pce_regexp = pcre_get_compiled_regex_cache(Z_STR_P(match) TSRMLS_CC)) == NULL) {
		return;
	} else {
		zval matches, subparts, *map;

		ZVAL_NULL(&subparts);

		map = zend_read_property(yaf_route_regex_ce, route, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_MAP), 1 TSRMLS_CC);
		if (IS_ARRAY != Z_TYPE_P(map)) {
			map = NULL;
		}

		php_pcre_match_impl(pce_regexp, uri, len, &matches, &subparts /* subpats */,
				0/* global */, 0/* ZEND_NUM_ARGS() >= 4 */, 0/*flags PREG_OFFSET_CAPTURE*/, 0/* start_offset */ TSRMLS_CC);

		if (!zend_hash_num_elements(Z_ARRVAL(subparts))) {
			zval_ptr_dtor(&subparts);
			return;
		} else {
			zval  *name, *pzval;
			zend_string	*key = NULL;
			ulong	idx	 = 0;
			HashTable 	*ht;

			array_init(ret);

			ht = Z_ARRVAL(subparts);
			ZEND_HASH_FOREACH_KEY_VAL(ht, idx, key, pzval) {

				if (key) {
					Z_ADDREF_P(pzval);
					zend_hash_update(Z_ARRVAL_P(ret), key, pzval);
				} else {	
					if (map && (name = zend_hash_index_find(Z_ARRVAL_P(map), idx)) != NULL && Z_TYPE_P(name) == IS_STRING) {
						Z_ADDREF_P(pzval);
						zend_hash_update(Z_ARRVAL_P(ret), Z_STR_P(name), pzval);
					}
				}
			} ZEND_HASH_FOREACH_END();

			zval_ptr_dtor(&subparts);
			return;
		}
	}
}
/* }}} */

/** {{{ void yaf_route_regex_assemble(yaf_route_t *this_ptr, zval *info, zval *query, zval *uri TSRMLS_DC)
 */
void yaf_route_regex_assemble(yaf_route_t *this_ptr, zval *info, zval *query, zval *uri TSRMLS_DC) {
	zval *reverse;
	zval *tmp;
	zend_string *tstr, *inter;
	smart_str squery = {0};

	reverse = zend_read_property(yaf_route_regex_ce, this_ptr, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_REVERSE), 0 TSRMLS_CC);

	if (Z_TYPE_P(reverse) != IS_STRING) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "%s", "Reverse property is not a valid string");
		return;
	}

	tstr = zend_string_init(Z_STRVAL_P(reverse), Z_STRLEN_P(reverse), 0);

	if ((tmp = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_MOUDLE_FORMAT))) != NULL) {
		inter = php_str_to_str(tstr->val, tstr->len, ZEND_STRL(YAF_ROUTE_ASSEMBLE_MOUDLE_FORMAT), Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
		zend_string_release(tstr);
		tstr = inter;
	}

	if ((tmp = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_CONTROLLER_FORMAT))) != NULL) {
		inter = php_str_to_str(tstr->val, tstr->len, ZEND_STRL(YAF_ROUTE_ASSEMBLE_CONTROLLER_FORMAT), Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
		zend_string_release(tstr);
		tstr = inter;
	}

	if ((tmp = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_ACTION_FORMAT))) != NULL) {
		inter = php_str_to_str(tstr->val, tstr->len, ZEND_STRL(YAF_ROUTE_ASSEMBLE_ACTION_FORMAT), Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
		zend_string_release(tstr);
		tstr = inter;
	}

	if (query && IS_ARRAY == Z_TYPE_P(query)) {
		zend_string *key;
		ulong key_idx;
		HashTable *ht = Z_ARRVAL_P(query);

		smart_str_appendc(&squery, '?');
		ZEND_HASH_FOREACH_KEY_VAL(ht, key_idx, key, tmp) {

			if (key) {
				if (IS_STRING == Z_TYPE_P(tmp)) {
					smart_str_appendl(&squery, key->val, key->len);
					smart_str_appendc(&squery, '=');
					smart_str_appendl(&squery, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
					smart_str_appendc(&squery, '&');
				}
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (squery.s->len) {
		uint tmp_len = tstr->len;
		squery.s->len--; /* get rid of the tail & */
		smart_str_0(&squery);
		tstr = zend_string_realloc(tstr, tstr->len + squery.s->len, 0);
		memcpy(tstr->val + tmp_len, squery.s->val, squery.s->len);
		tstr->val[tstr->len] = '\0';
	}

	ZVAL_STR(uri, tstr);
	zend_string_release(tstr);
	smart_str_free(&squery);
}
/** }}} */

/** {{{ int yaf_route_regex_route(yaf_route_t *router, yaf_request_t *request TSRMLS_DC)
 */
int yaf_route_regex_route(yaf_route_t *router, yaf_request_t *request TSRMLS_DC) {
	char *request_uri;
	zval *args, *base_uri, *zuri, rargs;

	zuri 	 = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_URI), 1 TSRMLS_CC);
	base_uri = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_BASE), 1 TSRMLS_CC);

	if (base_uri && IS_STRING == Z_TYPE_P(base_uri)
			&& !strncasecmp(Z_STRVAL_P(zuri), Z_STRVAL_P(base_uri), Z_STRLEN_P(base_uri))) {
		request_uri = estrdup(Z_STRVAL_P(zuri) + Z_STRLEN_P(base_uri));
	} else {
		request_uri = estrdup(Z_STRVAL_P(zuri));
	}

	args = &rargs;
	ZVAL_NULL(&rargs);
	yaf_route_regex_match(router, request_uri, strlen(request_uri), args TSRMLS_CC);
	if (ZVAL_IS_NULL(args) || Z_TYPE_P(args) != IS_ARRAY) {
		efree(request_uri);
		return 0;
	} else {
		zval *module, *controller, *action, *routes;

		routes = zend_read_property(yaf_route_regex_ce, router, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_ROUTE), 1 TSRMLS_CC);
		if ((module = zend_hash_str_find(Z_ARRVAL_P(routes), ZEND_STRL("module"))) != NULL && IS_STRING == Z_TYPE_P(module)) {
			if (Z_STRVAL_P(module)[0] != ':') {
				zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), module TSRMLS_CC);
			} else {
				zval *m;
				if ((m = zend_hash_str_find(Z_ARRVAL_P(args), Z_STRVAL_P(module) + 1, Z_STRLEN_P(module) - 1)) != NULL && IS_STRING == Z_TYPE_P(m)) {
					zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), m TSRMLS_CC);
				}
			}
		}

		if ((controller = zend_hash_str_find(Z_ARRVAL_P(routes), ZEND_STRL("controller"))) != NULL && IS_STRING == Z_TYPE_P(controller)) {
			if (Z_STRVAL_P(controller)[0] != ':') {
				zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), controller TSRMLS_CC);
			} else {
				zval *c;
				if ((c = zend_hash_str_find(Z_ARRVAL_P(args), Z_STRVAL_P(controller) + 1, Z_STRLEN_P(controller) - 1)) != NULL && IS_STRING == Z_TYPE_P(c)) {
					zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), c TSRMLS_CC);
				}
			}
		}

		if ((action = zend_hash_str_find(Z_ARRVAL_P(routes), ZEND_STRL("action"))) != NULL && IS_STRING == Z_TYPE_P(action)) {
			if (Z_STRVAL_P(action)[0] != ':') {
				zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), action TSRMLS_CC);
			} else {
				zval *a;
				if ((a = zend_hash_str_find(Z_ARRVAL_P(args), Z_STRVAL_P(action) + 1, Z_STRLEN_P(action) - 1)) != NULL && IS_STRING == Z_TYPE_P(a)) {
					zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), a TSRMLS_CC);
				}
			}
		}

		(void)yaf_request_set_params_multi(request, args TSRMLS_CC);
		zval_ptr_dtor(args);
		efree(request_uri);
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
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &request, yaf_request_ce) == FAILURE) {
		return;
	}

	if (!request || IS_OBJECT != Z_TYPE_P(request)
			|| !instanceof_function(Z_OBJCE_P(request), yaf_request_ce TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expects a %s instance",  yaf_request_ce->name->val);
		RETURN_FALSE;
	}

	RETURN_BOOL(yaf_route_regex_route(route, request TSRMLS_CC));
}
/** }}} */

/** {{{ proto public Yaf_Route_Regex::__construct(string $match, array $route, array $map = NULL, array $verify = NULL, string reverse = NULL)
 */
PHP_METHOD(yaf_route_regex, __construct) {
	zval 		*match, *route, *map = NULL, *verify = NULL, *reverse = NULL;
	yaf_route_t	rself, *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "za|aaz", &match, &route, &map, &verify, &reverse) ==  FAILURE) {
		YAF_UNINITIALIZED_OBJECT(getThis());
		return;
	}

	if (IS_STRING != Z_TYPE_P(match) || !Z_STRLEN_P(match)) {
		YAF_UNINITIALIZED_OBJECT(getThis());
		yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "Expects a valid string as the first parameter", yaf_route_regex_ce->name);
		RETURN_FALSE;
	}

	if (verify && IS_ARRAY != Z_TYPE_P(verify)) {
		YAF_UNINITIALIZED_OBJECT(getThis());
		yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "Expects an array as third parameter",  yaf_route_regex_ce->name);
		RETURN_FALSE;
	}

	if (reverse && IS_STRING != Z_TYPE_P(reverse)) {
		YAF_UNINITIALIZED_OBJECT(getThis());
		yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "Expects a valid string reverse as fourth parameter");
		RETURN_FALSE;
	}

	if (!self) {
		ZVAL_NULL(&rself);
		self = &rself;
	}

	yaf_route_regex_instance(self, match, route, map, verify, reverse TSRMLS_CC);

	if (self) {
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/** }}} */

/** {{{ proto public Yaf_Route_regex::assemble(array $info[, array $query = NULL])
 */
PHP_METHOD(yaf_route_regex, assemble) {
	zval *info, *query = NULL, *return_uri = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|a", &info, &query) == FAILURE) {
		return;
	} else {
        yaf_route_regex_assemble(getThis(), info, query, &return_uri TSRMLS_CC);
        RETURN_ZVAL(&return_uri, 0, 1);
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
	yaf_route_regex_ce = zend_register_internal_class_ex(&ce, yaf_route_ce TSRMLS_CC);
	zend_class_implements(yaf_route_regex_ce TSRMLS_CC, 1, yaf_route_ce);
	yaf_route_regex_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;

	zend_declare_property_null(yaf_route_regex_ce, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_MATCH),  ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_route_regex_ce, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_ROUTE),  ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_route_regex_ce, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_MAP),    ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_route_regex_ce, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_VERIFY), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_route_regex_ce, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_REVERSE), ZEND_ACC_PROTECTED TSRMLS_CC);

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

