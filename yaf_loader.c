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
#include "ext/standard/php_string.h" /* php_trim */

#ifdef __SSE2__
#include <emmintrin.h>
#endif

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
    ZEND_ARG_INFO(0, name_prefix)
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

int yaf_loader_register(yaf_loader_t *loader) /* {{{ */ {
	zval autoload, function, method, ret;

	array_init(&autoload);

    ZVAL_STRING(&method, YAF_AUTOLOAD_FUNC_NAME);
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
			1
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

static int yaf_loader_identify_category(zend_string *class_name, zend_bool suffix) /* {{{ */ {
	char *name = ZSTR_VAL(class_name);
	size_t len = ZSTR_LEN(class_name);
	int suspense_type = YAF_CLASS_NAME_NORMAL;

	if (suffix) {
		switch (name[len - 1]) {
			case 'r':
				if (len < sizeof(YAF_LOADER_CONTROLLER) - 1) {
					return YAF_CLASS_NAME_NORMAL;
				}
				len -= sizeof(YAF_LOADER_CONTROLLER) - 1;
				name += len;
				if (memcmp(name, YAF_LOADER_CONTROLLER, sizeof(YAF_LOADER_CONTROLLER) - 1 - 1)) {
					return YAF_CLASS_NAME_NORMAL;
				}
				suspense_type = YAF_CLASS_NAME_CONTROLLER;
				break;
			case 'n':
				if (len < sizeof(YAF_LOADER_PLUGIN) - 1) {
					return YAF_CLASS_NAME_NORMAL;
				}
				len -= sizeof(YAF_LOADER_PLUGIN) - 1;
				name += len;
				if (memcmp(name, YAF_LOADER_PLUGIN, sizeof(YAF_LOADER_PLUGIN) - 1 - 1)) {
					return YAF_CLASS_NAME_NORMAL;
				}
				suspense_type = YAF_CLASS_NAME_PLUGIN;
				break;
			case 'l':
				if (len < sizeof(YAF_LOADER_MODEL) - 1) {
					return YAF_CLASS_NAME_NORMAL;
				}
				len -= sizeof(YAF_LOADER_MODEL) - 1;
				name += len;
				if (memcmp(name, YAF_LOADER_MODEL, sizeof(YAF_LOADER_MODEL) - 1 - 1)) {
					return YAF_CLASS_NAME_NORMAL;
				}
				suspense_type = YAF_CLASS_NAME_MODEL;
				break;
			default:
				return YAF_CLASS_NAME_NORMAL;
		}
		if (YAF_G(name_separator)) {
			if (len > YAF_G(name_separator_len) &&
				memcmp(name - YAF_G(name_separator_len), YAF_G(name_separator), YAF_G(name_separator_len)) == 0) {
				return suspense_type;
			}
		}
	} else {
		switch (*name) {
			case 'C':
				if (len < sizeof(YAF_LOADER_CONTROLLER) - 1) {
					return YAF_CLASS_NAME_NORMAL;
				}
				if (memcmp(name + 1, YAF_LOADER_CONTROLLER + 1, sizeof(YAF_LOADER_CONTROLLER) - 1 - 1)) {
					return YAF_CLASS_NAME_NORMAL;
				}
				suspense_type = YAF_CLASS_NAME_CONTROLLER;
				name += sizeof(YAF_LOADER_CONTROLLER) - 1;
				len -= sizeof(YAF_LOADER_CONTROLLER) - 1;
				break;
			case 'P':
				if (len < sizeof(YAF_LOADER_PLUGIN) - 1) {
					return YAF_CLASS_NAME_NORMAL;
				}
				if (memcmp(name + 1, YAF_LOADER_PLUGIN + 1, sizeof(YAF_LOADER_PLUGIN) - 1 - 1)) {
					return YAF_CLASS_NAME_NORMAL;
				}
				suspense_type = YAF_CLASS_NAME_PLUGIN;
				name += sizeof(YAF_LOADER_PLUGIN) - 1;
				len -= sizeof(YAF_LOADER_PLUGIN) - 1;
				break;
			case 'M':
				if (len < sizeof(YAF_LOADER_MODEL) - 1) {
					return YAF_CLASS_NAME_NORMAL;
				}
				if (memcmp(name + 1, YAF_LOADER_MODEL + 1, sizeof(YAF_LOADER_MODEL) - 1 - 1)) {
					return YAF_CLASS_NAME_NORMAL;
				}
				suspense_type = YAF_CLASS_NAME_MODEL;
				name += sizeof(YAF_LOADER_MODEL) - 1;
				len -= sizeof(YAF_LOADER_MODEL) - 1;
				break;
			default:
				return YAF_CLASS_NAME_NORMAL;
		}
		if (YAF_G(name_separator)) {
			if (len > YAF_G(name_separator_len) &&
				memcmp(name, YAF_G(name_separator), YAF_G(name_separator_len)) == 0) {
				return suspense_type;
			}
		}
	}

	return YAF_CLASS_NAME_NORMAL;
}
/* }}} */

static inline char* yaf_loader_sanitize_name(char *name, size_t len) /* {{{ */ {
	char *pos;
	char *sanitized_name = name;

	if (UNEXPECTED((pos = memchr(name, '\\', len)))) {
		/* replace all '\' to '_' */
		sanitized_name = estrndup(name, len);
		pos = sanitized_name + (pos - name);
#ifdef __SSE2__
		do {
			const __m128i slash = _mm_set1_epi8('\\');
			const __m128i delta = _mm_set1_epi8('_' - '\\');
			len -= (pos - sanitized_name);
			while (len >= 16) {
				__m128i op = _mm_loadu_si128((__m128i *)pos);
				__m128i eq = _mm_cmpeq_epi8(op, slash);
				if (_mm_movemask_epi8(eq)) {
					eq = _mm_and_si128(eq, delta);
					op = _mm_add_epi8(op, eq);
					_mm_storeu_si128((__m128i*)pos, op);
				}
				len -= 16;
				pos += 16;
			}
		} while (0);

		if (len) {
			name = pos;
			while ((pos = memchr(pos, '\\', len - (pos - name)))) {
				*pos++ = '_';
			}
		}
#else
		while ((*pos++ = '_', pos = memchr(pos, '\\', len - (pos - sanitized_name))));
#endif
	}

	return sanitized_name;
}
/* }}} */

/** {{{ int yaf_loader_is_local_namespace(yaf_loader_t *loader, char *class_name, int len)
 */
int yaf_loader_is_local_namespace(yaf_loader_t *loader, char *class_name, int len) {
	char *pos;
	size_t prefix_len;

	if (!YAF_G(local_namespaces) || zend_hash_num_elements(YAF_G(local_namespaces)) == 0) {
		return 0;
	}


	if ((pos = memchr(class_name, '_', len))) {
		prefix_len = pos - class_name;
	} else {
		prefix_len = len;
	}

	return zend_hash_str_exists(YAF_G(local_namespaces), class_name, prefix_len);
}
/* }}} */

yaf_loader_t *yaf_loader_instance(yaf_loader_t *this_ptr, zend_string *library_path, zend_string *global_path) /* {{{ */ {
	yaf_loader_t *instance;

	instance = zend_read_static_property(yaf_loader_ce, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_INSTANCE), 1);

	if (IS_OBJECT == Z_TYPE_P(instance)) {
		if (library_path) {
			zend_update_property_str(yaf_loader_ce,
					instance, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_LIBRARY), library_path);
		}

		if (global_path) {
			zend_update_property_str(yaf_loader_ce,
					instance, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_GLOBAL_LIB), global_path);
		}
		return instance;
	}

	if (!global_path && !library_path) {
		php_error_docref(NULL, E_WARNING, "Missed library directory arguments");
		return NULL;
	}

	if (Z_ISUNDEF_P(this_ptr)) {
        object_init_ex(this_ptr, yaf_loader_ce);
	}

	if (library_path && global_path) {
		zend_update_property_str(yaf_loader_ce,
				this_ptr, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_LIBRARY), library_path);
		zend_update_property_str(yaf_loader_ce,
				this_ptr, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_GLOBAL_LIB), global_path);
	} else if (!global_path) {
		zend_update_property_str(yaf_loader_ce,
				this_ptr, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_LIBRARY), library_path);
		zend_update_property_str(yaf_loader_ce,
				this_ptr, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_GLOBAL_LIB), library_path);
	} else {
		zend_update_property_str(yaf_loader_ce,
				this_ptr, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_LIBRARY), global_path);
		zend_update_property_str(yaf_loader_ce,
				this_ptr, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_GLOBAL_LIB), global_path);
	}

	zend_update_static_property(yaf_loader_ce, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_INSTANCE), this_ptr);

	if (!yaf_loader_register(this_ptr)) {
		php_error_docref(NULL, E_WARNING, "Failed to register autoload function");
	}

	return this_ptr;
}
/* }}} */

