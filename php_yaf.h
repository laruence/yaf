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

#define PHP_YAF_VERSION 					"3.3.6-dev"

#define YAF_STARTUP_FUNCTION(module)   	ZEND_MINIT_FUNCTION(yaf_##module)
#define YAF_RINIT_FUNCTION(module)		ZEND_RINIT_FUNCTION(yaf_##module)
#define YAF_STARTUP(module)	 		  	ZEND_MODULE_STARTUP_N(yaf_##module)(INIT_FUNC_ARGS_PASSTHRU)
#define YAF_SHUTDOWN_FUNCTION(module)  	ZEND_MSHUTDOWN_FUNCTION(yaf_##module)
#define YAF_SHUTDOWN(module)	 	    ZEND_MODULE_SHUTDOWN_N(yaf_##module)(INIT_FUNC_ARGS_PASSTHRU)
#define YAF_ME(c, m, a, f)              {m, PHP_MN(c), a, (unsigned)(sizeof(a)/sizeof(struct _zend_arg_info)-1), f},
#define YAF_VAR_FLAGS(v)                ((v).u2.next)

#if PHP_VERSION_ID < 70200
#define YAF_ALLOW_VIOLATION(ht)
#else
#define YAF_ALLOW_VIOLATION(ht) do { \
	zend_hash_real_init(ht, 0);      \
	HT_ALLOW_COW_VIOLATION(ht);      \
} while (0)
#endif

#if PHP_VERSION_ID < 70400
#define YAF_WRITE_HANDLER       void
#define YAF_WHANDLER_RET(zv)    return
# if PHP_VERSION_ID < 70300
# define GC_ADDREF(gc)           (++GC_REFCOUNT(gc))
# define GC_DELREF(gc)           (--GC_REFCOUNT(gc))
# endif
# define yaf_fake_get_gc        NULL
#else
#define YAF_WRITE_HANDLER       zval *
#define YAF_WHANDLER_RET(zv)    return zv
#if PHP_VERSION_ID >= 80000
HashTable *yaf_fake_get_gc(zend_object *zobj, zval **table, int *n);
#else
HashTable *yaf_fake_get_gc(zval *zobj, zval **table, int *n);
#endif
#endif

#if PHP_VERSION_ID < 80000
#define yaf_object zval
#define yaf_strip_obj(o) Z_OBJ_P(o)
#else
#define yaf_object zend_object
#define yaf_strip_obj
#endif

#define yaf_application_t       zval
#define yaf_view_t              zval
#define yaf_controller_t        zval
#define yaf_request_t           zval
#define yaf_router_t            zval
#define yaf_route_t             zval
#define yaf_dispatcher_t        zval
#define yaf_action_t            zval
#define yaf_loader_t            zval
#define yaf_response_t          zval
#define yaf_config_t            zval
#define yaf_registry_t          zval
#define yaf_plugin_t            zval
#define yaf_session_t           zval
#define yaf_exception_t         zval

#define YAF_USE_SPL_AUTOLOAD    (1<<0)
#define YAF_LOWERCASE_PATH      (1<<1)
#define YAF_NAME_SUFFIX         (1<<2)
#define YAF_HAS_NAME_SEPERATOR  (1<<3)
#define YAF_USE_NAMESPACE       (1<<4)
#define YAF_ACTION_PREFER       (1<<5)
#define YAF_THROW_EXCEPTION     (1<<6)
#define YAF_CATCH_EXCEPTION     (1<<7)

#define YAF_FLAGS()             (YAF_VAR_FLAGS(YAF_G(app)))

ZEND_BEGIN_MODULE_GLOBALS(yaf)
	/* for instances stash, and flags */
	yaf_application_t app;
    yaf_loader_t      loader;
	yaf_registry_t    registry;
	yaf_session_t     session;

	/* ini configurations */
    char             *name_separator;
    size_t            name_separator_len;
    char             *global_library;
    char             *environ_name;

    /*for ini parsing */
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
zend_string *yaf_build_camel_name(const char *str, size_t len);
zend_string *yaf_build_lower_name(const char *str, size_t len);
int yaf_call_user_method(zend_object *obj, zend_function *fbc, int num_args, zval *args, zval *ret);
int yaf_call_user_method_with_0_arguments(zend_object *obj, zend_function *fbc, zval *ret);
int yaf_call_user_method_with_1_arguments(zend_object *obj, zend_function *fbc, zval *arg, zval *ret);
int yaf_call_user_method_with_2_arguments(zend_object *obj, zend_function *fbc, zval *arg1, zval *arg2, zval *ret);
void yaf_replace_chr(char *name, uint32_t len, zend_uchar f, zend_uchar t);

