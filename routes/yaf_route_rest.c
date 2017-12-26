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
#include "zend_smart_str.h" /* for smart_str */
#include "ext/pcre/php_pcre.h" /* for pcre */

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_request.h"

#include "yaf_router.h"
#include "routes/yaf_route_interface.h"
#include "routes/yaf_route_rest.h"

#include "ext/standard/php_string.h"

zend_class_entry *yaf_route_rest_ce;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_route_rest_construct_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, match)
    ZEND_ARG_ARRAY_INFO(0, route, 0)
    ZEND_ARG_ARRAY_INFO(0, verify, 1)
ZEND_END_ARG_INFO()
/* }}} */

yaf_route_t * yaf_route_rest_instance(yaf_route_t *this_ptr, zval *match, zval *route, zval *verify) /* {{{ */ {
    if (Z_ISUNDEF_P(this_ptr)) {
        object_init_ex(this_ptr, yaf_route_rest_ce);
    }

    zend_update_property(yaf_route_rest_ce, this_ptr, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_MATCH), match);
    zend_update_property(yaf_route_rest_ce, this_ptr, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_ROUTE), route);

    if (!verify) {
        zend_update_property_null(yaf_route_rest_ce, this_ptr, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_VERIFY));
    } else {
        zend_update_property(yaf_route_rest_ce, this_ptr, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_VERIFY), verify);
    }

    return this_ptr;
}
/* }}} */

/** {{{ static int yaf_route_rest_match(yaf_route_t *router, zend_string *uri, zval *ret)
 */
static int yaf_route_rest_match(yaf_route_t *router, zend_string *uri, zval *ret) {
    char *seg, *pmatch, *ptrptr;
    int  seg_len;
    zval *match;
    pcre_cache_entry *pce_regexp;
    smart_str pattern = {0};

    if (ZSTR_LEN(uri) == 0) {
        return 0;
    }

    match  = zend_read_property(yaf_route_rest_ce, router, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_MATCH), 0, NULL);
    pmatch = estrndup(Z_STRVAL_P(match), Z_STRLEN_P(match));

    smart_str_appendc(&pattern, YAF_ROUTE_REGEX_DILIMITER);
    smart_str_appendc(&pattern, '^');

    seg = php_strtok_r(pmatch, YAF_ROUTER_URL_DELIMIETER, &ptrptr);
    while (seg) {
        seg_len = strlen(seg);
        if (seg_len) {
            smart_str_appendl(&pattern, YAF_ROUTER_URL_DELIMIETER, 1);

            if(*(seg) == '*') {
                smart_str_appendl(&pattern, "(?P<__yaf_route_rest>.*)", sizeof("(?P<__yaf_route_rest>.*)") -1);
                break;
            }

            if(*(seg) == ':') {
                smart_str_appendl(&pattern, "(?P<", sizeof("(?P<") -1 );
                smart_str_appendl(&pattern, seg + 1, seg_len - 1);
                smart_str_appendl(&pattern, ">[^"YAF_ROUTER_URL_DELIMIETER"]+)", sizeof(">[^"YAF_ROUTER_URL_DELIMIETER"]+)") - 1);
            } else {
                smart_str_appendl(&pattern, seg, seg_len);
            }

        }
        seg = php_strtok_r(NULL, YAF_ROUTER_URL_DELIMIETER, &ptrptr);
    }

    efree(pmatch);
    smart_str_appendc(&pattern, YAF_ROUTE_REGEX_DILIMITER);
    smart_str_appendc(&pattern, 'i');
    smart_str_0(&pattern);

    if ((pce_regexp = pcre_get_compiled_regex_cache(pattern.s)) == NULL) {
        smart_str_free(&pattern);
        return 0;
    } else {
        zval matches, subparts;

        smart_str_free(&pattern);

        ZVAL_NULL(&subparts);
        php_pcre_match_impl(pce_regexp, ZSTR_VAL(uri), ZSTR_LEN(uri), &matches, &subparts /* subpats */,
                0/* global */, 0/* ZEND_NUM_ARGS() >= 4 */, 0/*flags PREG_OFFSET_CAPTURE*/, 0/* start_offset */);

        if (!zend_hash_num_elements(Z_ARRVAL(subparts))) {
            zval_ptr_dtor(&subparts);
            return 0;
        } else {
            zval *pzval;
            zend_string *key;
            HashTable *ht;

            array_init(ret);

            ht = Z_ARRVAL(subparts);
            ZEND_HASH_FOREACH_STR_KEY_VAL(ht, key, pzval) {
                if (key) {
                    if (zend_string_equals_literal(key, "__yaf_route_rest")) {
                        zval args;
                        (void)yaf_router_parse_parameters(Z_STRVAL_P(pzval), &args);
                         zend_hash_copy(Z_ARRVAL_P(ret), Z_ARRVAL(args), (copy_ctor_func_t) zval_add_ref);
                         zval_ptr_dtor(&args);
                    } else {
                        Z_ADDREF_P(pzval);
                        zend_hash_update(Z_ARRVAL_P(ret), key, pzval);
                    }
                }
            } ZEND_HASH_FOREACH_END();

            zval_ptr_dtor(&subparts);
            return 1;
        }
    }
}
/* }}} */

