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
#include "Zend/zend_interfaces.h" /* for zend_class_serialize_deny */

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_config.h"

#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
extern PHPAPI zend_class_entry *spl_ce_Countable;
#endif

#include "configs/yaf_config_ini.h"
#include "configs/yaf_config_simple.h"

zend_class_entry *yaf_config_ce;
static zend_object_handlers yaf_config_obj_handlers;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_config_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_get_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_unset_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_config_isset_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
/* }}} */

static HashTable *yaf_config_get_gc(zval *object, zval **table, int *n) /* {{{ */ {
	yaf_config_object *conf = Z_YAFCONFIGOBJ_P(object);;

	*table = NULL;
	*n = 0;

	return conf->config;
}
/* }}} */

static HashTable *yaf_config_get_properties(zval *object) /* {{{ */ {
	zval rv;
	HashTable *ht;
	yaf_config_object *conf = Z_YAFCONFIGOBJ_P(object);;

	if (!conf->properties) {
		ALLOC_HASHTABLE(conf->properties);
		zend_hash_init(conf->properties, 4, NULL, ZVAL_PTR_DTOR, 0);
		YAF_ALLOW_VIOLATION(conf->properties);
	}

	ht = conf->properties;
	ZVAL_BOOL(&rv, conf->flags & YAF_CONFIG_READONLY);
	zend_hash_str_update(ht, "readonly:protected", sizeof("readonly:protected") - 1, &rv);

	if (conf->config) {
		ZVAL_ARR(&rv, zend_array_dup(conf->config));
	}
	zend_hash_str_update(ht, "config:protected", sizeof("config:protected") - 1, &rv);

	if (conf->filename) {
		ZVAL_STR_COPY(&rv, conf->filename);
		zend_hash_str_update(ht, "filename:protected", sizeof("filename:protected") - 1, &rv);
	}

	return ht;
}
/* }}} */

static zend_object *yaf_config_new(zend_class_entry *ce) /* {{{ */ {
	yaf_config_object *conf = emalloc(sizeof(yaf_config_object) + zend_object_properties_size(ce));

	memset(conf, 0, XtOffsetOf(yaf_config_object, std));
	zend_object_std_init(&conf->std, ce);
	conf->std.handlers = &yaf_config_obj_handlers;
	
	return &conf->std;
}
/* }}} */

static void yaf_config_object_free(zend_object *object) /* {{{ */ {
	yaf_config_object *conf = php_yaf_config_fetch_object(object);
	if (conf->config) {
		if (!(GC_FLAGS(conf->config) & IS_ARRAY_IMMUTABLE) && (GC_DELREF(conf->config) == 0)) {
			GC_REMOVE_FROM_BUFFER(conf->config);
			zend_array_destroy(conf->config);
		}
	}
	if (conf->filename) {
		zend_string_release(conf->filename);
	}
	if (conf->properties) {
		if (GC_DELREF(conf->properties) == 0) {
			GC_REMOVE_FROM_BUFFER(conf->properties);
			zend_array_destroy(conf->properties);
		}
	}
	zend_object_std_dtor(object);
}
/* }}} */

zend_object *yaf_config_format_child(zend_class_entry *ce, zval *child, int readonly) /* {{{ */ {
	zend_object *obj = yaf_config_new(ce);
	yaf_config_object *conf = php_yaf_config_fetch_object(obj);

	if (ce == yaf_config_ini_ce) {
		yaf_config_ini_init(conf, child, NULL);
	} else {
		yaf_config_simple_init(conf, child, readonly);
	}

	return obj;
}
/* }}} */

zval *yaf_config_get(yaf_config_object *conf, zend_string *name) /* {{{ */ {
	if (UNEXPECTED(conf->config == NULL)) {
		return NULL;
	}
	return zend_hash_find(conf->config, name);
}
/* }}} */

static int yaf_config_count(yaf_config_object *conf) /* {{{ */ {
	if (UNEXPECTED(conf->config == NULL)) {
		return 0;
	}
	return zend_hash_num_elements(conf->config);
}
/* }}} */

static int yaf_config_has(yaf_config_object *conf, zend_string *name) /* {{{ */ {
	if (UNEXPECTED(conf->config == NULL)) {
		return 0;
	}
	return zend_hash_exists(conf->config, name);
}
/* }}} */

void yaf_config_instance(yaf_config_t *this_ptr, zval *config, zend_string *section) /* {{{ */ {
	zend_object *conf;

	if (Z_TYPE_P(config) == IS_STRING) {
		if (strncasecmp(Z_STRVAL_P(config) + Z_STRLEN_P(config) - 4, ".ini", 4) == 0) {
			conf = yaf_config_new(yaf_config_ini_ce);
			ZVAL_OBJ(this_ptr, conf);
			if (UNEXPECTED(!yaf_config_ini_init(php_yaf_config_fetch_object(conf), config, section))) {
				zval_ptr_dtor(this_ptr);
				ZVAL_UNDEF(this_ptr);
			}
			return;
		}
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Expects a path to *.ini configuration file as parameter");
	} else if (Z_TYPE_P(config) == IS_ARRAY) {
		conf = yaf_config_new(yaf_config_simple_ce);
		ZVAL_OBJ(this_ptr, conf);
		yaf_config_simple_init(php_yaf_config_fetch_object(conf), config, 1);
		return;
	} else {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Expects a string or an array as parameter");
	}
	ZVAL_NULL(this_ptr);
}
/* }}} */

