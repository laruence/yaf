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
#include "Zend/zend_interfaces.h" /* for zend_class_serialize_deny */
#include "ext/standard/php_string.h" /* php_trim */

#include "php_yaf.h"
#include "yaf_application.h"
#include "yaf_namespace.h"
#include "yaf_request.h"
#include "yaf_loader.h"
#include "yaf_exception.h"

#define YAF_LOADER_CONTROLLER		"Controller"
#define YAF_LOADER_MODEL			"Model"
#define YAF_LOADER_PLUGIN			"Plugin"
#define YAF_LOADER_RESERVERD		"Yaf_"

#define YAF_CLASS_NAME_NORMAL       0
#define YAF_CLASS_NAME_MODEL        1
#define YAF_CLASS_NAME_PLUGIN       2
#define YAF_CLASS_NAME_CONTROLLER   3

zend_class_entry *yaf_loader_ce;
static zend_object_handlers yaf_loader_obj_handlers;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_loader_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_loader_getinstance_arginfo, 0, 0, 0)
    ZEND_ARG_INFO(0, local_library_path)
    ZEND_ARG_INFO(0, global_library_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_loader_autoloader_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, class_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_loader_regnamespace_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, namespace)
    ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_loader_islocalname_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, class_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_loader_import_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, file)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_loader_setlib_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, library_path)
    ZEND_ARG_INFO(0, is_global)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_loader_getlib_arginfo, 0, 0, 0)
    ZEND_ARG_INFO(0, is_global)
ZEND_END_ARG_INFO()
/* }}} */

static void yaf_loader_obj_free(zend_object *object) /* {{{ */ {
	yaf_loader_object *loader = (yaf_loader_object*)object;

	if (loader->library) {
		zend_string_release(loader->library);
	}
	if (loader->glibrary) {
		zend_string_release(loader->glibrary);
	}
	if (GC_DELREF(YAF_LOADER_NAMESPACES(loader)) == 0) {
		GC_REMOVE_FROM_BUFFER(YAF_LOADER_NAMESPACES(loader));
		zend_array_destroy(YAF_LOADER_NAMESPACES(loader));
	}
	if (loader->properties) {
		if (GC_DELREF(loader->properties) == 0) {
			GC_REMOVE_FROM_BUFFER(loader->properties);
			zend_array_destroy(loader->properties);
		}
	}

	zend_object_std_dtor(object);
}
/* }}} */

int yaf_loader_register(yaf_loader_t *loader) /* {{{ */ {
	zval autoload, function, method, ret;

	array_init(&autoload);

    ZVAL_STRING(&method, YAF_AUTOLOAD_FUNC_NAME);
	Z_ADDREF_P(loader);
	zend_hash_next_index_insert(Z_ARRVAL(autoload), loader);
	zend_hash_next_index_insert(Z_ARRVAL(autoload), &method);

	ZVAL_STRING(&function, YAF_SPL_AUTOLOAD_REGISTER_NAME);

	do {
		zend_fcall_info fci = {
			sizeof(fci),
#if PHP_VERSION_ID < 70100
			EG(function_table),
#endif
			function,
#if PHP_VERSION_ID < 70100
			NULL,
#endif
			&ret,
			&autoload,
			NULL,
			1,
#if PHP_VERSION_ID < 80000
            1
#else
           NULL
#endif
		};

		if (zend_call_function(&fci, NULL) == FAILURE) {
			zval_ptr_dtor(&function);
			zval_ptr_dtor(&autoload);
			php_error_docref(NULL,
					E_WARNING,
					"Unable to register autoload function %s",
					YAF_AUTOLOAD_FUNC_NAME);
			return 0;
		}
		zval_ptr_dtor(&function);
		zval_ptr_dtor(&autoload);
	} while (0);
	return 1;
}
/* }}} */

void yaf_loader_set_global_library_path(yaf_loader_object *loader, zend_string *global_library) /* {{{ */ {
	if (EXPECTED(loader->glibrary)) {
		zend_string_release(loader->glibrary);
	}
	loader->glibrary = zend_string_copy(global_library);
}
/* }}} */

static zend_array *yaf_loader_get_namespaces(yaf_loader_object *loader) /* {{{ */ {
	zval *val, rv;
	HashTable *ht;
	zend_string *name;

	ALLOC_HASHTABLE(ht);
	zend_hash_init(ht, zend_hash_num_elements(YAF_LOADER_NAMESPACES(loader)), NULL, ZVAL_PTR_DTOR, 0);

	ZEND_HASH_FOREACH_STR_KEY_VAL(YAF_LOADER_NAMESPACES(loader), name, val) {
		ZEND_ASSERT(name);
		if (Z_TYPE_P(val) == IS_NULL) {
			ZVAL_STR_COPY(&rv, name);
			zend_hash_next_index_insert(ht, &rv);
		} else {
			zend_hash_update(ht, name, val);
			Z_TRY_ADDREF_P(val);
		}
	} ZEND_HASH_FOREACH_END();

	return ht;
}
/* }}} */

