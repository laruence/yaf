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

#if PHP_MAJOR_VERSION > 7
#include "yaf_dispatcher_arginfo.h"
#else
#include "yaf_dispatcher_legacy_arginfo.h"
#endif

zend_class_entry    *yaf_dispatcher_ce;
static zend_object_handlers yaf_dispatcher_obj_handlers;

static void yaf_dispatcher_obj_free(zend_object *object) /* {{{ */ {
	yaf_dispatcher_object *dispatcher = php_yaf_dispatcher_fetch_object(object);

	zval_ptr_dtor(&dispatcher->request);
	zval_ptr_dtor(&dispatcher->response);
	zval_ptr_dtor(&dispatcher->router);
	zval_ptr_dtor(&dispatcher->view);

	if (dispatcher->plugins) {
		if (GC_DELREF(dispatcher->plugins) == 0) {
			GC_REMOVE_FROM_BUFFER(dispatcher->plugins);
			zend_array_destroy(dispatcher->plugins);
		}
	}

	if (dispatcher->properties) {
		if (GC_DELREF(dispatcher->properties) == 0) {
			GC_REMOVE_FROM_BUFFER(dispatcher->properties);
			zend_array_destroy(dispatcher->properties);
		}
	}

	zend_object_std_dtor(object);
}
/* }}} */

static HashTable *yaf_dispatcher_get_properties(yaf_object *obj) /* {{{ */ {
	zval rv;
	HashTable *ht;
	yaf_dispatcher_object *dispatcher = php_yaf_dispatcher_fetch_object(yaf_strip_obj(obj));

	if (!dispatcher->properties) {
		ALLOC_HASHTABLE(dispatcher->properties);
		zend_hash_init(dispatcher->properties, 16, NULL, ZVAL_PTR_DTOR, 0);
		YAF_ALLOW_VIOLATION(dispatcher->properties);
	}

	ht = dispatcher->properties;

	ZVAL_BOOL(&rv, YAF_DISPATCHER_FLAGS(dispatcher) & YAF_DISPATCHER_AUTO_RENDER);
	zend_hash_str_update(ht, "auto_render:protected", sizeof("auto_render:protected") - 1, &rv);
	ZVAL_BOOL(&rv, YAF_DISPATCHER_FLAGS(dispatcher) & YAF_DISPATCHER_INSTANT_FLUSH);
	zend_hash_str_update(ht, "instant_flush:protected", sizeof("instant_flush:protected") - 1, &rv);
	ZVAL_BOOL(&rv, YAF_DISPATCHER_FLAGS(dispatcher) & YAF_DISPATCHER_RETURN_RESPONSE);
	zend_hash_str_update(ht, "return_response:protected", sizeof("return_response:protected") - 1, &rv);

	ZVAL_COPY(&rv, &dispatcher->request);
	zend_hash_str_update(ht, "request:protected", sizeof("request:protected") - 1, &rv);
	ZVAL_COPY(&rv, &dispatcher->response);
	zend_hash_str_update(ht, "response:protected", sizeof("response:protected") - 1, &rv);
	ZVAL_COPY(&rv, &dispatcher->router);
	zend_hash_str_update(ht, "router:protected", sizeof("router:protected") - 1, &rv);
	ZVAL_COPY(&rv, &dispatcher->view);
	zend_hash_str_update(ht, "view:protected", sizeof("view:protected") - 1, &rv);

	if (dispatcher->plugins) {
		ZVAL_ARR(&rv, dispatcher->plugins);
		GC_ADDREF(dispatcher->plugins);
	} else {
#if PHP_VERSION_ID < 70400
		array_init(&rv);
#else
		ZVAL_EMPTY_ARRAY(&rv);
#endif
	}
	zend_hash_str_update(ht, "plugins:protected", sizeof("plugins:protected") - 1, &rv);

	return ht;
}
/* }}} */

static HashTable *yaf_dispatcher_get_gc(yaf_object *obj, zval **table, int *n) /* {{{ */ {
	yaf_dispatcher_object *dispatcher = php_yaf_dispatcher_fetch_object(yaf_strip_obj(obj));

	*table = &dispatcher->request;
	*n = 4;

	return dispatcher->plugins;
}
/* }}} */

void yaf_dispatcher_instance(yaf_dispatcher_t *this_ptr) /* {{{ */ {
	yaf_application_object *app = Z_YAFAPPOBJ(YAF_G(app));
	yaf_dispatcher_object *dispatcher;

	if (IS_OBJECT != Z_TYPE(app->dispatcher)) {
		dispatcher = emalloc(sizeof(yaf_dispatcher_object) + zend_object_properties_size(yaf_dispatcher_ce));
		zend_object_std_init(&dispatcher->std, yaf_dispatcher_ce);
		dispatcher->std.handlers = &yaf_dispatcher_obj_handlers;

		yaf_router_instance(&dispatcher->router);

		ZVAL_NULL(&dispatcher->request);
		ZVAL_NULL(&dispatcher->response);
		ZVAL_NULL(&dispatcher->view);
		YAF_DISPATCHER_FLAGS(dispatcher) = YAF_DISPATCHER_AUTO_RENDER;
		dispatcher->plugins = NULL;
		dispatcher->properties = NULL;

		ZVAL_OBJ(&app->dispatcher, &dispatcher->std);
		return;
	}

	ZVAL_COPY(this_ptr, &app->dispatcher);
}
/* }}} */

