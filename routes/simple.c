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

/* $Id: simple.c 325432 2012-04-24 09:06:40Z laruence $ */

zend_class_entry *yaf_route_simple_ce;

#define YAF_ROUTE_SIMPLE_VAR_NAME_MODULE		"module"
#define	YAF_ROUTE_SIMPLE_VAR_NAME_CONTROLLER 	"controller"
#define YAF_ROUTE_SIMPLE_VAR_NAME_ACTION		"action"

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_route_simple_construct_arginfo, 0, 0, 3)
	ZEND_ARG_INFO(0, module_name)
    ZEND_ARG_INFO(0, controller_name)
    ZEND_ARG_INFO(0, action_name)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ int yaf_route_simple_route(yaf_route_t *route, yaf_request_t *request TSRMLS_DC)
 */
int yaf_route_simple_route(yaf_route_t *route, yaf_request_t *request TSRMLS_DC) {
	zval *module, *controller, *action;
	zval *nmodule, *ncontroller, *naction;

	nmodule 	= zend_read_property(yaf_route_simple_ce, route, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_MODULE), 1 TSRMLS_CC);
	ncontroller = zend_read_property(yaf_route_simple_ce, route, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_CONTROLLER), 1 TSRMLS_CC);
	naction 	= zend_read_property(yaf_route_simple_ce, route, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_ACTION), 1 TSRMLS_CC);

	/* if there is no expect parameter in supervars, then null will be return */
	module 		= yaf_request_query(YAF_GLOBAL_VARS_GET, Z_STRVAL_P(nmodule), Z_STRLEN_P(nmodule) TSRMLS_CC);
	controller 	= yaf_request_query(YAF_GLOBAL_VARS_GET, Z_STRVAL_P(ncontroller), Z_STRLEN_P(ncontroller) TSRMLS_CC);
	action 		= yaf_request_query(YAF_GLOBAL_VARS_GET, Z_STRVAL_P(naction), Z_STRLEN_P(naction) TSRMLS_CC);

	if (ZVAL_IS_NULL(module) && ZVAL_IS_NULL(controller) && ZVAL_IS_NULL(action)) {
		return 0;
	}

	if (Z_TYPE_P(module) == IS_STRING && yaf_application_is_module_name(Z_STRVAL_P(module), Z_STRLEN_P(module) TSRMLS_CC)) {
		zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), module TSRMLS_CC);
	}

	zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), controller TSRMLS_CC);
	zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), action TSRMLS_CC);

	return 1;
}
/* }}} */

/** {{{ yaf_route_t * yaf_route_simple_instance(yaf_route_t *this_ptr, zval *module, zval *controller, zval *action TSRMLS_DC)
 */
yaf_route_t * yaf_route_simple_instance(yaf_route_t *this_ptr, zval *module, zval *controller, zval *action TSRMLS_DC) {
	yaf_route_t *instance;

	if (this_ptr) {
		instance  = this_ptr;
	} else {
		MAKE_STD_ZVAL(instance);
		object_init_ex(instance, yaf_route_simple_ce);
	}

	zend_update_property(yaf_route_simple_ce, instance, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_MODULE), module TSRMLS_CC);
	zend_update_property(yaf_route_simple_ce, instance, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_CONTROLLER), controller TSRMLS_CC);
	zend_update_property(yaf_route_simple_ce, instance, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_ACTION), action TSRMLS_CC);

	return instance;
}
/* }}} */

/** {{{ zval * yaf_route_simple_assemble(zval *mvc, zval *query TSRMLS_DC)
 */
