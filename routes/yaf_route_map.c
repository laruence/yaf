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
#include "routes/yaf_route_map.h"

zend_class_entry *yaf_route_map_ce;
static zend_object_handlers yaf_route_map_obj_handlers;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_route_map_construct_arginfo, 0, 0, 0)
    ZEND_ARG_INFO(0, controller_prefer)
	ZEND_ARG_INFO(0, delimiter)
ZEND_END_ARG_INFO()
/* }}} */

static HashTable *yaf_route_map_get_properties(zval *object) /* {{{ */ {
	zval rv;
	HashTable *ht;
	yaf_route_map_object *map = Z_YAFROUTEMAPOBJ_P(object);

	if (!map->properties) {
		ALLOC_HASHTABLE(map->properties);
		zend_hash_init(map->properties, 2, NULL, ZVAL_PTR_DTOR, 0);
		YAF_ALLOW_VIOLATION(map->properties);

		ht = map->properties;

		ZVAL_BOOL(&rv, map->flags & YAF_ROUTE_MAP_CTL_PREFER);
		zend_hash_str_add(ht, "ctl_prefer:protected", sizeof("ctl_prefer:protected") - 1, &rv);

		if (map->delim) {
			ZVAL_STR_COPY(&rv, map->delim);
		} else {
			ZVAL_NULL(&rv);
		}
		zend_hash_str_add(ht, "delimiter:protected", sizeof("delimiter:protected") - 1, &rv);
	}

	return map->properties;
}
/* }}} */

static zend_object *yaf_route_map_new(zend_class_entry *ce) /* {{{ */ {
	yaf_route_map_object *map = emalloc(sizeof(yaf_route_map_object));

	zend_object_std_init(&map->std, ce);

	map->std.handlers = &yaf_route_map_obj_handlers;
	map->delim = NULL;
	map->properties = NULL;

	return &map->std;
}
/* }}} */

static void yaf_route_map_object_free(zend_object *object) /* {{{ */ {
	yaf_route_map_object *map = (yaf_route_map_object*)object;

	if (map->delim) {
		zend_string_release(map->delim);
	}

	if (map->properties) {
		if (GC_DELREF(map->properties) == 0) {
			GC_REMOVE_FROM_BUFFER(map->properties);
			zend_array_destroy(map->properties);
		}
	}

	zend_object_std_dtor(&map->std);
}
/* }}} */

void yaf_route_map_init(yaf_route_map_object *map, zend_bool ctl_prefer, zend_string *delim) /* {{{ */{
	map->flags = ctl_prefer? YAF_ROUTE_MAP_CTL_PREFER : 0;
	if (delim && ZSTR_LEN(delim)) {
		map->delim = zend_string_copy(delim);
	} else {
		map->delim = NULL;
	}
}
/* }}} */

void yaf_route_map_instance(yaf_route_t *route, zend_bool ctl_prefer, zend_string *delim) /* {{{ */{
	zend_object *map = yaf_route_map_new(yaf_route_map_ce);

	yaf_route_map_init((yaf_route_map_object*)map, ctl_prefer, delim);

	ZVAL_OBJ(route, map);
}
/* }}} */

static inline void yaf_route_map_append(smart_str *str, const char *seg, uint32_t len) /* {{{ */ {
	uint32_t i;

	smart_str_appendc(str, toupper(*seg));
	for (i = 1; i < len; i++) {
		smart_str_appendc(str, tolower(seg[i]));
	}
}
/* }}} */

int yaf_route_map_route(yaf_route_t *route, yaf_request_t *req) /* {{{ */ {
	const char *req_uri, *query_str, *pos;
	size_t req_uri_len, query_str_len;
	smart_str route_result = {0};
	yaf_request_object *request = Z_YAFREQUESTOBJ_P(req);
	yaf_route_map_object *map = Z_YAFROUTEMAPOBJ_P(route);

	if (request->base_uri) {
		req_uri = yaf_request_strip_base_uri(request->uri, request->base_uri, &req_uri_len);
	} else {
		req_uri = ZSTR_VAL(request->uri);
		req_uri_len = ZSTR_LEN(request->uri);
	}

	if (UNEXPECTED(map->delim)) {
		if ((query_str = strstr(req_uri, ZSTR_VAL(map->delim))) && *(query_str - 1) == YAF_ROUTER_URL_DELIMIETER) {
			const char *rest = query_str + ZSTR_LEN(map->delim);

			while (*rest == YAF_ROUTER_URL_DELIMIETER) {
				rest++;
			}
			if (*rest != '\0') {
				zval params;
				query_str_len = req_uri_len - (rest - req_uri);
				req_uri_len = query_str - req_uri;
				query_str = rest;
				yaf_router_parse_parameters(query_str, query_str_len, &params);
				yaf_request_set_params_multi(request, &params);
				zval_ptr_dtor(&params);
			} else {
				req_uri_len = query_str - req_uri;
			}
		}
	}

	while ((pos = memchr(req_uri, YAF_ROUTER_URL_DELIMIETER, req_uri_len))) {
		size_t seg_len = pos++ - req_uri;
		if (seg_len) {
			yaf_route_map_append(&route_result, req_uri, seg_len);
			smart_str_appendc(&route_result, '_');
		}
		req_uri_len -= pos - req_uri;
		req_uri = pos;
	}

	if (req_uri_len) {
		yaf_route_map_append(&route_result, req_uri, req_uri_len);
		smart_str_appendc(&route_result, '_');
	}

	if (route_result.s) {
		ZSTR_LEN(route_result.s)--;
		ZSTR_VAL(route_result.s)[ZSTR_LEN(route_result.s)] = '\0';
		if (map->flags & YAF_ROUTE_MAP_CTL_PREFER) {
			/* avoding double realloc */
			if (UNEXPECTED(request->controller)) {
				zend_string_release(request->controller);
			}
			request->controller = route_result.s;
		} else {
			yaf_request_set_action(request, route_result.s);
			smart_str_free(&route_result);
		}
	}

	return 1;
}
/* }}} */