static ZEND_COLD zend_never_inline zend_class_entry *yaf_dispatcher_get_errors_hub(int type, ...) /* {{{ */ {
	va_list args;

	va_start(args, type);
	if (type == 0) {
		zend_string *app_dir = va_arg(args, zend_string*);
		zend_string *module = va_arg(args, zend_string*);
		zend_string *controller = va_arg(args, zend_string*);
		yaf_trigger_error(YAF_ERR_AUTOLOAD_FAILED, "path too long %s%c%s%c%s",
			ZSTR_VAL(app_dir), DEFAULT_SLASH, ZSTR_VAL(module), DEFAULT_SLASH, ZSTR_VAL(controller));
	} else if (type == 1) {
		zend_class_entry *ce = va_arg(args, zend_class_entry*);
		zend_string *controller = va_arg(args, zend_string*);
		char *directory = va_arg(args, char*);
		size_t directory_len = va_arg(args, size_t);
		if (ce) {
			yaf_trigger_error(YAF_ERR_TYPE_ERROR,
					"Controller '%s' is not a subclass of %s", ZSTR_VAL(ce->name), ZSTR_VAL(yaf_controller_ce->name));
		} else if (strlen(directory) == directory_len) { 
			yaf_trigger_error(YAF_ERR_AUTOLOAD_FAILED, "path too long while loading '%s'", ZSTR_VAL(controller));
		} else if (!zend_hash_str_exists(&EG(included_files), directory, strlen(directory))) {
			yaf_trigger_error(YAF_ERR_NOTFOUND_CONTROLLER,
					"Failed opening controller script %s: %s", directory, strerror(errno));
		} else {
			if (EXPECTED(yaf_is_name_suffix())) {
				yaf_trigger_error(YAF_ERR_AUTOLOAD_FAILED,
						"Could not find class %s%s%s in controller script %s",
						ZSTR_VAL(controller), YAF_G(name_separator), "Controller", directory);
			} else {
				yaf_trigger_error(YAF_ERR_AUTOLOAD_FAILED,
						"Could not find class %s%s%s in controller script %s",
						"Controller", YAF_G(name_separator), ZSTR_VAL(controller), directory);
			}
		}
	} else {
		zval *pzval;
		zend_class_entry *ce = va_arg(args, zend_class_entry*);
		zval *action_map = va_arg(args, zval*);
		zend_string *action = va_arg(args, zend_string*);
		yaf_controller_t *controller = va_arg(args, yaf_controller_t*);
		char *path = va_arg(args, char*);
		if (Z_TYPE_P(action_map) != IS_ARRAY) {
			yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION,
					"There is no method %s%s in %s", ZSTR_VAL(action), "Action", ZSTR_VAL(Z_OBJCE_P(controller)->name));
		} else if ((pzval = zend_hash_find(Z_ARRVAL_P(action_map), action)) == NULL) {
			yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION, "There is no method %s%s in %s::$%s", ZSTR_VAL(action),
					"Action", ZSTR_VAL(Z_OBJCE_P(controller)->name), YAF_KNOWN_CHARS(YAF_ACTIONS_MAP));
		} else if (Z_TYPE_P(pzval) != IS_STRING) {
			yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION, "Action '%s' in %s::%s does not have a valid path", ZSTR_VAL(action),
					ZSTR_VAL(Z_OBJCE_P(controller)->name), YAF_KNOWN_CHARS(YAF_ACTIONS_MAP));
		} else if (ce) {
			yaf_trigger_error(YAF_ERR_TYPE_ERROR,
					"Action '%s' is not a subclass of %s", ZSTR_VAL(ce->name), ZSTR_VAL(yaf_action_ce->name));
		} else if (!zend_hash_str_exists(&EG(included_files), path, strlen(path))) {
			yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION,
					"Failed opening action script %s: %s", path, strerror(errno));
		} else {
			yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION,
					"Could not find action %s in %s", ZSTR_VAL(action), path);
		}
	}

	va_end(args);
	return NULL;
}
/* }}} */

static void yaf_dispatcher_get_call_parameters(yaf_request_object *request, zend_function *fptr, zval **args, uint32_t *count) /* {{{ */ {
	zval *arg;
	zval *params;
	uint32_t current;
	zend_arg_info *arg_info;
	HashTable *params_ht;

	if (request->params == NULL) {
		*count = 0;
		*args = NULL;
		return;
	}

	params_ht = request->params;
	arg_info = fptr->common.arg_info;
	*args = params = safe_emalloc(sizeof(zval), fptr->common.num_args, 0);
	for (current = 0; current < fptr->common.num_args; current++, arg_info++) {
		if ((arg = zend_hash_find(params_ht, arg_info->name)) != NULL) {
			ZVAL_COPY_VALUE(&(params[current]), arg);
		} else if (current >= fptr->common.required_num_args) {
			uint32_t idx = current + 1;
			zend_op *op = ((zend_op_array *)fptr)->opcodes;
			zend_op *end = op + ((zend_op_array *)fptr)->last;
			while (op < end) {
				if (op->op1.num == idx) {
					if (op->opcode == ZEND_RECV_INIT) {
#if PHP_VERSION_ID < 70300
						arg = RT_CONSTANT(&fptr->op_array, op->op2);
#else
						arg = RT_CONSTANT(op, op->op2);
#endif
						/* Constant evaluation? */
						if (Z_TYPE_P(arg) < IS_OBJECT) {
							ZVAL_COPY_VALUE(&(params[current]), arg);
						} else {
							goto out;
						}
					} else {
						ZVAL_NULL(&(params[current]));
					}
					break;
				}
				++op;
			}
			if (UNEXPECTED(op == end)) {
				goto out;
			}
		}
	}
out:
	*count = current;
	return;
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
	yaf_application_object *app = Z_YAFAPPOBJ(YAF_G(app));

	if (request->module == NULL) {
		request->module = zend_string_copy(app->default_module);
	}

	if (request->controller == NULL) {
		request->controller = zend_string_copy(app->default_controller);
	}

	if (request->action == NULL) {
		request->action = zend_string_copy(app->default_action);
	}
}
/* }}} */

