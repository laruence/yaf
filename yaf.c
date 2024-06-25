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
#include "php_ini.h"
#include "main/SAPI.h"
#include "Zend/zend_alloc.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"

#ifdef __SSE2__
#include <emmintrin.h>
#endif

#include "php_yaf.h"
#include "yaf_logo.h"
#include "yaf_loader.h"
#include "yaf_exception.h"
#include "yaf_application.h"
#include "yaf_dispatcher.h"
#include "yaf_config.h"
#include "yaf_view.h"
#include "yaf_controller.h"
#include "yaf_action.h"
#include "yaf_request.h"
#include "yaf_response.h"
#include "yaf_router.h"
#include "yaf_bootstrap.h"
#include "yaf_plugin.h"
#include "yaf_registry.h"
#include "yaf_session.h"

ZEND_DECLARE_MODULE_GLOBALS(yaf);

zend_string **yaf_known_strings = NULL;
const char const *yaf_known_chars[] = {
#define _YAF_CHARS(id, str) str,
YAF_KNOWN_NAMES(_YAF_CHARS)
#undef _YAF_CHARS
	NULL
};

/* {{{ yaf_functions[]
*/
zend_function_entry yaf_functions[] = {
	{NULL, NULL, NULL}
};
/* }}} */

void yaf_iterator_dtor(zend_object_iterator *iter) /* {{{ */ {
	zval_ptr_dtor(&iter->data);
	zval_ptr_dtor(&((yaf_iterator*)iter)->current);
	/* zend_iterator_dtor(iter); */
}
/* }}} */

#if PHP_VERSION_ID >= 70400
#if PHP_VERSION_ID >= 80000
HashTable *yaf_fake_get_gc(zend_object *zobj, zval **table, int *n) /* {{{ */ {
#else
HashTable *yaf_fake_get_gc(zval *zobj, zval **table, int *n) /* {{{ */ {
#endif
	*n = 0;
	*table = NULL;
	return NULL;
}
/* }}} */
#endif

static int yaf_iterator_valid(zend_object_iterator *iter) /* {{{ */ {
	return zend_hash_has_more_elements_ex(Z_ARRVAL(iter->data), &(((yaf_iterator*)iter)->pos));
}
/* }}} */

static void yaf_iterator_rewind(zend_object_iterator *iter) /* {{{ */ {
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL(iter->data), &(((yaf_iterator*)iter)->pos));
}
/* }}} */

static void yaf_iterator_move_forward(zend_object_iterator *iter) /* {{{ */ {
	zend_hash_move_forward_ex(Z_ARRVAL(iter->data), &(((yaf_iterator*)iter)->pos));
}
/* }}} */

static zval *yaf_iterator_get_current_data(zend_object_iterator *iter) /* {{{ */ {
	return zend_hash_get_current_data_ex(Z_ARRVAL(iter->data), &(((yaf_iterator*)iter)->pos));
}
/* }}} */

static void yaf_iterator_get_current_key(zend_object_iterator *iter, zval *key) /* {{{ */ {
	zend_ulong idx;
	zend_string *str;

	switch (zend_hash_get_current_key_ex(Z_ARRVAL(iter->data), &str, &idx, &(((yaf_iterator*)iter)->pos))) {
		case HASH_KEY_IS_STRING:
			ZVAL_STR_COPY(key, str);
			break;
		case HASH_KEY_IS_LONG:
			ZVAL_LONG(key, idx);
			break;
		default:
			ZVAL_NULL(key);
			break;
	}
}
/* }}} */

zend_object_iterator_funcs yaf_iterator_funcs = /* {{{ */ { 
	yaf_iterator_dtor,
	yaf_iterator_valid,
	yaf_iterator_get_current_data,
	yaf_iterator_get_current_key,
	yaf_iterator_move_forward,
	yaf_iterator_rewind,
	NULL
};
/* }}} */

