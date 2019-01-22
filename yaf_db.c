/*
  +----------------------------------------------------------------------+
  | yaf_db   /   yaf_db                                                |
  +----------------------------------------------------------------------+
  | yaf_db is a php database ORM extension for mysql written in c language |
  +----------------------------------------------------------------------+
  | Author: Cao Hao  <649947921@qq.com>                                  |
  | CreateTime: 2018-11-19                                               |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "Zend/zend_interfaces.h"

#include "php_yaf.h"
#include "yaf_exception.h"
#include "zend_variables.h"
#include "yaf_db.h"
#include <stdarg.h>
#include <ext/pcre/php_pcre.h>
#include <regex.h>

zend_class_entry yaf_db_ce;
zend_class_entry* yaf_db_ce_ptr;

//类的成员函数
static zend_function_entry yaf_db_methods[] = {
    PHP_ME(yaf_db, __construct, arginfo_yaf_db_construct_oo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(yaf_db, __destruct, arginfo_yaf_db_void, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(yaf_db, errorCode, arginfo_yaf_db_construct_oo, ZEND_ACC_PUBLIC)
    PHP_ME(yaf_db, errorInfo, arginfo_yaf_db_construct_oo, ZEND_ACC_PUBLIC)
    PHP_ME(yaf_db, select, arginfo_yaf_db_select_oo, ZEND_ACC_PUBLIC)
    PHP_ME(yaf_db, exec, arginfo_yaf_db_exec_oo, ZEND_ACC_PUBLIC)
    PHP_ME(yaf_db, query, arginfo_yaf_db_query_oo, ZEND_ACC_PUBLIC)
    PHP_ME(yaf_db, insert, arginfo_yaf_db_insert_oo, ZEND_ACC_PUBLIC)
    PHP_ME(yaf_db, replace, arginfo_yaf_db_replace_oo, ZEND_ACC_PUBLIC)
    PHP_ME(yaf_db, insert_id, arginfo_yaf_db_void, ZEND_ACC_PUBLIC)
    PHP_ME(yaf_db, update, arginfo_yaf_db_update_oo, ZEND_ACC_PUBLIC)
    PHP_ME(yaf_db, delete, arginfo_yaf_db_delete_oo, ZEND_ACC_PUBLIC)
    PHP_ME(yaf_db, initialize, arginfo_yaf_db_void, ZEND_ACC_PUBLIC)
    PHP_ME(yaf_db, begin, arginfo_yaf_db_void, ZEND_ACC_PUBLIC)
    PHP_ME(yaf_db, commit, arginfo_yaf_db_void, ZEND_ACC_PUBLIC)
    PHP_ME(yaf_db, rollback, arginfo_yaf_db_void, ZEND_ACC_PUBLIC)
    NULL, NULL, NULL
};


/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(db) {
    INIT_CLASS_ENTRY(yaf_db_ce, "yaf_db", yaf_db_methods);
    yaf_db_ce_ptr = zend_register_internal_class(&yaf_db_ce TSRMLS_CC);
    yaf_db_ce_ptr->serialize = zend_class_serialize_deny;
    yaf_db_ce_ptr->unserialize = zend_class_unserialize_deny;

    zend_declare_property_long(yaf_db_ce_ptr, ZEND_STRL("mapkey_index"), 0, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC);
    zend_declare_property_null(yaf_db_ce_ptr, ZEND_STRL("host"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(yaf_db_ce_ptr, ZEND_STRL("username"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(yaf_db_ce_ptr, ZEND_STRL("password"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(yaf_db_ce_ptr, ZEND_STRL("dbname"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_long(yaf_db_ce_ptr, ZEND_STRL("port"), 3306, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_string(yaf_db_ce_ptr, ZEND_STRL("charset"), "utf8", ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(yaf_db_ce_ptr, ZEND_STRL("option"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(yaf_db_ce_ptr, ZEND_STRL("errcode"),ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(yaf_db_ce_ptr, ZEND_STRL("errinfo"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(yaf_db_ce_ptr, ZEND_STRL("_pdo"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(yaf_db_ce_ptr, ZEND_STRL("cache"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_long(yaf_db_ce_ptr, ZEND_STRL("insert_id"), 0, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(yaf_db_ce_ptr, ZEND_STRL("unix_socket"), ZEND_ACC_PUBLIC TSRMLS_CC);

    return SUCCESS;
}
/* }}} */

//yaf_db构造函数
PHP_METHOD(yaf_db, __construct) {
    zval* thisObject = getThis();
    zval *option = NULL;
    zval *v;
    HashTable *vht;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &option) == FAILURE) {
        return;
    }

    if (Z_TYPE_P(option) != IS_ARRAY) {
        RETURN_FALSE;
    }

    vht = Z_ARRVAL_P(option);
    if ((v = php_yaf_array_get_value(vht, "host")) != NULL) {
        convert_to_string(v);
        zend_update_property_string(yaf_db_ce_ptr, thisObject, ZEND_STRL("host"), Z_STRVAL_P(v) TSRMLS_DC);
    }

    if ((v = php_yaf_array_get_value(vht, "username")) != NULL) {
        convert_to_string(v);
        zend_update_property_string(yaf_db_ce_ptr, thisObject, ZEND_STRL("username"), Z_STRVAL_P(v) TSRMLS_DC);
    }

    if ((v = php_yaf_array_get_value(vht, "password")) != NULL) {
        convert_to_string(v);
        zend_update_property_string(yaf_db_ce_ptr, thisObject, ZEND_STRL("password"), Z_STRVAL_P(v) TSRMLS_DC);
    }

    if ((v = php_yaf_array_get_value(vht, "dbname")) != NULL) {
        convert_to_string(v);
        zend_update_property_string(yaf_db_ce_ptr, thisObject, ZEND_STRL("dbname"), Z_STRVAL_P(v) TSRMLS_DC);
    }

    if ((v = php_yaf_array_get_value(vht, "port")) != NULL) {
        convert_to_long(v);
        zend_update_property_long(yaf_db_ce_ptr, thisObject, ZEND_STRL("port"), Z_LVAL_P(v) TSRMLS_DC);
    }

    if ((v = php_yaf_array_get_value(vht, "charset")) != NULL) {
        convert_to_string(v);
        zend_update_property_string(yaf_db_ce_ptr, thisObject, ZEND_STRL("charset"), Z_STRVAL_P(v) TSRMLS_DC);
    }

    if ((v = php_yaf_array_get_value(vht, "cache")) != NULL) {
        zend_update_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("cache"), v TSRMLS_DC);
    }
    
    if ((v = php_yaf_array_get_value(vht, "unix_socket")) != NULL) {
        convert_to_string(v);
        zend_update_property_string(yaf_db_ce_ptr, thisObject, ZEND_STRL("unix_socket"), Z_STRVAL_P(v) TSRMLS_DC);
    }

    if ((v = php_yaf_array_get_value(vht, "option")) != NULL) {
        zend_update_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("option"), v TSRMLS_DC);
    }

    update_error_info(thisObject, "00000", "");
}

PHP_METHOD(yaf_db, __destruct) {
}

PHP_METHOD(yaf_db, errorCode) {
    zval* errorCode = yaf_read_init_property(yaf_db_ce_ptr, getThis(), ZEND_STRL("errcode") TSRMLS_CC);
    RETURN_ZVAL(errorCode, 1, 0);
}

PHP_METHOD(yaf_db, errorInfo) {
    zval* errorInfo = yaf_read_init_property(yaf_db_ce_ptr, getThis(), ZEND_STRL("errinfo") TSRMLS_CC);
    RETURN_ZVAL(errorInfo, 1, 0);
}

PHP_METHOD(yaf_db, initialize) {
    zval* thisObject = getThis();
    
    zval* unix_socket = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("unix_socket") TSRMLS_CC);
	if(YAF_IS_NOT_NULL(unix_socket)) { ///// unix domain sockcet ////
		RETURN_TRUE;
	}
	
    zval* pdo = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("_pdo") TSRMLS_CC);

    if (YAF_IS_NULL(pdo)) {
        zval *dsn;
        zval **args[4];

        //参数
        zval* host = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("host") TSRMLS_CC);
        zval* username = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("username") TSRMLS_CC);
        zval* password = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("password") TSRMLS_CC);
        zval* dbname = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("dbname") TSRMLS_CC);
        zval* port = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("port") TSRMLS_CC);
        zval* charset = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("charset") TSRMLS_CC);
        zval* option = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("option") TSRMLS_CC);
        
        YAF_MAKE_STD_ZVAL(dsn);
        char str[128] = {0}; 
        sprintf(str, "mysql:host=%s;port=%d;dbname=%s", Z_STRVAL_P(host), Z_LVAL_P(port), Z_STRVAL_P(dbname)); 
        YAF_ZVAL_STRING(dsn, str, 1);
		
        args[0] = &dsn;
        args[1] = &username;
        args[2] = &password;
        args[3] = &option;
        
        //创建pdo, 连接数据库
        object_init_ex(pdo, get_pdo_ce());

        if (yaf_call_user_function_construct_fast(&pdo, 4, args) == FAILURE) {
            yaf_zval_ptr_dtor(&dsn);
            yaf_php_fatal_error(E_ERROR, "Fail to connect database by PDO");
            RETURN_FALSE;
        }

        if (EG(exception)) {
            yaf_zval_ptr_dtor(&dsn);
            RETURN_FALSE;
        }

        yaf_zval_ptr_dtor(&dsn);

        zend_update_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("_pdo"), pdo TSRMLS_CC);

        //设置字符集
        zval *charset_sql;
        zval** exec_args[1];
		
        YAF_MAKE_STD_ZVAL(charset_sql);
        char str2[128] = {0}; 
        sprintf(str2, "SET NAMES %s", Z_STRVAL_P(charset)); 
        YAF_ZVAL_STRING(charset_sql, str2, 1);
		
        exec_args[0] = &charset_sql;

        int setret = yaf_call_user_function_return_bool_or_unsigned(&pdo, "exec", 1, exec_args);

        if (setret == FAILURE) {
            yaf_zval_ptr_dtor(&charset_sql);
            yaf_php_fatal_error(E_WARNING, "failed to set database charset.");
            RETURN_FALSE;
        }

        if (EG(exception)) {
            yaf_zval_ptr_dtor(&charset_sql);
            RETURN_FALSE;
        }

        yaf_zval_ptr_dtor(&charset_sql);
    }

    RETURN_TRUE;
}

//事务开始
PHP_METHOD(yaf_db, begin) {
    zval* thisObject = getThis();
	
	zval* unix_socket = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("unix_socket") TSRMLS_CC);
	if(YAF_IS_NOT_NULL(unix_socket)) { ///// unix domain sockcet ////
		RETURN_MY_ERROR("unix domain socket not support transaction");
	}
		
    zval* pdo = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("_pdo") TSRMLS_CC);
    if (YAF_IS_NULL(pdo)) {
        RETURN_MY_ERROR("pdo is empty");
    }

    if (yaf_call_user_function_return_bool_or_unsigned(&pdo, "beginTransaction", 0, NULL) == 1) {
        RETURN_TRUE;
    }

    RETURN_FALSE;
}

//事务提交
PHP_METHOD(yaf_db, commit) {
    zval* thisObject = getThis();
	
	zval* unix_socket = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("unix_socket") TSRMLS_CC);
	if(YAF_IS_NOT_NULL(unix_socket)) { ///// unix domain sockcet ////
		RETURN_MY_ERROR("unix domain socket not support transaction");
	}
	
    zval* pdo = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("_pdo") TSRMLS_CC);
    if (YAF_IS_NULL(pdo)) {
        RETURN_MY_ERROR("pdo is empty");
    }

    if (yaf_call_user_function_return_bool_or_unsigned(&pdo, "commit", 0, NULL) == 1) {
        RETURN_TRUE;
    }

    RETURN_FALSE;
}


//事务回滚
PHP_METHOD(yaf_db, rollback) {
    zval* thisObject = getThis();
	
	zval* unix_socket = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("unix_socket") TSRMLS_CC);
	if(YAF_IS_NOT_NULL(unix_socket)) { ///// unix domain sockcet ////
		RETURN_MY_ERROR("unix domain socket not support transaction");
	}
	
    zval* pdo = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("_pdo") TSRMLS_CC);
    if (YAF_IS_NULL(pdo)) {
        RETURN_MY_ERROR("pdo is empty");
    }
	
    if (yaf_call_user_function_return_bool_or_unsigned(&pdo, "rollBack", 0, NULL) == 1) {
        RETURN_TRUE;
    }

    RETURN_FALSE;
}

