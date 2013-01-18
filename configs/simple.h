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

/* $Id: yaf_config.h 329002 2013-01-07 12:55:53Z laruence $ */

#ifndef YAF_CONFIG_SIMPLE_H
#define YAF_CONFIG_SIMPLE_H

extern zend_class_entry *yaf_config_simple_ce;

yaf_config_t * yaf_config_simple_instance(yaf_config_t *this_ptr, zval *values, zval *readonly TSRMLS_DC);

YAF_STARTUP_FUNCTION(config_simple);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
