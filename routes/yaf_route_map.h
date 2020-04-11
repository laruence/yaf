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

#ifndef YAF_ROUTER_MAP_H
#define YAF_ROUTER_MAP_H

#define YAF_ROUTE_MAP_CTL_PREFER  (1<<0)

typedef struct {
	zend_object  std;
	zend_string *delim;
	zend_array  *properties;
	zend_uchar   flags;
} yaf_route_map_object;

#define Z_YAFROUTEMAPOBJ(zv)    ((yaf_route_map_object*)(Z_OBJ(zv)))
#define Z_YAFROUTEMAPOBJ_P(zv)  Z_YAFROUTEMAPOBJ(*zv)

extern zend_class_entry *yaf_route_map_ce;

void yaf_route_map_instance(yaf_route_t *route, zend_bool controller_prefer, zend_string *delim);
int yaf_route_map_route(yaf_route_t *route, yaf_request_t *req);

YAF_STARTUP_FUNCTION(route_map);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