static HashTable *yaf_loader_get_properties(yaf_object *obj) /* {{{ */ {
	zval rv;
	HashTable *ht;
#if PHP_VERSION_ID < 80000
	yaf_loader_object *loader = Z_YAFLOADEROBJ_P(obj);
#else
	yaf_loader_object *loader = (yaf_loader_object*)(obj);
#endif

	if (!loader->properties) {
		ALLOC_HASHTABLE(loader->properties);
		zend_hash_init(loader->properties, 4, NULL, ZVAL_PTR_DTOR, 0);
		YAF_ALLOW_VIOLATION(loader->properties);;
	}

	ht = loader->properties;

	ZVAL_STR_COPY(&rv, loader->library);
	zend_hash_str_update(ht, "library:protected", sizeof("library:protected") - 1, &rv);
	if (loader->glibrary) {
		ZVAL_STR_COPY(&rv, loader->glibrary);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_update(ht, "global_library:protected", sizeof("global_library:protected") - 1, &rv);
	
	ZVAL_ARR(&rv, yaf_loader_get_namespaces(loader));
	zend_hash_str_update(ht, "namespace:protected", sizeof("namespace:protected") - 1, &rv);

	ZVAL_BOOL(&rv, yaf_loader_use_spl_autoload(loader));
	zend_hash_str_update(ht, "use_spl_autoload:protected", sizeof("use_spl_autoload:protected") - 1, &rv);

	ZVAL_BOOL(&rv, yaf_loader_is_lowcase_path(loader));
	zend_hash_str_update(ht, "lowercase_path:protected", sizeof("lowercase_path:protected") - 1, &rv);

	ZVAL_BOOL(&rv, yaf_loader_is_name_suffix(loader));
	zend_hash_str_update(ht, "is_name_suffix:protected", sizeof("is_name_suffix:protected") - 1, &rv);

	ZVAL_BOOL(&rv, yaf_loader_has_name_separator(loader));
	zend_hash_str_update(ht, "has_name_seperator:protected", sizeof("has_name_seperator:protected") - 1, &rv);

	return ht;
}
/* }}} */

void yaf_loader_reset(yaf_loader_object *loader) /* {{{ */ {
	/* for back-compatibility of change of YAF_G after loader in initialized only */
	YAF_LOADER_FLAGS(loader) = (zend_uchar)YAF_FLAGS();
}
/* }}} */

yaf_loader_t *yaf_loader_instance(zend_string *library_path) /* {{{ */ {
	yaf_loader_object *loader;
	yaf_loader_t *instance = &YAF_G(loader);

	if (EXPECTED(IS_OBJECT == Z_TYPE_P(instance))) {
		return instance;
	}

	loader = emalloc(sizeof(yaf_loader_object));
	zend_object_std_init(&loader->std, yaf_loader_ce);
	loader->std.handlers = &yaf_loader_obj_handlers;

	/* yaf_loader_reset(loader); */
	YAF_LOADER_FLAGS(loader) = (zend_uchar)YAF_FLAGS();
	if (library_path) {
		loader->library = zend_string_copy(library_path);
	} else {
		loader->library = ZSTR_EMPTY_ALLOC();
	}

	if (*YAF_G(global_library)) {
		loader->glibrary = zend_string_init(YAF_G(global_library), strlen(YAF_G(global_library)), 0);
	} else {
		loader->glibrary = NULL;
	}

	ZVAL_OBJ(&YAF_G(loader), &loader->std);
	if (UNEXPECTED(!yaf_loader_register(&YAF_G(loader)))) {
		php_error_docref(NULL, E_WARNING, "Failed to register autoload function");
	}
	
	ALLOC_HASHTABLE(YAF_LOADER_NAMESPACES(loader));
	zend_hash_init(YAF_LOADER_NAMESPACES(loader), 8, NULL, ZVAL_PTR_DTOR, 0);
	YAF_ALLOW_VIOLATION(YAF_LOADER_NAMESPACES(loader));

	loader->properties = NULL;

	return &YAF_G(loader);
}
/* }}} */

int yaf_loader_register_namespace(yaf_loader_object *loader, zend_string *class_name, zend_string *path) /* {{{ */ {
	zval *entry, rv;
	HashTable *target;
	char *delim;
	char *name = ZSTR_VAL(class_name);
	uint32_t len = ZSTR_LEN(class_name);

	ZVAL_NULL(&rv);
	target = YAF_LOADER_NAMESPACES(loader);

	if (*name == '\\') {
		name++;
		len--;
	}
	if (((delim = memchr(name, '\\', len)) || (delim = memchr(name, '_', len)))) {
		do {
loop:
			if ((entry = zend_hash_str_find(target, name, delim - name)) == NULL) {
				entry = zend_hash_str_update(target, name, delim - name, &rv);
				array_init(entry);
			} else if (UNEXPECTED(Z_TYPE_P(entry) != IS_ARRAY)) {
				zval_ptr_dtor(entry);
				array_init(entry);
			}
			len -= delim - name + 1;
			name = delim + 1;
			target = Z_ARRVAL_P(entry);
			if (((delim = memchr(name, '\\', len)) || (delim = memchr(name, '_', len)))) {
				goto loop;
			} else {
				entry = zend_hash_str_update(target, name, len, &rv);
			}
		} while (0);
	} else {
		entry = zend_hash_str_update(YAF_LOADER_NAMESPACES(loader), name, len, &rv);
	}

	if (path) {
		ZVAL_STR_COPY(entry, path);
	}

	return 1;
}
/* }}} */

int yaf_loader_register_namespace_multi(yaf_loader_object *loader, zval *namespaces) /* {{{ */ {
	zval *pzval;
	HashTable *ht;
	zend_string *key;

	ht = Z_ARRVAL_P(namespaces);
	ZEND_HASH_FOREACH_STR_KEY_VAL(ht, key, pzval) {
		if (key == NULL) {
			if (IS_STRING == Z_TYPE_P(pzval)) {
				yaf_loader_register_namespace(loader, Z_STR_P(pzval), NULL);
			}
		} else {
			if (IS_STRING == Z_TYPE_P(pzval)) {
				yaf_loader_register_namespace(loader, key, Z_STR_P(pzval));
			} else {
				yaf_loader_register_namespace(loader, key, NULL);
			}
		}
	} ZEND_HASH_FOREACH_END();

	return 1;
}
/* }}} */

static void yaf_loader_sanitize_path(char *name, uint32_t len) /* {{{ */ {
	yaf_replace_chr(name, len, '_', DEFAULT_SLASH);
}
/* }}} */

static void yaf_loader_sanitize_name(char *name, uint32_t len, char *buf) /* {{{ */ {
	memcpy(buf, name, len);
	/* replace all '\' to '_' */
	yaf_replace_chr(buf, len, '\\', '_');
}
/* }}} */

static zend_string *yaf_loader_resolve_namespace(yaf_loader_object *loader, const char *class_name, uint32_t *name_len) /* {{{ */ {
	zval *name;
	const char *delim;
	uint32_t len = *name_len;
	HashTable *target = YAF_LOADER_NAMESPACES(loader);

	if ((delim = memchr(class_name, '_', len))) {
		do {
			if ((name = zend_hash_str_find(target, class_name, delim - class_name))) {
				if (Z_TYPE_P(name) == IS_ARRAY) {
					target = Z_ARRVAL_P(name);
					len -= delim - class_name + 1;
					class_name = delim + 1;
				} else if (Z_TYPE_P(name) == IS_STRING) {
					*name_len = (len - (delim - class_name + 1));
					return Z_STR_P(name);
				} else {
					return (zend_string*)-1; /* use library path */
				}
			} else {
				return NULL;
			}
		} while ((delim = memchr(class_name, '_', len)));
	} else if ((name = zend_hash_str_find(target, class_name, len))) {
		return Z_TYPE_P(name) == IS_STRING? Z_STR_P(name) : (zend_string*)-1;
	}
	return NULL;
}
/* }}} */

static int yaf_loader_identify_category(yaf_loader_object *loader, zend_string *class_name) /* {{{ */ {
	char *name = ZSTR_VAL(class_name);
	size_t len = ZSTR_LEN(class_name);
	char *suspense_name;
	int suspense_len;
	int suspense_type = YAF_CLASS_NAME_NORMAL;

	if (EXPECTED(yaf_loader_is_name_suffix(loader))) {
		switch (name[len - 1]) {
			case 'l':
				suspense_name = YAF_LOADER_MODEL;
				suspense_len = sizeof(YAF_LOADER_MODEL) - 1;
				suspense_type = YAF_CLASS_NAME_MODEL;
				break;
			case 'n':
				suspense_name = YAF_LOADER_PLUGIN;
				suspense_len = sizeof(YAF_LOADER_PLUGIN) - 1;
				suspense_type = YAF_CLASS_NAME_PLUGIN;
				break;
			case 'r':
				suspense_name = YAF_LOADER_CONTROLLER;
				suspense_len = sizeof(YAF_LOADER_CONTROLLER) - 1;
				suspense_type = YAF_CLASS_NAME_CONTROLLER;
				break;
			default:
				return YAF_CLASS_NAME_NORMAL;
		}
		if (len <= suspense_len || !yaf_slip_equal(name + len - suspense_len, suspense_name, suspense_len - 1)) {
			return YAF_CLASS_NAME_NORMAL;
		}
		if (UNEXPECTED(yaf_loader_has_name_separator(loader))) {
			name += (len - suspense_len);
			if (len > YAF_G(name_separator_len) &&
				memcmp(name - YAF_G(name_separator_len), YAF_G(name_separator), YAF_G(name_separator_len)) == 0) {
				return suspense_type;
			}
			return YAF_CLASS_NAME_NORMAL;
		}
		return suspense_type;
	} else {
		switch (*name) {
			case 'M':
				suspense_name = YAF_LOADER_MODEL;
				suspense_len = sizeof(YAF_LOADER_MODEL) - 1;
				suspense_type = YAF_CLASS_NAME_MODEL;
				break;
			case 'P':
				suspense_name = YAF_LOADER_PLUGIN;
				suspense_len = sizeof(YAF_LOADER_PLUGIN) - 1;
				suspense_type = YAF_CLASS_NAME_PLUGIN;
				break;
			case 'C':
				suspense_name = YAF_LOADER_CONTROLLER;
				suspense_len = sizeof(YAF_LOADER_CONTROLLER) - 1;
				suspense_type = YAF_CLASS_NAME_CONTROLLER;
				break;
			default:
				return YAF_CLASS_NAME_NORMAL;
		}
		if (len <= suspense_len || !yaf_slip_equal(name + 1, suspense_name + 1, suspense_len - 1)) {
			return YAF_CLASS_NAME_NORMAL;
		}
		if (UNEXPECTED(yaf_loader_has_name_separator(loader))) {
			name += suspense_len;
			if (len > YAF_G(name_separator_len) &&
				memcmp(name, YAF_G(name_separator), YAF_G(name_separator_len)) == 0) {
				return suspense_type;
			}
			return YAF_CLASS_NAME_NORMAL;
		}
		return suspense_type;
	}
}
/* }}} */

ZEND_HOT int yaf_loader_import(const char *path, uint32_t len) /* {{{ */ {
	zend_file_handle file_handle;
	zend_op_array *op_array;
	zend_stat_t sb;

	if (UNEXPECTED(VCWD_STAT(path, &sb) == -1)) {
		return 0;
	}

#if PHP_VERSION_ID < 70400
	file_handle.filename = path;
	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.free_filename = 0;
	file_handle.opened_path = NULL;
	file_handle.handle.fp = NULL;
#else
	/* setup file-handle */
	zend_stream_init_filename(&file_handle, path);
#endif

	if (EXPECTED((op_array = zend_compile_file(&file_handle, ZEND_INCLUDE)))) {
		zval result;
		if (EXPECTED(file_handle.handle.stream.handle)) {
			if (UNEXPECTED(!file_handle.opened_path)) {
				file_handle.opened_path = zend_string_init(path, len, 0);
			}
			zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path);
		}

        ZVAL_UNDEF(&result);
		zend_execute(op_array, &result);
		destroy_op_array(op_array);
		efree_size(op_array, sizeof(zend_op_array));
        zval_ptr_dtor(&result);
		zend_destroy_file_handle(&file_handle);

		return 1;
	}

	zend_destroy_file_handle(&file_handle);
	return 0;
}
/* }}} */

