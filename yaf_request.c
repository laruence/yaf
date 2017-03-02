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
#include "standard/php_string.h" /* for php_basename */
#include "Zend/zend_exceptions.h" /* for zend_exception_get_default */

#include "php_yaf.h"
#include "yaf_request.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"

#include "requests/yaf_request_simple.h"
#include "requests/yaf_request_http.h"

zend_class_entry *yaf_request_ce;

/** {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(yaf_request_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_set_routed_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_set_module_name_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, module)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_set_controller_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, controller)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_set_action_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, action)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_set_baseuri_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_set_request_uri_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_set_param_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_get_param_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_getserver_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_request_getenv_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()
	/* }}} */

yaf_request_t *yaf_request_instance(yaf_request_t *this_ptr, zend_string *request_uri) /* {{{ */ {
	return yaf_request_http_instance(this_ptr, NULL, request_uri);
}
/* }}} */

int yaf_request_set_base_uri(yaf_request_t *request, zend_string *base_uri, zend_string *request_uri) /* {{{ */ {
	if (base_uri == NULL) {
		zend_string *basename = NULL;
		zval *script_filename = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, "SCRIPT_FILENAME", sizeof("SCRIPT_FILENAME") - 1);
		do {
			if (script_filename && IS_STRING == Z_TYPE_P(script_filename)) {
				zend_string *file_name;
				char *ext = ZSTR_VAL(YAF_G(ext));
				size_t ext_len = ZSTR_LEN(YAF_G(ext));
				zval *script_name, *phpself_name, *orig_name;

				script_name = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, "SCRIPT_NAME", sizeof("SCRIPT_NAME") - 1);
				file_name = php_basename(Z_STRVAL_P(script_filename), Z_STRLEN_P(script_filename), ext, ext_len);
				if (script_name && EXPECTED(IS_STRING == Z_TYPE_P(script_name))) {
					zend_string	*script = php_basename(Z_STRVAL_P(script_name), Z_STRLEN_P(script_name), NULL, 0);

					if (strncmp(ZSTR_VAL(file_name), ZSTR_VAL(script), ZSTR_LEN(file_name)) == 0) {
						basename = zend_string_copy(Z_STR_P(script_name));
						zend_string_release(file_name);
						zend_string_release(script);
						break;
					}
					zend_string_release(script);
				}

				phpself_name = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, "PHP_SELF", sizeof("PHP_SELF") - 1);
				if (phpself_name && EXPECTED(IS_STRING == Z_TYPE_P(phpself_name))) {
					zend_string *phpself = php_basename(Z_STRVAL_P(phpself_name), Z_STRLEN_P(phpself_name), NULL, 0);
					if (strncmp(ZSTR_VAL(file_name), ZSTR_VAL(phpself), ZSTR_LEN(file_name)) == 0) {
						basename = zend_string_copy(Z_STR_P(phpself_name));
						zend_string_release(file_name);
						zend_string_release(phpself);
						break;
					}
					zend_string_release(phpself);
				}

				orig_name = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, "ORIG_SCRIPT_NAME", sizeof("ORIG_SCRIPT_NAME") - 1);
				if (orig_name && IS_STRING == Z_TYPE_P(orig_name)) {
					zend_string *orig = php_basename(Z_STRVAL_P(orig_name), Z_STRLEN_P(orig_name), NULL, 0);
					if (strncmp(ZSTR_VAL(file_name), ZSTR_VAL(orig), ZSTR_LEN(file_name)) == 0) {
						basename = zend_string_copy(Z_STR_P(orig_name));
						zend_string_release(file_name);
						zend_string_release(orig);
						break;
					}
					zend_string_release(orig);
				}
				zend_string_release(file_name);
			}
		} while (0);

		if (basename && strncmp(ZSTR_VAL(request_uri), ZSTR_VAL(basename), ZSTR_LEN(basename)) == 0) {
			if (*(ZSTR_VAL(basename) + ZSTR_LEN(basename) - 1) == '/') {
				zend_string *garbage = basename;
				basename = zend_string_init(ZSTR_VAL(basename), ZSTR_LEN(basename) - 1, 0);
				zend_string_release(garbage);
			}
			zend_update_property_str(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_BASE), basename);
			zend_string_release(basename);
			return 1;
		} else if (basename) {
			zend_string *dir = zend_string_init(ZSTR_VAL(basename), ZSTR_LEN(basename), 0); /* php_dirname might alter the string */

			ZSTR_LEN(dir) = php_dirname(ZSTR_VAL(dir), ZSTR_LEN(dir));
			if (*(ZSTR_VAL(dir) + ZSTR_LEN(dir) - 1) == '/') {
				--ZSTR_LEN(dir);
			}

			if (ZSTR_LEN(dir)) {
				if (strncmp(ZSTR_VAL(request_uri), ZSTR_VAL(dir), ZSTR_LEN(dir)) == 0) {
					zend_update_property_str(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_BASE), dir);
					zend_string_release(dir);
					zend_string_release(basename);
					return 1;
				}
			}
			zend_string_release(dir);
			zend_string_release(basename);
		}

		zend_update_property_string(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_BASE), "");
	} else {
		zend_update_property_str(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_BASE), base_uri);
	}
	return 1;
}
/* }}} */

