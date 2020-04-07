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

zend_class_entry    *yaf_dispatcher_ce;
zend_object_handlers yaf_dispatcher_obj_handlers;

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

static void yaf_dispatcher_obj_free(zend_object *object) /* {{{ */ {
	yaf_dispatcher_object *dispatcher = (yaf_dispatcher_object*)object;

	zval_ptr_dtor(&dispatcher->request);
	zval_ptr_dtor(&dispatcher->response);
	zval_ptr_dtor(&dispatcher->router);
	zval_ptr_dtor(&dispatcher->view);
	zend_hash_destroy(&dispatcher->plugins);

	zend_object_std_dtor(object);
}
/* }}} */

void yaf_dispatcher_instance(yaf_dispatcher_t *this_ptr) /* {{{ */ {
	yaf_application_object *app = Z_YAFAPPOBJ(YAF_G(app));
	yaf_dispatcher_object *dispatcher;

	if (IS_OBJECT != Z_TYPE(app->dispatcher)) {
		dispatcher = emalloc(sizeof(yaf_dispatcher_object));
		zend_object_std_init(&dispatcher->std, yaf_dispatcher_ce);
		dispatcher->std.handlers = &yaf_dispatcher_obj_handlers;

		zend_hash_init(&dispatcher->plugins, 8, NULL, ZVAL_PTR_DTOR, 0);
		memset(&dispatcher->request, 0, sizeof(yaf_dispatcher_object) - sizeof(zend_object) - sizeof(zend_array));

		yaf_router_instance(&dispatcher->router);
		dispatcher->auto_render = 1;

		ZVAL_OBJ(&app->dispatcher, &dispatcher->std);
		return;
	}

	ZVAL_COPY(this_ptr, &app->dispatcher);
}
/* }}} */

static void yaf_dispatcher_get_call_parameters(yaf_request_object *request, zend_function *fptr, zval **params, unsigned *count) /* {{{ */ {
	zval          *arg;
	zend_arg_info *arg_info;
	unsigned       current;
	HashTable 	  *params_ht;

	params_ht = &request->params;

	arg_info = fptr->common.arg_info;
	*params = safe_emalloc(sizeof(zval), fptr->common.num_args, 0);
	for (current = 0; current < fptr->common.num_args; current++, arg_info++) {
		if ((arg = zend_hash_find(params_ht, arg_info->name)) != NULL) {
			ZVAL_COPY_VALUE(&((*params)[current]), arg);
			(*count)++;
		} else {
			/* we should not make parameters gaps */
			break;
		}
	}
}
/* }}} */

static int yaf_dispatcher_init_view(yaf_dispatcher_object *dispatcher, zend_string *tpl_dir, zval *options) /* {{{ */ {
	/*FIXME multiply re-instance?*/
	yaf_view_t *view = &dispatcher->view;

	if (EXPECTED(IS_OBJECT == Z_TYPE_P(view) &&
		instanceof_function(Z_OBJCE_P(view), yaf_view_interface_ce))) {
		return 1;
	}

	zval_ptr_dtor(&dispatcher->view);
	yaf_view_instance(&dispatcher->view, tpl_dir, options);

	return Z_TYPE(dispatcher->view) == IS_OBJECT;
}
/* }}} */

static inline void yaf_dispatcher_fix_default(yaf_dispatcher_object *dispatcher, yaf_request_object *request) /* {{{ */ {
	zend_string *p;
	yaf_application_object *app = Z_YAFAPPOBJ(YAF_G(app));

	if (!request->module) {
		request->module = zend_string_copy(app->default_module);
	} else {
		p = zend_string_init(ZSTR_VAL(request->module), ZSTR_LEN(request->module), 0);
		ZSTR_VAL(p)[0] = toupper(ZSTR_VAL(p)[0]);
		zend_str_tolower(ZSTR_VAL(p) + 1, ZSTR_LEN(p) - 1);
		zend_string_release(request->module);
		request->module = p;
	}

	if (!request->controller) {
		request->controller = zend_string_copy(app->default_controller);
	} else {
		char *q;
		p = zend_string_init(ZSTR_VAL(request->controller), ZSTR_LEN(request->controller), 0);
		zend_str_tolower(ZSTR_VAL(p), ZSTR_LEN(p));
		/**
		 * Upper controller name
		 * eg: Index_sub -> Index_Sub
		 */
		q = ZSTR_VAL(p);
		*q = toupper(*q);
		while (*q != '\0') {
			if (*q == '_' || *q == '\\') {
			   	if (*(q + 1) != '\0') {
					*(q + 1) = toupper(*(q+1));
					q++;
				}
			}
			q++;
		}
		zend_string_release(request->controller);
		request->controller = p;
	}

	if (!request->action) {
		request->action = zend_string_copy(app->default_action);
	} else {
		p = zend_string_tolower(request->action);
		zend_string_release(request->action);
		request->action = p;
	}
}
/* }}} */

