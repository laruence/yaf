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
#include "main/php_output.h"
#include "Zend/zend_interfaces.h" /* for zend_class_serialize_deny */

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_loader.h"
#include "yaf_request.h"
#include "yaf_view.h"

#include "views/yaf_view_interface.h"
#include "views/yaf_view_simple.h"

zend_class_entry *yaf_view_simple_ce;
static zend_object_handlers yaf_view_simple_obj_handlers;

/** {{{ ARG_INFO */
ZEND_BEGIN_ARG_INFO_EX(yaf_view_simple_construct_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, template_dir)
	ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(yaf_view_simple_get_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_view_simple_isset_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(yaf_view_simple_assign_by_ref_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(1, value)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(yaf_view_simple_eval_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, tpl_str)
	ZEND_ARG_INFO(0, vars)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(yaf_view_simple_clear_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO();
/* }}} */

static HashTable *yaf_view_simple_get_properties(zval *object) /* {{{ */ {
	zval rv;
	HashTable *ht;
	yaf_view_object *view = Z_YAFVIEWOBJ_P(object);

	if (!view->properties) {
		ALLOC_HASHTABLE(view->properties);
		zend_hash_init(view->properties, 2, NULL, ZVAL_PTR_DTOR, 0);
		YAF_ALLOW_VIOLATION(view->properties);
	}

	ht = view->properties;
	if (view->tpl_dir) {
		ZVAL_STR_COPY(&rv, view->tpl_dir);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_update(ht, "tpl_dir:protected", sizeof("tpl_dir:protected") - 1, &rv);

	ZVAL_ARR(&rv, zend_array_dup(&view->tpl_vars));
	zend_hash_str_update(ht, "tpl_vars:protected", sizeof("tpl_vars:protected") - 1, &rv);

	return ht;
}
/* }}} */

static zval* yaf_view_simple_read_property(zval *zobj, zval *name, int type, void **cache_slot, zval *rv) /* {{{ */ {
	zval *var;
	zend_string *member;
	yaf_view_object *view = Z_YAFVIEWOBJ_P(zobj);

	if (UNEXPECTED(Z_TYPE_P(name) != IS_STRING)) {
		return &EG(uninitialized_zval);
	}

	member = Z_STR_P(name);

	if ((var = zend_hash_find(&view->tpl_vars, member))) {
		return var;
	}

	return &EG(uninitialized_zval);
}
/* }}} */

static YAF_WRITE_HANDLER yaf_view_simple_write_property(zval *zobj, zval *name, zval *value, void **cache_slot) /* {{{ */ {
	zend_string *member;
	yaf_view_object *view = Z_YAFVIEWOBJ_P(zobj);

	if (UNEXPECTED(Z_TYPE_P(name) != IS_STRING)) {
		YAF_WHANDLER_RET(value);
	}

	member = Z_STR_P(name);

	zend_hash_update(&view->tpl_vars, member, value);
	Z_TRY_ADDREF_P(value);

	YAF_WHANDLER_RET(value);
}
/* }}} */

static zend_object *yaf_view_simple_new(zend_class_entry *ce) /* {{{ */ {
	yaf_view_object *view = emalloc(sizeof(yaf_view_object) + zend_object_properties_size(ce));

	zend_object_std_init(&view->std, ce);
	view->std.handlers = &yaf_view_simple_obj_handlers;

	view->tpl_dir = NULL;
	zend_hash_init(&view->tpl_vars, 8, NULL, ZVAL_PTR_DTOR, 0);
	view->properties = NULL;

	return &view->std;
}
/* }}} */

static void yaf_view_object_free(zend_object *object) /* {{{ */ {
	yaf_view_object *view = php_yaf_view_fetch_object(object);

	if (view->tpl_dir) {
		zend_string_release(view->tpl_dir);
	}
	zend_hash_destroy(&view->tpl_vars);
	if (view->properties) {
		if (GC_DELREF(view->properties) == 0) {
			GC_REMOVE_FROM_BUFFER(view->properties);
			zend_array_destroy(view->properties);
		}
	}

	zend_object_std_dtor(object);
}
/* }}} */

int yaf_view_simple_init(yaf_view_object *view, zend_string *tpl_dir, zend_array *options) /* {{{ */ {
	if (!IS_ABSOLUTE_PATH(ZSTR_VAL(tpl_dir), ZSTR_LEN(tpl_dir))) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Expects an absolute path for templates directory");
		return 0;
	}

	view->tpl_dir = zend_string_copy(tpl_dir);
	return 1;
}
/* }}} */

void yaf_view_simple_instance(yaf_view_t *view, zend_string *tpl_dir) /* {{{ */ {
	zend_object *v = yaf_view_simple_new(yaf_view_simple_ce);
	ZVAL_OBJ(view, v);
}
/* }}} */

static int yaf_view_simple_valid_var_name(zend_string *var_name) /* {{{ */ {
	int i, ch;

	if (UNEXPECTED(zend_string_equals_literal(var_name, "GLOBALS"))) {
		return 0;
	}

	if (UNEXPECTED(zend_string_equals_literal(var_name, "this"))) {
		return 0;
	}

	/* These are allowed as first char: [a-zA-Z_\x7f-\xff] */
	ch = (int)((unsigned char *)ZSTR_VAL(var_name))[0];
	if (ch != '_' &&
			(ch < 65  /* A    */ || /* Z    */ ch > 90)  &&
			(ch < 97  /* a    */ || /* z    */ ch > 122) &&
			(ch < 127 /* 0x7f */ || /* 0xff */ ch > 255)
	   ) {
		return 0;
	}

	/* And these as the rest: [a-zA-Z0-9_\x7f-\xff] */
	if (ZSTR_LEN(var_name) > 1) {
		for (i = 1; i < ZSTR_LEN(var_name); i++) {
			ch = (int)((unsigned char *)ZSTR_VAL(var_name))[i];
			if (ch != '_' &&
					(ch < 48  /* 0    */ || /* 9    */ ch > 57)  &&
					(ch < 65  /* A    */ || /* Z    */ ch > 90)  &&
					(ch < 97  /* a    */ || /* z    */ ch > 122) &&
					(ch < 127 /* 0x7f */ || /* 0xff */ ch > 255)
			   ) {
				return 0;
			}
		}
	}
	return 1;
}
/* }}} */

static void yaf_view_build_symtable(zend_array *symbol_table, zend_array *tpl_vars, zval *vars) /* {{{ */ {
	zval *entry;
	zend_string *var_name;

	zend_hash_init(symbol_table, 8, NULL, ZVAL_PTR_DTOR, 0);

	if (EXPECTED(tpl_vars)) {
	    ZEND_HASH_FOREACH_STR_KEY_VAL(tpl_vars, var_name, entry) {
			if (var_name == NULL) {
				continue;
			}
			if (yaf_view_simple_valid_var_name(var_name)) {
				if (EXPECTED(zend_hash_add_new(symbol_table, var_name, entry))) {
					Z_TRY_ADDREF_P(entry);
				}
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (vars && Z_TYPE_P(vars) == IS_ARRAY) {
	    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(vars), var_name, entry) {
			if (var_name == NULL) {
				continue;
			}
			if (yaf_view_simple_valid_var_name(var_name)) {
				if (EXPECTED(zend_hash_update(symbol_table, var_name, entry))) {
					Z_TRY_ADDREF_P(entry);
				}
			}
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

static int yaf_view_exec_tpl(yaf_view_t *view, zend_op_array *op_array, zend_array *symbol_table, zval* ret) /* {{{ */ {
	zend_execute_data *call;
	zval result;
	uint32_t call_info;

	ZVAL_UNDEF(&result);

	op_array->scope = Z_OBJCE_P(view);

	zend_function *func = (zend_function *)op_array;

#if PHP_VERSION_ID >= 70400
	call_info = ZEND_CALL_HAS_THIS | ZEND_CALL_NESTED_CODE | ZEND_CALL_HAS_SYMBOL_TABLE;
#elif PHP_VERSION_ID >= 70100
	call_info = ZEND_CALL_NESTED_CODE | ZEND_CALL_HAS_SYMBOL_TABLE;
#else
	call_info = ZEND_CALL_NESTED_CODE;
#endif

#if PHP_VERSION_ID < 70400
	call = zend_vm_stack_push_call_frame(call_info, func, 0, op_array->scope, Z_OBJ_P(view));
#else
    call = zend_vm_stack_push_call_frame(call_info, func, 0, Z_OBJ_P(view));
#endif

	call->symbol_table = symbol_table;

	if (ret && php_output_start_user(NULL, 0, PHP_OUTPUT_HANDLER_STDFLAGS) == FAILURE) {
		php_error_docref("ref.outcontrol", E_WARNING, "failed to create buffer");
		return 0;
	}

	zend_init_execute_data(call, op_array, &result);

	ZEND_ADD_CALL_FLAG(call, ZEND_CALL_TOP);
	zend_execute_ex(call);
	zend_vm_stack_free_call_frame(call);

	zval_ptr_dtor(&result);

	if (UNEXPECTED(EG(exception) != NULL)) {
		if (ret) {
			php_output_discard();
		}
		return 0;
	}

	if (ret) {
		if (php_output_get_contents(ret) == FAILURE) {
			php_output_end();
			php_error_docref(NULL, E_WARNING, "Unable to fetch ob content");
			return 0;
		}

		if (php_output_discard() != SUCCESS ) {
			return 0;
		}
	}

	return 1;
}
/* }}} */

static int yaf_view_render_tpl(yaf_view_t *view, const char *tpl, uint32_t tpl_len, zend_array *symbol_table, zval *ret) /* {{{ */ {
	int status = 0;
	zend_stat_t sb;
	zend_file_handle file_handle;
	zend_op_array *op_array;

	if (UNEXPECTED(VCWD_STAT(tpl, &sb) == -1)) {
		yaf_trigger_error(YAF_ERR_NOTFOUND_VIEW, "Failed opening template %s: %s", tpl, strerror(errno));
		return 0;
	}

#if PHP_VERSION_ID < 70400
	file_handle.filename = tpl;
	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.free_filename = 0;
	file_handle.opened_path = NULL;
	file_handle.handle.fp = NULL;
#else
	/* setup file-handle */
	zend_stream_init_filename(&file_handle, tpl);
#endif

	op_array = zend_compile_file(&file_handle, ZEND_INCLUDE);

	if (op_array) {
		if (file_handle.handle.stream.handle) {
			if (!file_handle.opened_path) {
				file_handle.opened_path = zend_string_init(tpl, tpl_len, 0);
			}
			zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path);
		}

		status = yaf_view_exec_tpl(view, op_array, symbol_table, ret);

		destroy_op_array(op_array);
		efree_size(op_array, sizeof(zend_op_array));
	}

	zend_destroy_file_handle(&file_handle);

	return status;
}
/* }}} */

int yaf_view_simple_render(yaf_view_t *view, zend_string *tpl, zval *vars, zval *ret) /* {{{ */ {
	zend_array symbol_table;
	char directory[MAXPATHLEN];
	char *tpl_dir;
	uint32_t tpl_len;
	yaf_view_object *v = Z_YAFVIEWOBJ_P(view);

	yaf_view_build_symtable(&symbol_table, &v->tpl_vars, vars);

	tpl_dir = ZSTR_VAL(tpl);
	tpl_len = ZSTR_LEN(tpl);
	if (!IS_ABSOLUTE_PATH(tpl_dir, tpl_len)) {
		if (UNEXPECTED(v->tpl_dir == NULL)) {
			zend_hash_destroy(&symbol_table);
			yaf_trigger_error(YAF_ERR_NOTFOUND_VIEW,
					"Could not determine the view script path, you should call %s::setScriptPath to specific it",
					ZSTR_VAL(yaf_view_simple_ce->name));
			return 0;
		} else {
			zend_string *v_tpl = v->tpl_dir;
			tpl_len = yaf_compose_2_pathes(directory, v_tpl, tpl_dir, tpl_len);
			directory[tpl_len] = '\0';
			tpl_dir = directory;
		}
	}

	if (EXPECTED(yaf_view_render_tpl(view, tpl_dir, tpl_len, &symbol_table, ret))) {
		zend_hash_destroy(&symbol_table);
		return 1;
	}

	zend_hash_destroy(&symbol_table);
	return 0;
}
/* }}} */

static int yaf_view_simple_eval(yaf_view_t *view, zend_string *tpl, zval * vars, zval *ret) /* {{{ */ {
	zend_array symbol_table;

	if (ZSTR_LEN(tpl)) {
		zval phtml;
		zend_op_array *op_array;
		char *eval_desc = zend_make_compiled_string_description("template code");

		/* eval require code mustn't be wrapped in opening and closing PHP tags */
		ZVAL_STR(&phtml, strpprintf(0, "?>%s", ZSTR_VAL(tpl)));

		op_array = zend_compile_string(&phtml, eval_desc);

		zval_dtor(&phtml);
		efree(eval_desc);

		if (op_array) {
			yaf_view_build_symtable(&symbol_table, &(Z_YAFVIEWOBJ_P(view))->tpl_vars, vars);
			yaf_view_exec_tpl(view, op_array, &symbol_table, ret);
			destroy_op_array(op_array);
			efree_size(op_array, sizeof(zend_op_array));
		}
	}

	zend_hash_destroy(&symbol_table);

	return 1;
}
/* }}} */

static zend_always_inline void yaf_view_simple_assign_single(yaf_view_object *view, zend_string *name, zval *value) /* {{{ */ {
	zend_hash_update(&view->tpl_vars, name, value);
	Z_TRY_ADDREF_P(value);
}
/* }}} */

static void yaf_view_simple_assign_multi(yaf_view_object *view, zval *value) /* {{{ */ {
	zend_hash_copy(&view->tpl_vars, Z_ARRVAL_P(value), (copy_ctor_func_t) zval_add_ref);
}
/* }}} */

static void yaf_view_simple_clear_assign(yaf_view_object *view, zend_string *name) /* {{{ */ {
	if (name) {
		zend_hash_del(&view->tpl_vars, name);
	} else {
		zend_hash_clean(&view->tpl_vars);
	}
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::__construct(string $tpl_dir, array $options = NULL)
*/
PHP_METHOD(yaf_view_simple, __construct) {
	zend_string *tpl_dir;
	zval *options = NULL;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "S|a!", &tpl_dir, &options) == FAILURE) {
		return;
	}

	yaf_view_simple_init(Z_YAFVIEWOBJ_P(getThis()), tpl_dir, options? Z_ARRVAL_P(options) : NULL);
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::setScriptPath(string $tpl_dir)
*/
PHP_METHOD(yaf_view_simple, setScriptPath) {
	zend_string *tpl_dir;
	yaf_view_object *view = Z_YAFVIEWOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &tpl_dir) == FAILURE) {
		return;
	}

	if (IS_ABSOLUTE_PATH(ZSTR_VAL(tpl_dir), ZSTR_LEN(tpl_dir))) {
		if (view->tpl_dir) {
			zend_string_release(view->tpl_dir);
		}
		view->tpl_dir = zend_string_copy(tpl_dir);
		RETURN_ZVAL(getThis(), 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::getScriptPath(Yaf_Request_Abstarct $request = NULL)
*/
PHP_METHOD(yaf_view_simple, getScriptPath) {
	yaf_request_t *request = NULL;
	yaf_view_object *view = Z_YAFVIEWOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|O!", &request, yaf_request_ce) == FAILURE) {
		return;
	}

	if (view->tpl_dir) {
		RETURN_STR_COPY(view->tpl_dir);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::assign(mixed $value, mixed $value = null)
*/
PHP_METHOD(yaf_view_simple, assign) {
	yaf_view_object *view = Z_YAFVIEWOBJ_P(getThis());

	if (ZEND_NUM_ARGS() == 2) {
		zval *value;
		zend_string *name;
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
			return;
		}
	    yaf_view_simple_assign_single(view, name, value);
		RETURN_ZVAL(getThis(), 1, 0);
	} else if (ZEND_NUM_ARGS() == 1) {
		zval *value;
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &value) == FAILURE) {
			return;
		}
		yaf_view_simple_assign_multi(view, value);
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		WRONG_PARAM_COUNT;
		RETURN_FALSE;
	}
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::assignRef(mixed $value, mixed &$value)
*/
PHP_METHOD(yaf_view_simple, assignRef) {
	zend_string *name;
	zval *value;
	yaf_view_object *view = Z_YAFVIEWOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
		return;
	}

	yaf_view_simple_assign_single(view, name, value);

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::get($name)
*/
PHP_METHOD(yaf_view_simple, get) {
	zend_string *name = NULL;
	yaf_view_object *view = Z_YAFVIEWOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S!", &name) == FAILURE) {
		return;
	}

	if (name && ZSTR_LEN(name)) {
		zval *val = zend_hash_find(&view->tpl_vars, name);
		if (val) {
			RETURN_ZVAL(val, 1, 0);
		} else {
			RETURN_NULL();
		}
	} else {
		RETURN_ARR(zend_array_dup(&view->tpl_vars));
	}
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::render(string $tpl, array $vars = NULL)
*/
PHP_METHOD(yaf_view_simple, render) {
	zval *tpl;
	zval *vars = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|a!", &tpl, &vars) == FAILURE) {
		return;
	}

	if (UNEXPECTED(Z_TYPE_P(tpl) != IS_STRING || Z_STRLEN_P(tpl) == 0)) {
		RETURN_FALSE;
	}

	if (!yaf_view_simple_render(getThis(), Z_STR_P(tpl), vars, return_value)) {
		RETURN_FALSE;
	}
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::eval(string $tpl_content, array $vars = NULL)
*/
PHP_METHOD(yaf_view_simple, eval) {
	zend_string *tpl;
	zval *vars = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|a!", &tpl, &vars) == FAILURE) {
		return;
	}

	if (!yaf_view_simple_eval(getThis(), tpl, vars, return_value)) {
		RETURN_FALSE;
	}
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::display(string $tpl, array $vars = NULL)
*/
PHP_METHOD(yaf_view_simple, display) {
	zval *tpl;
	zval *vars = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|a!", &tpl, &vars) == FAILURE) {
		return;
	}

	if (UNEXPECTED(Z_TYPE_P(tpl) != IS_STRING || Z_STRLEN_P(tpl) == 0)) {
		RETURN_FALSE;
	}

	if (!yaf_view_simple_render(getThis(), Z_STR_P(tpl), vars, NULL)) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_View_Simple::clear(string $name)
*/
PHP_METHOD(yaf_view_simple, clear) {
	zend_string *name = NULL;
	yaf_view_object *view = Z_YAFVIEWOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S!", &name) == FAILURE) {
		return;
	}

	yaf_view_simple_clear_assign(view, name);

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ yaf_view_simple_methods
*/
zend_function_entry yaf_view_simple_methods[] = {
	PHP_ME(yaf_view_simple, __construct, yaf_view_simple_construct_arginfo, ZEND_ACC_CTOR|ZEND_ACC_FINAL|ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, get, yaf_view_simple_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, assign, yaf_view_assign_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, render, yaf_view_render_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, eval,  yaf_view_simple_eval_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, display, yaf_view_display_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, assignRef, yaf_view_simple_assign_by_ref_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, clear, yaf_view_simple_clear_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, setScriptPath, yaf_view_setpath_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_view_simple, getScriptPath, yaf_view_getpath_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_view_simple, __get, get, yaf_view_simple_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_view_simple, __set, assign, yaf_view_assign_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(view_simple) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_View_Simple", "Yaf\\View\\Simple", yaf_view_simple_methods);
	yaf_view_simple_ce = zend_register_internal_class_ex(&ce, NULL);
	yaf_view_simple_ce->create_object = yaf_view_simple_new;
	yaf_view_simple_ce->serialize = zend_class_serialize_deny;
	yaf_view_simple_ce->unserialize = zend_class_unserialize_deny;

	zend_class_implements(yaf_view_simple_ce, 1, yaf_view_interface_ce);

	memcpy(&yaf_view_simple_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_view_simple_obj_handlers.offset = XtOffsetOf(yaf_view_object, std);
	yaf_view_simple_obj_handlers.free_obj = yaf_view_object_free;
	yaf_view_simple_obj_handlers.get_properties = yaf_view_simple_get_properties;
	yaf_view_simple_obj_handlers.read_property = yaf_view_simple_read_property;
	yaf_view_simple_obj_handlers.write_property = yaf_view_simple_write_property;
	yaf_view_simple_obj_handlers.clone_obj = NULL;
	yaf_view_simple_obj_handlers.get_gc = NULL;


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

