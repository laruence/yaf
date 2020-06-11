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
#include "Zend/zend_interfaces.h"  /* for zend_ce_iterator, zend_ce_countable*/

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_session.h"
#include "yaf_exception.h"

zend_class_entry *yaf_session_ce;
static zend_object_handlers yaf_session_obj_handlers;

#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
extern PHPAPI zend_class_entry *spl_ce_Countable;
#endif

/* {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_session_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_session_get_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_session_has_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_session_del_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_session_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
/* }}} */

static inline void yaf_session_start(yaf_session_object *session) /* {{{ */ {
	if (session->flags & YAF_SESSION_STARTED) {
		return;
	}
	php_session_start();
	session->flags |= YAF_SESSION_STARTED;
}
/* }}} */

static HashTable *yaf_session_get_properties(zval *object) /* {{{ */ {
	zval rv;
	HashTable *ht;
	yaf_session_object *sess = Z_YAFSESSIONOBJ_P(object);

	if (!sess->properties) {
		ALLOC_HASHTABLE(sess->properties);
		zend_hash_init(sess->properties, 2, NULL, ZVAL_PTR_DTOR, 0);
		YAF_ALLOW_VIOLATION(sess->properties);
	}

	ht = sess->properties;
	ZVAL_BOOL(&rv, sess->flags & YAF_SESSION_STARTED);
	zend_hash_str_update(ht, "started:protected", sizeof("started:protected") - 1, &rv);

	if (sess->session) {
		ZVAL_ARR(&rv, sess->session);
		Z_ADDREF(rv);
	} else {
		ZVAL_NULL(&rv);
	}
	zend_hash_str_update(ht, "session:protected", sizeof("session:protected") - 1, &rv);

	return ht;
}
/* }}} */

static void yaf_session_object_free(zend_object *object) /* {{{ */ {
	yaf_session_object *sess = php_yaf_session_fetch_object(object);

	if (sess->properties) {
		if (GC_DELREF(sess->properties)) {
			GC_REMOVE_FROM_BUFFER(sess->properties);
			zend_array_destroy(sess->properties);
		}
	}

	zend_object_std_dtor(object);
}
/* }}} */

zend_object_iterator *yaf_session_get_iterator(zend_class_entry *ce, zval *object, int by_ref) /* {{{ */ {
	yaf_iterator *iterator;
	yaf_session_object *sess = Z_YAFSESSIONOBJ_P(object);

	if (by_ref) {
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}

	if (!sess->session) {
		return NULL;
	}

	iterator = emalloc(sizeof(yaf_iterator));
	zend_iterator_init(&iterator->intern);
	iterator->intern.funcs = &yaf_iterator_funcs;

	ZVAL_ARR(&iterator->intern.data, sess->session);
	Z_ADDREF(iterator->intern.data);

	ZVAL_UNDEF(&iterator->current);

	return &iterator->intern;
}
/* }}} */

