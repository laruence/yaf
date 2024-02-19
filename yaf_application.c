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
#include "Zend/zend_exceptions.h" /* for zend_throw_exception_ex */
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

#if PHP_MAJOR_VERSION > 7
#include "yaf_application_arginfo.h"
#else
#include "yaf_application_legacy_arginfo.h"
#endif

zend_class_entry    *yaf_application_ce;
static zend_object_handlers yaf_application_obj_handlers;

static void yaf_application_free(zend_object *object) /* {{{ */ {
	yaf_application_object *app = yaf_application_instance();

	if ((app != php_yaf_application_fetch_object(object)) || !app->env) {
		zend_object_std_dtor(object);
		return;
	}

	zend_string_release(app->env);

	OBJ_RELEASE(Z_OBJ(app->config));
	OBJ_RELEASE(Z_OBJ(app->dispatcher));

	zend_string_release(app->default_module);
	zend_string_release(app->default_controller);
	zend_string_release(app->default_action);
	if (app->library) {
		zend_string_release(app->library);
	}
	zend_string_release(app->directory);

	if (app->ext) {
		zend_string_release(app->ext);
	}
	if (app->bootstrap) {
		zend_string_release(app->bootstrap);
	}
	if (app->view_ext) {
		zend_string_release(app->view_ext);
	}
	if (app->base_uri) {
		zend_string_release(app->base_uri);
	}
	if (app->err_msg) {
		zend_string_release(app->err_msg);
	}
	if (app->modules) {
		if (GC_DELREF(app->modules) == 0) {
			GC_REMOVE_FROM_BUFFER(app->modules);
			zend_array_destroy(app->modules);
		}
	}
	if (app->properties) {
		if (GC_DELREF(app->properties) == 0) {
			GC_REMOVE_FROM_BUFFER(app->properties);
			zend_array_destroy(app->properties);
		}
	}

	zend_object_std_dtor(object);
}
/* }}} */

static zend_object* yaf_application_new(zend_class_entry *ce) /* {{{ */ {
	yaf_application_object *app = emalloc(sizeof(yaf_application_object) + zend_object_properties_size(ce));

	memset(app, 0, XtOffsetOf(yaf_application_object, std));
	zend_object_std_init(&app->std, ce);
	app->std.handlers = &yaf_application_obj_handlers;

	return &app->std;
}
/* }}} */

static HashTable *yaf_application_get_gc(yaf_object *obj, zval **table, int *n) /* {{{ */ {
	yaf_application_object *app = php_yaf_application_fetch_object(yaf_strip_obj(obj));

	*table = &app->dispatcher;
	*n = 2;

	return NULL;
}
/* }}} */

