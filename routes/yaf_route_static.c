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
#include "routes/yaf_route_static.h"
#include "zend_smart_str.h" /* for smart_str */

zend_class_entry * yaf_route_static_ce;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_route_static_match_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()
/* }}} */

int yaf_route_pathinfo_route(yaf_request_t *request, zend_string *req_uri) /* {{{ */ {
	zval params;
	char *module = NULL, *controller = NULL, *action = NULL, *rest = NULL;
	size_t module_len, controller_len, action_len;

	do {
#define strip_slashs(p) while (*p == ' ' || *p == '/') { ++p; }
		char *s, *p;
		char *uri;

		if (ZSTR_LEN(req_uri) == 0 ||
			(ZSTR_LEN(req_uri) == 1 && *ZSTR_VAL(req_uri) == '/')) {
			break;
		}

		uri = ZSTR_VAL(req_uri);
		s = p = uri;

		if (ZSTR_LEN(req_uri)) {
			char *q = ZSTR_VAL(req_uri) + ZSTR_LEN(req_uri) - 1;
			while (q > (char *)ZSTR_VAL(req_uri) && (*q == ' ' || *q == '/')) {
				*q-- = '\0';
			}
		}

		strip_slashs(p);

		if ((s = strstr(p, "/")) != NULL) {
			if (yaf_application_is_module_name_str(p, s-p)) {
				module = p;
				module_len = s - p;
				p  = s + 1;
		        strip_slashs(p);
				if ((s = strstr(p, "/")) != NULL) {
					controller = p;
					controller_len = s - p;
					p  = s + 1;
				}
			} else {
				controller = p;
				controller_len = s - p;
				p  = s + 1;
			}
		}

		strip_slashs(p);
		if ((s = strstr(p, "/")) != NULL) {
			action = p;
			action_len = s - p;
			p  = s + 1;
		}

		strip_slashs(p);
		if (*p != '\0') {
			do {
				if (!module && !controller && !action) {
					if (yaf_application_is_module_name_str(p, strlen(p))) {
						module = p;
						module_len = strlen(p);
						break;
					}
				}

				if (!controller) {
					controller = p;
					controller_len = strlen(p);
					break;
				}

				if (!action) {
					action = p;
					action_len = strlen(p);
					break;
				}

				rest = p;
			} while (0);
		}

		if (module && controller == NULL) {
			controller = module;
			controller_len = module_len;
			module = NULL;
		} else if (module && action == NULL) {
			action = controller;
			action_len = controller_len;
			controller = module;
			controller_len = module_len;
			module = NULL;
	    } else if (controller && action == NULL ) {
			/* /controller */
			if (YAF_G(action_prefer)) {
				action = controller;
				action_len = controller_len;
				controller = NULL;
			}
		}
	} while (0);

	if (module != NULL) {
		zend_update_property_stringl(yaf_request_ce,
				request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), module, module_len);
	}
	if (controller != NULL) {
		zend_update_property_stringl(yaf_request_ce,
				request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), controller, controller_len);
	}

	if (action != NULL) {
		zend_update_property_stringl(yaf_request_ce,
				request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), action, action_len);
	}

	if (rest) {
		(void)yaf_router_parse_parameters(rest, &params);
		(void)yaf_request_set_params_multi(request, &params);
		zval_ptr_dtor(&params);
	}

	return 1;
}
/* }}} */

int yaf_route_static_route(yaf_route_t *route, yaf_request_t *request) /* {{{ */ {
	zval *zuri, *base_uri;
	zend_string *req_uri;

	zuri = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_URI), 1, NULL);
	base_uri = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_BASE), 1, NULL);

	if (base_uri && IS_STRING == Z_TYPE_P(base_uri)
			&& !strncasecmp(Z_STRVAL_P(zuri), Z_STRVAL_P(base_uri), Z_STRLEN_P(base_uri))) {
		req_uri = zend_string_init(Z_STRVAL_P(zuri) + Z_STRLEN_P(base_uri), Z_STRLEN_P(zuri) - Z_STRLEN_P(base_uri), 0);
	} else {
		req_uri = zend_string_init(Z_STRVAL_P(zuri), Z_STRLEN_P(zuri), 0);
	}

	yaf_route_pathinfo_route(request, req_uri);

	zend_string_release(req_uri);
	return 1;
}
/* }}} */