zval *yaf_request_query_ex(uint type, zend_bool fetch_type, void *name, size_t len) /* {{{ */ {
	zval *carrier = NULL, *ret;

	zend_bool jit_initialization = PG(auto_globals_jit);

	/* for phpunit test requirements */
#if PHP_YAF_DEBUG
	switch (type) {
		case YAF_GLOBAL_VARS_POST:
			carrier = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_POST"));
			break;
		case YAF_GLOBAL_VARS_GET:
			carrier = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_GET"));
			break;
		case YAF_GLOBAL_VARS_COOKIE:
			carrier = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_COOKIE"));
			break;
		case YAF_GLOBAL_VARS_SERVER:
			if (jit_initialization) {
				zend_string *server_str = zend_string_init("_SERVER", sizeof("_SERVER") - 1, 0);
				zend_is_auto_global(server_str);
				zend_string_release(server_str);
			}
			carrier = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_SERVER"));
			break;
		case YAF_GLOBAL_VARS_ENV:
			if (jit_initialization) {
				zend_string *env_str = zend_string_init("_ENV", sizeof("_ENV") - 1, 0);
				zend_is_auto_global(env_str);
				zend_string_release(env_str);
			}
			carrier = &PG(http_globals)[YAF_GLOBAL_VARS_ENV];
			break;
		case YAF_GLOBAL_VARS_FILES:
			carrier = &PG(http_globals)[YAF_GLOBAL_VARS_FILES];
			break;
		case YAF_GLOBAL_VARS_REQUEST:
			if (jit_initialization) {
				zend_string *request_str = zend_string_init("_REQUEST", sizeof("_REQUEST") - 1, 0);
				zend_is_auto_global(request_str);
				zend_string_release(request_str);
			}
			carrier = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_REQUEST"));
			break;
		default:
			break;
	}
#else
	switch (type) {
		case YAF_GLOBAL_VARS_POST:
		case YAF_GLOBAL_VARS_GET:
		case YAF_GLOBAL_VARS_FILES:
		case YAF_GLOBAL_VARS_COOKIE:
			carrier = &PG(http_globals)[type];
			break;
		case YAF_GLOBAL_VARS_ENV:
			if (jit_initialization) {
				zend_string *env_str = zend_string_init("_ENV", sizeof("_ENV") - 1, 0);
				zend_is_auto_global(env_str);
				zend_string_release(env_str);
			}
			carrier = &PG(http_globals)[type];
			break;
		case YAF_GLOBAL_VARS_SERVER:
			if (jit_initialization) {
				zend_string *server_str = zend_string_init("_SERVER", sizeof("_SERVER") - 1, 0);
				zend_is_auto_global(server_str);
				zend_string_release(server_str);
			}
			carrier = &PG(http_globals)[type];
			break;
		case YAF_GLOBAL_VARS_REQUEST:
			if (jit_initialization) {
				zend_string *request_str = zend_string_init("_REQUEST", sizeof("_REQUEST") - 1, 0);
				zend_is_auto_global(request_str);
				zend_string_release(request_str);
			}
			carrier = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_REQUEST"));
			break;
		default:
			break;
	}
#endif

	if (!carrier) {
		return NULL;
	}

	if (!name) {
		return carrier;
	}

	if (EXPECTED(fetch_type)) {
		if ((ret = zend_hash_find(Z_ARRVAL_P(carrier), (zend_string *)name)) == NULL) {
			return NULL;
		}
	} else {
		if ((ret = zend_hash_str_find(Z_ARRVAL_P(carrier), (char *)name, len)) == NULL) {
			return NULL;
		}
	}
	return ret;
}
/* }}} */

yaf_request_t * yaf_request_get_method(yaf_request_t *request) /* {{{ */ {
	return zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_METHOD), 1, NULL);
}
/* }}} */

