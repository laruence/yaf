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

zend_class_entry    *yaf_application_ce;
zend_object_handlers yaf_application_obj_handlers;

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

static inline HashTable* yaf_application_get_module_nams() /* {{{ */ {
	yaf_application_object *app;

	if ((app = yaf_application_instance()) == NULL) {
		return NULL;
	}

	return &app->modules;
}
/* }}} */

static HashTable *yaf_application_get_debug_info(zval *object, int *is_temp) /* {{{ */ {
	zval rv;
	HashTable *ht;
	yaf_application_object *app = Z_YAFAPPOBJ_P(object);

	*is_temp = 1;
	ALLOC_HASHTABLE(ht);
	zend_hash_init(ht, 16, NULL, ZVAL_PTR_DTOR, 0);

	ZVAL_STR_COPY(&rv, app->directory);
	zend_hash_str_add(ht, "directory", sizeof("directory") - 1, &rv);

	if (app->library) {
		ZVAL_STR_COPY(&rv, app->library);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_add(ht, "library", sizeof("library") - 1, &rv);

	if (app->bootstrap) {
		ZVAL_STR_COPY(&rv, app->bootstrap);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_add(ht, "bootstrap", sizeof("bootstrap") - 1, &rv);

	ZVAL_STR_COPY(&rv, app->ext);
	zend_hash_str_add(ht, "ext", sizeof("ext") - 1, &rv);

	ZVAL_STR_COPY(&rv, app->view_ext);
	zend_hash_str_add(ht, "view_ext", sizeof("view_ext") - 1, &rv);

	ZVAL_STR_COPY(&rv, app->env);
	zend_hash_str_add(ht, "environ:protected", sizeof("environ:protected") - 1, &rv);

	ZVAL_BOOL(&rv, app->running);
	zend_hash_str_add(ht, "running:protected", sizeof("running:protected") - 1, &rv);

	if (app->err_msg) {
		ZVAL_STR_COPY(&rv, app->err_msg);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_add(ht, "err_msg:protected", sizeof("err_msg:protected") - 1, &rv);

	ZVAL_BOOL(&rv, app->err_no);
	zend_hash_str_add(ht, "err_no:protected", sizeof("err_no:protected") - 1, &rv);

	if (Z_TYPE(app->config) == IS_OBJECT) {
		ZVAL_OBJ(&rv, Z_OBJ(app->config));
		Z_ADDREF(rv);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_add(ht, "config:protected", sizeof("config:protected") - 1, &rv);

	if (Z_TYPE(app->dispatcher) == IS_OBJECT) {
		ZVAL_OBJ(&rv, Z_OBJ(app->dispatcher));
		Z_ADDREF(rv);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_add(ht, "dispatcher:protected", sizeof("dispatcher:protected") - 1, &rv);

	ZVAL_ARR(&rv, zend_array_dup(&app->modules));
	zend_hash_str_add(ht, "modules:protected", sizeof("modules:protected") - 1, &rv);
	
	if (app->default_route) {
		ZVAL_ARR(&rv, zend_array_dup(app->default_route));
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_add(ht, "default_route:protected", sizeof("default_route:protected") - 1, &rv);

	return ht;
}
/* }}} */

static inline zend_object *yaf_application_get_dispatcher(yaf_application_object *app) /* {{{ */ {
	if (Z_TYPE(app->dispatcher) == IS_OBJECT) {
		Z_ADDREF(app->dispatcher);
		return Z_OBJ(app->dispatcher);
	}
	return NULL;
}
/* }}} */

static inline zend_object *yaf_application_get_config(yaf_application_object *app) /* {{{ */ {
	if (Z_TYPE(app->config) == IS_OBJECT) {
		Z_ADDREF(app->config);
		return Z_OBJ(app->config);
	}
	return NULL;
}
/* }}} */

static zval *yaf_application_read_property(zval *zobj, zval *name, int type, void **cache_slot, zval *rv) /* {{{ */ {
	zend_string *member;
	yaf_application_object *app = Z_YAFAPPOBJ_P(zobj);

	if (UNEXPECTED(Z_TYPE_P(name) != IS_STRING)) {
		return &EG(uninitialized_zval);
	}
	
	if (UNEXPECTED(type == BP_VAR_W || type == BP_VAR_RW)) {
		return &EG(error_zval);
	}

	member = Z_STR_P(name);
	
	if (zend_string_equals_literal(member, "directory")) {
		if (app->directory) {
			ZVAL_STR_COPY(rv, app->directory);
			return rv;
		}
		return &EG(uninitialized_zval);
	}

	if (zend_string_equals_literal(member, "library")) {
		if (app->library) {
			ZVAL_STR_COPY(rv, app->library);
			return rv;
		}
		return &EG(uninitialized_zval);
	}

	if (zend_string_equals_literal(member, "bootstrap")) {
		if (app->bootstrap) {
			ZVAL_STR_COPY(rv, app->bootstrap);
			return rv;
		}
		return &EG(uninitialized_zval);
	}

	if (zend_string_equals_literal(member, "ext")) {
		if (app->ext) {
			ZVAL_STR_COPY(rv, app->ext);
			return rv;
		}
		return &EG(uninitialized_zval);
	}

	if (zend_string_equals_literal(member, "view_ext")) {
		if (app->view_ext) {
			ZVAL_STR_COPY(rv, app->view_ext);
			return rv;
		}
		return &EG(uninitialized_zval);
	}

	/*
	if (zend_string_equals_literal(member, "environ")) {
		if (app->env) {
			ZVAL_STR_COPY(rv, app->env);
			return rv;
		}
		return &EG(uninitialized_zval);
	}

	if (zend_string_equals_literal(member, "running")) {
		ZVAL_BOOL(rv, app->running);
		return rv;
	}


	if (zend_string_equals_literal(member, "dispatcher")) {
		zend_object *dispatcher = yaf_application_get_dispatcher(app);
		if (dispatcher) {
			ZVAL_OBJ(rv, dispatcher);
			return rv;
		}
		return &EG(uninitialized_zval);
	}

	if (zend_string_equals_literal(member, "config")) {
		zend_object *config = yaf_application_get_config(app);
		if (config) {
			ZVAL_OBJ(rv, config);
			return rv;
		}
		return &EG(uninitialized_zval);
	}

	if (zend_string_equals_literal(member, "err_msg")) {
		if (app->err_msg) {
			ZVAL_STR_COPY(rv, app->err_msg);
			return rv;
		}
		return &EG(uninitialized_zval);
	}

	if (zend_string_equals_literal(member, "err_no")) {
		ZVAL_BOOL(rv, app->err_no);
		return rv;
	}
	*/

	return &EG(uninitialized_zval);
}
/* }}} */

static YAF_WRITE_HANDLER yaf_application_write_property(zval *zobj, zval *name, zval *value, void **cache_slot) /* {{{ */ {
	zend_string *member;
	yaf_application_object *app = Z_YAFAPPOBJ_P(zobj);

	if (UNEXPECTED(Z_TYPE_P(name) != IS_STRING)) {
		YAF_WHANDLER_RET(value);
	}
	
	member = Z_STR_P(name);
	
	if (zend_string_equals_literal(member, "directory")) {
		if (Z_TYPE_P(value) != IS_STRING) {
			YAF_WHANDLER_RET(value);
		}
		if (app->directory) {
			zend_string_release(app->directory);
		}
		app->directory = zend_string_copy(Z_STR_P(value));
		YAF_WHANDLER_RET(value);
	}

	if (zend_string_equals_literal(member, "bootstrap")) {
		if (Z_TYPE_P(value) != IS_STRING) {
			YAF_WHANDLER_RET(value);
		}
		if (app->bootstrap) {
			zend_string_release(app->bootstrap);
		}
		app->bootstrap = zend_string_copy(Z_STR_P(value));
		YAF_WHANDLER_RET(value);
	}

	if (zend_string_equals_literal(member, "library")) {
		if (Z_TYPE_P(value) != IS_STRING) {
			YAF_WHANDLER_RET(value);
		}
		if (app->library) {
			zend_string_release(app->library);
		}
		app->library = zend_string_copy(Z_STR_P(value));
		YAF_WHANDLER_RET(value);
	}

	if (zend_string_equals_literal(member, "view_ext")) {
		if (Z_TYPE_P(value) != IS_STRING) {
			YAF_WHANDLER_RET(value);
		}
		if (app->view_ext) {
			zend_string_release(app->view_ext);
		}
		app->view_ext = zend_string_copy(Z_STR_P(value));
		YAF_WHANDLER_RET(value);
	}

	if (zend_string_equals_literal(member, "ext")) {
		if (Z_TYPE_P(value) != IS_STRING) {
			YAF_WHANDLER_RET(value);
		}
		if (app->ext) {
			zend_string_release(app->ext);
		}
		app->ext = zend_string_copy(Z_STR_P(value));
		YAF_WHANDLER_RET(value);
	}

	YAF_WHANDLER_RET(value);
}
/* }}} */

int yaf_application_is_module_name(zend_string *name) /* {{{ */ {
	zval *pzval;
	HashTable *modules = yaf_application_get_module_nams();

	if (modules == NULL) {
		return 0;
	}

	ZEND_HASH_FOREACH_VAL(modules, pzval) {
		if (UNEXPECTED(Z_TYPE_P(pzval) != IS_STRING)) {
			continue;
		}
		if (zend_string_equals_ci(Z_STR_P(pzval), name)) {
			return 1;
		}
	} ZEND_HASH_FOREACH_END();
	return 0;
}
/* }}} */

int yaf_application_is_module_name_str(const char *name, size_t len) /* {{{ */ {
	zval *pzval;
	HashTable *modules = yaf_application_get_module_nams();

	if (modules == NULL) {
		return 0;
	}

	ZEND_HASH_FOREACH_VAL(modules, pzval) {
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

static int yaf_application_parse_option(yaf_application_object *app) /* {{{ */ {
	zval *config;
	HashTable *conf;
	zval *pzval, *psval;

	conf = Z_YAFCONFIGOBJ(app->config)->config;
	if (UNEXPECTED((config = zend_hash_str_find(conf, ZEND_STRL("application"))) == NULL)) {
		/* For back compatibilty */
		if ((config = zend_hash_str_find(conf, ZEND_STRL("yaf"))) == NULL) {
			yaf_trigger_error(YAF_ERR_TYPE_ERROR, "%s", "Expected an array of application configure");
			return 0;
		}
	}

	if (UNEXPECTED(Z_TYPE_P(config) != IS_ARRAY)) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "%s", "Expected an array of application configure");
		return 0;
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(config), ZEND_STRL("directory"))) == NULL ||
		Z_TYPE_P(pzval) != IS_STRING || Z_STRLEN_P(pzval) == 0)) {
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED, "%s", "Expected a directory entry in application configures");
		return 0;
	}

	if (UNEXPECTED(*(Z_STRVAL_P(pzval) + Z_STRLEN_P(pzval) - 1) == DEFAULT_SLASH)) {
		app->directory = zend_string_init(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval) - 1, 0);
	} else {
		app->directory = zend_string_copy(Z_STR_P(pzval));
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(config), ZEND_STRL("ext"))) != NULL &&
		Z_TYPE_P(pzval) == IS_STRING)) {
		app->ext = zend_string_copy(Z_STR_P(pzval));
	} else {
		app->ext = zend_string_init(ZEND_STRL(YAF_DEFAULT_EXT), 0);
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(config), ZEND_STRL("bootstrap"))) != NULL &&
		Z_TYPE_P(pzval) == IS_STRING)) {
		app->bootstrap = zend_string_copy(Z_STR_P(pzval));
	}

	if (EXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(config), ZEND_STRL("library"))) != NULL)) {
		if (IS_STRING == Z_TYPE_P(pzval)) {
			app->library = zend_string_copy(Z_STR_P(pzval));
		} else if (IS_ARRAY == Z_TYPE_P(pzval)) {
			if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("directory"))) != NULL &&
				Z_TYPE_P(psval) == IS_STRING) {
				app->library = zend_string_copy(Z_STR_P(psval));
			}
			if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("namespace"))) != NULL &&
				Z_TYPE_P(psval) == IS_STRING) {
				if (Z_STRLEN_P(psval)) {
					zend_string *prefix;
					char *src = Z_STRVAL_P(psval), *pos;
					size_t len = Z_STRLEN_P(psval);
					while ((pos = memchr(src, ',', len))) {
						len -= pos - src;
						prefix = zend_string_init(src, pos - src, 0);
						yaf_loader_register_namespace_single(Z_YAFLOADEROBJ(YAF_G(loader)), prefix);
						zend_string_release(prefix);
						src = pos + 1;
					}

					if (len) {
						prefix = zend_string_init(src, len, 0);
						yaf_loader_register_namespace_single(Z_YAFLOADEROBJ(YAF_G(loader)), prefix);
						zend_string_release(prefix);
					}
				}
			}
		}
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(config), ZEND_STRL("view"))) != NULL &&
		Z_TYPE_P(pzval) == IS_ARRAY &&
		((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("ext"))) != NULL &&
		Z_TYPE_P(psval) == IS_STRING))) {
		app->view_ext = zend_string_copy(Z_STR_P(psval));
	} else {
		app->view_ext = zend_string_init(YAF_DEFAULT_VIEW_EXT, sizeof(YAF_DEFAULT_VIEW_EXT) - 1, 0);
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(config), ZEND_STRL("baseUri"))) != NULL &&
		Z_TYPE_P(pzval) == IS_STRING)) {
		app->base_uri = zend_string_copy(Z_STR_P(pzval));
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(config), ZEND_STRL("dispatcher"))) != NULL &&
		Z_TYPE_P(pzval) == IS_ARRAY)) {
		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("defaultModule"))) != NULL &&
			Z_TYPE_P(psval) == IS_STRING) {
			app->default_module = yaf_canonical_name(1, Z_STR_P(psval));
		} else {
			app->default_module = zend_string_init(ZEND_STRL(YAF_ROUTER_DEFAULT_MODULE), 0);
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("defaultController"))) != NULL &&
			Z_TYPE_P(psval) == IS_STRING) {
			app->default_controller = yaf_canonical_name(1, Z_STR_P(psval));
		} else {
			app->default_controller = zend_string_init(ZEND_STRL(YAF_ROUTER_DEFAULT_CONTROLLER), 0);
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("defaultAction"))) != NULL &&
			Z_TYPE_P(psval) == IS_STRING) {
			app->default_action = yaf_canonical_name(0, Z_STR_P(psval));
		} else {
			app->default_action = zend_string_init(ZEND_STRL(YAF_ROUTER_DEFAULT_ACTION), 0);
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("throwException"))) != NULL) {
			YAF_G(throw_exception) = zend_is_true(psval);
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("catchException"))) != NULL) {
			YAF_G(catch_exception) = zend_is_true(psval);
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("defaultRoute"))) != NULL &&
			Z_TYPE_P(psval) == IS_ARRAY) {
			/* leave it to configs destructor */
			app->default_route = Z_ARRVAL_P(psval);
		}
	} else {
		app->default_module = zend_string_init(ZEND_STRL(YAF_ROUTER_DEFAULT_MODULE), 0);
		app->default_controller = zend_string_init(ZEND_STRL(YAF_ROUTER_DEFAULT_CONTROLLER), 0);
		app->default_action = zend_string_init(ZEND_STRL(YAF_ROUTER_DEFAULT_ACTION), 0);
		app->default_route = NULL;
	}

	do {
		zval rv;
		char *ptrptr;

		zend_hash_init(&app->modules, 8, NULL, ZVAL_PTR_DTOR, 0);

		if ((pzval = zend_hash_str_find(Z_ARRVAL_P(config), ZEND_STRL("modules"))) != NULL &&
			Z_TYPE_P(pzval) == IS_STRING && Z_STRLEN_P(pzval)) {
			char *seg, *modules;
			modules = estrndup(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval));
			seg = php_strtok_r(modules, ",", &ptrptr);
			while (seg) {
				if (seg && strlen(seg)) {
					ZVAL_STRINGL(&rv, seg, strlen(seg));
					*(Z_STRVAL(rv)) = toupper(*Z_STRVAL(rv));
					zend_hash_next_index_insert(&app->modules, &rv);
				}
				seg = php_strtok_r(NULL, ",", &ptrptr);
			}
			efree(modules);
		} else {
			ZVAL_STR_COPY(&rv, app->default_module);
			zend_hash_next_index_insert(&app->modules, &rv);
		}
	} while (0);

	if ((pzval = zend_hash_str_find(Z_ARRVAL_P(config), ZEND_STRL("system"))) != NULL &&
		Z_TYPE_P(pzval) == IS_ARRAY) {
		zval *value;
		char name[128];
		zend_string *key;
		size_t len;

		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pzval), key, value) {
			zend_string *str, *val;

			len = snprintf(name, sizeof(name), "%s.%s", "yaf", ZSTR_VAL(key));
			if (len > sizeof(name) -1) {
				len = sizeof(name) - 1;
			}
			str = zend_string_init(name, len, 0);
			val = zval_get_string(value);
			zend_alter_ini_entry(str, val, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
			zend_string_release(str);
			zend_string_release(val);
		} ZEND_HASH_FOREACH_END();
	}

	return 1;
}
/* }}} */