/** {{{ zend_string * yaf_route_static_assemble(zval *info, zval *query)
 */
zend_string * yaf_route_static_assemble(yaf_route_t *this_ptr, zval *info, zval *query) {
	zval *zv;
	zend_string *val;
	smart_str str = {0};

	do {

		if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_MOUDLE_FORMAT))) != NULL) {
			val = zval_get_string(zv);
			smart_str_appendc(&str, '/');
			smart_str_appendl(&str, ZSTR_VAL(val), ZSTR_LEN(val));
			zend_string_release(val);
		}

		if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_CONTROLLER_FORMAT))) == NULL) {
			yaf_trigger_error(YAF_ERR_TYPE_ERROR, "%s", "You need to specify the controller by ':c'");
			break;
		}

		val = zval_get_string(zv);
		smart_str_appendc(&str, '/');
		smart_str_appendl(&str, ZSTR_VAL(val), ZSTR_LEN(val));
		zend_string_release(val);

		if((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_ACTION_FORMAT))) == NULL) {
			yaf_trigger_error(YAF_ERR_TYPE_ERROR, "%s", "You need to specify the action by ':a'");
			break;
		}

		val = zval_get_string(zv);
		smart_str_appendc(&str, '/');
		smart_str_appendl(&str, ZSTR_VAL(val), ZSTR_LEN(val));
		zend_string_release(val);

		if (query && IS_ARRAY == Z_TYPE_P(query)) {
			zend_string *key;
			int start = 0, end = 0;

			ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(query), key, zv) {
				if (key) {
					val = zval_get_string(zv);
					if (!start) {
						smart_str_appendc(&str, '?');
						start = 1;
					}
					if (end) {
						smart_str_appendc(&str, '&');
						end = 0;
					}
					smart_str_appendl(&str, ZSTR_VAL(key), ZSTR_LEN(key));
					smart_str_appendc(&str, '=');
					smart_str_appendl(&str, ZSTR_VAL(val), ZSTR_LEN(val));
					end = 1;
					zend_string_release(val);
				}
			} ZEND_HASH_FOREACH_END();
		}

		smart_str_0(&str);
		return str.s;
	} while (0);
		
	smart_str_free(&str);
	return NULL;
}
/* }}} */

/** {{{ proto public Yaf_Router_Static::route(Yaf_Request $req)
*/
PHP_METHOD(yaf_route_static, route) {
	yaf_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &request, yaf_request_ce) == FAILURE) {
		return;
	} else {
		RETURN_BOOL(yaf_route_static_route(getThis(), request));
	}
}
/* }}} */

/** {{{ proto public Yaf_Router_Static::match(string $uri)
*/
PHP_METHOD(yaf_route_static, match) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Route_Static::assemble(array $info[, array $query = NULL])
*/
PHP_METHOD(yaf_route_static, assemble) {
	zval *info, *query = NULL;
	zend_string *str;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|a", &info, &query) == FAILURE) {
        return;
    } else {
        if ((str = yaf_route_static_assemble(getThis(), info, query))) {
			RETURN_STR(str);
		} else {
			RETURN_FALSE;
		}
    }
}
/* }}} */

/** {{{ yaf_route_static_methods
 */
zend_function_entry yaf_route_static_methods[] = {
	PHP_ME(yaf_route_static, match, yaf_route_static_match_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_route_static, route, yaf_route_route_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_route_static, assemble, yaf_route_assemble_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
 */
YAF_STARTUP_FUNCTION(route_static) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Route_Static", "Yaf\\Route_Static", yaf_route_static_methods);
	yaf_route_static_ce = zend_register_internal_class(&ce);
	zend_class_implements(yaf_route_static_ce, 1, yaf_route_ce);

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