static zend_bool yaf_ini_entry_is_true(const zend_string *new_value) /* {{{ */ {
	if (ZSTR_LEN(new_value) == 2 && strcasecmp("on", ZSTR_VAL(new_value)) == 0) {
		return 1;
	}
	else if (ZSTR_LEN(new_value) == 3 && strcasecmp("yes", ZSTR_VAL(new_value)) == 0) {
		return 1;
	}
	else if (ZSTR_LEN(new_value) == 4 && strcasecmp("true", ZSTR_VAL(new_value)) == 0) {
		return 1;
	}
	else {
		return (zend_bool) atoi(ZSTR_VAL(new_value));
	}
}
/* }}} */

PHP_INI_MH(OnUpdateForwardLimit) /* {{{ */ {
	int limit = atoi(ZSTR_VAL(new_value));
	if (limit >= 0) {
		YAF_VAR_FLAGS(YAF_G(loader)) = limit;
	}
	return SUCCESS;
}
/* }}} */

PHP_INI_MH(OnUpdateUseNamespace) /* {{{ */ {
	if (yaf_ini_entry_is_true(new_value)) {
		YAF_FLAGS() |= YAF_USE_NAMESPACE;
	} else {
		YAF_FLAGS() &= ~YAF_USE_NAMESPACE;
	}
	return SUCCESS;
}
/* }}} */

PHP_INI_MH(OnUpdateLowerCasePath) /* {{{ */ {
	if (yaf_ini_entry_is_true(new_value)) {
		YAF_FLAGS() |= YAF_LOWERCASE_PATH;
	} else {
		YAF_FLAGS() &= ~YAF_LOWERCASE_PATH;
	}
	return SUCCESS;
}
/* }}} */

PHP_INI_MH(OnUpdateActionPrefer) /* {{{ */ {
	if (yaf_ini_entry_is_true(new_value)) {
		YAF_FLAGS() |= YAF_ACTION_PREFER;
	} else {
		YAF_FLAGS() &= ~YAF_ACTION_PREFER;
	}
	return SUCCESS;
}
/* }}} */

PHP_INI_MH(OnUpdateUseSplAutoload) /* {{{ */ {
	if (yaf_ini_entry_is_true(new_value)) {
		YAF_FLAGS() |= YAF_USE_SPL_AUTOLOAD;
	} else {
		YAF_FLAGS() &= ~YAF_USE_SPL_AUTOLOAD;
	}
	return SUCCESS;
}
/* }}} */

PHP_INI_MH(OnUpdateNameSuffix) /* {{{ */ {
	if (yaf_ini_entry_is_true(new_value)) {
		YAF_FLAGS() |= YAF_NAME_SUFFIX;
	} else {
		YAF_FLAGS() &= ~YAF_NAME_SUFFIX;
	}
	return SUCCESS;
}
/* }}} */

PHP_INI_MH(OnUpdateSeparator) /* {{{ */ {
	YAF_G(name_separator) = ZSTR_VAL(new_value);
	YAF_G(name_separator_len) = ZSTR_LEN(new_value);
	if (ZSTR_LEN(new_value)) {
		YAF_FLAGS() |= YAF_HAS_NAME_SEPERATOR;
	} else {
		YAF_FLAGS() &= ~YAF_HAS_NAME_SEPERATOR;
	}
	return SUCCESS;
}
/* }}} */

/** {{{ PHP_YAF_INI_ENTRY
 */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("yaf.library",       "",  PHP_INI_ALL, OnUpdateString, global_library, zend_yaf_globals, yaf_globals)
	STD_PHP_INI_ENTRY("yaf.environ",       "product", PHP_INI_SYSTEM, OnUpdateString, environ_name, zend_yaf_globals, yaf_globals)
	PHP_INI_ENTRY("yaf.forward_limit",     "5", PHP_INI_ALL, OnUpdateForwardLimit)
	PHP_INI_ENTRY("yaf.use_namespace",     "0", PHP_INI_ALL, OnUpdateUseNamespace)
	PHP_INI_ENTRY("yaf.action_prefer",     "0", PHP_INI_ALL, OnUpdateActionPrefer)
	PHP_INI_ENTRY("yaf.lowcase_path",      "0", PHP_INI_ALL, OnUpdateLowerCasePath)
	PHP_INI_ENTRY("yaf.use_spl_autoload",  "0", PHP_INI_ALL, OnUpdateUseSplAutoload)
	PHP_INI_ENTRY("yaf.name_suffix",       "1", PHP_INI_ALL, OnUpdateNameSuffix)
	PHP_INI_ENTRY("yaf.name_separator",    "",  PHP_INI_ALL, OnUpdateSeparator)
