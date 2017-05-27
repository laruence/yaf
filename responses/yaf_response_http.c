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
#include "main/SAPI.h" /* for sapi_header_line */

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_response.h"
#include "yaf_exception.h"

#include "responses/yaf_response_http.h"

zend_class_entry *yaf_response_http_ce;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_response_http_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_http_set_header_arginfo, 0, 0, 2)
  ZEND_ARG_INFO(0, name)
  ZEND_ARG_INFO(0, value)
  ZEND_ARG_INFO(0, rep)
  ZEND_ARG_INFO(0, response_code)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_http_get_header_arginfo, 0, 0, 0)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_http_set_all_headers_arginfo, 0, 0, 1)
  ZEND_ARG_INFO(0, headers)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_response_http_set_redirect_arginfo, 0, 0, 1)
  ZEND_ARG_INFO(0, url)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ zval * yaf_response_get_header(yaf_response_t *response, zend_string *name)
*/
zval * yaf_response_get_header(yaf_response_t *response, zend_string *name) {
	zval *pzval;

	zval *zheaders = zend_read_property(yaf_response_ce,
			response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADER), 1, NULL);

	if (IS_ARRAY != Z_TYPE_P(zheaders)) {
		return NULL;
	}

	if (!name) {
		return zheaders;
	}

	if ((pzval = zend_hash_find(Z_ARRVAL_P(zheaders), name)) == NULL) {
		return NULL;
	}

	return pzval;
}
/* }}} */

/** {{{ int yaf_response_clear_header(yaf_response_t *response, zend_string *name)
*/
int yaf_response_clear_header(yaf_response_t *response, zend_string *name) {
	zval *zheader = zend_read_property(yaf_response_ce,
			response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADER), 1, NULL);

	if (name) {
		zend_hash_del(Z_ARRVAL_P(zheader), name);
	} else {
		zend_hash_clean(Z_ARRVAL_P(zheader));
	}
	return 1;
}
/* }}} */

/** {{{ int yaf_response_alter_header(yaf_response_t *response, zend_string *name, char *value, long value_len, int flag)
*/
int yaf_response_alter_header(yaf_response_t *response, zend_string *name, char *value, long value_len, uint rep) {
	zval *z_headers, *pzval;
	zend_string *oheader;

	if (!name) {
		return 1;
	}

	z_headers = zend_read_property(yaf_response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADER), 1, NULL);

	if ((pzval = zend_hash_find(Z_ARRVAL_P(z_headers), name)) == NULL) {
		add_assoc_stringl_ex(z_headers, ZSTR_VAL(name), ZSTR_LEN(name), value, value_len);
		return 1;
	}

	oheader = Z_STR_P(pzval);

	if (rep) {
		ZVAL_STRINGL(pzval, value, value_len);
	} else {
		zend_string *str = zend_string_alloc(ZSTR_LEN(oheader) + value_len + 2, 0);

		memcpy(ZSTR_VAL(str), ZSTR_VAL(oheader), ZSTR_LEN(oheader));
		memcpy(ZSTR_VAL(str) + ZSTR_LEN(oheader), ", ", sizeof(", ") - 1);
		memcpy(ZSTR_VAL(str) + ZSTR_LEN(oheader) + sizeof(", ") - 1, value, value_len);
		ZSTR_VAL(str)[ZSTR_LEN(str)] = '\0';
		ZVAL_STR(pzval, str);
	}

	zend_string_release(oheader);

	return 1;
}
/* }}} */

/** {{{ int yaf_response_set_redirect(yaf_response_t *response, char *url, int len)
*/
int yaf_response_set_redirect(yaf_response_t *response, char *url, int len) {
	sapi_header_line ctr = {0};

	ctr.line_len    = spprintf(&(ctr.line), 0, "%s %s", "Location:", url);
	ctr.response_code   = 0;
	if (sapi_header_op(SAPI_HEADER_REPLACE, &ctr) == SUCCESS) {
		efree(ctr.line);
		return 1;
	}
	efree(ctr.line);
	return 0;
}
/* }}} */