int yaf_loader_import(const char *path, uint32_t len) /* {{{ */ {
	zend_file_handle file_handle;
	zend_op_array 	*op_array;
	char realpath[MAXPATHLEN];

	if (!VCWD_REALPATH(path, realpath)) {
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

	op_array = zend_compile_file(&file_handle, ZEND_INCLUDE);

	if (op_array && file_handle.handle.stream.handle) {
		if (!file_handle.opened_path) {
			file_handle.opened_path = zend_string_init(path, len, 0);
		}

		zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path);
	}
	zend_destroy_file_handle(&file_handle);

	if (EXPECTED(op_array)) {
		zval result;

        ZVAL_UNDEF(&result);

		zend_execute(op_array, &result);

		destroy_op_array(op_array);
		efree(op_array);
        zval_ptr_dtor(&result);

	    return 1;
	}

	return 0;
}
/* }}} */

int yaf_loader_load(yaf_loader_t *loader, char *filename, size_t fname_len, char *directory, uint32_t directory_len) /* {{{ */ {
	char * position = directory + directory_len;

	if (directory_len == 0) {
		if (EXPECTED(loader)) {
			zval *library_dir, rv;

			if (UNEXPECTED(loader == NULL)) {
				loader = yaf_loader_instance(&rv, NULL, NULL);
			}

			if (yaf_loader_is_local_namespace(loader, filename, fname_len)) {
				library_dir = zend_read_property(yaf_loader_ce, loader, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_LIBRARY), 1, NULL);
			} else {
				library_dir	= zend_read_property(yaf_loader_ce, loader, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_GLOBAL_LIB), 1, NULL);
			}

			if (UNEXPECTED(Z_STRLEN_P(library_dir) + fname_len + directory_len + ZSTR_LEN(YAF_G(ext)) + 4 > MAXPATHLEN)) {
				*position = '\0';
				php_error_docref(NULL, E_WARNING, "path too long: '%s+%s+%s'", directory, Z_STRVAL_P(library_dir), filename);
				return 0;
			}

			if (EXPECTED(Z_STRLEN_P(library_dir))) {
				memcpy(position, Z_STRVAL_P(library_dir), Z_STRLEN_P(library_dir));
				position += Z_STRLEN_P(library_dir);
			}
		} else {
			*position = '\0';
			/* since only call from userspace can cause loader is NULL, exception throw should work well */
			php_error_docref(NULL, E_WARNING, "%s need to be initialize first", ZSTR_VAL(yaf_loader_ce->name));
			return 0;
		}
	} else if (UNEXPECTED((directory_len + fname_len + ZSTR_LEN(YAF_G(ext)) + 3) > MAXPATHLEN)) {
		*position = '\0';
		php_error_docref(NULL, E_WARNING, "path too long: '%s/%s'", directory, filename);
		return 0;
	}

	/* aussume all the path is not end in slash */
	*position++ = DEFAULT_SLASH;

	if (UNEXPECTED(YAF_G(lowcase_path))) {
		uint32_t i = 0;
		for (; i < fname_len; i++) {
			if (filename[i] == '_') {
				*position++ = DEFAULT_SLASH;
			} else {
				*position++ = tolower(filename[i]);
			}
		}
	} else {
		uint32_t i = 0;
		for (; i < fname_len; i++) {
			if (filename[i] == '_') {
				*position++ = DEFAULT_SLASH;
			} else {
				*position++ = filename[i];
			}
		}
	}

	*position++ = '.';
	memcpy(position, ZSTR_VAL(YAF_G(ext)), ZSTR_LEN(YAF_G(ext)));
	position += ZSTR_LEN(YAF_G(ext));
	*position = '\0';
	directory_len = position - directory;

	return yaf_loader_import(directory, directory_len);
}
/* }}} */