ZEND_HOT int yaf_loader_load_internal(yaf_loader_object *loader, char *filename, size_t fname_len, char *directory, uint32_t directory_len) /* {{{ */ {
   char *ext;
   uint32_t ext_len;
   yaf_application_object *app = yaf_application_instance();

   if (UNEXPECTED(app->ext)) {
	   ext = ZSTR_VAL(app->ext);
	   ext_len = ZSTR_LEN(app->ext);
   } else {
	   ext = YAF_DEFAULT_EXT;
	   ext_len = sizeof(YAF_DEFAULT_EXT) - 1;
   }

   if (UNEXPECTED((directory_len + fname_len + ext_len + 3) > MAXPATHLEN)) {
	   directory[directory_len] = '\0';
	   return 0;
   }

   directory[directory_len] = DEFAULT_SLASH;
   memcpy(directory + directory_len + 1, filename, fname_len);
   filename = directory + directory_len + 1;
   if (UNEXPECTED(yaf_loader_is_lowcase_path(loader))) {
	   zend_str_tolower(filename, fname_len);
   }
   yaf_loader_sanitize_path(filename, fname_len);
   directory[directory_len + 1 + fname_len] = '.';
   memcpy(directory + directory_len + 1 + fname_len + 1, ext, ext_len);
   /* aussume all the path is not end in slash */
   directory[directory_len + 1 + fname_len + 1 + ext_len] = '\0';
   directory_len = directory_len + 1 + fname_len + 1 + ext_len;

   return yaf_loader_import(directory, directory_len);
}
/* }}} */