zval *yaf_request_get_language(yaf_request_t *instance, zval *accept_language) /* {{{ */ {
	zval *lang = zend_read_property(yaf_request_ce,
			instance, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_LANG), 1, NULL);

	if (lang && UNEXPECTED(IS_STRING != Z_TYPE_P(lang))) {
		zval *accept_langs = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER,
				"HTTP_ACCEPT_LANGUAGE", sizeof("HTTP_ACCEPT_LANGUAGE") - 1);
		if (!accept_langs) {
			return NULL;
		} else if (UNEXPECTED(IS_STRING != Z_TYPE_P(accept_langs) || !Z_STRLEN_P(accept_langs))) {
			return NULL;
		} else {
			char  	*ptrptr, *seg;
			uint	prefer_len = 0;
			double	max_qvlaue = 0;
			char 	*prefer = NULL;
			char  	*langs = estrndup(Z_STRVAL_P(accept_langs), Z_STRLEN_P(accept_langs));

			seg = php_strtok_r(langs, ",", &ptrptr);
			while(seg) {
				char *qvalue;
				while (*(seg) == ' ') { 
					seg++;
				}
				/* Accept-Language: da, en-gb;q=0.8, en;q=0.7 */
				if ((qvalue = strstr(seg, "q="))) {
					float qval = strtod(qvalue + 2, NULL);
					if (qval > max_qvlaue) {
						max_qvlaue = qval;
						if (prefer) {
							efree(prefer);
						}
						prefer_len = qvalue - seg - 1;
						prefer 	   = estrndup(seg, prefer_len);
					}
				} else {
					if (max_qvlaue < 1) {
						max_qvlaue = 1;
						prefer_len = strlen(seg);
						prefer 	   = estrndup(seg, prefer_len);
					}
				}

				seg = php_strtok_r(NULL, ",", &ptrptr);
			}

			if (prefer) {
				ZVAL_STRINGL(accept_language, prefer, prefer_len);
				zend_update_property(yaf_request_ce,
						instance, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_LANG), accept_language);
				efree(prefer);
				efree(langs);
				zval_ptr_dtor(accept_language);
				return accept_language;
			}
			efree(langs);
			zval_ptr_dtor(accept_language);
		}
	}
	return lang;
}
/* }}} */

int yaf_request_is_routed(yaf_request_t *request) /* {{{ */{
	zval *routed = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ROUTED), 1, NULL);
	return Z_TYPE_P(routed) == IS_TRUE ? 1 : 0;
}
/* }}} */

int yaf_request_is_dispatched(yaf_request_t *request) /* {{{ */ {
	zval *dispatched = zend_read_property(yaf_request_ce,
			request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_STATE), 1, NULL);
	return Z_TYPE_P(dispatched) == IS_TRUE ? 1 : 0;
}
/* }}} */

void yaf_request_set_dispatched(yaf_request_t *instance, int flag) /* {{{ */ {
	zend_update_property_bool(yaf_request_ce, instance, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_STATE), flag);
}
/* }}} */

void yaf_request_set_routed(yaf_request_t *request, int flag) /* {{{ */ {
	zend_update_property_bool(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ROUTED), flag);
}
/* }}} */

int yaf_request_set_params_single(yaf_request_t *request, zend_string *key, zval *value) /* {{{ */ {
	zval *params = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_PARAMS), 1, NULL);
	if ((zend_hash_update(Z_ARRVAL_P(params), key, value)) != NULL) {
		Z_TRY_ADDREF_P(value);
		return 1;
	}
	return 0;
}
/* }}} */

int yaf_request_set_params_multi(yaf_request_t *request, zval *values) /* {{{ */ {
	zval *params = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_PARAMS), 1, NULL);
	if (values && Z_TYPE_P(values) == IS_ARRAY) {
		zend_hash_copy(Z_ARRVAL_P(params), Z_ARRVAL_P(values), (copy_ctor_func_t) zval_add_ref);
		return 1;
	}
	return 0;
}
/* }}} */

zval * yaf_request_get_param(yaf_request_t *request, zend_string *key) /* {{{ */ {
	zval *params = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_PARAMS), 1, NULL);
	return zend_hash_find(Z_ARRVAL_P(params), key);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isGet(void)
