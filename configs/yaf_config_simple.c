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
#include "Zend/zend_interfaces.h"

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_config.h"

#include "configs/yaf_config_simple.h"

zend_class_entry *yaf_config_simple_ce;

#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
extern PHPAPI zend_class_entry *spl_ce_Countable;
#endif

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_config_simple_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_simple_construct_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, config_file)
	ZEND_ARG_INFO(0, section)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_simple_get_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_simple_rget_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_simple_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_simple_isset_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_simple_unset_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
/* }}} */

yaf_config_t *yaf_config_simple_instance(yaf_config_t *this_ptr, zval *values, zval *readonly) /* {{{ */ {
	switch (Z_TYPE_P(values)) {
		case IS_ARRAY:
			if (Z_ISUNDEF_P(this_ptr)) {
				object_init_ex(this_ptr, yaf_config_simple_ce);
			}
			zend_update_property(yaf_config_simple_ce, this_ptr, ZEND_STRL(YAF_CONFIG_PROPERT_NAME), values);
			if (readonly) {
				convert_to_boolean(readonly);
				zend_update_property_bool(yaf_config_simple_ce, this_ptr, ZEND_STRL(YAF_CONFIG_PROPERT_NAME_READONLY), 
						Z_TYPE_P(readonly) == IS_TRUE ? 1 : 0);
			}
			return this_ptr;
		default:
			yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Invalid parameters provided, must be an array");
			return NULL;
	}
}
/* }}} */

zval *yaf_config_simple_format(yaf_config_t *instance, zval *pzval, zval *rv) /* {{{ */ {
	zval *readonly, *ret;
	readonly = zend_read_property(yaf_config_simple_ce, instance, ZEND_STRL(YAF_CONFIG_PROPERT_NAME_READONLY), 1, NULL);
	ret = yaf_config_simple_instance(rv, pzval, readonly);
	return ret;
}
/* }}} */

/** {{{ proto public Yaf_Config_Simple::__construct(mixed $array, string $readonly)
*/
PHP_METHOD(yaf_config_simple, __construct) {
	zval *values, *readonly = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z", &values, &readonly) == FAILURE) {
		zval prop;

		array_init(&prop);
		zend_update_property(yaf_config_simple_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), &prop);
		zval_ptr_dtor(&prop);

		return;
	}

	(void)yaf_config_simple_instance(getThis(), values, readonly);
}
/** }}} */

