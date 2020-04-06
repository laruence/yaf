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

#ifndef YAF_VIEW_SIMPLE_H
#define YAF_VIEW_SIMPLE_H

#define VIEW_BUFFER_BLOCK_SIZE	4096
#define VIEW_BUFFER_SIZE_MASK 	4095

typedef struct {
	zend_object std;
	zend_string *tpl_dir;
	zend_array  tpl_vars;
} yaf_view_simple_object;

#define Z_YAFVIEWSIMPLEOBJ(zv)    ((yaf_view_simple_object*)Z_OBJ(zv))
#define Z_YAFVIEWSIMPLEOBJ_P(zv)  Z_YAFVIEWSIMPLEOBJ(*zv)

extern zend_class_entry *yaf_view_simple_ce;

void yaf_view_simple_instance(yaf_view_t *view, zend_string *tpl_dir);
zend_string *yaf_view_simple_get_tpl_dir(yaf_view_t *view, yaf_request_t *request);
void yaf_view_simple_set_tpl_dir(yaf_view_t *view, zend_string *tpl_dir);
int yaf_view_simple_render(yaf_view_t *view, zend_string *tpl, zval *vars, zval *ret);

YAF_STARTUP_FUNCTION(view_simple);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
