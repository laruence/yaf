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

/* $Id: map.c 329197 2013-01-18 05:55:37Z laruence $*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/php_smart_str.h" /* for smart_str */

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_request.h"

#include "yaf_router.h"
#include "routes/yaf_route_interface.h"
#include "routes/yaf_route_map.h"

zend_class_entry *yaf_route_map_ce;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_route_map_construct_arginfo, 0, 0, 0)
    ZEND_ARG_INFO(0, controller_prefer)
	ZEND_ARG_INFO(0, delimiter)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ yaf_route_t * yaf_route_map_instance(yaf_route_t *this_ptr, zend_bool controller_prefer, char *delim, uint len TSRMLS_DC)
 */
yaf_route_t * yaf_route_map_instance(yaf_route_t *this_ptr, zend_bool controller_prefer, char *delim, uint len TSRMLS_DC) {
	yaf_route_t *instance;

	if (this_ptr) {
		instance  = this_ptr;
	} else {
		MAKE_STD_ZVAL(instance);
		object_init_ex(instance, yaf_route_map_ce);
	}

	if (controller_prefer) {
		zend_update_property_bool(yaf_route_map_ce, instance,
				ZEND_STRL(YAF_ROUTE_MAP_VAR_NAME_CTL_PREFER), 1 TSRMLS_CC);
	}

	if (delim && len) {
		zend_update_property_stringl(yaf_route_map_ce, instance,
				ZEND_STRL(YAF_ROUTE_MAP_VAR_NAME_DELIMETER), delim, len TSRMLS_CC);
	}

	return instance;
}
/* }}} */

/** {{{ int yaf_route_map_route(yaf_route_t *route, yaf_request_t *request TSRMLS_DC)
*/
int yaf_route_map_route(yaf_route_t *route, yaf_request_t *request TSRMLS_DC) {
	zval *ctl_prefer, *delimer, *zuri, *base_uri, *params;
	char *req_uri, *tmp, *rest, *ptrptr, *seg;
	char *query_str = NULL;
	uint seg_len = 0;

	smart_str route_result = {0};

	zuri 	 = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_URI), 1 TSRMLS_CC);
	base_uri = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_BASE), 1 TSRMLS_CC);

	ctl_prefer = zend_read_property(yaf_route_map_ce, route, ZEND_STRL(YAF_ROUTE_MAP_VAR_NAME_CTL_PREFER), 1 TSRMLS_CC);
	delimer	   = zend_read_property(yaf_route_map_ce, route, ZEND_STRL(YAF_ROUTE_MAP_VAR_NAME_DELIMETER), 1 TSRMLS_CC);

	if (base_uri && IS_STRING == Z_TYPE_P(base_uri)
			&& !strncasecmp(Z_STRVAL_P(zuri), Z_STRVAL_P(base_uri), Z_STRLEN_P(base_uri))) {
		req_uri  = estrdup(Z_STRVAL_P(zuri) + Z_STRLEN_P(base_uri));
	} else {
		req_uri  = estrdup(Z_STRVAL_P(zuri));
	}

	if (Z_TYPE_P(delimer) == IS_STRING
			&& Z_STRLEN_P(delimer)) {
		if ((query_str = strstr(req_uri, Z_STRVAL_P(delimer))) != NULL
			&& *(query_str - 1) == '/') {
			tmp  = req_uri;
			rest = query_str + Z_STRLEN_P(delimer);
			if (*rest == '\0') {
				req_uri 	= estrndup(req_uri, query_str - req_uri);
				query_str 	= NULL;
				efree(tmp);
			} else if (*rest == '/') {
				req_uri 	= estrndup(req_uri, query_str - req_uri);
				query_str   = estrdup(rest);
				efree(tmp);
			} else {
				query_str = NULL;
			}
		}
	}

	seg = php_strtok_r(req_uri, YAF_ROUTER_URL_DELIMIETER, &ptrptr);
	while (seg) {
		seg_len = strlen(seg);
		if (seg_len) {
			smart_str_appendl(&route_result, seg, seg_len);
		}
		smart_str_appendc(&route_result, '_');
		seg = php_strtok_r(NULL, YAF_ROUTER_URL_DELIMIETER, &ptrptr);
	}

	if (route_result.len) {
		if (Z_BVAL_P(ctl_prefer)) {
			zend_update_property_stringl(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), route_result.c, route_result.len - 1 TSRMLS_CC);
		} else {
			zend_update_property_stringl(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), route_result.c, route_result.len - 1 TSRMLS_CC);
		}
		efree(route_result.c);
	}

	if (query_str) {
		params = yaf_router_parse_parameters(query_str TSRMLS_CC);
		(void)yaf_request_set_params_multi(request, params TSRMLS_CC);
		zval_ptr_dtor(&params);
		efree(query_str);
	}

	efree(req_uri);

	return 1;
}
/* }}} */

