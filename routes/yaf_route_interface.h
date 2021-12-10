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

#ifndef YAF_ROUTER_INTERFACE_H
#define YAF_ROUTER_INTERFACE_H

#define YAF_ROUTE_ASSEMBLE_MOUDLE_FORMAT ":m"
#define YAF_ROUTE_ASSEMBLE_ACTION_FORMAT ":a"
#define YAF_ROUTE_ASSEMBLE_CONTROLLER_FORMAT ":c"

extern zend_class_entry *yaf_route_ce;

int yaf_route_instance(yaf_route_t *route, HashTable *config);

YAF_STARTUP_FUNCTION(route);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
