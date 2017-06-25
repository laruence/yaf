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
#include "main/SAPI.h" /* for sapi_module */
#include "Zend/zend_interfaces.h" /* for zend_call_method_with_* */
#include "Zend/zend_exceptions.h" /* for zend_exception_get_default */

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_dispatcher.h"
#include "yaf_controller.h"
#include "yaf_action.h"
#include "yaf_application.h"
#include "yaf_view.h"
#include "yaf_response.h"
#include "yaf_loader.h"
#include "yaf_router.h"
#include "yaf_request.h"
#include "yaf_config.h"
#include "yaf_plugin.h"
#include "yaf_exception.h"

zend_class_entry *yaf_dispatcher_ce;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_dispatch_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, request)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_seterrhdler_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, callback)
	ZEND_ARG_INFO(0, error_types)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_flush_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_regplugin_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, plugin)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_setrequest_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, request)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_throwex_arginfo, 0, 0, 0)
    ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_catchex_arginfo, 0, 0, 0)
    ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_autorender_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_returnresp_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_initview_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, templates_dir)
	ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_setview_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, view)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_setctrl_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, controller)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_setmodule_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, module)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_setaction_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, action)
ZEND_END_ARG_INFO()

/* }}} */

yaf_dispatcher_t *yaf_dispatcher_instance(yaf_dispatcher_t *this_ptr) /* {{{ */ {
	zval plugins;
	yaf_router_t *router, rv = {{0}};
	yaf_dispatcher_t *instance;

	instance = zend_read_static_property(yaf_dispatcher_ce, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_INSTANCE), 1);

	if (IS_OBJECT == Z_TYPE_P(instance)
			&& instanceof_function(Z_OBJCE_P(instance), yaf_dispatcher_ce)) {
		return instance;
	}

	if (Z_ISUNDEF_P(this_ptr)) {
		object_init_ex(this_ptr, yaf_dispatcher_ce);
	} else {
		return this_ptr;
	}

	array_init(&plugins);
	zend_update_property(yaf_dispatcher_ce, this_ptr, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_PLUGINS), &plugins);
	zval_ptr_dtor(&plugins);

	router = yaf_router_instance(&rv);
	zend_update_property(yaf_dispatcher_ce, this_ptr, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_ROUTER), router);
	zval_ptr_dtor(router);

	zend_update_property_str(yaf_dispatcher_ce,
			this_ptr, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_MODULE), YAF_G(default_module));
	zend_update_property_str(yaf_dispatcher_ce,
			this_ptr, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_CONTROLLER), YAF_G(default_controller));
	zend_update_property_str(yaf_dispatcher_ce,
			this_ptr, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_ACTION), YAF_G(default_action));
	zend_update_static_property(yaf_dispatcher_ce, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_INSTANCE), this_ptr);

	return this_ptr;
}
/* }}} */

static void yaf_dispatcher_get_call_parameters(zend_class_entry *request_ce, yaf_request_t *request, zend_function *fptr, zval **params, uint *count) /* {{{ */ {
	zval          *args, *arg;
	zend_arg_info *arg_info;
	uint           current;
	HashTable 	  *params_ht;

	args = zend_read_property(request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_PARAMS), 1, NULL);

	params_ht = Z_ARRVAL_P(args);
	arg_info  = fptr->common.arg_info;
	*params   = safe_emalloc(sizeof(zval), fptr->common.num_args, 0);
	for (current = 0; current < fptr->common.num_args; current++, arg_info++) {
		if ((arg = zend_hash_find(params_ht, arg_info->name)) != NULL) {
			ZVAL_COPY_VALUE(&((*params)[current]), arg);
			(*count)++;
		} else {
			zend_string *key;

			arg  = NULL;
			/* since we need search ignoring case, can't use zend_hash_find */
			ZEND_HASH_FOREACH_STR_KEY_VAL(params_ht, key, arg) {
                if (key) {
					if (zend_string_equals(key, arg_info->name)) {
							/* return when we find first match, there is a trap
							 * when multi different parameters in different case presenting in params_ht
							 * only the first take affect
							 */
							ZVAL_COPY_VALUE(&((*params)[current]), arg);
							(*count)++;
							break;
					}
				}
			} ZEND_HASH_FOREACH_END();

			if (NULL == arg) {
				break;
			}
		}
	}
}
/* }}} */

static yaf_view_t *yaf_dispatcher_init_view(yaf_dispatcher_t *dispatcher, zval *tpl_dir, zval *options, yaf_view_t *ptr) /* {{{ */ {
	yaf_view_t *view = zend_read_property(yaf_dispatcher_ce,
			dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_VIEW), 1, NULL);

	if (EXPECTED(IS_OBJECT == Z_TYPE_P(view)
			&& instanceof_function(Z_OBJCE_P(view), yaf_view_interface_ce))) {
		return view;
	}

	view = yaf_view_instance(ptr, tpl_dir, options);
	if (!view) {
		return NULL;
	}

	zend_update_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_VIEW), view);
	zval_ptr_dtor(view);

	return view;
}
/* }}} */

