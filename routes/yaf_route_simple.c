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
#include "Zend/zend_smart_str.h" /* for smart_str */
#include "Zend/zend_interfaces.h" /* for zend_class_serialize_deny */

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_application.h" /* for yaf_application_is_module_name */
#include "yaf_request.h"
#include "yaf_router.h"

#include "routes/yaf_route_interface.h"
#include "routes/yaf_route_simple.h"

zend_class_entry *yaf_route_simple_ce;
static zend_object_handlers yaf_route_simple_obj_handlers;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_route_simple_construct_arginfo, 0, 0, 3)
	ZEND_ARG_INFO(0, module_name)
    ZEND_ARG_INFO(0, controller_name)
    ZEND_ARG_INFO(0, action_name)
ZEND_END_ARG_INFO()
/* }}} */

static HashTable *yaf_route_simple_get_properties(zval *object) /* {{{ */ {
	zval rv;
	HashTable *ht;
	yaf_route_simple_object *simple = Z_YAFROUTESIMPLEOBJ_P(object);

	if (!simple->properties) {
		ALLOC_HASHTABLE(simple->properties);
		zend_hash_init(simple->properties, 4, NULL, ZVAL_PTR_DTOR, 0);

		ht = simple->properties;
		ZVAL_STR_COPY(&rv, simple->m);
		zend_hash_str_add(ht, "module:protected", sizeof("module:protected") - 1, &rv);

		ZVAL_STR_COPY(&rv, simple->c);
		zend_hash_str_add(ht, "controller:protected", sizeof("controller:protected") - 1, &rv);

		ZVAL_STR_COPY(&rv, simple->a);
		zend_hash_str_add(ht, "action:protected", sizeof("action:protected") - 1, &rv);
	}

	return simple->properties;
}
/* }}} */

static zend_object *yaf_route_simple_new(zend_class_entry *ce) /* {{{ */ {
	yaf_route_simple_object *simple = emalloc(sizeof(yaf_route_simple_object));

	zend_object_std_init(&simple->std, ce);

	simple->std.handlers = &yaf_route_simple_obj_handlers;
	simple->m = simple->c = simple->a = NULL;
	simple->properties = NULL;

	return &simple->std;
}
/* }}} */

static void yaf_route_simple_object_free(zend_object *object) /* {{{ */ {
	yaf_route_simple_object *simple = (yaf_route_simple_object*)object;

	zend_string_release(simple->m);
	zend_string_release(simple->c);
	zend_string_release(simple->a);

	if (simple->properties) {
		if (GC_DELREF(simple->properties) == 0) {
			GC_REMOVE_FROM_BUFFER(simple->properties);
			zend_array_destroy(simple->properties);
		}
	}

	zend_object_std_dtor(&simple->std);
}
/* }}} */

static void yaf_route_simple_init(yaf_route_simple_object *simple, zend_string *m, zend_string *c, zend_string *a) /* {{{ */ {
	simple->m = zend_string_copy(m);
	simple->c = zend_string_copy(c);
	simple->a = zend_string_copy(a);
}
/* }}} */

void yaf_route_simple_instance(yaf_route_t *route, zend_string *m, zend_string *c, zend_string *a) /* {{{ */ {
	zend_object *simple = yaf_route_simple_new(yaf_route_simple_ce);

	yaf_route_simple_init((yaf_route_simple_object*)simple, m, c, a);

	ZVAL_OBJ(route, simple);
}
/* }}} */

int yaf_route_simple_route(yaf_route_t *route, yaf_request_t *req) /* {{{ */ {
	zval *module, *controller, *action;
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(req);
	yaf_route_simple_object *simple = Z_YAFROUTESIMPLEOBJ_P(route);

	module 	= yaf_request_query(YAF_GLOBAL_VARS_GET, simple->m);
	controller = yaf_request_query(YAF_GLOBAL_VARS_GET, simple->c);
	action = yaf_request_query(YAF_GLOBAL_VARS_GET, simple->a);

	if (!module && !controller && !action) {
		return 0;
	}

	if (module && Z_TYPE_P(module) == IS_STRING && yaf_application_is_module_name(Z_STR_P(module))) {
		yaf_request_set_module(request, Z_STR_P(module));
	}

	if (controller && Z_TYPE_P(controller) == IS_STRING) {
		yaf_request_set_controller(request, Z_STR_P(controller));
	}

	if (action && Z_TYPE_P(action) == IS_STRING) {
		yaf_request_set_action(request, Z_STR_P(action));
	}

	return 1;
}
/* }}} */