ZEND_HOT static int yaf_loader_load_user(yaf_loader_object *loader, char *buf, uint32_t len) /* {{{ */ {
	zend_string *library_dir;
	const char *ext;
	char *name = buf;
	uint32_t ext_len;
	uint32_t origin_len = len;
	yaf_application_object *app = yaf_application_instance();

	if ((library_dir = yaf_loader_resolve_namespace(loader, buf, &len))) {
		if (library_dir == ((zend_string*)-1)) {
			library_dir = loader->library;
		} else {
			name += (origin_len - len);
		}
	} else {
		if (!loader->glibrary) {
			library_dir = loader->library;
		} else {
			library_dir = loader->glibrary;
		}
	}

	if (UNEXPECTED(yaf_loader_is_lowcase_path(loader))) {
		zend_str_tolower(name, len);
	}
	yaf_loader_sanitize_path(name, len);

	if (EXPECTED(app) && UNEXPECTED(app->ext)) {
		ext = ZSTR_VAL(app->ext);
		ext_len = ZSTR_LEN(app->ext);
	} else {
		ext = YAF_DEFAULT_EXT;
		ext_len = sizeof(YAF_DEFAULT_EXT) - 1;
	}

	ZEND_ASSERT(library_dir);
	if (UNEXPECTED(ZSTR_LEN(library_dir) + len + ext_len + 2 > MAXPATHLEN)) {
		return 0;
	}

	memmove(buf + ZSTR_LEN(library_dir) + 1, name, len);
	memcpy(buf, ZSTR_VAL(library_dir), ZSTR_LEN(library_dir));
	buf[ZSTR_LEN(library_dir)] = DEFAULT_SLASH;
	buf[ZSTR_LEN(library_dir) + 1 + len] = '.';
	memcpy(buf + ZSTR_LEN(library_dir) + 1 + len + 1, ext, ext_len);
	buf[ZSTR_LEN(library_dir) + 1 + len + 1 + ext_len] = '\0';
	len = ZSTR_LEN(library_dir) + 1 + len + 1 + ext_len;

	return yaf_loader_import(buf, len);
}
/* }}} */