static inline void yaf_dispatcher_fix_default(yaf_dispatcher_t *dispatcher, yaf_request_t *request) /* {{{ */ {
	zval *module, *controller, *action;

	module = zend_read_property(yaf_request_ce,
			request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), 1, NULL);
	action = zend_read_property(yaf_request_ce,
			request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), 1, NULL);
	controller = zend_read_property(yaf_request_ce, request,
			ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), 1, NULL);

	if (Z_TYPE_P(module) != IS_STRING || !Z_STRLEN_P(module)) {
		zval *default_module = zend_read_property(yaf_dispatcher_ce,
				dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_MODULE), 1, NULL);
		zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), default_module);
	} else {
		char *p = zend_str_tolower_dup(Z_STRVAL_P(module), Z_STRLEN_P(module));
		*p = toupper(*p);
		zend_update_property_stringl(yaf_request_ce,
				request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), p, Z_STRLEN_P(module));
		efree(p);
	}

	if (Z_TYPE_P(controller) != IS_STRING || !Z_STRLEN_P(controller)) {
		zval *default_controller = zend_read_property(yaf_dispatcher_ce,
				dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_CONTROLLER), 1, NULL);
		zend_update_property(yaf_request_ce,
				request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), default_controller);
	} else {
		char *q, *p = zend_str_tolower_dup(Z_STRVAL_P(controller), Z_STRLEN_P(controller));
		/**
		 * Upper controller name
		 * eg: Index_sub -> Index_Sub
		 */
		q = p;
		*q = toupper(*q);
		while (*q != '\0') {
			if (*q == '_' || *q == '\\') {
			   	if (*(q+1) != '\0') {
					*(q+1) = toupper(*(q+1));
					q++;
				}
			}
			q++;
		}

		zend_update_property_stringl(yaf_request_ce, request,
				ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), p, Z_STRLEN_P(controller));
		efree(p);
	}

	if (Z_TYPE_P(action) != IS_STRING || !Z_STRLEN_P(action)) {
		zval *default_action = zend_read_property(yaf_dispatcher_ce,
				dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_ACTION), 1, NULL);
		zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), default_action);
	} else {
		char *p = zend_str_tolower_dup(Z_STRVAL_P(action), Z_STRLEN_P(action));
		zend_update_property_stringl(yaf_request_ce,
				request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), p, Z_STRLEN_P(action));
		efree(p);
	}
}
/* }}} */

int yaf_dispatcher_set_request(yaf_dispatcher_t *dispatcher, yaf_request_t *request) /* {{{ */ {
	if (request) {
		zend_update_property(yaf_dispatcher_ce,
				dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_REQUEST), request);
		return 1;
	}
	return 0;
}
/* }}} */

zend_class_entry *yaf_dispatcher_get_controller(zend_string *app_dir, zend_string *module, zend_string *controller, int def_module) /* {{{ */ {
	char *directory;
	size_t directory_len;

	if (def_module) {
		directory_len = spprintf(&directory, 0,
				"%s%c%s", ZSTR_VAL(app_dir), DEFAULT_SLASH, YAF_CONTROLLER_DIRECTORY_NAME);
	} else {
		directory_len = spprintf(&directory, 0,
				"%s%c%s%c%s%c%s", ZSTR_VAL(app_dir), DEFAULT_SLASH, YAF_MODULE_DIRECTORY_NAME,
				DEFAULT_SLASH, ZSTR_VAL(module), DEFAULT_SLASH, YAF_CONTROLLER_DIRECTORY_NAME);
	}

	if (EXPECTED(directory_len)) {
		zend_string *class;
		zend_string *class_lowercase;
		zend_class_entry *ce 	= NULL;

		if (YAF_G(name_suffix)) {
			class = strpprintf(0, "%s%s%s", ZSTR_VAL(controller), YAF_G(name_separator), "Controller");
		} else {
			class = strpprintf(0, "%s%s%s", "Controller", YAF_G(name_separator), ZSTR_VAL(controller));
		}

		class_lowercase = zend_string_tolower(class);

		if ((ce = zend_hash_find_ptr(EG(class_table), class_lowercase)) == NULL) {
			if (!yaf_internal_autoload(ZSTR_VAL(controller), ZSTR_LEN(controller), &directory)) {
				yaf_trigger_error(YAF_ERR_NOTFOUND_CONTROLLER,
						"Failed opening controller script %s: %s", directory, strerror(errno));
				zend_string_release(class);
				zend_string_release(class_lowercase);
				efree(directory);
				return NULL;
			} else if ((ce = zend_hash_find_ptr(EG(class_table), class_lowercase)) == NULL)  {
				yaf_trigger_error(YAF_ERR_AUTOLOAD_FAILED,
						"Could not find class %s in controller script %s", ZSTR_VAL(class), directory);
				zend_string_release(class);
				zend_string_release(class_lowercase);
				efree(directory);
				return 0;
			} else if (!instanceof_function(ce, yaf_controller_ce)) {
				yaf_trigger_error(YAF_ERR_TYPE_ERROR,
						"Controller must be an instance of %s", ZSTR_VAL(yaf_controller_ce->name));
				zend_string_release(class);
				zend_string_release(class_lowercase);
				efree(directory);
				return 0;
			}
		}

		zend_string_release(class);
		zend_string_release(class_lowercase);
		efree(directory);

		return ce;
	}

	return NULL;
}
/* }}} */

