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

#include "php_yaf.h"

#include "yaf_request.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_router.h"

#include "routes/yaf_route_interface.h"
#include "routes/yaf_route_static.h"
#include "routes/yaf_route_simple.h"
#include "routes/yaf_route_supervar.h"
#include "routes/yaf_route_regex.h"
#include "routes/yaf_route_rewrite.h"
#include "routes/yaf_route_map.h"

zend_class_entry *yaf_route_ce;

int yaf_route_instance(yaf_route_t *route, HashTable *config) /* {{{ */ {
	zval *pzval;

	if (UNEXPECTED(config == NULL)) {
		return 0;
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(config, ZEND_STRL("type"))) == NULL || IS_STRING != Z_TYPE_P(pzval))) {
		return 0;
	}

	if (zend_string_equals_literal_ci(Z_STR_P(pzval), "rewrite")) {
		zval *match, *router, *verify;
		if (UNEXPECTED((match = zend_hash_str_find(config, ZEND_STRL("match"))) == NULL || Z_TYPE_P(match) != IS_STRING)) {
			return 0;
		}

		if (UNEXPECTED((router = zend_hash_str_find(config, ZEND_STRL("route"))) == NULL || Z_TYPE_P(router) != IS_ARRAY)) {
			return 0;
		}

		if (((verify = zend_hash_str_find(config, ZEND_STRL("verify"))) == NULL || Z_TYPE_P(verify) != IS_ARRAY)) {
			verify = NULL;
		}

        yaf_route_rewrite_instance(route, Z_STR_P(match), router, verify);
	} else if (zend_string_equals_literal_ci(Z_STR_P(pzval), "regex")) {
		zval *match, *router, *verify, *map, *reverse;
		if (UNEXPECTED((match = zend_hash_str_find(config, ZEND_STRL("match"))) == NULL || Z_TYPE_P(match) != IS_STRING)) {
			return 0;
		}

		if (UNEXPECTED((router = zend_hash_str_find(config, ZEND_STRL("route"))) == NULL || Z_TYPE_P(router) != IS_ARRAY)) {
			return 0;
		}

		if (((map = zend_hash_str_find(config, ZEND_STRL("map"))) == NULL || Z_TYPE_P(map) != IS_ARRAY)) {
			map = NULL;
		}

		if ((verify = zend_hash_str_find(config, ZEND_STRL("verify"))) == NULL || Z_TYPE_P(verify) != IS_ARRAY) {
			verify = NULL;
		}

		if ((reverse = zend_hash_str_find(config, ZEND_STRL("reverse"))) == NULL || Z_TYPE_P(reverse) != IS_STRING) {
			reverse = NULL;
		}

		yaf_route_regex_instance(route, Z_STR_P(match), router, map, verify, reverse? Z_STR_P(reverse) : NULL);
	} else if (zend_string_equals_literal_ci(Z_STR_P(pzval), "map")) {
		zend_string *delimiter = NULL;
		zend_bool ctl_prefer = 0;
		
		if ((pzval = zend_hash_str_find(config, ZEND_STRL("controllerPrefer"))) != NULL) {
			ctl_prefer = zend_is_true(pzval);
		}

		if ((pzval = zend_hash_str_find(config, ZEND_STRL("delimiter"))) != NULL && Z_TYPE_P(pzval) == IS_STRING) {
			delimiter = Z_STR_P(pzval);
		}

		yaf_route_map_instance(route, ctl_prefer, delimiter);
	} else if (zend_string_equals_literal_ci(Z_STR_P(pzval), "simple")) {
		zval *m, *c, *a;

		if (UNEXPECTED((m = zend_hash_str_find(config, ZEND_STRL("module"))) == NULL || Z_TYPE_P(m) != IS_STRING)) {
			return 0;
		}
		if (UNEXPECTED((c = zend_hash_str_find(config, ZEND_STRL("controller"))) == NULL || Z_TYPE_P(c) != IS_STRING)) {
			return 0;
		}
		if (UNEXPECTED((a = zend_hash_str_find(config, ZEND_STRL("action"))) == NULL || Z_TYPE_P(a) != IS_STRING)) {
			return 0;
		}

		yaf_route_simple_instance(route, Z_STR_P(a), Z_STR_P(c), Z_STR_P(a));
	} else if (zend_string_equals_literal_ci(Z_STR_P(pzval), "supervar")) {
		zval *varname;
		if (UNEXPECTED((varname = zend_hash_str_find(config, ZEND_STRL("varname"))) == NULL || Z_TYPE_P(varname) != IS_STRING)) {
			return 0;
		}
		yaf_route_supervar_instance(route, Z_STR_P(varname));
	}

	return 1;
}
/* }}} */

/** {{{ yaf_route_methods
 */
zend_function_entry yaf_route_methods[] = {
	PHP_ABSTRACT_ME(yaf_route, route, yaf_route_route_arginfo)
	PHP_ABSTRACT_ME(yaf_route, assemble, yaf_route_assemble_arginfo)
    {NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
 */
YAF_STARTUP_FUNCTION(route) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Route_Interface", "Yaf\\Route_Interface", yaf_route_methods);
	yaf_route_ce = zend_register_internal_interface(&ce);

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