//执行查询
PHP_METHOD(yaf_db, exec) {
    zval* thisObject = getThis();
    zval *query = NULL, *map = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &query, &map) == FAILURE) {
        RETURN_LONG(-1);
    }

    if (Z_TYPE_P(query) != IS_STRING) {
        RETURN_MY_ERROR("Argument 1 passed must be of the type string");
    }

    //初始化错误
    update_error_info(thisObject, "00000", "");

    //判断是否数据库 WRITE 写操作，或者 SELECT 查询
    int is_write = is_write_type(Z_STRVAL_P(query));
    int isinsert = is_insert(Z_STRVAL_P(query));
    
    //unix socket
    zval* unix_socket = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("unix_socket") TSRMLS_CC);
    
    if(YAF_IS_NOT_NULL(unix_socket)) { ////////////////// unix socket ////////////////////////
        php_stream* stream = unix_socket_conn(Z_STRVAL_P(unix_socket));
        
        if(stream == NULL) {
        	update_error_info(thisObject, "E0001", "can`t connect to unix socket");
        	RETURN_MY_ERROR("[exception] can`t connect to unix socket");
        }
        
        //发送请求
        size_t buf_size = 10 + Z_STRLEN_P(query) + 1;
        char* send_buf = (char*) malloc(buf_size);
        memset(send_buf, 0, buf_size);
      	sprintf(send_buf, "%d\n%s", Z_STRLEN_P(query), Z_STRVAL_P(query));
      	
        size_t send_len = php_stream_write(stream, send_buf, strlen(send_buf));
        free(send_buf);
        
        if(send_len <= 0) {
        	php_stream_close(stream);
        	RETURN_MY_ERROR("[exception] unixsocket send failed");
        }
        
        //发送 map
        if (YAF_IS_NOT_NULL(map) && Z_TYPE_P(map) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(map))) {
        	smart_str smart_map_buf = {0};
        	php_json_encode_ex(&smart_map_buf, map);
        	smart_str_0(&smart_map_buf);
        	
        	if(smart_map_buf.s != NULL && ZSTR_VAL(smart_map_buf.s) != NULL && ZSTR_LEN(smart_map_buf.s) > 0) {
        	    char* map_str = ZSTR_VAL(smart_map_buf.s);
        	    int smart_map_buf_len = strlen(map_str);
        		size_t map_buf_size = 10 + smart_map_buf_len + 2;
	        	char* map_buf = (char*)malloc(map_buf_size);
	        	memset(map_buf, 0, map_buf_size);
	        	
	        	if(map_str[0] == '{' && map_str[smart_map_buf_len-1] != '}') {  //php_json_encode_ex bug end with not '}'
	        	    sprintf(map_buf, "%d\n%s}", smart_map_buf_len+1, map_str);
	        	} else if(map_str[0] == '[' && map_str[smart_map_buf_len-1] != ']') {  //php_json_encode_ex bug end with not ']'
	        	    sprintf(map_buf, "%d\n%s]", smart_map_buf_len+1, map_str);
	        	} else {
      			    sprintf(map_buf, "%d\n%s", smart_map_buf_len, map_str);
      			}

      			php_stream_write(stream, map_buf, strlen(map_buf));
        		free(map_buf);
        	} else {
        		php_stream_write(stream, "0\n", strlen("0\n"));
        	}
        	
        	smart_str_free(&smart_map_buf);
        } else {
        	php_stream_write(stream, "0\n", strlen("0\n"));
        }
        
        //获取结果数据长度
        char recv_len_buf[10] = {0};
        size_t recv_len = 0;
        php_stream_get_line(stream, recv_len_buf, 10, &recv_len);
        if(recv_len <= 0) {
        	php_stream_close(stream);
        	RETURN_MY_ERROR("[exception] unixsocket receive length failed");
        }
        
       	recv_len_buf[recv_len - 1] = 0;
        recv_len = atoi(recv_len_buf) + 1;
        
        //获取结果数据
        char* recv_buf = (char*) malloc(recv_len);
        size_t len;
        memset(recv_buf, 0, recv_len);
        
        size_t p = 0;
        char * ret = NULL;
        do {
        	ret = php_stream_get_line(stream, recv_buf + p, recv_len, &len);
        	recv_len -= len;
        	p += len;
        } while(ret != NULL && recv_len > 0);
        
        //关闭 unix socket
        php_stream_close(stream);
        
        //string_debug("recv_buf", recv_buf);
        if(recv_buf[0] == 0) { 
        	free(recv_buf);
        	RETURN_MY_ERROR("[exception] receive buff failed");
        }
        
        //返回结果转化为json
        zval* recv_array = NULL;
        YAF_MAKE_STD_ZVAL(recv_array);
        
        php_json_decode_ex(recv_array, recv_buf, strlen(recv_buf), 1);
       	free(recv_buf);
       	
       	if(YAF_IS_NULL(recv_array) || YAF_IS_NOT_ARRAY(recv_array)) {
       		yaf_zval_ptr_dtor(&recv_array);
       		update_error_info(thisObject, "E0001", "convert recvbuff from json to array erro");
       		RETURN_MY_ERROR("[exception] convert recvbuff from json to array erro");
       	}
        
        //查询返回错误码
        zval* error_no = php_yaf_array_get_value(Z_ARRVAL_P(recv_array), "errno");
        if(YAF_IS_NULL(error_no) || Z_TYPE_P(error_no) != IS_LONG || Z_LVAL_P(error_no) != 0) { //返回错误信息
        	zval* errorCode = php_yaf_array_get_value(Z_ARRVAL_P(recv_array), "errorCode");
        	zval* errorInfo = php_yaf_array_get_value(Z_ARRVAL_P(recv_array), "errorInfo");
        	update_pdo_error(thisObject, errorCode, errorInfo);
			yaf_zval_ptr_dtor(&recv_array);
			RETURN_LONG(-1);
        }
        
		zval* data = php_yaf_array_get_value(Z_ARRVAL_P(recv_array), "data");
        
        if(is_write) {
        	zval* return_info = NULL;
        	if(isinsert) {
        		return_info = php_yaf_array_get_value(Z_ARRVAL_P(data), "insert_id");
        	} else {
        		return_info = php_yaf_array_get_value(Z_ARRVAL_P(data), "affected_rows");
        	}
        	
        	int return_int = Z_LVAL_P(return_info);
        	
			yaf_zval_ptr_dtor(&recv_array);
            RETURN_LONG(return_int);
        } else {
        	//是否单列
		    int ret_count = zend_hash_num_elements(Z_ARRVAL_P(data));
		    
		    if(ret_count > 0) {
		    	zval* first_element = yaf_get_element_by_hashtable_index(Z_ARRVAL_P(data), 0);
		    	ret_count = zend_hash_num_elements(Z_ARRVAL_P(first_element));
		    	
		    	if(ret_count == 1) { //单列
		    		zval *single_column_result = NULL;
					YAF_MAKE_STD_ZVAL(single_column_result);
		    		yaf_array_single_columns(&single_column_result, data);
				    yaf_zval_ptr_dtor(&recv_array);
		        	RETURN_ZVAL(single_column_result, 0, 0);
		    	}
		    }
        	
		    zval *copy = yaf_zval_copy(data);
		    yaf_zval_ptr_dtor(&recv_array);
        
        	RETURN_ZVAL(copy, 0, 0);
        }
        
    } else { /////////////////////////// pdo /////////////////////////////////
        zval* pdo = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("_pdo") TSRMLS_CC);
        if (YAF_IS_NULL(pdo)) {
            RETURN_MY_ERROR("pdo is empty");
        }
    
        //prepare query
        zval** prepare_args[1];
        zval* statement = NULL;
    
        prepare_args[0] = &query;
    
        if (yaf_call_user_function_ex_fast(&pdo, "prepare", &statement, 1, prepare_args) == FAILURE) {
            yaf_zval_free(statement);
            yaf_php_fatal_error(E_ERROR, "failed to prepare query");
            RETURN_LONG(-1);
        }
    
        if (EG(exception) || YAF_IS_NULL(statement)) {
            yaf_zval_free(statement);
            RETURN_MY_ERROR("failed to prepare query, pdo is not initialized");
        }
    
        //bind value
        if (YAF_IS_NOT_NULL(map) && Z_TYPE_P(map) == IS_ARRAY) {
            char * key;
            zval *value;
            uint32_t key_len;
            int key_type;
    
            YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(map), key, key_len, key_type, value)
            if (HASH_KEY_IS_STRING != key_type) {
                continue;
            }
    
            zval** bind_args[3];
            zval *bind_key = NULL, *bind_type = NULL;
    
            YAF_MAKE_STD_ZVAL(bind_key);
            YAF_MAKE_STD_ZVAL(bind_type);
            YAF_ZVAL_STRING(bind_key, key, 1);
    
            bind_args[0] = &bind_key;
            bind_args[1] = &value;
    
            //绑定类型
            switch (Z_TYPE_P(value)) {
            case IS_NULL:
                ZVAL_LONG(bind_type, PDO_PARAM_NULL);
                break;
            case IS_LONG:
                ZVAL_LONG(bind_type, PDO_PARAM_INT);
                break;
            case IS_DOUBLE:
                ZVAL_LONG(bind_type, PDO_PARAM_STR);
                break;
    #if PHP_MAJOR_VERSION < 7 /* PHP Version 5 */
            case IS_BOOL:
    #else /* PHP Version 7 */
            case IS_TRUE:
            case IS_FALSE:
    #endif
                ZVAL_LONG(bind_type, PDO_PARAM_BOOL);
                break;
            case IS_STRING:
                //if (is_numeric_string(Z_STRVAL_P(value), Z_STRLEN_P(value), NULL, NULL, 0)) { //当varchar类型的数字 where update为字符串的时候，数据库报错。
                //    ZVAL_LONG(bind_type, PDO_PARAM_INT);
                //} else {
                ZVAL_LONG(bind_type, PDO_PARAM_STR);
                //}
                break;
            default:
                break;
            }
    
            bind_args[2] = &bind_type;
    
            int ret = yaf_call_user_function_return_bool_or_unsigned(&statement, "bindValue", 3, bind_args);
            yaf_zval_ptr_dtor(&bind_key);
            yaf_zval_ptr_dtor(&bind_type);
    
            if (ret == FAILURE) {
                yaf_zval_free(statement);
                yaf_php_fatal_error(E_ERROR, "failed to bind value");
                RETURN_LONG(-1);
            }
    
            YAF_HASHTABLE_FOREACH_END();
        }
    
        //execute
        if (yaf_call_user_function_return_bool_or_unsigned(&statement, "execute", 0, NULL) == FAILURE) {
            yaf_zval_free(statement);
            yaf_php_fatal_error(E_ERROR, "failed to execute sql");
            RETURN_LONG(-1);
        }
    
        if (EG(exception)) {
            yaf_zval_free(statement);
            RETURN_MY_ERROR("[exception] failed to execute sql");
        }
    
        //获取查询 error 信息
        zval* errorCode = NULL;
        zval* errorInfo = NULL;
    
        yaf_call_user_function_ex_fast(&statement, "errorCode", &errorCode, 0, NULL);
        yaf_call_user_function_ex_fast(&statement, "errorInfo", &errorInfo, 0, NULL);
    
        update_pdo_error(thisObject, errorCode, errorInfo);
    	
        if (YAF_IS_NULL(errorCode) || strcmp(Z_STRVAL_P(errorCode), "00000") != 0) {
            yaf_zval_free(statement);
            yaf_zval_free(errorCode);
            yaf_zval_free(errorInfo);
            RETURN_LONG(-1);
        }
    
        yaf_zval_free(errorCode);
        yaf_zval_free(errorInfo);
    
        if (is_write) {
        	if(isinsert) {
        		zval* insertid = NULL;
        		yaf_zval_free(statement);
        		
        		if (yaf_call_user_function_ex_fast(&pdo, "lastInsertId", &insertid, 0, NULL) == FAILURE) {
			        yaf_zval_free(insertid);
			        yaf_php_fatal_error(E_ERROR, "failed to get lastInsertId");
			        RETURN_LONG(-1);
			    }
			
			    if (EG(exception) || YAF_IS_NULL(insertid)) {
			        yaf_zval_free(insertid);
			        RETURN_MY_ERROR("failed to get lastInsertId, pdo is not initialized");
			    }
			
			    RETVAL_ZVAL(insertid, 1, 1);
			    efree(insertid);
        	} else {
            	int row_count = yaf_call_user_function_return_bool_or_unsigned(&statement, "rowCount", 0, NULL);
           	 	yaf_zval_free(statement);
            	RETURN_LONG(row_count);
        	}
        } else {
            RETVAL_ZVAL(statement, 1, 1);
            efree(statement);
        }
    }
}