zend_class_entry *yaf_dispatcher_get_action(zend_string *app_dir, yaf_controller_t *controller, char *module, int def_module, zend_string *action) /* {{{ */ {
	zval *paction, *actions_map;
	actions_map = zend_read_property(Z_OBJCE_P(controller),
			controller, ZEND_STRL(YAF_CONTROLLER_PROPERTY_NAME_ACTIONS), 1, NULL);

	if (EXPECTED(IS_ARRAY == Z_TYPE_P(actions_map))) {
		zend_class_entry *ce;
		zend_string *class;
		zend_string *class_lowercase;
		char *action_upper = estrndup(ZSTR_VAL(action), ZSTR_LEN(action));

		*(action_upper) = toupper(*action_upper);

		if (YAF_G(name_suffix)) {
			class = strpprintf(0, "%s%s%s", action_upper, YAF_G(name_separator), "Action");
		} else {
			class = strpprintf(0, "%s%s%s", "Action", YAF_G(name_separator), action_upper);
		}

		class_lowercase = zend_string_tolower(class);

		if ((ce = zend_hash_find_ptr(EG(class_table), class_lowercase)) != NULL) {
			efree(action_upper);
			zend_string_release(class_lowercase);
			if (instanceof_function(ce, yaf_action_ce)) {
				zend_string_release(class);
				return ce;
			} else {
				yaf_trigger_error(YAF_ERR_TYPE_ERROR,
						"Action %s must extends from %s", ZSTR_VAL(class), ZSTR_VAL(yaf_action_ce->name));
				zend_string_release(class);
				return NULL;
			}
		}

		if ((paction = zend_hash_find(Z_ARRVAL_P(actions_map), action)) != NULL) {
			zend_string *action_path;

			action_path = strpprintf(0, "%s%c%s", ZSTR_VAL(app_dir), DEFAULT_SLASH, Z_STRVAL_P(paction));
			if (yaf_loader_import(action_path, 0)) {
				if ((ce = zend_hash_find_ptr(EG(class_table), class_lowercase)) != NULL) {
					zend_string_release(action_path);
					efree(action_upper);
					zend_string_release(class_lowercase);

					if (instanceof_function(ce, yaf_action_ce)) {
						zend_string_release(class);
						return ce;
					} else {
						yaf_trigger_error(YAF_ERR_TYPE_ERROR,
								"Action %s must extends from %s", ZSTR_VAL(class), ZSTR_VAL(yaf_action_ce->name));
						zend_string_release(class);
					}

				} else {
					yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION,
							"Could not find action %s in %s", ZSTR_VAL(class), ZSTR_VAL(action_path));
				}

				zend_string_release(action_path);
				efree(action_upper);
				zend_string_release(class);
				zend_string_release(class_lowercase);
			} else {
				yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION,
						"Failed opening action script %s: %s", ZSTR_VAL(action_path), strerror(errno));
				zend_string_release(action_path);
			}
		} else {
			yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION, "There is no method %s%s in %s::$%s", ZSTR_VAL(action),
					"Action", ZSTR_VAL(Z_OBJCE_P(controller)->name), YAF_CONTROLLER_PROPERTY_NAME_ACTIONS);
		}
	} else
/* {{{ This only effects internally */
	   	if (YAF_G(st_compatible)) {
		char *directory, *class, *class_lowercase, *p;
		uint class_len;
		zend_class_entry *ce;
		char *action_upper = estrndup(ZSTR_VAL(action), ZSTR_LEN(action));

		/**
		 * upper Action Name
		 * eg: Index_sub -> Index_Sub
		 */
		p = action_upper;
		*(p) = toupper(*p);
		while (*p != '\0') {
			if (*p == '_' || *p == '\\') {
				if (*(p+1) != '\0') {
					*(p+1) = toupper(*(p+1));
					p++;
				}
			}
			p++;
		}

		if (def_module) {
			spprintf(&directory, 0, "%s%c%s", ZSTR_VAL(app_dir), DEFAULT_SLASH, "actions");
		} else {
			spprintf(&directory, 0, "%s%c%s%c%s%c%s", ZSTR_VAL(app_dir), DEFAULT_SLASH,
					"modules", DEFAULT_SLASH, module, DEFAULT_SLASH, "actions");
		}

		if (YAF_G(name_suffix)) {
			class_len = spprintf(&class, 0, "%s%s%s", action_upper, YAF_G(name_separator), "Action");
		} else {
			class_len = spprintf(&class, 0, "%s%s%s", "Action", YAF_G(name_separator), action_upper);
		}

		class_lowercase = zend_str_tolower_dup(class, class_len);

		if ((ce = zend_hash_str_find_ptr(EG(class_table), class_lowercase, class_len)) == NULL) {
			if (!yaf_internal_autoload(action_upper, ZSTR_LEN(action), &directory)) {
				yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION, "Failed opening action script %s: %s", directory, strerror(errno));

				efree(class);
				efree(action_upper);
				efree(class_lowercase);
				efree(directory);
				return NULL;
			} else if ((ce = zend_hash_str_find_ptr(EG(class_table), class_lowercase, class_len)) == NULL)  {
				yaf_trigger_error(YAF_ERR_AUTOLOAD_FAILED, "Could find class %s in action script %s", class, directory);

				efree(class);
				efree(action_upper);
				efree(class_lowercase);
				efree(directory);
				return NULL;
			} else if (!instanceof_function(ce, yaf_action_ce)) {
				yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Action must be an instance of %s", ZSTR_VAL(yaf_action_ce->name));

				efree(class);
				efree(action_upper);
				efree(class_lowercase);
				efree(directory);
				return NULL;
			}
		}

		efree(class);
		efree(action_upper);
		efree(class_lowercase);
		efree(directory);

		return ce;
	} else
/* }}} */
	{
		yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION,
				"There is no method %s%s in %s", ZSTR_VAL(action), "Action", ZSTR_VAL(Z_OBJCE_P(controller)->name));
	}

	return NULL;
}
/* }}} */