PHP_INI_END();
/* }}} */

static zend_always_inline int yaf_do_call_user_method(zend_execute_data *call, zend_function *fbc, zval *ret) /* {{{ */ {
	/* At least we should in calls of Dispatchers */
	ZEND_ASSERT(EG(current_execute_data));

	zend_init_execute_data(call, (zend_op_array*)fbc, ret);
	/* const zend_op *current_opline_before_exception = EG(opline_before_exception); */
	zend_execute_ex(call);
	/* EG(opline_before_exception) = current_opline_before_exception; */

	zend_vm_stack_free_call_frame(call);
	if (UNEXPECTED(EG(exception))) {
		/* We should return directly to user codes */
		ZVAL_UNDEF(ret);
		return 0;
	}
	return 1;
}
/* }}} */

ZEND_HOT int yaf_call_user_method(zend_object *obj, zend_function* fbc, int num_arg, zval *args, zval *ret) /* {{{ */ {
	uint32_t i, call_info;
	zend_execute_data *call;

	if (UNEXPECTED(fbc->common.fn_flags & (ZEND_ACC_PROTECTED|ZEND_ACC_PRIVATE))) {
		php_error_docref(NULL, E_WARNING, "cannot call %s method %s::%s()", 
				(fbc->common.fn_flags & (ZEND_ACC_PRIVATE|ZEND_ACC_PROTECTED)) == ZEND_ACC_PROTECTED?
				"protected" : "private", ZSTR_VAL(obj->ce->name), ZSTR_VAL(fbc->common.function_name));
		return 0;
	}

#if PHP_VERSION_ID < 70400
	call_info = ZEND_CALL_TOP_FUNCTION;
	call = zend_vm_stack_push_call_frame(call_info, fbc, num_arg, NULL, obj);
#else
	call_info = ZEND_CALL_TOP_FUNCTION | ZEND_CALL_HAS_THIS;
	call = zend_vm_stack_push_call_frame(call_info, fbc, num_arg, obj);
#endif
	call->symbol_table = NULL;

	for (i = 0; i < num_arg; i++) {
		ZVAL_COPY(ZEND_CALL_ARG(call, i+1), &args[i]);
	}

	/* At least we should in calls of Dispatchers */
	ZEND_ASSERT(EG(current_execute_data));

	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION)) {
		return yaf_do_call_user_method(call, fbc, ret);
	} else {
		ZEND_ASSERT(fbc->type == ZEND_INTERNAL_FUNCTION);
		call->prev_execute_data = EG(current_execute_data);
		EG(current_execute_data) = call;
		if (EXPECTED(zend_execute_internal == NULL)) {
			fbc->internal_function.handler(call, ret);
		} else {
			zend_execute_internal(call, ret);
		}
		EG(current_execute_data) = call->prev_execute_data;
		zend_vm_stack_free_args(call);

		zend_vm_stack_free_call_frame(call);
		if (UNEXPECTED(EG(exception))) {
			/* We should return directly to user codes */
			ZVAL_UNDEF(ret);
			return 0;
		}
		return 1;
	}
}
/* }}} */

ZEND_HOT int yaf_call_user_method_with_0_arguments(zend_object *obj, zend_function* fbc, zval *ret) /* {{{ */ {
	uint32_t call_info;
	zend_execute_data *call;

	if (UNEXPECTED(fbc->common.fn_flags & (ZEND_ACC_PROTECTED|ZEND_ACC_PRIVATE))) {
		php_error_docref(NULL, E_WARNING, "cannot call %s method %s::%s()", 
				(fbc->common.fn_flags & (ZEND_ACC_PRIVATE|ZEND_ACC_PROTECTED)) == ZEND_ACC_PROTECTED?
				"protected" : "private", ZSTR_VAL(obj->ce->name), ZSTR_VAL(fbc->common.function_name));
		return 0;
	}

#if PHP_VERSION_ID < 70400
	call_info = ZEND_CALL_TOP_FUNCTION;
	call = zend_vm_stack_push_call_frame(call_info, fbc, 0, NULL, obj);
#else
	call_info = ZEND_CALL_TOP_FUNCTION | ZEND_CALL_HAS_THIS;
	call = zend_vm_stack_push_call_frame(call_info, fbc, 0, obj);
#endif
	call->symbol_table = NULL;

	return yaf_do_call_user_method(call, fbc, ret);
}
/* }}} */

