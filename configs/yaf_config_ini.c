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
#include "php_ini.h"
#include "Zend/zend_interfaces.h"

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_config.h"

#include "configs/yaf_config_ini.h"

zend_class_entry *yaf_config_ini_ce;

#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
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

/** {{{ static inline yaf_deep_copy_section(zval *dst, zval *src)
 */
static inline void yaf_deep_copy_section(zval *dst, zval *src) {
	zval *pzval, *dstpzval, value;
	HashTable *ht;
	ulong idx;
	zend_string *key;

	ht = Z_ARRVAL_P(src);
	ZEND_HASH_FOREACH_KEY_VAL(ht, idx, key, pzval) {

		if (key) {
			if (Z_TYPE_P(pzval) == IS_ARRAY 
					&& (dstpzval = zend_hash_find(Z_ARRVAL_P(dst), key)) != NULL
					&& Z_TYPE_P(dstpzval) == IS_ARRAY) {
				array_init(&value);
				yaf_deep_copy_section(&value, dstpzval);
				yaf_deep_copy_section(&value, pzval);
			} else {
				ZVAL_COPY_VALUE(&value, pzval);
				Z_ADDREF(value);
			}
			zend_hash_update(Z_ARRVAL_P(dst), key, &value);
		} else {
			if (Z_TYPE_P(pzval) == IS_ARRAY
					&& (dstpzval = zend_hash_index_find(Z_ARRVAL_P(dst), idx)) != NULL
					&& Z_TYPE_P(dstpzval) == IS_ARRAY) {
				array_init(&value);
				yaf_deep_copy_section(&value, dstpzval);
				yaf_deep_copy_section(&value, pzval);
			} else {
				ZVAL_COPY_VALUE(&value, pzval);
				Z_ADDREF(value);
			}
			zend_hash_index_update(Z_ARRVAL_P(dst), idx, &value);
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

zval *yaf_config_ini_format(yaf_config_t *instance, zval *pzval, zval *rv) /* {{{ */ {
	return yaf_config_ini_instance(rv, pzval, NULL);
}
/* }}} */

/** {{{ static void yaf_config_ini_simple_parser_cb(zval *key, zval *value, zval *index, int callback_type, zval *arr)
*/
static void yaf_config_ini_simple_parser_cb(zval *key, zval *value, zval *index, int callback_type, zval *arr) {
	zval element;
	switch (callback_type) {
		case ZEND_INI_PARSER_ENTRY:
			{
				char *skey, *seg, *ptr;
				zval *pzval, *dst;

				if (!value) {
					break;
				}

				dst = arr;
				skey = estrndup(Z_STRVAL_P(key), Z_STRLEN_P(key));
				if ((seg = php_strtok_r(skey, ".", &ptr))) {
					do {
						char *real_key = seg;
						seg = php_strtok_r(NULL, ".", &ptr);
						if ((pzval = zend_symtable_str_find(Z_ARRVAL_P(dst), real_key, strlen(real_key))) == NULL) {
							if (seg) {
								zval tmp;
								array_init(&tmp);
								pzval = zend_symtable_str_update(Z_ARRVAL_P(dst), 
										real_key, strlen(real_key), &tmp);
							} else {
								ZVAL_COPY(&element, value);
								zend_symtable_str_update(Z_ARRVAL_P(dst), 
										real_key, strlen(real_key), &element);
								break;
							}
						} else {
							SEPARATE_ZVAL(pzval);
							if (IS_ARRAY != Z_TYPE_P(pzval)) {
								if (seg) {
									zval tmp;
									array_init(&tmp);
									pzval = zend_symtable_str_update(Z_ARRVAL_P(dst), 
											real_key, strlen(real_key), &tmp);
								} else {
									ZVAL_DUP(&element, value);
									zend_symtable_str_update(Z_ARRVAL_P(dst), 
											real_key, strlen(real_key), &element);
								}
							} 
						}
						dst = pzval;
					} while (seg);
				}
				efree(skey);
			}
			break;

		case ZEND_INI_PARSER_POP_ENTRY:
			{
				zval hash, *find_hash, *dst;

				if (!value) {
					break;
				}

				if (!(Z_STRLEN_P(key) > 1 && Z_STRVAL_P(key)[0] == '0')
						&& is_numeric_string(Z_STRVAL_P(key), Z_STRLEN_P(key), NULL, NULL, 0) == IS_LONG) {
					zend_ulong skey = (zend_ulong)zend_atol(Z_STRVAL_P(key), Z_STRLEN_P(key));
					if ((find_hash = zend_hash_index_find(Z_ARRVAL_P(arr), skey)) == NULL) {
						array_init(&hash);
						find_hash = zend_hash_index_update(Z_ARRVAL_P(arr), skey, &hash);
					} 
				} else {
					char *seg, *ptr;
					char *skey = estrndup(Z_STRVAL_P(key), Z_STRLEN_P(key));

					dst = arr;
					if ((seg = php_strtok_r(skey, ".", &ptr))) {
						while (seg) {
							if ((find_hash = zend_symtable_str_find(Z_ARRVAL_P(dst), seg, strlen(seg))) == NULL) {
								array_init(&hash);
								find_hash = zend_symtable_str_update(Z_ARRVAL_P(dst), 
										seg, strlen(seg), &hash);
							}
							dst = find_hash;
							seg = php_strtok_r(NULL, ".", &ptr);
						}
					} else {
						if ((find_hash = zend_symtable_str_find(Z_ARRVAL_P(dst), seg, strlen(seg))) == NULL) {
							array_init(&hash);
							find_hash = zend_symtable_str_update(Z_ARRVAL_P(dst), seg, strlen(seg), &hash);
						} 
					}
					efree(skey);
				}

				if (Z_TYPE_P(find_hash) != IS_ARRAY) {
					zval_ptr_dtor(find_hash);
					array_init(find_hash);
				}

				ZVAL_DUP(&element, value);

				if (index && Z_STRLEN_P(index) > 0) {
					zend_symtable_update(Z_ARRVAL_P(find_hash), Z_STR_P(index), &element);
				} else {
					add_next_index_zval(find_hash, &element);
				}
			}
			break;

		case ZEND_INI_PARSER_SECTION:
			break;
	}
}
/* }}} */

/** {{{ static void yaf_config_ini_parser_cb(zval *key, zval *value, zval *index, int callback_type, zval *arr)
*/
static void yaf_config_ini_parser_cb(zval *key, zval *value, zval *index, int callback_type, zval *arr) {

	if (YAF_G(parsing_flag) == YAF_CONFIG_INI_PARSING_END) {
		return;
	}

	if (callback_type == ZEND_INI_PARSER_SECTION) {
		zval *parent;
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

		array_init(&YAF_G(active_ini_file_section));

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
					if ((parent = zend_symtable_str_find(Z_ARRVAL_P(arr), section, strlen(section))) != NULL) {
						yaf_deep_copy_section(&YAF_G(active_ini_file_section), parent);
					}
				} while ((section = strrchr(seg, ':')));
			}

			if ((parent = zend_symtable_str_find(Z_ARRVAL_P(arr), seg, strlen(seg))) != NULL) {
				yaf_deep_copy_section(&YAF_G(active_ini_file_section), parent);
			}
			skey_len = strlen(skey);
		}
		zend_symtable_str_update(Z_ARRVAL_P(arr), skey, skey_len, &YAF_G(active_ini_file_section));
		if (YAF_G(ini_wanted_section) && Z_STRLEN_P(YAF_G(ini_wanted_section)) == skey_len
				&& !strncasecmp(Z_STRVAL_P(YAF_G(ini_wanted_section)), skey, skey_len)) {
			YAF_G(parsing_flag) = YAF_CONFIG_INI_PARSING_PROCESS;
		}
		efree(skey_orig);
	} else if (value) {
		zval *active_arr;
		if (Z_TYPE(YAF_G(active_ini_file_section)) != IS_UNDEF) {
			active_arr = &YAF_G(active_ini_file_section);
		} else {
			active_arr = arr;
		}
		yaf_config_ini_simple_parser_cb(key, value, index, callback_type, active_arr);
	}
}
/* }}} */