static yaf_session_t *yaf_session_instance() /* {{{ */ {
	zval *pzval;
	yaf_session_object *sess;
	yaf_session_t *instance = &YAF_G(session);

	if (IS_OBJECT == Z_TYPE_P(instance)) {
		return instance;
	}

	sess = emalloc(sizeof(yaf_session_object) + zend_object_properties_size(yaf_session_ce));
	zend_object_std_init(&sess->std, yaf_session_ce);
	sess->std.handlers = &yaf_session_obj_handlers;

	ZVAL_OBJ(&YAF_G(session), &sess->std);

	sess->flags = 0;
	yaf_session_start(sess);

	if ((pzval = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_SESSION"))) == NULL ||
		Z_TYPE_P(pzval) != IS_REFERENCE || Z_TYPE_P(Z_REFVAL_P(pzval)) != IS_ARRAY) {
		php_error_docref(NULL, E_WARNING, "Attempt to start session failed");
		sess->session = NULL;
		return &YAF_G(session);
	}
	
	sess->session = Z_ARRVAL_P(Z_REFVAL_P(pzval));
	sess->properties = NULL;

	return &YAF_G(session);
}
/* }}} */

/** {{{ proto private Yaf_Session::__construct(void)
*/
PHP_METHOD(yaf_session, __construct) {
}
/* }}} */

/** {{{ proto public Yaf_Session::getInstance(void)
*/
PHP_METHOD(yaf_session, getInstance) {
	yaf_session_t *instance;

	if ((instance = yaf_session_instance())) {
		RETURN_ZVAL(instance, 1, 0);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/** {{{ proto public Yaf_Session::count(void)
*/
PHP_METHOD(yaf_session, count) {
	yaf_session_object *sess = Z_YAFSESSIONOBJ_P(getThis());

	if (sess->session) {
		RETURN_LONG(zend_hash_num_elements(sess->session));
	}
}
/* }}} */

/** {{{ proto public static Yaf_Session::start()
*/
PHP_METHOD(yaf_session, start) {
	yaf_session_object *sess = Z_YAFSESSIONOBJ_P(getThis());

	yaf_session_start(sess);

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public static Yaf_Session::get($name)
*/
PHP_METHOD(yaf_session, get) {
	zend_string *name = NULL;
	yaf_session_object *sess = Z_YAFSESSIONOBJ_P(getThis());
	
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S!", &name) == FAILURE) {
		return;
	} 

	if (EXPECTED(sess->session)) {
		if (name == NULL) {
			RETVAL_ARR(sess->session);
			Z_ADDREF_P(return_value);
			return;
		} else {
			zval *val;
			if ((val = zend_hash_find(sess->session, name))) {
				RETURN_ZVAL(val, 1, 0);
			}
		}
	}
	RETURN_NULL();
}
/* }}} */

/** {{{ proto public static Yaf_Session::has($name)
*/
PHP_METHOD(yaf_session, has) {
	zend_string *name = NULL;
	yaf_session_object *sess = Z_YAFSESSIONOBJ_P(getThis());
	
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S!", &name) == FAILURE) {
		return;
	} 

	if (EXPECTED(sess->session)) {
		RETURN_BOOL(zend_hash_exists(sess->session, name));
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public static Yaf_Session::set($name, $value)
*/
PHP_METHOD(yaf_session, set) {
	zval *value;
	zend_string *name;
	yaf_session_object *sess = Z_YAFSESSIONOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
		return;
	}

	if (EXPECTED(sess->session)) {
		if (zend_hash_update(sess->session, name, value)) {
			Z_TRY_ADDREF_P(value);
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public static Yaf_Session::del($name)
*/
PHP_METHOD(yaf_session, del) {
	zend_string *name;
	yaf_session_object *sess = Z_YAFSESSIONOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	if (EXPECTED(sess->session)) {
		if (zend_hash_del(sess->session, name)) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public static Yaf_Session::clear()
*/
PHP_METHOD(yaf_session, clear) {
	yaf_session_object *sess = Z_YAFSESSIONOBJ_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (EXPECTED(sess->session)) {
		zend_hash_clean(sess->session);
		RETURN_ZVAL(getThis(), 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ yaf_session_methods
*/
zend_function_entry yaf_session_methods[] = {
	PHP_ME(yaf_session, __construct, NULL, ZEND_ACC_CTOR|ZEND_ACC_PRIVATE)
	PHP_ME(yaf_session, getInstance, yaf_session_void_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(yaf_session, start, yaf_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_session, get, yaf_session_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_session, has, yaf_session_has_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_session, set, yaf_session_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_session, del, yaf_session_del_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_session, count, yaf_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_session, clear, yaf_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_session, offsetGet, get, yaf_session_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_session, offsetSet, set, yaf_session_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_session, offsetExists, has, yaf_session_has_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_session, offsetUnset, del, yaf_session_del_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_session, __get, get, yaf_session_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_session, __isset, has, yaf_session_has_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_session, __set, set, yaf_session_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_session, __unset, del, yaf_session_del_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(session) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Session", "Yaf\\Session", yaf_session_methods);

	yaf_session_ce = zend_register_internal_class_ex(&ce, NULL);
	yaf_session_ce->ce_flags |= ZEND_ACC_FINAL;

	yaf_session_ce->get_iterator = yaf_session_get_iterator;
	yaf_session_ce->serialize = zend_class_serialize_deny;
	yaf_session_ce->unserialize = zend_class_unserialize_deny;

	memcpy(&yaf_session_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	yaf_session_obj_handlers.offset = XtOffsetOf(yaf_session_object, std);
	yaf_session_obj_handlers.free_obj = yaf_session_object_free;
	yaf_session_obj_handlers.clone_obj = NULL;
	yaf_session_obj_handlers.get_gc = NULL;
	yaf_session_obj_handlers.get_properties = yaf_session_get_properties;

#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
	zend_class_implements(yaf_session_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, spl_ce_Countable);
#elif PHP_VERSION_ID >= 70200
	zend_class_implements(yaf_session_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, zend_ce_countable);
#else
	zend_class_implements(yaf_session_ce, 2, zend_ce_iterator, zend_ce_arrayaccess);
#endif

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
