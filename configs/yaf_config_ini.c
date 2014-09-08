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

/* $Id: ini.c 329197 2013-01-18 05:55:37Z laruence $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "Zend/zend_interfaces.h"

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_config.h"

#include "configs/yaf_config_ini.h"

zend_class_entry *yaf_config_ini_ce;

#ifdef HAVE_SPL
extern PHPAPI zend_class_entry *spl_ce_Countable;
#endif

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_config_ini_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_ini_construct_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, config_file)
	ZEND_ARG_INFO(0, section)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_ini_get_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_ini_rget_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_ini_unset_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_ini_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_ini_isset_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ static inline yaf_deep_copy_section(zval *dst, zval *src TSRMLS_DC)
 */
static inline void yaf_deep_copy_section(zval *dst, zval *src TSRMLS_DC) {
	zval **ppzval, **dstppzval, *value;
	HashTable *ht;
	ulong idx;
	char *key;
	uint key_len;

	ht = Z_ARRVAL_P(src);
	for(zend_hash_internal_pointer_reset(ht);
			zend_hash_has_more_elements(ht) == SUCCESS;
			zend_hash_move_forward(ht)) {
		if (zend_hash_get_current_data(ht, (void **)&ppzval) == FAILURE) {
			continue;
		}

		switch (zend_hash_get_current_key_ex(ht, &key, &key_len, &idx, 0, NULL)) {
			case HASH_KEY_IS_STRING:
				if (Z_TYPE_PP(ppzval) == IS_ARRAY 
						&& zend_hash_find(Z_ARRVAL_P(dst), key, key_len, (void **)&dstppzval) == SUCCESS
						&& Z_TYPE_PP(dstppzval) == IS_ARRAY) {
					MAKE_STD_ZVAL(value);
					array_init(value);
					yaf_deep_copy_section(value, *dstppzval TSRMLS_CC);
					yaf_deep_copy_section(value, *ppzval TSRMLS_CC);
				} else {
					value = *ppzval;
					Z_ADDREF_P(value);
				}
				zend_hash_update(Z_ARRVAL_P(dst), key, key_len, (void *)&value, sizeof(zval *), NULL);
				break;
			case HASH_KEY_IS_LONG:
				if (Z_TYPE_PP(ppzval) == IS_ARRAY
						&& zend_hash_index_find(Z_ARRVAL_P(dst), idx, (void **)&dstppzval) == SUCCESS
						&& Z_TYPE_PP(dstppzval) == IS_ARRAY) {
					MAKE_STD_ZVAL(value);
					array_init(value);
					yaf_deep_copy_section(value, *dstppzval TSRMLS_CC);
					yaf_deep_copy_section(value, *ppzval TSRMLS_CC);
				} else {
					value = *ppzval;
					Z_ADDREF_P(value);
				}
				zend_hash_index_update(Z_ARRVAL_P(dst), idx, (void *)&value, sizeof(zval *), NULL);
				break;
			case HASH_KEY_NON_EXISTANT:
				break;
		}
	}
}
/* }}} */

/** {{{ zval * yaf_config_ini_format(yaf_config_t *instance, zval **ppzval TSRMLS_DC)
*/
zval * yaf_config_ini_format(yaf_config_t *instance, zval **ppzval TSRMLS_DC) {
	zval *ret;
	ret = yaf_config_ini_instance(NULL, *ppzval, NULL TSRMLS_CC);
	return ret;
}
/* }}} */