zval * yaf_route_simple_assemble(yaf_route_t *this_ptr, zval *mvc, zval *query TSRMLS_DC) {
	char tvalue[1024];
	uint tvalue_len = 0;
	zval *nmodule, *ncontroller, *naction;
	zval *uri;

	MAKE_STD_ZVAL(uri);
	memcpy(tvalue, "?", 1);
	tvalue_len += 1;
	
	nmodule         = zend_read_property(yaf_route_simple_ce, this_ptr, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_MODULE), 1 TSRMLS_CC);
	ncontroller   = zend_read_property(yaf_route_simple_ce, this_ptr, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_CONTROLLER), 1 TSRMLS_CC);
	naction         = zend_read_property(yaf_route_simple_ce, this_ptr, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_ACTION), 1 TSRMLS_CC);

	do {
		zval **tmp;
		char tsprintf[1024];
		uint tlen;

		if (zend_hash_find(Z_ARRVAL_P(mvc), ZEND_STRS(YAF_ROUTE_VAR_NAME_MODULE), (void **)&tmp) == SUCCESS) {
			tlen = strlen("=&") + Z_STRLEN_P(nmodule) + Z_STRLEN_PP(tmp);
			tlen = snprintf(tsprintf, tlen + 1, "%s=%s&", Z_STRVAL_P(nmodule), Z_STRVAL_PP(tmp));
			if (tlen) {
				memcpy(&tvalue[tvalue_len], tsprintf, strlen(tsprintf));
				tvalue_len += tlen;
			}
		}
	
		if (zend_hash_find(Z_ARRVAL_P(mvc), ZEND_STRS(YAF_ROUTE_VAR_NAME_CONTROLLER), (void **)&tmp) == FAILURE) {
			yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "%s", "You need to specify the controller");
			break;
		} 
		
		tlen = strlen("=&") + Z_STRLEN_P(ncontroller) + Z_STRLEN_PP(tmp);
		tlen = snprintf(tsprintf, tlen + 1, "%s=%s&", Z_STRVAL_P(ncontroller), Z_STRVAL_PP(tmp));
		if (tlen) {
			memcpy(&tvalue[tvalue_len], tsprintf, strlen(tsprintf));
			tvalue_len += tlen;
		}

		if(zend_hash_find(Z_ARRVAL_P(mvc), ZEND_STRS(YAF_ROUTE_VAR_NAME_ACTION), (void **)&tmp) == FAILURE) {
			yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "%s", "You need to specify the action");
                        break;
                }
	
		tlen = strlen("=") + Z_STRLEN_P(naction) + Z_STRLEN_PP(tmp);
                tlen = snprintf(tsprintf, tlen + 1, "%s=%s", Z_STRVAL_P(naction), Z_STRVAL_PP(tmp));
		if (tlen) {
			memcpy(&tvalue[tvalue_len], tsprintf, strlen(tsprintf));
			tvalue_len += tlen;
		}

		if ( IS_ARRAY == Z_TYPE_P(query)) {
			uint key_type, key_len;
			char *key;
			ulong key_idx;
			
			for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(query));
                        	zend_hash_get_current_data(Z_ARRVAL_P(query), (void **)&tmp) == SUCCESS;
                        	zend_hash_move_forward(Z_ARRVAL_P(query))) {
	
				if (IS_STRING == Z_TYPE_PP(tmp) 
					&& HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(Z_ARRVAL_P(query), &key, &key_len, &key_idx, 0, NULL)) {

					tlen = strlen("=&") + key_len + Z_STRLEN_PP(tmp);
					tlen = snprintf(tsprintf, tlen + 1, "&%s=%s", key, Z_STRVAL_PP(tmp));
                			if (tlen) {
						memcpy(&tvalue[tvalue_len], tsprintf, strlen(tsprintf));
						tvalue_len += tlen;
                			}
				}
			}
		}

		tvalue[tvalue_len] = '\0';
		ZVAL_STRING(uri, tvalue, 0);
		return uri;
	} while (0);
	
	ZVAL_NULL(uri);
	return uri;
}

/** {{{ proto public Yaf_Route_Simple::route(Yaf_Request $req)
*/
PHP_METHOD(yaf_route_simple, route) {
	yaf_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &request) == FAILURE) {
		return;
	} else {
		RETURN_BOOL(yaf_route_simple_route(getThis(), request TSRMLS_CC));
	}
}
/* }}} */

/** {{{ proto public Yaf_Route_Simple::__construct(string $module, string $controller, string $action)
 */
PHP_METHOD(yaf_route_simple, __construct) {
	zval *module, *controller, *action;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &module, &controller, &action) == FAILURE) {
		return;
	}

	if (IS_STRING != Z_TYPE_P(module)
			|| IS_STRING != Z_TYPE_P(controller)
			|| IS_STRING != Z_TYPE_P(action)) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "Expect 3 string paramsters", yaf_route_simple_ce->name);
		RETURN_FALSE;
	} else {
		(void)yaf_route_simple_instance(getThis(), module, controller, action TSRMLS_CC);
	}
}
/* }}} */

/** {{{ proto public Yaf_Route_Simple::assemble(array $mvc[, array $query = NULL])
*/
PHP_METHOD(yaf_route_simple, assemble) {
	zval *mvc, *query;
	zval *return_uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &mvc, &query) == FAILURE) {
		return;
	} else {
		return_uri = yaf_route_simple_assemble(getThis(), mvc, query TSRMLS_CC);
		RETURN_ZVAL(return_uri, 1, 0);
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
	yaf_route_simple_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
	zend_class_implements(yaf_route_simple_ce TSRMLS_CC, 1, yaf_route_ce);

	yaf_route_simple_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;

	zend_declare_property_null(yaf_route_simple_ce, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_CONTROLLER), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_route_simple_ce, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_MODULE), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_route_simple_ce, ZEND_STRL(YAF_ROUTE_SIMPLE_VAR_NAME_ACTION), ZEND_ACC_PROTECTED TSRMLS_CC);

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
