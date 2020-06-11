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
#include "Zend/zend_interfaces.h" /* for zend_class_serialize_deny */

#include "yaf_namespace.h"
#include "yaf_registry.h"

zend_class_entry    *yaf_registry_ce;
static zend_object_handlers yaf_registry_obj_handlers;

/* {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_registry_get_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_registry_has_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_registry_del_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_registry_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
/* }}} */

static HashTable *yaf_registry_get_properties(zval *object) /* {{{ */ {
	zval rv;
	HashTable *ht;
	yaf_registry_object *registry = Z_YAFREGISTRYOBJ_P(object);

	if (!registry->properties) {
		ALLOC_HASHTABLE(registry->properties);
		zend_hash_init(registry->properties, 8, NULL, ZVAL_PTR_DTOR, 0);
		YAF_ALLOW_VIOLATION(registry->properties);
	}

	ht = registry->properties;

	ZVAL_ARR(&rv, zend_array_dup(&registry->entries));
	zend_hash_str_update(ht, "entries:protected", sizeof("entries:protected") - 1, &rv);

	return ht;
}
/* }}} */

static void yaf_registry_object_free(zend_object *object) /* {{{ */ {
	yaf_registry_object *registry = php_yaf_registry_fetch_object(object);

	ZEND_ASSERT(Z_OBJ(YAF_G(registry)) == object);

	zend_hash_destroy(&registry->entries);
	if (registry->properties) {
		if (GC_DELREF(registry->properties) == 0) {
			GC_REMOVE_FROM_BUFFER(registry->properties);
			zend_array_destroy(registry->properties);
		}
	}

	zend_object_std_dtor(object);
}
/* }}} */

yaf_registry_object *yaf_registry_instance() /* {{{ */ {
	yaf_registry_object *registry;

	if (EXPECTED(Z_TYPE(YAF_G(registry)) == IS_OBJECT)) {
		return Z_YAFREGISTRYOBJ(YAF_G(registry));
	}

	registry = emalloc(sizeof(yaf_registry_object) + zend_object_properties_size(yaf_registry_ce));

	zend_object_std_init(&registry->std, yaf_registry_ce);
	registry->std.handlers = &yaf_registry_obj_handlers;

	zend_hash_init(&registry->entries, 8, NULL, ZVAL_PTR_DTOR, 0);
	registry->properties = NULL;

	ZVAL_OBJ(&YAF_G(registry), &registry->std);
	return Z_YAFREGISTRYOBJ(YAF_G(registry));
}
/* }}} */

static zval *yaf_registry_find(yaf_registry_object *registry, zend_string *name) /* {{{ */ {
	return zend_hash_find(&registry->entries, name);
}
/* }}} */

static int yaf_registry_has(yaf_registry_object *registry, zend_string *name) /* {{{ */ {
	return zend_hash_exists(&registry->entries, name);
}
/* }}} */

static zval *yaf_registry_update(yaf_registry_object *registry, zend_string *name, zval *value) /* {{{ */ {
	Z_TRY_ADDREF_P(value);
	return zend_hash_update(&registry->entries, name, value);
}
/* }}} */

static void yaf_registry_del(yaf_registry_object *registry, zend_string *name) /* {{{ */ {
	zend_hash_del(&registry->entries, name);
}
/* }}} */

/** {{{ proto private Yaf_Registry::__construct(void)
*/
PHP_METHOD(yaf_registry, __construct) {
}
/* }}} */

/** {{{ proto public static Yaf_Registry::get($name)
*/
PHP_METHOD(yaf_registry, get) {
	zval *value;
	zend_string *name;
	yaf_registry_object *registry = yaf_registry_instance();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	if ((value = yaf_registry_find(registry, name))) {
		RETURN_ZVAL(value, 1, 0);
	}
	
	RETURN_NULL();
}
/* }}} */

/** {{{ proto public static Yaf_Registry::set($name, $value)
*/
PHP_METHOD(yaf_registry, set) {
	zval *value;
	zend_string *name;
	yaf_registry_object *registry = yaf_registry_instance();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
		return;
	}

	if (yaf_registry_update(registry, name, value)) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public static Yaf_Registry::del($name)
*/
PHP_METHOD(yaf_registry, del) {
	zend_string *name;
	yaf_registry_object *registry = yaf_registry_instance();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	yaf_registry_del(registry, name);

	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Registry::has($name)
*/
PHP_METHOD(yaf_registry, has) {
	zend_string *name;
	yaf_registry_object *registry = yaf_registry_instance();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	RETURN_BOOL(yaf_registry_has(registry, name));
}
/* }}} */

/** {{{ proto public Yaf_Registry::getInstance(void)
*/
PHP_METHOD(yaf_registry, getInstance) {
	ZVAL_OBJ(return_value, &(yaf_registry_instance())->std);
	Z_ADDREF_P(return_value);
	return;
}
/* }}} */

/** {{{ yaf_registry_methods
*/
zend_function_entry yaf_registry_methods[] = {
	PHP_ME(yaf_registry, __construct, 	NULL, ZEND_ACC_CTOR|ZEND_ACC_PRIVATE)
	PHP_ME(yaf_registry, get, yaf_registry_get_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(yaf_registry, has, yaf_registry_has_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(yaf_registry, set, yaf_registry_set_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(yaf_registry, del, yaf_registry_del_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(registry) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Registry", "Yaf\\Registry", yaf_registry_methods);

	yaf_registry_ce = zend_register_internal_class_ex(&ce, NULL);
	yaf_registry_ce->ce_flags |= ZEND_ACC_FINAL;
	yaf_registry_ce->serialize = zend_class_serialize_deny;
	yaf_registry_ce->unserialize = zend_class_unserialize_deny;

	memcpy(&yaf_registry_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_registry_obj_handlers.offset = XtOffsetOf(yaf_registry_object, std);
	yaf_registry_obj_handlers.clone_obj = NULL;
	yaf_registry_obj_handlers.get_gc = NULL;
	yaf_registry_obj_handlers.free_obj = yaf_registry_object_free;
	yaf_registry_obj_handlers.get_properties = yaf_registry_get_properties;

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
