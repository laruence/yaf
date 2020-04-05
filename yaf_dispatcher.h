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

#define YAF_DISPATCHER_PROPERTY_NAME_PLUGINS    "_plugins"

#define YAF_PLUGIN_HOOK_ROUTESTARTUP            "routerstartup"
#define YAF_PLUGIN_HOOK_ROUTESHUTDOWN           "routershutdown"
#define YAF_PLUGIN_HOOK_LOOPSTARTUP             "dispatchloopstartup"
#define YAF_PLUGIN_HOOK_PREDISPATCH             "predispatch"
#define YAF_PLUGIN_HOOK_POSTDISPATCH            "postdispatch"
#define YAF_PLUGIN_HOOK_LOOPSHUTDOWN            "dispatchloopshutdown"
#define YAF_PLUGIN_HOOK_PRERESPONSE             "preresponse"

#define YAF_DISPATCHER_CHECK_MODULE	1
#define YAF_DISPATCHER_CHECK_NONE	0

#define YAF_PLUGIN_HANDLE(dispatcher, ev) \
	do { \
		yaf_dispatcher_object *_d = (dispatcher); \
		if (zend_hash_num_elements(&_d->plugins)) { \
			zval *_t;\
			ZEND_HASH_FOREACH_VAL(&_d->plugins, _t) { \
			    if (zend_hash_str_exists(&(Z_OBJCE_P(_t)->function_table), (ev), sizeof(ev) - 1)) { \
			        zend_call_method_with_2_params(_t, Z_OBJCE_P(_t), NULL, (ev), NULL, &_d->request, &_d->response); \
				} \
			} ZEND_HASH_FOREACH_END(); \
		} \
	} while(0)

extern zend_class_entry *yaf_dispatcher_ce;

typedef struct {
	zend_object        std;
	zend_array         plugins;
	yaf_request_t      request;
	yaf_response_t     response;
	yaf_router_t       router;
	yaf_view_t         view;
	zend_bool          auto_render;
	zend_bool          instantly_flush;
	zend_bool          return_response;
} yaf_dispatcher_object;

#define Z_YAFDISPATCHEROBJ(zv)   ((yaf_dispatcher_object*)(Z_OBJ(zv)))
#define Z_YAFDISPATCHEROBJ_P(zv) Z_YAFDISPATCHEROBJ(*zv)

void yaf_dispatcher_instance(yaf_dispatcher_t *this_ptr);
yaf_response_t *yaf_dispatcher_dispatch(yaf_dispatcher_object *dispatcher);
int yaf_dispatcher_set_request(yaf_dispatcher_object *dispatcher, yaf_request_t *request);

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