static void yaf_dispatcher_set_request(yaf_dispatcher_object *dispatcher, yaf_request_t *request) /* {{{ */ {
	if (EXPECTED(Z_TYPE(dispatcher->request) == IS_OBJECT)) {
		OBJ_RELEASE(Z_OBJ(dispatcher->request));
	}
	ZVAL_COPY(&dispatcher->request, request);
}
/* }}} */

static void yaf_dispatcher_set_response(yaf_dispatcher_object *dispatcher, yaf_response_t *response) /* {{{ */ {
	if (EXPECTED(Z_TYPE(dispatcher->response) == IS_OBJECT)) {
		OBJ_RELEASE(Z_OBJ(dispatcher->response));
	} 
	ZVAL_COPY(&dispatcher->response, response);
}
/* }}} */

static zend_always_inline int yaf_dispatcher_route(yaf_dispatcher_object *dispatcher) /* {{{ */ {
	yaf_router_object *router = Z_YAFROUTEROBJ(dispatcher->router);

	return yaf_router_route(router, &dispatcher->request);
}
/* }}} */

static zend_class_entry *yaf_dispatcher_get_controller(zend_string *app_dir, yaf_request_object *request, int def_module) /* {{{ */ {
	char directory[MAXPATHLEN];
	size_t directory_len;
	zend_class_entry *ce;
	zend_string *lc_name;
	zend_string *controller = request->controller;
	zend_string *module = request->module;
	ALLOCA_FLAG(use_heap);

	if (def_module) {
		if (UNEXPECTED(ZSTR_LEN(app_dir) + sizeof(YAF_CONTROLLER_DIRECTORY_NAME) > MAXPATHLEN)) {
			return yaf_dispatcher_get_errors_hub(0, app_dir, module, controller);
		}
		directory_len = yaf_compose_2_pathes(directory, app_dir, ZEND_STRL(YAF_CONTROLLER_DIRECTORY_NAME));
	} else {
		if (UNEXPECTED(ZSTR_LEN(app_dir) + ZSTR_LEN(module) + sizeof(YAF_MODULE_DIRECTORY_NAME) + sizeof(YAF_CONTROLLER_DIRECTORY_NAME) > MAXPATHLEN)) {
			return yaf_dispatcher_get_errors_hub(0, app_dir, module, controller);
		}
		directory_len = yaf_compose_2_pathes(directory, app_dir, ZEND_STRL(YAF_MODULE_DIRECTORY_NAME));
		directory[directory_len++] = DEFAULT_SLASH;
		directory_len += yaf_compose_2_pathes(directory + directory_len, module, ZEND_STRL(YAF_CONTROLLER_DIRECTORY_NAME));
	}

	STR_ALLOCA_ALLOC(lc_name, ZSTR_LEN(controller) + YAF_G(name_separator_len) + sizeof("controller") - 1, use_heap);
	if (EXPECTED(yaf_is_name_suffix())) {
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
		zend_str_tolower_copy(p, ZSTR_VAL(controller), ZSTR_LEN(controller));
	}

	if ((ce = zend_hash_find_ptr(EG(class_table), lc_name)) == NULL || !instanceof_function(ce, yaf_controller_ce)) {
		yaf_loader_object *l = Z_YAFLOADEROBJ(YAF_G(loader));
		if (yaf_loader_load_internal(l, ZSTR_VAL(controller), ZSTR_LEN(controller), directory, directory_len)) {
			if (EXPECTED((ce = zend_hash_find_ptr(EG(class_table), lc_name)))) {
				if (EXPECTED(instanceof_function(ce, yaf_controller_ce))) {
					STR_ALLOCA_FREE(lc_name, use_heap);
					return ce;
				}
			}
		}
		STR_ALLOCA_FREE(lc_name, use_heap);
		return yaf_dispatcher_get_errors_hub(1, ce, controller, directory, directory_len);
	}
	STR_ALLOCA_FREE(lc_name, use_heap);
	return ce;
}
/* }}} */

