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

#ifndef YAF_REGISTRY_H
#define YAF_REGISTRY_H

typedef struct {
	zend_array  entries;
	zend_array *properties;
	zend_object std;
} yaf_registry_object;

#define Z_YAFREGISTRYOBJ(zv)    (php_yaf_registry_fetch_object(Z_OBJ(zv)))
#define Z_YAFREGISTRYOBJ_P(zv)  Z_YAFREGISTRYOBJ(*zv)
static zend_always_inline yaf_registry_object *php_yaf_registry_fetch_object(zend_object *obj) {
	return (yaf_registry_object *)((char*)(obj) - XtOffsetOf(yaf_registry_object, std));
}

extern zend_class_entry *yaf_registry_ce;

YAF_STARTUP_FUNCTION(registry);
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