#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION > 2))
/** {{{ static void yaf_config_ini_simple_parser_cb(zval *key, zval *value, zval *index, int callback_type, zval *arr TSRMLS_DC)
*/
static void yaf_config_ini_simple_parser_cb(zval *key, zval *value, zval *index, int callback_type, zval *arr TSRMLS_DC) {
	zval *element;
	switch (callback_type) {
		case ZEND_INI_PARSER_ENTRY:
			{
				char *skey, *seg, *ptr;
				zval **ppzval, *dst;

				if (!value) {
					break;
				}

				dst = arr;
				skey = estrndup(Z_STRVAL_P(key), Z_STRLEN_P(key));
				if ((seg = php_strtok_r(skey, ".", &ptr))) {
					do {
					    char *real_key = seg;
						seg = php_strtok_r(NULL, ".", &ptr);
						if (zend_symtable_find(Z_ARRVAL_P(dst), real_key, strlen(real_key) + 1, (void **) &ppzval) == FAILURE) {
							if (seg) {
								zval *tmp;
							    MAKE_STD_ZVAL(tmp);   
								array_init(tmp);
								zend_symtable_update(Z_ARRVAL_P(dst), 
										real_key, strlen(real_key) + 1, (void **)&tmp, sizeof(zval *), (void **)&ppzval);
							} else {
							    MAKE_STD_ZVAL(element);
								ZVAL_ZVAL(element, value, 1, 0);
								zend_symtable_update(Z_ARRVAL_P(dst), 
										real_key, strlen(real_key) + 1, (void **)&element, sizeof(zval *), NULL);
								break;
							}
						} else {
							SEPARATE_ZVAL(ppzval);
							if (IS_ARRAY != Z_TYPE_PP(ppzval)) {
								if (seg) {
									zval *tmp;
									MAKE_STD_ZVAL(tmp);   
									array_init(tmp);
									zend_symtable_update(Z_ARRVAL_P(dst), 
											real_key, strlen(real_key) + 1, (void **)&tmp, sizeof(zval *), (void **)&ppzval);
								} else {
									MAKE_STD_ZVAL(element);
									ZVAL_ZVAL(element, value, 1, 0);
									zend_symtable_update(Z_ARRVAL_P(dst), 
											real_key, strlen(real_key) + 1, (void **)&element, sizeof(zval *), NULL);
								}
							} 
						}
						dst = *ppzval;
					} while (seg);
				}
				efree(skey);
			}
			break;

		case ZEND_INI_PARSER_POP_ENTRY:
			{
				zval *hash, **find_hash, *dst;

				if (!value) {
					break;
				}

				if (!(Z_STRLEN_P(key) > 1 && Z_STRVAL_P(key)[0] == '0')
						&& is_numeric_string(Z_STRVAL_P(key), Z_STRLEN_P(key), NULL, NULL, 0) == IS_LONG) {
					ulong skey = (ulong)zend_atol(Z_STRVAL_P(key), Z_STRLEN_P(key));
					if (zend_hash_index_find(Z_ARRVAL_P(arr), skey, (void **) &find_hash) == FAILURE) {
						MAKE_STD_ZVAL(hash);
						array_init(hash);
						zend_hash_index_update(Z_ARRVAL_P(arr), skey, &hash, sizeof(zval *), NULL);
					} else {
						hash = *find_hash;
					}
				} else {
					char *seg, *ptr;
					char *skey = estrndup(Z_STRVAL_P(key), Z_STRLEN_P(key));

					dst = arr;
					if ((seg = php_strtok_r(skey, ".", &ptr))) {
						while (seg) {
							if (zend_symtable_find(Z_ARRVAL_P(dst), seg, strlen(seg) + 1, (void **) &find_hash) == FAILURE) {
								MAKE_STD_ZVAL(hash);
								array_init(hash);
								zend_symtable_update(Z_ARRVAL_P(dst), 
										seg, strlen(seg) + 1, (void **)&hash, sizeof(zval *), (void **)&find_hash);
							}
							dst = *find_hash;
							seg = php_strtok_r(NULL, ".", &ptr);
						}
						hash = dst;
					} else {
						if (zend_symtable_find(Z_ARRVAL_P(dst), seg, strlen(seg) + 1, (void **)&find_hash) == FAILURE) {
							MAKE_STD_ZVAL(hash);
							array_init(hash);
							zend_symtable_update(Z_ARRVAL_P(dst), seg, strlen(seg) + 1, (void **)&hash, sizeof(zval *), NULL);
						} else {
							hash = *find_hash;
						}
					}
					efree(skey);
				}

				if (Z_TYPE_P(hash) != IS_ARRAY) {
					zval_dtor(hash);
					INIT_PZVAL(hash);
					array_init(hash);
				}

				MAKE_STD_ZVAL(element);
				ZVAL_ZVAL(element, value, 1, 0);

				if (index && Z_STRLEN_P(index) > 0) {
					add_assoc_zval_ex(hash, Z_STRVAL_P(index), Z_STRLEN_P(index) + 1, element);
				} else {
					add_next_index_zval(hash, element);
				}
			}
			break;

		case ZEND_INI_PARSER_SECTION:
			break;
	}
}
/* }}} */

