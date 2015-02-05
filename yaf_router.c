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

/* $Id: yaf_router.c 329200 2013-01-18 06:26:40Z laruence $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "Zend/zend_interfaces.h"

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_application.h" /* for yaf_application_is_module_name */
#include "yaf_request.h" /* for yaf_request_set_routed */ 
#include "yaf_router.h"
#include "yaf_config.h"

#include "routes/yaf_route_interface.h"
#include "routes/yaf_route_static.h"
#include "routes/yaf_route_simple.h"
#include "routes/yaf_route_supervar.h"
#include "routes/yaf_route_regex.h"
#include "routes/yaf_route_rewrite.h"
#include "routes/yaf_route_map.h"

zend_class_entry *yaf_router_ce;

/** {{{ yaf_router_t * yaf_router_instance(yaf_router_t *this_ptr TSRMLS_DC)
 */
yaf_router_t * yaf_router_instance(yaf_router_t *this_ptr TSRMLS_DC) {
	zval 			routes;
	yaf_router_t 		*instance;
	yaf_route_t		*route, rroute;

    instance = this_ptr;
    if (ZVAL_IS_NULL(this_ptr)) {
		object_init_ex(instance, yaf_router_ce);
	}

	array_init(&routes);
	ZVAL_NULL(&rroute);
	route = &rroute;

	if (!YAF_G(default_route)) {
static_route:
		object_init_ex(&rroute, yaf_route_static_ce);
	} else {
		route = yaf_route_instance(&rroute, YAF_G(default_route) TSRMLS_CC);
		if (!route) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to initialize default route, use %s instead", yaf_route_static_ce->name->val);
			goto static_route;
		}
	}

	zend_hash_str_update(Z_ARRVAL(routes), "_default", sizeof("_default") - 1, route);
	zend_update_property(yaf_router_ce, instance, ZEND_STRL(YAF_ROUTER_PROPERTY_NAME_ROUTES), &routes TSRMLS_CC);
	zval_ptr_dtor(&routes);

	return instance;
}
/** }}} */

/** {{{ int yaf_router_route(yaf_router_t *router, yaf_request_t *request TSRMLS_DC)
*/
int yaf_router_route(yaf_router_t *router, yaf_request_t *request TSRMLS_DC) {
	zval 		*routers, ret;
	yaf_route_t	*route;
	HashTable 	*ht;
	zend_string *key;
	ulong idx = 0;

	routers = zend_read_property(yaf_router_ce, router, ZEND_STRL(YAF_ROUTER_PROPERTY_NAME_ROUTES), 1 TSRMLS_CC);

	ht = Z_ARRVAL_P(routers);

	ZEND_HASH_REVERSE_FOREACH_KEY_VAL(ht, idx, key, route) {

		zend_call_method_with_1_params(route, Z_OBJCE_P(route), NULL, "route", &ret, request);

		if (IS_TRUE == Z_TYPE(ret)) {
			if (key) {
				zend_update_property_string(yaf_router_ce, router, ZEND_STRL(YAF_ROUTER_PROPERTY_NAME_CURRENT_ROUTE), key->val TSRMLS_CC);
			} else {
				zend_update_property_long(yaf_router_ce, router, ZEND_STRL(YAF_ROUTER_PROPERTY_NAME_CURRENT_ROUTE), idx TSRMLS_CC);
			}

			yaf_request_set_routed(request, 1 TSRMLS_CC);
			zval_ptr_dtor(&ret);
			break;
		} else {
			zval_ptr_dtor(&ret);
			continue;
		}
	} ZEND_HASH_FOREACH_END();
	return 1;
}
/* }}} */

