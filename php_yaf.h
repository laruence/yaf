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

#define PHP_YAF_VERSION 					"3.1.4-dev"

#define YAF_STARTUP_FUNCTION(module)   	ZEND_MINIT_FUNCTION(yaf_##module)
#define YAF_RINIT_FUNCTION(module)		ZEND_RINIT_FUNCTION(yaf_##module)
#define YAF_STARTUP(module)	 		  	ZEND_MODULE_STARTUP_N(yaf_##module)(INIT_FUNC_ARGS_PASSTHRU)
#define YAF_SHUTDOWN_FUNCTION(module)  	ZEND_MSHUTDOWN_FUNCTION(yaf_##module)
#define YAF_SHUTDOWN(module)	 	    ZEND_MODULE_SHUTDOWN_N(yaf_##module)(INIT_FUNC_ARGS_PASSTHRU)
#define YAF_ME(c, m, a, f)              {m, PHP_MN(c), a, (unsigned)(sizeof(a)/sizeof(struct _zend_arg_info)-1), f},

#define yaf_application_t  zval
#define yaf_view_t         zval
#define yaf_controller_t   zval
#define yaf_request_t      zval
#define yaf_router_t       zval
#define yaf_route_t        zval
#define yaf_dispatcher_t   zval
#define yaf_action_t       zval
#define yaf_loader_t       zval
#define yaf_response_t     zval
#define yaf_config_t       zval
#define yaf_registry_t     zval
#define yaf_plugin_t       zval
#define yaf_session_t      zval
#define yaf_exception_t    zval

ZEND_BEGIN_MODULE_GLOBALS(yaf)
	/* for instances stash */
	yaf_application_t app;
    yaf_loader_t      loader;
	yaf_registry_t    registry;
	yaf_session_t     session;

	/* ini configurations */
    char             *global_library;
    char             *environ_name;
    char             *name_separator;
    size_t            name_separator_len;
    zend_bool         action_prefer;
    zend_bool         name_suffix;
    zend_bool         lowcase_path;
    zend_bool         use_spl_autoload;
    zend_bool         use_namespace;
    zend_bool         in_exception;
	zend_bool         throw_exception;
	zend_bool         catch_exception;
    unsigned int      forward_limit;

    /*for ini parsing */
    unsigned int      parsing_flag;
    zval              active_ini_file_section;
    zval             *ini_wanted_section;
ZEND_END_MODULE_GLOBALS(yaf)

PHP_MINIT_FUNCTION(yaf);
PHP_MSHUTDOWN_FUNCTION(yaf);
PHP_RINIT_FUNCTION(yaf);
PHP_RSHUTDOWN_FUNCTION(yaf);
PHP_MINFO_FUNCTION(yaf);

typedef struct {
	zend_object_iterator intern;
	zval current;
	HashPosition pos;
} yaf_iterator;

extern ZEND_DECLARE_MODULE_GLOBALS(yaf);
extern zend_object_iterator_funcs yaf_iterator_funcs;

zend_string *yaf_canonical_name(int type, zend_string *name);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
