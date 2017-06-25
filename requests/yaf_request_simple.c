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
#include "main/SAPI.h"

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_request.h"
#include "yaf_exception.h"
#include "requests/yaf_request_simple.h"

static zend_class_entry *yaf_request_simple_ce;

yaf_request_t *yaf_request_simple_instance(yaf_request_t *this_ptr, zval *module, zval *controller, zval *action, zval *method, zval *params) /* {{{ */ {
	zval zv;

	if (!method || Z_TYPE_P(method) != IS_STRING) {
		if (!SG(request_info).request_method) {
			if (!strncasecmp(sapi_module.name, "cli", 3)) {
				ZVAL_STRING(&zv, "CLI");
			} else {
				ZVAL_STRING(&zv, "Unknow");
			}
		} else {
			ZVAL_STRING(&zv, (char *)SG(request_info).request_method);
		}
		method = &zv;
	} else {
		Z_TRY_ADDREF_P(method);
	}

	zend_update_property(yaf_request_simple_ce, this_ptr, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_METHOD), method);
	zval_ptr_dtor(method);

	if (module || controller || action) {
		if (!module || Z_TYPE_P(module) != IS_STRING) {
			zend_update_property_str(yaf_request_simple_ce, this_ptr,
				   	ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), YAF_G(default_module));
		} else {
			zend_update_property(yaf_request_simple_ce, this_ptr, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), module);
		}

		if (!controller || Z_TYPE_P(controller) != IS_STRING) {
			zend_update_property_str(yaf_request_simple_ce, this_ptr,
				   	ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), YAF_G(default_controller));
		} else {
			zend_update_property(yaf_request_simple_ce, this_ptr,
					ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), controller);
		}

		if (!action || Z_TYPE_P(action) != IS_STRING) {
			zend_update_property_str(yaf_request_simple_ce, this_ptr,
				   	ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), YAF_G(default_action));
		} else {
			zend_update_property(yaf_request_simple_ce, this_ptr,
				   	ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), action);
		}

		zend_update_property_bool(yaf_request_simple_ce, this_ptr, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ROUTED), 1);
	} else {
		zval *argv, *pzval;
		char *query = NULL;

		argv = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, "argv", sizeof("argv") -1);
		if (argv && IS_ARRAY == Z_TYPE_P(argv)) {
		    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(argv), pzval) {
                if (Z_TYPE_P(pzval) == IS_STRING) {
                    if (strncasecmp(Z_STRVAL_P(pzval), YAF_REQUEST_SERVER_URI, sizeof(YAF_REQUEST_SERVER_URI) - 1)) {
                        continue;
                    }

                    query = estrdup(Z_STRVAL_P(pzval) + sizeof(YAF_REQUEST_SERVER_URI));
                    break;
                }
			} ZEND_HASH_FOREACH_END();
		}

		if (query) {
			zend_update_property_string(yaf_request_simple_ce, this_ptr, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_URI), query);
		} else {
			zend_update_property_string(yaf_request_simple_ce, this_ptr, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_URI), "");
		}
	}

	if (!params) {
		array_init(&zv);
		zend_update_property(yaf_request_simple_ce, this_ptr, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_PARAMS), &zv);
		zval_ptr_dtor(&zv);
	} else {
		zend_update_property(yaf_request_simple_ce, this_ptr, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_PARAMS), params);
	}

	return this_ptr;
}
/* }}} */

/** {{{ proto public Yaf_Request_Simple::__construct(string $method, string $module, string $controller, string $action, array $params = NULL)
*/
PHP_METHOD(yaf_request_simple, __construct) {
	zval *module  = NULL;
	zval *controller = NULL;
	zval *action = NULL;
	zval *params = NULL;
	zval *method = NULL;
	zval *self = getThis();

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|zzzza", &method, &module, &controller, &action, &params) == FAILURE) {
		return;
	} else {
        if (params) {
			SEPARATE_ZVAL(params);
		} 

        if (UNEXPECTED(!self)) {
			RETURN_FALSE;
        }

		(void)yaf_request_simple_instance(self, module, controller, action, method, params);
	}
}
/* }}} */