int yaf_dispatcher_set_request(yaf_dispatcher_object *dispatcher, yaf_request_t *request) /* {{{ */ {
	if (EXPECTED(request)) {
		zval garbage;
		ZVAL_COPY_VALUE(&garbage, &dispatcher->request);
		ZVAL_COPY(&dispatcher->request, request);
		zval_ptr_dtor(&garbage);
		return 1;
	}
	return 0;
}
/* }}} */

zend_class_entry *yaf_dispatcher_get_controller(zend_string *app_dir, yaf_request_object *request, int def_module) /* {{{ */ {
	char directory[MAXPATHLEN];
	size_t directory_len;
	zend_class_entry *ce;
	zend_string *lc_name;
	zend_string *controller = request->controller;
	zend_string *module = request->module;

	if (def_module) {
		directory_len = snprintf(directory, sizeof(directory),
				"%s%c%s", ZSTR_VAL(app_dir), DEFAULT_SLASH, YAF_CONTROLLER_DIRECTORY_NAME);
	} else {
		directory_len = snprintf(directory, sizeof(directory),
				"%s%c%s%c%s%c%s", ZSTR_VAL(app_dir), DEFAULT_SLASH, YAF_MODULE_DIRECTORY_NAME,
				DEFAULT_SLASH, ZSTR_VAL(module), DEFAULT_SLASH, YAF_CONTROLLER_DIRECTORY_NAME);
	}

	if (directory_len >= sizeof(directory)) {
		yaf_trigger_error(YAF_ERR_AUTOLOAD_FAILED, "path too long %s: %s", directory);
		return NULL;
	}

	lc_name = zend_string_alloc(ZSTR_LEN(controller) + YAF_G(name_separator_len) + sizeof("Controller") - 1, 0);
	if (EXPECTED(YAF_G(name_suffix))) {
		char *p = ZSTR_VAL(lc_name);
		zend_str_tolower_copy(p, ZSTR_VAL(controller), ZSTR_LEN(controller));
		p += ZSTR_LEN(controller);
		if (UNEXPECTED(YAF_G(name_separator_len))) {
			zend_str_tolower_copy(p, YAF_G(name_separator), YAF_G(name_separator_len));
			p += YAF_G(name_separator_len);
		}
		memcpy(p, "controller", sizeof("controller"));
	} else {
		char *p = ZSTR_VAL(lc_name);
		memcpy(p, "controller", sizeof("controller") - 1);
		p += sizeof("controller") - 1;
		if (UNEXPECTED(YAF_G(name_separator_len))) {
			zend_str_tolower_copy(p, YAF_G(name_separator), YAF_G(name_separator_len));
			p += YAF_G(name_separator_len);
		}
		zend_str_tolower_copy(p, ZSTR_VAL(controller), ZSTR_LEN(controller) + 1);
	}

	if ((ce = (zend_class_entry*)zend_hash_find_ptr(EG(class_table), lc_name)) == NULL) {
		if (!yaf_loader_load(NULL, ZSTR_VAL(controller), ZSTR_LEN(controller), directory, directory_len)) {
			yaf_trigger_error(YAF_ERR_NOTFOUND_CONTROLLER,
					"Failed opening controller script %s: %s", directory, strerror(errno));
			zend_string_release(lc_name);
			return NULL;
		} else if ((ce = zend_hash_find_ptr(EG(class_table), lc_name)) == NULL)  {
			zend_string_release(lc_name);
			if (EXPECTED(YAF_G(name_suffix))) {
				yaf_trigger_error(YAF_ERR_AUTOLOAD_FAILED,
					"Could not find class %s%s%s in controller script %s",
					ZSTR_VAL(controller), YAF_G(name_separator), "Controller", directory);
			} else {
				yaf_trigger_error(YAF_ERR_AUTOLOAD_FAILED,
					"Could not find class %s%s%s in controller script %s",
					"Controller", YAF_G(name_separator), ZSTR_VAL(controller), directory);
			}
			return 0;
		} else if (!instanceof_function(ce, yaf_controller_ce)) {
			yaf_trigger_error(YAF_ERR_TYPE_ERROR,
					"Controller must be an instance of %s", ZSTR_VAL(yaf_controller_ce->name));
			zend_string_release(lc_name);
			return 0;
		}
	}

	zend_string_release(lc_name);

	return ce;

}
/* }}} */