int yaf_loader_register_namespace_single(zend_string *prefix) /* {{{ */ {
	zend_string *sanitized_prefix;

	if (!YAF_G(local_namespaces)) {
		ALLOC_HASHTABLE(YAF_G(local_namespaces));
		zend_hash_init(YAF_G(local_namespaces), 8, NULL, NULL, 0);
	}
	sanitized_prefix = php_trim(prefix, NULL, 0, 3);

	zend_hash_add_empty_element(YAF_G(local_namespaces), sanitized_prefix);
	zend_string_release(sanitized_prefix);

	return 1;
}
/* }}} */

/** {{{ int yaf_loader_register_namespace_multi(zval *prefixes)
 */
int yaf_loader_register_namespace_multi(zval *prefixes) {
	zval *pzval;
	HashTable *ht;

	ht = Z_ARRVAL_P(prefixes);
	ZEND_HASH_FOREACH_VAL(ht, pzval) {
		if (IS_STRING == Z_TYPE_P(pzval)) {
			yaf_loader_register_namespace_single(Z_STR_P(pzval));
		}
	} ZEND_HASH_FOREACH_END();

	return 1;
}
/* }}} */

/** {{{ proto private Yaf_Loader::__construct(void)
*/
PHP_METHOD(yaf_loader, __construct) {
}
/* }}} */