static zend_class_entry *yaf_dispatcher_get_action(zend_string *app_dir, yaf_controller_t *controller, yaf_request_object *request) /* {{{ */ {
	zval *pzval;
	zval *actions_map;
	char path[MAXPATHLEN];
	zend_class_entry *ce = NULL;
	zend_string *action = request->action;

#if PHP_VERSION_ID < 80000
# if PHP_VERSION_ID < 70100
	actions_map = zend_read_property(Z_OBJCE_P(controller), controller, YAF_KNOWN_CHARS(YAF_ACTIONS_MAP), strlen(YAF_KNOWN_CHARS(YAF_ACTIONS_MAP)), 1, NULL);
# else
	actions_map = zend_read_property_ex(Z_OBJCE_P(controller), controller, YAF_KNOWN_STR(YAF_ACTIONS_MAP), 1, NULL);
# endif
#else
    actions_map = zend_read_property_ex(Z_OBJCE_P(controller), Z_OBJ_P(controller), YAF_KNOWN_STR(YAF_ACTIONS_MAP), 1, NULL);
#endif

	ZVAL_DEREF(actions_map);

	if (EXPECTED(IS_ARRAY == Z_TYPE_P(actions_map))) {
		zend_string *lc_name;
		ALLOCA_FLAG(use_heap);

		STR_ALLOCA_ALLOC(lc_name, ZSTR_LEN(action) + YAF_G(name_separator_len) + sizeof("action") - 1, use_heap);
		if (EXPECTED(yaf_is_name_suffix())) {
			char *p = ZSTR_VAL(lc_name);
			memcpy(p, ZSTR_VAL(action), ZSTR_LEN(action));
			p += ZSTR_LEN(action);
			if (UNEXPECTED(YAF_G(name_separator_len))) {
				zend_str_tolower_copy(p, YAF_G(name_separator), YAF_G(name_separator_len));
				p += YAF_G(name_separator_len);
			}
			memcpy(p, "action", sizeof("action"));
		} else {
			char *p = ZSTR_VAL(lc_name);
			memcpy(p, "action", sizeof("action") - 1);
			p += sizeof("action") - 1;
			if (UNEXPECTED(YAF_G(name_separator_len))) {
				zend_str_tolower_copy(p, YAF_G(name_separator), YAF_G(name_separator_len));
				p += YAF_G(name_separator_len);
			}
			memcpy(p, ZSTR_VAL(action), ZSTR_LEN(action) + 1);
		}

		if ((ce = zend_hash_find_ptr(EG(class_table), lc_name)) != NULL) {
			if (EXPECTED(instanceof_function(ce, yaf_action_ce))) {
				STR_ALLOCA_FREE(lc_name, use_heap);
				return ce;
			}
		} else if (((pzval = zend_hash_find_ind(Z_ARRVAL_P(actions_map), action)) != NULL) &&
				((Z_TYPE_P(pzval) == IS_STRING) ||
				(Z_TYPE_P(pzval) == IS_REFERENCE && (pzval = Z_REFVAL_P(pzval), (Z_TYPE_P(pzval) == IS_STRING))))) {
			uint32_t len;

			len = yaf_compose_2_pathes(path, app_dir, Z_STRVAL_P(pzval), Z_STRLEN_P(pzval));
			path[len] = '\0';
			if (yaf_loader_import(path, len)) {
				if ((ce = zend_hash_find_ptr(EG(class_table), lc_name)) != NULL) {
					if (EXPECTED(instanceof_function(ce, yaf_action_ce))) {
						STR_ALLOCA_FREE(lc_name, use_heap);
						return ce;
					}
				}
			}
		}
		STR_ALLOCA_FREE(lc_name, use_heap);
	}

	return yaf_dispatcher_get_errors_hub(2, ce, actions_map, action, controller, path);
}
/* }}} */

static zend_never_inline zend_function *yaf_dispatcher_handle_action(yaf_application_object *app, yaf_dispatcher_object *dispatcher, yaf_controller_t *controller) /* {{{ */ {
	zend_function *fptr;
	zend_class_entry *ce;

	if ((ce = yaf_dispatcher_get_action(app->directory, controller, Z_YAFREQUESTOBJ(dispatcher->request)))) {
		if ((fptr = zend_hash_find_ptr(&(ce->function_table), YAF_KNOWN_STR(YAF_EXECUTE)))) {
			yaf_action_t action;
			yaf_action_object *act;

			object_init_ex(&action, ce);
			act = Z_YAFCTLOBJ(action);

			yaf_controller_init(act, dispatcher);

			yaf_action_init(act, controller, Z_YAFREQUESTOBJ(dispatcher->request)->action);

			GC_DELREF(Z_OBJ_P(controller));
			ZVAL_COPY_VALUE(controller, &action);

			return fptr;
		}
	}
	return NULL;
}
/* }}} */