//原生查询query
PHP_METHOD(yaf_db, query) {
    zval* thisObject = getThis();
    zval *z_sql = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &z_sql) == FAILURE) {
        RETURN_LONG(-1);
    }

    if (Z_TYPE_P(z_sql) != IS_STRING) {
        RETURN_MY_ERROR("Argument 1 passed must be of the type string");
    }

    //执行SQL
    zval *statement = NULL;
    zval **exec_args[1];
    exec_args[0] = &z_sql;

    if (yaf_call_user_function_ex_fast(&thisObject, "exec", &statement, 1, exec_args) == FAILURE) {
        yaf_zval_free(statement);
        RETURN_LONG(-1);
    }
    
	//unix socket
    zval* unix_socket = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("unix_socket") TSRMLS_CC);
    if(YAF_IS_NOT_NULL(unix_socket)) { // unix socket 
    	if(YAF_IS_NULL(statement)) {
        	yaf_zval_free(statement);
        	RETURN_LONG(-1);
    	}
    	
    	zval *copy = yaf_zval_copy(statement);
    	yaf_zval_free(statement);
		RETURN_ZVAL(copy, 0, 0);
    } else if (YAF_IS_NOT_NULL(statement) && Z_TYPE_P(statement) == IS_OBJECT) { //获取结果
        zval **fetch_args[1];
        zval *result = NULL, *fetch_type = NULL;

        YAF_MAKE_STD_ZVAL(fetch_type);
        ZVAL_LONG(fetch_type, PDO_FETCH_ASSOC);

        fetch_args[0] = &fetch_type;

        if (yaf_call_user_function_ex_fast(&statement, "fetchAll", &result, 1, fetch_args) == FAILURE) {
            yaf_zval_free(statement);
            yaf_zval_free(result);
            yaf_zval_ptr_dtor(&fetch_type);
            RETURN_LONG(-1);
        }

        yaf_zval_free(statement);
        yaf_zval_ptr_dtor(&fetch_type);

        RETVAL_ZVAL(result, 1, 1);
        efree(result);
    } else {
        yaf_zval_free(statement);
        RETURN_LONG(-1);
    }
}

PHP_METHOD(yaf_db, insert_id) {
    zval *thisObject = getThis();

    //初始化错误
    update_error_info(thisObject, "00000", "");
	
	zval* unix_socket = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("unix_socket") TSRMLS_CC);
	if(YAF_IS_NOT_NULL(unix_socket)) { ///// unix domain sockcet ////
		zval* insert_id = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("insert_id") TSRMLS_CC);
		RETVAL_ZVAL(insert_id, 1, 0);
	} else {
	    zval* pdo = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("_pdo") TSRMLS_CC);
	    if (YAF_IS_NULL(pdo)) {
	        RETURN_MY_ERROR("pdo is empty");
	    }
	
	    //exec
	    zval* insertid = NULL;
	    if (yaf_call_user_function_ex_fast(&pdo, "lastInsertId", &insertid, 0, NULL) == FAILURE) {
	        yaf_zval_free(insertid);
	        yaf_php_fatal_error(E_ERROR, "failed to get lastInsertId");
	        RETURN_LONG(-1);
	    }
	
	    if (EG(exception) || YAF_IS_NULL(insertid)) {
	        yaf_zval_free(insertid);
	        RETURN_MY_ERROR("failed to get lastInsertId, pdo is not initialized");
	    }
	
	    RETVAL_ZVAL(insertid, 1, 1);
	    efree(insertid);
	}
}

//insert 插入
PHP_METHOD(yaf_db, insert) {
    char *table = NULL;
    zval *thisObject = getThis();
    zend_size_t table_len;
    zval *data = NULL, *cache_info = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|z", &table, &table_len, &data, &cache_info) == FAILURE) {
        RETURN_LONG(-1);
    }
	
	//删除缓存
    if(YAF_IS_NOT_NULL(cache_info) && YAF_IS_ARRAY(cache_info)) {
        zval* cache_obj = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("cache") TSRMLS_CC);
        del_cache(cache_obj, cache_info);
    }
    
    //解析data数据
    char *insert_sql, *insert_keys,*insert_value;
    char *key;
    zval *value;
    uint32_t key_len;
    int key_type;
    char longval[MAP_ITOA_INT_SIZE], doubleval[32];

    yaf_string_emalloc_32(&insert_sql, 0);
    yaf_string_emalloc_32(&insert_keys, 0);
    yaf_string_emalloc_32(&insert_value, 0);

    YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(data), key, key_len, key_type, value)
    if (HASH_KEY_IS_STRING != key_type) {
        yaf_string_efree_32(insert_keys);
        yaf_string_efree_32(insert_value);
        yaf_string_efree_32(insert_sql);
        RETURN_MY_ERROR("input data must be key/value hash, not index array.");
    } else {
        yaf_multi_memcpy_auto_realloc(&insert_keys, 3, "`", key, "`,");

        switch (Z_TYPE_P(value)) {
        case IS_NULL:
            yaf_multi_memcpy_auto_realloc(&insert_value, 1, "NULL,");
            break;
        case IS_ARRAY:
            yaf_multi_memcpy_auto_realloc(&insert_value, 1, "ARRAY,");
            break;
#if PHP_MAJOR_VERSION < 7 /* PHP Version 5 */
        case IS_BOOL:
            if (Z_BVAL_P(value)) {
                yaf_multi_memcpy_auto_realloc(&insert_value, 1, "1,");
            } else {
                yaf_multi_memcpy_auto_realloc(&insert_value, 1, "0,");
            }
            break;
#else /* PHP Version 7 */
        case IS_TRUE:
            yaf_multi_memcpy_auto_realloc(&insert_value, 1, "1,");
            break;
        case IS_FALSE:
            yaf_multi_memcpy_auto_realloc(&insert_value, 1, "0,");
            break;
#endif
        case IS_LONG:
            yaf_itoa(Z_LVAL_P(value), longval);
            yaf_multi_memcpy_auto_realloc(&insert_value, 2, longval, ",");
            break;

        case IS_DOUBLE:
            sprintf(doubleval, "%g", Z_DVAL_P(value));
            yaf_multi_memcpy_auto_realloc(&insert_value, 2, doubleval, ",");
            break;
        case IS_STRING:
            yaf_multi_memcpy_auto_realloc(&insert_value, 3, "'", Z_STRVAL_P(value), "',");
            break;
        }

    }
    YAF_HASHTABLE_FOREACH_END();

    rtrim_str(insert_keys, ",");
    rtrim_str(insert_value, ",");
    yaf_multi_memcpy_auto_realloc(&insert_sql, 7, "INSERT INTO `", table, "` (", insert_keys ,") values (", insert_value, ")");
    yaf_string_efree_32(insert_keys);
    yaf_string_efree_32(insert_value);

    //执行SQL
    zval *result = NULL, *z_sql = NULL;
    zval **exec_args[1];

    YAF_MAKE_STD_ZVAL(z_sql);
    YAF_ZVAL_STRING(z_sql, insert_sql, 1);
    yaf_string_efree_32(insert_sql);
    exec_args[0] = &z_sql;

    if (yaf_call_user_function_ex_fast(&thisObject, "exec", &result, 1, exec_args) == FAILURE) {
        yaf_zval_ptr_dtor(&z_sql);
        yaf_zval_free(result);
        RETURN_LONG(-1);
    }

    yaf_zval_ptr_dtor(&z_sql);
    RETVAL_ZVAL(result, 1, 1);
    efree(result);
}

//replace 替换
PHP_METHOD(yaf_db, replace) {
    char *table = NULL;
    zval *thisObject = getThis();
    zend_size_t table_len;
    zval *data = NULL, *cache_info = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|z", &table, &table_len, &data, &cache_info) == FAILURE) {
        RETURN_LONG(-1);
    }
	
	//删除缓存
    if(YAF_IS_NOT_NULL(cache_info) && YAF_IS_ARRAY(cache_info)) {
        zval* cache_obj = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("cache") TSRMLS_CC);
        del_cache(cache_obj, cache_info);
    }
    
    //解析data数据
    char *insert_sql, *insert_keys,*insert_value;
    char *key;
    zval *value;
    uint32_t key_len;
    int key_type;
    char longval[MAP_ITOA_INT_SIZE], doubleval[32];

    yaf_string_emalloc_32(&insert_sql, 0);
    yaf_string_emalloc_32(&insert_keys, 0);
    yaf_string_emalloc_32(&insert_value, 0);

    YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(data), key, key_len, key_type, value)
    if (HASH_KEY_IS_STRING != key_type) {
        yaf_string_efree_32(insert_keys);
        yaf_string_efree_32(insert_value);
        yaf_string_efree_32(insert_sql);
        RETURN_MY_ERROR("input data must be key/value hash, not index array.");
    } else {
        yaf_multi_memcpy_auto_realloc(&insert_keys, 3, "`", key, "`,");

        switch (Z_TYPE_P(value)) {
        case IS_NULL:
            yaf_multi_memcpy_auto_realloc(&insert_value, 1, "NULL,");
            break;
        case IS_ARRAY:
            yaf_multi_memcpy_auto_realloc(&insert_value, 1, "ARRAY,");
            break;
#if PHP_MAJOR_VERSION < 7 /* PHP Version 5 */
        case IS_BOOL:
            if (Z_BVAL_P(value)) {
                yaf_multi_memcpy_auto_realloc(&insert_value, 1, "1,");
            } else {
                yaf_multi_memcpy_auto_realloc(&insert_value, 1, "0,");
            }
            break;
#else /* PHP Version 7 */
        case IS_TRUE:
            yaf_multi_memcpy_auto_realloc(&insert_value, 1, "1,");
            break;
        case IS_FALSE:
            yaf_multi_memcpy_auto_realloc(&insert_value, 1, "0,");
            break;
#endif
        case IS_LONG:
            yaf_itoa(Z_LVAL_P(value), longval);
            yaf_multi_memcpy_auto_realloc(&insert_value, 2, longval, ",");
            break;

        case IS_DOUBLE:
            sprintf(doubleval, "%g", Z_DVAL_P(value));
            yaf_multi_memcpy_auto_realloc(&insert_value, 2, doubleval, ",");
            break;
        case IS_STRING:
            yaf_multi_memcpy_auto_realloc(&insert_value, 3, "'", Z_STRVAL_P(value), "',");
            break;
        }

    }
    YAF_HASHTABLE_FOREACH_END();

    rtrim_str(insert_keys, ",");
    rtrim_str(insert_value, ",");
    yaf_multi_memcpy_auto_realloc(&insert_sql, 7, "REPLACE INTO `", table, "` (", insert_keys ,") values (", insert_value, ")");
    yaf_string_efree_32(insert_keys);
    yaf_string_efree_32(insert_value);

    //执行SQL
    zval *result = NULL, *z_sql = NULL;
    zval **exec_args[1];

    YAF_MAKE_STD_ZVAL(z_sql);
    YAF_ZVAL_STRING(z_sql, insert_sql, 1);
    yaf_string_efree_32(insert_sql);
    exec_args[0] = &z_sql;

    if (yaf_call_user_function_ex_fast(&thisObject, "exec", &result, 1, exec_args) == FAILURE) {
        yaf_zval_ptr_dtor(&z_sql);
        yaf_zval_free(result);
        RETURN_LONG(-1);
    }

    yaf_zval_ptr_dtor(&z_sql);
    RETVAL_ZVAL(result, 1, 1);
    efree(result);
}

//update 更新
PHP_METHOD(yaf_db, update) {
    char *table = NULL;
    zval *thisObject = getThis();
    zend_size_t table_len;
    zval *data = NULL, *where = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|z", &table, &table_len, &data, &where) == FAILURE) {
        RETURN_LONG(-1);
    }

    //更新语句
    char *update_sql;
    yaf_string_emalloc_32(&update_sql, 0);

    char *update_datas;
    char *key;
    zval *value;
    uint32_t key_len;
    int key_type;
    char longval[MAP_ITOA_INT_SIZE], doubleval[32];

    yaf_string_emalloc_32(&update_datas, 0);

    YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(data), key, key_len, key_type, value)
    if (HASH_KEY_IS_STRING != key_type) {
        yaf_string_efree_32(update_datas);
        yaf_string_efree_32(update_sql);
        RETURN_MY_ERROR("input data must be key/value hash, not index array.");
    } else {
        yaf_multi_memcpy_auto_realloc(&update_datas, 3, "`", key, "` = ");

        switch (Z_TYPE_P(value)) {
        case IS_NULL:
            yaf_multi_memcpy_auto_realloc(&update_datas, 1, "NULL,");
            break;
        case IS_ARRAY:
            yaf_multi_memcpy_auto_realloc(&update_datas, 1, "ARRAY,");
            break;
#if PHP_MAJOR_VERSION < 7 /* PHP Version 5 */
        case IS_BOOL:
            if (Z_BVAL_P(value)) {
                yaf_multi_memcpy_auto_realloc(&update_datas, 1, "1,");
            } else {
                yaf_multi_memcpy_auto_realloc(&update_datas, 1, "0,");
            }
            break;
#else /* PHP Version 7 */
        case IS_TRUE:
            yaf_multi_memcpy_auto_realloc(&update_datas, 1, "1,");
            break;
        case IS_FALSE:
            yaf_multi_memcpy_auto_realloc(&update_datas, 1, "0,");
            break;
#endif
        case IS_LONG:
            yaf_itoa(Z_LVAL_P(value), longval);
            yaf_multi_memcpy_auto_realloc(&update_datas, 2, longval, ",");
            break;

        case IS_DOUBLE:
            sprintf(doubleval, "%g", Z_DVAL_P(value));
            yaf_multi_memcpy_auto_realloc(&update_datas, 2, doubleval, ",");
            break;
        case IS_STRING:
            yaf_multi_memcpy_auto_realloc(&update_datas, 3, "'", Z_STRVAL_P(value), "',");
            break;
        }

    }
    YAF_HASHTABLE_FOREACH_END();

    rtrim_str(update_datas, ",");
    yaf_multi_memcpy_auto_realloc(&update_sql, 4, "UPDATE `", table, "` SET ", update_datas);
    yaf_string_efree_32(update_datas);
    
    //where条件
    zval *map, *cache_info = NULL;
    YAF_MAKE_STD_ZVAL(map);
    array_init(map);
		
    where_clause(where, map, & update_sql, &cache_info);
    
    //删除缓存
    if(YAF_IS_NOT_NULL(cache_info) && YAF_IS_ARRAY(cache_info)) {
        zval* cache_obj = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("cache") TSRMLS_CC);
        del_cache(cache_obj, cache_info);
    }

    //执行 SQL 语句
    zval *z_sql = NULL, *result = NULL;
    zval **exec_args[2];

    YAF_MAKE_STD_ZVAL(z_sql);
    YAF_ZVAL_STRING(z_sql, update_sql, 1);
    yaf_string_efree_32(update_sql);

    exec_args[0] = &z_sql;
    exec_args[1] = &map;

    if (yaf_call_user_function_ex_fast(&thisObject, "exec", &result, 2, exec_args) == FAILURE) {
        yaf_zval_ptr_dtor(&map);
        yaf_zval_ptr_dtor(&z_sql);
        yaf_zval_free(result);
        RETURN_LONG(-1);
    }

    yaf_zval_ptr_dtor(&map);
    yaf_zval_ptr_dtor(&z_sql);
    RETVAL_ZVAL(result, 1, 1);
    efree(result);
}