int yaf_dispatcher_handle(yaf_dispatcher_t *dispatcher, yaf_request_t *request,  yaf_response_t *response, yaf_view_t *view) /* {{{ */ {
	zend_class_entry *request_ce;
	zend_string *app_dir = YAF_G(directory);

	request_ce = Z_OBJCE_P(request);

	yaf_request_set_dispatched(request, 1);
	if (!app_dir) {
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED,
				"%s requires %s(which set the application.directory) to be initialized first",
				ZSTR_VAL(yaf_dispatcher_ce->name), ZSTR_VAL(yaf_application_ce->name));
		return 0;
	} else {
		int	is_def_module = 0;
		zval *module, *controller, *dmodule, *instantly_flush;
		zend_class_entry *ce;
		yaf_controller_t *executor;
		zend_function *fptr;

		module = zend_read_property(request_ce,
				request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), 1, NULL);
		controller = zend_read_property(request_ce,
				request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), 1, NULL);
		dmodule	= zend_read_property(yaf_dispatcher_ce,
				dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_MODULE), 1, NULL);

		if (UNEXPECTED(Z_TYPE_P(module) != IS_STRING || !Z_STRLEN_P(module))) {
			yaf_trigger_error(YAF_ERR_DISPATCH_FAILED, "Unexcepted a empty module name");
			return 0;
		} else if (UNEXPECTED(!yaf_application_is_module_name(Z_STR_P(module)))) {
			yaf_trigger_error(YAF_ERR_NOTFOUND_MODULE, "There is no module %s", Z_STRVAL_P(module));
			return 0;
		}

		if (UNEXPECTED(Z_TYPE_P(controller) != IS_STRING || !Z_STRLEN_P(controller))) {
			yaf_trigger_error(YAF_ERR_DISPATCH_FAILED, "Unexcepted a empty controller name");
			return 0;
		}

		if (zend_string_equals(Z_STR_P(dmodule), Z_STR_P(module))) {
			is_def_module = 1;
		}

		ce = yaf_dispatcher_get_controller(app_dir, Z_STR_P(module), Z_STR_P(controller), is_def_module);
		if (!ce) {
			return 0;
		} else {
			zval ret;
			zval action, *render;
			zend_string *view_dir;
			zend_string *func_name;
			yaf_controller_t icontroller;

			object_init_ex(&icontroller, ce);

			yaf_controller_construct(ce, &icontroller, request, response, view, NULL);

			if (EG(exception)) {
				zval_ptr_dtor(&icontroller);
				return 0;
			}

			if (!yaf_request_is_dispatched(request)) {
				/* forward is called in init method */
				zval_ptr_dtor(&icontroller);
				return yaf_dispatcher_handle(dispatcher, request, response, view TSRMLS_CC);
			}
		
			/* view template directory for application, please notice that view engine's directory has high priority */
			if (is_def_module) {
				view_dir = strpprintf(0, "%s%c%s", ZSTR_VAL(app_dir), DEFAULT_SLASH, "views");
			} else {
				view_dir = strpprintf(0, "%s%c%s%c%s%c%s", ZSTR_VAL(app_dir),
						DEFAULT_SLASH, "modules", DEFAULT_SLASH, Z_STRVAL_P(module), DEFAULT_SLASH, "views");
			}

			if (YAF_G(view_directory)) {
				zend_string_release(YAF_G(view_directory));
			}
			YAF_G(view_directory) = view_dir;

			zend_update_property(ce, &icontroller, ZEND_STRL(YAF_CONTROLLER_PROPERTY_NAME_NAME), controller);

			ZVAL_COPY(&action,
					zend_read_property(request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), 1, NULL));

			func_name = strpprintf(0, "%s%s", Z_STRVAL(action), "action");
			zend_str_tolower(ZSTR_VAL(func_name), ZSTR_LEN(func_name));

			/* @TODO: Magic __call supports? */
			if ((fptr = zend_hash_find_ptr(&((ce)->function_table), func_name)) != NULL) {
				uint count = 0;
				zval *call_args = NULL;
				executor = &icontroller;
				if (fptr->common.num_args) {
					zval method_name;

					yaf_dispatcher_get_call_parameters(request_ce, request, fptr, &call_args, &count);
					ZVAL_STR(&method_name, func_name);
					call_user_function_ex(&(ce)->function_table,
							&icontroller, &method_name, &ret, count, call_args, 1, NULL);
					efree(call_args);
				} else {
					zend_call_method(&icontroller, ce,
							NULL, ZSTR_VAL(func_name), ZSTR_LEN(func_name), &ret, 0, NULL, NULL);
				}

				zend_string_release(func_name);

				if (Z_ISUNDEF(ret)) {
					zval_ptr_dtor(&action);
					zval_ptr_dtor(executor);
					return 0;
				}

				if ((Z_TYPE(ret) == IS_FALSE)) {
					/* no auto-renderring */
					zval_ptr_dtor(&action);
					zval_ptr_dtor(executor);
					return 1;
				}
				zval_ptr_dtor(&ret);
			} else if ((ce = yaf_dispatcher_get_action(app_dir,
							&icontroller, Z_STRVAL_P(module), is_def_module, Z_STR(action))) &&
					(fptr = zend_hash_str_find_ptr(&(ce->function_table),
								YAF_ACTION_EXECUTOR_NAME, sizeof(YAF_ACTION_EXECUTOR_NAME) - 1))) {
				zval *call_args;
				yaf_action_t iaction;
				uint count = 0;

				zend_string_release(func_name);

				object_init_ex(&iaction, ce);

				yaf_controller_construct(ce, &iaction, request, response, view, NULL);
				executor = &iaction;

				zend_update_property(ce, &iaction, ZEND_STRL(YAF_CONTROLLER_PROPERTY_NAME_NAME), controller);
				zend_update_property(ce, &iaction, ZEND_STRL(YAF_ACTION_PROPERTY_NAME_CTRL), &icontroller);
				zval_ptr_dtor(&icontroller);

				if (fptr->common.num_args) {
					zval method_name;

					yaf_dispatcher_get_call_parameters(request_ce, request, fptr, &call_args, &count);
					ZVAL_STRINGL(&method_name, YAF_ACTION_EXECUTOR_NAME, sizeof(YAF_ACTION_EXECUTOR_NAME) - 1);
					call_user_function_ex(&(ce)->function_table,
							&iaction, &method_name, &ret, count, call_args, 1, NULL);
					zval_ptr_dtor(&method_name);
					efree(call_args);
				} else {
					zend_call_method_with_0_params(&iaction, ce, NULL, "execute", &ret);
				}

				if (Z_ISUNDEF(ret)) {
					zval_ptr_dtor(&action);
					zval_ptr_dtor(&iaction);
					return 0;
				}

				if ((Z_TYPE(ret) == IS_FALSE)) {
					/* no auto-renderring */
					zval_ptr_dtor(&action);
					zval_ptr_dtor(&iaction);
					return 1;
				}
				zval_ptr_dtor(&ret);
			} else {
				zend_string_release(func_name);
				zval_ptr_dtor(&icontroller);
				return 0;
			}

			if (executor) {
				/* controller's property can override the Dispatcher's */
				zval rv;
				int auto_render;
				render = zend_read_property(ce, executor, ZEND_STRL(YAF_CONTROLLER_PROPERTY_NAME_RENDER), 1, &rv);
				if (render == &EG(uninitialized_zval)) {
					render = zend_read_property(yaf_dispatcher_ce,
							dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_RENDER), 1, NULL);
				} 
				auto_render = (Z_TYPE_P(render) == IS_TRUE || (Z_TYPE_P(render) == IS_LONG && Z_LVAL_P(render)));

				instantly_flush	= zend_read_property(yaf_dispatcher_ce,
						dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_FLUSH), 1, NULL);
				if (auto_render) {
					if (Z_TYPE_P(instantly_flush) == IS_FALSE) {
						zend_call_method_with_1_params(executor, ce, NULL, "render", &ret, &action);
						zval_ptr_dtor(executor);

						if (UNEXPECTED(Z_ISUNDEF(ret))) {
							zval_ptr_dtor(&action);
							return 0;
						} else if (IS_FALSE == Z_TYPE(ret)) {
							zval_ptr_dtor(&action);
							return 0;
						}

						if (Z_TYPE(ret) == IS_STRING && Z_STRLEN(ret)) {
							yaf_response_alter_body(response, NULL, Z_STR(ret), YAF_RESPONSE_APPEND );
						} 

						zval_ptr_dtor(&ret);
					} else {
						zend_call_method_with_1_params(executor, ce, NULL, "display", &ret, &action);
						zval_ptr_dtor(executor);

						if (UNEXPECTED(Z_ISUNDEF(ret))) {
							zval_ptr_dtor(&action);
							return 0;
						}

						if ((Z_TYPE(ret) == IS_FALSE)) {
							zval_ptr_dtor(&action);
							return 0;
						}
						zval_ptr_dtor(&ret);
					}
				} else {
					zval_ptr_dtor(executor);
				}
			}
			zval_ptr_dtor(&action);
		}
		return 1;
	}
	return 0;
}
/* }}} */

