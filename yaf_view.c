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
#include "Zend/zend_interfaces.h" /* for zend_call_method_with_1_params */

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_loader.h"
#include "yaf_view.h"

#include "views/yaf_view_interface.h"
#include "views/yaf_view_simple.h"

void yaf_view_instance(yaf_view_t *view, zend_string *tpl_dir, zval *options) /* {{{ */ {
	yaf_view_simple_instance(view, tpl_dir);
}
/* }}} */

zend_string *yaf_view_get_tpl_dir_ex(yaf_view_t *view, yaf_request_t *request) /* {{{ */ {
	zval ret;
	zend_call_method_with_1_params(view, Z_OBJCE_P(view), NULL, "getscriptpath", &ret, request);
	if (Z_TYPE(ret) != IS_STRING) {
		zval_ptr_dtor(&ret);
		return NULL;
	}
	return Z_STR(ret);
}
/* }}} */

void yaf_view_set_tpl_dir_ex(yaf_view_t *view, zend_string *tpl) /* {{{ */ {
	zval arg, ret;

	ZVAL_STR_COPY(&arg, tpl);
	zend_call_method_with_1_params(view, Z_OBJCE_P(view), NULL, "setscriptpath", &ret, &arg);
	if (Z_TYPE(ret) != IS_TRUE && (Z_TYPE(ret) != IS_LONG || !Z_LVAL(ret))) {
		/* error handle? */
	}
	zval_dtor(&arg);
}
/* }}} */

int yaf_view_render(yaf_view_t *view, zend_string *script, zval *var_array, zval *ret) /* {{{ */ {
	if (EXPECTED(Z_OBJCE_P(view) == yaf_view_simple_ce)) {
		yaf_view_simple_render(view, script, var_array, ret);
		return 1;
	} else {
		zval arg;
		
		ZVAL_STR_COPY(&arg, script);
		if (ret) {
			if (var_array == NULL) {
				zend_call_method_with_1_params(view, Z_OBJCE_P(view), NULL, "render", ret, &arg);
			} else {
				zend_call_method_with_2_params(view, Z_OBJCE_P(view), NULL, "render", ret, &arg, var_array);
			}
			zval_ptr_dtor(&arg);
			if (UNEXPECTED(Z_TYPE_P(ret) != IS_STRING || EG(exception))) {
				zval_ptr_dtor(ret);
				return 0;
			}
		} else {
			zval rt;
			if (var_array == NULL) {
				zend_call_method_with_1_params(view, Z_OBJCE_P(view), NULL, "display", &rt, &arg);
			} else {
				zend_call_method_with_2_params(view, Z_OBJCE_P(view), NULL, "display", &rt, &arg, var_array);
			}
			zval_ptr_dtor(&arg);
			if (UNEXPECTED(Z_TYPE(rt) == IS_FALSE || EG(exception))) {
				zval_ptr_dtor(&rt);
				return 0;
			}
			zval_ptr_dtor(&rt);
		}
	}

	return 1;
}
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(view) {
	/* tricky way to supress warning "defined but not used" */
	(void)yaf_view_assign_arginfo;
	(void)yaf_view_display_arginfo;
	(void)yaf_view_render_arginfo;
	(void)yaf_view_getpath_arginfo;
	(void)yaf_view_setpath_arginfo;

	YAF_STARTUP(view_interface);
	YAF_STARTUP(view_simple);

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