/** {{{ proto private Yaf_Loader::__sleep(void)
*/
PHP_METHOD(yaf_loader, __sleep) {
}
/* }}} */

/** {{{ proto private Yaf_Loader::__wakeup(void)
*/
PHP_METHOD(yaf_loader, __wakeup) {
}
/* }}} */

/** {{{ proto private Yaf_Loader::__clone(void)
*/
PHP_METHOD(yaf_loader, __clone) {
}
/* }}} */

/** {{{ proto public Yaf_Loader::registerLocalNamespace(mixed $namespace)
*/
PHP_METHOD(yaf_loader, registerLocalNamespace) {
	zval *namespaces;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &namespaces) == FAILURE) {
		return;
	}

	if (IS_STRING == Z_TYPE_P(namespaces)) {
		if (yaf_loader_register_namespace_single(Z_STR_P(namespaces))) {
			RETURN_ZVAL(getThis(), 1, 0);
		}
	} else if (IS_ARRAY == Z_TYPE_P(namespaces)) {
		if(yaf_loader_register_namespace_multi(namespaces)) {
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
	if (YAF_G(local_namespaces)) {
		ZVAL_ARR(return_value, YAF_G(local_namespaces));
		Z_TRY_ADDREF_P(return_value);
		return;
	}
	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Loader::clearLocalNamespace(void)
*/
PHP_METHOD(yaf_loader, clearLocalNamespace) {
	zend_hash_clean(YAF_G(local_namespaces));
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Loader::isLocalName(string $class_name)
*/
PHP_METHOD(yaf_loader, isLocalName) {
	zend_string *name;
	char *sanitized_name;
	int result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	sanitized_name = yaf_loader_sanitize_name(ZSTR_VAL(name), ZSTR_LEN(name));

	result = yaf_loader_is_local_namespace(getThis(), sanitized_name, ZSTR_LEN(name));

	if (sanitized_name != ZSTR_VAL(name)) {
		efree(sanitized_name);
	}

	RETURN_BOOL(result);
}
/* }}} */

/** {{{ proto public Yaf_Loader::setLibraryPath(string $path, $global = FALSE)
*/
PHP_METHOD(yaf_loader, setLibraryPath) {
	zend_string *library;
	zend_bool global = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|b", &library, &global) == FAILURE) {
		return;
	}

	if (!global) {
		zend_update_property_str(yaf_loader_ce, getThis(), ZEND_STRL(YAF_LOADER_PROPERTY_NAME_LIBRARY), library);
	} else {
		zend_update_property_str(yaf_loader_ce, getThis(), ZEND_STRL(YAF_LOADER_PROPERTY_NAME_GLOBAL_LIB), library);
	}

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Loader::getLibraryPath($global = FALSE)
*/
PHP_METHOD(yaf_loader, getLibraryPath) {
	zval *library;
	zend_bool global = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &global) == FAILURE) {
		return;
	}

	if (!global) {
		library = zend_read_property(yaf_loader_ce, getThis(), ZEND_STRL(YAF_LOADER_PROPERTY_NAME_LIBRARY), 1, NULL);
	} else {
		library = zend_read_property(yaf_loader_ce, getThis(), ZEND_STRL(YAF_LOADER_PROPERTY_NAME_GLOBAL_LIB), 1, NULL);
	}

	RETURN_ZVAL(library, 1, 0);
}
/* }}} */

/** {{{ proto public static Yaf_Loader::import($file)
*/
PHP_METHOD(yaf_loader, import) {
	zend_string *file;
	int need_free = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &file) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(file) == 0) {
		RETURN_FALSE;
	} else {
		int retval;
		yaf_loader_t *loader, rv = {{0}};

		if (!IS_ABSOLUTE_PATH(ZSTR_VAL(file), ZSTR_LEN(file))) {
			loader = yaf_loader_instance(&rv, NULL, NULL);
			if (loader == NULL) {
				php_error_docref(NULL, E_WARNING, "%s need to be initialize first", ZSTR_VAL(yaf_loader_ce->name));
				RETURN_FALSE;
			} else {
				zval *library = zend_read_property(yaf_loader_ce,
						loader, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_LIBRARY), 1, NULL);
				file = strpprintf(0, "%s%c%s", Z_STRVAL_P(library), DEFAULT_SLASH, ZSTR_VAL(file));
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

/** {{{ proto public Yaf_Loader::autoload($class_name)
*/
PHP_METHOD(yaf_loader, autoload) {
	zend_string *class_name;
	char directory[MAXPATHLEN];
	char *sanitized_name;
	size_t sanitized_len;
	uint32_t directory_len = 0;
	uint32_t class_type, status;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &class_name) == FAILURE) {
		return;
	}

	if (UNEXPECTED(ZSTR_LEN(class_name) == 0)) {
		RETURN_FALSE;
	}

	if (UNEXPECTED(ZSTR_LEN(class_name) >= sizeof(YAF_LOADER_RESERVERD) - 1 &&
		memcmp(ZSTR_VAL(class_name), YAF_LOADER_RESERVERD, sizeof(YAF_LOADER_RESERVERD) - 1) == 0)) {
		php_error_docref(NULL, E_WARNING, "You should not use '%s' as class name prefix", YAF_LOADER_RESERVERD);
	}

	sanitized_len = ZSTR_LEN(class_name);
	sanitized_name = yaf_loader_sanitize_name(ZSTR_VAL(class_name), ZSTR_LEN(class_name));

	if ((class_type = yaf_loader_identify_category(class_name, YAF_G(name_suffix))) != YAF_CLASS_NAME_NORMAL) {
		uint32_t fname_len;

		if (UNEXPECTED(YAF_G(directory) == NULL)) {
			php_error_docref(NULL, E_WARNING, "Couldn't load a MVC class unless an %s is initialized", ZSTR_VAL(yaf_application_ce->name));
			if (sanitized_name != ZSTR_VAL(class_name)) {
				efree(class_name);
			}
			RETURN_FALSE;
		}

		directory_len = snprintf(directory, sizeof(directory), "%s%c", ZSTR_VAL(YAF_G(directory)), DEFAULT_SLASH);
		switch (class_type) {
			case YAF_CLASS_NAME_MODEL:
				directory_len += snprintf(directory + directory_len, sizeof(directory) - directory_len, YAF_MODEL_DIRECTORY_NAME);
				fname_len = sanitized_len - (sizeof(YAF_LOADER_MODEL) - 1) - YAF_G(name_separator_len);
				break;
			case YAF_CLASS_NAME_PLUGIN:
				directory_len += snprintf(directory + directory_len, sizeof(directory) - directory_len, YAF_PLUGIN_DIRECTORY_NAME);
				fname_len = sanitized_len - (sizeof(YAF_LOADER_PLUGIN) - 1) - YAF_G(name_separator_len);
				break;
			case YAF_CLASS_NAME_CONTROLLER:
				directory_len += snprintf(directory + directory_len, sizeof(directory) - directory_len, YAF_CONTROLLER_DIRECTORY_NAME);
				fname_len = sanitized_len - (sizeof(YAF_LOADER_CONTROLLER) - 1) - YAF_G(name_separator_len);
				break;
			default:
				ZEND_ASSERT(0);
				break;
		}
		if (EXPECTED(YAF_G(name_suffix))) {
			status = yaf_loader_load(getThis(), sanitized_name, fname_len, directory, directory_len);
		} else {
			status = yaf_loader_load(getThis(), sanitized_name + sanitized_len - fname_len, fname_len, directory, directory_len);
		}
	} else {
		status = yaf_loader_load(getThis(), sanitized_name, sanitized_len, directory, 0);
	}

	if (sanitized_name != ZSTR_VAL(class_name)) {
		efree(sanitized_name);
	}

	if (status) {
		zend_string *lc_name = zend_string_tolower(class_name);
		if (UNEXPECTED(!zend_hash_exists(EG(class_table), lc_name))) {
			if (EXPECTED(!YAF_G(use_spl_autoload))) {
				php_error_docref(NULL, E_STRICT, "Could not find class %s in %s", ZSTR_VAL(class_name), directory);
				RETURN_TRUE;
			}
		}
		zend_string_release(lc_name);
	} else if (EXPECTED(!YAF_G(use_spl_autoload))) {
		php_error_docref(NULL, E_WARNING, "Failed opening script %s: %s", directory, strerror(errno));
		RETURN_TRUE;
	}

	RETURN_BOOL(status);
}
/* }}} */

