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

#ifdef __SSE2__
#include <emmintrin.h>
#endif

#include "php.h"
#include "Zend/zend_interfaces.h" /* for zend_call_method_with_* */

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_application.h"
#include "yaf_loader.h"
#include "yaf_request.h"
#include "yaf_response.h"
#include "yaf_dispatcher.h"
#include "yaf_view.h"
#include "yaf_exception.h"
#include "yaf_controller.h"
#include "yaf_action.h"

zend_class_entry     *yaf_controller_ce;
zend_object_handlers  yaf_controller_obj_handlers;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_controller_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_controller_ctor_arginfo, 0, 0, 3)
    ZEND_ARG_INFO(0, request)
    ZEND_ARG_INFO(0, response)
    ZEND_ARG_INFO(0, view)
    ZEND_ARG_ARRAY_INFO(0, args, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_controller_initview_arginfo, 0, 0, 0)
    ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_controller_getiarg_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_controller_setvdir_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, view_directory)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_controller_forward_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, module)
    ZEND_ARG_INFO(0, controller)
    ZEND_ARG_INFO(0, action)
    ZEND_ARG_ARRAY_INFO(0, parameters, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_controller_redirect_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, url)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_controller_render_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, tpl)
    ZEND_ARG_ARRAY_INFO(0, parameters, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_controller_display_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, tpl)
    ZEND_ARG_ARRAY_INFO(0, parameters, 1)
ZEND_END_ARG_INFO()
/* }}} */

static inline void yaf_controller_sanitize_view_path_normal(char *str, size_t len) /* {{{ */ {
	register char *s = str;
	while ((s = memchr(s, '_', len - (s - str)))) {
		*s++ = DEFAULT_SLASH;
	}
}
/* }}} */

static void yaf_controller_sanitize_view_path(zend_string *path) /* {{{ */ {
	char *pos = ZSTR_VAL(path);
	size_t len = ZSTR_LEN(path);

#ifdef __SSE2__
	const __m128i sep = _mm_set1_epi8('_');
	const __m128i delta = _mm_set1_epi8('_' - DEFAULT_SLASH);

	while (len >= 16) {
		__m128i op = _mm_loadu_si128((__m128i *)pos);
		__m128i eq = _mm_cmpeq_epi8(op, sep);
		if (_mm_movemask_epi8(eq)) {
			eq = _mm_and_si128(eq, delta);
			op = _mm_sub_epi8(op, eq);
			_mm_storeu_si128((__m128i*)pos, op);
		}
		len -= 16;
		pos += 16;
	}
#endif
	if (len) {
		yaf_controller_sanitize_view_path_normal(pos, len);
	}
}
/* }}} */

static void yaf_controller_object_free(zend_object *object) /* {{{ */ {
	yaf_controller_object *ctl = php_yaf_controller_fetch_object(object);

	if (ctl->module) {
		zend_string_release(ctl->module);
	}

	if (ctl->name) {
		zend_string_release(ctl->name);
	}

	if (ctl->invoke_args) {
	}

	if (ctl->script_path) {
		zend_string_release(ctl->script_path);
	}

	/* leave to dispatcher?
	zval_ptr_dtor(&ctl->request);
	zval_ptr_dtor(&ctl->response);
	zval_ptr_dtor(&ctl->view);
	*/

	/* yaf_action struct */
	if (ctl->ctl.name) {
		zval_ptr_dtor(&ctl->ctl.ctl);
		zend_string_release(ctl->ctl.name);
	}

	zend_object_std_dtor(object);
}
/* }}} */