/** {{{ proto public Yaf_Route_Simple::route(Yaf_Request $req)
*/
PHP_METHOD(yaf_route_simple, route) {
	yaf_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &request, yaf_request_ce) == FAILURE) {
		return;
	} else {
		RETURN_BOOL(yaf_route_simple_route(getThis(), request));
	}
}
/* }}} */

zend_string * yaf_route_simple_assemble(yaf_route_simple_object *simple, zval *info, zval *query) /* {{{ */ {
	zval *zv;
	smart_str uri = {0};
	zend_string *val;

	smart_str_appendc(&uri, '?');

	if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_MOUDLE_FORMAT))) != NULL) {
		val = zval_get_string(zv);
		smart_str_appendl(&uri, ZSTR_VAL(simple->m), ZSTR_LEN(simple->m));
		smart_str_appendc(&uri, '=');
		smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
		smart_str_appendc(&uri, '&');
		zend_string_release(val);
	} 

	if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_CONTROLLER_FORMAT))) == NULL) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "%s", "You need to specify the controller by ':c'");
		smart_str_free(&uri);
		return NULL;
	}

	val = zval_get_string(zv);
	smart_str_appendl(&uri, ZSTR_VAL(simple->c), ZSTR_LEN(simple->c));
	smart_str_appendc(&uri, '=');
	smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
	smart_str_appendc(&uri, '&');
	zend_string_release(val);

	if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_ACTION_FORMAT))) == NULL) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "%s", "You need to specify the action by ':a'");
		smart_str_free(&uri);
		return NULL;
	}

	val = zval_get_string(zv);
	smart_str_appendl(&uri, ZSTR_VAL(simple->a), ZSTR_LEN(simple->a));
	smart_str_appendc(&uri, '=');
	smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
	zend_string_release(val);

	if (query && IS_ARRAY == Z_TYPE_P(query)) {
		zend_string *key;

		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(query), key, zv) {
			if (key) {
				val = zval_get_string(zv);
				smart_str_appendc(&uri, '&');
				smart_str_appendl(&uri, ZSTR_VAL(key), ZSTR_LEN(key));
				smart_str_appendc(&uri, '=');
				smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
				zend_string_release(val);
			}
		} ZEND_HASH_FOREACH_END();
	}

	smart_str_0(&uri);
	return uri.s;
}
/* }}} */

/** {{{ proto public Yaf_Route_Simple::__construct(string $module, string $controller, string $action)
 */
PHP_METHOD(yaf_route_simple, __construct) {
	zend_string *m, *c, *a;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "SSS", &m, &c, &a) == FAILURE) {
		return;
	}

	yaf_route_simple_init(Z_YAFROUTESIMPLEOBJ_P(getThis()), m, c, a);
}
/* }}} */

/** {{{ proto public Yaf_Route_Simple::assemble(array $info[, array $query = NULL])
 */
PHP_METHOD(yaf_route_simple, assemble) {
	zend_string *str;
    zval *info, *query = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|a", &info, &query) == FAILURE) {
        return;
    }

	if ((str = yaf_route_simple_assemble(Z_YAFROUTESIMPLEOBJ_P(getThis()), info, query))) {
		RETURN_STR(str);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ yaf_route_simple_methods
 */
zend_function_entry yaf_route_simple_methods[] = {
	PHP_ME(yaf_route_simple, __construct, yaf_route_simple_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(yaf_route_simple, route, yaf_route_route_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_route_simple, assemble, yaf_route_assemble_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
 */
YAF_STARTUP_FUNCTION(route_simple) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Route_Simple", "Yaf\\Route\\Simple", yaf_route_simple_methods);
	yaf_route_simple_ce = zend_register_internal_class(&ce);
	yaf_route_simple_ce->create_object = yaf_route_simple_new;
	yaf_route_simple_ce->ce_flags |= ZEND_ACC_FINAL;
	yaf_route_simple_ce->serialize = zend_class_serialize_deny;
	yaf_route_simple_ce->unserialize = zend_class_unserialize_deny;

	zend_class_implements(yaf_route_simple_ce, 1, yaf_route_ce);

	memcpy(&yaf_route_simple_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_route_simple_obj_handlers.free_obj = yaf_route_simple_object_free;
	yaf_route_simple_obj_handlers.clone_obj = NULL;
	yaf_route_simple_obj_handlers.get_gc = NULL;
	yaf_route_simple_obj_handlers.get_properties = yaf_route_simple_get_properties;

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