static HashTable *yaf_application_get_properties(yaf_object *obj) /* {{{ */ {
	zval rv;
	HashTable *ht;
	yaf_application_object *app = php_yaf_application_fetch_object(yaf_strip_obj(obj));

    if (!app->properties) {
		ALLOC_HASHTABLE(app->properties);
		zend_hash_init(app->properties, 16, NULL, ZVAL_PTR_DTOR, 0);
		YAF_ALLOW_VIOLATION(app->properties);
	}

	ht = app->properties;
	ZVAL_STR_COPY(&rv, app->directory);
	zend_hash_str_update(ht, "directory", sizeof("directory") - 1, &rv);

	if (app->library) {
		ZVAL_STR_COPY(&rv, app->library);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_update(ht, "library", sizeof("library") - 1, &rv);

	if (app->bootstrap) {
		ZVAL_STR_COPY(&rv, app->bootstrap);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_update(ht, "bootstrap", sizeof("bootstrap") - 1, &rv);

	if (app->ext) {
		ZVAL_STR_COPY(&rv, app->ext);
	} else {
		ZVAL_STRINGL(&rv, YAF_DEFAULT_EXT, sizeof(YAF_DEFAULT_EXT) - 1);
	}
	zend_hash_str_update(ht, "ext", sizeof("ext") - 1, &rv);

	if (app->view_ext) {
		ZVAL_STR_COPY(&rv, app->view_ext);
	} else {
		ZVAL_STRINGL(&rv, YAF_DEFAULT_VIEW_EXT, sizeof(YAF_DEFAULT_VIEW_EXT) - 1);
	}
	zend_hash_str_update(ht, "view_ext", sizeof("view_ext") - 1, &rv);

	ZVAL_STR_COPY(&rv, app->env);
	zend_hash_str_update(ht, "environ:protected", sizeof("environ:protected") - 1, &rv);

	ZVAL_BOOL(&rv, YAF_APP_FLAGS(app) & YAF_APP_RUNNING);
	zend_hash_str_update(ht, "running:protected", sizeof("running:protected") - 1, &rv);

	if (app->err_msg) {
		ZVAL_STR_COPY(&rv, app->err_msg);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_update(ht, "err_msg:protected", sizeof("err_msg:protected") - 1, &rv);

	ZVAL_LONG(&rv, app->err_no);
	zend_hash_str_update(ht, "err_no:protected", sizeof("err_no:protected") - 1, &rv);
	if (Z_TYPE(app->config) == IS_OBJECT) {
		ZVAL_OBJ(&rv, Z_OBJ(app->config));
		Z_ADDREF(rv);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_update(ht, "config:protected", sizeof("config:protected") - 1, &rv);

	if (Z_TYPE(app->dispatcher) == IS_OBJECT) {
		ZVAL_OBJ(&rv, Z_OBJ(app->dispatcher));
		GC_ADDREF(Z_OBJ(app->dispatcher));
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_update(ht, "dispatcher:protected", sizeof("dispatcher:protected") - 1, &rv);

	if (app->modules) {
		ZVAL_ARR(&rv, app->modules);
		GC_ADDREF(app->modules);
	} else {
		zval t;
		array_init(&rv);
		if (app->default_module) {
			ZVAL_STR_COPY(&t, app->default_module);
		} else {
			ZVAL_STR(&t, YAF_KNOWN_STR(YAF_DEFAULT_MODULE));
		}
		zend_hash_index_update(Z_ARRVAL(rv), 0, &t);
	}
	zend_hash_str_update(ht, "modules:protected", sizeof("modules:protected") - 1, &rv);
	
	if (app->default_route) {
		ZVAL_ARR(&rv, zend_array_dup(app->default_route));
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_update(ht, "default_route:protected", sizeof("default_route:protected") - 1, &rv);

	return ht;
}
/* }}} */

static ZEND_COLD zend_never_inline void yaf_application_errors_hub(int type, ...) /* {{{ */ {
	va_list args;

	va_start(args, type);
	if (type == 0) {
		yaf_application_object *app = va_arg(args, yaf_application_object*);
		if (Z_TYPE(YAF_G(app)) == IS_OBJECT) {
			zend_throw_exception_ex(NULL, YAF_ERR_STARTUP_FAILED, "Only one application can be initialized");
		} else if (Z_TYPE(app->config) != IS_OBJECT) {
			zend_throw_exception_ex(NULL, YAF_ERR_STARTUP_FAILED, "Initialization of application config failed");
		} else {
			zval *pzval;
			HashTable *conf = Z_YAFCONFIGOBJ(app->config)->config;
			if ((((pzval = zend_hash_find(conf, YAF_KNOWN_STR(YAF_APPLICATION))) == NULL) || Z_TYPE_P(pzval) != IS_ARRAY) &&
					(((pzval = zend_hash_find(conf, YAF_KNOWN_STR(YAF))) == NULL) || Z_TYPE_P(pzval) != IS_ARRAY)) {
				yaf_trigger_error(YAF_ERR_TYPE_ERROR, "%s", "Expected an array of application configuration");
			} else {
				yaf_trigger_error(YAF_ERR_STARTUP_FAILED, "%s", "Expected 'directory' entry in application configuration");
			}
			zval_ptr_dtor(&app->config);
		}
	} else if (type == 1) {
		zend_class_entry *ce = va_arg(args, zend_class_entry*);
		char *bootstrap_path = va_arg(args, char*);

		if (ce) {
			yaf_trigger_error(YAF_ERR_TYPE_ERROR, "'%s' is not a subclass of %s", ZSTR_VAL(ce->name), ZSTR_VAL(yaf_bootstrap_ce->name));
		} else if (zend_hash_str_exists(&EG(included_files), bootstrap_path, strlen(bootstrap_path))) {
			php_error_docref(NULL, E_WARNING, "Couldn't find class %s in %s", YAF_DEFAULT_BOOTSTRAP, bootstrap_path);
		} else {
			php_error_docref(NULL, E_WARNING, "Couldn't find bootstrap file %s", bootstrap_path);
		}
	}
	va_end(args);
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
		GC_ADDREF(Z_OBJ(app->config));
		return Z_OBJ(app->config);
	}
	return NULL;
}
/* }}} */

static zval *yaf_application_read_property(yaf_object *obj, void *name, int type, void **cache_slot, zval *rv) /* {{{ */ {
	zend_string *member;
	yaf_application_object *app = php_yaf_application_fetch_object(yaf_strip_obj(obj));

#if PHP_VERSION_ID < 80000
	if (UNEXPECTED(Z_TYPE_P((zval*)name) != IS_STRING)) {
		return &EG(uninitialized_zval);
	}
    member = Z_STR_P((zval*)name);
#else
	member = (zend_string*)name;
#endif

	if (UNEXPECTED(type == BP_VAR_W || type == BP_VAR_RW)) {
		return &EG(error_zval);
	}

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

static YAF_WRITE_HANDLER yaf_application_write_property(yaf_object *obj, void *name, zval *value, void **cache_slot) /* {{{ */ {
	zend_string *member;
	yaf_application_object *app = php_yaf_application_fetch_object(yaf_strip_obj(obj));

#if PHP_VERSION_ID < 80000
	if (UNEXPECTED(Z_TYPE_P((zval*)name) != IS_STRING)) {
		YAF_WHANDLER_RET(value);
	}
    member = Z_STR_P((zval*)name);
#else
	member = (zend_string*)name;
#endif

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

ZEND_HOT int yaf_application_is_module_name(zend_string *name) /* {{{ */ {
	zval *pzval;
	yaf_application_object *app = yaf_application_instance();

	if (UNEXPECTED(app == NULL)) {
		return 0;
	}
	if (app->modules == NULL) {
		if (UNEXPECTED(app->default_module)) {
			return zend_string_equals_ci(app->default_module, name);
		}
		return zend_string_equals_ci(name, YAF_KNOWN_STR(YAF_DEFAULT_MODULE));
	}

	ZEND_HASH_FOREACH_VAL(app->modules, pzval) {
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

ZEND_HOT int yaf_application_is_module_name_str(const char *name, size_t len) /* {{{ */ {
	zval *pzval;
	yaf_application_object *app = yaf_application_instance();

	if (UNEXPECTED(app == NULL)) {
		return 0;
	}
	if (app->modules == NULL) {
		if (UNEXPECTED(app->default_module)) {
			return len == ZSTR_LEN(app->default_module) && !strncasecmp(name, ZSTR_VAL(app->default_module), len);
		}
		return len == strlen(YAF_KNOWN_CHARS(YAF_DEFAULT_MODULE)) &&
			!strncasecmp(name, YAF_KNOWN_CHARS(YAF_DEFAULT_MODULE), len);
	}

	ZEND_HASH_FOREACH_VAL(app->modules, pzval) {
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

static zend_never_inline void yaf_application_parse_optional(yaf_application_object *app, zend_array *conf) /* {{{ */ {
	zval *pzval, *psval;

	/* following options are optional */
	if (UNEXPECTED((pzval = zend_hash_find(conf, YAF_KNOWN_STR(YAF_BOOTSTRAP))) != NULL &&
		Z_TYPE_P(pzval) == IS_STRING)) {
		app->bootstrap = zend_string_copy(Z_STR_P(pzval));
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(conf, ZEND_STRL("ext"))) != NULL &&
		Z_TYPE_P(pzval) == IS_STRING)) {
		app->ext = zend_string_copy(Z_STR_P(pzval));
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(conf, ZEND_STRL("library"))) != NULL)) {
		if (EXPECTED(IS_STRING == Z_TYPE_P(pzval))) {
			app->library = zend_string_copy(Z_STR_P(pzval));
		} else if (IS_ARRAY == Z_TYPE_P(pzval)) {
			if ((psval = zend_hash_find(Z_ARRVAL_P(pzval), YAF_KNOWN_STR(YAF_DIRECTORY))) != NULL &&
				Z_TYPE_P(psval) == IS_STRING) {
				app->library = zend_string_copy(Z_STR_P(psval));
			}
			if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("namespace"))) != NULL) {
				yaf_loader_object *loader = Z_YAFLOADEROBJ(YAF_G(loader));
				if (Z_TYPE_P(psval) == IS_STRING) {
					if (Z_STRLEN_P(psval)) {
						zend_string *prefix;
						char *src = Z_STRVAL_P(psval), *pos;
						size_t len = Z_STRLEN_P(psval);
						while ((pos = memchr(src, ',', len))) {
							len -= (pos - src) + 1;
							while (*src == ' ') src++;
							prefix = zend_string_init(src, pos - src, 0);
							yaf_loader_register_namespace(loader, prefix, NULL);
							zend_string_release(prefix);
							src = pos + 1;
						}

						if (len) {
							while (*src == ' ') src++, len--;
							prefix = zend_string_init(src, len, 0);
							yaf_loader_register_namespace(loader, prefix, NULL);
							zend_string_release(prefix);
						}
					} 
				} else if (Z_TYPE_P(psval) == IS_ARRAY) {
					zend_string *name;
					zval *path;
					ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(psval), name, path) {
						if (name == NULL) {
							continue;
						}
						if (Z_TYPE_P(path) == IS_STRING) {
							yaf_loader_register_namespace(Z_YAFLOADEROBJ(YAF_G(loader)), name, Z_STR_P(path));
						} else {
							yaf_loader_register_namespace(Z_YAFLOADEROBJ(YAF_G(loader)), name, NULL);
						}
					} ZEND_HASH_FOREACH_END();
				}
			}
		}
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(conf, ZEND_STRL("view"))) != NULL &&
		Z_TYPE_P(pzval) == IS_ARRAY &&
		((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("ext"))) != NULL &&
		 Z_TYPE_P(psval) == IS_STRING))) {
		app->view_ext = zend_string_copy(Z_STR_P(psval));
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(conf, ZEND_STRL("baseUri"))) != NULL &&
		Z_TYPE_P(pzval) == IS_STRING)) {
		app->base_uri = zend_string_copy(Z_STR_P(pzval));
	}

	do {
		zval rv;
		if (UNEXPECTED((pzval = zend_hash_str_find(conf, ZEND_STRL("modules"))) != NULL &&
			Z_TYPE_P(pzval) == IS_STRING && Z_STRLEN_P(pzval))) {
			char *ptrptr = NULL;
			char *seg, *modules;

			ALLOC_HASHTABLE(app->modules);
			zend_hash_init(app->modules, 8, NULL, ZVAL_PTR_DTOR, 0);
			YAF_ALLOW_VIOLATION(app->modules);

			modules = estrndup(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval));
			seg = php_strtok_r(modules, ",", &ptrptr);
			while (seg) {
				if (seg && strlen(seg)) {
					ZVAL_STR(&rv, yaf_build_camel_name(seg, strlen(seg)));
					zend_hash_next_index_insert(app->modules, &rv);
				}
				seg = php_strtok_r(NULL, ",", &ptrptr);
			}
			efree(modules);
		}
	} while (0);

	if (UNEXPECTED((pzval = zend_hash_str_find(conf, ZEND_STRL("system"))) != NULL &&
		Z_TYPE_P(pzval) == IS_ARRAY)) {
		zval *value;
		char name[128];
		zend_string *key;
		size_t len;

		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pzval), key, value) {
			zend_string *str, *val;
			len = snprintf(name, sizeof(name), "%s.%s", "yaf", ZSTR_VAL(key));
			if (len > sizeof(name) -1) {
				continue;
			}
			str = zend_string_init(name, len, 0);
			val = zval_get_string(value);
			zend_alter_ini_entry(str, val, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
			zend_string_release(str);
			zend_string_release(val);
		} ZEND_HASH_FOREACH_END();

		/* We have to reset the loader as the yaf.* inis has beend changed */
		yaf_loader_reset(Z_YAFLOADEROBJ(YAF_G(loader)));
		if (*YAF_G(global_library)) {
			zend_string *library = zend_string_init(YAF_G(global_library), strlen(YAF_G(global_library)), 0);
			yaf_loader_set_global_library_path(Z_YAFLOADEROBJ(YAF_G(loader)), library);
			zend_string_release(library);
		}
	}
}
/* }}} */