static HashTable *yaf_controller_get_debug_info(zval *object, int *is_temp) /* {{{ */ {
	zval rv;
	HashTable *ht;
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(object);

	*is_temp = 1;
	ALLOC_HASHTABLE(ht);
	zend_hash_init(ht, 8, NULL, ZVAL_PTR_DTOR, 0);

	if (ctl->module) {
		ZVAL_STR_COPY(&rv, ctl->module);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_add(ht, "module:protected", sizeof("module:protected") - 1, &rv);

	if (ctl->request) {
		ZVAL_COPY(&rv, ctl->request);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_add(ht, "request:protected", sizeof("request:protected") - 1, &rv);

	if (ctl->response) {
		ZVAL_COPY(&rv, ctl->response);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_add(ht, "response:protected", sizeof("response:protected") - 1, &rv);
	
	if (ctl->view) {
		ZVAL_COPY(&rv, ctl->view);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_add(ht, "view:protected", sizeof("view:protected") - 1, &rv);

	return ht;
}
/* }}} */

int yaf_controller_auto_render(yaf_controller_t *ctl, int dispatch_render) /* {{{ */ {
	zval rv;
	zval *render = zend_read_property(Z_OBJCE_P(ctl), ctl, ZEND_STRL(YAF_CONTROLLER_PROPERTY_NAME_RENDER), 1, &rv);

	if (EXPECTED(Z_TYPE_P(render) == IS_NULL)) {
		return dispatch_render;
	}

	if (Z_TYPE_P(render) == IS_TRUE) {
		return 1;
	}

	if (Z_TYPE_P(render) == IS_FALSE) {
		return 0;	
	}

	return zend_is_true(render);
}
/* }}} */

void yaf_controller_set_module_name(yaf_controller_object *ctl, zend_string *module) /* {{{ */ {
	if (ctl->module) {
		zend_string_release(ctl->module);
	}
	ctl->module = zend_string_copy(module);
}
/* }}} */

static zval *yaf_controller_read_property(zval *zobj, zval *name, int type, void **cache_slot, zval *rv) /* {{{ */ {
	const char *member;
	size_t member_len;
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(zobj);

	if (UNEXPECTED(Z_TYPE_P(name) != IS_STRING)) {
		return &EG(uninitialized_zval);
	}

	if (UNEXPECTED(type == BP_VAR_W || type == BP_VAR_RW)) {
		php_error_docref(NULL, E_WARNING,
				"Indirect modification of Yaf_Controller internal property '%s' is not allowed", Z_STRVAL_P(name));
		return &EG(error_zval);
	}

	if (UNEXPECTED(!instanceof_function(Z_OBJCE_P(zobj), yaf_controller_ce))) {
		return &EG(uninitialized_zval);
	}

	member = Z_STRVAL_P(name);
	member_len = Z_STRLEN_P(name);

	/* for back compatibility of leading _ access */
	if (*member == '_')	{
		member++;
		member_len--;
	}

	if (strncmp(member, "request", sizeof("request")) == 0) {
		ZVAL_COPY(rv, ctl->request);
		return rv;
	}

	if (strncmp(member, "view", sizeof("view")) == 0) {
		ZVAL_COPY(rv, ctl->view);
		return rv;
	}

	if (strncmp(member, "response", sizeof("response")) == 0) {
		ZVAL_COPY(rv, ctl->response);
		return rv;
	}

	if (strncmp(member, "module", sizeof("module")) == 0) {
		ZVAL_STR_COPY(rv, ctl->module);
		return rv;
	}

	return std_object_handlers.read_property(zobj, name, type, cache_slot, rv);
}
/* }}} */

static zval *yaf_controller_get_property(zval *zobj, zval *name, int type, void **cache_slot) /* {{{ */ {
	const char *member;
	size_t member_len;
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(zobj);

	if (UNEXPECTED(Z_TYPE_P(name) != IS_STRING)) {
		return &EG(error_zval);
	}

	if (UNEXPECTED(!instanceof_function(Z_OBJCE_P(zobj), yaf_controller_ce))) {
		return &EG(error_zval);
	}

	member = Z_STRVAL_P(name);
	member_len = Z_STRLEN_P(name);

	/* for back compatibility of leading _ access */
	if (*member == '_')	{
		member++;
		member_len--;
	}

	if (strncmp(member, "request", sizeof("request")) == 0) {
		return ctl->request;
	}

	if (strncmp(member, "view", sizeof("view")) == 0) {
		return ctl->view;
	}

	if (strncmp(member, "response", sizeof("response")) == 0) {
		return ctl->response;
	}

	return std_object_handlers.get_property_ptr_ptr(zobj, name, type, cache_slot);
}
/* }}} */

static YAF_WRITE_HANDLER yaf_controller_write_property(zval *zobj, zval *name, zval *value, void **cache_slot) /* {{{ */ {
	const char *member;
	size_t member_len;

	if (UNEXPECTED(Z_TYPE_P(name) != IS_STRING)) {
		YAF_WHANDLER_RET(value);
	}

	if (UNEXPECTED(!instanceof_function(Z_OBJCE_P(zobj), yaf_controller_ce))) {
		YAF_WHANDLER_RET(value);
	}

	member = Z_STRVAL_P(name);
	member_len = Z_STRLEN_P(name);

	/* for back compatibility of leading _ access */
	if (*member == '_')	{
		member++;
		member_len--;
	}

	if (strncmp(member, "request", sizeof("request")) == 0 ||
		strncmp(member, "view", sizeof("view")) == 0 ||
		strncmp(member, "response", sizeof("response")) == 0  ||
		strncmp(member, "module", sizeof("module")) == 0) {
		php_error_docref(NULL, E_WARNING,
				"Modification of Yaf_Controller internal property '%s' is not allowed", Z_STRVAL_P(name));
		YAF_WHANDLER_RET(value);
	}

	return std_object_handlers.write_property(zobj, name, value, cache_slot);
}
/* }}} */

static zend_object *yaf_controller_new(zend_class_entry *ce) /* {{{ */ {
	yaf_controller_object *ctl = emalloc(sizeof(yaf_controller_object) + zend_object_properties_size(ce));

	memset(ctl, 0, XtOffsetOf(yaf_controller_object, std));
	zend_object_std_init(&ctl->std, ce);
	if (ce->default_properties_count) {
		object_properties_init(&ctl->std, ce);
	}
	ctl->std.handlers = &yaf_controller_obj_handlers;

	return &ctl->std;
}
/* }}} */

static int yaf_controller_render_ex(yaf_controller_object *ctl, zend_string *action, zval *var_array, zval *ret) /* {{{ */ {
	zend_string *path;
	zend_string *controller;
	const char *view_ext;
	yaf_application_object *app;

	if (UNEXPECTED(ctl->view == NULL || ctl->module == NULL)) {
		return 0;
	}

	if (EXPECTED(app = yaf_application_instance())) {
		view_ext = ZSTR_VAL(app->view_ext);
	} else {
		view_ext = YAF_DEFAULT_VIEW_EXT;
	}

	if (EXPECTED(ctl->ctl.name == NULL)) {
		controller = ctl->name;
	} else {
		controller = ctl->ctl.name;
	}

	path = strpprintf(0, "%s%c%s.%s", ZSTR_VAL(controller), DEFAULT_SLASH, ZSTR_VAL(action), view_ext);

	zend_str_tolower(ZSTR_VAL(path), ZSTR_LEN(controller));

	yaf_controller_sanitize_view_path(path);
	
	if (UNEXPECTED(!(yaf_view_render(ctl->view, path, var_array, ret)))) {
		zend_string_release(path);
		return 0;
	}

	zend_string_release(path);
	if (UNEXPECTED(EG(exception))) {
		return 0;
	}

	return 1;
}
/* }}} */

int yaf_controller_render(yaf_controller_t *controller, zend_string *action, zval *var_array, zval *ret) /* {{{ */ {
	zend_function *fbc;
	zend_class_entry *ce = Z_OBJCE_P(controller);

	if (ret != NULL) {
		fbc = (zend_function*)zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("render"));
	} else {
		fbc = (zend_function*)zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("display"));
	}

	if (EXPECTED(fbc->type == ZEND_INTERNAL_FUNCTION)) {
		ZEND_ASSERT(fbc->common.scope == yaf_controller_ce);
		return yaf_controller_render_ex(Z_YAFCTLOBJ_P(controller), action, var_array, ret);
	} else {
		zval arg;

		ZVAL_STR(&arg, action);
		if (var_array == NULL) {
			if (ret != NULL) {
				zend_call_method_with_1_params(controller, ce, NULL, "render", ret, &arg);
				if (UNEXPECTED(Z_TYPE_P(ret) != IS_STRING || EG(exception))) {
					zval_ptr_dtor(ret);
					return 0;
				}
			} else {
				zval rt;
				zend_call_method_with_1_params(controller, ce, NULL, "display", &rt, &arg);
				if (UNEXPECTED(Z_TYPE(rt) == IS_FALSE || EG(exception))) {
					zval_ptr_dtor(&rt);
					return 0;
				}
				zval_ptr_dtor(&rt);
				ZVAL_TRUE(ret);
			}
		} else {
			if (ret != NULL) {
				zend_call_method_with_2_params(controller, ce, NULL, "render", ret, &arg, var_array);
				if (UNEXPECTED(Z_TYPE_P(ret) != IS_STRING || EG(exception))) {
					zval_ptr_dtor(ret);
					return 0;
				}
			} else {
				zval rt;
				zend_call_method_with_2_params(controller, ce, NULL, "display", &rt, &arg, var_array);
				if (UNEXPECTED(Z_TYPE(rt) == IS_FALSE || EG(exception))) {
					zval_ptr_dtor(&rt);
					return 0;
				}
				zval_ptr_dtor(&rt);
				ZVAL_TRUE(ret);
			}
		}
	}

	return 1;
}
/* }}} */

void yaf_controller_init(yaf_controller_object *ctl, yaf_request_t *req, yaf_response_t *response, yaf_view_t *view, zval *args) /* {{{ */ {
	zend_class_entry *ce = ctl->std.ce;

	ctl->request = req;
	ctl->response = response;
	ctl->view = view;
	ctl->name = zend_string_copy(Z_YAFREQUESTOBJ_P(req)->controller);
	ctl->module = zend_string_copy(Z_YAFREQUESTOBJ_P(req)->module);
/*
	if (args) {
		zend_update_property(ce, self, ZEND_STRL(YAF_CONTROLLER_PROPERTY_NAME_ARGS), args);
	}
*/
	if (!instanceof_function(ce, yaf_action_ce) &&
		zend_hash_str_exists(&(ce->function_table), ZEND_STRL("init"))) {
		zval obj;
		ZVAL_OBJ(&obj, &ctl->std);
		zend_call_method_with_0_params(&obj, ce, NULL, "init", NULL);
	}
}
/* }}} */

/** {{{ proto public Yaf_Controller_Abstract::init()
*/
PHP_METHOD(yaf_controller, init) {
}

/* }}} */

/** {{{ proto protected Yaf_Controller_Abstract::__construct(Yaf_Request_Abstract $request, Yaf_Response_abstrct $response, Yaf_View_Interface $view, array $invokeArgs = NULL)
*/
PHP_METHOD(yaf_controller, __construct) {
	yaf_request_t 	*request;
	yaf_response_t	*response;
	yaf_view_t		*view;
	zval 			*invoke_arg = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OOO|a",
		&request, yaf_request_ce, &response, yaf_response_ce, &view, yaf_view_interface_ce, &invoke_arg) == FAILURE) {
		return;
	}

	yaf_controller_init(Z_YAFCTLOBJ_P(getThis()), request, response, view, invoke_arg);
}
/* }}} */

