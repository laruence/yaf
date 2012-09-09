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

/* $Id: supervar.c 327549 2012-09-09 03:02:48Z laruence $ */

#define YAF_ROUTE_SUPERVAR_PROPETY_NAME_VAR "_var_name"

zend_class_entry *yaf_route_supervar_ce;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_route_supervar_construct_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, supervar_name)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ int yaf_route_supervar_route(yaf_route_t *route, yaf_request_t *request TSRMLS_DC)
 */
int yaf_route_supervar_route(yaf_route_t *route, yaf_request_t *request TSRMLS_DC) {
	zval *varname, *zuri;
	char *req_uri;

	varname = zend_read_property(yaf_route_supervar_ce, route, ZEND_STRL(YAF_ROUTE_SUPERVAR_PROPETY_NAME_VAR), 1 TSRMLS_CC);

	zuri = yaf_request_query(YAF_GLOBAL_VARS_GET, Z_STRVAL_P(varname), Z_STRLEN_P(varname) TSRMLS_CC);

	if (!zuri || ZVAL_IS_NULL(zuri)) {
		return 0;
	}

	req_uri = estrndup(Z_STRVAL_P(zuri), Z_STRLEN_P(zuri));
    yaf_route_pathinfo_route(request, req_uri, Z_STRLEN_P(zuri) TSRMLS_CC);
	efree(req_uri);
	return 1;
}
/* }}} */

/** {{{ yaf_route_t * yaf_route_supervar_instance(yaf_route_t *this_ptr, zval *name TSRMLS_DC)
 */
yaf_route_t * yaf_route_supervar_instance(yaf_route_t *this_ptr, zval *name TSRMLS_DC) {
	yaf_route_t *instance;

	if (!name || IS_STRING != Z_TYPE_P(name) || !Z_STRLEN_P(name)) {
		return NULL;
	}

	if (this_ptr) {
		instance  = this_ptr;
	} else {
		MAKE_STD_ZVAL(instance);
		object_init_ex(instance, yaf_route_supervar_ce);
	}

	zend_update_property(yaf_route_supervar_ce, instance, ZEND_STRL(YAF_ROUTE_SUPERVAR_PROPETY_NAME_VAR), name TSRMLS_CC);

	return instance;
}
/* }}} */

/** {{{ proto public Yaf_Route_Supervar::route(Yaf_Request_Abstarct $request)
 */
PHP_METHOD(yaf_route_supervar, route) {
	yaf_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &request, yaf_request_ce) == FAILURE) {
		return;
	} else {
		RETURN_BOOL(yaf_route_supervar_route(getThis(), request TSRMLS_CC));
	}
}
/** }}} */

/** {{{ proto public Yaf_Route_Supervar::__construct(string $varname)
 */
PHP_METHOD(yaf_route_supervar, __construct) {
	zval *var;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &var) ==   FAILURE) {
		YAF_UNINITIALIZED_OBJECT(getThis());
		return;
	}

	if (Z_TYPE_P(var) != IS_STRING || !Z_STRLEN_P(var)) {
		YAF_UNINITIALIZED_OBJECT(getThis());
		yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "Expects a valid string super var name", yaf_route_supervar_ce->name);
		RETURN_FALSE;
	}

	zend_update_property(yaf_route_supervar_ce, getThis(), ZEND_STRL(YAF_ROUTE_SUPERVAR_PROPETY_NAME_VAR), var TSRMLS_CC);
}
/** }}} */

/** {{{ yaf_route_supervar_methods
 */
zend_function_entry yaf_route_supervar_methods[] = {
	PHP_ME(yaf_route_supervar, __construct, yaf_route_supervar_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(yaf_route_supervar, route, yaf_route_route_arginfo, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
 */
YAF_STARTUP_FUNCTION(route_supervar) {
	zend_class_entry ce;
	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Route_Supervar", "Yaf\\Route\\Supervar", yaf_route_supervar_methods);
	yaf_route_supervar_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
	zend_class_implements(yaf_route_supervar_ce TSRMLS_CC, 1, yaf_route_ce);
	yaf_route_supervar_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;

	zend_declare_property_null(yaf_route_supervar_ce, ZEND_STRL(YAF_ROUTE_SUPERVAR_PROPETY_NAME_VAR),  ZEND_ACC_PROTECTED TSRMLS_CC);

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

