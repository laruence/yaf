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
#include "yaf_router.h"
#include "yaf_exception.h"
#include "requests/yaf_request_simple.h"

zend_class_entry *yaf_request_simple_ce;

void yaf_request_simple_init(yaf_request_object *request, zend_string *module, zend_string *controller, zend_string *action, zend_string *method, zval *params) /* {{{ */ {
	if (!method) {
		const char *method = yaf_request_get_request_method();
		request->method = zend_string_init(method, strlen(method), 0);
	} else {
		request->method = zend_string_copy(method);
	}

	if (module || controller || action) {
		if (module) {
			yaf_request_set_module(request, module);
		} else {
			request->module = zend_string_init(ZEND_STRL(YAF_ROUTER_DEFAULT_MODULE), 0);
		}

		if (controller) {
			yaf_request_set_controller(request, controller);
		} else {
			request->controller = zend_string_init(ZEND_STRL(YAF_ROUTER_DEFAULT_CONTROLLER), 0);
		}

		if (action) {
			yaf_request_set_action(request, action);
		} else {
			request->action = zend_string_init(ZEND_STRL(YAF_ROUTER_DEFAULT_ACTION), 0);
		}

		yaf_request_set_routed(request, 1);
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

                    query = Z_STRVAL_P(pzval) + sizeof(YAF_REQUEST_SERVER_URI);
                    break;
                }
			} ZEND_HASH_FOREACH_END();
		}

		if (query) {
			request->uri = zend_string_init(query, strlen(query), 0);
		} else {
			request->uri = ZSTR_EMPTY_ALLOC();
		}
	}

	if (params) {
		if (!request->params) {
			ALLOC_HASHTABLE(request->params);
			zend_hash_init(request->params, zend_hash_num_elements(Z_ARRVAL_P(params)), NULL, ZVAL_PTR_DTOR, 0);
			YAF_ALLOW_VIOLATION(request->params);
		}
		zend_hash_copy(request->params, Z_ARRVAL_P(params), (copy_ctor_func_t)zval_add_ref);
	}

	return;
}
/* }}} */

/** {{{ proto public Yaf_Request_Simple::__construct(string $method, string $module, string $controller, string $action, array $params = NULL)
*/
PHP_METHOD(yaf_request_simple, __construct) {
	zend_string *module  = NULL;
	zend_string *controller = NULL;
	zend_string *action = NULL;
	zend_string *method = NULL;
	zval *params = NULL;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|S!S!S!S!a", &method, &module, &controller, &action, &params) == FAILURE) {
		return;
	}
	yaf_request_simple_init(Z_YAFREQUESTOBJ_P(getThis()), module, controller, action, method, params);
}
/* }}} */

/** {{{ proto public Yaf_Request_Simple::isXmlHttpRequest()
*/
PHP_METHOD(yaf_request_simple, isXmlHttpRequest) {
	RETURN_FALSE;
}
/* }}} */

/** {{{ yaf_request_simple_methods
 */
zend_function_entry yaf_request_simple_methods[] = {
	PHP_ME(yaf_request_simple, __construct,	NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
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
