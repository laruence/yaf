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

typedef struct {
	zend_string *tpl_dir;
	zend_array   tpl_vars;
	zend_array  *properties;
	zend_object  std;
} yaf_view_object;

#define Z_YAFVIEWOBJ(zv)    (php_yaf_view_fetch_object(Z_OBJ(zv)))
#define Z_YAFVIEWOBJ_P(zv)  Z_YAFVIEWOBJ(*zv)

static zend_always_inline yaf_view_object *php_yaf_view_fetch_object(zend_object *obj) {
	return (yaf_view_object *)((char*)(obj) - XtOffsetOf(yaf_view_object, std));
}

void yaf_view_instance(yaf_view_t *view, zend_string *tpl_dir, zval *options);
int yaf_view_render(yaf_view_t *view, zend_string *script, zval *var_array, zval *ret);
void yaf_view_set_tpl_dir_ex(yaf_view_t *view, zend_string *tpl_dir);
zend_string *yaf_view_get_tpl_dir_ex(yaf_view_t *view, yaf_request_t *request);

#define yaf_view_set_tpl_dir(v, d) do { \
	if (EXPECTED(Z_OBJCE_P(v) == yaf_view_simple_ce)) { \
		if (UNEXPECTED(Z_YAFVIEWOBJ_P(v)->tpl_dir)) { \
			zend_string_release(Z_YAFVIEWOBJ_P(v)->tpl_dir); \
		} \
		Z_YAFVIEWOBJ_P(v)->tpl_dir = zend_string_copy(d); \
	} else { \
		yaf_view_set_tpl_dir_ex(v, d); \
	} \
} while (0)

#define yaf_view_get_tpl_dir(v, q)  ((Z_OBJCE_P(v)==yaf_view_simple_ce)?Z_YAFVIEWOBJ_P(v)->tpl_dir:yaf_view_get_tpl_dir_ex(v, q))

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