void yaf_dispatcher_exception_handler(yaf_dispatcher_t *dispatcher, yaf_request_t *request, yaf_response_t *response) /* {{{ */ {
	zval *module, controller, action, exception;
	yaf_view_t *view, rv = {{0}};
	zend_string *exception_str;
	const zend_op *opline;

	if (YAF_G(in_exception) || !EG(exception)) {
		return;
	}

	YAF_G(in_exception) = 1;

	module = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), 1, NULL);

	if (Z_TYPE_P(module) != IS_STRING || !Z_STRLEN_P(module)) {
		module = zend_read_property(yaf_dispatcher_ce,
				dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_MODULE), 1, NULL);
		zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), module);
	}

	ZVAL_STRING(&controller, YAF_ERROR_CONTROLLER);
	ZVAL_STRING(&action, YAF_ERROR_ACTION);

	ZVAL_OBJ(&exception, EG(exception));
	EG(exception) = NULL;
	opline = EG(opline_before_exception);
#if ZEND_DEBUG 
	EG(opline_before_exception) = NULL;
#endif

	zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), &controller);
	zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), &action);
	zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_EXCEPTION), &exception);

	zval_ptr_dtor(&controller);
	zval_ptr_dtor(&action);

	/** use $request->getException() instand of */
	exception_str = zend_string_init(ZEND_STRL("exception"), 0);
	if (yaf_request_set_params_single(request, exception_str, &exception)) {
		zend_string_release(exception_str);
		zval_ptr_dtor(&exception);
	} else {
		/* failover to uncaught exception */
		zend_string_release(exception_str);
		EG(exception) = Z_OBJ(exception);
		return;
	}
	yaf_request_set_dispatched(request, 0);

	view = yaf_dispatcher_init_view(dispatcher, NULL, NULL, &rv);

	if (UNEXPECTED(!view)) {
		return;
	}

	if (!yaf_dispatcher_handle(dispatcher, request, response, view)) {
		if (EG(exception) &&
			instanceof_function(EG(exception)->ce,
				yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(YAF_ERR_NOTFOUND_CONTROLLER)])) {
			zval *m = zend_read_property(yaf_dispatcher_ce,
					dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_MODULE), 1, NULL);
			/* failover to default module error catcher */
			zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), m);
			EG(exception) = NULL;
			(void)yaf_dispatcher_handle(dispatcher, request, response, view);
		}
	}

	(void)yaf_response_send(response);

	EG(opline_before_exception) = opline;
	YAF_EXCEPTION_ERASE_EXCEPTION();
}
/* }}} */

int yaf_dispatcher_route(yaf_dispatcher_t *dispatcher, yaf_request_t *request) /* {{{ */ {
	zval ret;
	zend_class_entry *router_ce;
	yaf_router_t *router = zend_read_property(yaf_dispatcher_ce,
			dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_ROUTER), 1, NULL);
	if (EXPECTED(IS_OBJECT == Z_TYPE_P(router))) {
		if (EXPECTED((router_ce = Z_OBJCE_P(router)) == yaf_router_ce)) {
			/* use built-in route */
			if (yaf_router_route(router, request)) {
				return 1;
			}
		} else {
			/* user custom route */
			zend_call_method_with_1_params(router, router_ce, NULL, "route", &ret, request);
			if (Z_TYPE(ret) != IS_TRUE) {
				yaf_trigger_error(YAF_ERR_ROUTE_FAILED, "Routing request faild");
				zval_ptr_dtor(&ret);
				return 0;
			}
			zval_ptr_dtor(&ret);
		}
		return 1;
	}
	return 0;
}
/* }}} */