/** {{{ proto public Yaf_Controller_Abstract::getView(void)
*/
PHP_METHOD(yaf_controller, getView) {
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (ctl->view) {
		RETURN_ZVAL(ctl->view, 1, 0);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Controller_Abstract::getRequest(void)
*/
PHP_METHOD(yaf_controller, getRequest) {
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (ctl->request) {
		RETURN_ZVAL(ctl->request, 1, 0);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Controller_Abstract::getResponse(void)
*/
PHP_METHOD(yaf_controller, getResponse) {
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (ctl->response) {
		RETURN_ZVAL(ctl->response, 1, 0);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Controller_Abstract::initView(array $options = NULL)
*/
PHP_METHOD(yaf_controller, initView) {
	zval *args;
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &args) == FAILURE) {
		return;
	}

	if (ctl->view) {
		RETURN_ZVAL(ctl->view, 1, 0);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Controller_Abstract::getInvokeArg(string $name)
 */
PHP_METHOD(yaf_controller, getInvokeArg) {
	zend_string *name = NULL;
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S",  &name) == FAILURE) {
		return;
	}

	if (EXPECTED(ctl->invoke_args && ZSTR_LEN(name))) {
		zval *arg = zend_hash_find(ctl->invoke_args, name);
		if (EXPECTED(arg != NULL)) {
			RETURN_ZVAL(arg, 1, 0);
		}
	}
	
	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Controller_Abstract::getInvokeArgs(void)
 */
PHP_METHOD(yaf_controller, getInvokeArgs) {
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (ctl->invoke_args) {
		RETURN_ARR(zend_array_dup(ctl->invoke_args));
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Controller_Abstract::getModuleName(void)
 */
PHP_METHOD(yaf_controller, getModuleName) {
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (EXPECTED(ctl->module)) {
		RETURN_STR_COPY(ctl->module);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Controller_Abstract::getName(void)
 */
PHP_METHOD(yaf_controller, getName) {
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (EXPECTED(ctl->name)) {
		RETURN_STR_COPY(ctl->name);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Controller_Abstract::setViewpath(string $view_directory)
*/
PHP_METHOD(yaf_controller, setViewpath) {
	zend_string *path;
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &path) == FAILURE) {
		return;
	}

	if (EXPECTED(ctl->view)) {
		yaf_view_set_tpl_dir(ctl->view, path);
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Controller_Abstract::getViewpath(void)
*/
PHP_METHOD(yaf_controller, getViewpath) {
	zend_string *tpl_dir;
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (EXPECTED(ctl->view)) {
		if ((tpl_dir = yaf_view_get_tpl_dir(ctl->view, NULL))) {
			RETURN_STR_COPY(tpl_dir);
		}
	}

	RETURN_EMPTY_STRING();
}
/* }}} */

/** {{{ proto public Yaf_Controller_Abstract::forward($module, $controller, $action, $args = NULL)
*/
PHP_METHOD(yaf_controller, forward) {
	zval *controller, *module, *action, *args;
	yaf_request_object *request;
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|zza", &module, &controller, &action, &args) == FAILURE) {
		return;
	}

	if (UNEXPECTED(ctl->request == NULL)) {
		RETURN_FALSE;
	}
	
	request = Z_YAFREQUESTOBJ_P(ctl->request);
	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (Z_TYPE_P(module) != IS_STRING) {
				php_error_docref(NULL, E_WARNING, "Expect a string action name");
				RETURN_FALSE;
			}
			yaf_request_set_mvc(request, NULL, NULL, Z_STR_P(module), NULL);
			break;
		case 2:
			if (Z_TYPE_P(controller) ==  IS_STRING) {
				yaf_request_set_mvc(request, NULL, Z_STR_P(module), Z_STR_P(controller), NULL);
			} else if (Z_TYPE_P(controller) == IS_ARRAY) {
				yaf_request_set_mvc(request, NULL, NULL, Z_STR_P(module), Z_ARRVAL_P(controller));
			} else {
				RETURN_FALSE;
			}
			break;
		case 3:
			if (Z_TYPE_P(action) == IS_STRING) {
				yaf_request_set_mvc(request, Z_STR_P(module), Z_STR_P(controller), Z_STR_P(action), NULL);
			} else if (Z_TYPE_P(action) == IS_ARRAY) {
				yaf_request_set_mvc(request, NULL, Z_STR_P(module), Z_STR_P(controller), Z_ARRVAL_P(action));
			} else {
				RETURN_FALSE;
			}
			break;
		case 4:
			yaf_request_set_mvc(request, Z_STR_P(module), Z_STR_P(controller), Z_STR_P(action), Z_ARRVAL_P(args));
			break;
	}

	yaf_request_set_dispatched(request, 0);

	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Controller_Abstract::redirect(string $url)
*/
PHP_METHOD(yaf_controller, redirect) {
	zend_string *location;
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &location) == FAILURE) {
		return;
	}

	if (EXPECTED(ctl->response)) {
		yaf_response_set_redirect(Z_YAFRESPONSEOBJ_P(ctl->response), location);
	}

	RETURN_TRUE;
}
/* }}} */

/** {{{ proto protected Yaf_Controller_Abstract::render(string $action, array $var_array = NULL)
*/
PHP_METHOD(yaf_controller, render) {
	zend_string *action_name;
	zval *var_array	= NULL;
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|a!", &action_name, &var_array) == FAILURE) {
		return;
	}

	if (UNEXPECTED(!yaf_controller_render_ex(ctl, action_name, var_array, return_value))) {
		RETURN_FALSE;
	}
}
/* }}} */

/** {{{ proto protected Yaf_Controller_Abstract::display(string $action, array $var_array = NULL)
*/
PHP_METHOD(yaf_controller, display) {
	zend_string *action_name;
	zval *var_array	= NULL;
	yaf_controller_object *ctl = Z_YAFCTLOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|a!", &action_name, &var_array) == FAILURE) {
		return;
	}

	RETURN_BOOL(yaf_controller_render_ex(ctl, action_name, var_array, NULL));
}
/* }}} */

/** {{{ yaf_controller_methods
*/
zend_function_entry yaf_controller_methods[] = {
	PHP_ME(yaf_controller, __construct,	yaf_controller_ctor_arginfo,    ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(yaf_controller, render,	    yaf_controller_render_arginfo, 	ZEND_ACC_PROTECTED)
	PHP_ME(yaf_controller, display,	    yaf_controller_display_arginfo, ZEND_ACC_PROTECTED)
	PHP_ME(yaf_controller, getRequest,	yaf_controller_void_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_controller, getResponse,	yaf_controller_void_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_controller, getView, 	yaf_controller_void_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_controller, getName,     yaf_controller_void_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_controller, getModuleName,yaf_controller_void_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_controller, initView,	yaf_controller_initview_arginfo,ZEND_ACC_PUBLIC)
	PHP_ME(yaf_controller, setViewpath,	yaf_controller_setvdir_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_controller, getViewpath,	yaf_controller_void_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_controller, forward,	   	yaf_controller_forward_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_controller, redirect,    yaf_controller_redirect_arginfo,ZEND_ACC_PUBLIC)
	PHP_ME(yaf_controller, getInvokeArgs,yaf_controller_void_arginfo,   ZEND_ACC_PUBLIC)
	PHP_ME(yaf_controller, getInvokeArg, yaf_controller_getiarg_arginfo,ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(controller) {
	zend_class_entry ce;
	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Controller_Abstract", "Yaf\\Controller_Abstract", yaf_controller_methods);
	yaf_controller_ce = zend_register_internal_class_ex(&ce, NULL);
	yaf_controller_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	yaf_controller_ce->create_object = yaf_controller_new;
	yaf_controller_ce->serialize = zend_class_serialize_deny;
	yaf_controller_ce->unserialize = zend_class_unserialize_deny;

	memcpy(&yaf_controller_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_controller_obj_handlers.offset = XtOffsetOf(yaf_controller_object, std);
	yaf_controller_obj_handlers.free_obj = yaf_controller_object_free;
	yaf_controller_obj_handlers.get_debug_info = yaf_controller_get_debug_info;
	yaf_controller_obj_handlers.read_property = yaf_controller_read_property;
	yaf_controller_obj_handlers.get_property_ptr_ptr = yaf_controller_get_property;
	yaf_controller_obj_handlers.write_property = yaf_controller_write_property;
	yaf_controller_obj_handlers.clone_obj = NULL;

	//zend_declare_property_null(yaf_controller_ce, ZEND_STRL(YAF_CONTROLLER_PROPERTY_NAME_ACTIONS),	ZEND_ACC_PUBLIC);

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