/** {{{ proto public Yaf_Config_Simple::get(string $name = NULL)
*/
PHP_METHOD(yaf_config_simple, get) {
	zval *ret, *pzval;
	zend_string *name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &name) == FAILURE) {
		return;
	}

	if (!name) {
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		zval *properties;
		HashTable *hash;
		long lval;
		double dval;

		properties = zend_read_property(yaf_config_simple_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
		hash  = Z_ARRVAL_P(properties);

		if (is_numeric_string(ZSTR_VAL(name), ZSTR_LEN(name), &lval, &dval, 0) != IS_LONG) {
			if ((pzval = zend_hash_find(hash, name)) == NULL) {
				RETURN_FALSE;
			} 
		} else {
			if ((pzval = zend_hash_index_find(hash, lval)) == NULL) {
				RETURN_FALSE;
			} 
		}

		if (Z_TYPE_P(pzval) == IS_ARRAY) {
			zval rv = {{0}};
			if ((ret = yaf_config_simple_format(getThis(), pzval, &rv))) {
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

/** {{{ proto public Yaf_Config_Simple::toArray(void)
*/
PHP_METHOD(yaf_config_simple, toArray) {
	zval *properties = zend_read_property(yaf_config_simple_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
	RETURN_ZVAL(properties, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Config_Simple::set($name, $value)
*/
PHP_METHOD(yaf_config_simple, set) {
	zval *readonly = zend_read_property(yaf_config_simple_ce,
			getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME_READONLY), 1, NULL);

	if (Z_TYPE_P(readonly) == IS_FALSE) {
		zend_string *name;
		zval *value, *props;
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
			return;
		}

		props = zend_read_property(yaf_config_simple_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
		if (zend_hash_update(Z_ARRVAL_P(props), name, value) != NULL) {
			Z_TRY_ADDREF_P(value);
			RETURN_TRUE;
		} 
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Config_Simple::__isset($name)
*/
PHP_METHOD(yaf_config_simple, __isset) {
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	} else {
		zval *prop = zend_read_property(yaf_config_simple_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
		RETURN_BOOL(zend_hash_exists(Z_ARRVAL_P(prop), name));
	}
}
/* }}} */

/** {{{ proto public Yaf_Config_Simple::offsetUnset($index)
*/
PHP_METHOD(yaf_config_simple, offsetUnset) {
	zval *readonly = zend_read_property(yaf_config_simple_ce,
			getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME_READONLY), 1, NULL);

	if (Z_TYPE_P(readonly) == IS_FALSE) {
		zval *props;
		zend_string *name;
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
			return;
		}

		props = zend_read_property(yaf_config_simple_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
		if (UNEXPECTED(Z_TYPE_P(props) != IS_ARRAY)) {
			RETURN_FALSE;
		}
		if (zend_hash_del(Z_ARRVAL_P(props), name) == SUCCESS) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Config_Simple::count($name)
*/
PHP_METHOD(yaf_config_simple, count) {
	zval *prop = zend_read_property(yaf_config_simple_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
	RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(prop)));
}
/* }}} */

/** {{{ proto public Yaf_Config_Simple::rewind(void)
*/
PHP_METHOD(yaf_config_simple, rewind) {
	zval *prop = zend_read_property(yaf_config_simple_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(prop));
}
/* }}} */

/** {{{ proto public Yaf_Config_Simple::current(void)
*/
PHP_METHOD(yaf_config_simple, current) {
	zval *prop, *pzval, *ret;

	prop = zend_read_property(yaf_config_simple_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
	if ((pzval = zend_hash_get_current_data(Z_ARRVAL_P(prop))) == NULL) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(pzval) == IS_ARRAY) {
		zval rv = {{0}};
		if ((ret = yaf_config_simple_format(getThis(), pzval, &rv))) {
			RETURN_ZVAL(ret, 1, 1);
		} else {
			RETURN_NULL();
		}
	} else {
		RETURN_ZVAL(pzval, 1, 0);
	}
}
/* }}} */

/** {{{ proto public Yaf_Config_Simple::key(void)
*/
PHP_METHOD(yaf_config_simple, key) {
	zval *prop;
	zend_string *string;
	ulong index;

	prop = zend_read_property(yaf_config_simple_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
	zend_hash_get_current_key(Z_ARRVAL_P(prop), &string, &index);
	switch(zend_hash_get_current_key_type(Z_ARRVAL_P(prop))) {
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

/** {{{ proto public Yaf_Config_Simple::next(void)
*/
PHP_METHOD(yaf_config_simple, next) {
	zval *prop = zend_read_property(yaf_config_simple_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
	zend_hash_move_forward(Z_ARRVAL_P(prop));
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Config_Simple::valid(void)
*/
PHP_METHOD(yaf_config_simple, valid) {
	zval *prop = zend_read_property(yaf_config_simple_ce, getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1, NULL);
	RETURN_BOOL(zend_hash_has_more_elements(Z_ARRVAL_P(prop)) == SUCCESS);
}
/* }}} */

/** {{{ proto public Yaf_Config_Simple::readonly(void)
*/
PHP_METHOD(yaf_config_simple, readonly) {
	zval *readonly = zend_read_property(yaf_config_simple_ce,
			getThis(), ZEND_STRL(YAF_CONFIG_PROPERT_NAME_READONLY), 1, NULL);
	RETURN_BOOL(Z_TYPE_P(readonly) == IS_TRUE);
}
/* }}} */

/** {{{ proto public Yaf_Config_Simple::__destruct
*/
PHP_METHOD(yaf_config_simple, __destruct) {
}
/* }}} */

/** {{{ proto private Yaf_Config_Simple::__clone
*/
PHP_METHOD(yaf_config_simple, __clone) {
}
/* }}} */

/** {{{ yaf_config_simple_methods
*/
zend_function_entry yaf_config_simple_methods[] = {
	PHP_ME(yaf_config_simple, __construct, yaf_config_simple_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	/* PHP_ME(yaf_config_simple, __destruct,	NULL, ZEND_ACC_PUBLIC|ZEND_ACC_DTOR) */
	PHP_ME(yaf_config_simple, __isset, yaf_config_simple_isset_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_simple, get, yaf_config_simple_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_simple, set, yaf_config_simple_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_simple, count, yaf_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_simple, offsetUnset,	yaf_config_simple_unset_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_simple, rewind, yaf_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_simple, current, yaf_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_simple, next,	yaf_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_simple, valid, yaf_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_simple, key, yaf_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_simple, readonly,	yaf_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_simple, toArray, yaf_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_config_simple, __set, set, yaf_config_simple_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_config_simple, __get, get, yaf_config_simple_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_config_simple, offsetGet, get, yaf_config_simple_rget_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_config_simple, offsetExists, __isset, yaf_config_simple_isset_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_config_simple, offsetSet, set, yaf_config_simple_set_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(config_simple) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Config_Simple", "Yaf\\Config\\Simple", yaf_config_simple_methods);
	yaf_config_simple_ce = zend_register_internal_class_ex(&ce, yaf_config_ce);

#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
	zend_class_implements(yaf_config_simple_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, spl_ce_Countable);
#elif PHP_VERSION_ID >= 70200
	zend_class_implements(yaf_config_simple_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, zend_ce_countable);
#else
	zend_class_implements(yaf_config_simple_ce, 2, zend_ce_iterator, zend_ce_arrayaccess);
#endif
	zend_declare_property_bool(yaf_config_simple_ce, ZEND_STRL(YAF_CONFIG_PROPERT_NAME_READONLY), 0, ZEND_ACC_PROTECTED);

	yaf_config_simple_ce->ce_flags |= ZEND_ACC_FINAL;

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