yaf_response_t *yaf_dispatcher_dispatch(yaf_dispatcher_t *dispatcher, zval *response_ptr) /* {{{ */ {
	zval *return_response, *plugins, *view, rv;
	yaf_response_t *response;
	yaf_request_t *request;
	uint nesting = YAF_G(forward_limit);

	response = yaf_response_instance(response_ptr, sapi_module.name);
	request	= zend_read_property(yaf_dispatcher_ce,
			dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_REQUEST), 1, NULL);
	plugins	= zend_read_property(yaf_dispatcher_ce,
			dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_PLUGINS), 1, NULL);

	if (UNEXPECTED(IS_OBJECT != Z_TYPE_P(request))) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Expect a %s instance", ZSTR_VAL(yaf_request_ce->name));
		zval_ptr_dtor(response);
		return NULL;
	}

	/* route request */
	if (!yaf_request_is_routed(request)) {
		YAF_PLUGIN_HANDLE(plugins, YAF_PLUGIN_HOOK_ROUTESTARTUP, request, response);
		YAF_EXCEPTION_HANDLE(dispatcher, request, response);
		if (!yaf_dispatcher_route(dispatcher, request)) {
			yaf_trigger_error(YAF_ERR_ROUTE_FAILED, "Routing request failed");
			YAF_EXCEPTION_HANDLE_NORET(dispatcher, request, response);
			zval_ptr_dtor(response);
			return NULL;
		}
		yaf_dispatcher_fix_default(dispatcher, request);
		YAF_PLUGIN_HANDLE(plugins, YAF_PLUGIN_HOOK_ROUTESHUTDOWN, request, response);
		YAF_EXCEPTION_HANDLE(dispatcher, request, response);
		(void)yaf_request_set_routed(request, 1);
	} else {
		yaf_dispatcher_fix_default(dispatcher, request);
	}

	YAF_PLUGIN_HANDLE(plugins, YAF_PLUGIN_HOOK_LOOPSTARTUP, request, response);
	YAF_EXCEPTION_HANDLE(dispatcher, request, response);

	ZVAL_UNDEF(&rv);
	view = yaf_dispatcher_init_view(dispatcher, NULL, NULL, &rv);
	if (!view) {
		return NULL;
	}

	do {
		YAF_PLUGIN_HANDLE(plugins, YAF_PLUGIN_HOOK_PREDISPATCH, request, response);
		YAF_EXCEPTION_HANDLE(dispatcher, request, response);
		if (!yaf_dispatcher_handle(dispatcher, request, response, view)) {
			YAF_EXCEPTION_HANDLE(dispatcher, request, response);
			zval_ptr_dtor(response);
			return NULL;
		}
		yaf_dispatcher_fix_default(dispatcher, request);
		YAF_PLUGIN_HANDLE(plugins, YAF_PLUGIN_HOOK_POSTDISPATCH, request, response);
		YAF_EXCEPTION_HANDLE(dispatcher, request, response);
	} while (--nesting > 0 && !yaf_request_is_dispatched(request));

	YAF_PLUGIN_HANDLE(plugins, YAF_PLUGIN_HOOK_LOOPSHUTDOWN, request, response);
	YAF_EXCEPTION_HANDLE(dispatcher, request, response);

	if (0 == nesting && !yaf_request_is_dispatched(request)) {
		yaf_trigger_error(YAF_ERR_DISPATCH_FAILED, "The max dispatch nesting %ld was reached", YAF_G(forward_limit));
		YAF_EXCEPTION_HANDLE_NORET(dispatcher, request, response);
		zval_ptr_dtor(response);
		return NULL;
	}

	return_response = zend_read_property(yaf_dispatcher_ce,
			dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_RETURN), 1, NULL);

	if (Z_TYPE_P(return_response) == IS_FALSE) {
		zval ret;
		zend_call_method_with_0_params(response, Z_OBJCE_P(response), NULL, "response", &ret);
		/* it must return bool */
		/* zval_ptr_dtor(&ret); */
		yaf_response_clear_body(response, NULL);
	}

	return response;
}
/* }}} */

/** {{{ proto private Yaf_Dispatcher::__construct(void)
*/
PHP_METHOD(yaf_dispatcher, __construct) {
}
/* }}} */

/** {{{ proto private Yaf_Dispatcher::__sleep(void)
*/
PHP_METHOD(yaf_dispatcher, __sleep) {
}
/* }}} */

/** {{{ proto private Yaf_Dispatcher::__wakeup(void)
*/
PHP_METHOD(yaf_dispatcher, __wakeup) {
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setErrorHandler(string $callbacak[, int $error_types = E_ALL | E_STRICT ] )
*/
PHP_METHOD(yaf_dispatcher, setErrorHandler) {
	zval *callback, *error_type = NULL;
	zval params[2];
	zval function = {{0}};

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z", &callback, &error_type) == FAILURE) {
		return;
	}

	ZVAL_COPY(&params[0], callback);
	if (error_type) {
		ZVAL_COPY(&params[1], error_type);
	}

	ZVAL_STRING(&function, "set_error_handler");
	if (call_user_function(EG(function_table), NULL, &function, return_value, ZEND_NUM_ARGS(), params) == FAILURE) {
		zval_ptr_dtor(return_value);
		zval_ptr_dtor(&params[0]);
		if (error_type) {
			zval_ptr_dtor(&params[1]);
		}
		zval_ptr_dtor(&function);
		php_error_docref(NULL, E_WARNING, "Call to set_error_handler failed");
		RETURN_FALSE;
	}

	zval_ptr_dtor(return_value);
	zval_ptr_dtor(&function);
	zval_ptr_dtor(&params[0]);
	if (error_type) {
		zval_ptr_dtor(&params[1]);
	}

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::disableView()
*/
PHP_METHOD(yaf_dispatcher, disableView) {
	yaf_dispatcher_t *self = getThis();
	zend_update_property_bool(yaf_dispatcher_ce, self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_RENDER), 0);
	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::enableView()
*/
PHP_METHOD(yaf_dispatcher, enableView) {
	yaf_dispatcher_t *self = getThis();
	zend_update_property_bool(yaf_dispatcher_ce, self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_RENDER), 1);
	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::returnResponse()
*/
PHP_METHOD(yaf_dispatcher, returnResponse) {
	zend_bool auto_response;
	yaf_dispatcher_t *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &auto_response) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS()) {
		zend_update_property_bool(yaf_dispatcher_ce,
				self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_RETURN), auto_response);
		RETURN_ZVAL(self, 1, 0);
	} else {
		RETURN_BOOL(Z_TYPE_P(zend_read_property(yaf_dispatcher_ce,
						self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_RETURN), 1, NULL)) == IS_TRUE ? 1 : 0);
	}
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::flushInstantly(bool $flag)
*/
PHP_METHOD(yaf_dispatcher, flushInstantly) {
	zend_bool instantly_flush;
	yaf_dispatcher_t *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &instantly_flush) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS()) {
		zend_update_property_bool(yaf_dispatcher_ce,
				self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_FLUSH), instantly_flush);
		RETURN_ZVAL(self, 1, 0);
	} else {
		RETURN_BOOL(Z_TYPE_P(zend_read_property(yaf_dispatcher_ce,
						self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_FLUSH), 1, NULL)) == IS_TRUE ? 1 : 0);		
	}
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::registerPlugin(Yaf_Plugin_Abstract $plugin)
*/
PHP_METHOD(yaf_dispatcher, registerPlugin) {
	zval *plugin, *plugins;
	yaf_dispatcher_t *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &plugin) == FAILURE) {
		return;
	}

	if (EXPECTED(Z_TYPE_P(plugin) != IS_OBJECT ||
		!instanceof_function(Z_OBJCE_P(plugin), yaf_plugin_ce))) {
		php_error_docref(NULL, E_WARNING, "Expect a %s instance", ZSTR_VAL(yaf_plugin_ce->name));
		RETURN_FALSE;
	}

	plugins = zend_read_property(yaf_dispatcher_ce, self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_PLUGINS), 1, NULL);

	Z_ADDREF_P(plugin);
	add_next_index_zval(plugins, plugin);

	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setRequest(Yaf_Request_Abstract $request)
