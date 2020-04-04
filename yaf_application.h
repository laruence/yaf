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

#ifndef PHP_YAF_APPLICATION_H
#define PHP_YAF_APPLICATION_H

#define YAF_APPLICATION_PROPERTY_NAME_CONFIG        "config"
#define YAF_APPLICATION_PROPERTY_NAME_DISPATCHER    "dispatcher"
#define YAF_APPLICATION_PROPERTY_NAME_RUN           "_running"
#define YAF_APPLICATION_PROPERTY_NAME_APP           "_app"
#define YAF_APPLICATION_PROPERTY_NAME_ENV           "_environ"
#define YAF_APPLICATION_PROPERTY_NAME_MODULES       "_modules"
#define YAF_APPLICATION_PROPERTY_NAME_ERRNO         "_err_no"
#define YAF_APPLICATION_PROPERTY_NAME_ERRMSG        "_err_msg"

typedef struct {
    zend_object      std;
	zend_array       modules;
    zval             config;
    yaf_dispatcher_t dispatcher;
	zend_string     *environ;
	zend_string     *default_module;
	zend_string     *default_controller;
	zend_string     *default_action;
	zend_string     *library;
	zend_string     *directory;
	zend_string     *ext;
	zend_string     *bootstrap;
	zend_string     *view_ext;
	zend_string     *base_uri;
	zend_array      *default_route;
	zend_string     *err_msg;
	unsigned int     err_no;
    zend_bool        running;
} yaf_application_object;

#define Z_YAFAPPOBJ(zv)     ((yaf_application_object*)(Z_OBJ(zv)))
#define Z_YAFAPPOBJ_P(zv)   Z_YAFAPPOBJ(*(zv))

extern zend_class_entry *yaf_application_ce;

int yaf_application_is_module_name(zend_string *name);
int yaf_application_is_module_name_str(const char *name, size_t len);

static inline yaf_application_object *yaf_application_instance() {
	if (Z_TYPE(YAF_G(app)) == IS_OBJECT) {
		return Z_YAFAPPOBJ(YAF_G(app));
	}
	return NULL;
}

YAF_STARTUP_FUNCTION(application);
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
