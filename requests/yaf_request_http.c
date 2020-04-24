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
#include "main/SAPI.h"
#include "ext/standard/url.h" /* for php_url */

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_request.h"
#include "yaf_exception.h"

#include "requests/yaf_request_http.h"

zend_class_entry *yaf_request_http_ce;

void yaf_request_http_init(yaf_request_object *req, zend_string *request_uri, zend_string *base_uri) /* {{{ */ {
	const char *method;
	zend_string *settled_uri = NULL;
	
	method = yaf_request_get_request_method();
	req->method = zend_string_init(method, strlen(method), 0);

	if (request_uri) {
		settled_uri = zend_string_copy(request_uri);
	} else {
		zval *uri;
		do {
#ifdef PHP_WIN32
			zval *rewrited;
			/* check this first so IIS will catch */
			uri = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, ZEND_STRL("HTTP_X_REWRITE_URL"));
			if (uri) {
				if (EXPECTED(Z_TYPE_P(uri) == IS_STRING))  {
					settled_uri = zend_string_copy(Z_STR_P(uri));
					break;
				}
			}

			/* IIS7 with URL Rewrite: make sure we get the unencoded url (double slash problem) */
			rewrited = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, ZEND_STRL("IIS_WasUrlRewritten"));
			if (rewrited) {
				if (zend_is_true(rewrited)) {
					uri = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, ZEND_STRL("UNENCODED_URL"));
					if (uri) {
						if (EXPECTED(Z_TYPE_P(uri) == IS_STRING && Z_STRLEN_P(uri))) {
							settled_uri = zend_string_copy(Z_STR_P(uri));
							break;
						}
					}
				}
			}
#endif
			uri = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, ZEND_STRL("PATH_INFO"));
			if (uri) {
				if (EXPECTED(Z_TYPE_P(uri) == IS_STRING)) {
					settled_uri = zend_string_copy(Z_STR_P(uri));
					break;
				}
			}

			uri = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, ZEND_STRL("REQUEST_URI"));
			if (uri) {
				if (EXPECTED(Z_TYPE_P(uri) == IS_STRING)) {
					/* Http proxy reqs setup request uri with scheme and host [and port] + the url path,
					 * only use url path */
					if (strncasecmp(Z_STRVAL_P(uri), "http", sizeof("http") - 1) == 0) {
						php_url *url_info = php_url_parse(Z_STRVAL_P(uri));
#if PHP_VERSION_ID < 70300
						if (url_info && url_info->path) {
							settled_uri = zend_string_init(url_info->path, strlen(url_info->path), 0);
						}
#else
						settled_uri = url_info->path;
						url_info->path = NULL;
#endif
						php_url_free(url_info);
					} else {
						char *pos = NULL;
						if ((pos = strstr(Z_STRVAL_P(uri), "?"))) {
							settled_uri = zend_string_init(Z_STRVAL_P(uri), pos - Z_STRVAL_P(uri), 0);
						} else {
							settled_uri = zend_string_copy(Z_STR_P(uri));
						}
					}
					break;
				}
			}

			uri = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, ZEND_STRL("ORIG_PATH_INFO"));
			if (uri) {
				if (EXPECTED(Z_TYPE_P(uri) == IS_STRING)) {
					settled_uri = zend_string_copy(Z_STR_P(uri));
					break;
				}
			}
		} while (0);
	}

	if (settled_uri) {
		req->uri = settled_uri;
		yaf_request_set_base_uri(req, base_uri, settled_uri);
	}

	return;
}
/* }}} */

/** {{{ proto public Yaf_Request_Http::__construct(string $request_uri, string $base_uri)
*/
PHP_METHOD(yaf_request_http, __construct) {
	zend_string *request_uri = NULL;
	zend_string *base_uri = NULL;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|SS", &request_uri, &base_uri) == FAILURE) {
		return;
	}

	yaf_request_http_init(Z_YAFREQUESTOBJ_P(getThis()), request_uri, base_uri);
}
/* }}} */

/** {{{ yaf_request_http_methods
 */
zend_function_entry yaf_request_http_methods[] = {
	PHP_ME(yaf_request_http, __construct,      NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
 */
YAF_STARTUP_FUNCTION(request_http){
	zend_class_entry ce;
	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Request_Http", "Yaf\\Request\\Http", yaf_request_http_methods);
	yaf_request_http_ce = zend_register_internal_class_ex(&ce, yaf_request_ce);

	zend_declare_class_constant_string(yaf_request_ce, ZEND_STRL("SCHEME_HTTP"), "http");
	zend_declare_class_constant_string(yaf_request_ce, ZEND_STRL("SCHEME_HTTPS"), "https");

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
