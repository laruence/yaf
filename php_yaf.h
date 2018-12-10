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

#ifndef PHP_YAF_H
#define PHP_YAF_H

extern zend_module_entry yaf_module_entry;
#define phpext_yaf_ptr &yaf_module_entry

#ifdef PHP_WIN32
#define PHP_YAF_API __declspec(dllexport)
#ifndef _MSC_VER
#define _MSC_VER 1600
#endif
#else
#define PHP_YAF_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef ZTS
#define YAF_G(v) TSRMG(yaf_globals_id, zend_yaf_globals *, v)
#else
#define YAF_G(v) (yaf_globals.v)
#endif

#ifndef ZEND_ACC_CTOR
# define ZEND_ACC_CTOR	0x0
# define ZEND_ACC_DTOR	0x0
#endif

#define PHP_YAF_VERSION 					"3.0.9-dev"

#define YAF_STARTUP_FUNCTION(module)   	ZEND_MINIT_FUNCTION(yaf_##module)
#define YAF_RINIT_FUNCTION(module)		ZEND_RINIT_FUNCTION(yaf_##module)
#define YAF_STARTUP(module)	 		  	ZEND_MODULE_STARTUP_N(yaf_##module)(INIT_FUNC_ARGS_PASSTHRU)
#define YAF_SHUTDOWN_FUNCTION(module)  	ZEND_MSHUTDOWN_FUNCTION(yaf_##module)
#define YAF_SHUTDOWN(module)	 	    ZEND_MODULE_SHUTDOWN_N(yaf_##module)(INIT_FUNC_ARGS_PASSTHRU)

#define yaf_application_t	zval
#define yaf_view_t 			zval
#define yaf_controller_t	zval
#define yaf_request_t		zval
#define yaf_router_t		zval
#define yaf_route_t			zval
#define yaf_dispatcher_t	zval
#define yaf_action_t		zval
#define yaf_loader_t		zval
#define yaf_response_t		zval
#define yaf_config_t		zval
#define yaf_registry_t		zval
#define yaf_plugin_t		zval
#define yaf_session_t		zval
#define yaf_exception_t		zval

#define YAF_ME(c, m, a, f) {m, PHP_MN(c), a, (uint) (sizeof(a)/sizeof(struct _zend_arg_info)-1), f},

extern PHPAPI void php_var_dump(zval **struc, int level);
extern PHPAPI void php_debug_zval_dump(zval **struc, int level);

ZEND_BEGIN_MODULE_GLOBALS(yaf)
	zend_string	*ext;
	zend_string *base_uri;
	zend_string *directory;
	zend_string *local_library;
	zend_string *local_namespaces;
	zend_string *view_directory;
	zend_string *view_ext;
	zend_string *default_module;
	zend_string *default_controller;
	zend_string *default_action;
	zend_string *bootstrap;
	char         *global_library;
    char         *environ_name;
    char         *name_separator;
    size_t        name_separator_len;
	zend_bool 	lowcase_path;
	zend_bool 	use_spl_autoload;
	zend_bool 	throw_exception;
	zend_bool   action_prefer;
	zend_bool	name_suffix;
	zend_bool  	autoload_started;
	zend_bool  	running;
	zend_bool  	in_exception;
	zend_bool  	catch_exception;
	zend_bool   suppressing_warning;
/* {{{ This only effects internally */
	zend_bool  	st_compatible;
/* }}} */
	long		forward_limit;
	HashTable	*configs;
	zval 		 modules;
	zval        *default_route;
	zval        active_ini_file_section;
	zval        *ini_wanted_section;
	uint        parsing_flag;
	zend_bool	use_namespace;
ZEND_END_MODULE_GLOBALS(yaf)

PHP_MINIT_FUNCTION(yaf);
PHP_MSHUTDOWN_FUNCTION(yaf);
PHP_RINIT_FUNCTION(yaf);
PHP_RSHUTDOWN_FUNCTION(yaf);
PHP_MINFO_FUNCTION(yaf);

extern ZEND_DECLARE_MODULE_GLOBALS(yaf);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
