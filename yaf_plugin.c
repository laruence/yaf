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

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_plugin.h"

#include "yaf_plugin_arginfo.h"

zend_class_entry * yaf_plugin_ce;

/** {{{ proto public Yaf_Plugin::routerStartup(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response)
*/
PHP_METHOD(yaf_plugin, routerStartup) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Plugin::routerShutdown(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response)
*/
PHP_METHOD(yaf_plugin, routerShutdown) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Plugin::dispatchLoopStartup(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response)
*/
PHP_METHOD(yaf_plugin, dispatchLoopStartup) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Plugin::preDispatch(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response)
*/
PHP_METHOD(yaf_plugin, preDispatch) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Plugin::postDispatch(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response)
*/
PHP_METHOD(yaf_plugin, postDispatch) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Plugin::dispatchLoopShutdown(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response)
*/
PHP_METHOD(yaf_plugin, dispatchLoopShutdown) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Plugin::preResponse(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response)
*/
PHP_METHOD(yaf_plugin, preResponse) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ yaf_plugin_methods
*/
zend_function_entry yaf_plugin_methods[] = {
	PHP_ME(yaf_plugin, routerStartup, arginfo_class_Yaf_Plugin_Abstract_routerStartup, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, routerShutdown, arginfo_class_Yaf_Plugin_Abstract_routerShutdown, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, dispatchLoopStartup, arginfo_class_Yaf_Plugin_Abstract_dispatchLoopStartup, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, dispatchLoopShutdown, arginfo_class_Yaf_Plugin_Abstract_dispatchLoopShutdown, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, preDispatch, arginfo_class_Yaf_Plugin_Abstract_preDispatch, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, postDispatch, arginfo_class_Yaf_Plugin_Abstract_postDispatch, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, preResponse, arginfo_class_Yaf_Plugin_Abstract_preResponse, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

zend_function_entry yaf_plugin_methods_ns[] = {
	PHP_ME(yaf_plugin, routerStartup, arginfo_class_NS_Yaf_Plugin_Abstract_routerStartup, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, routerShutdown, arginfo_class_NS_Yaf_Plugin_Abstract_routerShutdown, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, dispatchLoopStartup, arginfo_class_NS_Yaf_Plugin_Abstract_dispatchLoopStartup, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, dispatchLoopShutdown, arginfo_class_NS_Yaf_Plugin_Abstract_dispatchLoopShutdown, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, preDispatch, arginfo_class_NS_Yaf_Plugin_Abstract_preDispatch, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, postDispatch, arginfo_class_NS_Yaf_Plugin_Abstract_postDispatch, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, preResponse, arginfo_class_NS_Yaf_Plugin_Abstract_preResponse, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(plugin) {
	zend_class_entry ce;
	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Plugin_Abstract", "Yaf\\Plugin_Abstract", namespace_switch(yaf_plugin_methods));
	yaf_plugin_ce = zend_register_internal_class_ex(&ce, NULL);
	yaf_plugin_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

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