static ZEND_HOT int yaf_dispatcher_handle(yaf_dispatcher_object *dispatcher) /* {{{ */ {
	yaf_application_object *app = Z_YAFAPPOBJ(YAF_G(app));

	yaf_request_set_dispatched(Z_YAFREQUESTOBJ(dispatcher->request), 1);
	if (EXPECTED(app->directory)) {
		int is_def_module = 0;
		zend_class_entry *ce;
		yaf_request_object *request = Z_YAFREQUESTOBJ(dispatcher->request);

		ZEND_ASSERT(request->module);
		ZEND_ASSERT(request->controller);
		ZEND_ASSERT(request->action);

		if (zend_string_equals(app->default_module, request->module)) {
			is_def_module = 1;
		}

		if ((ce = yaf_dispatcher_get_controller(app->directory, request, is_def_module))) {
			zend_function *fptr;
			yaf_controller_object *ctl;
			yaf_controller_t controller;
			zend_string *current_action;

			object_init_ex(&controller, ce);
			ctl = Z_YAFCTLOBJ(controller);
			if (UNEXPECTED(!yaf_controller_init(ctl, dispatcher))) {
				OBJ_RELEASE(Z_OBJ(controller));
				return 0;
			}

			if (UNEXPECTED(!yaf_request_is_dispatched(request))) {
				/* forward is called in init method */
				OBJ_RELEASE(Z_OBJ(controller));
				return yaf_dispatcher_handle(dispatcher);
			}

			if (EXPECTED(yaf_view_get_tpl_dir(&dispatcher->view, &dispatcher->request) == NULL)) {
				zend_string *view_dir;
				/* view template directory for application, view engine's directory has high priority */
				if (is_def_module) {
					view_dir = zend_string_alloc(ZSTR_LEN(app->directory) + sizeof("views"), 0);
					yaf_compose_2_pathes(ZSTR_VAL(view_dir), app->directory, ZEND_STRS("views"));
				} else {
					view_dir = strpprintf(0, "%s%c%s%c%s%c%s", ZSTR_VAL(app->directory),
							DEFAULT_SLASH, "modules", DEFAULT_SLASH, ZSTR_VAL(request->module), DEFAULT_SLASH, "views");
				}
				yaf_view_set_tpl_dir(&dispatcher->view, view_dir);
				zend_string_release(view_dir);
			}

			do { /* Fetch the action method, if it doesn't exist , fall to $action_map property */
				char *func_name;
				uint32_t func_len;
				ALLOCA_FLAG(use_heap);

				func_len = ZSTR_LEN(request->action) + sizeof("action") - 1;
				func_name = do_alloca(func_len, use_heap);
				memcpy(func_name, ZSTR_VAL(request->action), ZSTR_LEN(request->action));
				memcpy(func_name + ZSTR_LEN(request->action), "action", sizeof("action") - 1);
				/* Magic __call supports? */
				if (UNEXPECTED((fptr = zend_hash_str_find_ptr(&((ce)->function_table), func_name, func_len)) == NULL)) {
					/* Fallback to lowercase searching */
					zend_str_tolower(func_name, ZSTR_LEN(request->action));
					if ((fptr = zend_hash_str_find_ptr(&((ce)->function_table), func_name, func_len)) == NULL) {
						if (UNEXPECTED((fptr = yaf_dispatcher_handle_action(app, dispatcher, &controller)) == NULL)) {
							free_alloca(func_name, use_heap);
							OBJ_RELEASE(Z_OBJ(controller));
							return 0;
						}
						ctl = Z_YAFCTLOBJ(controller);
					}
				}
				free_alloca(func_name, use_heap);
			} while (0);

			do { /* Execute the method */
				zval ret;
				zval *args = NULL;
				uint32_t count = 0;

				current_action = zend_string_copy(request->action);
				if ((fptr->common.num_args)) {
					yaf_dispatcher_get_call_parameters(request, fptr, &args, &count);
				}
				if (UNEXPECTED(!yaf_controller_execute(&controller, fptr, count, args, &ret))) {
					if ((args)) {
						efree(args);
					}
					if (UNEXPECTED(Z_ISUNDEF(ret))) {
						zend_string_release(current_action);
						OBJ_RELEASE(Z_OBJ(controller));
						return 0;
					}
				}
				if ((args)) {
					efree(args);
				}
				if ((Z_TYPE(ret) == IS_FALSE)) {
					/* no auto-renderring */
					zend_string_release(current_action);
					OBJ_RELEASE(Z_OBJ(controller));
					return 1;
				}
				zval_ptr_dtor(&ret);
			} while(0);

			/* controller's property yafAutoRender has higher priority */
			if (yaf_controller_auto_render(ctl, YAF_DISPATCHER_FLAGS(dispatcher) & YAF_DISPATCHER_AUTO_RENDER)) {
				zval res;
				zend_bool flush_instantly = YAF_DISPATCHER_FLAGS(dispatcher) & YAF_DISPATCHER_INSTANT_FLUSH;
				if (EXPECTED(yaf_controller_render(&controller, current_action, NULL, flush_instantly? NULL : &res))) {
					if (UNEXPECTED(!flush_instantly)) {
						ZEND_ASSERT(Z_TYPE(res) == IS_STRING);
						yaf_response_append_body(Z_YAFRESPONSEOBJ(dispatcher->response), NULL, Z_STR(res));
						zend_string_release(Z_STR(res));
					}
				} else {
					zend_string_release(current_action);
					OBJ_RELEASE(Z_OBJ(controller));
					return 0;
				}
			}

			zend_string_release(current_action);
			OBJ_RELEASE(Z_OBJ(controller));
			return 1;
		}
	} else {
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED,
				"%s requires %s(which set the application.directory) to be initialized first",
				ZSTR_VAL(yaf_dispatcher_ce->name), ZSTR_VAL(yaf_application_ce->name));
	}
	return 0;
}
/* }}} */

