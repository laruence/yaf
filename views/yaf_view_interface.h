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

/* $Id: yaf_view.h 329002 2013-01-07 12:55:53Z laruence $ */

#ifndef YAF_VIEW_INTERFACE_H
#define YAF_VIEW_INTERFACE_H

YAF_BEGIN_ARG_INFO_EX(yaf_view_assign_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
YAF_END_ARG_INFO()

YAF_BEGIN_ARG_INFO_EX(yaf_view_display_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, tpl)
	ZEND_ARG_INFO(0, tpl_vars)
YAF_END_ARG_INFO()

YAF_BEGIN_ARG_INFO_EX(yaf_view_render_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, tpl)
	ZEND_ARG_INFO(0, tpl_vars)
YAF_END_ARG_INFO()

YAF_BEGIN_ARG_INFO_EX(yaf_view_setpath_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, template_dir)
YAF_END_ARG_INFO()

YAF_BEGIN_ARG_INFO_EX(yaf_view_getpath_arginfo, 0, 0, 0)
YAF_END_ARG_INFO()

extern zend_class_entry *yaf_view_interface_ce;

YAF_STARTUP_FUNCTION(view_interface);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