/** {{{ static void yaf_config_ini_parser_cb(zval *key, zval *value, zval *index, int callback_type, zval *arr TSRMLS_DC)
*/
static void yaf_config_ini_parser_cb(zval *key, zval *value, zval *index, int callback_type, zval *arr TSRMLS_DC) {

	if (YAF_G(parsing_flag) == YAF_CONFIG_INI_PARSING_END) {
		return;
	}

	if (callback_type == ZEND_INI_PARSER_SECTION) {
		zval **parent;
		char *seg, *skey, *skey_orig;
		uint skey_len;

		if (YAF_G(parsing_flag) == YAF_CONFIG_INI_PARSING_PROCESS) {
			YAF_G(parsing_flag) = YAF_CONFIG_INI_PARSING_END;
			return;
		}

		skey_orig = skey = estrndup(Z_STRVAL_P(key), Z_STRLEN_P(key));
		skey_len = Z_STRLEN_P(key);
		while (*skey == ' ') {
			*(skey++) = '\0';
			skey_len--;
		}
		if (skey_len > 1) {
			seg = skey + skey_len - 1;
			while (*seg == ' ' || *seg == ':') {
				*(seg--) = '\0';
				skey_len--;
			}
		}

		MAKE_STD_ZVAL(YAF_G(active_ini_file_section));
		array_init(YAF_G(active_ini_file_section));

		if ((seg = strchr(skey, ':'))) {
			char *section, *p;

			if (seg > skey) {
				p = seg - 1;
				while (*p == ' ' || *p == ':') {
					*(p--) = '\0';
				}
			}

			while (*(seg) == ' ' || *(seg) == ':') {
				*(seg++) = '\0';
			}

			if ((section = strrchr(seg, ':'))) {
			    /* muilt-inherit */
				do {
					if (section > seg) {
						p = section - 1;
						while (*p == ' ' || *p == ':') {
							*(p--) = '\0';
						}
					}
					while (*(section) == ' ' || *(section) == ':') {
						*(section++) = '\0';
					}
					if (zend_symtable_find(Z_ARRVAL_P(arr), section, strlen(section) + 1, (void **)&parent) == SUCCESS) {
						yaf_deep_copy_section(YAF_G(active_ini_file_section), *parent TSRMLS_CC);
					}
				} while ((section = strrchr(seg, ':')));
			}

			if (zend_symtable_find(Z_ARRVAL_P(arr), seg, strlen(seg) + 1, (void **)&parent) == SUCCESS) {
				yaf_deep_copy_section(YAF_G(active_ini_file_section), *parent TSRMLS_CC);
			}
			skey_len = strlen(skey);
		}
		zend_symtable_update(Z_ARRVAL_P(arr), skey, skey_len + 1, &YAF_G(active_ini_file_section), sizeof(zval *), NULL);
		if (YAF_G(ini_wanted_section) && Z_STRLEN_P(YAF_G(ini_wanted_section)) == skey_len
				&& !strncasecmp(Z_STRVAL_P(YAF_G(ini_wanted_section)), skey, skey_len)) {
			YAF_G(parsing_flag) = YAF_CONFIG_INI_PARSING_PROCESS;
		}
		efree(skey_orig);
	} else if (value) {
		zval *active_arr;
		if (YAF_G(active_ini_file_section)) {
			active_arr = YAF_G(active_ini_file_section);
		} else {
			active_arr = arr;
		}
		yaf_config_ini_simple_parser_cb(key, value, index, callback_type, active_arr TSRMLS_CC);
	}
}
/* }}} */
#else 
/** {{{ static void yaf_config_ini_simple_parser_cb(zval *key, zval *value, int callback_type, zval *arr)
*/
static void yaf_config_ini_simple_parser_cb(zval *key, zval *value, int callback_type, zval *arr) {
	zval *element;
	switch (callback_type) {
		case ZEND_INI_PARSER_ENTRY:
			{
				char *skey, *seg, *ptr;
				zval **ppzval, *dst;

				if (!value) {
					break;
				}
				
				dst = arr;
				skey = estrndup(Z_STRVAL_P(key), Z_STRLEN_P(key));
				if ((seg = php_strtok_r(skey, ".", &ptr))) {
					do {
					    char *real_key = seg;
						seg = php_strtok_r(NULL, ".", &ptr);
						if (zend_symtable_find(Z_ARRVAL_P(dst), real_key, strlen(real_key) + 1, (void **) &ppzval) == FAILURE) {
							if (seg) {
								zval *tmp;
							    MAKE_STD_ZVAL(tmp);   
								array_init(tmp);
								zend_symtable_update(Z_ARRVAL_P(dst), 
										real_key, strlen(real_key) + 1, (void **)&tmp, sizeof(zval *), (void **)&ppzval);
							} else {
							    MAKE_STD_ZVAL(element);
								ZVAL_ZVAL(element, value, 1, 0);
								zend_symtable_update(Z_ARRVAL_P(dst), 
										real_key, strlen(real_key) + 1, (void **)&element, sizeof(zval *), NULL);
								break;
							}
						} else {
							SEPARATE_ZVAL(ppzval);
							if (IS_ARRAY != Z_TYPE_PP(ppzval)) {
								if (seg) {
									zval *tmp;
									MAKE_STD_ZVAL(tmp);   
									array_init(tmp);
									zend_symtable_update(Z_ARRVAL_P(dst), 
											real_key, strlen(real_key) + 1, (void **)&tmp, sizeof(zval *), (void **)&ppzval);
								} else {
									MAKE_STD_ZVAL(element);
									ZVAL_ZVAL(element, value, 1, 0);
									zend_symtable_update(Z_ARRVAL_P(dst), 
											real_key, strlen(real_key) + 1, (void **)&element, sizeof(zval *), NULL);
								}
							} 
						}
						dst = *ppzval;
					} while (seg);
				}
				efree(skey);
			}
			break;

		case ZEND_INI_PARSER_POP_ENTRY:
			{
				zval *hash, **find_hash, *dst;

				if (!value) {
					break;
				}

				if (!(Z_STRLEN_P(key) > 1 && Z_STRVAL_P(key)[0] == '0')
						&& is_numeric_string(Z_STRVAL_P(key), Z_STRLEN_P(key), NULL, NULL, 0) == IS_LONG) {
					ulong skey = (ulong)zend_atol(Z_STRVAL_P(key), Z_STRLEN_P(key));
					if (zend_hash_index_find(Z_ARRVAL_P(arr), skey, (void **) &find_hash) == FAILURE) {
						MAKE_STD_ZVAL(hash);
						array_init(hash);
						zend_hash_index_update(Z_ARRVAL_P(arr), skey, &hash, sizeof(zval *), NULL);
					} else {
						hash = *find_hash;
					}
				} else {
					char *seg, *ptr;
					char *skey = estrndup(Z_STRVAL_P(key), Z_STRLEN_P(key));

					dst = arr;
					if ((seg = php_strtok_r(skey, ".", &ptr))) {
						while (seg) {
							if (zend_symtable_find(Z_ARRVAL_P(dst), seg, strlen(seg) + 1, (void **) &find_hash) == FAILURE) {
								MAKE_STD_ZVAL(hash);
								array_init(hash);
								zend_symtable_update(Z_ARRVAL_P(dst), 
										seg, strlen(seg) + 1, (void **)&hash, sizeof(zval *), (void **)&find_hash);
							}
							dst = *find_hash;
							seg = php_strtok_r(NULL, ".", &ptr);
						}
						hash = dst;
					} else {
						if (zend_symtable_find(Z_ARRVAL_P(dst), seg, strlen(seg) + 1, (void **)&find_hash) == FAILURE) {
							MAKE_STD_ZVAL(hash);
							array_init(hash);
							zend_symtable_update(Z_ARRVAL_P(dst), seg, strlen(seg) + 1, (void **)&hash, sizeof(zval *), NULL);
						} else {
							hash = *find_hash;
						}
					}
					efree(skey);
				}

				if (Z_TYPE_P(hash) != IS_ARRAY) {
					zval_dtor(hash);
					INIT_PZVAL(hash);
					array_init(hash);
				}

				MAKE_STD_ZVAL(element);
				ZVAL_ZVAL(element, value, 1, 0);
				add_next_index_zval(hash, element);
			}
			break;

		case ZEND_INI_PARSER_SECTION:
			break;
	}
}
/* }}} */