zend_class_entry *yaf_dispatcher_get_action(zend_string *app_dir, yaf_controller_t *controller, yaf_request_object *request) /* {{{ */ {
	zval *pzval;
	zval *actions_map;
	zend_string *action = request->action;

	actions_map = zend_read_property(Z_OBJCE_P(controller), controller, ZEND_STRL(YAF_CONTROLLER_PROPERTY_NAME_ACTIONS), 1, NULL);

	ZVAL_DEREF(actions_map);

	if (EXPECTED(IS_ARRAY == Z_TYPE_P(actions_map))) {
		zend_class_entry *ce;
		zend_string *lc_name;

		lc_name = zend_string_alloc(ZSTR_LEN(action) + YAF_G(name_separator_len) + sizeof("Action") - 1, 0);
		if (EXPECTED(YAF_G(name_suffix))) {
			char *p = ZSTR_VAL(lc_name);
			zend_str_tolower_copy(p, ZSTR_VAL(action), ZSTR_LEN(action));
			p += ZSTR_LEN(action);
			if (UNEXPECTED(YAF_G(name_separator_len))) {
				zend_str_tolower_copy(p, YAF_G(name_separator), YAF_G(name_separator_len));
				p += YAF_G(name_separator_len);
			}
			memcpy(p, "action", sizeof("action"));
		} else {
			char *p = ZSTR_VAL(lc_name);
			memcpy(p, "action", sizeof("action"));
			p += sizeof("action") - 1;
			if (UNEXPECTED(YAF_G(name_separator_len))) {
				zend_str_tolower_copy(p, YAF_G(name_separator), YAF_G(name_separator_len));
				p += YAF_G(name_separator_len);
			}
			zend_str_tolower_copy(p, ZSTR_VAL(action), ZSTR_LEN(action) + 1);
		}

		if ((ce = zend_hash_find_ptr(EG(class_table), lc_name)) != NULL) {
			zend_string_release(lc_name);
			if (instanceof_function(ce, yaf_action_ce)) {
				return ce;
			} else {
				yaf_trigger_error(YAF_ERR_TYPE_ERROR,
						"Action %s must extends from %s", ZSTR_VAL(action), ZSTR_VAL(yaf_action_ce->name));
				return NULL;
			}
		}

		if ((pzval = zend_hash_find(Z_ARRVAL_P(actions_map), action)) != NULL) {
			zend_string *action_path;
			ZVAL_DEREF(pzval);

			action_path = strpprintf(0, "%s%c%s", ZSTR_VAL(app_dir), DEFAULT_SLASH, Z_STRVAL_P(pzval));
			if (yaf_loader_import(ZSTR_VAL(action_path), ZSTR_LEN(action_path))) {
				if ((ce = zend_hash_find_ptr(EG(class_table), lc_name)) != NULL) {
					if (instanceof_function(ce, yaf_action_ce)) {
						zend_string_release(action_path);
						zend_string_release(lc_name);
						return ce;
					} else {
						yaf_trigger_error(YAF_ERR_TYPE_ERROR,
								"Action %s must extends from %s", ZSTR_VAL(action), ZSTR_VAL(yaf_action_ce->name));
					}
				} else {
					yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION,
							"Could not find action %s in %s", ZSTR_VAL(action), ZSTR_VAL(action_path));
				}
			} else {
				yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION,
						"Failed opening action script %s: %s", ZSTR_VAL(action_path), strerror(errno));
			}
			zend_string_release(action_path);
		} else {
			yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION, "There is no method %s%s in %s::$%s", ZSTR_VAL(action),
					"Action", ZSTR_VAL(Z_OBJCE_P(controller)->name), YAF_CONTROLLER_PROPERTY_NAME_ACTIONS);
		}
	} else {
		yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION,
				"There is no method %s%s in %s", ZSTR_VAL(action), "Action", ZSTR_VAL(Z_OBJCE_P(controller)->name));
	}

	return NULL;
}
/* }}} */

