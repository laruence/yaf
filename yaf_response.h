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

#ifndef PHP_YAF_RESPONSE_H
#define PHP_YAF_RESPONSE_H

#define YAF_RESPONSE_PROPERTY_NAME_HEADER			"_header"
#define YAF_RESPONSE_PROPERTY_NAME_BODY				"_body"
#define YAF_RESPONSE_PROPERTY_NAME_HEADEREXCEPTION	"_sendheader"
#define YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODY		"content"
#define YAF_RESPONSE_PROPERTY_NAME_DEFAULTBODYNAME  "DEFAULT_BODY"

#define YAF_RESPONSE_REPLACE 0
#define YAF_RESPONSE_PREPEND 1
#define YAF_RESPONSE_APPEND  2

extern zend_class_entry *yaf_response_ce;
extern zend_class_entry *yaf_response_http_ce;
extern zend_class_entry *yaf_response_cli_ce;

yaf_response_t * yaf_response_instance(yaf_response_t *this_ptr, char *sapi_name);
int yaf_response_alter_body(yaf_response_t *response, zend_string *name, zend_string *body, int flag);
int yaf_response_send(yaf_response_t *response);
int yaf_response_set_redirect(yaf_response_t *response, char *url, int len);
int yaf_response_clear_body(yaf_response_t *response, zend_string *name);

YAF_STARTUP_FUNCTION(response);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