//delete 删除
PHP_METHOD(yaf_db, delete) {
    char *table = NULL;
    zval *thisObject = getThis();
    zend_size_t table_len;
    zval *where = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z", &table, &table_len, &where) == FAILURE) {
        RETURN_LONG(-1);
    }

    //更新语句
    char *delete_sql;
    yaf_string_emalloc_32(&delete_sql, 0);
    yaf_multi_memcpy_auto_realloc(&delete_sql, 3, "DELETE FROM `", table, "` ");

	//cache 缓存
	zval *cache_info = NULL;
	
    //where条件
    zval *map;
    YAF_MAKE_STD_ZVAL(map);
    array_init(map);

    where_clause(where, map, & delete_sql, &cache_info);
    
    //删除缓存
    if(YAF_IS_NOT_NULL(cache_info) && YAF_IS_ARRAY(cache_info)) {
        zval* cache_obj = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("cache") TSRMLS_CC);
        del_cache(cache_obj, cache_info);
    }

    //执行 SQL 语句
    zval *z_sql = NULL, *result = NULL;
    zval **exec_args[2];

    YAF_MAKE_STD_ZVAL(z_sql);
    YAF_ZVAL_STRING(z_sql, delete_sql, 1);
    yaf_string_efree_32(delete_sql);

    exec_args[0] = &z_sql;
    exec_args[1] = &map;

    if (yaf_call_user_function_ex_fast(&thisObject, "exec", &result, 2, exec_args) == FAILURE) {
        yaf_zval_ptr_dtor(&map);
        yaf_zval_ptr_dtor(&z_sql);
        yaf_zval_free(result);
        RETURN_LONG(-1);
    }

    yaf_zval_ptr_dtor(&map);
    yaf_zval_ptr_dtor(&z_sql);
    RETVAL_ZVAL(result, 1, 1);
    efree(result);
}

//select 查询
PHP_METHOD(yaf_db, select) {
    char* table = NULL;
    zval* thisObject = getThis();
    zend_size_t table_len;
    zval* join = NULL, *columns = NULL, *where = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|zz", &table, &table_len, &join, &columns, &where) == FAILURE) {
        RETURN_LONG(-1);
    }

    //是否查询单个列，当 where 为空的时候， 判断 join 是不是 "*"，否则判断 columns 是不是 "*"
    int is_single_column = 0;
    if ((YAF_IS_NULL(where) && Z_TYPE_P(join) == IS_STRING && strcmp(Z_STRVAL_P(join), "*") != 0 && yaf_strpos(Z_STRVAL_P(join), ",") < 0)
            ||(YAF_IS_NOT_NULL(where) && Z_TYPE_P(columns) == IS_STRING && strcmp(Z_STRVAL_P(columns), "*") != 0) && yaf_strpos(Z_STRVAL_P(join), ",") < 0) {
        is_single_column = 1;
    }

    //查询语句初始化
    char *sql;
    zval *map;
    zval *cache_info = NULL, *cache_obj = NULL, *cache_key = NULL;
    
    YAF_MAKE_STD_ZVAL(map);
    array_init(map);
    yaf_string_emalloc_32(&sql, 0);
    
    select_context(table, map, join, columns, where, &sql, &cache_info);
    
    //缓存获取数据
    if(YAF_IS_NOT_NULL(cache_info) && YAF_IS_ARRAY(cache_info)) {
        cache_obj = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("cache") TSRMLS_CC);
        if (YAF_IS_NOT_NULL(cache_obj)) {
            cache_key = php_yaf_array_get_value(Z_ARRVAL_P(cache_info), "key");
            if(YAF_IS_NOT_NULL(cache_key)) {
                zval* cache_ret = get_cache(cache_obj, cache_key);
                
                if(cache_ret != NULL) {
                    RETVAL_ZVAL(cache_ret, 1, 1);
                    efree(cache_ret);
                    yaf_string_efree_32(sql);
                    yaf_zval_ptr_dtor(&map);
                    return;
                }
            }
        }
    }
    
    //执行SQL
    zval *z_sql = NULL, *statement = NULL;
    zval **exec_args[2];
    
    YAF_MAKE_STD_ZVAL(z_sql);
    YAF_ZVAL_STRING(z_sql, sql, 1);
    yaf_string_efree_32(sql);

    exec_args[0] = &z_sql;
    exec_args[1] = &map;

    if (yaf_call_user_function_ex_fast(&thisObject, "exec", &statement, 2, exec_args) == FAILURE) {
        yaf_zval_ptr_dtor(&map);
        yaf_zval_ptr_dtor(&z_sql);
        yaf_zval_free(statement);
        RETURN_LONG(-1);
    }

    yaf_zval_ptr_dtor(&map);
    yaf_zval_ptr_dtor(&z_sql);
	
	//unix socket
    zval* unix_socket = yaf_read_init_property(yaf_db_ce_ptr, thisObject, ZEND_STRL("unix_socket") TSRMLS_CC);
    if(YAF_IS_NOT_NULL(unix_socket)) { // unix socket 
    	if(YAF_IS_NULL(statement)) {
        	yaf_zval_free(statement);
        	RETURN_LONG(-1);
    	}
    	
    	zval *copy = yaf_zval_copy(statement);
    	yaf_zval_free(statement);
		RETURN_ZVAL(copy, 0, 0);
    } else if (YAF_IS_NOT_NULL(statement) && Z_TYPE_P(statement) == IS_OBJECT) { //获取结果
        zval **fetch_args[1];
        zval *result = NULL, *fetch_type = NULL;

        YAF_MAKE_STD_ZVAL(fetch_type);
        if (is_single_column == 1) {
            ZVAL_LONG(fetch_type, PDO_FETCH_COLUMN);
        } else {
            ZVAL_LONG(fetch_type, PDO_FETCH_ASSOC);
        }

        fetch_args[0] = &fetch_type;

        if (yaf_call_user_function_ex_fast(&statement, "fetchAll", &result, 1, fetch_args) == FAILURE) {
            yaf_zval_free(statement);
            yaf_zval_free(result);
            yaf_zval_ptr_dtor(&fetch_type);
            RETURN_LONG(-1);
        }

        yaf_zval_free(statement);
        yaf_zval_ptr_dtor(&fetch_type);
        
		//缓存数据
        if(YAF_IS_NOT_NULL(cache_key)) {
            zval* cache_expire = php_yaf_array_get_value(Z_ARRVAL_P(cache_info), "expire");
            zval* real_expire = NULL;
            if(YAF_IS_NULL(cache_expire)) { //默认5分钟/300秒
                YAF_MAKE_STD_ZVAL(real_expire);
                ZVAL_LONG(real_expire, 300);
            } else {
                real_expire = cache_expire;
            }
            
            set_cache(cache_obj, cache_key, real_expire, result);
        }
		
        RETVAL_ZVAL(result, 1, 1);
        efree(result);
    } else {
        yaf_zval_free(statement);
        RETURN_LONG(-1);
    }
}

int set_cache(zval* cache_obj, zval* cache_key, zval* cache_expire, zval* cache_value) {
    zval* set_string_value;
    YAF_MAKE_STD_ZVAL(set_string_value);
    yaf_serialize(set_string_value, cache_value);
    
    //set value
    zval **set_args[2];
    set_args[0] = &cache_key;
    set_args[1] = &set_string_value;
    int ret = yaf_call_user_function_return_bool_or_unsigned(&cache_obj, "set", 2, set_args);
    yaf_zval_ptr_dtor(&set_string_value);
    
    //set expire time
    if(ret == 1) {
        zval **set_args[2];
        set_args[0] = &cache_key;
        set_args[1] = &cache_expire;
        ret = yaf_call_user_function_return_bool_or_unsigned(&cache_obj, "expire", 2, set_args);
    }
}

zval* get_cache(zval* cache_obj, zval* cache_key) {
    zval *ret_string = NULL;
    zval **get_args[1];
    get_args[0] = &cache_key;
    
    if (yaf_call_user_function_ex_fast(&cache_obj, "get", &ret_string, 1, get_args) == FAILURE) {
        yaf_zval_free(ret_string);
        return NULL;
    }
    
    if (Z_TYPE_P(ret_string) != IS_STRING){
        yaf_zval_free(ret_string);
        return NULL;
    }
    
    zval *ret_array;
    YAF_ALLOC_INIT_ZVAL(ret_array);
    yaf_unserialize(ret_array, ret_string);
    yaf_zval_free(ret_string);
    
    if (YAF_IS_NULL(ret_array) || Z_TYPE_P(ret_array) != IS_ARRAY) {
        yaf_zval_free(ret_array);
        return NULL;
    }
    
    return ret_array;
}

void del_cache(zval* cache_obj, zval* cache_info) {
    if (YAF_IS_NOT_NULL(cache_obj)) {
        zval* cache_key = php_yaf_array_get_value(Z_ARRVAL_P(cache_info), "key");
        if(YAF_IS_NOT_NULL(cache_key)) {
            zval **del_args[1];
            del_args[0] = &cache_key;
            yaf_call_user_function_return_bool_or_unsigned(&cache_obj, "del", 1, del_args);
        }
    }
}

char *select_context(char* table, zval* map, zval* join, zval* columns, zval* where, char** sql, zval** cache_info) {
    //表别名
    char* table_query;
    yaf_string_emalloc_32(&table_query, 0);

    char table_match[MAX_TABLE_SIZE] = {0};
    char alias_match[MAX_TABLE_SIZE] = {0};
    preg_table_match(table, table_match, alias_match);
    if (yaf_is_string_not_empty(table_match) && yaf_is_string_not_empty(alias_match)) {
        yaf_multi_memcpy_auto_realloc(&table_query, 5, "`", table_match, "` AS `", alias_match, "`");
    } else {
        yaf_multi_memcpy_auto_realloc(&table_query, 3, "`", table, "`");
    }

    //解析 join
    char* first_join_key = NULL;
    zval* real_where = where;
    zval* real_columns = columns;

    if (YAF_IS_ARRAY(join) && (first_join_key = yaf_get_array_key_index(join, 0)) != NULL && yaf_strpos(first_join_key, "[") == 0) { //第二个参数不为空，而且判断是 join 的情况
        if (yaf_is_string_not_empty(alias_match)) {
            handle_join(join, alias_match, &table_query);
        } else {
            handle_join(join, table, &table_query);
        }
    } else {
        if (YAF_IS_NULL(where)) { //第四个参数为空时，第二个参数为 column , 第三个参数为 where
            real_columns = join;
            real_where = columns;
        }
    }

    //选择要查询的列
    char* column_query;
    yaf_string_emalloc_32(&column_query, 0);

    column_push(real_columns, map, &column_query);

    yaf_multi_memcpy_auto_realloc(sql, 4, "SELECT ", column_query, " FROM ", table_query);

    yaf_string_efree_32(column_query);
    yaf_string_efree_32(table_query);
		
    where_clause(real_where, map, sql, cache_info);
    return *sql;
}