*/
PHP_METHOD(yaf_dispatcher, setRequest) {
	yaf_request_t	 *request;
	yaf_dispatcher_t *self;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &request) == FAILURE) {
		return;
	}

	if (IS_OBJECT != Z_TYPE_P(request) ||
		!instanceof_function(Z_OBJCE_P(request), yaf_request_ce))	{
		php_error_docref(NULL, E_WARNING, "Expects a %s instance", ZSTR_VAL(yaf_request_ce->name));
		RETURN_FALSE;
	}

	self = getThis();
	if (yaf_dispatcher_set_request(self, request)) {
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::getInstance(void)
*/
PHP_METHOD(yaf_dispatcher, getInstance) {
	zval *instance, rv = {{0}};
	if ((instance = yaf_dispatcher_instance(&rv)) != NULL) {
		RETURN_ZVAL(instance, 1, 0);
	}
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::getRouter(void)
*/
PHP_METHOD(yaf_dispatcher, getRouter) {
	yaf_router_t *router = zend_read_property(yaf_dispatcher_ce,
			getThis(), ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_ROUTER), 1, NULL);
	RETURN_ZVAL(router, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::getRequest(void)
*/
PHP_METHOD(yaf_dispatcher, getRequest) {
	yaf_request_t *request = zend_read_property(yaf_dispatcher_ce,
			getThis(), ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_REQUEST), 1, NULL);
	RETURN_ZVAL(request, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::getApplication(void)
*/
PHP_METHOD(yaf_dispatcher, getApplication) {
	PHP_MN(yaf_application_app)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::dispatch(yaf_request_t $request)
*/
PHP_METHOD(yaf_dispatcher, dispatch) {
	yaf_request_t 	*request;
	yaf_response_t 	*response, rresponse;
	yaf_dispatcher_t *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &request, yaf_request_ce) == FAILURE) {
		return;
	}

	zend_update_property(yaf_dispatcher_ce, self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_REQUEST), request);
	ZVAL_NULL(&rresponse);
	if ((response = yaf_dispatcher_dispatch(self, &rresponse))) {
		RETURN_ZVAL(response, 1, 1);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::throwException(bool $flag=0)
*/
PHP_METHOD(yaf_dispatcher, throwException) {
	zend_bool flag;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &flag) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS()) {
		YAF_G(throw_exception) = flag? 1: 0;
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		RETURN_BOOL(YAF_G(throw_exception));
	}
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::catchException(bool $flag=0)
*/
PHP_METHOD(yaf_dispatcher, catchException) {
	zend_bool flag;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &flag) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS()) { 
		YAF_G(catch_exception) = flag? 1: 0;
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		RETURN_BOOL(YAF_G(catch_exception));
	}
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::autoRender(int $flag)
 */
PHP_METHOD(yaf_dispatcher, autoRender) {
	zend_bool flag;
	yaf_dispatcher_t *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &flag) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS()) {
		zend_update_property_bool(yaf_dispatcher_ce, self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_RENDER), flag? 1 : 0);
		RETURN_ZVAL(self, 1, 0);
	} else {
		RETURN_BOOL(Z_TYPE_P(zend_read_property(yaf_dispatcher_ce,
						self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_RENDER), 1, NULL)) == IS_TRUE ? 1 : 0);
	} 
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::initView(string $tpl_dir, array $options = NULL)
*/
PHP_METHOD(yaf_dispatcher, initView) {
	yaf_view_t *view, rv = {{0}};
	zval *tpl_dir, *options = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z", &tpl_dir, &options) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	view = yaf_dispatcher_init_view(getThis(), tpl_dir, options, &rv);

	RETURN_ZVAL(view, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setView(Yaf_View_Interface $view)
*/
PHP_METHOD(yaf_dispatcher, setView) {
	yaf_view_t		*view;
	yaf_dispatcher_t 	*self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &view) == FAILURE) {
		return;
	}

	if (IS_OBJECT == Z_TYPE_P(view)
			&& instanceof_function(Z_OBJCE_P(view), yaf_view_interface_ce)) {
		zend_update_property(yaf_dispatcher_ce, self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_VIEW), view);
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setDefaultModule(string $name)
*/
PHP_METHOD(yaf_dispatcher, setDefaultModule) {
	zval *module;
	zval *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &module) == FAILURE) {
		return;
	}

	if (IS_STRING == Z_TYPE_P(module) && Z_STRLEN_P(module)
			&& yaf_application_is_module_name(Z_STR_P(module))) {
		zval module_std;
		ZVAL_STRING(&module_std, zend_str_tolower_dup(Z_STRVAL_P(module), Z_STRLEN_P(module)));
		*Z_STRVAL(module_std) = toupper(*Z_STRVAL(module_std));
		zend_update_property(yaf_dispatcher_ce, self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_MODULE), &module_std);
		zval_ptr_dtor(&module_std);

		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setDefaultController(string $name)
