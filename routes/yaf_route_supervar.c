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
#include "yaf_request.h"

#include "yaf_router.h"
#include "routes/yaf_route_interface.h"
#include "routes/yaf_route_static.h" /* for yaf_route_pathinfo_route */
#include "routes/yaf_route_supervar.h"

zend_class_entry *yaf_route_supervar_ce;
static zend_object_handlers yaf_route_supervar_obj_handlers;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_route_supervar_construct_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, supervar_name)
ZEND_END_ARG_INFO()
/* }}} */

static HashTable *yaf_route_supervar_get_properties(zval *object) /* {{{ */ {
	zval rv;
	yaf_route_supervar_object *super = Z_YAFROUTESUPEROBJ_P(object);

	if (!super->properties) {
		ALLOC_HASHTABLE(super->properties);
		zend_hash_init(super->properties, 2, NULL, ZVAL_PTR_DTOR, 0);

		ZVAL_STR_COPY(&rv, super->varname);
		zend_hash_str_add(super->properties, "varname:protected", sizeof("varname:protected") - 1, &rv);
	}

	return super->properties;
}
/* }}} */

static zend_object *yaf_route_supervar_new(zend_class_entry *ce) /* {{{ */ {
	yaf_route_supervar_object *supervar = emalloc(sizeof(yaf_route_supervar_object));

	zend_object_std_init(&supervar->std, ce);

	supervar->std.handlers = &yaf_route_supervar_obj_handlers;

	supervar->varname = NULL;
	supervar->properties = NULL;

	return &supervar->std;
}
/* }}} */

static void yaf_route_supervar_object_free(zend_object *object) /* {{{ */ {
	yaf_route_supervar_object *supervar = (yaf_route_supervar_object*)object;

	zend_string_release(supervar->varname);

	if (supervar->properties) {
		if (GC_DELREF(supervar->properties) == 0) {
			GC_REMOVE_FROM_BUFFER(supervar->properties);
			zend_array_destroy(supervar->properties);
		}
	}

	zend_object_std_dtor(&supervar->std);
}
/* }}} */

int yaf_route_supervar_route(yaf_route_t *route, yaf_request_t *req) /* {{{ */ {
	zval *uri;
	yaf_route_supervar_object *super = Z_YAFROUTESUPEROBJ_P(route);

	uri = yaf_request_query(YAF_GLOBAL_VARS_GET, super->varname);

	if (UNEXPECTED(uri == NULL)) {
		return 0;
	}

	yaf_route_pathinfo_route(Z_YAFREQUESTOBJ_P(req), Z_STRVAL_P(uri), Z_STRLEN_P(uri));

	return 1;
}
/* }}} */

static void yaf_route_supervar_init(yaf_route_supervar_object *super, zend_string *varname) /* {{{ */ {
	super->varname = zend_string_copy(varname);
}
/* }}} */

void yaf_route_supervar_instance(yaf_route_t *route, zend_string *varname) /* {{{ */ {
	zend_object *router = yaf_route_supervar_new(yaf_route_supervar_ce);

	yaf_route_supervar_init((yaf_route_supervar_object*)router, varname);

	ZVAL_OBJ(route, router);
}
/* }}} */

zend_string * yaf_route_supervar_assemble(yaf_route_supervar_object *super, zval *info, zval *query) /* {{{ */ {
	zval *zv;
	smart_str uri = {0};
	zend_string *val;


	smart_str_appendc(&uri, '?');
	smart_str_appendl(&uri, ZSTR_VAL(super->varname), ZSTR_LEN(super->varname));
	smart_str_appendc(&uri, '=');

	if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_MOUDLE_FORMAT))) != NULL) {
		val = zval_get_string(zv);
		smart_str_appendc(&uri, '/');
		smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
		zend_string_release(val);
	}

	if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_CONTROLLER_FORMAT))) == NULL) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "%s", "You need to specify the controller by ':c'");
		smart_str_free(&uri);
		return NULL;
	}

	val = zval_get_string(zv);
	smart_str_appendc(&uri, '/');
	smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
	zend_string_release(val);

	if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_ACTION_FORMAT))) == NULL) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "%s", "You need to specify the action by ':a'");
		smart_str_free(&uri);
		return NULL;
	}

	val = zval_get_string(zv);
	smart_str_appendc(&uri, '/');
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

/** {{{ proto public Yaf_Route_Supervar::route(Yaf_Request_Abstarct $request)
 */
PHP_METHOD(yaf_route_supervar, route) {
	yaf_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &request, yaf_request_ce) == FAILURE) {
		return;
	}
	RETURN_BOOL(yaf_route_supervar_route(getThis(), request));
}
/** }}} */

/** {{{ proto public Yaf_Route_Supervar::__construct(string $varname)
*/
PHP_METHOD(yaf_route_supervar, __construct) {
	zend_string *varname;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "S", &varname) == FAILURE) {
        return;
    }

	yaf_route_supervar_init(Z_YAFROUTESUPEROBJ_P(getThis()), varname);
}
/** }}} */

/** {{{ proto public Yaf_Route_Supervar::assemble(array $info[, array $query = NULL])
*/
PHP_METHOD(yaf_route_supervar, assemble) {
	zend_string *str;
    zval *info, *query = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|a", &info, &query) == FAILURE) {
        return;
    }
	if ((str = yaf_route_supervar_assemble(Z_YAFROUTESUPEROBJ_P(getThis()), info, query))) {
		RETURN_STR(str);
	}
	RETURN_NULL();
}
/* }}} */

/** {{{ yaf_route_supervar_methods
*/
zend_function_entry yaf_route_supervar_methods[] = {
	PHP_ME(yaf_route_supervar, __construct, yaf_route_supervar_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(yaf_route_supervar, route, yaf_route_route_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_route_supervar, assemble, yaf_route_assemble_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
 */
YAF_STARTUP_FUNCTION(route_supervar) {
	zend_class_entry ce;
	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Route_Supervar", "Yaf\\Route\\Supervar", yaf_route_supervar_methods);
	yaf_route_supervar_ce = zend_register_internal_class(&ce);
	yaf_route_supervar_ce->ce_flags |= ZEND_ACC_FINAL;

	yaf_route_supervar_ce->create_object = yaf_route_supervar_new;
	yaf_route_supervar_ce->serialize = zend_class_serialize_deny;
	yaf_route_supervar_ce->unserialize = zend_class_unserialize_deny;

	zend_class_implements(yaf_route_supervar_ce, 1, yaf_route_ce);

	memcpy(&yaf_route_supervar_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_route_supervar_obj_handlers.free_obj = yaf_route_supervar_object_free;
	yaf_route_supervar_obj_handlers.clone_obj = NULL;
	yaf_route_supervar_obj_handlers.get_gc = NULL;
	yaf_route_supervar_obj_handlers.get_properties = yaf_route_supervar_get_properties;


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