/** {{{ int yaf_route_rest_route(yaf_route_t *router, yaf_request_t *request)
 */
int yaf_route_rest_route(yaf_route_t *router, yaf_request_t *request) {
    zend_string *request_uri;
    zval args, *base_uri, *zuri;

    zuri     = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_URI), 1, NULL);
    base_uri = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_BASE), 1, NULL);

    if (base_uri && IS_STRING == Z_TYPE_P(base_uri)
            && !strncasecmp(Z_STRVAL_P(zuri), Z_STRVAL_P(base_uri), Z_STRLEN_P(base_uri))) {
        request_uri  = zend_string_init(Z_STRVAL_P(zuri) + Z_STRLEN_P(base_uri), Z_STRLEN_P(zuri) - Z_STRLEN_P(base_uri), 0);
    } else {
        request_uri  = zend_string_copy(Z_STR_P(zuri));
    }

    if (!yaf_route_rest_match(router, request_uri, &args)) {
        zend_string_release(request_uri);
        return 0;
    } else {
        zval *module, *controller, *action, *routes;

        routes = zend_read_property(yaf_route_rest_ce, router, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_ROUTE), 1, NULL);
        if ((module = zend_hash_str_find(Z_ARRVAL_P(routes), ZEND_STRL("module"))) != NULL && IS_STRING == Z_TYPE_P(module)) {
            if (Z_STRVAL_P(module)[0] != ':') {
                zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), module);
            } else {
                zval *m;
                if ((m = zend_hash_str_find(Z_ARRVAL(args), Z_STRVAL_P(module) + 1, Z_STRLEN_P(module) - 1)) != NULL && IS_STRING == Z_TYPE_P(m)) {
                    zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), m);
                }
            }
        }

        if ((controller = zend_hash_str_find(Z_ARRVAL_P(routes), ZEND_STRL("controller"))) != NULL && IS_STRING == Z_TYPE_P(controller)) {
            if (Z_STRVAL_P(controller)[0] != ':') {
                zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), controller);
            } else {
                zval *c;
                if ((c = zend_hash_str_find(Z_ARRVAL(args), Z_STRVAL_P(controller) + 1, Z_STRLEN_P(controller) - 1)) != NULL && IS_STRING == Z_TYPE_P(c)) {
                    zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), c);
                }
            }
        }

        if ((action = zend_hash_str_find(Z_ARRVAL_P(routes), ZEND_STRL("action"))) != NULL && IS_STRING == Z_TYPE_P(action)) {
            zval *method_ptr;
            method_ptr = yaf_request_get_method(request);
            //Method_
            zval t;
            ZVAL_STRINGL(&t, "_", sizeof("_") - 1);
            concat_function(method_ptr, method_ptr, &t);
            zval_ptr_dtor(&t);

            zval rest_action;
            ZVAL_STRINGL(&rest_action, "", 0);

            if(Z_STRVAL_P(method_ptr)[0] == 'G'){
                //method为GET,有page 取分页数据参数
                zval page, pageSize;
                zval *page_ptr = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, "HTTP_PAGE", sizeof("HTTP_PAGE") - 1);
                zval *pageSize_ptr = yaf_request_query_str(YAF_GLOBAL_VARS_SERVER, "HTTP_PAGESIZE", sizeof("HTTP_PAGESIZE") - 1);
                
                if(page_ptr) {
                    ZVAL_STRINGL(&page, Z_STRVAL_P(page_ptr), Z_STRLEN_P(page_ptr));  
                    zval_ptr_dtor(page_ptr);

                    if(pageSize_ptr) {
                        ZVAL_STRINGL(&pageSize, Z_STRVAL_P(pageSize_ptr), Z_STRLEN_P(pageSize_ptr));  
                        zval_ptr_dtor(pageSize_ptr);      
                    }else{
                        ZVAL_STRINGL(&pageSize, "20", sizeof("20") - 1);                           
                    }
                    
                    convert_to_long(&page);
                    convert_to_long(&pageSize);
                    
                    //php_printf("page=");
                    //zend_print_zval(&page, 0);
                    //php_printf("\n");
                    
                    //setParam("page", page);
                    //setParam("pageSize", pageSize);
                    //给php增加参数page,pageSize
                    zend_string *param_page, *param_pageSize;
                    param_page = zend_string_init("page", sizeof("page") - 1, 0);
                    param_pageSize = zend_string_init("pageSize", sizeof("pageSize") - 1, 0);
                    yaf_request_set_params_single(request, param_page, &page);
                    yaf_request_set_params_single(request, param_pageSize, &pageSize);
                    zend_string_release(param_page);
                    zend_string_release(param_pageSize);
                    zval_ptr_dtor(&page);
                    zval_ptr_dtor(&pageSize);                         
                }
            }

            if (Z_STRVAL_P(action)[0] != ':') {
                //Method_action
                concat_function(&rest_action, method_ptr, action);
            } else {
                zval *a;
                if ((a = zend_hash_str_find(Z_ARRVAL(args), Z_STRVAL_P(action) + 1, Z_STRLEN_P(action) - 1)) != NULL && IS_STRING == Z_TYPE_P(a)) {
    
                    long lval;
                    double dval;
                    //对action作是否数字的判断
                    if (is_numeric_string(Z_STRVAL_P(a), Z_STRLEN_P(a), &lval, &dval, 0) == IS_LONG) {
                        zval zval_request_uri, str_ci, url_delimieter;

                        ZVAL_STRINGL(&url_delimieter, YAF_ROUTER_URL_DELIMIETER, sizeof(YAF_ROUTER_URL_DELIMIETER) - 1);

                        //str_ci = /controller/action(id)/
                        ZVAL_STRINGL(&str_ci, "", 0);
                        concat_function(&str_ci, &str_ci, &url_delimieter);
                        concat_function(&str_ci, &str_ci, zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), 1, NULL));
                        concat_function(&str_ci, &str_ci, &url_delimieter);
                        concat_function(&str_ci, &str_ci, a);
                        concat_function(&str_ci, &str_ci, &url_delimieter);

                        //uri = */controller/action(id)/*
                        ZVAL_STRINGL(&zval_request_uri, ZSTR_VAL(request_uri), ZSTR_LEN(request_uri));
                        /*php_printf("str_ci=");
                        zend_print_zval(&str_ci, 0);
                        php_printf("\n");
                        php_printf("request_uri_ci=");
                        zend_print_zval(&request_uri, 0);
                        php_printf("\n");*/
                        //通过在uri中查找的ci以后的第一个元素取action
                        char *uri, *ci, *ptrptr;
                        uri = estrndup(Z_STRVAL(zval_request_uri), Z_STRLEN(zval_request_uri));
                        ci = estrndup(Z_STRVAL(str_ci), Z_STRLEN(str_ci));
                        int ci_len = strlen(ci);
                        char str_action[32];
                        memset((void*)str_action, 0, 32);
                        if((ptrptr = strstr(uri, ci)) != NULL){
                            ptrptr += ci_len;
                            int i = 0;
                            while (*(ptrptr) && *(ptrptr)!='/')
                            {
                                str_action[i]=*ptrptr;
                                ++i;
                                ++ptrptr;
                            }
                        }
                        efree(uri);
                        efree(ci);
                        ptrptr = NULL;
                                                
                        zval tempActionName;
                        
                        if(str_action[0]){
                            //tempAction = action
                            ZVAL_STRINGL(&tempActionName, str_action, strlen(str_action));                          
                        }
                        else{
                            //tempAction = ""
                            ZVAL_STRINGL(&tempActionName, "", 0);
                        }
                        //rest_action = Method_(tempAction)
                        concat_function(&rest_action, method_ptr, &tempActionName);                     
                        zval_ptr_dtor(&tempActionName);
                        
                        //setParam("id", a);
                        zend_string *param_id;
                        param_id = zend_string_init("id", sizeof("id") - 1, 0);
                        yaf_request_set_params_single(request, param_id, a);
                        zend_string_release(param_id);
                        zval_ptr_dtor(&str_ci);
                        zval_ptr_dtor(&url_delimieter);
                        zval_ptr_dtor(&zval_request_uri);
                    }else{
                        concat_function(&rest_action, method_ptr, a);
                    }
                }
                zval_ptr_dtor(a);
            }
            
            zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), &rest_action);

            zval_ptr_dtor(&rest_action);
            zval_ptr_dtor(method_ptr);
        }

        (void)yaf_request_set_params_multi(request, &args);
        zval_ptr_dtor(&args);
        zend_string_release(request_uri);
        return 1;
    }

}
/* }}} */

