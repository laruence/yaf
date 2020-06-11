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
#include "routes/yaf_route_map.h"
#include "routes/yaf_route_rewrite.h"
#include "routes/yaf_route_regex.h"
#include "routes/yaf_route_supervar.h"
#include "routes/yaf_route_simple.h"

zend_class_entry *yaf_router_ce;
static zend_object_handlers yaf_router_obj_handlers;

/** {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(yaf_router_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_router_name_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
/* }}} */

static zend_object *yaf_router_new(zend_class_entry *ce) /* {{{ */ {
	yaf_router_object *router = emalloc(sizeof(yaf_router_object) + zend_object_properties_size(yaf_router_ce));

	zend_object_std_init(&router->std, ce);
	router->std.handlers = &yaf_router_obj_handlers;

	zend_hash_init(&router->routes, 8, NULL, ZVAL_PTR_DTOR, 0);
	ZVAL_NULL(&router->current);
	router->properties = NULL;

	return &router->std;
}
/* }}} */

static void yaf_router_object_free(zend_object *object) /* {{{ */ {
	yaf_router_object *router = php_yaf_router_fetch_object(object);

	zend_hash_destroy(&router->routes);
	if (router->properties) {
		if (GC_DELREF(router->properties) == 0) {
			GC_REMOVE_FROM_BUFFER(router->properties);
			zend_array_destroy(router->properties);
		}
	}
	zend_object_std_dtor(object);
}
/* }}} */

static HashTable *yaf_router_get_properties(zval *object) /* {{{ */ {
	zval rv;
	HashTable *ht;
	yaf_router_object *router = Z_YAFROUTEROBJ_P(object);

	if (!router->properties) {
		ALLOC_HASHTABLE(router->properties);
		zend_hash_init(router->properties, 2, NULL, ZVAL_PTR_DTOR, 0);
		YAF_ALLOW_VIOLATION(router->properties);
	}

	ht = router->properties;
	ZVAL_ARR(&rv, zend_array_dup(&router->routes));
	zend_hash_str_update(ht, "routes:protected", sizeof("routes:protected") - 1, &rv);

	ZVAL_COPY(&rv, &router->current);
	zend_hash_str_update(ht, "current:protected", sizeof("current:protected") - 1, &rv);

	return ht;
}
/* }}} */

void yaf_router_init(yaf_router_object *router) /* {{{ */ {
	zval route;
	yaf_application_object *app = yaf_application_instance();

	if (app == NULL || app->default_route == NULL) {
static_route:
		object_init_ex(&route, yaf_route_static_ce);
	} else {
		if (UNEXPECTED(!yaf_route_instance(&route, app->default_route))) {
			OBJ_RELEASE(Z_OBJ(route));
			php_error_docref(NULL, E_WARNING,
					"Unable to initialize default route, use %s instead", ZSTR_VAL(yaf_route_static_ce->name));
			goto static_route;
		}
	}
	zend_hash_str_update(&router->routes, "_default", sizeof("_default") - 1, &route);
}
/* }}} */

void yaf_router_instance(yaf_router_t *this_ptr) /* {{{ */ {
	ZVAL_OBJ(this_ptr, yaf_router_new(yaf_router_ce));
	yaf_router_init(Z_YAFROUTEROBJ_P(this_ptr));
}
/** }}} */