ZEND_HOT int yaf_call_user_method_with_1_arguments(zend_object *obj, zend_function* fbc, zval *arg, zval *ret) /* {{{ */ {
	uint32_t call_info;
	zend_execute_data *call;

	if (UNEXPECTED(fbc->common.fn_flags & (ZEND_ACC_PROTECTED|ZEND_ACC_PRIVATE))) {
		php_error_docref(NULL, E_WARNING, "cannot call %s method %s::%s()", 
				(fbc->common.fn_flags & (ZEND_ACC_PRIVATE|ZEND_ACC_PROTECTED)) == ZEND_ACC_PROTECTED?
				"protected" : "private", ZSTR_VAL(obj->ce->name), ZSTR_VAL(fbc->common.function_name));
		return 0;
	}

#if PHP_VERSION_ID < 70400
	call_info = ZEND_CALL_TOP_FUNCTION;
	call = zend_vm_stack_push_call_frame(call_info, fbc, 1, NULL, obj);
#else
	call_info = ZEND_CALL_TOP_FUNCTION | ZEND_CALL_HAS_THIS;
	call = zend_vm_stack_push_call_frame(call_info, fbc, 1, obj);
#endif
	call->symbol_table = NULL;

	ZVAL_COPY(ZEND_CALL_ARG(call, 1), arg);

	return yaf_do_call_user_method(call, fbc, ret);
}
/* }}} */

ZEND_HOT int yaf_call_user_method_with_2_arguments(zend_object *obj, zend_function* fbc, zval *arg1, zval *arg2, zval *ret) /* {{{ */ {
	uint32_t call_info;
	zend_execute_data *call;

	if (UNEXPECTED(fbc->common.fn_flags & (ZEND_ACC_PROTECTED|ZEND_ACC_PRIVATE))) {
		php_error_docref(NULL, E_WARNING, "cannot call %s method %s::%s()", 
				(fbc->common.fn_flags & (ZEND_ACC_PRIVATE|ZEND_ACC_PROTECTED)) == ZEND_ACC_PROTECTED?
				"protected" : "private", ZSTR_VAL(obj->ce->name), ZSTR_VAL(fbc->common.function_name));
		return 0;
	}

#if PHP_VERSION_ID < 70400
	call_info = ZEND_CALL_TOP_FUNCTION;
	call = zend_vm_stack_push_call_frame(call_info, fbc, 2, NULL, obj);
#else
	call_info = ZEND_CALL_TOP_FUNCTION | ZEND_CALL_HAS_THIS;
	call = zend_vm_stack_push_call_frame(call_info, fbc, 2, obj);
#endif
	call->symbol_table = NULL;

	ZVAL_COPY(ZEND_CALL_ARG(call, 1), arg1);
	ZVAL_COPY(ZEND_CALL_ARG(call, 2), arg2);

	return yaf_do_call_user_method(call, fbc, ret);
}
/* }}} */