int yaf_dispatcher_handle(yaf_dispatcher_object *dispatcher) /* {{{ */ {
	yaf_application_object *app = Z_YAFAPPOBJ(YAF_G(app));

	yaf_request_set_dispatched(Z_YAFREQUESTOBJ(dispatcher->request), 1);
	if (!app->directory) {
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED,
				"%s requires %s(which set the application.directory) to be initialized first",
				ZSTR_VAL(yaf_dispatcher_ce->name), ZSTR_VAL(yaf_application_ce->name));
		return 0;
	} else {
		int is_def_module = 0;
		zend_class_entry *ce;
		yaf_request_object *request = Z_YAFREQUESTOBJ(dispatcher->request);

		if (UNEXPECTED(request->module == NULL)) {
			yaf_trigger_error(YAF_ERR_DISPATCH_FAILED, "Unexcepted a empty module name");
			return 0;
		} else if (UNEXPECTED(!yaf_application_is_module_name(request->module))) {
			yaf_trigger_error(YAF_ERR_NOTFOUND_MODULE, "There is no module %s", ZSTR_VAL(request->module));
			return 0;
		}

		if (UNEXPECTED(request->controller == NULL)) {
			yaf_trigger_error(YAF_ERR_DISPATCH_FAILED, "Unexcepted a empty controller name");
			return 0;
		}

		if (zend_string_equals(app->default_module, request->module)) {
			is_def_module = 1;
		}

		if ((ce = yaf_dispatcher_get_controller(app->directory, request, is_def_module))) {
			zval ret;
			zend_string *view_dir;
			zend_function *fptr;
			zend_string *func_name;
			zend_string *origin_action;
			yaf_controller_t controller;
			yaf_controller_object *ctl;

			object_init_ex(&controller, ce);

			ctl = Z_YAFCTLOBJ(controller);

			yaf_controller_init(ctl, &dispatcher->request, &dispatcher->response, &dispatcher->view, NULL);
			if (EG(exception)) {
				zval_ptr_dtor(&controller);
				return 0;
			}

			if (!yaf_request_is_dispatched(Z_YAFREQUESTOBJ(dispatcher->request))) {
				/* forward is called in init method */
				zval_ptr_dtor(&controller);
				return yaf_dispatcher_handle(dispatcher);
			}

			yaf_view_get_tpl_dir(view_dir, &dispatcher->view, &dispatcher->request);
			if (EXPECTED(view_dir == NULL)) {
				/* view template directory for application, please notice that view engine's directory has high priority */
				if (is_def_module) {
					view_dir = strpprintf(0, "%s%c%s", ZSTR_VAL(app->directory), DEFAULT_SLASH, "views");
				} else {
					view_dir = strpprintf(0, "%s%c%s%c%s%c%s", ZSTR_VAL(app->directory),
							DEFAULT_SLASH, "modules", DEFAULT_SLASH, ZSTR_VAL(request->module), DEFAULT_SLASH, "views");
				}

				yaf_view_set_tpl_dir(&dispatcher->view, view_dir);
				zend_string_release(view_dir);
			}

			origin_action = zend_string_copy(request->action); /* Action maybe changed while controller executing by forward() */
			yaf_controller_set_module_name(ctl, request->module);

			func_name = strpprintf(0, "%s%s", ZSTR_VAL(request->action), "action");
			/* @TODO: Magic __call supports? */
			if ((fptr = (zend_function*)zend_hash_find_ptr(&((ce)->function_table), func_name)) != NULL) {
				unsigned count = 0;
				zval *call_args;
				if (UNEXPECTED(fptr->common.num_args)) {
					zval method_name;

					yaf_dispatcher_get_call_parameters(Z_YAFREQUESTOBJ(dispatcher->request), fptr, &call_args, &count);
					ZVAL_STR(&method_name, func_name);
					call_user_function_ex(&(ce)->function_table, &controller, &method_name, &ret, count, call_args, 1, NULL);
					efree(call_args);
				} else {
					zend_call_method(&controller, ce, NULL, ZSTR_VAL(func_name), ZSTR_LEN(func_name), &ret, 0, NULL, NULL);
				}

				zend_string_release(func_name);

				if (Z_ISUNDEF(ret)) {
					zend_string_release(origin_action);
					zval_ptr_dtor(&controller);
					return 0;
				}

				if ((Z_TYPE(ret) == IS_FALSE)) {
					/* no auto-renderring */
					zend_string_release(origin_action);
					zval_ptr_dtor(&controller);
					return 1;
				}
				zval_ptr_dtor(&ret);
			} else if ((ce = yaf_dispatcher_get_action(app->directory, &controller, request)) &&
					(fptr = zend_hash_str_find_ptr(&(ce->function_table), ZEND_STRL(YAF_ACTION_EXECUTOR_NAME)))) {
				zval *call_args;
				yaf_action_t action;
				yaf_controller_object *act;
				unsigned count = 0;

				zend_string_release(func_name);

				object_init_ex(&action, ce);
				act = Z_YAFCTLOBJ(action);

				yaf_controller_init(act, &dispatcher->request, &dispatcher->response, &dispatcher->view, NULL);

				yaf_action_init(act, &controller, request->action);
				
				/* for convenientaly call method below */
				GC_DELREF(Z_OBJ(controller));
				ZVAL_COPY_VALUE(&controller, &action);
				ctl = act;

				if (UNEXPECTED(fptr->common.num_args)) {
					zval method_name;

					yaf_dispatcher_get_call_parameters(request, fptr, &call_args, &count);
					ZVAL_STRINGL(&method_name, YAF_ACTION_EXECUTOR_NAME, sizeof(YAF_ACTION_EXECUTOR_NAME) - 1);
					call_user_function_ex(&(ce)->function_table, &action, &method_name, &ret, count, call_args, 1, NULL);
					zval_ptr_dtor(&method_name);
					efree(call_args);
				} else {
					zend_call_method_with_0_params(&action, ce, NULL, "execute", &ret);
				}

				if (Z_ISUNDEF(ret)) {
					zend_string_release(origin_action);
					zval_ptr_dtor(&action);
					return 0;
				}

				if ((Z_TYPE(ret) == IS_FALSE)) {
					/* no auto-renderring */
					zend_string_release(origin_action);
					zval_ptr_dtor(&action);
					return 1;
				}
				zval_ptr_dtor(&ret);
			} else {
				zend_string_release(origin_action);
				zend_string_release(func_name);
				zval_ptr_dtor(&controller);
				return 0;
			}

			if (yaf_controller_auto_render(ctl, dispatcher->auto_render)) {
				zval res;
				if ((yaf_controller_render(&controller, origin_action, NULL, dispatcher->instantly_flush? NULL : &res))) {
					if (!dispatcher->instantly_flush) {
						ZEND_ASSERT(Z_TYPE(res) == IS_STRING);
						yaf_response_alter_body(Z_YAFRESPONSEOBJ(dispatcher->response), NULL, Z_STR(res), YAF_RESPONSE_APPEND );
						zend_string_release(Z_STR(res));
					}
				} else {
					zend_string_release(origin_action);
					zval_ptr_dtor(&controller);
					return 0;
				}
			}

			zend_string_release(origin_action);
			zval_ptr_dtor(&controller);
			return 1;
		}
	}

	return 0;
}
/* }}} */