/** {{{ proto public Yaf_Route_Simple::route(Yaf_Request $req)
*/
PHP_METHOD(yaf_route_map, route) {
	yaf_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &request, yaf_request_ce) == FAILURE) {
		return;
	} else {
		RETURN_BOOL(yaf_route_map_route(getThis(), request));
	}
}
/* }}} */

zend_string * yaf_route_map_assemble(yaf_route_t *route, zval *info, zval *query) /* {{{ */ {
	zval *zv;
	char *seg, *pname;
	size_t seg_len;
	char *ptrptr = NULL;
	smart_str uri = {0};
	yaf_route_map_object *map = Z_YAFROUTEMAPOBJ_P(route);

	if (map->flags & YAF_ROUTE_MAP_CTL_PREFER) {
		if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_ACTION_FORMAT))) && Z_TYPE_P(zv) == IS_STRING) {
			pname = estrndup(Z_STRVAL_P(zv), Z_STRLEN_P(zv));
		} else {
			yaf_trigger_error(YAF_ERR_TYPE_ERROR, "%s",
					"Undefined the 'action' parameter for the 1st parameter");
			return NULL;
		}
	} else {
		if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(YAF_ROUTE_ASSEMBLE_CONTROLLER_FORMAT))) && Z_TYPE_P(zv) == IS_STRING) {
			pname = estrndup(Z_STRVAL_P(zv), Z_STRLEN_P(zv));
		} else {
			yaf_trigger_error(YAF_ERR_TYPE_ERROR, "%s",
					"Undefined the 'controller' parameter for the 1st parameter");
			return NULL;
		}
	}

	seg = php_strtok_r(pname, "_", &ptrptr);	
	while (seg) {
		seg_len = strlen(seg);
		if (seg_len) {
			smart_str_appendc(&uri, '/');
			smart_str_appendl(&uri, seg, seg_len);
		}
		seg = php_strtok_r(NULL, "_", &ptrptr);
	}
	efree(pname);

	if (query && IS_ARRAY == Z_TYPE_P(query)) {
		zend_string *key, *val;
		if (zend_hash_num_elements(Z_ARRVAL_P(query))) {
			if (map->delim) {
				smart_str_appendc(&uri, '/');
				smart_str_appendl(&uri, ZSTR_VAL(map->delim), ZSTR_LEN(map->delim));
				smart_str_appendc(&uri, '/');
			} else {
				smart_str_appendc(&uri, '?');
			}

			ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(query), key, zv) {
				if (key) {
					val = zval_get_string(zv);
					if (map->delim) {
						smart_str_appendl(&uri, ZSTR_VAL(key), ZSTR_LEN(key));
						smart_str_appendc(&uri, '/');
						smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
						smart_str_appendc(&uri, '/');
					} else {
						smart_str_appendl(&uri, ZSTR_VAL(key), ZSTR_LEN(key));
						smart_str_appendc(&uri, '=');
						smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
						smart_str_appendc(&uri, '&');
					}
					zend_string_release(val);
				}
			} ZEND_HASH_FOREACH_END();
			ZSTR_LEN(uri.s)--;
		}
	}
	smart_str_0(&uri);

	return uri.s;
}
/* }}} */

/** {{{ proto public Yaf_Route_Simple::__construct(bool $controller_prefer=FALSE, string $delimer = '#!')
*/
PHP_METHOD(yaf_route_map, __construct) {
	zend_string *delim	= NULL;
	zend_bool ctl_prefer = 0;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|bS!", &ctl_prefer, &delim) == FAILURE) {
		return;
	}

	yaf_route_map_init(Z_YAFROUTEMAPOBJ_P(getThis()), ctl_prefer, delim);
}
/* }}} */

/** {{{ proto public Yaf_Route_Map::assemble(array $info[, array $query = NULL])
*/
PHP_METHOD(yaf_route_map, assemble) {
	zval *info, *query = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|a", &info, &query) == FAILURE) {
        return;
    } else {
		zend_string *str;
        if ((str = yaf_route_map_assemble(getThis(), info, query)) != NULL) {
			RETURN_STR(str);
		}
		RETURN_NULL();
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
	yaf_route_map_ce = zend_register_internal_class(&ce);
	yaf_route_map_ce->create_object = yaf_route_map_new;
	yaf_route_map_ce->ce_flags |= ZEND_ACC_FINAL;
	yaf_route_map_ce->serialize = zend_class_serialize_deny;
	yaf_route_map_ce->unserialize = zend_class_unserialize_deny;

	zend_class_implements(yaf_route_map_ce, 1, yaf_route_ce);

	memcpy(&yaf_route_map_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_route_map_obj_handlers.free_obj = yaf_route_map_object_free;
	yaf_route_map_obj_handlers.clone_obj = NULL;
	yaf_route_map_obj_handlers.get_gc = NULL;
	yaf_route_map_obj_handlers.get_properties = yaf_route_map_get_properties;


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