//处理 where 条件
char* where_clause(zval* where, zval* map, char** sql, zval** cache_info) {
    if (YAF_IS_EMPTY(where)) {
        return *sql;
    }

    char* group_by_condition = NULL;
    char* having_condition = NULL;
    char* order_condition = NULL;
    char* limit_condition = NULL;

    char* where_condition = NULL;
    yaf_string_emalloc_32(&where_condition, 0);

    if (YAF_IS_ARRAY(where)) {
        char * key;
        zval *value;
        uint32_t key_len;
        int key_type;

        YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(where), key, key_len, key_type, value)
        if (HASH_KEY_IS_STRING != key_type) {
            continue;
        } else {
            if (strcmp(key, "GROUP") == 0) { //分组
                yaf_string_emalloc_32(&group_by_condition, 0);
                group_by_implode(value, &group_by_condition);
            } else if (strcmp(key, "HAVING") == 0) { //分组条件
                yaf_string_emalloc_32(&having_condition, 0);
                having_implode(value, map, &having_condition);
            } else if (strcmp(key, "ORDER") == 0) { //排序
                yaf_string_emalloc_32(&order_condition, 0);
                order_implode(value, &order_condition);
            } else if (strcmp(key, "LIMIT") == 0) { //结果条数
                yaf_string_emalloc_32(&limit_condition, 0);
                limit_implode(value, &limit_condition);
            }  else if (strcmp(key, "CACHE") == 0) { //结果条数
            		*cache_info = value;
            } else { // where clause
                where_implode(key, value, map, &where_condition, "AND");
            }
        }
        YAF_HASHTABLE_FOREACH_END();

        strreplace(where_condition, "( AND", "(");
        trim(ltrim_str(ltrim(where_condition), "AND"));
        if (where_condition[0] != '\0') {
            yaf_multi_memcpy_auto_realloc(sql, 2, " WHERE ", where_condition);
        }
    }

    yaf_string_efree_32(where_condition);

    if (group_by_condition != NULL) {
        yaf_multi_memcpy_auto_realloc(sql, 2, " GROUP BY ", group_by_condition);
        yaf_string_efree_32(group_by_condition);
    }

    if (having_condition != NULL) {
        yaf_multi_memcpy_auto_realloc(sql, 2, " HAVING ", having_condition);
        yaf_string_efree_32(having_condition);
    }

    if (order_condition != NULL) {
        yaf_multi_memcpy_auto_realloc(sql, 2, " ORDER BY ", order_condition);
        yaf_string_efree_32(order_condition);
    }

    if (limit_condition != NULL) {
        yaf_multi_memcpy_auto_realloc(sql, 2, " LIMIT ", limit_condition);
        yaf_string_efree_32(limit_condition);
    }

    return *sql;
}


//where condition
char* where_implode(char* key, zval* value, zval* map, char** where_query, char* connector) {
    //key是否是 AND 或者 OR
    char relation_ship[MAX_OPERATOR_SIZE] = {0};
    preg_and_or_match(key, relation_ship);

    if (Z_TYPE_P(value) == IS_ARRAY && yaf_is_string_not_empty(relation_ship)) {
        char* relation_key;
        zval* relation_value;
        uint32_t relation_key_len;
        int relation_key_type;

        char* sub_where_clause = NULL;
        yaf_string_emalloc_32(&sub_where_clause, 0);

        yaf_multi_memcpy_auto_realloc(where_query, 1, " AND (");

        YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(value), relation_key, relation_key_len, relation_key_type, relation_value)
        if (HASH_KEY_IS_STRING != relation_key_type) {
            continue;
        } else {
            where_implode(relation_key, relation_value, map, &sub_where_clause, relation_ship);
        }
        YAF_HASHTABLE_FOREACH_END();

        yaf_multi_memcpy_auto_realloc(where_query, 2, sub_where_clause, ")");
        yaf_string_efree_32(sub_where_clause);
        return *where_query;
    }

    char column[MAX_TABLE_SIZE] = {0};
    char operator[MAX_OPERATOR_SIZE] = {0};
    preg_operator_match(key, column, operator);

    if (yaf_is_string_not_empty(column)) {
        char mapkey[MAP_KEY_SIZE] = {0};

        column_quote(column, column);

        if (yaf_is_string_not_empty(operator)) {
            if (strcmp(operator, ">") == 0 || strcmp(operator, ">=") == 0 || strcmp(operator, "<") == 0 || strcmp(operator, "<=") == 0) { // >, >=, <, <=
                get_mapkey(mapkey);
                add_map(map, mapkey, value);
                yaf_multi_memcpy_auto_realloc(where_query, 6, connector, column, operator, " ", mapkey, " ");
            } else if (strcmp(operator, "!") == 0) { //not equal
                switch (Z_TYPE_P(value)) {
                case IS_NULL:
                    yaf_multi_memcpy_auto_realloc(where_query, 3, connector, column, "IS NOT NULL ");
                    break;
                case IS_ARRAY:
                    yaf_multi_memcpy_auto_realloc(where_query, 3, connector, column, "NOT IN (");
                    handle_where_not_in(value, where_query, map);
                    yaf_multi_memcpy_auto_realloc(where_query, 1, ") ");
                    break;
                case IS_LONG:
                case IS_STRING:
                case IS_DOUBLE:
#if PHP_MAJOR_VERSION < 7 /* PHP Version 5 */
                case IS_BOOL:
#else /* PHP Version 7 */
                case IS_TRUE:
                case IS_FALSE:
#endif
                    get_mapkey(mapkey);
                    add_map(map, mapkey, value);
                    yaf_multi_memcpy_auto_realloc(where_query, 5, connector, column, "!= ", mapkey, " ");
                    break;
                }
            } else if (strcmp(operator, "~") == 0 ||  strcmp(operator, "!~") == 0) { //like
                if (Z_TYPE_P(value) == IS_STRING) {
                    get_mapkey(mapkey);
                    add_map(map, mapkey, value);
                    yaf_multi_memcpy_auto_realloc(where_query, 5, connector, column, (strcmp(operator, "~") == 0 ? "LIKE " : "NOT LIKE "), mapkey, " ");
                } else if (Z_TYPE_P(value) == IS_ARRAY) {
                    char* like_connector = yaf_get_array_key_index(value, 0);
                    if (like_connector != NULL && (strcmp(like_connector, "AND") == 0 || strcmp(like_connector, "OR") == 0)) {
                        //自定义LIKE连接符
                        zval* connetor_value = php_yaf_array_get_value(Z_ARRVAL_P(value), like_connector);

                        if (Z_TYPE_P(connetor_value) == IS_ARRAY) {
                            yaf_multi_memcpy_auto_realloc(where_query, 2, connector, " (");
                            handle_like_array(connetor_value, where_query, column, operator, map, like_connector);
                            yaf_multi_memcpy_auto_realloc(where_query, 1, ") ");
                        }
                    } else { //默认括号内LIKE连接符为 OR
                        yaf_multi_memcpy_auto_realloc(where_query, 2, connector, " (");
                        handle_like_array(value, where_query, column, operator, map, "OR");
                        yaf_multi_memcpy_auto_realloc(where_query, 1, ") ");
                    }
                }
            } else if (strcmp(operator, "<>") == 0 ||  strcmp(operator, "><") == 0) {
                if (Z_TYPE_P(value) == IS_ARRAY) {
                    zval* between_a = yaf_get_element_by_hashtable_index(Z_ARRVAL_P(value), 0);
                    zval* between_b = yaf_get_element_by_hashtable_index(Z_ARRVAL_P(value), 1);
                    if (YAF_IS_NOT_EMPTY(between_a) && (Z_TYPE_P(between_a) == IS_LONG || Z_TYPE_P(between_a) == IS_STRING)
                            && YAF_IS_NOT_EMPTY(between_b) && (Z_TYPE_P(between_b) == IS_LONG || Z_TYPE_P(between_b) == IS_STRING)) {
                        yaf_multi_memcpy_auto_realloc(where_query, 2, connector, " ");
                        if (strcmp(operator, "><") == 0) {
                            yaf_multi_memcpy_auto_realloc(where_query, 1, "NOT ");
                        }
                        get_mapkey(mapkey);
                        add_map(map, mapkey, between_a);
                        yaf_multi_memcpy_auto_realloc(where_query, 5, "(", column, "BETWEEN ", mapkey, " ");
                        get_mapkey(mapkey);
                        add_map(map, mapkey, between_b);
                        yaf_multi_memcpy_auto_realloc(where_query, 3, "AND ", mapkey, ") ");
                    }
                }
            }
        } else {
            switch (Z_TYPE_P(value)) {
            case IS_NULL:
                yaf_multi_memcpy_auto_realloc(where_query, 3, connector, column, "IS NULL ");
                break;
            case IS_ARRAY:
                yaf_multi_memcpy_auto_realloc(where_query, 3, connector, column, "IN (");
                handle_where_not_in(value, where_query, map);
                yaf_multi_memcpy_auto_realloc(where_query, 1, ") ");
                break;
            case IS_LONG:
            case IS_STRING:
            case IS_DOUBLE:
#if PHP_MAJOR_VERSION < 7 /* PHP Version 5 */
            case IS_BOOL:
#else /* PHP Version 7 */
            case IS_TRUE:
            case IS_FALSE:
#endif
                get_mapkey(mapkey);
                add_map(map, mapkey, value);
                yaf_multi_memcpy_auto_realloc(where_query, 5, connector, column, "= ", mapkey, " ");
                break;
            }
        }
    }

    ltrim_str(*where_query, connector);
    return *where_query;
}

//handle group by
char* group_by_implode(zval* group, char** group_by_condition) {
    if (YAF_IS_NOT_EMPTY(group)) {
        if (Z_TYPE_P(group) == IS_STRING) {
            yaf_multi_memcpy_auto_realloc(group_by_condition, 1, Z_STRVAL_P(group));
        } else if (Z_TYPE_P(group) == IS_ARRAY) {
            char* key;
            zval* value;
            uint32_t key_len;
            int key_type;


            YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(group), key, key_len, key_type, value)
            if (Z_TYPE_P(value) == IS_STRING) {
                yaf_multi_memcpy_auto_realloc(group_by_condition, 2, Z_STRVAL_P(value), ",");
            }
            YAF_HASHTABLE_FOREACH_END();

            char* tmp = (*group_by_condition) +  strlen(*group_by_condition) - 1;
            if (*tmp == ',') {
                *tmp = '\0';
            }
        }
    }
    return *group_by_condition;
}

//handle having
char* having_implode(zval* having, zval* map, char** having_condition) {

    if (YAF_IS_ARRAY(having)) {
        char * key;
        zval *value;
        uint32_t key_len;
        int key_type;

        YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(having), key, key_len, key_type, value)
        if (HASH_KEY_IS_STRING != key_type) {
            continue;
        } else {
            where_implode(key, value, map, having_condition, "AND");
        }
        YAF_HASHTABLE_FOREACH_END();
    }

    strreplace(*having_condition, "( AND", "(");
    trim(ltrim_str(ltrim(*having_condition), "AND"));
    return *having_condition;
}

//order by
char* order_implode(zval* order, char** order_condition) {
    if (YAF_IS_NOT_EMPTY(order)) {
        if (Z_TYPE_P(order) == IS_STRING) {
            yaf_multi_memcpy_auto_realloc(order_condition, 1, Z_STRVAL_P(order));
        } else if (Z_TYPE_P(order) == IS_ARRAY) {
            char* key;
            zval* value;
            uint32_t key_len;
            int key_type;

            YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(order), key, key_len, key_type, value)
            if (HASH_KEY_IS_STRING != key_type) {
                if (Z_TYPE_P(value) == IS_STRING) {
                    char column[MAX_TABLE_SIZE] = {0};
                    column_quote(Z_STRVAL_P(value), column);
                    yaf_multi_memcpy_auto_realloc(order_condition, 2, column, ",");
                }
            } else {
                if (Z_TYPE_P(value) == IS_STRING && (strcmp(Z_STRVAL_P(value), "ASC") == 0 || strcmp(Z_STRVAL_P(value), "DESC") == 0)) {
                    char column[MAX_TABLE_SIZE] = {0};
                    column_quote(key, column);
                    yaf_multi_memcpy_auto_realloc(order_condition, 3, column, Z_STRVAL_P(value), ",");
                }
            }
            YAF_HASHTABLE_FOREACH_END();
            rtrim_str(*order_condition, ",");
        }
    }
    return *order_condition;
}

//limit
char* limit_implode(zval* limit, char** limit_condition) {
    if (YAF_IS_NOT_EMPTY(limit)) {
        if (Z_TYPE_P(limit) == IS_STRING || Z_TYPE_P(limit) == IS_LONG) {
            convert_to_string(limit);
            if (is_numeric_string(Z_STRVAL_P(limit), Z_STRLEN_P(limit), NULL, NULL, 0)) {
                yaf_multi_memcpy_auto_realloc(limit_condition, 1, Z_STRVAL_P(limit));
            }
        } else if (Z_TYPE_P(limit) == IS_ARRAY) {
            zval* offset_val = yaf_get_element_by_hashtable_index(Z_ARRVAL_P(limit), 0);
            zval* limit_val = yaf_get_element_by_hashtable_index(Z_ARRVAL_P(limit), 1);
            convert_to_string(limit_val);
            convert_to_string(offset_val);

            if (is_numeric_string(Z_STRVAL_P(limit_val), Z_STRLEN_P(limit_val), NULL, NULL, 0)
                    && is_numeric_string(Z_STRVAL_P(offset_val), Z_STRLEN_P(offset_val), NULL, NULL, 0)) {
                yaf_multi_memcpy_auto_realloc(limit_condition, 3, Z_STRVAL_P(limit_val), " OFFSET ", Z_STRVAL_P(offset_val));
            }
        }
    }

    return *limit_condition;
}