/** {{{ static void yaf_config_ini_parser_cb(zval *key, zval *value, int callback_type, zval *arr)
*/
static void yaf_config_ini_parser_cb(zval *key, zval *value, int callback_type, zval *arr) {
	TSRMLS_FETCH();

	if (YAF_G(parsing_flag) == YAF_CONFIG_INI_PARSING_END) {
		return;
	}

	if (callback_type == ZEND_INI_PARSER_SECTION) {
		zval **parent;
		char *seg, *skey, *skey_orig;
		uint skey_len;

		if (YAF_G(parsing_flag) == YAF_CONFIG_INI_PARSING_PROCESS) {
			YAF_G(parsing_flag) = YAF_CONFIG_INI_PARSING_END;
			return;
		}

		skey_orig = skey = estrndup(Z_STRVAL_P(key), Z_STRLEN_P(key));
		skey_len = Z_STRLEN_P(key);
		while (*skey == ' ') {
			*(skey++) = '\0';
			skey_len--;
		}
		if (skey_len > 1) {
			seg = skey + skey_len - 1;
			while (*seg == ' ' || *seg == ':') {
				*(seg--) = '\0';
				skey_len--;
			}
		}

		MAKE_STD_ZVAL(YAF_G(active_ini_file_section));
		array_init(YAF_G(active_ini_file_section));

		if ((seg = strchr(skey, ':'))) {
			char *section, *p;

			if (seg > skey) {
				p = seg - 1;
				while (*p == ' ' || *p == ':') {
					*(p--) = '\0';
				}
			}

			while (*seg == ' ' || *seg == ':') {
				*(seg++) = '\0';
			}

			if ((section = strrchr(seg, ':'))) {
			    /* muilt-inherit */
				do {
					if (section > seg) {
						p = section - 1;
						while (*p == ' ' || *p == ':') {
							*(p--) = '\0';
						}
					}
					while (*section == ' ' || *section == ':') {
						*(section++) = '\0';
					}
					if (zend_symtable_find(Z_ARRVAL_P(arr), section, strlen(section) + 1, (void **)&parent) == SUCCESS) {
						yaf_deep_copy_section(YAF_G(active_ini_file_section), *parent TSRMLS_CC);
					}
				} while ((section = strrchr(seg, ':')));
			}

			if (zend_symtable_find(Z_ARRVAL_P(arr), seg, strlen(seg) + 1, (void **)&parent) == SUCCESS) {
				yaf_deep_copy_section(YAF_G(active_ini_file_section), *parent TSRMLS_CC);
			}
			skey_len = strlen(skey);
		}
		zend_symtable_update(Z_ARRVAL_P(arr), skey, skey_len + 1, &YAF_G(active_ini_file_section), sizeof(zval *), NULL);
		if (YAF_G(ini_wanted_section) && Z_STRLEN_P(YAF_G(ini_wanted_section)) == skey_len
				&& !strncasecmp(Z_STRVAL_P(YAF_G(ini_wanted_section)), skey, Z_STRLEN_P(YAF_G(ini_wanted_section)))) {
			YAF_G(parsing_flag) = YAF_CONFIG_INI_PARSING_PROCESS;
		}
		efree(skey_orig);
	} else if (value) {
		zval *active_arr;
		if (YAF_G(active_ini_file_section)) {
			active_arr = YAF_G(active_ini_file_section);
		} else {
			active_arr = arr;
		}
		yaf_config_ini_simple_parser_cb(key, value, callback_type, active_arr);
	}
}
/* }}} */
#endif