/** {{{ int yaf_router_add_config(yaf_router_t *router, zval *configs TSRMLS_DC)
*/
int yaf_router_add_config(yaf_router_t *router, zval *configs TSRMLS_DC) {
	zval 		*entry;
	HashTable 	*ht;
	yaf_route_t 	*route, rroute;

	if (!configs || IS_ARRAY != Z_TYPE_P(configs)) {
		return 0;
	} else {
		zend_string *key = NULL;
		ulong idx = 0;
		zval *routes;

		routes = zend_read_property(yaf_router_ce, router, ZEND_STRL(YAF_ROUTER_PROPERTY_NAME_ROUTES), 1 TSRMLS_CC);

		ht = Z_ARRVAL_P(configs);
		ZEND_HASH_FOREACH_KEY_VAL(ht, idx, key, entry) {
			if (!entry || Z_TYPE_P(entry) != IS_ARRAY) {
				continue;
			}

			ZVAL_NULL(&rroute);
			route = yaf_route_instance(&rroute, entry TSRMLS_CC);
			if (key) {
				if (!route) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to initialize route named '%s'", key->val);
					continue;
				}
				zend_hash_update(Z_ARRVAL_P(routes), key, route);
			} else {
				if (!route) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to initialize route at index '%ld'", idx);
					continue;
				}
				zend_hash_index_update(Z_ARRVAL_P(routes), idx, route);
			}
		} ZEND_HASH_FOREACH_END();
		return 1;
	}
}
/* }}} */

/** {{{ void yaf_router_parse_parameters(char *uri, zval *params TSRMLS_DC)
 */
void yaf_router_parse_parameters(char *uri, zval *params TSRMLS_DC) {
	char *key, *ptrptr, *tmp, *value;
	zval val;
	uint key_len;

	array_init(params);

	tmp = estrdup(uri);
	key = php_strtok_r(tmp, YAF_ROUTER_URL_DELIMIETER, &ptrptr);
	while (key) {
		key_len = strlen(key);
		if (key_len) {
			value = php_strtok_r(NULL, YAF_ROUTER_URL_DELIMIETER, &ptrptr);
			if (value && strlen(value)) {
				ZVAL_STRING(&val, value);
			} else {
				ZVAL_NULL(&val);
			}
			zend_hash_str_update(Z_ARRVAL_P(params), key, key_len, &val);
		}

		key = php_strtok_r(NULL, YAF_ROUTER_URL_DELIMIETER, &ptrptr);
	}

	efree(tmp);
}
/* }}} */

/** {{{ proto public Yaf_Router::__construct(void)
 */
PHP_METHOD(yaf_router, __construct) {
	zval rself, *self = getThis();

	if (!self) {
		ZVAL_NULL(&rself);
		self = &rself;
	}
	yaf_router_instance(self TSRMLS_CC);
}
/* }}} */

/** {{{ proto public Yaf_Router::route(Yaf_Request $req)
*/
PHP_METHOD(yaf_router, route) {
	yaf_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &request) == FAILURE) {
		return;
	} else {
		RETURN_BOOL(yaf_router_route(getThis(), request TSRMLS_CC));
	}
}
/* }}} */

/** {{{  proto public Yaf_Router::addRoute(string $name, Yaf_Route_Interface $route)
 */