/** {{{ proto public Yaf_Config::get(string $name = NULL)
*/
PHP_METHOD(yaf_config, get) {
	zend_string *name = NULL;
	yaf_config_object *conf = Z_YAFCONFIGOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S!", &name) == FAILURE) {
		return;
	}

	if (name == NULL) {
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		zval *val = yaf_config_get(conf, name);
		if (val == NULL) {
			RETURN_NULL();
		}
		if (Z_TYPE_P(val) == IS_ARRAY) {
			RETURN_OBJ(yaf_config_format_child(Z_OBJCE_P(getThis()), val, conf->flags & YAF_CONFIG_READONLY));
		} else {
			RETURN_ZVAL(val, 1, 0);
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Config::toArray(void)
*/
PHP_METHOD(yaf_config, toArray) {
	yaf_config_object *conf = Z_YAFCONFIGOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (UNEXPECTED(conf->config == NULL)) {
		RETURN_NULL();
	}

	RETVAL_ARR(conf->config);
	Z_ADDREF_P(return_value);
}
/* }}} */

/** {{{ proto public Yaf_Config::__isset($name)
*/
PHP_METHOD(yaf_config, __isset) {
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	RETURN_BOOL(yaf_config_has(Z_YAFCONFIGOBJ_P(getThis()), name));
}
/* }}} */

/** {{{ proto public Yaf_Config::count(void)
*/
PHP_METHOD(yaf_config, count) {
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(yaf_config_count(Z_YAFCONFIGOBJ_P(getThis())));
}
/* }}} */

/** {{{ proto public Yaf_Config::offsetUnset($offset)
*/
PHP_METHOD(yaf_config, offsetUnset) {
	zval *offset;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &offset) == FAILURE) {
		return;
	}
	php_error_docref(NULL, E_WARNING, "config is readonly");

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Config::rewind(void)
*/
PHP_METHOD(yaf_config, rewind) {
	yaf_config_object *conf = Z_YAFCONFIGOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (conf->config) {
		zend_hash_internal_pointer_reset(conf->config);
	}
}
/* }}} */

/** {{{ proto public Yaf_Config::current(void)
*/
PHP_METHOD(yaf_config, current) {
	yaf_config_object *conf = Z_YAFCONFIGOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (conf->config) {
		zval *val = zend_hash_get_current_data(conf->config);
		if (val == NULL) {
			RETURN_FALSE;
		}
		if (Z_TYPE_P(val) == IS_ARRAY) {
			RETURN_OBJ(yaf_config_format_child(Z_OBJCE_P(getThis()), val, conf->flags & YAF_CONFIG_READONLY));
		} else {
			RETURN_ZVAL(val, 1, 0);
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Config::key(void)
*/
PHP_METHOD(yaf_config, key) {
	zend_ulong index;
	zend_string *string;
	yaf_config_object *conf = Z_YAFCONFIGOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (conf->config) {
		switch (zend_hash_get_current_key(conf->config, &string, &index)) {
			case HASH_KEY_IS_LONG:
				RETURN_LONG(index);
				break;
			case HASH_KEY_IS_STRING:
				RETURN_STR_COPY(string);
				break;
			default:
				break;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Config::next(void)
*/
PHP_METHOD(yaf_config, next) {
	yaf_config_object *conf = Z_YAFCONFIGOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (conf->config) {
		zend_hash_move_forward(conf->config);
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Config::valid(void)
*/
PHP_METHOD(yaf_config, valid) {
	yaf_config_object *conf = Z_YAFCONFIGOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (conf->config) {
		RETURN_BOOL(zend_hash_has_more_elements(conf->config) == SUCCESS);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ yaf_config_methods
*/
zend_function_entry yaf_config_methods[] = {
	PHP_ME(yaf_config, get, yaf_config_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config, count, yaf_config_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config, toArray, yaf_config_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config, offsetUnset, yaf_config_unset_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config, rewind, yaf_config_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config, current, yaf_config_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config, key, yaf_config_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config, next, yaf_config_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config, valid, yaf_config_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_config, __isset, yaf_config_isset_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_config, __get, get, yaf_config_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_config, offsetGet, get, yaf_config_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_config, offsetExists, __isset, yaf_config_isset_arginfo, ZEND_ACC_PUBLIC)
	PHP_ABSTRACT_ME(yaf_config, offsetSet, yaf_config_set_arginfo)
	PHP_ABSTRACT_ME(yaf_config, set, yaf_config_set_arginfo)
	PHP_ABSTRACT_ME(yaf_config, readonly, yaf_config_void_arginfo)
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
	yaf_config_ce->create_object = yaf_config_new;
	yaf_config_ce->serialize = zend_class_serialize_deny;
	yaf_config_ce->unserialize = zend_class_unserialize_deny;
	
	memcpy(&yaf_config_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_config_obj_handlers.offset = XtOffsetOf(yaf_config_object, std);
	yaf_config_obj_handlers.free_obj = yaf_config_object_free;
	yaf_config_obj_handlers.clone_obj = NULL;
	yaf_config_obj_handlers.get_gc = yaf_config_get_gc;
	yaf_config_obj_handlers.get_properties = yaf_config_get_properties;

#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
	zend_class_implements(yaf_config_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, spl_ce_Countable);
#elif PHP_VERSION_ID >= 70200
	zend_class_implements(yaf_config_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, zend_ce_countable);
#else
	zend_class_implements(yaf_config_ce, 2, zend_ce_iterator, zend_ce_arrayaccess);
#endif

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