ZEND_HOT int yaf_router_route(yaf_router_object *router, yaf_request_t *request) /* {{{ */ {
	zend_string *key;
	zend_ulong  idx;
	yaf_route_t *route;
	HashTable *routes = &router->routes;

	ZEND_HASH_REVERSE_FOREACH_KEY_VAL(routes, idx, key, route) {
		if (Z_OBJCE_P(route) == yaf_route_static_ce) {
			yaf_route_static_route(route, request);
		} else if (Z_OBJCE_P(route) == yaf_route_map_ce) {
			if (!yaf_route_map_route(route, request)) {
				continue;
			}
		} else if (Z_OBJCE_P(route) == yaf_route_rewrite_ce) {
			if (!yaf_route_rewrite_route(route, request)) {
				continue;
			}
		} else if (UNEXPECTED(Z_OBJCE_P(route) == yaf_route_regex_ce)) {
			if (!yaf_route_regex_route(route, request)) {
				continue;
			}
		} else if (UNEXPECTED(Z_OBJCE_P(route) == yaf_route_supervar_ce)) {
			if (!yaf_route_supervar_route(route, request)) {
				continue;
			}
		} else if (UNEXPECTED(Z_OBJCE_P(route) == yaf_route_simple_ce)) {
			if (!yaf_route_simple_route(route, request)) {
				continue;
			}
		} else {
			zval ret;
			zend_call_method_with_1_params(route, Z_OBJCE_P(route), NULL, "route", &ret, request);
			if (Z_TYPE(ret) != IS_TRUE && (Z_TYPE(ret) != IS_LONG || !Z_LVAL(ret))) {
				zval_ptr_dtor(&ret);
				continue;
			}
		}

		if (key) {
			ZVAL_STR(&router->current, key);
		} else {
			ZVAL_LONG(&router->current, idx);
		}

		yaf_request_set_routed(Z_YAFREQUESTOBJ_P(request), 1);
		return 1;
	} ZEND_HASH_FOREACH_END();

	return 0;
}
/* }}} */

void yaf_router_parse_parameters(const char *str, size_t len, zval *params) /* {{{ */ {
	char *k, *v;
	uint32_t l;
	zval *zv, rv;

	array_init(params);

	if (UNEXPECTED(len == 0)) {
		return;
	}

	ZVAL_NULL(&rv);
	while (1) {
		if ((k = memchr(str, YAF_ROUTER_URL_DELIMIETER, len))) {
			l = k++ - str;
			if (l) {
				zv = zend_hash_str_update(Z_ARRVAL_P(params), str, l, &rv);
				len -= k - str;
				if ((v = memchr(k, YAF_ROUTER_URL_DELIMIETER, len))) {
					if (v - k) {
						ZVAL_STRINGL(zv, k, v - k);
					}
					str = v + 1;
					len -= str - k;
					if (len) {
						continue;
					}
				} else if (len) {
					ZVAL_STRINGL(zv, k, len);
				}
			} else {
				str = k;
				len--;
				continue;
			}
		} else if (len) {
			zend_hash_str_update(Z_ARRVAL_P(params), str, len, &rv);
		}
		return;
	}
}
/* }}} */

int yaf_router_add_config(yaf_router_object *router, zend_array *configs) /* {{{ */ {
	zval rv;
	zend_ulong idx;
	zend_string *key;
	zval *entry;

	if (UNEXPECTED(configs == NULL)) {
		return 0;
	}

	ZEND_HASH_FOREACH_KEY_VAL(configs, idx, key, entry) {
		if (Z_TYPE_P(entry) != IS_ARRAY) {
			continue;
		}
		if (UNEXPECTED(!yaf_route_instance(&rv, Z_ARRVAL_P(entry)))) {
			if (key) {
				php_error_docref(NULL, E_WARNING, "Unable to initialize route named '%s'", ZSTR_VAL(key));
			} else {
				php_error_docref(NULL, E_WARNING, "Unable to initialize route at index '"ZEND_ULONG_FMT"'", idx);
			}
			continue;
		} else if (key) {
			zend_hash_update(&router->routes, key, &rv);
		} else {
			zend_hash_index_update(&router->routes, idx, &rv);
		}
	} ZEND_HASH_FOREACH_END();

	return 1;
}
/* }}} */

/** {{{ proto public Yaf_Router::__construct(void)
 */
PHP_METHOD(yaf_router, __construct) {
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	yaf_router_init(Z_YAFROUTEROBJ_P(getThis()));
}
/* }}} */

/** {{{ proto public Yaf_Router::route(Yaf_Request $req)
*/
PHP_METHOD(yaf_router, route) {
	yaf_request_t *request;
	yaf_router_object *router = Z_YAFROUTEROBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &request, yaf_request_ce) == FAILURE) {
		return;
	}

	RETURN_BOOL(yaf_router_route(router, request));
}
/* }}} */

