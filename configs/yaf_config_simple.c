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

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_config.h"

#include "configs/yaf_config_simple.h"

zend_class_entry *yaf_config_simple_ce;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_config_simple_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_simple_construct_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, config)
	ZEND_ARG_INFO(0, readonly)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_simple_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_simple_unset_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
/* }}} */

void yaf_config_simple_init(yaf_config_object *conf, zval *val, int readonly) /* {{{ */ {
	if (Z_TYPE_P(val) == IS_ARRAY) {
		if (readonly) {
			conf->config = Z_ARRVAL_P(val);
			if (!(GC_FLAGS(conf->config) & IS_ARRAY_IMMUTABLE)) {
				GC_ADDREF(conf->config);
			}
		} else {
			conf->config = zend_array_dup(Z_ARRVAL_P(val));
		}
		conf->flags = readonly? YAF_CONFIG_READONLY : 0;
		return;
	}
	yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Invalid parameters provided, must be an array");
}
/* }}} */

static int yaf_config_simple_update(yaf_config_object *conf, zend_string *name, zval *val) /* {{{ */ {
	if (UNEXPECTED(conf->config == NULL)) {
		return 0;
	}
	if (zend_hash_update(conf->config, name, val) != NULL) {
		Z_TRY_ADDREF_P(val);
		return 1;
	}
	return 0;
}
/* }}} */

/** {{{ proto public Yaf_Config_Simple::__construct(mixed $array, string $readonly = false)
*/
PHP_METHOD(yaf_config_simple, __construct) {
	zval *values;
	zend_bool readonly = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|b", &values, &readonly) == FAILURE) {
		return;
	}

	if (!readonly) {
		SEPARATE_ARRAY(values);
	}

	yaf_config_simple_init(Z_YAFCONFIGOBJ_P(getThis()), values, readonly);
}
/** }}} */

/** {{{ proto public Yaf_Config_Simple::set($name, $value)
*/
PHP_METHOD(yaf_config_simple, set) {
	zval *val;
	zend_string *name;
	yaf_config_object *conf = Z_YAFCONFIGOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &val) == FAILURE) {
		return;
	}

	if (conf->flags & YAF_CONFIG_READONLY) {
		RETURN_FALSE;
	} else {
		RETURN_BOOL(yaf_config_simple_update(conf, name, val));
	}
}
/* }}} */

/** {{{ proto public Yaf_Config_Simple::readonly(void)
*/
PHP_METHOD(yaf_config_simple, readonly) {
	yaf_config_object *conf = Z_YAFCONFIGOBJ_P(getThis());

	RETURN_BOOL(conf->flags & YAF_CONFIG_READONLY);
}
/* }}} */

/** {{{ proto public Yaf_Config_Simple::offsetUnset($offset)
*/
PHP_METHOD(yaf_config_simple, offsetUnset) {
	zval *offset;
	yaf_config_object *conf = Z_YAFCONFIGOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &offset) == FAILURE) {
		return;
	}

	if (conf->flags & YAF_CONFIG_READONLY) {
		php_error_docref(NULL, E_WARNING, "config is readonly");
	}

	if (conf->config) {
		if (Z_TYPE_P(offset) == IS_STRING) {
			zend_hash_del(conf->config, Z_STR_P(offset));
			RETURN_TRUE;
		} else if (Z_TYPE_P(offset) == IS_LONG) {
			zend_hash_index_del(conf->config, Z_LVAL_P(offset));
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/** {{{ yaf_config_simple_methods
*/
zend_function_entry yaf_config_simple_methods[] = {
	PHP_ME(yaf_config_simple,     __construct, yaf_config_simple_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(yaf_config_simple,     set, yaf_config_simple_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_simple,     readonly,	yaf_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config_simple,     offsetUnset, yaf_config_simple_unset_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_config_simple, __set, set, yaf_config_simple_set_arginfo, ZEND_ACC_PUBLIC)
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
