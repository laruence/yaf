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
#include "standard/php_filestat.h" /* for php_stat */

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_config.h"

#include "configs/yaf_config_ini.h"
#include "configs/yaf_config_simple.h"

zend_class_entry *yaf_config_ce;

static void yaf_config_ini_zval_persistent(zval *zvalue, zval *ret);
static void yaf_config_ini_zval_losable(zval *zvalue, zval *ret);

/* {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_config_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ yaf_config_ini_modified
*/
static int yaf_config_ini_modified(zval * file, long ctime) {
	zval  n_ctime;
	php_stat(Z_STRVAL_P(file), Z_STRLEN_P(file), 7 /* FS_CTIME */ , &n_ctime);
	if ((Z_TYPE(n_ctime) != IS_TRUE || Z_TYPE(n_ctime) != IS_FALSE) && ctime != Z_LVAL(n_ctime)) {
		return Z_LVAL(n_ctime);
	}
	return 0;
}
/* }}} */

/** {{{ static void yaf_config_cache_dtor(yaf_config_cache **cache)
 */
static void yaf_config_cache_dtor(yaf_config_cache **cache) {
	if (*cache) {
		zend_hash_destroy((*cache)->data);
		pefree((*cache)->data, 1);
		pefree(*cache, 1);
	}
}
/* }}} */

/** {{{ static void yaf_config_zval_dtor(zval *value)
 */
static void yaf_config_zval_dtor(zval *value) {
	if (value) {
		switch(Z_TYPE_P(value)) {
			case IS_STRING:
			case IS_CONSTANT:
				CHECK_ZVAL_STRING(Z_STR_P(value));
				zend_string_release(Z_STR_P(value));
				pefree(value, 1);
				break;
#ifdef IS_CONSTANT_ARRAY
			case IS_CONSTANT_ARRAY:
#endif
			case IS_ARRAY: {
				zend_hash_destroy(Z_ARRVAL_P(value));
				pefree(Z_ARR_P(value), 1);
				pefree(value, 1);
			}
			break;
		}
	}
}
/* }}} */

/** {{{ static void yaf_config_copy_persistent(HashTable *pdst, HashTable *src)
 */