static zend_object* yaf_application_new(zend_class_entry *ce) /* {{{ */ {
	yaf_application_object *app = emalloc(sizeof(yaf_application_object));

	app->std.handlers = &yaf_application_obj_handlers;
	memset(((char*)app) + sizeof(zend_object), 0, sizeof(yaf_application_object) - sizeof(zend_object));

	zend_object_std_init(&app->std, ce);

	return &app->std;
}
/* }}} */

static void yaf_application_free(zend_object *object) /* {{{ */ {
	yaf_application_object *app = yaf_application_instance();

	if (app != (yaf_application_object*)object) {
		zend_object_std_dtor(object);
		return;
	}

	zend_string_release(app->env);
	if (Z_TYPE(app->config) != IS_OBJECT) {
		zend_object_std_dtor(object);
		return;
	}

	OBJ_RELEASE(Z_OBJ(app->config));
	zend_hash_destroy(&app->modules);
	OBJ_RELEASE(Z_OBJ(app->dispatcher));
	zend_string_release(app->default_module);
	zend_string_release(app->default_controller);
	zend_string_release(app->default_action);
	if (app->library) {
		zend_string_release(app->library);
	}
	zend_string_release(app->directory);
	zend_string_release(app->ext);

	if (app->bootstrap) {
		zend_string_release(app->bootstrap);
	}

	zend_string_release(app->view_ext);

	if (app->base_uri) {
		zend_string_release(app->base_uri);
	}

	if (app->err_msg) {
		zend_string_release(app->err_msg);
	}

	zend_object_std_dtor(object);
}
/* }}} */