extern const char const *yaf_known_chars[];
extern zend_string **yaf_known_strings;
#define YAF_KNOWN_STR(id)        (yaf_known_strings[id])
#define YAF_KNOWN_CHARS(id)      (yaf_known_chars[id])
#define YAF_KNOWN_NAMES(_) \
	_(YAF,                       "yaf") \
	_(YAF_APPLICATION,           "application") \
	_(YAF_DIRECTORY,             "directory") \
	_(YAF_DISPATCHER,            "dispatcher") \
	_(YAF_DEFAULT_MODULE,        "Index") \
	_(YAF_DEFAULT_CONTROLLER,    "Index") \
	_(YAF_DEFAULT_ACTION,        "index") \
	_(YAF_ACTIONS_MAP,           "actions") \
	_(YAF_BOOTSTRAP,             "bootstrap") \
	_(YAF_CONTENT,               "content") \
	_(YAF_VAR_POST,              "_POST") \
	_(YAF_VAR_GET,               "_GET") \
	_(YAF_VAR_SERVER,            "_SERVER") \
	_(YAF_VAR_COOKIE,            "_COOKIE") \
	_(YAF_VAR_FILES,             "_FILES") \
	_(YAF_VAR_REQUEST,           "_REQUEST") \
	_(YAF_VAR_ENV,               "_ENV") \
	_(YAF_VAR_SESSION,           "_SESSION") \
	_(YAF_HOOK_ROUTESTARTUP,     "routerstartup") \
	_(YAF_HOOK_ROUTESHUTDOWN,    "routershutdown") \
	_(YAF_HOOK_LOOPSTARTUP,      "dispatchloopstartup") \
	_(YAF_HOOK_PREDISPATCH,      "predispatch") \
	_(YAF_HOOK_POSTDISPATCH,     "postdispatch") \
	_(YAF_HOOK_LOOPSHUTDOWN,     "dispatchloopshutdown") \
	_(YAF_AUTORENDER,            "yafAutoRender") \
	_(YAF_RENDER,                "render") \
	_(YAF_DISPLAY,               "display") \
	_(YAF_EXECUTE,               "execute") \
	_(YAF_PATH_INFO,             "PATH_INFO") \
	_(YAF_REQUEST_URI,           "REQUEST_URI") \

enum _yaf_known_chars_id {
#define _YAF_CHARS_ID(id, str) id,
YAF_KNOWN_NAMES(_YAF_CHARS_ID)
#undef _YAF_CHARS_ID
	YAF_LAST_KNOWN_CHARS
};

#define YSCMP(a, b, l, s)  do { \
	if (l>=sizeof(uint##s##_t)) { \
		if (*(uint##s##_t*)a != *(uint##s##_t*)b) return 0; \
		l-=sizeof(uint##s##_t),a+=sizeof(uint##s##_t),b+=sizeof(uint##s##_t); \
	}\
} while (0)
static zend_always_inline int yaf_slip_equal(const char *s, const char *p, unsigned char l) {
	ZEND_ASSERT(l < 16);
#if SIZEOF_ZEND_LONG == 8
	YSCMP(s, p, l, 64);
#else
	YSCMP(s, p, l, 32); YSCMP(s, p, l, 32);
#endif
	YSCMP(s, p, l, 32);
   	YSCMP(s, p, l, 16);
	return (l == 0 || *s == *p);
}

static zend_always_inline unsigned int yaf_get_forward_limit() {
	return YAF_VAR_FLAGS(YAF_G(loader));
}
static zend_always_inline zend_bool yaf_is_use_namespace() {
	return YAF_FLAGS() & YAF_USE_NAMESPACE;
}
static zend_always_inline zend_bool yaf_is_action_prefer() {
	return YAF_FLAGS() & YAF_ACTION_PREFER;
}
static zend_always_inline zend_bool yaf_is_name_suffix() {
	return YAF_FLAGS() & YAF_NAME_SUFFIX;
}
static zend_always_inline zend_bool yaf_has_name_separator() {
	return YAF_FLAGS() & YAF_HAS_NAME_SEPERATOR;
}
static zend_always_inline zend_bool yaf_is_throw_exception() {
	return YAF_FLAGS() & YAF_THROW_EXCEPTION;
}
static zend_always_inline zend_bool yaf_is_catch_exception() {
	return YAF_FLAGS() & YAF_CATCH_EXCEPTION;
}
static zend_always_inline void yaf_set_throw_exception(zend_bool flag) {
	if (flag) {
		YAF_FLAGS() |= YAF_THROW_EXCEPTION;
	} else {
		YAF_FLAGS() &= ~YAF_THROW_EXCEPTION;
	}
}
static zend_always_inline void yaf_set_catch_exception(zend_bool flag) {
	if (flag) {
		YAF_FLAGS() |= YAF_CATCH_EXCEPTION;
	} else {
		YAF_FLAGS() &= ~YAF_CATCH_EXCEPTION;
	}
}
static zend_always_inline uint32_t yaf_compose_2_pathes(char *buf, zend_string *c1, const char *c2, uint32_t l2) {
	uint32_t len = ZSTR_LEN(c1);
	memcpy(buf, ZSTR_VAL(c1), len);
	buf[len] = DEFAULT_SLASH;
	memcpy(buf + len + 1, c2, l2);
	len = len + l2 + 1;
	return len;
}
#endif

/* removed in 8.3 */
#ifndef ZEND_HOT
#define ZEND_HOT
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