/** {{{ int yaf_response_http_send(yaf_response_t *response)
*/
int yaf_response_http_send(yaf_response_t *response) {
	zval *zresponse_code, *zheader, *zbody;
	zval *val, *entry;
	ulong num_key;
	zend_string *header_name;
	sapi_header_line ctr = {0};

	zresponse_code = zend_read_property(yaf_response_ce,
			response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_RESPONSECODE), 1, NULL);

	if (Z_LVAL_P(zresponse_code)) {
		SG(sapi_headers).http_response_code = Z_LVAL_P(zresponse_code);
	}

	zheader = zend_read_property(yaf_response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADER), 1, NULL);

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(zheader), num_key, header_name, entry) {
        if (header_name) {
			ctr.line_len = spprintf(&(ctr.line), 0, "%s: %s", ZSTR_VAL(header_name), Z_STRVAL_P(entry));
		} else {
			ctr.line_len = spprintf(&(ctr.line), 0, "%lu: %s", num_key, Z_STRVAL_P(entry));
		}

		ctr.response_code = 0;
		if (sapi_header_op(SAPI_HEADER_REPLACE, &ctr) != SUCCESS) {
			efree(ctr.line);
			return 0;
		}
	} ZEND_HASH_FOREACH_END();
	efree(ctr.line);

	zbody = zend_read_property(yaf_response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1, NULL);

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zbody), val) {
		zend_string *str = zval_get_string(val);
		php_write(ZSTR_VAL(str), ZSTR_LEN(str));
		zend_string_release(str);
	} ZEND_HASH_FOREACH_END();

	return 1;
}
/* }}} */

/** {{{ proto public Yaf_Response_Http::setHeader($name, $value, $replace = 0)
*/
PHP_METHOD(yaf_response_http, setHeader) {
	zend_long response_code = 0;
	zend_bool rep = 1;
	yaf_response_t  *self;
	zend_string *name;
	zend_string *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|bl", &name, &value, &rep, &response_code) == FAILURE) {
		return;
	}

	self = getThis();

	if (response_code) {
		zend_update_property_long(yaf_response_ce, self, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_RESPONSECODE), response_code);
	}

	if (yaf_response_alter_header(self, name, ZSTR_VAL(value), ZSTR_LEN(value), rep ? 1 : 0)) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Http::setAllHeaders(void)
*/
PHP_METHOD(yaf_response_http, setAllHeaders) {
  zval *headers;
  zval *entry;
  zend_string *name;
  yaf_response_t  *self;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &headers) == FAILURE) {
	  return;
  }

  self = getThis();

  ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(headers), name, entry) {
	  zend_string *str = zval_get_string(entry);
	  yaf_response_alter_header(self, name, ZSTR_VAL(str), ZSTR_LEN(str), 1);
	  zend_string_release(str);
  } ZEND_HASH_FOREACH_END();

  RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Http::getHeader(void)
*/
PHP_METHOD(yaf_response_http, getHeader) {
	zval *header;
	zend_string *name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &name) == FAILURE) {
		return;
	}

	header = yaf_response_get_header(getThis(), name);

	if (header) {
		RETURN_ZVAL(header, 1, 0);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Response_Http::clearHeaders(void)
*/
PHP_METHOD(yaf_response_http, clearHeaders) {
	if (yaf_response_clear_header(getThis(), NULL)) {
		RETURN_ZVAL(getThis(), 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Http::setRedirect(string $url)
*/
PHP_METHOD(yaf_response_http, setRedirect) {
  char  *url;
  size_t  url_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &url, &url_len) == FAILURE) {
    return;
  }

  if (!url_len) {
    RETURN_FALSE;
  }

  RETURN_BOOL(yaf_response_set_redirect(getThis(), url, url_len));
}
/* }}} */

/** {{{ proto public Yaf_Response_Http::response(void)
 */
PHP_METHOD(yaf_response_http, response) {
  RETURN_BOOL(yaf_response_http_send(getThis()));
}
/* }}} */

/** {{{ yaf_response_methods
*/
zend_function_entry yaf_response_http_methods[] = {
  PHP_ME(yaf_response_http, setHeader,     yaf_response_http_set_header_arginfo,        ZEND_ACC_PUBLIC)
  PHP_ME(yaf_response_http, setAllHeaders, yaf_response_http_set_all_headers_arginfo,   ZEND_ACC_PUBLIC)
  PHP_ME(yaf_response_http, getHeader,     yaf_response_http_get_header_arginfo,        ZEND_ACC_PUBLIC)
  PHP_ME(yaf_response_http, clearHeaders,  yaf_response_http_void_arginfo,     ZEND_ACC_PUBLIC)
  PHP_ME(yaf_response_http, setRedirect,   yaf_response_http_set_redirect_arginfo,      ZEND_ACC_PUBLIC)
  PHP_ME(yaf_response_http, response,      yaf_response_http_void_arginfo,              ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(response_http) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Response_Http", "Yaf\\Response\\Http", yaf_response_http_methods);

	yaf_response_http_ce = zend_register_internal_class_ex(&ce, yaf_response_ce);

	zend_declare_property_bool(yaf_response_http_ce, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADEREXCEPTION), 1, ZEND_ACC_PROTECTED);
	zend_declare_property_long(yaf_response_http_ce, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_RESPONSECODE),	0, ZEND_ACC_PROTECTED);

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
