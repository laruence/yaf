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

#define YAF_DISPATCHER_PROPERTY_NAME_REQUEST    "_request"
#define YAF_DISPATCHER_PROPERTY_NAME_VIEW       "_view"
#define YAF_DISPATCHER_PROPERTY_NAME_ROUTER     "_router"
#define YAF_DISPATCHER_PROPERTY_NAME_INSTANCE   "_instance"
#define YAF_DISPATCHER_PROPERTY_NAME_RENDER     "_auto_render"
#define YAF_DISPATCHER_PROPERTY_NAME_RETURN     "_return_response"
#define YAF_DISPATCHER_PROPERTY_NAME_FLUSH      "_instantly_flush"
#define YAF_DISPATCHER_PROPERTY_NAME_ARGS       "_invoke_args"

#define YAF_DISPATCHER_PROPERTY_NAME_MODULE     "_default_module"
#define YAF_DISPATCHER_PROPERTY_NAME_CONTROLLER "_default_controller"
#define YAF_DISPATCHER_PROPERTY_NAME_ACTION     "_default_action"

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

#define YAF_PLUGIN_HANDLE(p, n, request, response) \
	do { \
		if (!ZVAL_IS_NULL(p)) { \
			zval *_t_plugin;\
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(p), _t_plugin) { \
			    if (zend_hash_str_exists(&(Z_OBJCE_P(_t_plugin)->function_table), n, sizeof(n) - 1)) { \
			        zend_call_method_with_2_params(_t_plugin, Z_OBJCE_P(_t_plugin), NULL, n, NULL, request, response); \
				} \
			} ZEND_HASH_FOREACH_END(); \
		} \
	} while(0)

extern zend_class_entry *yaf_dispatcher_ce;

yaf_dispatcher_t *yaf_dispatcher_instance(yaf_dispatcher_t *this_ptr);
yaf_response_t *yaf_dispatcher_dispatch(yaf_dispatcher_t *dispatcher, zval *response_ptr);
int yaf_dispatcher_set_request(yaf_dispatcher_t *dispatcher, yaf_request_t *request);

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