/** {{{ proto public Yaf_Route_Rest::route(Yaf_Request_Abstract $request)
 */
PHP_METHOD(yaf_route_rest, route) {
    yaf_route_t     *route;
    yaf_request_t   *request;

    route = getThis();

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &request, yaf_request_ce) == FAILURE) {
        return;
    }

    if (!request || IS_OBJECT != Z_TYPE_P(request)
            || !instanceof_function(Z_OBJCE_P(request), yaf_request_ce)) {
        php_error_docref(NULL, E_WARNING, "Expect a %s instance", ZSTR_VAL(yaf_request_ce->name));
        RETURN_FALSE;
    }

    RETURN_BOOL(yaf_route_rest_route(route, request));
}
/** }}} */

/** {{{ zend_string * yaf_route_rest_assemble(yaf_route_t *this_ptr, zval *info, zval *query)
 */
zend_string * yaf_route_rest_assemble(yaf_route_t *this_ptr, zval *info, zval *query) {
    zval *match, pidents, *zv;
    char *seg, *pmatch, *ptrptr;
    zend_string *key, *inter, *uri, *val;
    size_t seg_len;
    smart_str query_str = {0};
    smart_str wildcard = {0};
    
    array_init(&pidents);

    match  = zend_read_property(yaf_route_rest_ce, this_ptr, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_MATCH), 0, NULL);
    uri = zend_string_copy(Z_STR_P(match));
    pmatch = estrndup(Z_STRVAL_P(match), Z_STRLEN_P(match));
    zend_hash_copy(Z_ARRVAL(pidents), Z_ARRVAL_P(info), (copy_ctor_func_t) zval_add_ref);

    seg = php_strtok_r(pmatch, YAF_ROUTER_URL_DELIMIETER, &ptrptr); 
    while (seg) {
        seg_len = strlen(seg);
        if (seg_len) {
            if (*(seg) == '*') {
                ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL(pidents), key, zv) {
                    if (key) {
                        if (IS_STRING == Z_TYPE_P(zv)) {
                            smart_str_appendl(&wildcard, ZSTR_VAL(key) + 1, ZSTR_LEN(key) - 1);
                            smart_str_appendl(&wildcard, YAF_ROUTER_URL_DELIMIETER, 1);
                            smart_str_appendl(&wildcard, Z_STRVAL_P(zv), Z_STRLEN_P(zv));
                            smart_str_appendl(&wildcard, YAF_ROUTER_URL_DELIMIETER, 1);
                        }
                    }
                } ZEND_HASH_FOREACH_END();
                smart_str_0(&wildcard);
                inter = php_str_to_str(ZSTR_VAL(uri), ZSTR_LEN(uri),
                        "*", 1, ZSTR_VAL(wildcard.s), ZSTR_LEN(wildcard.s));    
                zend_string_release(uri);
                uri = inter;
                break;
            }

            if (*(seg) == ':') {
                if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), seg, seg_len)) != NULL) {
                    val = zval_get_string(zv);
                    inter = php_str_to_str(ZSTR_VAL(uri),
                            ZSTR_LEN(uri), seg, seg_len, ZSTR_VAL(val), ZSTR_LEN(val));
                    zend_string_release(val);
                    zend_string_release(uri);
                    zend_hash_str_del(Z_ARRVAL(pidents), seg, seg_len);
                    uri = inter;
                } 
            } 
        }
        seg = php_strtok_r(NULL, YAF_ROUTER_URL_DELIMIETER, &ptrptr);
    }
    
    smart_str_free(&wildcard);
    efree(pmatch);
    zval_ptr_dtor(&pidents);

    if (query && IS_ARRAY == Z_TYPE_P(query)) {
        HashTable *ht = Z_ARRVAL_P(query);

        smart_str_appendc(&query_str, '?');
        ZEND_HASH_FOREACH_STR_KEY_VAL(ht, key, zv) {
            if (key) {
                val = zval_get_string(zv);
                smart_str_appendl(&query_str, ZSTR_VAL(key), ZSTR_LEN(key));
                smart_str_appendc(&query_str, '=');
                smart_str_appendl(&query_str, ZSTR_VAL(val), ZSTR_LEN(val));
                smart_str_appendc(&query_str, '&');
                zend_string_release(val);
            }
        } ZEND_HASH_FOREACH_END();
    }

    if (query_str.s) {
        size_t orig_len = ZSTR_LEN(uri);
        ZSTR_LEN(query_str.s)--; /* get rid of the tail & */
        smart_str_0(&query_str);
        uri = zend_string_realloc(uri, ZSTR_LEN(uri) + ZSTR_LEN(query_str.s), 0); 
        memcpy(ZSTR_VAL(uri) + orig_len, ZSTR_VAL(query_str.s), ZSTR_LEN(query_str.s));
        ZSTR_VAL(uri)[ZSTR_LEN(uri)] = '\0';
        smart_str_free(&query_str);
    }   

    return uri;
}
/* }}} */