yaf_config_t *yaf_config_ini_instance(yaf_config_t *this_ptr, zval *filename, zval *section_name) /* {{{ */ {
	if (filename && Z_TYPE_P(filename) == IS_ARRAY) {
		if (Z_ISUNDEF_P(this_ptr)) {
			object_init_ex(this_ptr, yaf_config_ini_ce);
		} 
		zend_update_property(yaf_config_ini_ce, this_ptr, ZEND_STRL(YAF_CONFIG_PROPERT_NAME), filename);
		return this_ptr;
	} else if (filename && Z_TYPE_P(filename) == IS_STRING) {
		zval configs;
		zend_stat_t sb;
		zend_file_handle fh;
		char *ini_file = Z_STRVAL_P(filename);
		
		if (VCWD_STAT(ini_file, &sb) == 0) {
			if (S_ISREG(sb.st_mode)) {
				if ((fh.handle.fp = VCWD_FOPEN(ini_file, "r"))) {
					fh.filename = ini_file;
					fh.type = ZEND_HANDLE_FP;
					fh.free_filename = 0;
					fh.opened_path = NULL;
					ZVAL_UNDEF(&YAF_G(active_ini_file_section));

					YAF_G(parsing_flag) = YAF_CONFIG_INI_PARSING_START;
					if (section_name && EXPECTED(Z_TYPE_P(section_name) == IS_STRING && Z_STRLEN_P(section_name))) {
						YAF_G(ini_wanted_section) = section_name;
					} else {
						YAF_G(ini_wanted_section) = NULL;
					}

	 				array_init(&configs);
					if (zend_parse_ini_file(&fh, 0, 0 /* ZEND_INI_SCANNER_NORMAL */,
						   	(zend_ini_parser_cb_t)yaf_config_ini_parser_cb, &configs) == FAILURE
							|| Z_TYPE(configs) != IS_ARRAY) {
						zval_ptr_dtor(&configs);
						yaf_trigger_error(E_ERROR, "Parsing ini file '%s' failed", ini_file);
						return NULL;
					}
				}
			} else {
				yaf_trigger_error(E_ERROR, "Argument is not a valid ini file '%s'", ini_file);
				return NULL;
			}
		} else {
			yaf_trigger_error(E_ERROR, "Unable to find config file '%s'", ini_file);
			return NULL;
		}

		if (section_name && EXPECTED(Z_TYPE_P(section_name) == IS_STRING && Z_STRLEN_P(section_name))) {
			zval *section, zv, garbage;
			if ((section = zend_symtable_find(Z_ARRVAL(configs), Z_STR_P(section_name))) == NULL) {
				zval_ptr_dtor(&configs);
				yaf_trigger_error(E_ERROR, "There is no section '%s' in '%s'", Z_STRVAL_P(section_name), ini_file);
				return NULL;
			}
			array_init(&zv);
			zend_hash_copy(Z_ARRVAL(zv), Z_ARRVAL_P(section), (copy_ctor_func_t) zval_add_ref);
			ZVAL_COPY_VALUE(&garbage, &configs);
			ZVAL_COPY_VALUE(&configs, &zv);
			zval_ptr_dtor(&garbage);
		} 

		if (Z_ISUNDEF_P(this_ptr)) {
			object_init_ex(this_ptr, yaf_config_ini_ce);
		}   

		zend_update_property(yaf_config_ini_ce, this_ptr, ZEND_STRL(YAF_CONFIG_PROPERT_NAME), &configs);
		zval_ptr_dtor(&configs);

		return this_ptr;
	} else {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Invalid parameters provided, must be path of ini file");
		return NULL;
	}
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::__construct(mixed $config_path, string $section_name)
*/
PHP_METHOD(yaf_config_ini, __construct) {
	zval *filename, *section = NULL;
	zval *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z", &filename, &section) == FAILURE) {
		if (self) {
			zval prop;
			array_init(&prop);
			zend_update_property(yaf_config_ini_ce, self, ZEND_STRL(YAF_CONFIG_PROPERT_NAME), &prop);
			zval_ptr_dtor(&prop);
		}
		return;
	}

	if (!self) {
		RETURN_FALSE;
	}

	yaf_config_ini_instance(self, filename, section);
}
/** }}} */