static zend_never_inline int yaf_loader_load_mvc(yaf_loader_object *loader, char *buf, uint32_t len, int type) /* {{{ */ {
	char *name;
	const char *folder, *ext;
	uint32_t folder_len, ext_len;
	zend_string *library_dir;
	yaf_application_object *app = yaf_application_instance();

	if (UNEXPECTED(app == NULL)) {
		php_error_docref(NULL, E_WARNING, "Couldn't load a MVC class unless an %s is initialized", ZSTR_VAL(yaf_application_ce->name));
		*buf = '\0';
		return 0;
	}

	switch (type) {
		case YAF_CLASS_NAME_MODEL:
			folder = YAF_MODEL_DIRECTORY_NAME;
			folder_len = sizeof(YAF_MODEL_DIRECTORY_NAME) - 1;
			break;
		case YAF_CLASS_NAME_PLUGIN:
			folder = YAF_PLUGIN_DIRECTORY_NAME;
			folder_len = sizeof(YAF_PLUGIN_DIRECTORY_NAME) - 1;
			break;
		case YAF_CLASS_NAME_CONTROLLER:
			folder = YAF_CONTROLLER_DIRECTORY_NAME;
			folder_len = sizeof(YAF_CONTROLLER_DIRECTORY_NAME) - 1;
			break;
		default:
			ZEND_ASSERT(0);
			break;
	}

	len -= (folder_len - 1); /* models -> model etc*/
	if (EXPECTED(yaf_loader_is_name_suffix(loader))) {
		name = buf;
		if (UNEXPECTED(yaf_loader_has_name_separator(loader))) {
			len -= YAF_G(name_separator_len);
		}
	} else {
		name = buf + folder_len - 1;
		if (UNEXPECTED(yaf_loader_has_name_separator(loader))) {
			name += YAF_G(name_separator_len);
			len -= YAF_G(name_separator_len);
		}
	}
	if (UNEXPECTED(yaf_loader_is_lowcase_path(loader))) {
		zend_str_tolower(name, len);
	}
	yaf_loader_sanitize_path(name, len);

	if (UNEXPECTED(app->ext)) {
		ext = ZSTR_VAL(app->ext);
		ext_len = ZSTR_LEN(app->ext);
	} else {
		ext = YAF_DEFAULT_EXT;
		ext_len = sizeof(YAF_DEFAULT_EXT) - 1;
	}

	if (UNEXPECTED(ZSTR_LEN(app->directory) + 1 + folder_len + 1 + len + 1 + ext_len > MAXPATHLEN)) {
		php_error_docref(NULL, E_WARNING, "Path too long '%s'", ZSTR_VAL(app->directory));
		*buf = '\0';
		return 0;
	}

	library_dir = app->directory;
	memmove(buf + ZSTR_LEN(library_dir) + 1 + folder_len + 1, name, len);
	memcpy(buf, ZSTR_VAL(library_dir), ZSTR_LEN(library_dir));
	buf[ZSTR_LEN(library_dir)] = DEFAULT_SLASH;
	memcpy(buf + ZSTR_LEN(library_dir) + 1, folder, folder_len);
	buf[ZSTR_LEN(library_dir) + 1 + folder_len] = DEFAULT_SLASH;
	buf[ZSTR_LEN(library_dir) + 1 + folder_len + 1 + len] = '.';
	memcpy(buf + ZSTR_LEN(library_dir) + 1 + folder_len + 1 + len + 1, ext, ext_len);
	buf[ZSTR_LEN(library_dir) + 1 + folder_len + 1 + len + 1 + ext_len] = '\0';

	return yaf_loader_import(buf, len);
}
/* }}} */