/** {{{ proto Yaf_Application::__construct(mixed $config, string $environ = YAF_G(environ_name))
*/
PHP_METHOD(yaf_application, __construct) {
	zval *config;
	zval *section = NULL;
	yaf_loader_t *loader;
	yaf_application_object *app;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z|z", &config, &section) == FAILURE) {
		return;
	}

	if (Z_TYPE(YAF_G(app)) == IS_OBJECT) {
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED, "Only one application can be initialized");
		return;
	}

	ZVAL_OBJ(&YAF_G(app), Z_OBJ_P(getThis()));
	Z_ADDREF(YAF_G(app));

	app = Z_YAFAPPOBJ_P(getThis());

	if (!section || Z_TYPE_P(section) != IS_STRING || !Z_STRLEN_P(section)) {
		zend_string *s = zend_string_init(YAF_G(environ_name), strlen(YAF_G(environ_name)), 0);
		yaf_config_instance(&app->config, config, s);
		app->env = s;
	} else {
		yaf_config_instance(&app->config, config, Z_STR_P(section));
		app->env = zend_string_copy(Z_STR_P(section));
	}

	if (Z_TYPE(app->config) != IS_OBJECT) {
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED, "Initialization of application config failed");
		zval_ptr_dtor(&app->config);
		ZVAL_NULL(&app->config);
		RETURN_FALSE;
	}

	loader = yaf_loader_instance(NULL, NULL);

	if (!yaf_application_parse_option(app)) {
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED, "Parsing application config failed");
		zval_ptr_dtor(&app->config);
		ZVAL_NULL(&app->config);
		RETURN_FALSE;
	}

	if (app->library) {
		zend_string *global_library = strlen(YAF_G(global_library))?
			zend_string_init(YAF_G(global_library), strlen(YAF_G(global_library)), 0) : NULL;
		yaf_loader_set_library_path(Z_YAFLOADEROBJ_P(loader), app->library, global_library);
		if (global_library) {
			zend_string_release(global_library);
		}
	} else {
		zend_string *local_library, *global_library;
		local_library = strpprintf(0, "%s%c%s", ZSTR_VAL(app->directory), DEFAULT_SLASH, YAF_LIBRARY_DIRECTORY_NAME);
		global_library = strlen(YAF_G(global_library))?
			zend_string_init(YAF_G(global_library), strlen(YAF_G(global_library)), 0) : NULL;
		yaf_loader_set_library_path(Z_YAFLOADEROBJ_P(loader), local_library, global_library);
		zend_string_release(local_library);
		if (global_library) {
			zend_string_release(global_library);
		}
	}

	yaf_dispatcher_instance(&app->dispatcher);

	yaf_request_instance(&Z_YAFDISPATCHEROBJ(app->dispatcher)->request, app->base_uri);

}
/* }}} */