/** {{{ yaf_config_t * yaf_config_ini_instance(yaf_config_t *this_ptr, zval *filename, zval *section_name TSRMLS_DC)
*/
yaf_config_t * yaf_config_ini_instance(yaf_config_t *this_ptr, zval *filename, zval *section_name TSRMLS_DC) {
	yaf_config_t *instance;
	zval *configs = NULL;

	if (filename && Z_TYPE_P(filename) == IS_ARRAY) {
		if (this_ptr) {
			instance = this_ptr;
		} else {
			MAKE_STD_ZVAL(instance);
			object_init_ex(instance, yaf_config_ini_ce);
		}

		zend_update_property(yaf_config_ini_ce, instance, ZEND_STRL(YAF_CONFIG_PROPERT_NAME), filename TSRMLS_CC);

		return instance;
	} else if (filename && Z_TYPE_P(filename) == IS_STRING) {
	    struct stat sb;
	    zend_file_handle fh = {0};
		char *ini_file = Z_STRVAL_P(filename);
		
		MAKE_STD_ZVAL(configs);
		ZVAL_NULL(configs);

		if (VCWD_STAT(ini_file, &sb) == 0) {
			if (S_ISREG(sb.st_mode)) {
				if ((fh.handle.fp = VCWD_FOPEN(ini_file, "r"))) {
					fh.filename = ini_file;
					fh.type = ZEND_HANDLE_FP;
					YAF_G(active_ini_file_section) = NULL;

					YAF_G(parsing_flag) = YAF_CONFIG_INI_PARSING_START;
					if (section_name && Z_STRLEN_P(section_name)) {
						YAF_G(ini_wanted_section) = section_name;
					} else {
						YAF_G(ini_wanted_section) = NULL;
					}

	 				array_init(configs);
#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION > 2))
					if (zend_parse_ini_file(&fh, 0, 0 /* ZEND_INI_SCANNER_NORMAL */,
						   	(zend_ini_parser_cb_t)yaf_config_ini_parser_cb, configs TSRMLS_CC) == FAILURE
							|| Z_TYPE_P(configs) != IS_ARRAY)
#else
					if (zend_parse_ini_file(&fh, 0, (zend_ini_parser_cb_t)yaf_config_ini_parser_cb, configs) == FAILURE
							|| Z_TYPE_P(configs) != IS_ARRAY)
#endif
					{
						zval_ptr_dtor(&configs);
						yaf_trigger_error(E_ERROR TSRMLS_CC, "Parsing ini file '%s' failed", ini_file);
						return NULL;
					}
				}
			} else {
				zval_ptr_dtor(&configs);
				yaf_trigger_error(E_ERROR TSRMLS_CC, "Argument is not a valid ini file '%s'", ini_file);
				return NULL;
			}
		} else {
			zval_ptr_dtor(&configs);
			yaf_trigger_error(E_ERROR TSRMLS_CC, "Unable to find config file '%s'", ini_file);
			return NULL;
		}

		if (section_name && Z_STRLEN_P(section_name)) {
			zval **section;
			zval tmp;
			if (zend_symtable_find(Z_ARRVAL_P(configs),
						Z_STRVAL_P(section_name), Z_STRLEN_P(section_name) + 1, (void **)&section) == FAILURE) {
				zval_ptr_dtor(&configs);
				yaf_trigger_error(E_ERROR TSRMLS_CC, "There is no section '%s' in '%s'", Z_STRVAL_P(section_name), ini_file);
				return NULL;
			}
			INIT_PZVAL(&tmp);
			array_init(&tmp);
			zend_hash_copy(Z_ARRVAL(tmp), Z_ARRVAL_PP(section), (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval *));
			zval_dtor(configs);

			*configs = tmp;
		} 

		if (this_ptr) {
			instance = this_ptr;
		} else {
			MAKE_STD_ZVAL(instance);
			object_init_ex(instance, yaf_config_ini_ce);
		}

		zend_update_property(yaf_config_ini_ce, instance, ZEND_STRL(YAF_CONFIG_PROPERT_NAME), configs TSRMLS_CC);
		zval_ptr_dtor(&configs);

		return instance;
	} else {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "Invalid parameters provided, must be path of ini file");
		return NULL;
	}
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::__construct(mixed $config_path, string $section_name)
*/
PHP_METHOD(yaf_config_ini, __construct) {
	zval *filename, *section = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &filename, &section) == FAILURE) {
		zval *prop;
		MAKE_STD_ZVAL(prop);
		array_init(prop);
		zend_update_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), prop TSRMLS_CC);
		zval_ptr_dtor(&prop);
		return;
	}

	(void)yaf_config_ini_instance(getThis(), filename, section TSRMLS_CC);
}
/** }}} */