ZEND_HOT zend_string *yaf_build_camel_name(const char *str, size_t len) /* {{{ */ {
	unsigned int ucfirst = 1;
	zend_string *name = zend_string_alloc(len, 0);
	unsigned char *p = (unsigned char*)ZSTR_VAL(name);
	unsigned char *e = p + ZSTR_LEN(name);
#ifdef __SSE2__
	while (p + 16 <= e) {
		uint32_t mask;
		__m128i rot, lower, upper, delta, line;
		const __m128i upper_guard = _mm_set1_epi8('A' + (char)128);
		const __m128i lower_guard = _mm_set1_epi8('a' + (char)128);
		__m128i in = _mm_loadu_si128((__m128i*)str);

		rot = _mm_sub_epi8(in, upper_guard);
		upper = _mm_cmpgt_epi8(rot, _mm_set1_epi8((char)-128 + 'Z' - 'A'));
		delta = _mm_andnot_si128(upper, _mm_set1_epi8('a' - 'A'));
	    in = _mm_add_epi8(in, delta);

		line = _mm_cmpeq_epi8(in, _mm_set1_epi8('_'));
		mask = _mm_movemask_epi8(line);
		delta = _mm_slli_si128(line, 1);
		if (ucfirst) {
			delta = _mm_or_si128(delta, _mm_set_epi8(0, 0, 0, 0,
												     0, 0, 0, 0,
												     0, 0, 0, 0,
												     0, 0, 0, 0xff));
		}
		ucfirst = mask & (0x1 << 15);

		rot = _mm_sub_epi8(in, lower_guard);
		lower = _mm_cmpgt_epi8(rot, _mm_set1_epi8(-128 + 'z' - 'a'));;
		delta = _mm_andnot_si128(lower, delta);
		delta = _mm_and_si128(delta, _mm_set1_epi8('a' - 'A'));
		in = _mm_sub_epi8(in, delta);

		_mm_storeu_si128((__m128i*)p, in);
		p += 16;
		str += 16;
	}
#endif
	if (p != e) {
		if (ucfirst) {
			*p = toupper(*str++);
		} else {
			*p = tolower(*str++);
		}
		while (++p != e) {
			if (*(str - 1) != '_') {
				*p = tolower(*str++);
			} else {
				*p = toupper(*str++);
			}
		}
	}
	*p = '\0';

	return name;
}
/* }}} */

ZEND_HOT zend_string *yaf_build_lower_name(const char *str, size_t len) /* {{{ */ {
	unsigned int i;
	zend_string *name = zend_string_alloc(len, 0);
	unsigned char *p = (unsigned char*)ZSTR_VAL(name);

	for (i = 0; i < len; i++) {
		*p++ = tolower(str[i]);
	}
	*p = '\0';

	return name;
}
/* }}} */

ZEND_HOT zend_string *yaf_canonical_name(int type, zend_string *name) /* {{{ */ {
	if (type) {
#if 0
		const char *p = ZSTR_VAL(name);
		const char *e = ZSTR_VAL(name) + ZSTR_LEN(name);
		/* Module, Controller */
		if ((*p < 'A' || *p > 'Z') && *p != '_') {
			goto sanitize;
		}
		while (p++ != e) {
			if (((*p == '_' && (*(p + 1) >= 'a' || *(p + 1) <= 'z'))) || (*p >= 'A' && *p <= 'Z')) {
				goto sanitize;
			}
		}
		return zend_string_copy(name);
sanitize:
#endif
		return yaf_build_camel_name(ZSTR_VAL(name), ZSTR_LEN(name));
	} else {
		return zend_string_tolower(name);
	}
}
/* }}} */

ZEND_HOT void yaf_replace_chr(char *name, uint32_t len, zend_uchar f, zend_uchar t) /* {{{ */ {
	char *pos = name;
#ifdef __SSE2__
	do {
		const __m128i from = _mm_set1_epi8(f);
		const __m128i delta = _mm_set1_epi8(t - f);
		while (len >= 16) {
			__m128i op = _mm_loadu_si128((__m128i *)pos);
			__m128i eq = _mm_cmpeq_epi8(op, from);
			if (_mm_movemask_epi8(eq)) {
				eq = _mm_and_si128(eq, delta);
				op = _mm_add_epi8(op, eq);
				_mm_storeu_si128((__m128i*)pos, op);
			}
			len -= 16;
			pos += 16;
		}
	} while (0);
#endif
	if (len) {
		name = pos; /* reset start */
		while ((pos = memchr(pos, f, len - (pos - name)))) {
			*pos++ = t;
		}
	}
}
/* }}} */

/** {{{ PHP_GINIT_FUNCTION
*/
PHP_GINIT_FUNCTION(yaf)
{

	memset(yaf_globals, 0, sizeof(*yaf_globals));
}
/* }}} */