static ZEND_COLD zend_never_inline void yaf_dispatcher_exception_handler(yaf_dispatcher_object *dispatcher) /* {{{ */ {
	zend_string *controller, *action;
	zval exception;
	const zend_op *opline;
	yaf_request_object *request = Z_YAFREQUESTOBJ(dispatcher->request);

#if PHP_VERSION_ID >= 80000
	if (zend_is_unwind_exit(EG(exception))) {
		return;
	}
#endif

	if ((YAF_DISPATCHER_FLAGS(dispatcher) & YAF_DISPATCHER_IN_EXCEPTION)|| !EG(exception)) {
		return;
	}

	YAF_DISPATCHER_FLAGS(dispatcher) |= YAF_DISPATCHER_IN_EXCEPTION;

	ZVAL_OBJ(&exception, EG(exception));
	EG(exception) = NULL;
	opline = EG(opline_before_exception);
#if ZEND_DEBUG
	EG(opline_before_exception) = NULL;
#endif

	controller = zend_string_init(ZEND_STRL(YAF_ERROR_CONTROLLER), 0);
	action = zend_string_init(ZEND_STRL(YAF_ERROR_ACTION), 0);

	yaf_request_set_mvc(request, NULL, controller, action, NULL);
	if (UNEXPECTED(request->module == NULL)) {
		/* must threw in routerStartup hook ?*/
		yaf_dispatcher_fix_default(dispatcher, request);
	}

	zend_string_release(controller);
	zend_string_release(action);

	/** use $request->getException() instand of */
	if (yaf_request_set_str_params_single(request, "exception", sizeof("exception") - 1, &exception)) {
		zval_ptr_dtor(&exception);
	} else {
		/* failover to uncaught exception */
		EG(exception) = Z_OBJ(exception);
		YAF_DISPATCHER_FLAGS(dispatcher) = ~YAF_DISPATCHER_IN_EXCEPTION;
		return;
	}
	yaf_request_set_dispatched(request, 0);

	if (UNEXPECTED(!yaf_dispatcher_init_view(dispatcher, NULL, NULL))) {
		yaf_request_del_str_param(request, "exception", sizeof("exception") - 1);
		YAF_DISPATCHER_FLAGS(dispatcher) = ~YAF_DISPATCHER_IN_EXCEPTION;
		return;
	}

	if (!yaf_dispatcher_handle(dispatcher)) {
		if (UNEXPECTED(EG(exception)) &&
			instanceof_function(EG(exception)->ce,
				yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(YAF_ERR_NOTFOUND_CONTROLLER)])) {
			zend_string_release(request->module);
			request->module = zend_string_copy(Z_YAFAPPOBJ(YAF_G(app))->default_module);
			/* failover to default module error catcher */
			zend_clear_exception();
			yaf_dispatcher_handle(dispatcher);
		}
	}

	yaf_request_del_str_param(request, "exception", sizeof("exception") - 1);

	if (!(YAF_DISPATCHER_FLAGS(dispatcher) & YAF_DISPATCHER_RETURN_RESPONSE)) {
		yaf_response_response(Z_YAFRESPONSEOBJ(dispatcher->response));
		yaf_response_clear_body(Z_YAFRESPONSEOBJ(dispatcher->response), NULL);
	}

	EG(opline_before_exception) = opline;
	YAF_DISPATCHER_FLAGS(dispatcher) = ~YAF_DISPATCHER_IN_EXCEPTION;
	YAF_EXCEPTION_ERASE_EXCEPTION();
}
/* }}} */