//like array情况
char* handle_like_array(zval* like_array, char** where_query, char* column, char* operator, zval* map, char* connector) {
    char * key;
    zval *value;
    uint32_t key_len;
    int key_type;

    char mapkey[MAP_KEY_SIZE] = {0};
    YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(like_array), key, key_len, key_type, value)
    if (Z_TYPE_P(value) == IS_STRING || Z_TYPE_P(value) == IS_LONG) {
        get_mapkey(mapkey);
        add_map(map, mapkey, value);
        yaf_multi_memcpy_auto_realloc(where_query, 5, column, strcmp(operator, "~") == 0 ? "LIKE " : "NOT LIKE ", mapkey, " ", connector);
    }
    YAF_HASHTABLE_FOREACH_END();
    rtrim_str(rtrim(*where_query), connector);
    return *where_query;
}

//not in 情况
char* handle_where_not_in(zval* not_in_array, char** where_query, zval* map) {
    char * key;
    zval *value;
    uint32_t key_len;
    int key_type;

    char mapkey[MAP_KEY_SIZE] = {0};

    YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(not_in_array), key, key_len, key_type, value)
    if (Z_TYPE_P(value) == IS_STRING || Z_TYPE_P(value) == IS_LONG) {
        get_mapkey(mapkey);
        add_map(map, mapkey, value);
        yaf_multi_memcpy_auto_realloc(where_query, 3, " ", mapkey, ",");
    }
    YAF_HASHTABLE_FOREACH_END();

    rtrim_str(rtrim(*where_query), ",");
    return *where_query;
}

char* get_mapkey(char* mapkey) {
    memset(mapkey, 0, MAP_KEY_SIZE);
    zval* mapkey_index = zend_read_static_property(yaf_db_ce_ptr, ZEND_STRL("mapkey_index"), 1 TSRMLS_DC);
    long l_mapkey_index = Z_LVAL_P(mapkey_index);
    l_mapkey_index = (l_mapkey_index + 1) % 9999;
    zend_update_static_property_long(yaf_db_ce_ptr, ZEND_STRL("mapkey_index"), l_mapkey_index TSRMLS_DC);
    sprintf(mapkey, ":param_%d", l_mapkey_index);
    return mapkey;
}

zval* add_map(zval* map, char* mapkey, zval* value) {
    zval *copy = yaf_zval_copy(value);
    add_assoc_zval(map, mapkey, copy);
    return map;
}

//处理查询列
char* column_push(zval* columns, zval* map, char** column_query) {
    if (YAF_IS_EMPTY(columns) || (Z_TYPE_P(columns) == IS_STRING && strcmp(Z_STRVAL_P(columns), "*") == 0)) {
        yaf_multi_memcpy_auto_realloc(column_query, 1, "*");
        return *column_query;
    }

    if (Z_TYPE_P(columns) == IS_STRING) {
        yaf_multi_memcpy_auto_realloc(column_query, 1, Z_STRVAL_P(columns));
        return *column_query;
    } else if (YAF_IS_ARRAY(columns)) {
        char * key;
        zval *value;
        uint32_t key_len;
        int key_type;

        YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(columns), key, key_len, key_type, value)
        if (Z_TYPE_P(value) != IS_STRING) {
            continue;
        }

        char match_column[MAX_TABLE_SIZE] = {0};
        char match_alias[MAX_TABLE_SIZE] = {0};
        preg_table_match(Z_STRVAL_P(value), match_column, match_alias);

        if (yaf_is_string_not_empty(match_column) && yaf_is_string_not_empty(match_alias)) {
            yaf_multi_memcpy_auto_realloc(column_query, 4, match_column, " AS `", match_alias, "`,");
        } else {
            yaf_multi_memcpy_auto_realloc(column_query, 2, Z_STRVAL_P(value), ",");
        }

        YAF_HASHTABLE_FOREACH_END();

        rtrim_str(rtrim(*column_query), ",");
        return *column_query;
    } else {
        yaf_multi_memcpy_auto_realloc(column_query, 1, "*");
        return *column_query;
    }
}

//处理join
char* handle_join(zval *join, char *table, char** table_query) {
    char* sub_table;
    zval* relation;
    uint32_t key_len;
    int key_type;

    YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(join), sub_table, key_len, key_type, relation)
    if (HASH_KEY_IS_STRING != key_type) { //非字符串
        continue;
    }

    char join_join[MAX_TABLE_SIZE] = {0};
    char join_table[MAX_TABLE_SIZE] = {0};
    char join_alias[MAX_TABLE_SIZE] = {0};
    preg_join_match(sub_table, join_join, join_table, join_alias);


    if (yaf_is_string_not_empty(join_join) && yaf_is_string_not_empty(join_table)) {
        yaf_multi_memcpy_auto_realloc(table_query, 5, " ", get_join_type(join_join), " JOIN `", join_table, "` ");
        if (yaf_is_string_not_empty(join_alias)) {
            yaf_multi_memcpy_auto_realloc(table_query, 3, "AS `", join_alias, "` ");
        }

        if (Z_TYPE_P(relation) == IS_STRING) {
            yaf_multi_memcpy_auto_realloc(table_query, 3, "USING (`", Z_STRVAL_P(relation), "`) ");
        } else if (Z_TYPE_P(relation) == IS_ARRAY) {
            if (is_set_array_index(Z_ARRVAL_P(relation), 0)) { //relation 为数组
                yaf_multi_memcpy_auto_realloc(table_query, 1, "USING (`");
                yaf_implode(relation, "`,`", table_query);
                yaf_multi_memcpy_auto_realloc(table_query, 1, "`) ");
            } else { //relation 为 Key Hash
                char *key;
                zval *value;

                yaf_multi_memcpy_auto_realloc(table_query, 1, "ON ");

                YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(relation), key, key_len, key_type, value)
                if (HASH_KEY_IS_STRING != key_type) { //非字符串
                    continue;
                }

                char* table_column = NULL;
                yaf_string_emalloc_32(&table_column, 0);
                if (yaf_strpos(key, ".") >= 0) {
                    column_quote(key, table_column);
                } else {
                    yaf_multi_memcpy_auto_realloc(&table_column, 5, "`", table, "`.`", key, "`");
                }

                //alias
                if (yaf_is_string_not_empty(join_alias)) {
                    yaf_multi_memcpy_auto_realloc(table_query, 4, table_column, "=`", join_alias, "`");
                } else {
                    yaf_multi_memcpy_auto_realloc(table_query, 4, table_column, "= `", join_table, "`");
                }

                yaf_string_efree_32(table_column);

                yaf_multi_memcpy_auto_realloc(table_query, 3, ".`", Z_STRVAL_P(value), "` AND");
                YAF_HASHTABLE_FOREACH_END();

                rtrim_str(rtrim(*table_query), "AND");
            }
        }

    }
    YAF_HASHTABLE_FOREACH_END();


    return *table_query;
}

char* column_quote(char* string, char* table_column) {
    char tmp[MAX_TABLE_SIZE] = {0};

    sprintf(tmp, " `%s` ", string);

    if (strlen(tmp) >= MAX_TABLE_SIZE) {
        yaf_php_fatal_error(E_ERROR, "column size is too long, [%s]", string);
    }

    if (yaf_strpos(tmp, ".") >= 0) {
        if (strlen(tmp) + 5 >= MAX_TABLE_SIZE) {
            yaf_php_fatal_error(E_ERROR, "column + alias size is too long, [%s]", string);
        }
        strreplace(tmp, ".", "`.`");
    }

    strcpy(table_column, tmp);
    return table_column;
}

char* get_join_type(char* type) {
    if (strcmp(type, "<") == 0) {
        return "LEFT";
    } else if (strcmp(type, ">") == 0) {
        return "RIGHT";
    } else if (strcmp(type, "<>") == 0) {
        return "FULL";
    } else if (strcmp(type, "><") == 0) {
        return "INNER";
    } else {
        return "";
    }
}

//匹配表和别名
int preg_join_match(char* key, char* join, char* table, char* alias) {
    int join_start = -1;
    int join_end = -1;
    int table_start = -1;
    int table_end = -1;
    int alias_start = -1;
    int alias_end = -1;

    int key_len = strlen(key);

    join[0] = '\0';
    table[0] = '\0';
    alias[0] = '\0';

    if (key_len == 0) {
        return 0;
    }

    int i = -1;
    while (i < key_len) {
        i++;
        char c_key = key[i];
        if ( join_start == -1 && c_key == '[') {
            join_start = i;
        }

        if (table_start == -1 && join_start == -1 && c_key != '[' && !yaf_is_space(c_key)) {
            table_start = i;
        }

        if (join_end != -1 && table_start == -1 && !yaf_is_space(c_key)) {
            table_start = i;
        }

        if ( join_start != -1 && c_key == ']') {
            join_end = i;
        }

        if (table_start != -1 && c_key == '(') {
            table_end = i;
        }

        if ( alias_start == -1 && c_key == '(') {
            alias_start = i;
        }

        if ( alias_end == -1 && c_key == ')') {
            alias_end = i;
        }
    }

    if (alias_start == -1 || alias_end == -1 || alias_start > alias_end) {
        table_end = key_len;
    }

    if (table_start != -1 && table_end != -1 && table_end > table_start) {
        if (table_end - table_start >= MAX_TABLE_SIZE) {
            yaf_php_fatal_error(E_ERROR, "join table size is too long, [%s]", key);
        }

        memset(table, 0, MAX_TABLE_SIZE);
        memcpy(table, key + table_start, table_end - table_start);
    }

    if (alias_start != -1 && alias_end != -1 && alias_end > alias_start) {
        if (alias_end - alias_start >= MAX_TABLE_SIZE) {
            yaf_php_fatal_error(E_ERROR, "join alias size is too long, [%s]", key);
        }

        memset(alias, 0, MAX_TABLE_SIZE);
        memcpy(alias, key + alias_start + 1, alias_end - alias_start - 1);
    }

    if (join_start != -1 && join_end != -1 && join_start < join_end) {
        if (join_end - join_start >= MAX_OPERATOR_SIZE) {
            yaf_php_fatal_error(E_ERROR, "join operator size is too long, [%s]", key);
        }

        memset(join, 0, MAX_OPERATOR_SIZE);
        memcpy(join, key + join_start + 1, join_end - join_start - 1);
        if (!(strcmp(join, ">") == 0 || strcmp(join, "<") == 0 || strcmp(join, "<>") == 0 || strcmp(join, "><") == 0)) {
            join[0] = '\0';
        }
    }
    return 1;
}

//匹配表和别名
int preg_table_match(char* key, char* table, char* alias) {
    int table_start = -1;
    int table_end = -1;
    int alias_start = -1;
    int alias_end = -1;

    int key_len = strlen(key);

    table[0] = '\0';
    alias[0] = '\0';

    if (key_len == 0) {
        return 0;
    }

    int i = -1;
    while (i < key_len) {
        i++;
        char c_key = key[i];
        if ( table_start == -1 && !yaf_is_space(c_key)) {
            table_start = i;
        }

        if (table_end == -1 && (c_key == '(' || yaf_is_space(c_key))) {
            table_end = i;
        }

        if ( alias_start == -1 && c_key == '(') {
            alias_start = i;
        }

        if ( alias_end == -1 && c_key == ')') {
            alias_end = i;
        }
    }

    if (alias_start == -1 || alias_end == -1 || alias_start > alias_end) {
        table_end = key_len;
    }

    if (table_start != -1 && table_end != -1 && table_end > table_start) {
        if (table_end - table_start >= MAX_TABLE_SIZE) {
            yaf_php_fatal_error(E_ERROR, "table size is too long, [%s]", key);
        }

        memset(table, 0, MAX_TABLE_SIZE);
        memcpy(table, key + table_start, table_end - table_start);
    }

    if (alias_start != -1 && alias_end != -1 && alias_end > alias_start) {
        if (alias_end - alias_start >= MAX_TABLE_SIZE) {
            yaf_php_fatal_error(E_ERROR, "alias size is too long, [%s]", key);
        }

        memset(alias, 0, MAX_TABLE_SIZE);
        memcpy(alias, key + alias_start + 1, alias_end - alias_start - 1);
    }

    return 1;
}

