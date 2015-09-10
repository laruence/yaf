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
/* {{{ This only effects internally */
	STD_PHP_INI_BOOLEAN("yaf.st_compatible",     "0", PHP_INI_ALL, OnUpdateBool, st_compatible, zend_yaf_globals, yaf_globals)
/* }}} */
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

	if (YAF_G(configs)) {
		zend_hash_destroy(YAF_G(configs));
		pefree(YAF_G(configs), 1);
	}

	return SUCCESS;
}
/* }}} */

/** {{{ PHP_RINIT_FUNCTION
*/
PHP_RINIT_FUNCTION(yaf)
{
	YAF_G(throw_exception) = 1;
	YAF_G(ext) = zend_string_init(YAF_DEFAULT_EXT, sizeof(YAF_DEFAULT_EXT) - 1, 0);
	YAF_G(view_ext) = zend_string_init(YAF_DEFAULT_VIEW_EXT, sizeof(YAF_DEFAULT_VIEW_EXT) - 1, 0);
	YAF_G(default_module) = zend_string_init(
			YAF_ROUTER_DEFAULT_MODULE, sizeof(YAF_ROUTER_DEFAULT_MODULE) - 1, 0);
	YAF_G(default_controller) = zend_string_init(
			YAF_ROUTER_DEFAULT_CONTROLLER, sizeof(YAF_ROUTER_DEFAULT_CONTROLLER) - 1, 0);
	YAF_G(default_action) = zend_string_init(
			YAF_ROUTER_DEFAULT_ACTION, sizeof(YAF_ROUTER_DEFAULT_ACTION) - 1, 0);
	return SUCCESS;
}
/* }}} */

/** {{{ PHP_RSHUTDOWN_FUNCTION
*/
PHP_RSHUTDOWN_FUNCTION(yaf)
{
	YAF_G(running) = 0;
	YAF_G(in_exception)	= 0;
	YAF_G(catch_exception) = 0;

	if (YAF_G(directory)) {
		zend_string_release(YAF_G(directory));
		YAF_G(directory) = NULL;
	}
	if (YAF_G(local_library)) {
		zend_string_release(YAF_G(local_library));
		YAF_G(local_library) = NULL;
	}
	if (YAF_G(local_namespaces)) {
		zend_string_release(YAF_G(local_namespaces));
		YAF_G(local_namespaces) = NULL;
	}
	if (YAF_G(bootstrap)) {
		zend_string_release(YAF_G(bootstrap));
		YAF_G(bootstrap) = NULL;
	}
	if (Z_TYPE(YAF_G(modules)) == IS_ARRAY) {
		zval_ptr_dtor(&YAF_G(modules));
		ZVAL_UNDEF(&YAF_G(modules));
	}
	if (YAF_G(base_uri)) {
		zend_string_release(YAF_G(base_uri));
		YAF_G(base_uri) = NULL;
	}
	if (YAF_G(view_directory)) {
		zend_string_release(YAF_G(view_directory));
		YAF_G(view_directory) = NULL;
	}
	if (YAF_G(view_ext)) {
		zend_string_release(YAF_G(view_ext));
	}
	if (YAF_G(default_module)) {
		zend_string_release(YAF_G(default_module));
	}
	if (YAF_G(default_controller)) {
		zend_string_release(YAF_G(default_controller));
	}
	if (YAF_G(default_action)) {
		zend_string_release(YAF_G(default_action));
	}
	if (YAF_G(ext)) {
		zend_string_release(YAF_G(ext));
	}
	YAF_G(default_route) = NULL;

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
