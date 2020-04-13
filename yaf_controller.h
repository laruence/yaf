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

#ifndef YAF_CONTROLLER_H
#define YAF_CONTROLLER_H

#define YAF_CONTROLLER_PROPERTY_NAME_ACTIONS	"actions"
#define YAF_CONTROLLER_PROPERTY_NAME_RENDER     "yafAutoRender"

#define YAF_CTL_AUTORENDER         (1<<0)
#define YAF_CTL_AUTORENDER_DEPEND  (1<<1)

extern zend_class_entry *yaf_controller_ce;

typedef struct {
	zend_uchar      flags;
	zend_string    *module;
	zend_string    *name;
	yaf_request_t  *request;
	yaf_response_t *response;
	yaf_view_t     *view;
	zend_string    *script_path;
	struct {
		yaf_controller_t ctl;
		zend_string     *name;
	} ctl;
	zend_array     *invoke_args;
	zend_array     *properties;
	zend_object     std;
} yaf_controller_object;

#define Z_YAFCTLOBJ(zv)    (php_yaf_controller_fetch_object(Z_OBJ(zv)))
#define Z_YAFCTLOBJ_P(zv)  Z_YAFCTLOBJ(*zv)

static zend_always_inline yaf_controller_object *php_yaf_controller_fetch_object(zend_object *obj) {
	return (yaf_controller_object *)((char*)(obj) - XtOffsetOf(yaf_controller_object, std));
}

int yaf_controller_render(yaf_controller_t *ctl, zend_string *action, zval *vars, zval *ret);
int yaf_controller_init(yaf_controller_object *ctl, yaf_dispatcher_object *dispatcher);
void yaf_controller_set_module_name(yaf_controller_object *ctl, zend_string *module);

static zend_always_inline zend_bool yaf_controller_auto_render(yaf_controller_object *ctl, zend_bool dispatch_render) {
	return (ctl->flags & YAF_CTL_AUTORENDER_DEPEND)? dispatch_render : (ctl->flags & YAF_CTL_AUTORENDER);
}

static zend_always_inline int yaf_controller_execute(yaf_controller_t *ctl, zend_function* func, int count, zval *args, zval *ret) {
	int result;
#if PHP_VERSION_ID < 70100
	zend_class_entry *old_scope = EG(scope);
	EG(scope) = Z_OBJCE_P(ctl);
#endif
	if (EXPECTED(count == 0)) {
		result = yaf_call_user_method_with_0_arguments(Z_OBJ_P(ctl), func, ret);
	} else {
		result = yaf_call_user_method(Z_OBJ_P(ctl), func, count, args, ret);
	}
#if PHP_VERSION_ID < 70100
	EG(scope) = old_scope;
#endif

	return result;
}

YAF_STARTUP_FUNCTION(controller);
#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