*/
PHP_METHOD(yaf_dispatcher, setDefaultController) {
	zval *controller;
	zval *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &controller) == FAILURE) {
		return;
	}

	if (IS_STRING == Z_TYPE_P(controller) && Z_STRLEN_P(controller)) {
		zval controller_std;
		ZVAL_STRING(&controller_std, zend_str_tolower_dup(Z_STRVAL_P(controller), Z_STRLEN_P(controller)));
		*Z_STRVAL_P(&controller_std) = toupper(*Z_STRVAL(controller_std));
		zend_update_property(yaf_dispatcher_ce, self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_CONTROLLER), &controller_std);

		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setDefaultAction(string $name)
*/
PHP_METHOD(yaf_dispatcher, setDefaultAction) {
	zval *action;
	zval *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &action) == FAILURE) {
		return;
	}

	if (IS_STRING == Z_TYPE_P(action) && Z_STRLEN_P(action)) {
		zval action_lower;
		ZVAL_STRING(&action_lower, zend_str_tolower_dup(Z_STRVAL_P(action), Z_STRLEN_P(action)));
		zend_update_property(yaf_dispatcher_ce, self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_ACTION), &action_lower);
		zval_ptr_dtor(&action_lower);

		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::__desctruct(void)
*/
PHP_METHOD(yaf_dispatcher, __destruct) {
}
/* }}} */

/** {{{ proto private Yaf_Dispatcher::__clone(void)
*/
PHP_METHOD(yaf_dispatcher, __clone) {
}
/* }}} */

/** {{{ yaf_dispatcher_methods
*/
zend_function_entry yaf_dispatcher_methods[] = {
	PHP_ME(yaf_dispatcher, __construct, 			NULL, 					ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(yaf_dispatcher, __clone,				NULL, 					ZEND_ACC_PRIVATE)
	PHP_ME(yaf_dispatcher, __sleep,				NULL, 					ZEND_ACC_PRIVATE)
	PHP_ME(yaf_dispatcher, __wakeup,			NULL, 					ZEND_ACC_PRIVATE)
	PHP_ME(yaf_dispatcher, enableView,			yaf_dispatcher_void_arginfo,  		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, disableView,			yaf_dispatcher_void_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, initView,			yaf_dispatcher_initview_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setView,				yaf_dispatcher_setview_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setRequest,			yaf_dispatcher_setrequest_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, getApplication,			yaf_dispatcher_void_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, getRouter,			yaf_dispatcher_void_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, getRequest,			yaf_dispatcher_void_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setErrorHandler,			yaf_dispatcher_seterrhdler_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setDefaultModule,		yaf_dispatcher_setmodule_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setDefaultController, 		yaf_dispatcher_setctrl_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setDefaultAction,		yaf_dispatcher_setaction_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, returnResponse,			yaf_dispatcher_returnresp_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, autoRender,			yaf_dispatcher_autorender_arginfo,	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, flushInstantly,			yaf_dispatcher_flush_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, getInstance,			yaf_dispatcher_void_arginfo, 		ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(yaf_dispatcher, dispatch,			yaf_dispatcher_dispatch_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, throwException,			yaf_dispatcher_throwex_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, catchException,			yaf_dispatcher_catchex_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, registerPlugin,			yaf_dispatcher_regplugin_arginfo, 	ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(dispatcher) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Dispatcher", "Yaf\\Dispatcher", yaf_dispatcher_methods);

	yaf_dispatcher_ce = zend_register_internal_class_ex(&ce, NULL);
	yaf_dispatcher_ce->ce_flags |= ZEND_ACC_FINAL;

	zend_declare_property_null(yaf_dispatcher_ce, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_ROUTER), 	ZEND_ACC_PROTECTED);
	zend_declare_property_null(yaf_dispatcher_ce, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_VIEW), 	ZEND_ACC_PROTECTED);
	zend_declare_property_null(yaf_dispatcher_ce, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_REQUEST), 	ZEND_ACC_PROTECTED);
	zend_declare_property_null(yaf_dispatcher_ce, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_PLUGINS), 	ZEND_ACC_PROTECTED);

	zend_declare_property_null(yaf_dispatcher_ce, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_INSTANCE), ZEND_ACC_PROTECTED|ZEND_ACC_STATIC);

	zend_declare_property_bool(yaf_dispatcher_ce, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_RENDER),	1,  ZEND_ACC_PROTECTED);
	zend_declare_property_bool(yaf_dispatcher_ce, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_RETURN),   0, ZEND_ACC_PROTECTED);
	zend_declare_property_bool(yaf_dispatcher_ce, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_FLUSH), 	0, ZEND_ACC_PROTECTED);

	zend_declare_property_null(yaf_dispatcher_ce, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_MODULE), 		ZEND_ACC_PROTECTED);
	zend_declare_property_null(yaf_dispatcher_ce, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_CONTROLLER), 	ZEND_ACC_PROTECTED);
	zend_declare_property_null(yaf_dispatcher_ce, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_ACTION), 	 	ZEND_ACC_PROTECTED);

	/*zend_declare_property_null(yaf_dispatcher_ce, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_ARGS), 	 	ZEND_ACC_PROTECTED);*/

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