/** {{{ proto public Yaf_Loader::autoload($class_name)
*/
PHP_METHOD(yaf_loader, autoload) {
	char directory[MAXPATHLEN];
	uint32_t class_type, status;
	zend_string *class_name;
	zend_string *unqualified = NULL;
	yaf_loader_object *loader = Z_YAFLOADEROBJ_P(getThis());

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(class_name)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(ZSTR_LEN(class_name) == 0 || ZSTR_LEN(class_name) > MAXPATHLEN)) {
		RETURN_FALSE;
	}

	/*
	if (UNEXPECTED(ZSTR_LEN(class_name) >= sizeof(YAF_LOADER_RESERVERD) - 1 &&
		yaf_slip_equal(ZSTR_VAL(class_name), YAF_LOADER_RESERVERD, sizeof(YAF_LOADER_RESERVERD) - 1))) {
		php_error_docref(NULL, E_WARNING, "You should not use '%s' as class name prefix", YAF_LOADER_RESERVERD);
	}
	*/
	if (ZSTR_VAL(class_name)[0] == '\\') {
		unqualified = zend_string_init(ZSTR_VAL(class_name) + 1, ZSTR_LEN(class_name) - 1, 0);
		class_name = unqualified;
	}
	yaf_loader_sanitize_name(ZSTR_VAL(class_name), ZSTR_LEN(class_name), directory);
	if ((class_type = yaf_loader_identify_category(loader, class_name)) == YAF_CLASS_NAME_NORMAL) {
		status = yaf_loader_load_user(loader, directory, ZSTR_LEN(class_name));
	} else {
		status = yaf_loader_load_mvc(loader, directory, ZSTR_LEN(class_name), class_type);
	}

	if (unqualified) {
		zend_string_release(unqualified);
	}
	if (EXPECTED(!yaf_loader_use_spl_autoload(loader))) {
		if (EXPECTED(status)) {
			zend_string *lc_name = zend_string_tolower(class_name);
			if (UNEXPECTED(!zend_hash_exists(EG(class_table), lc_name))) {
				php_error_docref(NULL, E_WARNING, "Could not find class %s in %s", ZSTR_VAL(class_name), directory);
			}
			zend_string_release(lc_name);
		} else if (*directory) {
			php_error_docref(NULL, E_WARNING, "Failed opening script %s: %s", directory, strerror(errno));
		}
		RETURN_TRUE;
	}
	RETURN_BOOL(status);
}
/* }}} */