*/
YAF_REQUEST_IS_METHOD(Get);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isPost(void)
*/
YAF_REQUEST_IS_METHOD(Post);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isPut(void)
*/
YAF_REQUEST_IS_METHOD(Put);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isDelete(void)
*/
YAF_REQUEST_IS_METHOD(Delete);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isPatch(void)
*/
YAF_REQUEST_IS_METHOD(Patch);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isHead(void)
*/
YAF_REQUEST_IS_METHOD(Head);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isOptions(void)
*/
YAF_REQUEST_IS_METHOD(Options);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isCli(void)
*/
YAF_REQUEST_IS_METHOD(Cli);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isXmlHttpRequest(void)
*/
PHP_METHOD(yaf_request, isXmlHttpRequest) {
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getEnv(mixed $name, mixed $default = NULL)
*/
YAF_REQUEST_METHOD(yaf_request, Env, 	YAF_GLOBAL_VARS_ENV);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getServer(mixed $name, mixed $default = NULL)
*/
YAF_REQUEST_METHOD(yaf_request, Server, YAF_GLOBAL_VARS_SERVER);
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getModuleName(void)
*/
PHP_METHOD(yaf_request, getModuleName) {
	zval *module = zend_read_property(yaf_request_ce, getThis(), ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), 1, NULL);
	RETVAL_ZVAL(module, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getControllerName(void)
*/
PHP_METHOD(yaf_request, getControllerName) {
	zval *controller = zend_read_property(yaf_request_ce,
		   	getThis(), ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), 1, NULL);
	RETVAL_ZVAL(controller, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getActionName(void)
*/
PHP_METHOD(yaf_request, getActionName) {
	zval *action = zend_read_property(yaf_request_ce,
			getThis(), ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), 1, NULL);
	RETVAL_ZVAL(action, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::setModuleName(string $module)
*/
PHP_METHOD(yaf_request, setModuleName) {
	zval *module;
	yaf_request_t *self	= getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &module) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(module) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "Expect a string module name");
		RETURN_FALSE;
	}

	zend_update_property(yaf_request_ce, self, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), module);

	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::setControllerName(string $controller)
*/
PHP_METHOD(yaf_request, setControllerName) {
	zval *controller;
	yaf_request_t *self	= getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &controller) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(controller) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "Expect a string controller name");
		RETURN_FALSE;
	}

	zend_update_property(yaf_request_ce, getThis(), ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), controller);

	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::setActionName(string $action)
*/
PHP_METHOD(yaf_request, setActionName) {
	zval *action;
	zval *self	 = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &action) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(action) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "Expect a string action name");
		RETURN_FALSE;
	}

	zend_update_property(yaf_request_ce, getThis(), ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), action);

	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::setParam(mixed $value)