//匹配列名和操作符
int preg_operator_match(char* key, char* column, char* operator) {
    int column_start = -1;
    int column_end = -1;
    int column_end_is_space = -1;
    int operator_start = -1;
    int operator_end = -1;

    int key_len = strlen(key);

    column[0] = '\0';
    operator[0] = '\0';

    if (key_len == 0) {
        return 0;
    }

    int i = -1;
    while (i < key_len) {
        i++;
        char c_key = key[i];
        if ( column_start == -1 && !yaf_is_space(c_key)) {
            column_start = i;
        }

        if (column_end == -1 && (c_key == '[' || yaf_is_space(c_key))) {
            column_end = i;
        }

        if (column_end_is_space == -1 && yaf_is_space(c_key)) {
            column_end_is_space = i;
        }

        if ( operator_start == -1 && c_key == '[') {
            operator_start = i;
        }

        if ( operator_end == -1 && c_key == ']') {
            operator_end = i;
        }
    }

    if (operator_start == -1 || operator_end == -1 || operator_start > operator_end) {
        column_end = column_end_is_space == -1 ? key_len : column_end_is_space;
    }

    if (column_start != -1 && column_end != -1 && column_end > column_start) {
        if (column_end - column_start - 1 >= MAX_TABLE_SIZE) {
            yaf_php_fatal_error(E_ERROR, "column size is too long [%s]", key);
        }

        memset(column, 0, MAX_TABLE_SIZE);
        memcpy(column, key + column_start, column_end - column_start);
    }

    if (operator_start != -1 && operator_end != -1 && operator_start < operator_end) {
        if (operator_end - operator_start - 1 >= MAX_OPERATOR_SIZE) {
            yaf_php_fatal_error(E_ERROR, "operator size is too long [%s]", key);
        }

        memset(operator, 0, MAX_OPERATOR_SIZE);
        memcpy(operator, key + operator_start + 1, operator_end - operator_start - 1);
        if (!(strcmp(operator, ">") == 0 || strcmp(operator, ">=") == 0 || strcmp(operator, "<") == 0 || strcmp(operator, "<=") == 0 ||
                strcmp(operator, "!") == 0 || strcmp(operator, "~") == 0 || strcmp(operator, "!~") == 0 || strcmp(operator, "<>") == 0 || strcmp(operator, "><") == 0)) {
            operator[0] = '\0';
        }
    }

    return 1;
}

//匹配是否 AND 或者 OR
int preg_and_or_match(char* key, char* relation) {
    int relation_start = -1;
    int relation_end = -1;

    relation[0] = '\0';

    int key_len = strlen(key);
    if (key_len == 0) {
        return 0;
    }

    int i = -1;
    while (i < key_len) {
        i++;
        char c_key = key[i];

        if ( relation_start == -1 && !yaf_is_space(c_key)) {
            relation_start = i;
        }

        if (relation_end == -1 && ( c_key == '#' || yaf_is_space(c_key))) {
            relation_end = i;
        }

        if (relation_end == -1 && i == key_len - 1) {
            relation_end = key_len;
        }
    }

    if (relation_start != -1 && relation_end != -1 && relation_end > relation_start && relation_end - relation_start - 1 < MAX_OPERATOR_SIZE) {
        memset(relation, 0, MAX_OPERATOR_SIZE);
        memcpy(relation, key + relation_start, relation_end - relation_start);
        if (strcmp(relation, "AND") != 0 && strcmp(relation, "OR") != 0 && strcmp(relation, "and") != 0 && strcmp(relation, "or") != 0 ) {
            relation[0] = '\0';
        }
    }

    return 1;
}

zend_class_entry* get_pdo_ce() {
    zend_class_entry* ce;
    zend_string* pdo_class_name = zend_string_init("PDO", strlen("PDO"), 0);
    ce = zend_fetch_class(pdo_class_name, ZEND_FETCH_CLASS_AUTO TSRMLS_CC);
    zend_string_free(pdo_class_name);
    return ce;
}

void update_error_info(zval* obj, char* code, char* errmsg) {
    zend_update_property_string(yaf_db_ce_ptr, obj, ZEND_STRL("errcode"), code TSRMLS_CC);
    zval* errorinfo = yaf_read_init_property(yaf_db_ce_ptr, obj, ZEND_STRL("errinfo") TSRMLS_CC);

    if (errorinfo != NULL) {
        zend_update_property_null(yaf_db_ce_ptr, obj, ZEND_STRL("errinfo"));
    }

    zval* error_array;
    YAF_MAKE_STD_ZVAL(error_array);
    array_init(error_array);

    add_index_string(error_array, 0, code);
    if (strcmp(code, "00000") == 0) {
        add_index_long(error_array, 1, 0);
    } else {
        add_index_long(error_array, 1, -1);
    }
    add_index_string(error_array, 2, errmsg);

    zend_update_property(yaf_db_ce_ptr, obj, ZEND_STRL("errinfo"), error_array TSRMLS_CC);
    yaf_zval_ptr_dtor(&error_array);
}

void update_pdo_error(zval* obj, zval* errorcode, zval* errorinfo) {
    zend_update_property(yaf_db_ce_ptr, obj, ZEND_STRL("errcode"), errorcode TSRMLS_CC);
    zend_update_property(yaf_db_ce_ptr, obj, ZEND_STRL("errinfo"), errorinfo TSRMLS_CC);
}

int is_write_type(char* sql) {
    char *start = sql;
    int sql_len = strlen(sql);
    int i=0, len=0;
    char operator[10] = {0};

    while (i < sql_len) {
        i++;
        if (!isspace(*start) && (*start) != '\n' && (*start) != '\r' && (*start) != '\t') {
            break;
        }
        start++;
    }

    char* end = start;

    while (i < sql_len && len < 8) {
        if (isspace(*end) || (*end) == '\n' || (*end) == '\r' || (*end) == '\t') {
            break;
        }

        end++;
        i++;
        len++;
    }

    memcpy(operator, start, len + 1);
    rtrim(operator);
    php_strtoupper(operator, strlen(operator));

    if (strcmp(operator, "INSERT") == 0 || strcmp(operator, "UPDATE") == 0 || strcmp(operator, "DELETE") == 0 || strcmp(operator, "REPLACE") == 0
            || strcmp(operator, "SET") == 0 || strcmp(operator, "CREATE") == 0 || strcmp(operator, "DROP") == 0 || strcmp(operator, "TRUNCATE") == 0
            || strcmp(operator, "ALTER") == 0 || strcmp(operator, "LOCK") == 0 || strcmp(operator, "UNLOCK") == 0) {
        return 1;
    }

    return 0;
}

int is_insert(char* sql) {
    char *start = sql;
    int sql_len = strlen(sql);
    int i=0, len=0;
    char operator[10] = {0};

    while (i < sql_len) {
        i++;
        if (!isspace(*start) && (*start) != '\n' && (*start) != '\r' && (*start) != '\t') {
            break;
        }
        start++;
    }

    char* end = start;

    while (i < sql_len && len < 8) {
        if (isspace(*end) || (*end) == '\n' || (*end) == '\r' || (*end) == '\t') {
            break;
        }

        end++;
        i++;
        len++;
    }

    memcpy(operator, start, len + 1);
    rtrim(operator);
    php_strtoupper(operator, strlen(operator));

    if (strcmp(operator, "INSERT") == 0 || strcmp(operator, "REPLACE") == 0) {
        return 1;
    }

    return 0;
}


//根据数组下标从数组中获取字符串
char* yaf_get_string_from_array_index(zval *array, ulong index) 
{
	zval *pData = NULL;
	
	yaf_zend_hash_index_find(Z_ARRVAL_P(array), index, (void**) &pData);

	if(pData == NULL){
		return NULL;
	}
	
	if(Z_TYPE_P(pData) != IS_STRING) {
		return NULL;
	}
	
	char * str = Z_STRVAL_P(pData);
	return str;
}

//获取数组的 array_keys ，注意不用了销毁返回的HashTable指针指向的内存地址
HashTable* yaf_get_array_keys(zval *p) {
	if(YAF_IS_NOT_ARRAY(p)) {
		return NULL;
	}
	
	uint32_t array_size = zend_hash_num_elements(Z_ARRVAL_P(p));
	if(array_size == 0) {
		return NULL;
	}
	
	char * key;
	zval *value;
	uint32_t key_len;
	int key_type;
	ulong_t num = 0;
	
	HashTable *new_hash;
	ALLOC_HASHTABLE(new_hash);
	zend_hash_init(new_hash, array_size, NULL, ZVAL_PTR_DTOR, 0);
	
	YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(p), key, key_len, key_type, value)
			if (HASH_KEY_IS_STRING != key_type) { //非字符串
				continue;
			}
			
			zval *zval_key;
			YAF_MAKE_STD_ZVAL(zval_key);
			YAF_ZVAL_STRING(zval_key, key, 1);
			yaf_zend_hash_index_update(new_hash, num, (void*) zval_key, sizeof(zval *), NULL);
			yaf_zval_ptr_dtor(&zval_key);
			num++;
  YAF_HASHTABLE_FOREACH_END();
  
  if(zend_hash_num_elements(new_hash) == 0) {
  	yaf_free_hash(new_hash);
  	new_hash = NULL;
  }
  
  return new_hash;
}

//获取数组的第N个key
char* yaf_get_array_key_index(zval *p, uint32_t index) {
	if(YAF_IS_NOT_ARRAY(p)) {
		return NULL;
	}
	
	uint32_t array_size = zend_hash_num_elements(Z_ARRVAL_P(p));
	if(array_size < index) {
		return NULL;
	}
	
	char * key;
	zval *value;
	uint32_t key_len;
	int key_type;
	ulong_t num = 0;
	
	YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(p), key, key_len, key_type, value)
			if (HASH_KEY_IS_STRING != key_type) { //非字符串
				continue;
			}
			
			if(num == index) {
				return key;
			}
			
			num++;
  YAF_HASHTABLE_FOREACH_END();
  return NULL;
}

//根据 index 获取 hashtable 的元素
zval* yaf_get_element_by_hashtable_index(HashTable *ht, int index) {
	if(ht == NULL) {
		return NULL;
	}
	zval *value;
	yaf_zend_hash_index_find(ht, index, (void**) &value);
	return value;
}

//根据 key 获取 hashtable 的元素
zval* php_yaf_array_get_value(HashTable *ht, char *key) {
	zval *v;
	if( yaf_zend_hash_find(ht, key, strlen(key), (void **) &v) == SUCCESS ) {
		if(ZVAL_IS_NULL(v)) {
			return NULL;
		} else {
			return v;
		}
	} else {
		return NULL;
	}
}

//销毁 HashTable， 删除HashTable的数据， 并销毁释放 HashTable 内存
void yaf_destroy_hash(HashTable *ht) {
	uint32_t array_size = zend_hash_num_elements(ht);
	if(array_size == 0) {
		zend_hash_destroy(ht);
		FREE_HASHTABLE(ht);
		return;
	}
	
	char *key;
	zval *value;
	uint32_t key_len;
	int key_type;
	
	YAF_HASHTABLE_FOREACH_START2(ht, key, key_len, key_type, value)
		if(YAF_IS_ARRAY(value)) {
			yaf_destroy_hash(Z_ARRVAL_P(value));
		}
		
		yaf_zval_ptr_dtor(&value);
  	YAF_HASHTABLE_FOREACH_END();
  	
	zend_hash_destroy(ht);
	FREE_HASHTABLE(ht);
}

//销毁数组， 删除其中HashTable的数据， 并销毁释放 HashTable 内存，并销毁数组内存，将数组指针置为NULL
void yaf_destroy_array(zval **array) {
	yaf_destroy_hash(Z_ARRVAL_P(*array));
	yaf_zval_ptr_dtor(array);
	*array = NULL;
}

//清理 HashTable 内的数据元素。
void yaf_clean_hash(HashTable *ht) {
	uint32_t array_size = zend_hash_num_elements(ht);
	if(array_size == 0) {
		return;
	}
	
	char *key;
	zval *value;
	uint32_t key_len;
	int key_type;
	
	YAF_HASHTABLE_FOREACH_START2(ht, key, key_len, key_type, value)
		if(YAF_IS_ARRAY(value)) {
			yaf_clean_hash(Z_ARRVAL_P(value));
		}
		yaf_zval_ptr_dtor(&value);
  	YAF_HASHTABLE_FOREACH_END();
}

int is_set_array_index(HashTable *ht, int index) {
	zval* p = yaf_get_element_by_hashtable_index(ht, index);
	if(YAF_IS_NOT_EMPTY(p)) {
		return 1;
	} else {
		return 0;
	}
}


char* yaf_get_string_from_hashtable_index(HashTable *ht, int index) {
	zval* val = yaf_get_element_by_hashtable_index(ht, index);
	if(YAF_IS_EMPTY(val)) {
		return NULL;
	}
	
	if(Z_TYPE_P(val) == IS_STRING) {
		return Z_STRVAL_P(val);
	} else {
		return NULL;
	}
}

char* yaf_get_string_from_hashtable(HashTable *ht, char* key) {
	zval* val = php_yaf_array_get_value(ht, key);
	if(YAF_IS_EMPTY(val)) {
		return NULL;
	}
	
	if(Z_TYPE_P(val) == IS_STRING) {
		return Z_STRVAL_P(val);
	} else {
		return NULL;
	}
}

//php implode 功能
char* yaf_implode(zval *arr, const char *delim_str, char** result)
{
	zval *return_value = NULL;
	YAF_MAKE_STD_ZVAL(return_value);
	zend_string *delim = zend_string_init(delim_str, strlen(delim_str), 0);
	
	php_implode(delim, arr, return_value);
	
	yaf_multi_memcpy_auto_realloc(result, 1, Z_STRVAL_P(return_value));
	
	efree(delim);
	yaf_zval_ptr_dtor(&return_value);
	
	return *result;
}

