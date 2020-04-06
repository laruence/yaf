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

#ifndef YAF_VIEW_H
#define YAF_VIEW_H

extern zend_class_entry *yaf_view_interface_ce;
extern zend_class_entry *yaf_view_simple_ce;

void yaf_view_instance(yaf_view_t *view, zend_string *tpl_dir, zval *options);

void yaf_view_render(yaf_view_t *view, zend_string *script, zval *var_array, zval *ret);
int yaf_view_display(yaf_view_t *view, zend_string *script, zval *var_array);
zend_string *yaf_view_get_tpl_dir(yaf_view_t *view, yaf_request_t *request);
void yaf_view_set_tpl_dir(yaf_view_t *view, zend_string *tpl_dir);

YAF_STARTUP_FUNCTION(view);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