/** {{{ proto public Yaf_Loader::getInstance($library = NULL, $global_library = NULL)
*/
PHP_METHOD(yaf_loader, getInstance) {
	zend_string *library = NULL;
	zend_string *global = NULL;
	yaf_loader_t *loader, rv = {{0}};

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|SS", &library, &global) == FAILURE) {
		return;
	}

	loader = yaf_loader_instance(&rv, library, global);
	if (loader) {
		RETURN_ZVAL(loader, 1, 0);
    } else {

	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto private Yaf_Loader::__desctruct(void)
*/
PHP_METHOD(yaf_loader, __destruct) {
}
/* }}} */

/** {{{ yaf_loader_methods
*/
zend_function_entry yaf_loader_methods[] = {
	PHP_ME(yaf_loader, __construct, yaf_loader_void_arginfo, ZEND_ACC_PRIVATE|ZEND_ACC_CTOR)
	PHP_ME(yaf_loader, __clone, NULL, ZEND_ACC_PRIVATE)
	PHP_ME(yaf_loader, __sleep, NULL, ZEND_ACC_PRIVATE)
	PHP_ME(yaf_loader, __wakeup, NULL, ZEND_ACC_PRIVATE)
	PHP_ME(yaf_loader, autoload, yaf_loader_autoloader_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_loader, getInstance, yaf_loader_getinstance_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(yaf_loader, registerLocalNamespace, yaf_loader_regnamespace_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_loader, getLocalNamespace, yaf_loader_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_loader, clearLocalNamespace, yaf_loader_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_loader, isLocalName, yaf_loader_islocalname_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_loader, import, yaf_loader_import_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(yaf_loader, setLibraryPath, yaf_loader_setlib_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_loader, getLibraryPath, yaf_loader_getlib_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(loader) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Loader",  "Yaf\\Loader", yaf_loader_methods);
	yaf_loader_ce = zend_register_internal_class_ex(&ce, NULL);
	yaf_loader_ce->ce_flags |= ZEND_ACC_FINAL;

	zend_declare_property_null(yaf_loader_ce, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_LIBRARY), 	 ZEND_ACC_PROTECTED);
	zend_declare_property_null(yaf_loader_ce, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_GLOBAL_LIB), ZEND_ACC_PROTECTED);
	zend_declare_property_null(yaf_loader_ce, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_INSTANCE),	 ZEND_ACC_PROTECTED|ZEND_ACC_STATIC);

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
