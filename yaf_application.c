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

/* $Id: yaf_application.c 329197 2013-01-18 05:55:37Z laruence $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h" /* for zend_alter_ini_entry */
#include "Zend/zend_interfaces.h" /* for zend_call_method_with_* */

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_application.h"
#include "yaf_dispatcher.h"
#include "yaf_router.h"
#include "yaf_config.h"
#include "yaf_loader.h"
#include "yaf_request.h"
#include "yaf_bootstrap.h"
#include "yaf_exception.h"

zend_class_entry * yaf_application_ce;

/** {{{ ARG_INFO
 *  */
ZEND_BEGIN_ARG_INFO_EX(yaf_application_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_application_construct_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, config)
	ZEND_ARG_INFO(0, environ)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_application_app_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_application_execute_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_application_getconfig_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_application_getmodule_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_application_getdispatch_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_application_bootstrap_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, bootstrap)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_application_environ_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_application_run_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_application_setappdir_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, directory)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ int yaf_application_is_module_name(char *name, int len TSRMLS_DC)
*/
int yaf_application_is_module_name(char *name, int len TSRMLS_DC) {
	zval 			  *modules, *pzval, rv;
	yaf_application_t *app;

	app = zend_read_static_property(yaf_application_ce,
			ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_APP), 1 TSRMLS_CC);
	if (UNEXPECTED(Z_TYPE_P(app) != IS_OBJECT)) {
		return 0;
	}

	modules = zend_read_property(yaf_application_ce, app,
			ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_MODULES), 1, &rv TSRMLS_CC);
	if (UNEXPECTED(Z_TYPE_P(modules) != IS_ARRAY)) {
		return 0;
	}

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(modules), pzval) {
		if (UNEXPECTED(Z_TYPE_P(pzval) != IS_STRING)) {
			continue;
		}
		if (Z_STRLEN_P(pzval) == len && strncasecmp(Z_STRVAL_P(pzval), name, len) == 0) {
			return 1;
		}
	} ZEND_HASH_FOREACH_END();
	return 0;
}
/* }}} */