/** {{{ proto public static Yaf_Loader::import($file)
*/
PHP_METHOD(yaf_loader, import) {
	zend_string *file;
	int need_free = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(file)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_LEN(file) == 0) {
		RETURN_FALSE;
	} else {
		int retval;

		if (!IS_ABSOLUTE_PATH(ZSTR_VAL(file), ZSTR_LEN(file))) {
			if (UNEXPECTED(Z_TYPE(YAF_G(loader)) != IS_OBJECT)) {
				php_error_docref(NULL, E_WARNING, "%s need to be initialize first", ZSTR_VAL(yaf_loader_ce->name));
				RETURN_FALSE;
			} else {
				yaf_loader_object *loader = Z_YAFLOADEROBJ(YAF_G(loader));
				zend_string *library = loader->library;
				file = strpprintf(0, "%s%c%s", ZSTR_VAL(library), DEFAULT_SLASH, ZSTR_VAL(file));
				need_free = 1;
			}
		}

		retval = zend_hash_exists(&EG(included_files), file);
		if (retval) {
			if (need_free) {
				zend_string_release(file);
			}
			RETURN_TRUE;
		}

		retval = yaf_loader_import(ZSTR_VAL(file), ZSTR_LEN(file));
		if (need_free) {
			zend_string_release(file);
		}

		RETURN_BOOL(retval);
	}
}
/* }}} */