/** {{{ proto public Yaf_Route_Simple::route(Yaf_Request $req)
*/
PHP_METHOD(yaf_route_map, route) {
	yaf_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &request, yaf_request_ce) == FAILURE) {
		return;
	} else {
		RETURN_BOOL(yaf_route_map_route(getThis(), request TSRMLS_CC));
	}
}
/* }}} */

/** {{{ zval * yaf_route_map_assemble(zval *info, zval *query TSRMLS_DC)
 */
zval * yaf_route_map_assemble(yaf_route_t *this_ptr, zval *info, zval *query TSRMLS_DC) {
	char *tmp, *ptrptr, *pname;
	smart_str tvalue = {0};
	uint tmp_len, has_delim = 0;
	zval *uri, *delim, *ctl_prefer, **tmp_data;

	MAKE_STD_ZVAL(uri);
	
	ctl_prefer = zend_read_property(yaf_route_map_ce, this_ptr, ZEND_STRL(YAF_ROUTE_MAP_VAR_NAME_CTL_PREFER), 1 TSRMLS_CC);
	delim = zend_read_property(yaf_route_map_ce, this_ptr, ZEND_STRL(YAF_ROUTE_MAP_VAR_NAME_DELIMETER), 1 TSRMLS_CC);
	if (IS_STRING == Z_TYPE_P(delim) && Z_STRLEN_P(delim)) {
		has_delim = 1;
	}

	do {
		if (Z_BVAL_P(ctl_prefer)) {
			if (zend_hash_find(Z_ARRVAL_P(info), ZEND_STRS(YAF_ROUTE_ASSEMBLE_ACTION_FORMAT), (void **)&tmp_data) == SUCCESS) {
				pname = estrndup(Z_STRVAL_PP(tmp_data), Z_STRLEN_PP(tmp_data));
			} else {
				yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "%s", "Undefined the 'action' parameter for the 1st parameter");
				break;
			}
		} else {
			if (zend_hash_find(Z_ARRVAL_P(info), ZEND_STRS(YAF_ROUTE_ASSEMBLE_CONTROLLER_FORMAT), (void **)&tmp_data) == SUCCESS) {
				pname = estrndup(Z_STRVAL_PP(tmp_data), Z_STRLEN_PP(tmp_data));
			} else {
				yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "%s", "Undefined the 'controller' parameter for the 1st parameter");
				break;
			}
		}

		tmp = php_strtok_r(pname, "_", &ptrptr);	
		while(tmp) {
			tmp_len = strlen(tmp);
			if (tmp_len) {
				smart_str_appendc(&tvalue, '/');
				smart_str_appendl(&tvalue, tmp, tmp_len);
			}
			tmp = php_strtok_r(NULL, "_", &ptrptr);
		}
		efree(pname);

		if (IS_ARRAY == Z_TYPE_P(query)) {
			uint key_len, i = 0;
			char *key;
			ulong key_idx;
			zval **tmp_data;

			if (has_delim) {
				smart_str_appendc(&tvalue, '/');
				smart_str_appendl(&tvalue, Z_STRVAL_P(delim), Z_STRLEN_P(delim));
			}

			for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(query));
					zend_hash_get_current_data(Z_ARRVAL_P(query), (void **)&tmp_data) == SUCCESS;
					zend_hash_move_forward(Z_ARRVAL_P(query))) {

				if (IS_STRING == Z_TYPE_PP(tmp_data)
						&& HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(Z_ARRVAL_P(query), &key, &key_len, &key_idx, 0, NULL)) {

					if (has_delim) {
						smart_str_appendc(&tvalue, '/');
						smart_str_appendl(&tvalue, key, key_len - 1);
						smart_str_appendc(&tvalue, '/');
						smart_str_appendl(&tvalue, Z_STRVAL_PP(tmp_data), Z_STRLEN_PP(tmp_data));
					} else {
						if (i == 0) {
							smart_str_appendc(&tvalue, '?');
							smart_str_appendl(&tvalue, key, key_len - 1);
							smart_str_appendc(&tvalue, '=');
							smart_str_appendl(&tvalue, Z_STRVAL_PP(tmp_data), Z_STRLEN_PP(tmp_data));
						} else {
							smart_str_appendc(&tvalue, '&');
							smart_str_appendl(&tvalue, key, key_len - 1);
							smart_str_appendc(&tvalue, '=');
							smart_str_appendl(&tvalue, Z_STRVAL_PP(tmp_data), Z_STRLEN_PP(tmp_data));
						}
					}
				}
				i += 1;
			}
		}

		smart_str_0(&tvalue);
		ZVAL_STRING(uri, tvalue.c, 1);
		smart_str_free(&tvalue);
		return uri;
	} while (0);

	ZVAL_NULL(uri);
	return uri;
}