int yaf_application_parse_option(yaf_application_object *app) /* {{{ */ {
	zval *pzval;
	HashTable *conf;
	uint32_t items;

	conf = Z_YAFCONFIGOBJ(app->config)->config;
	if (UNEXPECTED((pzval = zend_hash_find(conf, YAF_KNOWN_STR(YAF_APPLICATION))) == NULL) || Z_TYPE_P(pzval) != IS_ARRAY) {
		/* For back compatibilty */
		if (((pzval = zend_hash_find(conf, YAF_KNOWN_STR(YAF))) == NULL) || Z_TYPE_P(pzval) != IS_ARRAY) {
			return 0;
		}
	}

	conf = Z_ARRVAL_P(pzval);
	if (UNEXPECTED((pzval = zend_hash_find(conf, YAF_KNOWN_STR(YAF_DIRECTORY))) == NULL ||
		Z_TYPE_P(pzval) != IS_STRING || Z_STRLEN_P(pzval) == 0)) {
		return 0;
	}

	if (UNEXPECTED(*(Z_STRVAL_P(pzval) + Z_STRLEN_P(pzval) - 1) == DEFAULT_SLASH)) {
		app->directory = zend_string_init(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval) - 1, 0);
	} else {
		app->directory = zend_string_copy(Z_STR_P(pzval));
	}

	items = zend_hash_num_elements(conf) - 1;
	if (UNEXPECTED((pzval = zend_hash_find(conf, YAF_KNOWN_STR(YAF_DISPATCHER))) != NULL &&
		Z_TYPE_P(pzval) == IS_ARRAY)) {
		zval *psval;

		items--;
		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("defaultModule"))) != NULL &&
			Z_TYPE_P(psval) == IS_STRING) {
			app->default_module = yaf_canonical_name(1, Z_STR_P(psval));
		} else {
			app->default_module = YAF_KNOWN_STR(YAF_DEFAULT_MODULE);
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("defaultController"))) != NULL &&
			Z_TYPE_P(psval) == IS_STRING) {
			app->default_controller = yaf_canonical_name(1, Z_STR_P(psval));
		} else {
			app->default_controller = YAF_KNOWN_STR(YAF_DEFAULT_CONTROLLER);
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("defaultAction"))) != NULL &&
			Z_TYPE_P(psval) == IS_STRING) {
			app->default_action = yaf_canonical_name(0, Z_STR_P(psval));
		} else {
			app->default_action = YAF_KNOWN_STR(YAF_DEFAULT_ACTION);
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("throwException"))) != NULL) {
			yaf_set_throw_exception(zend_is_true(psval));
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("catchException"))) != NULL) {
			yaf_set_catch_exception(zend_is_true(psval));
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("defaultRoute"))) != NULL &&
			Z_TYPE_P(psval) == IS_ARRAY) {
			/* leave it to configs destructor */
			app->default_route = Z_ARRVAL_P(psval);
		}
	} else {
		app->default_module = YAF_KNOWN_STR(YAF_DEFAULT_MODULE);
		app->default_controller = YAF_KNOWN_STR(YAF_DEFAULT_CONTROLLER);
		app->default_action = YAF_KNOWN_STR(YAF_DEFAULT_ACTION);
	}

	/* prasing optional configs */
	if (items) {
		yaf_application_parse_optional(app, conf);
	}

	return 1;
}
/* }}} */

