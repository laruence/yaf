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

#ifndef YAF_CONFIG_H
#define YAF_CONFIG_H

#define YAF_EXTRACT_FUNC_NAME            "extract"
#define YAF_CONFIG_PROPERT_NAME          "_config"
#define YAF_CONFIG_PROPERT_NAME_READONLY "_readonly"

#define YAF_CONFIG_READONLY  (1<<0)

typedef struct {
	zend_uchar   flags;
	zend_array  *config;
	zend_string *filename;
	zend_array  *properties;
	zend_object  std;
} yaf_config_object;

#define Z_YAFCONFIGOBJ(zv)    (php_yaf_config_fetch_object(Z_OBJ(zv)))
#define Z_YAFCONFIGOBJ_P(zv)  Z_YAFCONFIGOBJ(*zv)
static zend_always_inline yaf_config_object *php_yaf_config_fetch_object(zend_object *obj) {
	return (yaf_config_object *)((char*)(obj) - XtOffsetOf(yaf_config_object, std));
}

extern zend_class_entry *yaf_config_ce;
void yaf_config_instance(yaf_config_t *this_ptr, zval*, zend_string*);
zend_object *yaf_config_format_child(zend_class_entry *ce, zval *child, int readonly);

YAF_STARTUP_FUNCTION(config);
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