/** {{{ proto public Yaf_Route_Rest::match(string $uri)
 */
PHP_METHOD(yaf_route_rest, match) {
    zend_string *uri;
    zval matches;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &uri) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    if (ZSTR_LEN(uri) == 0) {
        RETURN_FALSE;
    }

    if (yaf_route_rest_match(getThis(), uri, &matches)) {
        RETURN_ZVAL(&matches, 0, 0);
    }

    RETURN_FALSE;
}
/** }}} */

/** {{{ proto public Yaf_Route_Rest::__construct(string $match, array $route, array $verify, string $reverse = NULL)
 */
PHP_METHOD(yaf_route_rest, __construct) {
    zval        *match, *route, *verify = NULL;
    yaf_route_t rself, *self = getThis();

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "za|a", &match, &route, &verify) ==  FAILURE) {
        return;
    }

    if (IS_STRING != Z_TYPE_P(match) || !Z_STRLEN_P(match)) {
        yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Expects a valid string match as the first parameter");
        RETURN_FALSE;
    }

    if (verify && IS_ARRAY != Z_TYPE_P(verify)) {
        yaf_trigger_error(YAF_ERR_TYPE_ERROR, "Expects an array as third parameter");
        RETURN_FALSE;
    }

    if (!self) {
        ZVAL_NULL(&rself);
        self = &rself;
    }

    (void)yaf_route_rest_instance(self, match, route, verify);

    if (self) {
        RETURN_ZVAL(self, 1, 0);
    }

    RETURN_FALSE;
}
/** }}} */

