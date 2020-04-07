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

#ifndef YAF_REQUEST_H
#define YAF_REQUEST_H

#define YAF_REQUEST_SERVER_URI               "request_uri="

#define YAF_GLOBAL_VARS_TYPE                 unsigned int
#define YAF_GLOBAL_VARS_POST                 TRACK_VARS_POST
#define YAF_GLOBAL_VARS_GET                  TRACK_VARS_GET
#define YAF_GLOBAL_VARS_ENV                  TRACK_VARS_ENV
#define YAF_GLOBAL_VARS_FILES                TRACK_VARS_FILES
#define YAF_GLOBAL_VARS_SERVER               TRACK_VARS_SERVER
#define YAF_GLOBAL_VARS_REQUEST              TRACK_VARS_REQUEST
#define YAF_GLOBAL_VARS_COOKIE               TRACK_VARS_COOKIE

extern zend_class_entry *yaf_request_ce;

typedef struct {
	zend_bool   routed;
	zend_bool   dispatched;
	zend_string *method;
	zend_array  params;
	zend_string *language;
	zend_string *base_uri;
	zend_string *uri;
	zend_string *module;
	zend_string *controller;
	zend_string *action;
	zend_object std;
} yaf_request_object;

#define Z_YAFREQUESTOBJ(zv)    (php_yaf_request_fetch_object(Z_OBJ(zv)))
#define Z_YAFREQUESTOBJ_P(zv)  Z_YAFREQUESTOBJ(*zv)

static inline yaf_request_object *php_yaf_request_fetch_object(zend_object *obj) {
	return (yaf_request_object *)((char*)(obj) - XtOffsetOf(yaf_request_object, std));
}

void yaf_request_instance(yaf_request_t *this_ptr, zend_string *info);
int yaf_request_set_base_uri(yaf_request_object *request, zend_string *base_uri, zend_string *request_uri);

zval *yaf_request_query(unsigned type, zend_string *name);
zval *yaf_request_query_str(unsigned type, const char *name, size_t len);

zval *yaf_request_get_param(yaf_request_object *request, zend_string *key);
void yaf_request_clean_params(yaf_request_object *request);
zend_string *yaf_request_get_language(yaf_request_object *request);

void yaf_request_set_mvc(yaf_request_object *request, zend_string *module, zend_string *controller, zend_string *action, zend_array *params);
int yaf_request_set_params_single(yaf_request_object *instance, zend_string *key, zval *value);
int yaf_request_set_params_multi(yaf_request_object *instance, zval *values);
const char *yaf_request_strip_base_uri(zend_string *uri, zend_string *base_uri, size_t *len);
const char *yaf_request_get_request_method(void);

static inline int yaf_request_is_routed(yaf_request_object *request) {
	return request->routed;
}

static inline int yaf_request_is_dispatched(yaf_request_object *request) {
	return request->dispatched;
}

static inline void yaf_request_set_routed(yaf_request_object *request, int flag) {
	request->routed = flag? 1 : 0;
}

static inline void yaf_request_set_dispatched(yaf_request_object *request, int flag) {
	request->dispatched = flag? 1 : 0;
}

#define YAF_REQUEST_IS_METHOD(x) \
PHP_METHOD(yaf_request, is##x) {\
	zend_string *method = Z_YAFREQUESTOBJ_P(getThis())->method; \
	if (zend_parse_parameters_none() == FAILURE) { \
		return; \
	} \
	if (zend_string_equals_literal_ci(method, #x)) { \
		RETURN_TRUE; \
	} \
	RETURN_FALSE; \
}

#define YAF_REQUEST_METHOD(ce, x, type) \
PHP_METHOD(ce, get##x) { \
	zend_string *name; \
	zval *ret; \
	zval *def = NULL; \
	if (ZEND_NUM_ARGS() == 0) { \
		ret = yaf_request_query(type, NULL); \
	}else if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &name, &def) == FAILURE) { \
		return; \
	} else { \
		ret = yaf_request_query(type, name); \
		if (!ret) { \
			if (def != NULL) { \
				RETURN_ZVAL(def, 1, 0); \
			} \
		} \
	} \
	if (ret) { \
	    RETURN_ZVAL(ret, 1, 0); \
	} else { \
		RETURN_NULL(); \
	} \
}

YAF_STARTUP_FUNCTION(request);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