void yaf_dispatcher_exception_handler(yaf_dispatcher_object *dispatcher) /* {{{ */ {
	zend_string *exception_str, *controller, *action;
	zval exception;
	const zend_op *opline;
	yaf_request_object *request = Z_YAFREQUESTOBJ(dispatcher->request);
	yaf_response_object *response = Z_YAFRESPONSEOBJ(dispatcher->response);

	if (YAF_G(in_exception) || !EG(exception)) {
		return;
	}

	YAF_G(in_exception) = 1;

	ZVAL_OBJ(&exception, EG(exception));
	EG(exception) = NULL;
	opline = EG(opline_before_exception);
#if ZEND_DEBUG
	EG(opline_before_exception) = NULL;
#endif

	controller = zend_string_init(ZEND_STRL(YAF_ERROR_CONTROLLER), 0);
	action = zend_string_init(ZEND_STRL(YAF_ERROR_ACTION), 0);

	yaf_request_set_mvc(request, NULL, controller, action, NULL);

	zend_string_release(controller);
	zend_string_release(action);

	/** use $request->getException() instand of */
	exception_str = zend_string_init(ZEND_STRL("exception"), 0);
	if (yaf_request_set_params_single(request, exception_str, &exception)) {
		zval_ptr_dtor(&exception);
		zend_string_release(exception_str);
	} else {
		/* failover to uncaught exception */
		zend_string_release(exception_str);
		EG(exception) = Z_OBJ(exception);
		return;
	}
	yaf_request_set_dispatched(request, 0);

	if (UNEXPECTED(!yaf_dispatcher_init_view(dispatcher, NULL, NULL))) {
		yaf_request_clean_params(request);
		return;
	}

	if (!yaf_dispatcher_handle(dispatcher)) {
		if (EG(exception) &&
			instanceof_function(EG(exception)->ce,
				yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(YAF_ERR_NOTFOUND_CONTROLLER)])) {
			zend_string_release(request->module);
			request->module = zend_string_copy(Z_YAFAPPOBJ(YAF_G(app))->default_module);
			/* failover to default module error catcher */
			zend_clear_exception();
			yaf_dispatcher_handle(dispatcher);
		}
	}

	yaf_request_clean_params(request);
	yaf_response_send(response);

	EG(opline_before_exception) = opline;
	YAF_EXCEPTION_ERASE_EXCEPTION();
}
/* }}} */

