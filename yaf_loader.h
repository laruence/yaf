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

#ifndef YAF_LOADER_H
#define YAF_LOADER_H

#define YAF_DEFAULT_VIEW_EXT               "phtml"
#define YAF_DEFAULT_EXT                    "php"
#define YAF_DEFAULT_LIBRARY_EXT             YAF_DEFAULT_CONTROLLER_EXT

#define YAF_LIBRARY_DIRECTORY_NAME         "library"
#define YAF_CONTROLLER_DIRECTORY_NAME      "controllers"
#define YAF_PLUGIN_DIRECTORY_NAME          "plugins"
#define YAF_MODULE_DIRECTORY_NAME          "modules"
#define YAF_VIEW_DIRECTORY_NAME            "views"
#define YAF_MODEL_DIRECTORY_NAME           "models"

#define YAF_SPL_AUTOLOAD_REGISTER_NAME     "spl_autoload_register"
#define YAF_AUTOLOAD_FUNC_NAME             "autoload"

typedef struct {
	zend_object std;
	zend_string *library;
	zend_string *glibrary;
	zend_array  *namespaces;
	zend_bool    use_spl_autoload;
	zend_bool    lowcase_path;
	zend_bool    name_suffix;
	zend_bool    name_separator;
} yaf_loader_object;

#define Z_YAFLOADEROBJ(zv)    ((yaf_loader_object*)(Z_OBJ(zv)))
#define Z_YAFLOADEROBJ_P(zv)  Z_YAFLOADEROBJ(*zv)

extern zend_class_entry *yaf_loader_ce;

yaf_loader_t *yaf_loader_instance(zend_string *library_path);
void yaf_loader_reset(yaf_loader_object *loader);
int yaf_loader_load(yaf_loader_object *loader, char *file_name, size_t name_len, char *directory, uint32_t directory_len);
int yaf_register_autoloader(yaf_loader_t *loader);
int yaf_loader_import(const char* path, uint32_t path_len);
int yaf_loader_register_namespace_single(yaf_loader_object *loader, zend_string *prefix);
void yaf_loader_set_global_library_path(yaf_loader_object *loader, zend_string *library_path);

static zend_always_inline void yaf_loader_set_library_path(yaf_loader_object *loader, zend_string *library_path) {
	if (UNEXPECTED(loader->library)) {
		zend_string_release(loader->library);
	}
	loader->library = zend_string_copy(library_path);
}

extern PHPAPI int php_stream_open_for_zend_ex(const char *filename, zend_file_handle *handle, int mode);

YAF_STARTUP_FUNCTION(loader);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