/** {{{ proto public Yaf_Config_Ini::get(string $name = NULL)
*/
PHP_METHOD(yaf_config_ini, get) {
	zval *ret, *pzval;
	zend_string *name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &name) == FAILURE) {
		return;
	}

	if (name == NULL) {
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		zval *properties;
		char *entry, *seg, *pptr;
		int seg_len;
	   	long lval;
		double dval;

		properties = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);

		if (Z_TYPE_P(properties) != IS_ARRAY) {
			RETURN_NULL();
		}

		if (strchr(ZSTR_VAL(name), '.')) {
			entry = estrndup(ZSTR_VAL(name), ZSTR_LEN(name));
			if ((seg = php_strtok_r(entry, ".", &pptr))) {
				while (seg) {
					seg_len = strlen(seg);
					if (is_numeric_string(seg, seg_len, &lval, &dval, 0) != IS_LONG) {
						if ((pzval = zend_hash_str_find(Z_ARRVAL_P(properties), seg, seg_len)) == NULL) {
							efree(entry);
							RETURN_NULL();
						}
					} else {
						if ((pzval = zend_hash_index_find(Z_ARRVAL_P(properties), lval)) == NULL) {
							efree(entry);
							RETURN_NULL();
						}
					}

					properties = pzval;
					seg = php_strtok_r(NULL, ".", &pptr);
				}
			}
		} else if (is_numeric_string(ZSTR_VAL(name), ZSTR_LEN(name), &lval, &dval, 0) != IS_LONG) {
			if ((pzval = zend_hash_find(Z_ARRVAL_P(properties), name)) == NULL) {
				RETURN_NULL();
			}
		} else {
			if ((pzval = zend_hash_index_find(Z_ARRVAL_P(properties), lval)) == NULL) {
				RETURN_NULL();
			}
		}

		if (Z_TYPE_P(pzval) == IS_ARRAY) {
			zval rv = {{0}};
			if ((ret = yaf_config_ini_format(getThis(), pzval, &rv))) {
				RETURN_ZVAL(ret, 1, 1);
			} else {
				RETURN_NULL();
			}
		} else {
			RETURN_ZVAL(pzval, 1, 0);
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::toArray(void)
*/
PHP_METHOD(yaf_config_ini, toArray) {
	zval *properties = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
	RETURN_ZVAL(properties, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::set($name, $value)
*/
PHP_METHOD(yaf_config_ini, set) {
	php_error_docref(NULL, E_WARNING, "Yaf_Config_Ini is readonly");
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::__isset($name)
*/
PHP_METHOD(yaf_config_ini, __isset) {
	zend_string* name;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	} else {
		zval *prop = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
		RETURN_BOOL(zend_hash_exists(Z_ARRVAL_P(prop), name));
	}
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::count($name)
*/
PHP_METHOD(yaf_config_ini, count) {
	zval *prop = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
	RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(prop)));
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::offsetUnset($index)
*/
PHP_METHOD(yaf_config_ini, offsetUnset) {
	php_error_docref(NULL, E_WARNING, "Yaf_Config_Ini is readonly");
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::rewind(void)
*/
PHP_METHOD(yaf_config_ini, rewind) {
	zval *prop = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(prop));
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::current(void)
*/
PHP_METHOD(yaf_config_ini, current) {
	zval *prop, *pzval, *ret;

	prop = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
	if ((pzval = zend_hash_get_current_data(Z_ARRVAL_P(prop))) == NULL) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(pzval) == IS_ARRAY) {
		zval rv = {{0}};
		if ((ret = yaf_config_ini_format(getThis(), pzval, &rv))) {
			RETURN_ZVAL(ret, 1, 1);
		} else {
			RETURN_NULL();
		}
	} else {
		RETURN_ZVAL(pzval, 1, 0);
	}
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::key(void)
*/
PHP_METHOD(yaf_config_ini, key) {
	zval *prop;
	zend_string *string;
	ulong index;

	prop = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 0, NULL);
	switch (zend_hash_get_current_key(Z_ARRVAL_P(prop), &string, &index)) {
		case HASH_KEY_IS_LONG:
			RETURN_LONG(index);
			break;
		case HASH_KEY_IS_STRING:
			RETURN_STR(zend_string_copy(string));
			break;
		default:
			RETURN_FALSE;
	}
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::next(void)
*/
PHP_METHOD(yaf_config_ini, next) {
	zval *prop = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
	zend_hash_move_forward(Z_ARRVAL_P(prop));
}
/* }}} */

/** {{{ proto public Yaf_Config_Ini::valid(void)
*/
PHP_METHOD(yaf_config_ini, valid) {
	zval *prop = zend_read_property(yaf_config_ini_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
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
	yaf_config_ini_ce = zend_register_internal_class_ex(&ce, yaf_config_ce);

#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
	zend_class_implements(yaf_config_ini_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, spl_ce_Countable);
#elif PHP_VERSION_ID >= 70200
	zend_class_implements(yaf_config_ini_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, zend_ce_countable);
#else
	zend_class_implements(yaf_config_ini_ce, 2, zend_ce_iterator, zend_ce_arrayaccess);
#endif

	yaf_config_ini_ce->ce_flags |= ZEND_ACC_FINAL;

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
