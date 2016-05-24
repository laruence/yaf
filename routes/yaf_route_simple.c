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
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_application.h" /* for yaf_application_is_module_name */
#include "yaf_request.h"
#include "yaf_router.h"

#include "routes/yaf_route_interface.h"
#include "routes/yaf_route_simple.h"
#include "zend_smart_str.h" /* for smart_str */

zend_class_entry *yaf_route_simple_ce;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_route_simple_construct_arginfo, 0, 0, 3)
	ZEND_ARG_INFO(0, module_name)
    ZEND_ARG_INFO(0, controller_name)
    ZEND_ARG_INFO(0, action_name)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ int yaf_route_simple_route(yaf_route_t *route, yaf_request_t *request)
 */
int yaf_route_simple_route(yaf_route_t *route, yaf_request_t *request) {
	zval *module, *controller, *action;
	zval *nmodule, *ncontroller, *naction;

	nmodule	= zend_read_property(yaf_route_simple_ce,
			route, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_MODULE), 1, NULL);
	ncontroller = zend_read_property(yaf_route_simple_ce,
			route, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_CONTROLLER), 1, NULL);
	naction = zend_read_property(yaf_route_simple_ce,
			route, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_ACTION), 1, NULL);

	/* if there is no expect parameter in supervars, then null will be return */
	module 	= yaf_request_query(YAF_GLOBAL_VARS_GET, Z_STR_P(nmodule));
	controller = yaf_request_query(YAF_GLOBAL_VARS_GET, Z_STR_P(ncontroller));
	action = yaf_request_query(YAF_GLOBAL_VARS_GET, Z_STR_P(naction));

	if (!module && !controller && !action) {
		return 0;
	}

	if (module && Z_TYPE_P(module) == IS_STRING && yaf_application_is_module_name(Z_STR_P(module))) {
		zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), module);
	}

	if (controller) {
		zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), controller);
	}

	if (action) {
		zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), action);
	}

	return 1;
}
/* }}} */

/** {{{ yaf_route_t * yaf_route_simple_instance(yaf_route_t *this_ptr, zval *module, zval *controller, zval *action)
 */
yaf_route_t * yaf_route_simple_instance(yaf_route_t *this_ptr, zval *module, zval *controller, zval *action) {
	if (Z_ISUNDEF_P(this_ptr)) {
		object_init_ex(this_ptr, yaf_route_simple_ce);
	}

	zend_update_property(yaf_route_simple_ce, this_ptr, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_MODULE), module);
	zend_update_property(yaf_route_simple_ce, this_ptr, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_CONTROLLER), controller);
	zend_update_property(yaf_route_simple_ce, this_ptr, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_ACTION), action);

	return this_ptr;
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

/** {{{ zend_string * yaf_route_simple_assemble(zval *info, zval *query)
 */
zend_string * yaf_route_simple_assemble(yaf_route_t *this_ptr, zval *info, zval *query) {
	smart_str uri = {0};
	zend_string *val;
	zval *nmodule, *ncontroller, *naction;

	smart_str_appendc(&uri, '?');

	nmodule = zend_read_property(yaf_route_simple_ce,
			this_ptr, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_MODULE), 1, NULL);
	ncontroller = zend_read_property(yaf_route_simple_ce,
			this_ptr, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_CONTROLLER), 1, NULL);
	naction = zend_read_property(yaf_route_simple_ce,
			this_ptr, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_ACTION), 1, NULL);

	do {
		zval *zv;

		if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_MOUDLE_FORMAT))) != NULL) {
			val = zval_get_string(zv);
			smart_str_appendl(&uri, Z_STRVAL_P(nmodule), Z_STRLEN_P(nmodule));
			smart_str_appendc(&uri, '=');
			smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
			smart_str_appendc(&uri, '&');
			zend_string_release(val);
		} 

		if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_CONTROLLER_FORMAT))) == NULL) {
			yaf_trigger_error(YAF_ERR_TYPE_ERROR, "%s", "You need to specify the controller by ':c'");
			break;
		}

		val = zval_get_string(zv);
		smart_str_appendl(&uri, Z_STRVAL_P(ncontroller), Z_STRLEN_P(ncontroller));
		smart_str_appendc(&uri, '=');
		smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
		smart_str_appendc(&uri, '&');
		zend_string_release(val);

		if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_ACTION_FORMAT))) == NULL) {
			yaf_trigger_error(YAF_ERR_TYPE_ERROR, "%s", "You need to specify the action by ':a'");
			break;
		}

		val = zval_get_string(zv);
		smart_str_appendl(&uri, Z_STRVAL_P(naction), Z_STRLEN_P(naction));
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
	} while (0);

	return NULL;
}
/* }}} */

/** {{{ proto public Yaf_Route_Simple::__construct(string $module, string $controller, string $action)
 */
PHP_METHOD(yaf_route_simple, __construct) {
	zval *module, *controller, *action;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zzz", &module, &controller, &action) == FAILURE) {
		return;
	}

	if (IS_STRING != Z_TYPE_P(module)
			|| IS_STRING != Z_TYPE_P(controller)
			|| IS_STRING != Z_TYPE_P(action)) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Expect 3 string parameters");
		RETURN_FALSE;
	} else {
	    zval rself, *self = getThis();

	    if (!self) {
	        ZVAL_NULL(&rself);
	        self = &rself;
        }
		(void)yaf_route_simple_instance(self, module, controller, action);
	}
}
/* }}} */

/** {{{ proto public Yaf_Route_Simple::assemble(array $info[, array $query = NULL])
 */
PHP_METHOD(yaf_route_simple, assemble) {
    zval *info, *query = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|a", &info, &query) == FAILURE) {
        return;
    } else {
		zend_string *str;
        if ((str = yaf_route_simple_assemble(getThis(), info, query)) != NULL) {
			RETURN_STR(str);
		}
		RETURN_NULL();
    }
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
	zend_class_implements(yaf_route_simple_ce, 1, yaf_route_ce);

	yaf_route_simple_ce->ce_flags |= ZEND_ACC_FINAL;

	zend_declare_property_null(yaf_route_simple_ce, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_CONTROLLER), ZEND_ACC_PROTECTED);
	zend_declare_property_null(yaf_route_simple_ce, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_MODULE), ZEND_ACC_PROTECTED);
	zend_declare_property_null(yaf_route_simple_ce, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_ACTION), ZEND_ACC_PROTECTED);

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