PHP_METHOD(yaf_router, addRoute) {
	zend_string 	   *name = NULL;
	zval 	   *routes;
	yaf_route_t *route;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Sz", &name, &route) == FAILURE) {
		return;
	}

	if (!name) {
		RETURN_FALSE;
	}

	if (IS_OBJECT != Z_TYPE_P(route)
			|| !instanceof_function(Z_OBJCE_P(route), yaf_route_ce TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expects a %s instance", yaf_route_ce->name->val);
		RETURN_FALSE;
	}

	routes = zend_read_property(yaf_router_ce, getThis(), ZEND_STRL(YAF_ROUTER_PROPERTY_NAME_ROUTES), 1 TSRMLS_CC);

	Z_TRY_ADDREF_P(route);
	zend_hash_update(Z_ARRVAL_P(routes), name, route);

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{  proto public Yaf_Router::addConfig(Yaf_Config_Abstract $config)
 */
PHP_METHOD(yaf_router, addConfig) {
	yaf_config_t 	*config;
	zval		*routes;
	zval rself, *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &config) == FAILURE) {
		return;
	}

	if (IS_OBJECT == Z_TYPE_P(config) && instanceof_function(Z_OBJCE_P(config), yaf_config_ce TSRMLS_CC)){
		routes = zend_read_property(yaf_config_ce, config, ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
	} else if (IS_ARRAY == Z_TYPE_P(config)) {
		routes = config;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,  "Expect a %s instance or an array, %s given", yaf_config_ce->name->val, zend_zval_type_name(config));
		RETURN_FALSE;
	}

	if (!self) {
		ZVAL_NULL(&rself);
		self = &rself;
	}

	if (yaf_router_add_config(self, routes TSRMLS_CC)) {
		RETURN_ZVAL(self, 1, 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/** {{{  proto public Yaf_Router::getRoute(string $name)
 */
PHP_METHOD(yaf_router, getRoute) {
	zend_string  *name;
	zval  *routes;
	yaf_route_t *route;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &name) == FAILURE) {
		return;
	}

	if (!name->len) {
		RETURN_FALSE;
	}

	routes = zend_read_property(yaf_router_ce, getThis(), ZEND_STRL(YAF_ROUTER_PROPERTY_NAME_ROUTES), 1 TSRMLS_CC);

	if ((route = zend_hash_find(Z_ARRVAL_P(routes), name)) != NULL) {
		RETURN_ZVAL(route, 1, 0);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{  proto public Yaf_Router::getRoutes(void)
 */
PHP_METHOD(yaf_router, getRoutes) {
	zval * routes = zend_read_property(yaf_router_ce, getThis(), ZEND_STRL(YAF_ROUTER_PROPERTY_NAME_ROUTES), 1 TSRMLS_CC);
	RETURN_ZVAL(routes, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Router::isModuleName(string $name)
 */
PHP_METHOD(yaf_router, isModuleName) {
	char *name;
	size_t len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len) == FAILURE) {
		return;
	}

	RETURN_BOOL(yaf_application_is_module_name(name, len TSRMLS_CC));
}
/* }}} */

/** {{{  proto public Yaf_Router::getCurrentRoute(void)
 */
PHP_METHOD(yaf_router, getCurrentRoute) {
	zval *route = zend_read_property(yaf_router_ce, getThis(), ZEND_STRL(YAF_ROUTER_PROPERTY_NAME_CURRENT_ROUTE), 1 TSRMLS_CC);
	RETURN_ZVAL(route, 1, 0);
}
/* }}} */

/** {{{ yaf_router_methods
 */
zend_function_entry yaf_router_methods[] = {
	PHP_ME(yaf_router, __construct, 	NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(yaf_router, addRoute,  		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_router, addConfig, 		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_router, route,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_router, getRoute,  		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_router, getRoutes, 		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_router, getCurrentRoute, 	NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
 */
YAF_STARTUP_FUNCTION(router) {
	zend_class_entry ce;
	(void)yaf_route_route_arginfo; /* tricky, supress warning "defined but not used" */

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Router", "Yaf\\Router", yaf_router_methods);
	yaf_router_ce = zend_register_internal_class_ex(&ce, NULL TSRMLS_CC);

	yaf_router_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;

	zend_declare_property_null(yaf_router_ce, ZEND_STRL(YAF_ROUTER_PROPERTY_NAME_ROUTES), 		 ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_router_ce, ZEND_STRL(YAF_ROUTER_PROPERTY_NAME_CURRENT_ROUTE), ZEND_ACC_PROTECTED TSRMLS_CC);

	YAF_STARTUP(route);
	YAF_STARTUP(route_static);
	YAF_STARTUP(route_simple);
	YAF_STARTUP(route_supervar);
	YAF_STARTUP(route_rewrite);
	YAF_STARTUP(route_regex);
	YAF_STARTUP(route_map);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
