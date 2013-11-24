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

/* $Id: http.c 329197 2013-01-18 05:55:37Z laruence $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_response.h"
#include "yaf_exception.h"

#include "responses/yaf_response_http.h"

zend_class_entry *yaf_response_http_ce;

/** {{{ ARG_INFO
 */
 ZEND_BEGIN_ARG_INFO_EX(yaf_response_set_header_arginfo, 0, 0, 2)
  ZEND_ARG_INFO(0, name)
  ZEND_ARG_INFO(0, value)
  ZEND_ARG_INFO(0, rep)
  ZEND_ARG_INFO(0, response_code)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_clear_headers_arginfo, 0, 0, 0)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_get_header_arginfo, 0, 0, 0)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_set_all_headers_arginfo, 0, 0, 1)
  ZEND_ARG_INFO(0, headers)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ int yaf_response_alter_header(yaf_response_t *response, char *name, uint name_len, char *value, long value_len, int flag TSRMLS_DC)
*/
int yaf_response_alter_header(yaf_response_t *response, char *name, uint name_len, char *value, long value_len, uint rep TSRMLS_DC) {
  zval *z_headers, **ppzval;
  char *oheader;

  if (!name_len || !value_len) {
    return 1;
  }

  z_headers = zend_read_property(yaf_response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADER), 1 TSRMLS_CC);

  if (zend_hash_find(Z_ARRVAL_P(z_headers), name, name_len, (void **)&ppzval) == FAILURE) {
    add_assoc_stringl_ex(z_headers, name, name_len, value, value_len, 1);

    return 1;
  }

  oheader = Z_STRVAL_PP(ppzval);

  if (rep) {
    ZVAL_STRINGL(*ppzval, value, value_len, 1);
  } else {
    Z_STRLEN_PP(ppzval) = spprintf(&Z_STRVAL_PP(ppzval), 0, "%s, %s", oheader, value);
  }

  efree(oheader);

  return 1;
}
/* }}} */

/** {{{ zval * yaf_response_get_header(yaf_response_t *response, char *name, uint name_len TSRMLS_DC)
 */
zval * yaf_response_get_header(yaf_response_t *response, char *name, uint name_len TSRMLS_DC) {
    zval **ppzval;
    
    zval *zheaders = zend_read_property(yaf_response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADER), 1 TSRMLS_CC);

    if (IS_ARRAY != Z_TYPE_P(zheaders)) {
      return NULL;
    }
    
    if (!name_len) {
      return zheaders;
    }

    if (zend_hash_find(Z_ARRVAL_P(zheaders), name, name_len + 1, (void **)&ppzval) == FAILURE) {
      return NULL;
    }

    return *ppzval;
}
/* }}} */

/** {{{ int yaf_response_clear_header(yaf_response_t *response, char *name, uint name_len TSRMLS_DC)
 */
int yaf_response_clear_header(yaf_response_t *response, char *name, uint name_len TSRMLS_DC) {
        zval *zheader;
        zheader = zend_read_property(yaf_response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADER), 1 TSRMLS_CC);

        if (name_len) {
                zend_hash_del(Z_ARRVAL_P(zheader), name, name_len + 1);
        } else {
                zend_hash_clean(Z_ARRVAL_P(zheader));
        }
        return 1;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::setHeader($name, $value, $replace = 0)
*/
PHP_METHOD(yaf_response, setHeader) {
  zval    *response_code = NULL;
  char    *name, *value;
  uint    name_len, value_len;
  zend_bool   rep = 1;
  yaf_response_t  *self;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|bz", &name, &name_len, &value, &value_len, &rep, &response_code) == FAILURE) {
    return;
  }

  self = getThis();
  
  if (response_code) {
    zend_update_property(yaf_response_ce, self, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_RESPONSECODE), response_code TSRMLS_CC);
  }
  
  if (yaf_response_alter_header(self, name, name_len + 1, value, value_len, rep ? 1 : 0 TSRMLS_CC)) {
    RETURN_TRUE;
  }

  RETURN_FALSE;
}
/* }}} */

/** {{{ proto protected Yaf_Response_Abstract::setAllHeaders(void)
*/
PHP_METHOD(yaf_response, setAllHeaders) {
  zval      *headers;
  zval      **entry;
  char      *header_name;
  uint      header_name_len;
  ulong       num_key;
  HashPosition  pos;
  yaf_response_t  *self;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &headers) == FAILURE) {
    return;
  }

  self = getThis();

  for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(headers), &pos);
      zend_hash_get_current_data_ex(Z_ARRVAL_P(headers), (void **)&entry, &pos) == SUCCESS;
      zend_hash_move_forward_ex(Z_ARRVAL_P(headers), &pos)) {
    if (zend_hash_get_current_key_ex(Z_ARRVAL_P(headers), &header_name, &header_name_len, &num_key, 0, &pos) != HASH_KEY_IS_STRING) {
        continue;
      }

    yaf_response_alter_header(self, header_name, header_name_len, Z_STRVAL_PP(entry), Z_STRLEN_PP(entry), 1 TSRMLS_CC);
  }

  RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::getHeader(void)
*/
PHP_METHOD(yaf_response, getHeader) {
  zval *header = NULL;
  char *name;
  uint name_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
    return;
  }
  
  if (!name_len) {
    header = yaf_response_get_header(getThis(), NULL, 0 TSRMLS_CC);
  } else {
    header = yaf_response_get_header(getThis(), name, name_len TSRMLS_CC);
  }

  if (header) {
    RETURN_ZVAL(header, 1, 0);
  }

  RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::clearHeaders(void)
*/
PHP_METHOD(yaf_response, clearHeaders) {
  char *name = NULL;
    uint name_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
      return;
  }

  if (!name_len) {
    zval *default_response_code;
    MAKE_STD_ZVAL(default_response_code);
    ZVAL_LONG(default_response_code, 200);

    zend_update_property(yaf_response_ce, getThis(), ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_RESPONSECODE), default_response_code TSRMLS_CC);
    zval_ptr_dtor(&default_response_code);
  }

    if (yaf_response_clear_header(getThis(), name, name_len TSRMLS_CC)) {
      RETURN_ZVAL(getThis(), 1, 0);
    }

  RETURN_FALSE;
}
/* }}} */

/** {{{ yaf_response_methods
*/
zend_function_entry yaf_response_http_methods[] = {
  PHP_ME(yaf_response, setHeader,   yaf_response_set_header_arginfo,            ZEND_ACC_PUBLIC)
  PHP_ME(yaf_response, setAllHeaders, yaf_response_set_all_headers_arginfo,     ZEND_ACC_PUBLIC)
  PHP_ME(yaf_response, getHeader,   yaf_response_get_header_arginfo,            ZEND_ACC_PUBLIC)
  PHP_ME(yaf_response, clearHeaders,  yaf_response_clear_headers_arginfo,       ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(response_http) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Response_Http", "Yaf\\Response\\Http", yaf_response_http_methods);

	yaf_response_http_ce = zend_register_internal_class_ex(&ce, yaf_response_ce, NULL TSRMLS_CC);

	zend_declare_property_bool(yaf_response_http_ce, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADEREXCEPTION), 1, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(yaf_response_http_ce, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_RESPONSECODE),	200, ZEND_ACC_PROTECTED TSRMLS_CC);

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
