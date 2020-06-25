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

#ifndef PHP_YAF_DISPATCHER_H
#define PHP_YAF_DISPATCHER_H

#define YAF_ERROR_CONTROLLER                    "Error"
#define YAF_ERROR_ACTION                        "error"

#define YAF_PLUGIN_HOOK_ROUTESTARTUP            "routerstartup"
#define YAF_PLUGIN_HOOK_ROUTESHUTDOWN           "routershutdown"
#define YAF_PLUGIN_HOOK_LOOPSTARTUP             "dispatchloopstartup"
#define YAF_PLUGIN_HOOK_PREDISPATCH             "predispatch"
#define YAF_PLUGIN_HOOK_POSTDISPATCH            "postdispatch"
#define YAF_PLUGIN_HOOK_LOOPSHUTDOWN            "dispatchloopshutdown"
#define YAF_PLUGIN_HOOK_PRERESPONSE             "preresponse"

extern zend_class_entry *yaf_dispatcher_ce;

#define YAF_DISPATCHER_AUTO_RENDER       (1<<0)
#define YAF_DISPATCHER_INSTANT_FLUSH     (1<<1)
#define YAF_DISPATCHER_RETURN_RESPONSE   (1<<2)
#define YAF_DISPATCHER_IN_EXCEPTION      (1<<7)

#define YAF_DISPATCHER_FLAGS(d)   YAF_VAR_FLAGS((d)->request)

typedef struct {
	yaf_request_t      request;
	yaf_response_t     response;
	yaf_router_t       router;
	yaf_view_t         view;
	zend_array        *plugins;
	zend_array        *properties;
	zend_object        std;
} yaf_dispatcher_object;

#define Z_YAFDISPATCHEROBJ(zv)   (php_yaf_dispatcher_fetch_object(Z_OBJ(zv)))
#define Z_YAFDISPATCHEROBJ_P(zv) Z_YAFDISPATCHEROBJ(*zv)

static zend_always_inline yaf_dispatcher_object *php_yaf_dispatcher_fetch_object(zend_object *obj) {
	return (yaf_dispatcher_object *)((char*)(obj) - XtOffsetOf(yaf_dispatcher_object, std));
}

#define YAF_PLUGIN_HANDLE(dispatcher, ev) \
	do { \
		if ((dispatcher)->plugins) { \
			zval _r, *_t;\
			zend_function *_f; \
			yaf_dispatcher_object *_d = (dispatcher); \
			HashTable *_pls = _d->plugins; \
			ZEND_HASH_FOREACH_VAL(_pls, _t) { \
				_f = zend_hash_str_find_ptr(&(Z_OBJCE_P(_t)->function_table), (ev), sizeof(ev) - 1); \
				ZEND_ASSERT(_f); \
				if (_f->type == ZEND_USER_FUNCTION) {\
			        if (!yaf_call_user_method_with_2_arguments(Z_OBJ_P(_t), _f, &_d->request, &_d->response, &_r)) { \
						YAF_EXCEPTION_HANDLE(_d); \
					} \
				} \
			} ZEND_HASH_FOREACH_END(); \
		} \
	} while(0)


void yaf_dispatcher_instance(yaf_dispatcher_t *this_ptr);
yaf_response_t *yaf_dispatcher_dispatch(yaf_dispatcher_object *dispatcher);

PHP_METHOD(yaf_application, app);
PHP_FUNCTION(set_error_handler);
YAF_STARTUP_FUNCTION(dispatcher);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