/** {{{ static int yaf_application_parse_option(zval *options TSRMLS_DC)
*/
static int yaf_application_parse_option(zval *options TSRMLS_DC) {
	HashTable 	*conf;
	zval  		*pzval, *psval, *app;

	conf = HASH_OF(options);
	if ((pzval = zend_hash_str_find(conf, ZEND_STRL("application"))) == NULL) {
		/* For back compatibilty */
		if ((pzval = zend_hash_str_find(conf, ZEND_STRL("yaf"))) == NULL) {
			yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "%s", "Expected an array of application configure");
			return FAILURE;
		}
	}

	app = pzval;
	if (Z_TYPE_P(app) != IS_ARRAY) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "%s", "Expected an array of application configure");
		return FAILURE;
	}

	if ((pzval = zend_hash_str_find(Z_ARRVAL_P(app), ZEND_STRL("directory"))) == NULL
			|| Z_TYPE_P(pzval) != IS_STRING || Z_STRLEN_P(pzval) == 0) {
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED TSRMLS_CC, "%s", "Expected a directory entry in application configures");
		return FAILURE;
	}

	if (*(Z_STRVAL_P(pzval) + Z_STRLEN_P(pzval) - 1) == DEFAULT_SLASH) {
		YAF_G(directory) = estrndup(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval) - 1);
	} else {
		YAF_G(directory) = estrndup(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval));
	}

	if ((pzval = zend_hash_str_find(Z_ARRVAL_P(app), ZEND_STRL("ext"))) != NULL
			&& Z_TYPE_P(pzval) == IS_STRING) {
		YAF_G(ext) = estrndup(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval));
	} else {
		YAF_G(ext) = YAF_DEFAULT_EXT;
	}

	if ((pzval = zend_hash_str_find(Z_ARRVAL_P(app), ZEND_STRL("bootstrap"))) != NULL
			&& Z_TYPE_P(pzval) == IS_STRING) {
		YAF_G(bootstrap) = estrndup(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval));
	}

	if ((pzval = zend_hash_str_find(Z_ARRVAL_P(app), ZEND_STRL("library"))) != NULL) {
		if (IS_STRING == Z_TYPE_P(pzval)) {
			YAF_G(local_library) = estrndup(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval));
		} else if (IS_ARRAY == Z_TYPE_P(pzval)) {
			if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("directory"))) != NULL
					&& Z_TYPE_P(psval) == IS_STRING) {
				YAF_G(local_library) = estrndup(Z_STRVAL_P(psval), Z_STRLEN_P(psval));
			}
			if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("namespace"))) != NULL
					&& Z_TYPE_P(psval) == IS_STRING) {
				uint i, len;
				char *src = Z_STRVAL_P(psval);
				if (Z_STRLEN_P(psval)) {
				    char *target = emalloc(Z_STRLEN_P(psval) + 1);
					len = 0;
					for(i=0; i<Z_STRLEN_P(psval); i++) {
						if (src[i] == ',') {
							target[len++] = DEFAULT_DIR_SEPARATOR;
						} else if (src[i] != ' ') {
							target[len++] = src[i];
						}
					}
					target[len] = '\0';
					yaf_loader_register_namespace_single(target, len TSRMLS_CC);
					efree(target);
				}
			}
		}
	}

	if ((pzval = zend_hash_str_find(Z_ARRVAL_P(app), ZEND_STRL("view"))) == NULL 
			|| Z_TYPE_P(pzval) != IS_ARRAY) {
		YAF_G(view_ext) = YAF_DEFAULT_VIEW_EXT;
	} else {
		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("ext"))) == NULL
				|| Z_TYPE_P(psval) != IS_STRING) {
			YAF_G(view_ext) = YAF_DEFAULT_VIEW_EXT;
		} else {
			YAF_G(view_ext) = estrndup(Z_STRVAL_P(psval), Z_STRLEN_P(psval));
		}
	}

	if ((pzval = zend_hash_str_find(Z_ARRVAL_P(app), ZEND_STRL("baseUri"))) != NULL
			&& Z_TYPE_P(pzval) == IS_STRING) {
		YAF_G(base_uri) = estrndup(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval));
	}

	if ((pzval = zend_hash_str_find(Z_ARRVAL_P(app), ZEND_STRL("dispatcher"))) == NULL
			|| Z_TYPE_P(pzval) != IS_ARRAY) {
		YAF_G(default_module) = YAF_ROUTER_DEFAULT_MODULE;
		YAF_G(default_controller) = YAF_ROUTER_DEFAULT_CONTROLLER;
		YAF_G(default_action)  = YAF_ROUTER_DEFAULT_ACTION;
	} else {
		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("defaultModule"))) == NULL
				|| Z_TYPE_P(psval) != IS_STRING) {
			YAF_G(default_module) = YAF_ROUTER_DEFAULT_MODULE;
		} else {
			YAF_G(default_module) = zend_str_tolower_dup(Z_STRVAL_P(psval), Z_STRLEN_P(psval));
			*(YAF_G(default_module)) = toupper(*YAF_G(default_module));
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("defaultController"))) == NULL
				|| Z_TYPE_P(psval) != IS_STRING) {
			YAF_G(default_controller) = YAF_ROUTER_DEFAULT_CONTROLLER;
		} else {
			YAF_G(default_controller) = zend_str_tolower_dup(Z_STRVAL_P(psval), Z_STRLEN_P(psval));
			*(YAF_G(default_controller)) = toupper(*YAF_G(default_controller));
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("defaultAction"))) == NULL
				|| Z_TYPE_P(psval) != IS_STRING) {
			YAF_G(default_action)	  = YAF_ROUTER_DEFAULT_ACTION;
		} else {
			YAF_G(default_action) = zend_str_tolower_dup(Z_STRVAL_P(psval), Z_STRLEN_P(psval));
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("throwException"))) != NULL) {
			zval *tmp = psval;
			Z_TRY_ADDREF_P(tmp);
			convert_to_boolean_ex(tmp);
			YAF_G(throw_exception) = Z_TYPE_P(tmp) == IS_TRUE ? 1 : 0;
			zval_ptr_dtor(tmp);
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("catchException"))) != NULL) {
			zval *tmp = psval;
			Z_TRY_ADDREF_P(tmp);
			convert_to_boolean_ex(tmp);
			YAF_G(catch_exception) = Z_TYPE_P(tmp) == IS_TRUE ? 1 : 0;
			zval_ptr_dtor(tmp);
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("defaultRoute"))) != NULL
				&& Z_TYPE_P(psval) == IS_ARRAY) {
			YAF_G(default_route) = psval;
		}
	}

	do {
		char *ptrptr;
		zval module, *zmodules;

		zmodules = (zval *)emalloc(sizeof(zval));
		array_init(zmodules);
		if ((pzval = zend_hash_str_find(Z_ARRVAL_P(app), ZEND_STRL("modules"))) != NULL
				&& Z_TYPE_P(pzval) == IS_STRING && Z_STRLEN_P(pzval)) {
			char *seg, *modules;
			modules = estrndup(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval));
			seg = php_strtok_r(modules, ",", &ptrptr);
			while (seg) {
				if (seg && strlen(seg)) {
					ZVAL_STRINGL(&module, seg, strlen(seg));
					zend_hash_next_index_insert(Z_ARRVAL_P(zmodules), &module);
				}
				seg = php_strtok_r(NULL, ",", &ptrptr);
			}
			efree(modules);
		} else {
			ZVAL_STRING(&module, YAF_G(default_module));
			zend_hash_next_index_insert(Z_ARRVAL_P(zmodules), &module);
		}
		YAF_G(modules) = zmodules;
	} while (0);

	if ((pzval = zend_hash_str_find(Z_ARRVAL_P(app), ZEND_STRL("system"))) != NULL 
	        && Z_TYPE_P(pzval) == IS_ARRAY) {
		zval *value;
		char name[128];
		zend_string *key;
		ulong idx;
		uint len;

		HashTable *ht = Z_ARRVAL_P(pzval);

		ZEND_HASH_FOREACH_KEY_VAL(ht, idx, key, value) {
			if (key) {
				zend_string *tmp_name;

				len = snprintf(name, sizeof(name), "%s.%s", "yaf", key->val);
				if (len > sizeof(name) -1) {
					len = sizeof(name) - 1;
				}
				convert_to_string(value);
				tmp_name = zend_string_init(name, len, 0); 
				zend_alter_ini_entry(tmp_name, Z_STR_P(value), PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
				zend_string_release(tmp_name);
			}
		} ZEND_HASH_FOREACH_END();
	}

	return SUCCESS;
}
/* }}} */