int yaf_dispatcher_route(yaf_dispatcher_object *dispatcher) /* {{{ */ {
	yaf_router_object *router = Z_YAFROUTEROBJ(dispatcher->router);

	/* use built-in route */
	if (yaf_router_route(router, &dispatcher->request)) {
		return 1;
	}

	return 0;
}
/* }}} */

yaf_response_t *yaf_dispatcher_dispatch(yaf_dispatcher_object *dispatcher) /* {{{ */ {
	yaf_request_object *request;
	unsigned nesting = YAF_G(forward_limit);

	if (Z_TYPE(dispatcher->response) != IS_OBJECT) {
		yaf_response_instance(&dispatcher->response, sapi_module.name);
	} else {
		yaf_response_clear_body(Z_YAFRESPONSEOBJ(dispatcher->response), NULL);
	}

	if (UNEXPECTED(IS_OBJECT != Z_TYPE(dispatcher->request))) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Expect a %s instance", ZSTR_VAL(yaf_request_ce->name));
		return NULL;
	}

	request = Z_YAFREQUESTOBJ(dispatcher->request);
	/* route request */
	if (EXPECTED(!yaf_request_is_routed(request))) {
		YAF_PLUGIN_HANDLE(dispatcher, YAF_PLUGIN_HOOK_ROUTESTARTUP);
		YAF_EXCEPTION_HANDLE(dispatcher);
		if (!yaf_dispatcher_route(dispatcher)) {
			yaf_trigger_error(YAF_ERR_ROUTE_FAILED, "Routing request failed");
			YAF_EXCEPTION_HANDLE_NORET(dispatcher);
			return NULL;
		}
		yaf_dispatcher_fix_default(dispatcher, request);
		YAF_PLUGIN_HANDLE(dispatcher, YAF_PLUGIN_HOOK_ROUTESHUTDOWN);
		YAF_EXCEPTION_HANDLE(dispatcher);
		yaf_request_set_routed(request, 1);
	} else {
		yaf_dispatcher_fix_default(dispatcher, request);
	}

	YAF_PLUGIN_HANDLE(dispatcher, YAF_PLUGIN_HOOK_LOOPSTARTUP);
	YAF_EXCEPTION_HANDLE(dispatcher);

	if (!yaf_dispatcher_init_view(dispatcher, NULL, NULL)) {
		return NULL;
	}

	do {
		YAF_PLUGIN_HANDLE(dispatcher, YAF_PLUGIN_HOOK_PREDISPATCH);
		YAF_EXCEPTION_HANDLE(dispatcher);
		if (!yaf_dispatcher_handle(dispatcher)) {
			YAF_EXCEPTION_HANDLE(dispatcher);
			return NULL;
		}
		yaf_dispatcher_fix_default(dispatcher, request);
		YAF_PLUGIN_HANDLE(dispatcher, YAF_PLUGIN_HOOK_POSTDISPATCH);
		YAF_EXCEPTION_HANDLE(dispatcher);
	} while (--nesting > 0 && !yaf_request_is_dispatched(request));

	YAF_PLUGIN_HANDLE(dispatcher, YAF_PLUGIN_HOOK_LOOPSHUTDOWN);
	YAF_EXCEPTION_HANDLE(dispatcher);

	if (0 == nesting && !yaf_request_is_dispatched(request)) {
		yaf_trigger_error(YAF_ERR_DISPATCH_FAILED, "The max dispatch nesting %ld was reached", YAF_G(forward_limit));
		YAF_EXCEPTION_HANDLE_NORET(dispatcher);
		return NULL;
	}

	if (!dispatcher->return_response) {
		zval ret;
		zend_call_method_with_0_params(&dispatcher->response, Z_OBJCE(dispatcher->response), NULL, "response", &ret);
		/* it must return bool */
		/* zval_ptr_dtor(&ret); */
		yaf_response_clear_body(Z_YAFRESPONSEOBJ(dispatcher->response), NULL);
	}

	return &dispatcher->response;
}
/* }}} */