static void yaf_config_copy_persistent(HashTable *pdst, HashTable *src) {
	zval *pzval;
	zend_string *key;
	ulong idx;
    zval tmp;

    ZEND_HASH_FOREACH_KEY_VAL(src, idx, key, pzval) {

        if (key) {
			yaf_config_ini_zval_persistent(pzval, &tmp);
			if (&tmp) {
				zend_hash_update(pdst, key, &tmp);
			}
        } else {
			yaf_config_ini_zval_persistent(pzval, &tmp);
			if (&tmp) {
				zend_hash_index_update(pdst, idx, &tmp);
			}
        }
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/** {{{ static void yaf_config_copy_losable(HashTable *ldst, HashTable *src)
 */
static void yaf_config_copy_losable(HashTable *ldst, HashTable *src) {
	zval *pzval, tmp;
	zend_string *key;
	ulong idx;

    ZEND_HASH_FOREACH_KEY_VAL(src, idx, key, pzval) {

        if (key) {
			yaf_config_ini_zval_losable(pzval, &tmp);
            zend_hash_update(ldst, key, &tmp);
        } else {
			yaf_config_ini_zval_losable(pzval, &tmp);
            zend_hash_index_update(ldst, idx, &tmp);
        }
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/** {{{ static void yaf_config_ini_zval_persistent(zval *zvalue, zval *ret)
 */
static void yaf_config_ini_zval_persistent(zval *zvalue, zval *ret) {
	switch (Z_TYPE_P(zvalue)) {
		case IS_RESOURCE:
		case IS_OBJECT:
			break;
		case IS_TRUE:
		case IS_FALSE:
		case IS_LONG:
		case IS_NULL:
			break;
		case IS_CONSTANT:
		case IS_STRING:
				CHECK_ZVAL_STRING(Z_STR_P(zvalue));
				convert_to_string(zvalue);
				ZVAL_STR(ret, zend_string_dup(Z_STR_P(zvalue), 1));
			break;
#ifdef IS_CONSTANT_ARRAY
		case IS_CONSTANT_ARRAY:
#endif
		case IS_ARRAY: {
				HashTable *original_ht = Z_ARRVAL_P(zvalue);

                ZVAL_NEW_PERSISTENT_ARR(ret);

				zend_hash_init(Z_ARRVAL_P(ret), zend_hash_num_elements(original_ht), NULL, (dtor_func_t)yaf_config_zval_dtor, 1);
				yaf_config_copy_persistent(Z_ARRVAL_P(ret), original_ht);
			}
			break;
	}
}
/* }}} */

/** {{{ static void yaf_config_ini_zval_losable(zval *zvalue, zval *ret)
 */
static void yaf_config_ini_zval_losable(zval *zvalue, zval *ret) {
	switch (Z_TYPE_P(zvalue)) {
		case IS_RESOURCE:
		case IS_OBJECT:
			break;
		case IS_TRUE:
		case IS_FALSE:
		case IS_LONG:
		case IS_NULL:
			break;
		case IS_CONSTANT:
		case IS_STRING:
			CHECK_ZVAL_STRING(Z_STR_P(zvalue));
			ZVAL_NEW_STR(ret, zend_string_dup(Z_STR_P(zvalue), 0));
			break;
#ifdef IS_CONSTANT_ARRAY
		case IS_CONSTANT_ARRAY:
#endif
		case IS_ARRAY: {
			HashTable *original_ht = Z_ARRVAL_P(zvalue);
			array_init(ret);
			yaf_config_copy_losable(Z_ARRVAL_P(ret), original_ht);
		}
			break;
	}
}
/* }}} */

/** {{{ static yaf_config_t * yaf_config_ini_unserialize(yaf_config_t *this_ptr, zval *filename, zval *section)
 */
static yaf_config_t * yaf_config_ini_unserialize(yaf_config_t *this_ptr, zval *filename, zval *section) {
	char *key;
	uint len;
	yaf_config_cache *pval;

	if (!YAF_G(configs)) {
		return NULL;
	}

	len = spprintf(&key, 0, "%s#%s", Z_STRVAL_P(filename), Z_STRVAL_P(section));

	if ((pval = zend_hash_str_find_ptr(YAF_G(configs), key, len)) != NULL) {
		if (yaf_config_ini_modified(filename, pval->ctime)) {
			efree(key);
			return NULL;
		} else {
			zval props;

			array_init(&props);
			yaf_config_copy_losable(Z_ARRVAL(props), pval->data);
			efree(key);
			/* tricky way */
			Z_SET_REFCOUNT(props, 0);
			return yaf_config_ini_instance(this_ptr, &props, section);
		}
		efree(key);
	}

	return NULL;
}
/* }}} */

/** {{{ static void yaf_config_ini_serialize(yaf_config_t *this_ptr, zval *filename, zval *section)
 */
static void yaf_config_ini_serialize(yaf_config_t *this_ptr, zval *filename, zval *section) {
	long ctime;
	zval *configs;
	zend_string *key;
	HashTable *persistent;
	yaf_config_cache *cache;

	if (!YAF_G(configs)) {
		YAF_G(configs) = (HashTable *)pemalloc(sizeof(HashTable), 1);
		if (!YAF_G(configs)) {
			return;
		}
		zend_hash_init(YAF_G(configs), 8, NULL, (dtor_func_t) yaf_config_cache_dtor, 1);
	}

	cache = (yaf_config_cache *)pemalloc(sizeof(yaf_config_cache), 1);

	if (!cache) {
		return;
	}

	persistent = (HashTable *)pemalloc(sizeof(HashTable), 1);
	if (!persistent) {
		return;
	}

	configs = zend_read_property(yaf_config_ini_ce, this_ptr, ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);

	zend_hash_init(persistent, zend_hash_num_elements(Z_ARRVAL_P(configs)), NULL, (dtor_func_t) yaf_config_zval_dtor, 1);

	yaf_config_copy_persistent(persistent, Z_ARRVAL_P(configs));

	ctime = yaf_config_ini_modified(filename, 0);
	cache->ctime = ctime;
	cache->data  = persistent;
	key = strpprintf(0, "%s#%s", Z_STRVAL_P(filename), Z_STRVAL_P(section));

	zend_hash_update_ptr(YAF_G(configs), key, cache);

	zend_string_release(key);
}
/* }}} */

yaf_config_t *yaf_config_instance(yaf_config_t *this_ptr, zval *arg1, zval *arg2) /* {{{ */ {
	yaf_config_t *instance;

	if (!arg1) {
		return NULL;
	}

	if (Z_TYPE_P(arg1) == IS_STRING) {
		if (strncasecmp(Z_STRVAL_P(arg1) + Z_STRLEN_P(arg1) - 3, "ini", 3) == 0) {
			if (YAF_G(cache_config)) {
				if ((instance = yaf_config_ini_unserialize(this_ptr, arg1, arg2))) {
					return instance;
				}
			}

			instance = yaf_config_ini_instance(this_ptr, arg1, arg2);

			if (!instance) {
				return NULL;
			}

			if (YAF_G(cache_config)) {
				yaf_config_ini_serialize(instance, arg1, arg2);
			}

			return instance;
		}
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Expects a path to *.ini configuration file as parameter");
		return NULL;
	}

	if (Z_TYPE_P(arg1) == IS_ARRAY) {
		zval readonly;
		ZVAL_BOOL(&readonly, 1);
		instance = yaf_config_simple_instance(this_ptr, arg1, &readonly);
		return instance;
	}

	yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Expects a string or an array as parameter");
	return NULL;
}
/* }}} */

/** {{{ yaf_config_methods
*/
zend_function_entry yaf_config_methods[] = {
	PHP_ABSTRACT_ME(yaf_config, get, NULL)
	PHP_ABSTRACT_ME(yaf_config, set, NULL)
	PHP_ABSTRACT_ME(yaf_config, readonly, NULL)
	PHP_ABSTRACT_ME(yaf_config, toArray, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(config) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Config_Abstract", "Yaf\\Config_Abstract", yaf_config_methods);
	yaf_config_ce = zend_register_internal_class_ex(&ce, NULL);
	yaf_config_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	zend_declare_property_null(yaf_config_ce, ZEND_STRL(YAF_CONFIG_PROPERT_NAME), ZEND_ACC_PROTECTED);
	zend_declare_property_bool(yaf_config_ce, ZEND_STRL(YAF_CONFIG_PROPERT_NAME_READONLY), 1, ZEND_ACC_PROTECTED);

	YAF_STARTUP(config_ini);
	YAF_STARTUP(config_simple);

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