/** {{{ proto Yaf_Application::__construct(mixed $config, string $environ = YAF_G(environ))
*/
PHP_METHOD(yaf_application, __construct) {
	yaf_config_t 	  *zconfig, rconfig;
	yaf_request_t 	  *request, rrequest;
	yaf_dispatcher_t  *zdispatcher, rdispather;
	yaf_application_t *app, *self;
	yaf_loader_t	  *loader, rloader;
	zval 			  *config, rv;
	zval 			  *section = NULL, rsection;

	app	= zend_read_static_property(yaf_application_ce, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_APP), 1 TSRMLS_CC);

#if PHP_YAF_DEBUG
	php_error_docref(NULL TSRMLS_CC, E_STRICT, "Yaf is running in debug mode");
#endif

	if (!ZVAL_IS_NULL(app)) {
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED TSRMLS_CC, "Only one application can be initialized");
		RETURN_FALSE;
	}

	self = getThis();
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &config, &section) == FAILURE) {
		YAF_UNINITIALIZED_OBJECT(getThis());
		return;
	}

	ZVAL_NULL(&rconfig);
	if (!section || Z_TYPE_P(section) != IS_STRING || !Z_STRLEN_P(section)) {
		ZVAL_STRING(&rsection, YAF_G(environ));
		zconfig = yaf_config_instance(&rconfig, config, &rsection TSRMLS_CC);
		zval_ptr_dtor(&rsection);
	} else {
		zconfig = yaf_config_instance(&rconfig, config, section TSRMLS_CC);
	}

	if  (!zconfig || Z_TYPE_P(zconfig) != IS_OBJECT
			|| !instanceof_function(Z_OBJCE_P(zconfig), yaf_config_ce TSRMLS_CC)
			|| yaf_application_parse_option(zend_read_property(yaf_config_ce,
				   	zconfig, ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, &rv TSRMLS_CC) TSRMLS_CC) == FAILURE) {
		YAF_UNINITIALIZED_OBJECT(getThis());
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED TSRMLS_CC, "Initialization of application config failed");
		RETURN_FALSE;
	}

	ZVAL_NULL(&rrequest);
	request = yaf_request_instance(&rrequest, YAF_G(base_uri) TSRMLS_CC);
	if (YAF_G(base_uri)) {
		efree(YAF_G(base_uri));
		YAF_G(base_uri) = NULL;
	}

	if (!request) {
		YAF_UNINITIALIZED_OBJECT(getThis());
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED TSRMLS_CC, "Initialization of request failed");
		RETURN_FALSE;
	}

	ZVAL_NULL(&rdispather);
	zdispatcher = yaf_dispatcher_instance(&rdispather TSRMLS_CC);
	if (Z_TYPE_P(zdispatcher) != IS_OBJECT
			|| !instanceof_function(Z_OBJCE_P(zdispatcher), yaf_dispatcher_ce TSRMLS_CC)) {
		YAF_UNINITIALIZED_OBJECT(getThis());
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED TSRMLS_CC, "Instantiation of application dispatcher failed");
		RETURN_FALSE;
	}
	yaf_dispatcher_set_request(zdispatcher, request TSRMLS_CC);

	zend_update_property(yaf_application_ce, self, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_CONFIG), zconfig TSRMLS_CC);
	zend_update_property(yaf_application_ce, self, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_DISPATCHER), zdispatcher TSRMLS_CC);

	zval_ptr_dtor(request);
	zval_ptr_dtor(zdispatcher);
	zval_ptr_dtor(zconfig);

	ZVAL_NULL(&rloader);
	if (YAF_G(local_library)) {
		loader = yaf_loader_instance(&rloader, YAF_G(local_library),
				strlen(YAF_G(global_library))? YAF_G(global_library) : NULL TSRMLS_CC);
		efree(YAF_G(local_library));
		YAF_G(local_library) = NULL;
	} else {
		char *local_library;
		spprintf(&local_library, 0, "%s%c%s", YAF_G(directory), DEFAULT_SLASH, YAF_LIBRARY_DIRECTORY_NAME);
		loader = yaf_loader_instance(&rloader, local_library,
				strlen(YAF_G(global_library))? YAF_G(global_library) : NULL TSRMLS_CC);
		efree(local_library);
	}

	if (!loader) {
		YAF_UNINITIALIZED_OBJECT(getThis());
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED TSRMLS_CC, "Initialization of application auto loader failed");
		RETURN_FALSE;
	}

	zend_update_property_bool(yaf_application_ce, self, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_RUN), 0 TSRMLS_CC);
	zend_update_property_string(yaf_application_ce, self, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_ENV), YAF_G(environ) TSRMLS_CC);

	if (YAF_G(modules)) {
		zend_update_property(yaf_application_ce, self, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_MODULES), YAF_G(modules) TSRMLS_CC);
		Z_DELREF_P(YAF_G(modules));
		efree(YAF_G(modules));
		YAF_G(modules) = NULL;
	} else {
		zend_update_property_null(yaf_application_ce, self, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_MODULES) TSRMLS_CC);
	}

	zend_update_static_property(yaf_application_ce, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_APP), self TSRMLS_CC);
}
/* }}} */

