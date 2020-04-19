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

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_application.h" /* for yaf_application_is_module_name */
#include "yaf_request.h"

#include "yaf_router.h"
#include "routes/yaf_route_interface.h"
#include "routes/yaf_route_static.h"

zend_class_entry * yaf_route_static_ce;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_route_static_match_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()
/* }}} */

static inline void yaf_route_strip_uri(const char **req_uri, size_t *req_uri_len) /* {{{ */ {
	register const char *p = *req_uri;
	size_t l = *req_uri_len;
	while (*p == YAF_ROUTER_URL_DELIMIETER) {
		p++; l--;
	}
	*req_uri = p;
	*req_uri_len = l;
}
/* }}} */

void yaf_route_pathinfo_route(yaf_request_object *request, const char *req_uri, size_t req_uri_len) /* {{{ */ {
	const char *pos;
	const char *parts[3];
	uint32_t lens[3];
	uint32_t current = 0;

	yaf_route_strip_uri(&req_uri, &req_uri_len);
	if (req_uri_len == 0) {
		return;
	}

	do {
		parts[current] = req_uri;
		lens[current] = req_uri_len;
		req_uri_len = 0;
		if ((pos = memchr(req_uri, YAF_ROUTER_URL_DELIMIETER, lens[current])) != NULL) {
			req_uri_len = lens[current] - (pos - req_uri + 1);
			lens[current] = pos - req_uri;
			req_uri = pos + 1;
			yaf_route_strip_uri(&req_uri, &req_uri_len);
		}
	} while (++current < 3 && req_uri_len);

	switch (current) {
		case 1:
			if (UNEXPECTED(yaf_is_action_prefer())) {
				parts[2] = parts[0];
				parts[0] = NULL;
				parts[1] = NULL;
				lens[2] = lens[0];
			} else {
				parts[1] = parts[0];
				parts[0] = NULL;
				parts[2] = NULL;
				lens[1] = lens[0];
			}
			break;
		case 2:
			/*if (!yaf_application_is_module_name_str(parts[0], lens[0]) || UNEXPECTED(yaf_is_action_prefer())) { */
				/* /module/controller/ -> /controller/action */
				parts[2] = parts[1];
				parts[1] = parts[0];
				parts[0] = NULL;
				lens[2] = lens[1];
				lens[1] = lens[0];
			/*}*/
			break;
		case 3:
			if (!yaf_application_is_module_name_str(parts[0], lens[0])) {
				/* /module/controller/ -> /controller/action */
				/* action -> call args */
				req_uri = parts[2];
				req_uri_len += lens[2] + (req_uri_len? 1 : 0) /* stripped back slash */;
				parts[2] = parts[1];
				parts[1] = parts[0];
				parts[0] = NULL;
				lens[2] = lens[1];
				lens[1] = lens[0];
			}
			break;
		default:
			break;
	}

	if (parts[0]) {
		if (UNEXPECTED(request->module)) {
			zend_string_release(request->module);
		}
		request->module = yaf_build_camel_name(parts[0], lens[0]);
	}

	if (parts[1]) {
		if (UNEXPECTED(request->controller)) {
			zend_string_release(request->controller);
		}
		request->controller = yaf_build_camel_name(parts[1], lens[1]);
	}

	if (parts[2]) {
		if (UNEXPECTED(request->action)) {
			zend_string_release(request->action);
		}
		request->action = yaf_build_lower_name(parts[2], lens[2]);
	}

	if (req_uri_len) {
		/* call args */
		zval params;
		yaf_router_parse_parameters(req_uri, req_uri_len, &params);
		yaf_request_set_params_multi(request, &params);
		zval_ptr_dtor(&params);
	}
}
/* }}} */

ZEND_HOT void yaf_route_static_route(yaf_route_t *route, yaf_request_t *req) /* {{{ */ {
	const char *req_uri;
	size_t req_uri_len;
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(req);

	if (request->base_uri) {
		req_uri = yaf_request_strip_base_uri(request->uri, request->base_uri, &req_uri_len);
	} else {
		req_uri = ZSTR_VAL(request->uri);
		req_uri_len = ZSTR_LEN(request->uri);
	}
	yaf_route_pathinfo_route(request, req_uri, req_uri_len);
}
/* }}} */

/** {{{ proto public Yaf_Router_Static::route(Yaf_Request $req)
*/
PHP_METHOD(yaf_route_static, route) {
	yaf_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &request, yaf_request_ce) == FAILURE) {
		return;
	}

	yaf_route_static_route(getThis(), request);
	RETURN_TRUE;
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
	yaf_route_static_ce->ce_flags |= ZEND_ACC_FINAL;

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