/** {{{ proto public Yaf_Config_Ini::get(string $name = NULL)
*/
PHP_METHOD(yaf_config_ini, get) {
	zval *ret, **ppzval;
	char *name;
	uint len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &len) == FAILURE) {
		return;
	}

	if (!len) {
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		zval *properties;
		char *entry, *seg, *pptr;
		int seg_len;
	   	long lval;
		double dval;

		properties = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);

		if (Z_TYPE_P(properties) != IS_ARRAY) {
			RETURN_NULL();
		}

		entry = estrndup(name, len);
		if ((seg = php_strtok_r(entry, ".", &pptr))) {
			while (seg) {
				seg_len = strlen(seg);
				if (is_numeric_string(seg, seg_len, &lval, &dval, 0) != IS_LONG) {
					if (zend_hash_find(Z_ARRVAL_P(properties), seg, seg_len + 1, (void **) &ppzval) == FAILURE) {
						efree(entry);
						RETURN_NULL();
					}
				} else {
					if (zend_hash_index_find(Z_ARRVAL_P(properties), lval, (void **) &ppzval) == FAILURE) {
						efree(entry);
						RETURN_NULL();
					}
				}

				properties = *ppzval;
				seg = php_strtok_r(NULL, ".", &pptr);
			}
		} else if (is_numeric_string(name, len, &lval, &dval, 0) != IS_LONG) {
			if (zend_hash_find(Z_ARRVAL_P(properties), name, len + 1, (void **)&ppzval) == FAILURE) {
				efree(entry);
				RETURN_NULL();
			}
		} else {
			if (zend_hash_index_find(Z_ARRVAL_P(properties), lval, (void **) &ppzval) == FAILURE) {
				efree(entry);
				RETURN_NULL();
			}
		}

		efree(entry);

		if (Z_TYPE_PP(ppzval) == IS_ARRAY) {
			if ((ret = yaf_config_ini_format(getThis(), ppzval TSRMLS_CC))) {
				RETURN_ZVAL(ret, 1, 1);
			} else {
				RETURN_NULL();
			}
		} else {
			RETURN_ZVAL(*ppzval, 1, 0);
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::toArray(void)
*/
PHP_METHOD(yaf_config_ini, toArray) {
	zval *properties = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
	RETURN_ZVAL(properties, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::set($name, $value)
*/
PHP_METHOD(yaf_config_ini, set) {
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::__isset($name)
*/
PHP_METHOD(yaf_config_ini, __isset) {
	char * name;
	int len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len) == FAILURE) {
		return;
	} else {
		zval *prop = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
		RETURN_BOOL(zend_hash_exists(Z_ARRVAL_P(prop), name, len + 1));
	}
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::count($name)
*/
PHP_METHOD(yaf_config_ini, count) {
	zval *prop = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
	RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(prop)));
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::offsetUnset($index)
*/
PHP_METHOD(yaf_config_ini, offsetUnset) {
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::rewind(void)
*/
PHP_METHOD(yaf_config_ini, rewind) {
	zval *prop = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(prop));
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::current(void)
*/
PHP_METHOD(yaf_config_ini, current) {
	zval *prop, **ppzval, *ret;

	prop = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
	if (zend_hash_get_current_data(Z_ARRVAL_P(prop), (void **)&ppzval) == FAILURE) {
		RETURN_FALSE;
	}

	if (Z_TYPE_PP(ppzval) == IS_ARRAY) {
		if ((ret = yaf_config_ini_format(getThis(),  ppzval TSRMLS_CC))) {
			RETURN_ZVAL(ret, 1, 1);
		} else {
			RETURN_NULL();
		}
	} else {
		RETURN_ZVAL(*ppzval, 1, 0);
	}
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::key(void)
*/
PHP_METHOD(yaf_config_ini, key) {
	zval *prop;
	char *string;
	ulong index;

	prop = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 0 TSRMLS_CC);
	switch (zend_hash_get_current_key(Z_ARRVAL_P(prop), &string, &index, 0)) {
		case HASH_KEY_IS_LONG:
			RETURN_LONG(index);
			break;
		case HASH_KEY_IS_STRING:
			RETURN_STRING(string, 1);
			break;
		default:
			RETURN_FALSE;
	}
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::next(void)
*/
PHP_METHOD(yaf_config_ini, next) {
	zval *prop = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
	zend_hash_move_forward(Z_ARRVAL_P(prop));
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::valid(void)
*/
PHP_METHOD(yaf_config_ini, valid) {
	zval *prop = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
	RETURN_LONG(zend_hash_has_more_elements(Z_ARRVAL_P(prop)) == SUCCESS);
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::readonly(void)
*/
PHP_METHOD(yaf_config_ini, readonly) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::__destruct
*/
PHP_METHOD(yaf_config_ini, __destruct) {
}
/* }}} */

/** {{{ proto private Yaf_Config_Ini::__clone
*/
PHP_METHOD(yaf_config_ini, __clone) {
}
/* }}} */

/** {{{ yaf_config_ini_methods
*/
zend_function_entry yaf_config_ini_methods[] = {
	PHP_ME(yaf_config_ini, __construct,	yaf_config_ini_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	/* PHP_ME(yaf_config_ini, __destruct,	NULL, ZEND_ACC_PUBLIC|ZEND_ACC_DTOR) */
	PHP_ME(yaf_config_ini, __isset, yaf_config_ini_isset_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_ini, get,	yaf_config_ini_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_ini, set, yaf_config_ini_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_ini, count, yaf_config_ini_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_ini, rewind, yaf_config_ini_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_ini, current, yaf_config_ini_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_ini, next, yaf_config_ini_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_ini, valid, yaf_config_ini_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_ini, key, yaf_config_ini_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_ini, toArray, yaf_config_ini_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_ini, readonly, yaf_config_ini_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_ini, offsetUnset, yaf_config_ini_unset_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_config_ini, offsetGet, get, yaf_config_ini_rget_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_config_ini, offsetExists, __isset, yaf_config_ini_isset_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_config_ini, offsetSet, set, yaf_config_ini_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_config_ini, __get, get, yaf_config_ini_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_config_ini, __set, set, yaf_config_ini_set_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(config_ini) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Config_Ini", "Yaf\\Config\\Ini", yaf_config_ini_methods);
	yaf_config_ini_ce = zend_register_internal_class_ex(&ce, yaf_config_ce, NULL TSRMLS_CC);

#ifdef HAVE_SPL
	zend_class_implements(yaf_config_ini_ce TSRMLS_CC, 3, zend_ce_iterator, zend_ce_arrayaccess, spl_ce_Countable);
#else
	zend_class_implements(yaf_config_ini_ce TSRMLS_CC, 2, zend_ce_iterator, zend_ce_arrayaccess);
#endif

	yaf_config_ini_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;

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