/** {{{ proto public Yaf_Loader::registerLocalNamespace(mixed $namespace, string $path = NULL)
*/
PHP_METHOD(yaf_loader, registerLocalNamespace) {
	zval *namespaces;
	zend_string *path = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|S", &namespaces, &path) == FAILURE) {
		return;
	}

	if (IS_STRING == Z_TYPE_P(namespaces)) {
		if (yaf_loader_register_namespace(Z_YAFLOADEROBJ_P(getThis()), Z_STR_P(namespaces), path)) {
			RETURN_ZVAL(getThis(), 1, 0);
		}
	} else if (IS_ARRAY == Z_TYPE_P(namespaces)) {
		if (yaf_loader_register_namespace_multi(Z_YAFLOADEROBJ_P(getThis()), namespaces)) {
			RETURN_ZVAL(getThis(), 1, 0);
		}
	} else {
		php_error_docref(NULL, E_WARNING, "Invalid parameters provided, must be a string, or an array");
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Loader::getLocalNamespace(void)
*/
PHP_METHOD(yaf_loader, getLocalNamespace) {
	yaf_loader_object *loader = Z_YAFLOADEROBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	ZVAL_ARR(return_value, yaf_loader_get_namespaces(loader));
}
/* }}} */

/** {{{ proto public Yaf_Loader::clearLocalNamespace(void)
*/
PHP_METHOD(yaf_loader, clearLocalNamespace) {
	yaf_loader_object *loader = Z_YAFLOADEROBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_hash_clean(YAF_LOADER_NAMESPACES(loader));

	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Loader::isLocalName(string $class_name)
*/
PHP_METHOD(yaf_loader, isLocalName) {
	zend_string *name;
	int result;
	char *sanitized_name;
	uint32_t sanitized_len;
	yaf_loader_object *loader = Z_YAFLOADEROBJ_P(getThis());
	ALLOCA_FLAG(use_heap);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	if (ZSTR_VAL(name)[0] == '\\') {
		sanitized_len = ZSTR_LEN(name) - 1;
		sanitized_name = do_alloca(sanitized_len, use_heap);
		yaf_loader_sanitize_name(ZSTR_VAL(name) + 1, sanitized_len, sanitized_name);
	} else {
		sanitized_len = ZSTR_LEN(name);
		sanitized_name = do_alloca(sanitized_len, use_heap);
		yaf_loader_sanitize_name(ZSTR_VAL(name), sanitized_len, sanitized_name);
	}
	result = YAF_LOADER_NAMESPACES(loader) && yaf_loader_resolve_namespace(loader, sanitized_name, &sanitized_len);
	free_alloca(sanitized_name, use_heap);

	RETURN_BOOL(result);
}
/* }}} */

/** {{{ proto public Yaf_Loader::getNamespacePath(string $class_name)
*/
PHP_METHOD(yaf_loader, getNamespacePath) {
	zend_string *name;
	zend_string *path;
	char *sanitized_name;
	uint32_t sanitized_len;
	yaf_loader_object *loader = Z_YAFLOADEROBJ_P(getThis());
	ALLOCA_FLAG(use_heap);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	if (ZSTR_VAL(name)[0] == '\\') {
		sanitized_len = ZSTR_LEN(name) - 1;
		sanitized_name = do_alloca(sanitized_len, use_heap);
		yaf_loader_sanitize_name(ZSTR_VAL(name) + 1, sanitized_len, sanitized_name);
	} else {
		sanitized_len = ZSTR_LEN(name);
		sanitized_name = do_alloca(sanitized_len, use_heap);
		yaf_loader_sanitize_name(ZSTR_VAL(name), sanitized_len, sanitized_name);
	}
	if ((path = yaf_loader_resolve_namespace(loader, sanitized_name, &sanitized_len))) {
		if (path == ((zend_string*)-1)) {
			RETVAL_STR_COPY(loader->library);
		} else {
			RETVAL_STR_COPY(path);
		}
	} else {
		if (loader->glibrary) {
			RETVAL_STR_COPY(loader->glibrary);
		} else {
			RETVAL_STR_COPY(loader->library);
		}
	}
	free_alloca(sanitized_name, use_heap);

	return;
}
/* }}} */

/** {{{ proto public Yaf_Loader::setLibraryPath(string $path, $global = FALSE)
*/
PHP_METHOD(yaf_loader, setLibraryPath) {
	zend_string *library;
	zend_bool global = 0;
	yaf_loader_object *loader = Z_YAFLOADEROBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|b", &library, &global) == FAILURE) {
		return;
	}

	if (!global) {
		yaf_loader_set_library_path(loader, library);
	} else {
		yaf_loader_set_global_library_path(loader, library);
	}

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Loader::getLibraryPath($global = FALSE)
*/
PHP_METHOD(yaf_loader, getLibraryPath) {
	zend_bool global = 0;
	yaf_loader_object *loader = Z_YAFLOADEROBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &global) == FAILURE) {
		return;
	}

	if (!global) {
		RETURN_STR_COPY(loader->library);
	} else if (loader->glibrary) {
		RETURN_STR_COPY(loader->glibrary);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/** {{{ proto public Yaf_Loader::getInstance($library = NULL, $global_library = NULL)
*/
PHP_METHOD(yaf_loader, getInstance) {
	zend_string *library = NULL;
	zend_string *global = NULL;
	yaf_loader_t *loader;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S!S!", &library, &global) == FAILURE) {
		return;
	}

	if ((loader = yaf_loader_instance(NULL))) {
		if (library) {
			yaf_loader_set_library_path(Z_YAFLOADEROBJ_P(loader), library);
		}
		if (global) {
			yaf_loader_set_global_library_path(Z_YAFLOADEROBJ_P(loader), global);
		}
		/* for back-compatible with changing of YAF_G(lowcase_path) ini_set */
		yaf_loader_reset(Z_YAFLOADEROBJ_P(loader));
		RETURN_ZVAL(loader, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto private Yaf_Loader::__construct(void)
*/
PHP_METHOD(yaf_loader, __construct) {
}
/* }}} */

/** {{{ yaf_loader_methods
*/
zend_function_entry yaf_loader_methods[] = {
	PHP_ME(yaf_loader, __construct, yaf_loader_void_arginfo, ZEND_ACC_PRIVATE|ZEND_ACC_CTOR)
	PHP_ME(yaf_loader, autoload, yaf_loader_autoloader_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_loader, getInstance, yaf_loader_getinstance_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(yaf_loader, registerLocalNamespace, yaf_loader_regnamespace_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_loader, getLocalNamespace, yaf_loader_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_loader, clearLocalNamespace, yaf_loader_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_loader, isLocalName, yaf_loader_islocalname_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_loader, getNamespacePath, yaf_loader_islocalname_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_loader, import, yaf_loader_import_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(yaf_loader, setLibraryPath, yaf_loader_setlib_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_loader, getLibraryPath, yaf_loader_getlib_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_loader, registerNamespace, registerLocalNamespace, yaf_loader_regnamespace_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_loader, getNamespaces, getLocalNamespace, yaf_loader_void_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(loader) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Loader",  "Yaf\\Loader", yaf_loader_methods);
	yaf_loader_ce = zend_register_internal_class_ex(&ce, NULL);
#if PHP_VERSION_ID < 80100
	yaf_loader_ce->ce_flags |= ZEND_ACC_FINAL;
	yaf_loader_ce->serialize = zend_class_serialize_deny;
	yaf_loader_ce->unserialize = zend_class_unserialize_deny;
#else
	yaf_loader_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NOT_SERIALIZABLE;
#endif

	memcpy(&yaf_loader_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_loader_obj_handlers.clone_obj = NULL;
	yaf_loader_obj_handlers.get_gc = yaf_fake_get_gc;
	yaf_loader_obj_handlers.free_obj = yaf_loader_obj_free;
	yaf_loader_obj_handlers.get_properties = yaf_loader_get_properties;

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
