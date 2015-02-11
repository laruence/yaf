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

#ifndef YAF_LOGO_H
#define YAF_LOGO_H

#define YAF_LOGO_BASE64  \
"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"

#define YAF_LOGO_DATA      "data:image/png;base64," YAF_LOGO_BASE64
#define YAF_SUPPORT_URL	   "http://pecl.php.net/package/yaf"
#define YAF_LOGO_IMG       "<a href=" YAF_SUPPORT_URL "> \
						   <img src=\"" YAF_LOGO_DATA "\" alt=\"Yaf logo\" /></a>\n"

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
