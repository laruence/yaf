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

#ifndef YAF_SESSION_H
#define YAF_SESSION_H

typedef struct {
	zend_object std;
	zend_array *session;
	zend_bool started;
} yaf_session_object;

typedef struct {
	zend_object_iterator intern;
	HashPosition pos;
} yaf_session_iterator;

#define Z_YAFSESSIONOBJ(zv)   ((yaf_session_object*)(Z_OBJ(zv)))
#define Z_YAFSESSIONOBJ_P(zv) Z_YAFSESSIONOBJ(*zv)

extern zend_class_entry *yaf_session_ce;

#if PHP_VERSION_ID < 70200
PHPAPI void php_session_start();
#else
PHPAPI int php_session_start();
#endif
YAF_STARTUP_FUNCTION(session);
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