/** {{{ proto public Yaf_Application::run(void)
*/
PHP_METHOD(yaf_application, run) {
	yaf_application_object *app = Z_YAFAPPOBJ_P(getThis());
	yaf_response_t *response;

	if (app->running) {
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED, "An application instance already run");
		RETURN_FALSE;
	}

	app->running = 1;

	if ((response = yaf_dispatcher_dispatch(Z_YAFDISPATCHEROBJ(app->dispatcher))) == NULL) {
		RETURN_FALSE;
	}

	RETURN_ZVAL(response, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Application::execute(callback $func)
 * We can not call to zif_call_user_func on windows, since it was not declared with dllexport
 */
PHP_METHOD(yaf_application, execute) {
	zval retval;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "f*", &fci, &fci_cache, &fci.params, &fci.param_count) == FAILURE) {
		return;
	}

	fci.retval = &retval;

	if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		ZVAL_COPY_VALUE(return_value, &retval);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/** {{{ proto public Yaf_Application::app(void)
*/
PHP_METHOD(yaf_application, app) {
	RETURN_ZVAL(&YAF_G(app), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Application::getConfig(void)
*/
PHP_METHOD(yaf_application, getConfig) {
	yaf_application_object *app = Z_YAFAPPOBJ_P(getThis());
	zend_object *config = yaf_application_get_config(app);

	if (config) {
		RETURN_OBJ(config);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Application::getDispatcher(void)
*/
PHP_METHOD(yaf_application, getDispatcher) {
	yaf_application_object *app = Z_YAFAPPOBJ_P(getThis());
	zend_object *dispatcher = yaf_application_get_dispatcher(app);

	if (dispatcher) {
		RETURN_OBJ(dispatcher);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Application::getModules(void)
*/
PHP_METHOD(yaf_application, getModules) {
	yaf_application_object *app = Z_YAFAPPOBJ_P(getThis());

	RETURN_ARR(zend_array_dup(&app->modules));
}
/* }}} */

/** {{{ proto public Yaf_Application::environ(void)
*/
PHP_METHOD(yaf_application, environ) {
	yaf_application_object *app = Z_YAFAPPOBJ_P(getThis());
	if (app->env) {
		RETURN_STR(zend_string_copy(app->env));
	}
	RETURN_EMPTY_STRING();
}
/* }}} */

/** {{{ proto public Yaf_Application::bootstrap(void)
*/
PHP_METHOD(yaf_application, bootstrap) {
	unsigned  retval = 1;
	zend_class_entry  *ce;
	yaf_application_object *app = Z_YAFAPPOBJ_P(getThis());

	if (!(ce = zend_hash_str_find_ptr(EG(class_table), YAF_DEFAULT_BOOTSTRAP_LOWER, sizeof(YAF_DEFAULT_BOOTSTRAP_LOWER) - 1))) {
		zend_string *bootstrap_path;
		if (app->bootstrap) {
			bootstrap_path = zend_string_copy(app->bootstrap);
		} else {
			bootstrap_path = strpprintf(0, "%s%c%s.%s",
					ZSTR_VAL(app->directory), DEFAULT_SLASH, YAF_DEFAULT_BOOTSTRAP, ZSTR_VAL(app->ext));
		}
		if (!yaf_loader_import(ZSTR_VAL(bootstrap_path), ZSTR_LEN(bootstrap_path))) {
			php_error_docref(NULL, E_WARNING, "Couldn't find bootstrap file %s", ZSTR_VAL(bootstrap_path));
			retval = 0;
		} else if (UNEXPECTED((ce = zend_hash_str_find_ptr(EG(class_table),
						YAF_DEFAULT_BOOTSTRAP_LOWER, sizeof(YAF_DEFAULT_BOOTSTRAP_LOWER) - 1)) == NULL)) {
			php_error_docref(NULL, E_WARNING, "Couldn't find class %s in %s", YAF_DEFAULT_BOOTSTRAP, ZSTR_VAL(bootstrap_path));
			retval = 0;
		} else if (UNEXPECTED(!instanceof_function(ce, yaf_bootstrap_ce))) {
			php_error_docref(NULL, E_WARNING,
					"Expect a %s instance, %s give", ZSTR_VAL(yaf_bootstrap_ce->name), ZSTR_VAL(ce->name));
			retval = 0;
		}
		zend_string_release(bootstrap_path);
	}

	if (UNEXPECTED(!retval)) {
		RETURN_FALSE;
	} else {
		zend_string *func;
		zval bootstrap;
		yaf_dispatcher_t *dispatcher = &app->dispatcher;

		object_init_ex(&bootstrap, ce);

		ZEND_HASH_FOREACH_STR_KEY(&(ce->function_table), func) {
			/* cann't use ZEND_STRL in strncasecmp, it cause a compile failed in VS2009 */
			if (strncasecmp(ZSTR_VAL(func), YAF_BOOTSTRAP_INITFUNC_PREFIX, sizeof(YAF_BOOTSTRAP_INITFUNC_PREFIX) - 1)) {
				continue;
			}
			zend_call_method(&bootstrap, ce, NULL, ZSTR_VAL(func), ZSTR_LEN(func), NULL, 1, dispatcher, NULL);
			/** an uncaught exception threw in function call */
			if (UNEXPECTED(EG(exception))) {
				zval_ptr_dtor(&bootstrap);
				RETURN_FALSE;
			}
		} ZEND_HASH_FOREACH_END();
		zval_ptr_dtor(&bootstrap);
	}

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Application::getLastErrorNo(void)
*/
PHP_METHOD(yaf_application, getLastErrorNo) {
	RETURN_LONG(Z_YAFAPPOBJ_P(getThis())->err_no);
}
/* }}} */

/** {{{ proto public Yaf_Application::getLastErrorMsg(void)
*/
PHP_METHOD(yaf_application, getLastErrorMsg) {
	yaf_application_object *app = Z_YAFAPPOBJ_P(getThis());
	if (app->err_msg) {
		RETURN_STR_COPY(app->err_msg);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/** {{{ proto public Yaf_Application::clearLastError(void)
*/
PHP_METHOD(yaf_application, clearLastError) {
	yaf_application_object *app = Z_YAFAPPOBJ_P(getThis());
	if (app->err_msg) {
		zend_string_release(app->err_msg);
		app->err_msg = NULL;
	}
	app->err_no = 0;
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Application::setAppDirectory(string $directory)
*/
PHP_METHOD(yaf_application, setAppDirectory) {
	zend_string *directory;
	yaf_application_object *app = Z_YAFAPPOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &directory) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(directory) == 0 || !IS_ABSOLUTE_PATH(ZSTR_VAL(directory), ZSTR_LEN(directory))) {
		RETURN_FALSE;
	}

	zend_string_release(app->directory);
	app->directory = zend_string_copy(directory);

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Application::getAppDirectory(void)
*/
PHP_METHOD(yaf_application, getAppDirectory) {
	RETURN_STR(zend_string_copy(Z_YAFAPPOBJ_P(getThis())->directory));
}
/* }}} */

/** {{{ yaf_application_methods
*/
zend_function_entry yaf_application_methods[] = {
	PHP_ME(yaf_application, __construct,         yaf_application_construct_arginfo,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(yaf_application, run,                 yaf_application_run_arginfo,         ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, execute,             yaf_application_execute_arginfo,     ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, app,                 yaf_application_app_arginfo,         ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	YAF_ME(yaf_application_environ, "environ",   yaf_application_environ_arginfo,     ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, bootstrap,           yaf_application_bootstrap_arginfo,      ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, getConfig,           yaf_application_getconfig_arginfo,     ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, getModules,          yaf_application_getmodule_arginfo,      ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, getDispatcher,       yaf_application_getdispatch_arginfo,    ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, setAppDirectory,     yaf_application_setappdir_arginfo,      ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, getAppDirectory,     yaf_application_void_arginfo,         ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, getLastErrorNo,      yaf_application_void_arginfo,         ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, getLastErrorMsg,     yaf_application_void_arginfo,         ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, clearLastError,      yaf_application_void_arginfo,         ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(application) {
	zend_class_entry ce;
	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Application", "Yaf\\Application", yaf_application_methods);

	yaf_application_ce = zend_register_internal_class_ex(&ce, NULL);
	yaf_application_ce->ce_flags |= ZEND_ACC_FINAL;
	yaf_application_ce->create_object = yaf_application_new;
	yaf_application_ce->serialize = zend_class_serialize_deny;
	yaf_application_ce->unserialize = zend_class_unserialize_deny;

	memcpy(&yaf_application_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_application_obj_handlers.clone_obj = NULL;
	yaf_application_obj_handlers.free_obj = yaf_application_free;
	yaf_application_obj_handlers.get_debug_info = yaf_application_get_debug_info;
	yaf_application_obj_handlers.read_property = yaf_application_read_property;
	yaf_application_obj_handlers.write_property = yaf_application_write_property;

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