/** {{{ PHP_MINIT_FUNCTION
*/
PHP_MINIT_FUNCTION(yaf)
{
	uint32_t idx = 0;

	REGISTER_INI_ENTRIES();

    yaf_known_strings = malloc(sizeof(yaf_known_chars));
	while (yaf_known_chars[idx]) {
		yaf_known_strings[idx] = zend_string_init(yaf_known_chars[idx], strlen(yaf_known_chars[idx]), 1);
#if PHP_VERSION_ID < 70300
		zend_string_hash_val(yaf_known_strings[idx]);
		GC_REFCOUNT(yaf_known_strings[idx]) = 1;
		GC_FLAGS(yaf_known_strings[idx]) |= IS_STR_INTERNED | IS_STR_PERSISTENT;
#else
		zend_string_hash_func(yaf_known_strings[idx]);
		GC_SET_REFCOUNT(yaf_known_strings[idx], 1);
		GC_ADD_FLAGS(yaf_known_strings[idx], IS_STR_INTERNED | IS_STR_PERSISTENT);
#endif
		idx++;
	}

	if (yaf_is_use_namespace()) {

		REGISTER_STRINGL_CONSTANT("YAF\\VERSION", PHP_YAF_VERSION, 	sizeof(PHP_YAF_VERSION) - 1, CONST_PERSISTENT | CONST_CS);
		REGISTER_STRINGL_CONSTANT("YAF\\ENVIRON", YAF_G(environ_name), strlen(YAF_G(environ_name)), CONST_PERSISTENT | CONST_CS);

		REGISTER_LONG_CONSTANT("YAF\\ERR\\STARTUP_FAILED", 		YAF_ERR_STARTUP_FAILED, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF\\ERR\\ROUTE_FAILED", 		YAF_ERR_ROUTE_FAILED, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF\\ERR\\DISPATCH_FAILED", 	YAF_ERR_DISPATCH_FAILED, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF\\ERR\\AUTOLOAD_FAILED", 	YAF_ERR_AUTOLOAD_FAILED, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF\\ERR\\NOTFOUND\\MODULE", 	YAF_ERR_NOTFOUND_MODULE, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF\\ERR\\NOTFOUND\\CONTROLLER",YAF_ERR_NOTFOUND_CONTROLLER, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF\\ERR\\NOTFOUND\\ACTION", 	YAF_ERR_NOTFOUND_ACTION, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF\\ERR\\NOTFOUND\\VIEW", 		YAF_ERR_NOTFOUND_VIEW, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF\\ERR\\CALL_FAILED",			YAF_ERR_CALL_FAILED, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF\\ERR\\TYPE_ERROR",			YAF_ERR_TYPE_ERROR, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF\\ERR\\ACCESS",			    YAF_ERR_ACCESS_ERROR, CONST_PERSISTENT | CONST_CS);

	} else {
		REGISTER_STRINGL_CONSTANT("YAF_VERSION", PHP_YAF_VERSION, 	sizeof(PHP_YAF_VERSION) - 1, 	CONST_PERSISTENT | CONST_CS);
		REGISTER_STRINGL_CONSTANT("YAF_ENVIRON", YAF_G(environ_name),strlen(YAF_G(environ_name)), 	CONST_PERSISTENT | CONST_CS);

		REGISTER_LONG_CONSTANT("YAF_ERR_STARTUP_FAILED", 		YAF_ERR_STARTUP_FAILED, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF_ERR_ROUTE_FAILED", 			YAF_ERR_ROUTE_FAILED, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF_ERR_DISPATCH_FAILED", 		YAF_ERR_DISPATCH_FAILED, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF_ERR_AUTOLOAD_FAILED", 		YAF_ERR_AUTOLOAD_FAILED, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF_ERR_NOTFOUND_MODULE", 		YAF_ERR_NOTFOUND_MODULE, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF_ERR_NOTFOUND_CONTROLLER", 	YAF_ERR_NOTFOUND_CONTROLLER, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF_ERR_NOTFOUND_ACTION", 		YAF_ERR_NOTFOUND_ACTION, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF_ERR_NOTFOUND_VIEW", 		YAF_ERR_NOTFOUND_VIEW, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF_ERR_CALL_FAILED",			YAF_ERR_CALL_FAILED, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF_ERR_TYPE_ERROR",			YAF_ERR_TYPE_ERROR, CONST_PERSISTENT | CONST_CS);
		REGISTER_LONG_CONSTANT("YAF_ERR_ACCESS_ERROR",		    YAF_ERR_ACCESS_ERROR, CONST_PERSISTENT | CONST_CS);
	}

	/* startup components */
	YAF_STARTUP(application);
	YAF_STARTUP(bootstrap);
	YAF_STARTUP(dispatcher);
	YAF_STARTUP(loader);
	YAF_STARTUP(request);
	YAF_STARTUP(response);
	YAF_STARTUP(controller);
	YAF_STARTUP(action);
	YAF_STARTUP(config);
	YAF_STARTUP(view);
	YAF_STARTUP(router);
	YAF_STARTUP(plugin);
	YAF_STARTUP(registry);
	YAF_STARTUP(session);
	YAF_STARTUP(exception);

	return SUCCESS;
}
/* }}} */

/** {{{ PHP_MSHUTDOWN_FUNCTION
*/
PHP_MSHUTDOWN_FUNCTION(yaf)
{
	uint32_t idx = 0;

	UNREGISTER_INI_ENTRIES();

	for (idx; idx < sizeof(yaf_known_chars)/sizeof(char*) - 1; idx++) {
		free(yaf_known_strings[idx]);
	}
	free(yaf_known_strings);

	return SUCCESS;
}
/* }}} */

/** {{{ PHP_RINIT_FUNCTION
*/
PHP_RINIT_FUNCTION(yaf)
{
	YAF_FLAGS() |= YAF_THROW_EXCEPTION;
	YAF_FLAGS() &= ~YAF_CATCH_EXCEPTION;

	ZVAL_NULL(&YAF_G(app));
	ZVAL_NULL(&YAF_G(loader));
	ZVAL_NULL(&YAF_G(registry));
	ZVAL_NULL(&YAF_G(session));

	return SUCCESS;
}
/* }}} */

/** {{{ PHP_RSHUTDOWN_FUNCTION
*/
PHP_RSHUTDOWN_FUNCTION(yaf)
{
	zval_ptr_dtor(&YAF_G(registry));
	zval_ptr_dtor(&YAF_G(session));
	zval_ptr_dtor(&YAF_G(loader));
	zval_ptr_dtor(&YAF_G(app));

	return SUCCESS;
}
/* }}} */

/** {{{ PHP_MINFO_FUNCTION
*/
PHP_MINFO_FUNCTION(yaf)
{
	php_info_print_table_start();
	if (PG(expose_php) && !sapi_module.phpinfo_as_text) {
		php_info_print_table_header(2, "yaf support", YAF_LOGO_IMG"enabled");
	} else {
		php_info_print_table_header(2, "yaf support", "enabled");
	}

	php_info_print_table_row(2, "Version", PHP_YAF_VERSION);
	php_info_print_table_row(2, "Supports", YAF_SUPPORT_URL);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/** {{{ DL support
 */
#ifdef COMPILE_DL_YAF
ZEND_GET_MODULE(yaf)
#endif
/* }}} */

/** {{{ module depends
 */
#if ZEND_MODULE_API_NO >= 20050922
zend_module_dep yaf_deps[] = {
	ZEND_MOD_REQUIRED("spl")
	ZEND_MOD_REQUIRED("pcre")
	ZEND_MOD_OPTIONAL("session")
	{NULL, NULL, NULL}
};
#endif
/* }}} */

/** {{{ yaf_module_entry
*/
zend_module_entry yaf_module_entry = {
#if ZEND_MODULE_API_NO >= 20050922
	STANDARD_MODULE_HEADER_EX, NULL,
	yaf_deps,
#else
	STANDARD_MODULE_HEADER,
#endif
	"yaf",
	yaf_functions,
	PHP_MINIT(yaf),
	PHP_MSHUTDOWN(yaf),
	PHP_RINIT(yaf),
	PHP_RSHUTDOWN(yaf),
	PHP_MINFO(yaf),
	PHP_YAF_VERSION,
	PHP_MODULE_GLOBALS(yaf),
	PHP_GINIT(yaf),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