/** {{{ proto Yaf_Application::__construct(mixed $config, string $environ = YAF_G(environ_name))
*/
PHP_METHOD(yaf_application, __construct) {
	zval *config;
	zend_string *section = NULL;
	yaf_loader_t *loader;
	yaf_application_object *app = Z_YAFAPPOBJ_P(getThis());

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z|S", &config, &section) == FAILURE) {
		return;
	}

	if (EXPECTED(Z_TYPE(YAF_G(app)) != IS_OBJECT)) {
		if (!section || !ZSTR_LEN(section)) {
			section = zend_string_init(YAF_G(environ_name), strlen(YAF_G(environ_name)), 0);
		} else {
			zend_string_copy(section);
		}
		yaf_config_instance(&app->config, config, section);
		if (EXPECTED(Z_TYPE(app->config) == IS_OBJECT)) {
			loader = yaf_loader_instance(NULL);
			if (EXPECTED(yaf_application_parse_option(app))) {
				app->env = section /* initialized flag */;
				if (app->library == NULL) {
					zend_string *local_library = zend_string_alloc(ZSTR_LEN(app->directory) + sizeof(YAF_LIBRARY_DIRECTORY_NAME), 0);
					yaf_compose_2_pathes(ZSTR_VAL(local_library), app->directory, ZEND_STRS(YAF_LIBRARY_DIRECTORY_NAME));
					yaf_loader_set_library_path_ex(Z_YAFLOADEROBJ_P(loader), local_library);
				} else {
					yaf_loader_set_library_path(Z_YAFLOADEROBJ_P(loader), app->library);
				}

				GC_ADDREF(&app->std);
				ZVAL_OBJ(&YAF_G(app), &app->std);
				yaf_dispatcher_instance(&app->dispatcher);
				yaf_request_instance(&Z_YAFDISPATCHEROBJ(app->dispatcher)->request, app->base_uri);
				return;
			}
		}
		zend_string_release(section);
	}

	yaf_application_errors_hub(0, app);
	return;
}
/* }}} */