//单元素数组重组
void yaf_array_single_columns(zval** return_single_column_result, zval* data) {
	array_init(*return_single_column_result);
		    		
	char *key;
	zval *value;
	uint32_t key_len;
	int key_type;
		    		
	YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(data), key, key_len, key_type, value)
		char *key2;
		zval *value2;
		uint32_t key_len2;
		int key_type2;
		YAF_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(value), key2, key_len2, key_type2, value2)
			zval *copy = yaf_zval_copy(value2);
			add_next_index_zval(*return_single_column_result, copy);
			break;
		YAF_HASHTABLE_FOREACH_END();
	YAF_HASHTABLE_FOREACH_END();
}


char* yaf_multi_memcpy(char* source, int n_value, ...) {
	va_list var_arg;
	int count=0;
	va_start(var_arg, n_value);
	while(count < n_value) {
		char *tmp = va_arg(var_arg, char*);
		memcpy(source + strlen(source), tmp, strlen(tmp));
		count++;
	}
	va_end(var_arg);
	return source;
}

static yaf_inline size_t get_string_emalloc_size(char* source) {
	size_t source_size = 0;
	memcpy(&source_size, source - 4, sizeof(size_t));
	return source_size;
}

char* yaf_multi_memcpy_auto_realloc(char** source, int n_value, ...) {
	int source_size = get_string_emalloc_size(*source);
	
	va_list var_arg;
	int count = 0;
	int dest_len = strlen(*source) + 1;
	va_start(var_arg, n_value);
	while(count < n_value) {
		char *tmp = va_arg(var_arg, char*);
		dest_len += strlen(tmp);
		count++;
	}
	va_end(var_arg);
	
	//need realloc
	char* dest = NULL;
	if(source_size < MM_REAL_SIZE(dest_len)) {
		yaf_string_emalloc_32(&dest, dest_len);
		memcpy(dest, *source, strlen(*source));
		yaf_string_efree_32(*source);
		*source = dest;
	} else {
		dest = *source;
	}
	
	count=0;
	va_start(var_arg, n_value);
	while(count < n_value) {
		char *tmp = va_arg(var_arg, char*);
		memcpy(dest + strlen(dest), tmp, strlen(tmp));
		count++;
	}
	va_end(var_arg);
	
	//php_printf("====yaf_multi_memcpy_auto_alloc : source_size=[%d], dest_size=[%d], dest_size=[%d], \ndest_point=[%x]  dest=[%s]\n", source_size, dest_len, MM_REAL_SIZE(dest_len), dest, dest);
	return dest;
}


php_stream* unix_socket_conn(char *servername)
{
    char host[1024] = {0};
    int host_len, err = 0;
    
#if (PHP_MAJOR_VERSION < 7)
    char *estr = NULL;
#else
    zend_string *estr = NULL;
#endif
    host_len = snprintf(host, sizeof(host), "unix://%s", servername);
    
    //超时时间
    struct timeval tv, read_tv;
    tv.tv_sec  = (time_t) 1;  //连接超时
    tv.tv_usec = (int) 0;
    
    read_tv.tv_sec  = (time_t) 5; //读取超时
    read_tv.tv_usec = (int) 0;
    
    php_stream *stream = php_stream_xport_create(host, host_len, STREAM_OPEN_PERSISTENT | STREAM_LOCATE_WRAPPERS_ONLY, STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT_ASYNC, NULL, &tv, NULL, &estr, &err);
    
    if (!stream) {
    	return NULL;
    }
    
    php_stream_auto_cleanup(stream);
    php_stream_set_option(stream, PHP_STREAM_OPTION_READ_TIMEOUT, 0, &read_tv);
    php_stream_set_option(stream, PHP_STREAM_OPTION_WRITE_BUFFER, PHP_STREAM_BUFFER_NONE, NULL);
    php_stream_set_option(stream, PHP_STREAM_OPTION_READ_BUFFER, PHP_STREAM_BUFFER_NONE, NULL);
    
    return stream;
}


int yaf_compare_strict_bool(zval *op1, zend_bool op2 TSRMLS_DC) {

	int bool_result;
	if(op1 == NULL) {
		return 0 == op2;
	}
	
	switch (Z_TYPE_P(op1)) {
		case IS_LONG:
			return (Z_LVAL_P(op1) ? 1 : 0) == op2;
		case IS_DOUBLE:
			return (Z_DVAL_P(op1) ? 1 : 0) == op2;
		case IS_NULL:
			return 0 == op2;
#if PHP_MAJOR_VERSION < 7
		case IS_BOOL:
			if (Z_BVAL_P(op1)) {
				return 1 == op2;
			} else {
				return 0 == op2;
			}
#else
		case IS_TRUE:
			return 1 == op2;
		case IS_FALSE:
			return 0 == op2;
#endif
		default:
			return 0;
	}

	return 0;
}


//字符串复制
#if defined(YAF_USE_JEMALLOC) || defined(YAF_USE_TCMALLOC)
yaf_inline char* yaf_strdup(const char *s)
{
    size_t l = strlen(s) + 1;
    char *p = yaf_malloc(l);
    memcpy(p, s, l);
    return p;
}
yaf_inline char* yaf_strndup(const char *s, size_t n)
{
    char *p = yaf_malloc(n + 1);
    strncpy(p, s, n);
    p[n] = '\0';
    return p;
}
#endif

//preg_match函数
zval* yaf_preg_match(const char* regex_str, char* subject_str)
{
	//return NULL;
	zval retval;
	zval* matches;
	
	if(regex_str == NULL || subject_str == NULL) {
		return NULL;
	}
	
	pcre_cache_entry *pce;
	zend_string *regex = zend_string_init(regex_str, strlen(regex_str), 0);
	pce = pcre_get_compiled_regex_cache(regex);
	zend_string_free(regex);
	
	if (pce == NULL) {
		return NULL;
	}
	
	YAF_ALLOC_INIT_ZVAL(matches);
	ZVAL_NULL(matches);
	
	//执行正则
	php_pcre_match_impl(pce, subject_str, strlen(subject_str), &retval, matches, 0, 0, Z_L(0), Z_L(0));

	if(Z_TYPE(retval) == IS_FALSE) {
		return NULL;
	} else {
		return matches;
	}
}

//找到 needle 在 haystack 中的位置， 找到则 >= 0，否则为 -1
int yaf_strpos(const char* haystack,const char* needle)    
{
	int ignorecase = 0;
    register unsigned char c, needc;  
    unsigned char const *from, *end;  
    int len = strlen(haystack);  
    int needlen = strlen(needle);
    from = (unsigned char *)haystack;  
    end = (unsigned char *)haystack + len;  
    const char *findreset = needle;  
    
    int i = 0;
    
    while (from < end) {
        c = *from++;  
        needc = *needle;  
        if (ignorecase) {
            if (c >= 65 && c < 97)  
                c += 32;  
            if (needc >= 65 && needc < 97)  
                needc += 32;  
        }  
        if(c == needc) {
            ++needle;  
            if(*needle == '\0') {  
                if (len == needlen)   
                    return 0;  
                else  
                    return i - needlen+1;  
            }  
        }
        else {
            if(*needle == '\0' && needlen > 0)  
                return i - needlen +1;  
            needle = findreset;    
        }
    		i++;
    }    
    return  -1;    
}

//去除尾部空格
char *rtrim(char *str)
{
	if (str == NULL || *str == '\0') {
		return str;
	}
 
	int len = strlen(str);
	char *p = str + len - 1;
	while (p >= str  && (isspace(*p) || (*p) == '\n' || (*p) == '\r' || (*p) == '\t')) {
		*p = '\0';
		--p;
	}
	return str;
}
 
//去除首部空格
char *ltrim(char *str)
{
	if (str == NULL || *str == '\0') {
		return str;
	}
 
	int len = 0;
	char *p = str;
	while (*p != '\0' && (isspace(*p) || (*p) == '\n' || (*p) == '\r' || (*p) == '\t')) {
		++p;
		++len;
	}
 
	memmove(str, p, strlen(str) - len + 1);
 
	return str;
}

//去除首尾空格
char *trim(char *str)
{
	str = rtrim(str);
	str = ltrim(str);
	return str;
}

//去除尾部字符串
char* rtrim_str(char *str, char *remove)
{
	if (str == NULL || *str == '\0') {
		return str;
	}
 
	int len = strlen(str);
	int r_len = strlen(remove);
	
	if(r_len > len) {
		return str;
	}
	
	char *end = str + len - 1;
	char *r_end = remove + r_len - 1;
	
	int remove_flag = 1;
	
	while (end >= str && r_end >= remove) {
		if((*r_end) == (*end)) {
			--r_end;
			--end;
		} else {
			remove_flag = 0;
			break;
		}
	}
	
	if (remove_flag){
		char *end = str + len - 1;
		char *r_end = remove + r_len - 1;
		while (end >= str && r_end >= remove) {
			if((*r_end) == (*end)) {
				*end = '\0';
				--r_end;
				--end;
			} else {
				break;
			}
		}
	}
	
	return str;
}

//去除头部字符串
char *ltrim_str(char *str, char *remove){
	if (str == NULL || *str == '\0') {
		return str;
	}
 
	int len = strlen(str);
	int r_len = strlen(remove);
	
	if(r_len > len) {
		return str;
	}
	
	char *end = str + len - 1;
	char *r_end = remove + r_len - 1;
	
	char *start = str;
	char *r_start = remove;
	
	int remove_flag = 1;
	while (start <= end && r_start <= r_end) {
		if((*start) == (*r_start)) {
			++r_start;
			++start;
		} else {
			remove_flag = 0;
			break;
		}
	}
	
	if(remove_flag) {
		memmove(str, start, len - r_len);
		str[len - r_len] = '\0';
	}
	
	return str;
}

char* yaf_itoa(long num, char* str) {
	int radix = 10; //十进制
	memset(str, 0, MAP_ITOA_INT_SIZE);
    char index[]="0123456789ABCDEF";
    unsigned long unum;
    int i=0,j,k;
    if (radix==10&&num<0) {
        unum=(unsigned long)-num;
        str[i++]='-';
    } else unum=(unsigned long)num;
    do {
        str[i++]=index[unum%(unsigned long)radix];
        unum/=radix;
    } while (unum);
    str[i]='\0';
    if (str[0]=='-')k=1;
    else k=0;
    char temp;
    for (j=k;j<=(i-1)/2;j++) {
        temp=str[j];
        str[j]=str[i-1+k-j];
        str[i-1+k-j]=temp;
    }
    return str;
}

char* strreplace(char* original, char const * const pattern, char const * const replacement)
{
    size_t const replen = strlen(replacement);
    size_t const patlen = strlen(pattern);
    size_t const orilen = strlen(original);

    size_t patcnt = 0;
    const char * oriptr;
    const char * patloc;

    for (oriptr = original; (patloc = strstr(oriptr, pattern)); oriptr = patloc + patlen) {
        patcnt++;
    }
    // allocate memory for the new string
    size_t const retlen = orilen + patcnt * (replen - patlen);
    char * const returned = (char *) emalloc( sizeof(char) * (retlen + 1) );

    if (returned != NULL) {
        // copy the original string,
        // replacing all the instances of the pattern
        char * retptr = returned;
        for (oriptr = original; (patloc = strstr(oriptr, pattern)); oriptr = patloc + patlen) {
            size_t const skplen = patloc - oriptr;
            // copy the section until the occurence of the pattern
            strncpy(retptr, oriptr, skplen);
            retptr += skplen;
            // copy the replacement
            strncpy(retptr, replacement, replen);
            retptr += replen;
        }
        // copy the rest of the string.
        strcpy(retptr, oriptr);
    }

    size_t val_len = strlen(returned);
    strcpy(original, returned);
    efree(returned);
    return original;
}

/**
 * Serializes php variables without using the PHP userland
 */
void yaf_serialize(zval *return_value, zval *var TSRMLS_DC) 
{
	php_serialize_data_t var_hash;
	smart_str buf = {0};

	PHP_VAR_SERIALIZE_INIT(var_hash);
	yaf_php_var_serialize(&buf, var, &var_hash TSRMLS_CC);
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	if (EG(exception)) {
		smart_str_free(&buf);
		RETURN_FALSE;
	}
    
	if (buf.s) {
		RETURN_STR(buf.s);
	} else {
		RETURN_NULL();
	}
}

/**
 * Unserializes php variables without using the PHP userland
 */
void yaf_unserialize(zval *return_value, zval *var TSRMLS_DC) {

	const unsigned char *p;
	php_unserialize_data_t var_hash;

	if (Z_TYPE_P(var) != IS_STRING) {
		RETURN_FALSE;
	}

	if (Z_STRLEN_P(var) == 0) {
		RETURN_FALSE;
	}
	
	p = (const unsigned char*) Z_STRVAL_P(var);
	PHP_VAR_UNSERIALIZE_INIT(var_hash);
	if (!yaf_php_var_unserialize(&return_value, &p, p + Z_STRLEN_P(var), &var_hash TSRMLS_CC)) {
		PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
		zval_dtor(return_value);
		ZVAL_NULL(return_value);
		if (!EG(exception)) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Error at offset %ld of %d bytes", (long)((char*)p - Z_STRVAL_P(var)), Z_STRLEN_P(var));
		}
		RETURN_FALSE;
	}
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
}