*/
PHP_METHOD(yaf_request, setParam) {
	uint argc;
	yaf_request_t *self	= getThis();

	argc = ZEND_NUM_ARGS();

	if (1 == argc) {
		zval *value ;
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &value) == FAILURE) {
			return;
		}
		if (yaf_request_set_params_multi(self, value)) {
			RETURN_ZVAL(self, 1, 0);
		}
	} else if (2 == argc) {
		zval *value;
		zend_string *name;
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
			return;
		}

		if (yaf_request_set_params_single(getThis(), name, value)) {
			RETURN_ZVAL(self, 1, 0);
		}
	} else {
		WRONG_PARAM_COUNT;
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getParam(string $name, $mixed $default = NULL)
*/
PHP_METHOD(yaf_request, getParam) {
	zend_string *name;
	zval *def = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &name, &def) == FAILURE) {
		return;
	} else {
		zval *value = yaf_request_get_param(getThis(), name);
		if (value) {
			RETURN_ZVAL(value, 1, 0);
		}
		if (def) {
			RETURN_ZVAL(def, 1, 0);
		}
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getException(void)
*/
PHP_METHOD(yaf_request, getException) {
	zval *exception = zend_read_property(yaf_request_ce,
			getThis(), ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_EXCEPTION), 1, NULL);
	if (IS_OBJECT == Z_TYPE_P(exception)
			&& instanceof_function(Z_OBJCE_P(exception), zend_exception_get_default())) {
		RETURN_ZVAL(exception, 1, 0);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getParams(void)
*/
PHP_METHOD(yaf_request, getParams) {
	zval *params = zend_read_property(yaf_request_ce,
			getThis(), ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_PARAMS), 1, NULL);
	RETURN_ZVAL(params, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getLanguage(void)
*/
PHP_METHOD(yaf_request, getLanguage) {
	zval accept_language;
	zval *lang = yaf_request_get_language(getThis(), &accept_language);
	if (lang) {
		RETURN_ZVAL(lang, 1, 0);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getMethod(void)
*/
PHP_METHOD(yaf_request, getMethod) {
	zval *method = yaf_request_get_method(getThis());
	RETURN_ZVAL(method, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isDispatched(void)
*/
PHP_METHOD(yaf_request, isDispatched) {
	RETURN_BOOL(yaf_request_is_dispatched(getThis()));
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::setDispatched(void)
*/
PHP_METHOD(yaf_request, setDispatched) {
	yaf_request_set_dispatched(getThis(), 1);
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::setBaseUri(string $name)
*/
PHP_METHOD(yaf_request, setBaseUri) {
	zend_string *uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &uri) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(uri) == 0) {
		RETURN_FALSE;
	}

	if (yaf_request_set_base_uri(getThis(), uri, NULL)) {
		RETURN_ZVAL(getThis(), 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getBaseUri(string $name)
*/
PHP_METHOD(yaf_request, getBaseUri) {
	zval *uri = zend_read_property(yaf_request_ce,
			getThis(), ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_BASE), 1, NULL);
	RETURN_ZVAL(uri, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::getRequestUri(string $name)
*/
PHP_METHOD(yaf_request, getRequestUri) {
	zval *uri = zend_read_property(yaf_request_ce, getThis(), ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_URI), 1, NULL);
	RETURN_ZVAL(uri, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::setRequestUri(string $name)
*/
PHP_METHOD(yaf_request, setRequestUri) {
	zend_string *uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &uri) == FAILURE) {
		return;
	}

	zend_update_property_str(yaf_request_ce, getThis(), ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_URI), uri);
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::isRouted(void)
*/
PHP_METHOD(yaf_request, isRouted) {
	RETURN_BOOL(yaf_request_is_routed(getThis()));
}
/* }}} */

/** {{{ proto public Yaf_Request_Abstract::setRouted(void)
*/
PHP_METHOD(yaf_request, setRouted) {
	yaf_request_set_routed(getThis(), 1);
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ yaf_request_methods
*/
zend_function_entry yaf_request_methods[] = {
	PHP_ME(yaf_request, isGet, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isPost, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isDelete, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isPatch, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isPut, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isHead, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isOptions, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isCli, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isXmlHttpRequest, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getServer, yaf_request_getserver_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getEnv, yaf_request_getenv_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, setParam, yaf_request_set_param_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getParam, yaf_request_get_param_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getParams, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getException, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getModuleName, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getControllerName, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getActionName, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, setModuleName, yaf_request_set_module_name_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, setControllerName, yaf_request_set_controller_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, setActionName, yaf_request_set_action_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getMethod, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getLanguage, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, setBaseUri, yaf_request_set_baseuri_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getBaseUri, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, getRequestUri, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, setRequestUri, yaf_request_set_request_uri_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isDispatched, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, setDispatched, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, isRouted, yaf_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_request, setRouted, yaf_request_set_routed_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(request){
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Request_Abstract", "Yaf\\Request_Abstract", yaf_request_methods);
	yaf_request_ce 			= zend_register_internal_class_ex(&ce, NULL);
	yaf_request_ce->ce_flags = ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	zend_declare_property_null(yaf_request_ce, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), ZEND_ACC_PUBLIC);
	zend_declare_property_null(yaf_request_ce, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), ZEND_ACC_PUBLIC);
	zend_declare_property_null(yaf_request_ce, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION),     ZEND_ACC_PUBLIC);
	zend_declare_property_null(yaf_request_ce, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_METHOD), 	ZEND_ACC_PUBLIC);
	zend_declare_property_null(yaf_request_ce, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_PARAMS),  	ZEND_ACC_PROTECTED);
	zend_declare_property_null(yaf_request_ce, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_LANG), 		ZEND_ACC_PROTECTED);
	zend_declare_property_null(yaf_request_ce, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_EXCEPTION),  ZEND_ACC_PROTECTED);

	zend_declare_property_string(yaf_request_ce, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_BASE), "", ZEND_ACC_PROTECTED);
	zend_declare_property_string(yaf_request_ce, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_URI),  "", ZEND_ACC_PROTECTED);
	zend_declare_property_bool(yaf_request_ce, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_STATE),	0,	ZEND_ACC_PROTECTED);
	zend_declare_property_bool(yaf_request_ce, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ROUTED), 0, 	ZEND_ACC_PROTECTED);

	YAF_STARTUP(request_http);
	YAF_STARTUP(request_simple);

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