/** {{{ proto public Yaf_Application::run(void)
*/
PHP_METHOD(yaf_application, run) {
	yaf_application_object *app = Z_YAFAPPOBJ_P(getThis());
	yaf_response_t *response;

	if (UNEXPECTED(YAF_APP_FLAGS(app) & YAF_APP_RUNNING)) {
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED, "Application is already started");
		RETURN_FALSE;
	}

	YAF_APP_FLAGS(app) |= YAF_APP_RUNNING;
	if ((response = yaf_dispatcher_dispatch(Z_YAFDISPATCHEROBJ(app->dispatcher)))) {
		YAF_APP_FLAGS(app) &= ~YAF_APP_RUNNING;
		RETURN_ZVAL(response, 1, 0);
	}

	YAF_APP_FLAGS(app) &= ~YAF_APP_RUNNING;
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Application::bootstrap(void)
*/
PHP_METHOD(yaf_application, bootstrap) {
	zval bootstrap;
	zend_string *func;
	char buf[MAXPATHLEN];
	zend_function *fptr;
	zend_class_entry  *ce;
	const char *bootstrap_path;
	uint32_t bootstrap_path_len;
	yaf_application_object *app = Z_YAFAPPOBJ_P(getThis());
	yaf_dispatcher_t *dispatcher = &app->dispatcher;

	if (!(ce = zend_hash_find_ptr(EG(class_table), YAF_KNOWN_STR(YAF_BOOTSTRAP)))) {
		if (UNEXPECTED(app->bootstrap)) {
			bootstrap_path = ZSTR_VAL(app->bootstrap);
			bootstrap_path_len = ZSTR_LEN(app->bootstrap);
		} else {
			bootstrap_path_len = yaf_compose_2_pathes(buf, app->directory, ZEND_STRL(YAF_DEFAULT_BOOTSTRAP));
			buf[bootstrap_path_len++] = '.';
			if (UNEXPECTED(app->ext)) {
				memcpy(buf + bootstrap_path_len, ZSTR_VAL(app->ext), ZSTR_LEN(app->ext));
				bootstrap_path_len += ZSTR_LEN(app->ext);
			} else {
				memcpy(buf + bootstrap_path_len, YAF_DEFAULT_EXT, sizeof(YAF_DEFAULT_EXT) - 1);
				bootstrap_path_len += sizeof(YAF_DEFAULT_EXT) - 1;
			}
			buf[bootstrap_path_len] = '\0';
			bootstrap_path = buf;
		}
		if (UNEXPECTED((!yaf_loader_import(bootstrap_path, bootstrap_path_len)) ||
			(!(ce = zend_hash_find_ptr(EG(class_table), YAF_KNOWN_STR(YAF_BOOTSTRAP)))))) {
			goto error;
		}
	}

	if (EXPECTED(instanceof_function(ce, yaf_bootstrap_ce))) {
		zend_object *obj;

		object_init_ex(&bootstrap, ce);
		obj = Z_OBJ(bootstrap);
		ZEND_HASH_FOREACH_STR_KEY_PTR(&(ce->function_table), func, fptr) {
			zval ret;
			if (UNEXPECTED(ZSTR_LEN(func) < (sizeof("_init")) - 1) ||
				!yaf_slip_equal(ZSTR_VAL(func), ZEND_STRL(YAF_BOOTSTRAP_INITFUNC_PREFIX))) {
				continue;
			}
			if (UNEXPECTED(!yaf_call_user_method_with_1_arguments(obj, fptr, dispatcher, &ret))) {
				/** an uncaught exception threw in function call */
				if (UNEXPECTED(EG(exception))) {
					OBJ_RELEASE(Z_OBJ(bootstrap));
					RETURN_FALSE;
				}
			}
			/* Must always return bool? */
			/* zval_ptr_dtor(&ret); */
		} ZEND_HASH_FOREACH_END();
		OBJ_RELEASE(Z_OBJ(bootstrap));

		RETURN_ZVAL(getThis(), 1, 0);
	}

error:
	yaf_application_errors_hub(1, ce, bootstrap_path);
	RETURN_FALSE;
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

/** {{{ proto public Yaf_Application::app(void)
*/
PHP_METHOD(yaf_application, app) {
	RETURN_ZVAL(&YAF_G(app), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Application::getModules(void)
*/
PHP_METHOD(yaf_application, getModules) {
	yaf_application_object *app = Z_YAFAPPOBJ_P(getThis());

	if (app->modules) {
		GC_ADDREF(app->modules);
		RETURN_ARR(app->modules);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/** {{{ proto public Yaf_Application::environ(void)
*/
PHP_METHOD(yaf_application, environ) {
	yaf_application_object *app = Z_YAFAPPOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	if (app->env) {
		RETURN_STR(zend_string_copy(app->env));
	}
	RETURN_EMPTY_STRING();
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

/** {{{ proto public Yaf_Application::getLastErrorNo(void)
*/
PHP_METHOD(yaf_application, getLastErrorNo) {
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_LONG(Z_YAFAPPOBJ_P(getThis())->err_no);
}
/* }}} */

/** {{{ proto public Yaf_Application::getLastErrorMsg(void)
*/
PHP_METHOD(yaf_application, getLastErrorMsg) {
	yaf_application_object *app = Z_YAFAPPOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
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

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
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
	yaf_application_object *app = Z_YAFAPPOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (app->directory) {
		RETURN_STR(zend_string_copy(app->directory));
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/** {{{ yaf_application_methods
*/
zend_function_entry yaf_application_methods[] = {
	PHP_ME(yaf_application, __construct, arginfo_class_Yaf_Application___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(yaf_application, run, arginfo_class_Yaf_Application_run, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, execute, arginfo_class_Yaf_Application_execute, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, app, arginfo_class_Yaf_Application_app, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	YAF_ME(yaf_application_environ, "environ", arginfo_class_Yaf_Application_environ, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, bootstrap, arginfo_class_Yaf_Application_bootstrap, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, getConfig, arginfo_class_Yaf_Application_getConfig, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, getModules, arginfo_class_Yaf_Application_getModules, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, getDispatcher, arginfo_class_Yaf_Application_getDispatcher, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, setAppDirectory, arginfo_class_Yaf_Application_setAppDirectory, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, getAppDirectory, arginfo_class_Yaf_Application_getAppDirectory, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, getLastErrorNo, arginfo_class_Yaf_Application_getLastErrorNo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, getLastErrorMsg, arginfo_class_Yaf_Application_getLastErrorMsg, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_application, clearLastError, arginfo_class_Yaf_Application_clearLastError, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_application, getInstance, app, arginfo_class_Yaf_Application_app, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(application) {
	zend_class_entry ce;
	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Application", "Yaf\\Application", yaf_application_methods);

	yaf_application_ce = zend_register_internal_class_ex(&ce, NULL);
	yaf_application_ce->create_object = yaf_application_new;
#if PHP_VERSION_ID < 80100
	yaf_application_ce->ce_flags |= ZEND_ACC_FINAL;
	yaf_application_ce->serialize = zend_class_serialize_deny;
	yaf_application_ce->unserialize = zend_class_unserialize_deny;
#elif PHP_VERSION_ID < 80200
	yaf_application_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NOT_SERIALIZABLE;
#else
	yaf_application_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NOT_SERIALIZABLE | ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;
#endif

	memcpy(&yaf_application_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_application_obj_handlers.offset = XtOffsetOf(yaf_application_object, std);
	yaf_application_obj_handlers.clone_obj = NULL;
	yaf_application_obj_handlers.get_gc = yaf_application_get_gc;
	yaf_application_obj_handlers.free_obj = yaf_application_free;
	yaf_application_obj_handlers.get_properties = yaf_application_get_properties;
	yaf_application_obj_handlers.read_property = (zend_object_read_property_t)yaf_application_read_property;
	yaf_application_obj_handlers.write_property = (zend_object_write_property_t)yaf_application_write_property;

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