/** {{{ proto public Yaf_Route_rest::assemble(array $info[, array $query = NULL])
*/
PHP_METHOD(yaf_route_rest, assemble) {
    zval *info, *query = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|a", &info, &query) == FAILURE) {
        return;
    } else {
        zend_string *str;
        if ((str = yaf_route_rest_assemble(getThis(), info, query)) != NULL) {
            RETURN_STR(str);
        }
        RETURN_NULL();
    }
}
/* }}} */

/** {{{ yaf_route_rest_methods
 */
zend_function_entry yaf_route_rest_methods[] = {
    PHP_ME(yaf_route_rest, __construct, yaf_route_rest_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(yaf_route_rest, route, yaf_route_route_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(yaf_route_rest, assemble, yaf_route_assemble_arginfo, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
 */
YAF_STARTUP_FUNCTION(route_rest) {
    zend_class_entry ce;
    YAF_INIT_CLASS_ENTRY(ce, "Yaf_Route_Rest", "Yaf\\Route\\Rest", yaf_route_rest_methods);
    yaf_route_rest_ce = zend_register_internal_class_ex(&ce, NULL);
    zend_class_implements(yaf_route_rest_ce, 1, yaf_route_ce);
    yaf_route_rest_ce->ce_flags |= ZEND_ACC_FINAL;

    zend_declare_property_null(yaf_route_rest_ce, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_MATCH),  ZEND_ACC_PROTECTED);
    zend_declare_property_null(yaf_route_rest_ce, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_ROUTE),  ZEND_ACC_PROTECTED);
    zend_declare_property_null(yaf_route_rest_ce, ZEND_STRL(YAF_ROUTE_PROPETY_NAME_VERIFY), ZEND_ACC_PROTECTED);

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