/** {{{ proto public Yaf_Application::__desctruct(void)
*/
PHP_METHOD(yaf_application, __destruct) {
	zend_update_static_property_null(yaf_application_ce, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_APP) TSRMLS_CC);
}
/* }}} */

/** {{{ proto private Yaf_Application::__sleep(void)
*/
PHP_METHOD(yaf_application, __sleep) {
}
/* }}} */

/** {{{ proto private Yaf_Application::__wakeup(void)
*/
PHP_METHOD(yaf_application, __wakeup) {
}
/* }}} */

/** {{{ proto private Yaf_Application::__clone(void)
*/
PHP_METHOD(yaf_application, __clone) {
}
/* }}} */

/** {{{ proto public Yaf_Application::run(void)
*/
PHP_METHOD(yaf_application, run) {
	zval *running;
	yaf_dispatcher_t  *dispatcher;
	yaf_response_t	  *response, rresponse;
	yaf_application_t *self = getThis();

	running = zend_read_property(yaf_application_ce, self, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_RUN), 1 TSRMLS_CC);
	if (IS_TRUE == Z_TYPE_P(running)) {
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED TSRMLS_CC, "An application instance already run");
		RETURN_TRUE;
	}

	ZVAL_BOOL(running, 1);
	zend_update_property(yaf_application_ce, self, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_RUN), running TSRMLS_CC);

	dispatcher = zend_read_property(yaf_application_ce, self, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_DISPATCHER), 1 TSRMLS_CC);
	ZVAL_NULL(&rresponse);
	if ((response = yaf_dispatcher_dispatch(dispatcher, &rresponse TSRMLS_CC))) {
		RETURN_ZVAL(response, 1, 1);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Application::execute(callback $func)
 * We can not call to zif_call_user_func on windows, since it was not declared with dllexport
 */
PHP_METHOD(yaf_application, execute) {
	zval retval;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f*", &fci, &fci_cache, &fci.params, &fci.param_count) == FAILURE) {
		return;
	}

	fci.retval = &retval;

	if (zend_call_function(&fci, &fci_cache TSRMLS_CC) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		ZVAL_COPY_VALUE(return_value, &retval);
	}
}
/* }}} */