/** {{{ proto private Yaf_Dispatcher::__construct(void)
*/
PHP_METHOD(yaf_dispatcher, __construct) {
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
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	dispatcher->auto_render = 0;

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::enableView()
*/
PHP_METHOD(yaf_dispatcher, enableView) {
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	dispatcher->auto_render = 1;

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::returnResponse(bool $return_response = 1)
*/
PHP_METHOD(yaf_dispatcher, returnResponse) {
	zend_bool return_response = 0;
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &return_response) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS()) {
		dispatcher->return_response = return_response;
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		RETURN_BOOL(dispatcher->return_response);
	}
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::flushInstantly(bool $flag)
*/
PHP_METHOD(yaf_dispatcher, flushInstantly) {
	zend_bool instantly_flush;
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &instantly_flush) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS()) {
		dispatcher->instantly_flush = instantly_flush? 1 : 0;
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		RETURN_BOOL(dispatcher->instantly_flush);
	}
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::registerPlugin(Yaf_Plugin_Abstract $plugin)
*/
PHP_METHOD(yaf_dispatcher, registerPlugin) {
	zval *plugin;
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &plugin) == FAILURE) {
		return;
	}

	if (EXPECTED(Z_TYPE_P(plugin) != IS_OBJECT ||
		!instanceof_function(Z_OBJCE_P(plugin), yaf_plugin_ce))) {
		php_error_docref(NULL, E_WARNING, "Expect a %s instance", ZSTR_VAL(yaf_plugin_ce->name));
		RETURN_FALSE;
	}


	Z_ADDREF_P(plugin);
	zend_hash_next_index_insert(&dispatcher->plugins, plugin);

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setRequest(Yaf_Request_Abstract $request)
*/
PHP_METHOD(yaf_dispatcher, setRequest) {
	yaf_request_t *request;
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());


	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &request, yaf_request_ce) == FAILURE) {
		return;
	}

	if (yaf_dispatcher_set_request(dispatcher, request)) {
		RETURN_ZVAL(getThis(), 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::getInstance(void)
*/
PHP_METHOD(yaf_dispatcher, getInstance) {
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!yaf_application_instance()) {
		RETURN_NULL();
	}

	yaf_dispatcher_instance(return_value);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::getRouter(void)
*/
PHP_METHOD(yaf_dispatcher, getRouter) {
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_ZVAL(&dispatcher->router, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::getRequest(void)
*/
PHP_METHOD(yaf_dispatcher, getRequest) {
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_ZVAL(&dispatcher->request, 1, 0);
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
	yaf_response_t 	*response;
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &request, yaf_request_ce) == FAILURE) {
		return;
	}

	yaf_dispatcher_set_request(dispatcher, request);
	if ((response = yaf_dispatcher_dispatch(dispatcher))) {
		RETURN_ZVAL(response, 1, 0);
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
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &flag) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS()) {
		dispatcher->auto_render = flag? 1 : 0;
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		RETURN_BOOL(dispatcher->auto_render);
	}
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::initView(string $tpl_dir, array $options = NULL)
*/
PHP_METHOD(yaf_dispatcher, initView) {
	zend_string *tpl_dir;
	zval *options = NULL;
	yaf_dispatcher_object *dispatcher;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &tpl_dir, &options) == FAILURE) {
		return;
	}
	
	dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());
	if (yaf_dispatcher_init_view(dispatcher, tpl_dir, options)) {
		RETURN_ZVAL(&dispatcher->view, 1, 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setView(Yaf_View_Interface $view)
*/
PHP_METHOD(yaf_dispatcher, setView) {
	yaf_view_t *view;
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &view, yaf_view_interface_ce) == FAILURE) {
		return;
	}

	zval_ptr_dtor(&dispatcher->view);
	ZVAL_COPY(&dispatcher->view, view);

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setDefaultModule(string $name)
*/
PHP_METHOD(yaf_dispatcher, setDefaultModule) {
	zend_string *value;
	yaf_application_object *app = yaf_application_instance();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &value) == FAILURE) {
		return;
	}

	if (!app) {
		RETURN_FALSE;
	}

	if (!yaf_application_is_module_name(value)) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "There is no module '%s'", ZSTR_VAL(value));
		RETURN_FALSE;
	}

	if (app->default_module) {
		zend_string_release(app->default_module);
	}

	app->default_module = yaf_canonical_name(1, value);

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setDefaultController(string $name)
*/
PHP_METHOD(yaf_dispatcher, setDefaultController) {
	zend_string *value;
	yaf_application_object *app = yaf_application_instance();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &value) == FAILURE) {
		return;
	}

	if (app == NULL) {
		RETURN_FALSE;
	}

	if (app->default_controller) {
		zend_string_release(app->default_controller);
	}

	app->default_controller = yaf_canonical_name(1, value);

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setDefaultAction(string $name)
*/
PHP_METHOD(yaf_dispatcher, setDefaultAction) {
	zend_string *value;
	yaf_application_object *app = yaf_application_instance();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &value) == FAILURE) {
		return;
	}

	if (app == NULL) {
		RETURN_FALSE;
	}

	if (app->default_action) {
		zend_string_release(app->default_action);
	}

	app->default_action = yaf_canonical_name(0, value);

	RETURN_ZVAL(getThis(), 1, 0);
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
	yaf_dispatcher_ce->serialize = zend_class_serialize_deny;
	yaf_dispatcher_ce->unserialize = zend_class_unserialize_deny;

	memcpy(&yaf_dispatcher_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_dispatcher_obj_handlers.clone_obj = NULL;
	yaf_dispatcher_obj_handlers.free_obj = yaf_dispatcher_obj_free;

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