/** {{{ proto public Yaf_Route_Simple::__construct(bool $controller_prefer=FALSE, string $delimer = '#!')
*/
PHP_METHOD(yaf_route_map, __construct) {
	char *delim	= NULL;
	uint delim_len = 0;
	zend_bool controller_prefer = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|bs",
			   	&controller_prefer, &delim, &delim_len) == FAILURE) {
		YAF_UNINITIALIZED_OBJECT(getThis());
		return;
	}

	(void)yaf_route_map_instance(getThis(), controller_prefer, delim, delim_len TSRMLS_CC);
}
/* }}} */

/** {{{ proto public Yaf_Route_Map::assemble(array $info[, array $query = NULL])
*/
PHP_METHOD(yaf_route_map, assemble) {
	zval *info, *query;
	zval *return_uri;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|a", &info, &query) == FAILURE) {
        return;
    } else {
        if ((return_uri = yaf_route_map_assemble(getThis(), info, query TSRMLS_CC))) {
            RETURN_ZVAL(return_uri, 0, 1);
        }
    }
}
/* }}} */

/** {{{ yaf_route_map_methods
*/
zend_function_entry yaf_route_map_methods[] = {
	PHP_ME(yaf_route_map, __construct, yaf_route_map_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(yaf_route_map, route, yaf_route_route_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_route_map, assemble, yaf_route_assemble_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(route_map) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Route_Map", "Yaf\\Route\\Map", yaf_route_map_methods);
	yaf_route_map_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
	zend_class_implements(yaf_route_map_ce TSRMLS_CC, 1, yaf_route_ce);

	yaf_route_map_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;

	zend_declare_property_bool(yaf_route_map_ce, ZEND_STRL(YAF_ROUTE_MAP_VAR_NAME_CTL_PREFER), 0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_route_map_ce, ZEND_STRL(YAF_ROUTE_MAP_VAR_NAME_DELIMETER),  ZEND_ACC_PROTECTED TSRMLS_CC);

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