/** {{{ proto public Yaf_Application::app(void)
*/
PHP_METHOD(yaf_application, app) {
	yaf_application_t *app = zend_read_static_property(yaf_application_ce, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_APP), 1 TSRMLS_CC);
	RETVAL_ZVAL(app, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Application::getConfig(void)
*/
PHP_METHOD(yaf_application, getConfig) {
	yaf_config_t *config = zend_read_property(yaf_application_ce, getThis(), ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_CONFIG), 1 TSRMLS_CC);
	RETVAL_ZVAL(config, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Application::getDispatcher(void)
*/
PHP_METHOD(yaf_application, getDispatcher) {
	yaf_dispatcher_t *dispatcher = zend_read_property(yaf_application_ce, getThis(), ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_DISPATCHER), 1 TSRMLS_CC);
	RETVAL_ZVAL(dispatcher, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Application::getModules(void)
*/
PHP_METHOD(yaf_application, getModules) {
	zval *modules = zend_read_property(yaf_application_ce, getThis(), ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_MODULES), 1 TSRMLS_CC);
	RETVAL_ZVAL(modules, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Application::environ(void)
*/
PHP_METHOD(yaf_application, environ) {
	zval *env = zend_read_property(yaf_application_ce, getThis(), ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_ENV), 1 TSRMLS_CC);
	RETURN_STRINGL(Z_STRVAL_P(env), Z_STRLEN_P(env));
}
/* }}} */

/** {{{ proto public Yaf_Application::bootstrap(void)
*/
PHP_METHOD(yaf_application, bootstrap) {
	char			*bootstrap_path;
	size_t 			len;
	uint			retval = 1;
	zend_class_entry	*ce;
	yaf_application_t	*self = getThis();

	if ((ce = zend_hash_str_find_ptr(EG(class_table), YAF_DEFAULT_BOOTSTRAP_LOWER, YAF_DEFAULT_BOOTSTRAP_LEN)) == NULL) {
		if (YAF_G(bootstrap)) {
			bootstrap_path  = estrdup(YAF_G(bootstrap));
			len = strlen(YAF_G(bootstrap));
		} else {
			len = spprintf(&bootstrap_path, 0, "%s%c%s.%s", YAF_G(directory), DEFAULT_SLASH, YAF_DEFAULT_BOOTSTRAP, YAF_G(ext));
		}

		if (!yaf_loader_import(bootstrap_path, len + 1, 0 TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't find bootstrap file %s", bootstrap_path);
			retval = 0;
		} else if ((ce = zend_hash_str_find_ptr(EG(class_table), YAF_DEFAULT_BOOTSTRAP_LOWER, YAF_DEFAULT_BOOTSTRAP_LEN)) == NULL)  {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't find class %s in %s", YAF_DEFAULT_BOOTSTRAP, bootstrap_path);
			retval = 0;
		} else if (!instanceof_function(ce, yaf_bootstrap_ce TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expect a %s instance, %s give", yaf_bootstrap_ce->name->val, ce->name->val);
			retval = 0;
		}

		efree(bootstrap_path);
	}

	if (!retval) {
		RETURN_FALSE;
	} else {
		zval 			bootstrap;
		HashTable 		*methods;
		yaf_dispatcher_t 	*dispatcher;

		object_init_ex(&bootstrap, ce);
		dispatcher = zend_read_property(yaf_application_ce, self, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_DISPATCHER), 1 TSRMLS_CC);

		methods	= &(ce->function_table);
		for(zend_hash_internal_pointer_reset(methods);
				zend_hash_has_more_elements(methods) == SUCCESS;
				zend_hash_move_forward(methods)) {
			zend_string *func;
			ulong idx;

			zend_hash_get_current_key_ex(methods, &func, &idx, 0, NULL);
			/* cann't use ZEND_STRL in strncasecmp, it cause a compile failed in VS2009 */
			if (strncasecmp(func->val, YAF_BOOTSTRAP_INITFUNC_PREFIX, sizeof(YAF_BOOTSTRAP_INITFUNC_PREFIX)-1)) {
				continue;
			}

			zend_call_method(&bootstrap, ce, NULL, func->val, func->len, NULL, 1, dispatcher, NULL TSRMLS_CC);
			/** an uncaught exception threw in function call */
			if (EG(exception)) {
				zval_ptr_dtor(&bootstrap);
				RETURN_FALSE;
			}
		}

		zval_ptr_dtor(&bootstrap);
	}

	RETVAL_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Application::getLastErrorNo(void)
*/
PHP_METHOD(yaf_application, getLastErrorNo) {
	zval *errcode = zend_read_property(yaf_application_ce, getThis(), ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_ERRNO), 1 TSRMLS_CC);
	RETURN_LONG(Z_LVAL_P(errcode));
}
/* }}} */

/** {{{ proto public Yaf_Application::getLastErrorMsg(void)
*/
PHP_METHOD(yaf_application, getLastErrorMsg) {
	zval *errmsg = zend_read_property(yaf_application_ce, getThis(), ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_ERRMSG), 1 TSRMLS_CC);
	RETURN_STRINGL(Z_STRVAL_P(errmsg), Z_STRLEN_P(errmsg));
}
/* }}} */

/** {{{ proto public Yaf_Application::clearLastError(void)
*/
PHP_METHOD(yaf_application, clearLastError) {
	zend_update_property_long(yaf_application_ce, getThis(), ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_ERRNO), 0 TSRMLS_CC);
	zend_update_property_string(yaf_application_ce, getThis(), ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_ERRMSG), "" TSRMLS_CC);

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Application::setAppDirectory(string $directory)
*/
PHP_METHOD(yaf_application, setAppDirectory) {
	size_t			len;
	char	 		*directory;
	yaf_dispatcher_t 	*self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &directory, &len) == FAILURE) {
		return;
	}

	if (!len || !IS_ABSOLUTE_PATH(directory, len)) {
		RETURN_FALSE;
	}

	efree(YAF_G(directory));

	YAF_G(directory) = estrndup(directory, len);

	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Application::getAppDirectory(void)
*/
PHP_METHOD(yaf_application, getAppDirectory) {
	RETURN_STRING(YAF_G(directory));
}
/* }}} */

/** {{{ yaf_application_methods
*/
zend_function_entry yaf_application_methods[] = {
	PHP_ME(yaf_application, __construct, 		yaf_application_construct_arginfo, 	ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
		PHP_ME(yaf_application, run, 	 	 	yaf_application_run_arginfo, 		ZEND_ACC_PUBLIC)
		PHP_ME(yaf_application, execute,	  	yaf_application_execute_arginfo, 	ZEND_ACC_PUBLIC)
		PHP_ME(yaf_application, app, 	 	 	yaf_application_app_arginfo, 		ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
		YAF_ME(yaf_application_environ, "environ", 	yaf_application_environ_arginfo, 	ZEND_ACC_PUBLIC)
		PHP_ME(yaf_application, bootstrap,   		yaf_application_bootstrap_arginfo,  	ZEND_ACC_PUBLIC)
		PHP_ME(yaf_application, getConfig,   		yaf_application_getconfig_arginfo, 	ZEND_ACC_PUBLIC)
		PHP_ME(yaf_application, getModules,   		yaf_application_getmodule_arginfo,  	ZEND_ACC_PUBLIC)
		PHP_ME(yaf_application, getDispatcher, 		yaf_application_getdispatch_arginfo,	ZEND_ACC_PUBLIC)
		PHP_ME(yaf_application, setAppDirectory,	yaf_application_setappdir_arginfo,  	ZEND_ACC_PUBLIC)
		PHP_ME(yaf_application, getAppDirectory,	yaf_application_void_arginfo, 		ZEND_ACC_PUBLIC)
		PHP_ME(yaf_application, getLastErrorNo, 	yaf_application_void_arginfo, 		ZEND_ACC_PUBLIC)
		PHP_ME(yaf_application, getLastErrorMsg,	yaf_application_void_arginfo, 		ZEND_ACC_PUBLIC)
		PHP_ME(yaf_application, clearLastError, 	yaf_application_void_arginfo, 		ZEND_ACC_PUBLIC)
		PHP_ME(yaf_application, __destruct,		NULL, 					ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
		PHP_ME(yaf_application, __clone,		NULL, 					ZEND_ACC_PRIVATE | ZEND_ACC_CLONE)
		PHP_ME(yaf_application, __sleep,		NULL, 					ZEND_ACC_PRIVATE)
		PHP_ME(yaf_application, __wakeup,		NULL, 					ZEND_ACC_PRIVATE)
		{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(application) {
	zend_class_entry ce;
	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Application", "Yaf\\Application", yaf_application_methods);

	yaf_application_ce = zend_register_internal_class_ex(&ce, NULL TSRMLS_CC);
	yaf_application_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;

	zend_declare_property_null(yaf_application_ce, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_CONFIG), 	ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_application_ce, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_DISPATCHER), 	ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_application_ce, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_APP), 	 	ZEND_ACC_STATIC | ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_application_ce, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_MODULES), 	ZEND_ACC_PROTECTED TSRMLS_CC);

	zend_declare_property_bool(yaf_application_ce, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_RUN), 	0, 		ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_string(yaf_application_ce, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_ENV), 	YAF_G(environ), ZEND_ACC_PROTECTED TSRMLS_CC);

	zend_declare_property_long(yaf_application_ce, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_ERRNO), 		0,	ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_string(yaf_application_ce, ZEND_STRL(YAF_APPLICATION_PROPERTY_NAME_ERRMSG), 	"", 	ZEND_ACC_PROTECTED TSRMLS_CC);

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
