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

/* $Id: yaf_namespace.h 327565 2012-09-09 07:48:24Z laruence $ */

#ifndef YAF_NAMESPACE_H
#define YAF_NAMESPACE_H

#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION > 2)) || (PHP_MAJOR_VERSION > 5)
#define YAF_BEGIN_ARG_INFO		ZEND_BEGIN_ARG_INFO
#define YAF_BEGIN_ARG_INFO_EX	ZEND_BEGIN_ARG_INFO_EX

#define YAF_INIT_CLASS_ALIAS(name, ce) \
	do {\
		zend_register_class_alias_ex(name, sizeof(name) - 1, ce TSRMLS_CC); \
    } while(0)
#else

#ifdef YAF_HAVE_NAMESPACE
#undef YAF_HAVE_NAMESPACE
#endif

#define YAF_INIT_CLASS_ALIAS(name, ce)
#define YAF_BEGIN_ARG_INFO		static ZEND_BEGIN_ARG_INFO
#define YAF_BEGIN_ARG_INFO_EX	static ZEND_BEGIN_ARG_INFO_EX
#endif

#define YAF_END_ARG_INFO		ZEND_END_ARG_INFO
#define YAF_ARG_INFO			ZEND_ARG_INFO
#define YAF_ARG_OBJ_INFO 	ZEND_ARG_OBJ_INFO
#define YAF_ARG_ARRAY_INFO 	ZEND_ARG_ARRAY_INFO

#endif	/* PHP_YAF_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