ZEND_HOT yaf_response_t *yaf_dispatcher_dispatch(yaf_dispatcher_object *dispatcher) /* {{{ */ {
	yaf_request_object *request;
	zend_bool catch_exception = yaf_is_catch_exception();
	uint32_t nesting = yaf_get_forward_limit();

	if (EXPECTED(Z_TYPE(dispatcher->response) != IS_OBJECT)) {
		yaf_response_instance(&dispatcher->response, sapi_module.name);
	} else {
		yaf_response_clear_body(Z_YAFRESPONSEOBJ(dispatcher->response), NULL);
	}

	request = Z_YAFREQUESTOBJ(dispatcher->request);
	/* route request */
	if (EXPECTED(!yaf_request_is_routed(request))) {
		YAF_PLUGIN_HANDLE(dispatcher, YAF_HOOK_ROUTESTARTUP);
		if (UNEXPECTED(!yaf_dispatcher_route(dispatcher))) {
			yaf_trigger_error(YAF_ERR_ROUTE_FAILED, "Routing request failed");
			YAF_EXCEPTION_HANDLE(dispatcher);
		}
		yaf_dispatcher_fix_default(dispatcher, request);
		YAF_PLUGIN_HANDLE(dispatcher, YAF_HOOK_ROUTESHUTDOWN);
		yaf_request_set_routed(request, 1);
	} else {
		yaf_dispatcher_fix_default(dispatcher, request);
	}

	YAF_PLUGIN_HANDLE(dispatcher, YAF_HOOK_LOOPSTARTUP);

	if (UNEXPECTED(!yaf_dispatcher_init_view(dispatcher, NULL, NULL))) {
		return NULL;
	}

	do {
		YAF_PLUGIN_HANDLE(dispatcher, YAF_HOOK_PREDISPATCH);
		if (UNEXPECTED(!yaf_dispatcher_handle(dispatcher))) {
			YAF_EXCEPTION_HANDLE(dispatcher);
		}
		/* yaf_dispatcher_fix_default(dispatcher, request); */
		YAF_PLUGIN_HANDLE(dispatcher, YAF_HOOK_POSTDISPATCH);
	} while (!yaf_request_is_dispatched(request) && --nesting > 0);

	YAF_PLUGIN_HANDLE(dispatcher, YAF_HOOK_LOOPSHUTDOWN);

	if (EXPECTED(yaf_request_is_dispatched(request))) {
		if (!(YAF_DISPATCHER_FLAGS(dispatcher) & YAF_DISPATCHER_RETURN_RESPONSE)) {
			yaf_response_response(Z_YAFRESPONSEOBJ(dispatcher->response));
			yaf_response_clear_body(Z_YAFRESPONSEOBJ(dispatcher->response), NULL);
		}
		return &dispatcher->response;
	} else {
		ZEND_ASSERT(nesting == 0);
		yaf_trigger_error(YAF_ERR_DISPATCH_FAILED, "The maximum dispatching count %ld is reached", yaf_get_forward_limit());
		YAF_EXCEPTION_HANDLE(dispatcher);
	}
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

/** {{{ proto public Yaf_Dispatcher::disableView()
*/
PHP_METHOD(yaf_dispatcher, disableView) {
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	YAF_DISPATCHER_FLAGS(dispatcher) &= ~YAF_DISPATCHER_AUTO_RENDER;

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

	YAF_DISPATCHER_FLAGS(dispatcher) |= YAF_DISPATCHER_AUTO_RENDER;

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::returnResponse(bool $return_response = 0)
*/
PHP_METHOD(yaf_dispatcher, returnResponse) {
	zend_bool return_response = 0;
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &return_response) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS()) {
		if (return_response) {
			YAF_DISPATCHER_FLAGS(dispatcher) |= YAF_DISPATCHER_RETURN_RESPONSE;
		} else {
			YAF_DISPATCHER_FLAGS(dispatcher) &= ~YAF_DISPATCHER_RETURN_RESPONSE;
		}
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		RETURN_BOOL(YAF_DISPATCHER_FLAGS(dispatcher) & YAF_DISPATCHER_RETURN_RESPONSE);
	}
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::flushInstantly(bool $flag = 1)
*/
PHP_METHOD(yaf_dispatcher, flushInstantly) {
	zend_bool instantly_flush = 1;
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &instantly_flush) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS()) {
		if (instantly_flush) {
			YAF_DISPATCHER_FLAGS(dispatcher) |= YAF_DISPATCHER_INSTANT_FLUSH;
		} else {
			YAF_DISPATCHER_FLAGS(dispatcher) &= ~YAF_DISPATCHER_INSTANT_FLUSH;
		}

		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		RETURN_BOOL(YAF_DISPATCHER_FLAGS(dispatcher) & YAF_DISPATCHER_INSTANT_FLUSH);
	}
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::registerPlugin(Yaf_Plugin_Abstract $plugin)
*/
PHP_METHOD(yaf_dispatcher, registerPlugin) {
	yaf_plugin_t *plugin;
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &plugin, yaf_plugin_ce) == FAILURE) {
		return;
	}

	if (!dispatcher->plugins) {
		ALLOC_HASHTABLE(dispatcher->plugins);
		zend_hash_init(dispatcher->plugins, 8, NULL, ZVAL_PTR_DTOR, 0);
	}
	Z_ADDREF_P(plugin);
	zend_hash_next_index_insert(dispatcher->plugins, plugin);

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

	yaf_dispatcher_set_request(dispatcher, request);
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setResponse(Yaf_Response_Abstract $response)
*/
PHP_METHOD(yaf_dispatcher, setResponse) {
	yaf_response_t *response;
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &response, yaf_response_ce) == FAILURE) {
		return;
	}

	yaf_dispatcher_set_response(dispatcher, response);
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::getInstance(void)
*/
PHP_METHOD(yaf_dispatcher, getInstance) {
	if (zend_parse_parameters_none() == FAILURE) {
		return;
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

/** {{{ proto public Yaf_Dispatcher::getResponse(void)
*/
PHP_METHOD(yaf_dispatcher, getResponse) {
	yaf_dispatcher_object *dispatcher = Z_YAFDISPATCHEROBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_ZVAL(&dispatcher->response, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::getApplication(void)
*/
PHP_METHOD(yaf_dispatcher, getApplication) {
	PHP_MN(yaf_application_app)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
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
		yaf_set_throw_exception(flag);
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		RETURN_BOOL(yaf_is_throw_exception());
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
		yaf_set_catch_exception(flag);
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		RETURN_BOOL(yaf_is_catch_exception());
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
		if (flag) {
			YAF_DISPATCHER_FLAGS(dispatcher) |= YAF_DISPATCHER_AUTO_RENDER;
		} else {
			YAF_DISPATCHER_FLAGS(dispatcher) &= ~YAF_DISPATCHER_AUTO_RENDER;
		}

		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		RETURN_BOOL(YAF_DISPATCHER_FLAGS(dispatcher) & YAF_DISPATCHER_AUTO_RENDER);
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

/** {{{ proto public Yaf_Dispatcher::getDefaultModule(void)
*/
PHP_METHOD(yaf_dispatcher, getDefaultModule) {
	yaf_application_object *app = yaf_application_instance();

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (app->default_module) {
		RETURN_STR_COPY(app->default_module);
	} else {
		RETURN_STR(YAF_KNOWN_STR(YAF_DEFAULT_MODULE));
	}
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::getDefaultController(void)
*/
PHP_METHOD(yaf_dispatcher, getDefaultController) {
	yaf_application_object *app = yaf_application_instance();

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (app->default_controller) {
		RETURN_STR_COPY(app->default_controller);
	} else {
		RETURN_STR(YAF_KNOWN_STR(YAF_DEFAULT_CONTROLLER));
	}
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::getDefaultAction(void)
*/
PHP_METHOD(yaf_dispatcher, getDefaultAction) {
	yaf_application_object *app = yaf_application_instance();

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (app->default_action) {
		RETURN_STR_COPY(app->default_action);
	} else {
		RETURN_STR(YAF_KNOWN_STR(YAF_DEFAULT_ACTION));
	}
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

/** {{{ proto public Yaf_Dispatcher::setErrorHandler(string $callbacak[, int $error_types = E_ALL | E_STRICT ] )
*/
PHP_METHOD(yaf_dispatcher, setErrorHandler) {
	zval *callback, *error_type = NULL;
	zval params[2];
	zval function;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z", &callback, &error_type) == FAILURE) {
		return;
	}

	ZVAL_COPY(&params[0], callback);
	if (error_type) {
		ZVAL_COPY(&params[1], error_type);
	}

	ZVAL_STRING(&function, "set_error_handler");
	if (call_user_function(EG(function_table), NULL, &function, return_value, ZEND_NUM_ARGS(), params) == FAILURE) {
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

/** {{{ proto private Yaf_Dispatcher::__construct(void)
*/
PHP_METHOD(yaf_dispatcher, __construct) {
}
/* }}} */

/** {{{ yaf_dispatcher_methods
*/
zend_function_entry yaf_dispatcher_methods[] = {
	PHP_ME(yaf_dispatcher, __construct, arginfo_class_Yaf_Dispatcher___construct,  ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(yaf_dispatcher, enableView, arginfo_class_Yaf_Dispatcher_enableView,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, disableView, arginfo_class_Yaf_Dispatcher_disableView,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, initView, arginfo_class_Yaf_Dispatcher_initView,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setView, arginfo_class_Yaf_Dispatcher_setView,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setRequest, arginfo_class_Yaf_Dispatcher_setRequest,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setResponse, arginfo_class_Yaf_Dispatcher_setResponse,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, getApplication, arginfo_class_Yaf_Dispatcher_getApplication,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, getRouter, arginfo_class_Yaf_Dispatcher_getRouter,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, getResponse, arginfo_class_Yaf_Dispatcher_getResponse,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, getRequest, arginfo_class_Yaf_Dispatcher_getRequest,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, getDefaultModule, arginfo_class_Yaf_Dispatcher_getDefaultModule,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, getDefaultController, arginfo_class_Yaf_Dispatcher_getDefaultController,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, getDefaultAction, arginfo_class_Yaf_Dispatcher_getDefaultAction,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setErrorHandler, arginfo_class_Yaf_Dispatcher_setErrorHandler,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setDefaultModule, arginfo_class_Yaf_Dispatcher_setDefaultModule,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setDefaultController, arginfo_class_Yaf_Dispatcher_setDefaultController,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setDefaultAction, arginfo_class_Yaf_Dispatcher_setDefaultAction,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, returnResponse, arginfo_class_Yaf_Dispatcher_returnResponse,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, autoRender, arginfo_class_Yaf_Dispatcher_autoRender,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, flushInstantly, arginfo_class_Yaf_Dispatcher_flushInstantly,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, getInstance, arginfo_class_Yaf_Dispatcher_getInstance,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(yaf_dispatcher, dispatch, arginfo_class_Yaf_Dispatcher_dispatch,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, throwException, arginfo_class_Yaf_Dispatcher_throwException,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, catchException, arginfo_class_Yaf_Dispatcher_catchException,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, registerPlugin, arginfo_class_Yaf_Dispatcher_registerPlugin,  ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(dispatcher) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Dispatcher", "Yaf\\Dispatcher", yaf_dispatcher_methods);

	yaf_dispatcher_ce = zend_register_internal_class_ex(&ce, NULL);
#if PHP_VERSION_ID < 80100
	yaf_dispatcher_ce->ce_flags |= ZEND_ACC_FINAL;
	yaf_dispatcher_ce->serialize = zend_class_serialize_deny;
	yaf_dispatcher_ce->unserialize = zend_class_unserialize_deny;
#else
	yaf_dispatcher_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NOT_SERIALIZABLE;
#endif
	memcpy(&yaf_dispatcher_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_dispatcher_obj_handlers.offset = XtOffsetOf(yaf_dispatcher_object, std);
	yaf_dispatcher_obj_handlers.free_obj = yaf_dispatcher_obj_free;
	yaf_dispatcher_obj_handlers.clone_obj = NULL;
	yaf_dispatcher_obj_handlers.get_gc = yaf_dispatcher_get_gc;
	yaf_dispatcher_obj_handlers.get_properties = yaf_dispatcher_get_properties;

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