/** {{{ proto public Yaf_Request_Simple::getQuery(mixed $name, mixed $default = NULL)
*/
YAF_REQUEST_METHOD(yaf_request_simple, Query, 	YAF_GLOBAL_VARS_GET);
/* }}} */

/** {{{ proto public Yaf_Request_Simple::getPost(mixed $name, mixed $default = NULL)
*/
YAF_REQUEST_METHOD(yaf_request_simple, Post,  	YAF_GLOBAL_VARS_POST);
/* }}} */

/** {{{ proto public Yaf_Request_Simple::getRequet(mixed $name, mixed $default = NULL)
*/
YAF_REQUEST_METHOD(yaf_request_simple, Request, YAF_GLOBAL_VARS_REQUEST);
/* }}} */

/** {{{ proto public Yaf_Request_Simple::getFiles(mixed $name, mixed $default = NULL)
*/
YAF_REQUEST_METHOD(yaf_request_simple, Files, 	YAF_GLOBAL_VARS_FILES);
/* }}} */

/** {{{ proto public Yaf_Request_Simple::getCookie(mixed $name, mixed $default = NULL)
*/
YAF_REQUEST_METHOD(yaf_request_simple, Cookie, 	YAF_GLOBAL_VARS_COOKIE);
/* }}} */

/** {{{ proto public Yaf_Request_Simple::isXmlHttpRequest()
*/
PHP_METHOD(yaf_request_simple, isXmlHttpRequest) {
	zend_string *name;
	zval *header;
	name = zend_string_init("X-Requested-With", sizeof("X-Requested-With") - 1 , 0);
   	header = yaf_request_query(YAF_GLOBAL_VARS_SERVER, name);
	zend_string_release(name);
	if (header && Z_TYPE_P(header) == IS_STRING
			&& strncasecmp("XMLHttpRequest", Z_STRVAL_P(header), Z_STRLEN_P(header)) == 0) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Request_Simple::get(mixed $name, mixed $default)
 * params -> post -> get -> cookie -> server
 */
PHP_METHOD(yaf_request_simple, get) {
	zend_string	*name 	= NULL;
	zval 	*def 	= NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &name, &def) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		zval *value = yaf_request_get_param(getThis(), name);
		if (value) {
			RETURN_ZVAL(value, 1, 0);
		} else {
			zval *params	= NULL;
			zval *pzval	= NULL;

			YAF_GLOBAL_VARS_TYPE methods[4] = {
				YAF_GLOBAL_VARS_POST,
				YAF_GLOBAL_VARS_GET,
				YAF_GLOBAL_VARS_COOKIE,
				YAF_GLOBAL_VARS_SERVER
			};

			{
				int i = 0;
				for (;i<4; i++) {
					params = &PG(http_globals)[methods[i]];
					if (params && Z_TYPE_P(params) == IS_ARRAY) {
						if ((pzval = zend_hash_find(Z_ARRVAL_P(params), name)) != NULL ){
							RETURN_ZVAL(pzval, 1, 0);
						}
					}
				}

			}
			if (def) {
				RETURN_ZVAL(def, 1, 0);
			}
		}
	}
	RETURN_NULL();
}
/* }}} */

/** {{{ proto private Yaf_Request_Simple::__clone
 */
PHP_METHOD(yaf_request_simple, __clone) {
}
/* }}} */

/** {{{ yaf_request_simple_methods
 */
zend_function_entry yaf_request_simple_methods[] = {
	PHP_ME(yaf_request_simple, __construct,	NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(yaf_request_simple, __clone,		NULL, ZEND_ACC_PRIVATE)
	PHP_ME(yaf_request_simple, getQuery, 	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request_simple, getRequest, 	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request_simple, getPost, 		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request_simple, getCookie,	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request_simple, getFiles,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request_simple, get,			NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request_simple, isXmlHttpRequest,NULL,ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
 */
YAF_STARTUP_FUNCTION(request_simple){
	zend_class_entry ce;
	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Request_Simple", "Yaf\\Request\\Simple", yaf_request_simple_methods);
	yaf_request_simple_ce = zend_register_internal_class_ex(&ce, yaf_request_ce);
	yaf_request_simple_ce->ce_flags |= ZEND_ACC_FINAL;

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
