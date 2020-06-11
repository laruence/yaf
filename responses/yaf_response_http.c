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

zval *yaf_response_get_header(yaf_response_object *response, zend_string *name) /* {{{ */ {
	if (response->header) {
		return zend_hash_find(response->header, name);
	}
	return NULL;
}
/* }}} */

int yaf_response_clear_header(yaf_response_object *response, zend_string *name) /* {{{ */ {
	if (response->header) {
		if (name) {
			zend_hash_del(response->header, name);
		} else {
			zend_hash_clean(response->header);
		}
	}
	return 1;
}
/* }}} */

int yaf_response_alter_header(yaf_response_object *response, zend_string *name, zend_string *value, unsigned rep) /* {{{ */ {
	zval *pzval;

	if (!name) {
		return 1;
	}

	if (!response->header) {
		ALLOC_HASHTABLE(response->header);
		zend_hash_init(response->header, 8, NULL, ZVAL_PTR_DTOR, 0);
		YAF_ALLOW_VIOLATION(response->header);
	}

	if ((pzval = zend_hash_find(response->header, name)) == NULL) {
		zval rv;
		ZVAL_STR_COPY(&rv, value);
		zend_hash_update(response->header, name, &rv);
		return 1;
	}

	if (rep) {
		zval_ptr_dtor(pzval);
		ZVAL_STR_COPY(pzval, value);
	} else {
		zend_string *str = zend_string_alloc(Z_STRLEN_P(pzval) + ZSTR_LEN(value) + 2, 0);

		memcpy(ZSTR_VAL(str), Z_STRVAL_P(pzval), Z_STRLEN_P(pzval));
		memcpy(ZSTR_VAL(str) + Z_STRLEN_P(pzval), ", ", sizeof(", ") - 1);
		memcpy(ZSTR_VAL(str) + Z_STRLEN_P(pzval) + sizeof(", ") - 1, ZSTR_VAL(value), ZSTR_LEN(value));
		ZSTR_VAL(str)[ZSTR_LEN(str)] = '\0';

		zval_ptr_dtor(pzval);
		ZVAL_STR(pzval, str);
	}

	return 1;
}
/* }}} */

int yaf_response_set_redirect(yaf_response_object *response, zend_string *url) /* {{{ */ {
	sapi_header_line ctr = {0};

	if (strcmp("cli", sapi_module.name) == 0 || strcmp("phpdbg", sapi_module.name) == 0) {
		return 0;
	}
	ctr.line_len = spprintf(&(ctr.line), 0, "%s %s", "Location:", ZSTR_VAL(url));
	ctr.response_code = 0;
	if (sapi_header_op(SAPI_HEADER_REPLACE, &ctr) == SUCCESS) {
		response->flags = YAF_RESPONSE_HEADER_SENT;
		efree(ctr.line);
		return 1;
	}
	efree(ctr.line);
	return 0;
}
/* }}} */

int yaf_response_http_send(yaf_response_object *response) /* {{{ */ {
	zval *entry;
	zend_ulong num_key;
	zend_string *header_name;

	if (!(response->flags & YAF_RESPONSE_HEADER_SENT)) {
		if (response->code) {
			SG(sapi_headers).http_response_code = response->code;
		}
		if (response->header) {
			sapi_header_line ctr = {0};
			ZEND_HASH_FOREACH_KEY_VAL(response->header, num_key, header_name, entry) {
				if (header_name) {
					ctr.line_len = spprintf(&(ctr.line), 0, "%s: %s", ZSTR_VAL(header_name), Z_STRVAL_P(entry));
				} else {
					ctr.line_len = spprintf(&(ctr.line), 0, ""ZEND_ULONG_FMT": %s", num_key, Z_STRVAL_P(entry));
				}

				ctr.response_code = 0;
				if (sapi_header_op(SAPI_HEADER_REPLACE, &ctr) != SUCCESS) {
					efree(ctr.line);
					return 0;
				}
			} ZEND_HASH_FOREACH_END();
			efree(ctr.line);
			response->flags |= YAF_RESPONSE_HEADER_SENT;
		}
	}

	if (response->body) {
		ZEND_HASH_FOREACH_VAL(response->body, entry) {
			zend_string *str = zval_get_string(entry);
			php_write(ZSTR_VAL(str), ZSTR_LEN(str));
			zend_string_release(str);
		} ZEND_HASH_FOREACH_END();
	}

	return 1;
}
/* }}} */

/** {{{ proto public Yaf_Response_Http::setHeader($name, $value, $replace = 0)
*/
PHP_METHOD(yaf_response_http, setHeader) {
	zend_long response_code = 0;
	zend_bool rep = 1;
	zend_string *name;
	zend_string *value;
	yaf_response_object *response = Z_YAFRESPONSEOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|bl", &name, &value, &rep, &response_code) == FAILURE) {
		return;
	}

	if (response_code) {
		response->code = response_code;
	}

	if (yaf_response_alter_header(response, name, value, rep ? 1 : 0)) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Http::setAllHeaders(array $headers)
*/
PHP_METHOD(yaf_response_http, setAllHeaders) {
  zval *headers;
  zval *entry;
  zend_string *name;
  yaf_response_object *response = Z_YAFRESPONSEOBJ_P(getThis());

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &headers) == FAILURE) {
	  return;
  }

  ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(headers), name, entry) {
	  zend_string *str = zval_get_string(entry);
	  yaf_response_alter_header(response, name, str, 1);
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
	yaf_response_object *response = Z_YAFRESPONSEOBJ_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &name) == FAILURE) {
		return;
	}

	if (name == NULL) {
		if (response->header) {
			GC_ADDREF(response->header);
			RETURN_ARR(response->header);
		}
#if PHP_VERSION_ID < 70400
		array_init(return_value);
		return;
#else
		RETURN_EMPTY_ARRAY();
#endif
	} else {
		if ((header = yaf_response_get_header(Z_YAFRESPONSEOBJ_P(getThis()), name))) {
			RETURN_ZVAL(header, 1, 0);
		}
		RETURN_NULL();
	}
}
/* }}} */

/** {{{ proto public Yaf_Response_Http::clearHeaders(void)
*/
PHP_METHOD(yaf_response_http, clearHeaders) {
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	if (yaf_response_clear_header(Z_YAFRESPONSEOBJ_P(getThis()), NULL)) {
		RETURN_ZVAL(getThis(), 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Http::setRedirect(string $url)
*/
PHP_METHOD(yaf_response_http, setRedirect) {
	zend_string *url;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &url) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(url)) {
		RETURN_FALSE;
	}

	RETURN_BOOL(yaf_response_set_redirect(Z_YAFRESPONSEOBJ_P(getThis()), url));
}
/* }}} */

/** {{{ proto public Yaf_Response_Http::response(void)
 */
PHP_METHOD(yaf_response_http, response) {
  if (zend_parse_parameters_none() == FAILURE) {
	  return;
  }
  RETURN_BOOL(yaf_response_http_send(Z_YAFRESPONSEOBJ_P(getThis())));
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
