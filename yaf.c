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

zend_string *yaf_canonical_name(int type, zend_string *name) /* {{{ */ {
	zend_string *canocical;
	const char *p = ZSTR_VAL(name);
	const char *e = ZSTR_VAL(name) + ZSTR_LEN(name);

	if (type) {
		/* Module, Controller */
		if ((*p < 'A' || *p > 'Z') && *p != '_') {
			goto sanitize;
		}
		while (p++ != e) {
			if (*p >= 'A' && *p <= 'Z') {
				goto sanitize;
			}
		}
		return zend_string_copy(name);
sanitize:
		canocical = zend_string_init(ZSTR_VAL(name), ZSTR_LEN(name), 0);
		zend_str_tolower(ZSTR_VAL(canocical), ZSTR_LEN(canocical));
		*ZSTR_VAL(canocical) = toupper(*ZSTR_VAL(canocical));
		return canocical;
	} else {
		return zend_string_tolower(name);
	}
}
/* }}} */

/** {{{ PHP_INI_MH(OnUpdateSeparator)
 */
PHP_INI_MH(OnUpdateSeparator) {
	YAF_G(name_separator) = ZSTR_VAL(new_value);
	YAF_G(name_separator_len) = ZSTR_LEN(new_value);
	return SUCCESS;
}
/* }}} */

/** {{{ PHP_INI
 */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("yaf.library",         	"",  PHP_INI_ALL, OnUpdateString, global_library, zend_yaf_globals, yaf_globals)
	STD_PHP_INI_BOOLEAN("yaf.action_prefer",   	"0", PHP_INI_ALL, OnUpdateBool, action_prefer, zend_yaf_globals, yaf_globals)
	STD_PHP_INI_BOOLEAN("yaf.lowcase_path",    	"0", PHP_INI_ALL, OnUpdateBool, lowcase_path, zend_yaf_globals, yaf_globals)
	STD_PHP_INI_BOOLEAN("yaf.use_spl_autoload", "0", PHP_INI_ALL, OnUpdateBool, use_spl_autoload, zend_yaf_globals, yaf_globals)
	STD_PHP_INI_ENTRY("yaf.forward_limit", 		"5", PHP_INI_ALL, OnUpdateLongGEZero, forward_limit, zend_yaf_globals, yaf_globals)
	STD_PHP_INI_BOOLEAN("yaf.name_suffix", 		"1", PHP_INI_ALL, OnUpdateBool, name_suffix, zend_yaf_globals, yaf_globals)
	PHP_INI_ENTRY("yaf.name_separator", 		"",  PHP_INI_ALL, OnUpdateSeparator)
	STD_PHP_INI_ENTRY("yaf.environ",        	"product", PHP_INI_SYSTEM, OnUpdateString, environ_name, zend_yaf_globals, yaf_globals)
	STD_PHP_INI_BOOLEAN("yaf.use_namespace",   	"0", PHP_INI_SYSTEM, OnUpdateBool, use_namespace, zend_yaf_globals, yaf_globals)
PHP_INI_END();
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
	REGISTER_INI_ENTRIES();

	if (YAF_G(use_namespace)) {

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
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/** {{{ PHP_RINIT_FUNCTION
*/
PHP_RINIT_FUNCTION(yaf)
{
	YAF_G(throw_exception) = 1;
	YAF_G(catch_exception) = 0;

	ZVAL_UNDEF(&YAF_G(registry));
	ZVAL_UNDEF(&YAF_G(session));
	ZVAL_UNDEF(&YAF_G(loader));
	ZVAL_UNDEF(&YAF_G(app));

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
