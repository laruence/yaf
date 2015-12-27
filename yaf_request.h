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

#define YAF_REQUEST_PROPERTY_NAME_MODULE     "module"
#define YAF_REQUEST_PROPERTY_NAME_CONTROLLER "controller"
#define YAF_REQUEST_PROPERTY_NAME_ACTION     "action"
#define YAF_REQUEST_PROPERTY_NAME_METHOD     "method"
#define YAF_REQUEST_PROPERTY_NAME_PARAMS     "params"
#define YAF_REQUEST_PROPERTY_NAME_URI        "uri"
#define YAF_REQUEST_PROPERTY_NAME_STATE      "dispatched"
#define YAF_REQUEST_PROPERTY_NAME_LANG       "language"
#define YAF_REQUEST_PROPERTY_NAME_ROUTED     "routed"
#define YAF_REQUEST_PROPERTY_NAME_BASE       "_base_uri"
#define YAF_REQUEST_PROPERTY_NAME_EXCEPTION  "_exception"

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

extern PHPAPI void php_session_start();

yaf_request_t *yaf_request_instance(yaf_request_t *this_ptr, zend_string *info);
int yaf_request_set_base_uri(yaf_request_t *request, zend_string *base_uri, zend_string *request_uri);
zval *yaf_request_query_ex(uint type, zend_bool fetch_type, void *name, size_t len);

zval *yaf_request_get_method(yaf_request_t *instance);
zval *yaf_request_get_param(yaf_request_t *instance, zend_string *key);
zval *yaf_request_get_language(yaf_request_t *instance, zval *accept_language);

int yaf_request_is_routed(yaf_request_t *request);
int yaf_request_is_dispatched(yaf_request_t *request);
void yaf_request_set_dispatched(yaf_request_t *request, int flag);
void yaf_request_set_routed(yaf_request_t *request, int flag);
int yaf_request_set_params_single(yaf_request_t *instance, zend_string *key, zval *value);
int yaf_request_set_params_multi(yaf_request_t *instance, zval *values);

#define yaf_request_query(type, name)  yaf_request_query_ex((type), 1, (name), 0)
#define yaf_request_query_str(type, name, len)  yaf_request_query_ex((type), 0, (name), (len))

#define YAF_REQUEST_IS_METHOD(x) \
PHP_METHOD(yaf_request, is##x) {\
	zval *method = zend_read_property(Z_OBJCE_P(getThis()), \
			getThis(), ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_METHOD), 0, NULL); \
	if (zend_string_equals_literal_ci(Z_STR_P(method), #x)) { \
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