/** {{{  proto public Yaf_Router::addRoute(string $name, Yaf_Route_Interface $route)
 */
PHP_METHOD(yaf_router, addRoute) {
	zend_string *name = NULL;
	yaf_route_t *route;
	yaf_router_object *router = Z_YAFROUTEROBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SO", &name, &route, yaf_route_ce) == FAILURE) {
		return;
	}

	Z_TRY_ADDREF_P(route);
	zend_hash_update(&router->routes, name, route);

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{  proto public Yaf_Router::addConfig(Yaf_Config_Abstract $config)
 */
PHP_METHOD(yaf_router, addConfig) {
	zend_array *routes;
	yaf_config_t *config;
	yaf_router_object *router = Z_YAFROUTEROBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &config) == FAILURE) {
		return;
	}

	if (IS_OBJECT == Z_TYPE_P(config) && instanceof_function(Z_OBJCE_P(config), yaf_config_ce)){
		yaf_config_object *conf = Z_YAFCONFIGOBJ_P(config);
		routes = conf->config;
	} else if (IS_ARRAY == Z_TYPE_P(config)) {
		routes = Z_ARRVAL_P(config);
	} else {
		php_error_docref(NULL, E_WARNING,
				"Expect a %s instance or an array, %s given",
				ZSTR_VAL(yaf_config_ce->name), zend_zval_type_name(config));
		RETURN_FALSE;
	}

	if (yaf_router_add_config(router, routes)) {
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/** {{{  proto public Yaf_Router::getRoute(string $name)
 */
PHP_METHOD(yaf_router, getRoute) {
	zend_string *name;
	yaf_route_t *route;
	yaf_router_object *router = Z_YAFROUTEROBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(name) == 0) {
		RETURN_FALSE;
	}

	if ((route = zend_hash_find(&router->routes, name)) != NULL) {
		RETURN_ZVAL(route, 1, 0);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{  proto public Yaf_Router::getRoutes(void)
 */
PHP_METHOD(yaf_router, getRoutes) {
	yaf_router_object *router = Z_YAFROUTEROBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_ARR(zend_array_dup(&router->routes));
}
/* }}} */

/** {{{ proto public Yaf_Router::isModuleName(string $name)
 */
PHP_METHOD(yaf_router, isModuleName) {
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	RETURN_BOOL(yaf_application_is_module_name(name));
}
/* }}} */

/** {{{  proto public Yaf_Router::getCurrentRoute(void)
 */
PHP_METHOD(yaf_router, getCurrentRoute) {
	yaf_router_object *router = Z_YAFROUTEROBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_ZVAL(&router->current, 1, 0);
}
/* }}} */

/** {{{ yaf_router_methods
 */
zend_function_entry yaf_router_methods[] = {
	PHP_ME(yaf_router, __construct,	yaf_router_void_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(yaf_router, addRoute,  	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_router, addConfig, 	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_router, route,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_router, getRoute,  	yaf_router_name_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_router, getRoutes,   yaf_router_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_router, getCurrentRoute,	yaf_router_void_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
 */
YAF_STARTUP_FUNCTION(router) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Router", "Yaf\\Router", yaf_router_methods);
	yaf_router_ce = zend_register_internal_class_ex(&ce, NULL);
	yaf_router_ce->ce_flags |= ZEND_ACC_FINAL;
	yaf_router_ce->create_object = yaf_router_new;
	yaf_router_ce->serialize = zend_class_serialize_deny;
	yaf_router_ce->unserialize = zend_class_unserialize_deny;

	memcpy(&yaf_router_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_router_obj_handlers.offset = XtOffsetOf(yaf_router_object, std);
	yaf_router_obj_handlers.clone_obj = NULL;
	yaf_router_obj_handlers.get_gc = NULL;
	yaf_router_obj_handlers.free_obj = yaf_router_object_free;
	yaf_router_obj_handlers.get_properties = yaf_router_get_properties;

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
