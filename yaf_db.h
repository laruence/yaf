/*
  +----------------------------------------------------------------------+
  | yaf_db   /   yaf_db                                                |
  +----------------------------------------------------------------------+
  | yaf_db is a php database ORM extension for mysql written in c language |
  +----------------------------------------------------------------------+
  | LICENSE: https://github.com/caohao0730/yaf_db/blob/master/LICENSE|
  +----------------------------------------------------------------------+
  | Author: Cao Hao  <649947921@qq.com>                                  |
  | CreateTime: 2018-11-19                                               |
  +----------------------------------------------------------------------+
*/
#ifndef YAF_DB_H_
#define YAF_DB_H_

#include "zend_variables.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef YAF_STATIC_COMPILATION
#include "php_config.h"
#endif

#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/file.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <sys/un.h>
#include <ext/standard/php_var.h>

extern zend_class_entry * yaf_db_ce_ptr;

YAF_STARTUP_FUNCTION(db);

/////////////////////////////// 公共宏定义 //////////////////////////////////
#ifdef _MSC_VER
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
#else
#include <inttypes.h>   /* uint32_t */
#endif

#if defined(__GNUC__)
#if __GNUC__ >= 3
#define yaf_inline inline __attribute__((always_inline))
#else
#define yaf_inline inline
#endif
#elif defined(_MSC_VER)
#define yaf_inline __forceinline
#else
#define yaf_inline inline
#endif

#define YAF_OK                  0
#define YAF_ERR                -1
#define YAF_AGAIN              -2
#define YAF_BUSY               -3
#define YAF_DONE               -4
#define YAF_DECLINED           -5
#define YAF_ABORT              -6

#define MAP_ITOA_INT_SIZE 16

#ifndef ulong
#define ulong unsigned long
typedef unsigned long ulong_t;
#endif

//内存分配大小
#define ALIGNED_SIZE_32 32
#define MM_ALIGNED_SIZE_16(size) ((size + Z_L(16) - Z_L(1)) & (~(Z_L(16) - Z_L(1)))) //16位内存对齐
#define MM_ALIGNED_SIZE_32(size) ((size + Z_L(32) - Z_L(1)) & (~(Z_L(32) - Z_L(1)))) //32位内存对齐
#define MM_REAL_SIZE(len) (MM_ALIGNED_SIZE_32(len + 1 + sizeof(size_t)))

enum yafErrorCode
{
	YAF_ERROR_PHP_FATAL_ERROR,
	YAF_ERROR_PDO_CONNECT,
	YAF_ERROR_PDO_CHARSET
};

#define PDO_PARAM_NULL 0
#define PDO_PARAM_INT 1
#define PDO_PARAM_STR 2
#define PDO_PARAM_LOB 3
#define PDO_PARAM_STMT 4
#define PDO_PARAM_BOOL 5

#define PDO_FETCH_ASSOC 2
#define PDO_FETCH_COLUMN 7 

#define MAP_KEY_SIZE 16
#define MAX_TABLE_SIZE 48
#define MAX_OPERATOR_SIZE 4

//php 7 wrapper
#if PHP_MAJOR_VERSION < 7
typedef zend_rsrc_list_entry zend_resource;
#define YAF_RETURN_STRING                      RETURN_STRING
#define YAF_Z_ARRVAL_P                         Z_ARRVAL_P
#define IS_TRUE                               1

static inline int yaf_zend_hash_find(HashTable *ht, char *k, int len, void **v)
{
    zval **tmp = NULL;
    if (zend_hash_find(ht, k, len, (void **) &tmp) == SUCCESS)
    {
        *v = *tmp;
        return SUCCESS;
    }
    else
    {
        *v = NULL;
        return FAILURE;
    }
}

#define yaf_zend_hash_index_find               zend_hash_index_find
#define yaf_zend_hash_del                      zend_hash_del
#define yaf_zend_hash_update                   zend_hash_update
#define YAF_ZVAL_STRINGL                       ZVAL_STRINGL
#define YAF_ZEND_FETCH_RESOURCE_NO_RETURN      ZEND_FETCH_RESOURCE_NO_RETURN
#define YAF_ZEND_FETCH_RESOURCE                ZEND_FETCH_RESOURCE
#define YAF_ZEND_REGISTER_RESOURCE             ZEND_REGISTER_RESOURCE
#define YAF_MAKE_STD_ZVAL(p)                   MAKE_STD_ZVAL(p)
#define YAF_ALLOC_INIT_ZVAL(p)                 ALLOC_INIT_ZVAL(p)
#define YAF_SEPARATE_ZVAL(p)
#define YAF_ZVAL_STRING                        ZVAL_STRING
#define YAF_RETVAL_STRINGL                     RETVAL_STRINGL
#define yaf_smart_str                          smart_str
#define yaf_php_var_unserialize                php_var_unserialize
#define yaf_zend_is_callable                   zend_is_callable
#define yaf_zend_is_callable_ex                zend_is_callable_ex
#define yaf_zend_hash_add                      zend_hash_add
#define yaf_zend_hash_index_update             zend_hash_index_update
#define yaf_call_user_function_ex              call_user_function_ex

static yaf_inline int yaf_call_user_function_fast(zval *function_name, zend_fcall_info_cache *fci_cache, zval **retval_ptr_ptr, uint32_t param_count, zval ***params TSRMLS_DC)
{
    zend_fcall_info fci;

    fci.size = sizeof(fci);
    fci.function_table = EG(function_table);
    fci.object_ptr = NULL;
    fci.function_name = function_name;
    fci.retval_ptr_ptr = retval_ptr_ptr;
    fci.param_count = param_count;
    fci.params = params;
    fci.no_separation = 0;
    fci.symbol_table = NULL;

    return zend_call_function(&fci, fci_cache TSRMLS_CC);
}

#define yaf_copy_to_stack(a, b)
#define YAF_DB_GET_TSRMLS                     TSRMLS_FETCH_FROM_CTX(yaf_thread_ctx ? yaf_thread_ctx : NULL)

#define yaf_add_assoc_string                   add_assoc_string
#define yaf_add_assoc_stringl_ex               add_assoc_stringl_ex
#define yaf_add_assoc_stringl                  add_assoc_stringl
#define yaf_add_assoc_double_ex                add_assoc_double_ex
#define yaf_add_assoc_long_ex                  add_assoc_long_ex
#define yaf_add_next_index_stringl             add_next_index_stringl

#define yaf_zval_ptr_dtor                      zval_ptr_dtor
#define yaf_zend_hash_copy                     zend_hash_copy
#define yaf_zval_add_ref                       zval_add_ref
#define yaf_zval_dup(val)                      (val)
#define yaf_zval_free(val)                     (yaf_zval_ptr_dtor(&val))
#define yaf_zend_hash_exists                   zend_hash_exists
#define yaf_php_array_merge(dest,src)          php_array_merge(dest,src,1 TSRMLS_CC)
#define YAF_RETURN_STRINGL                     RETURN_STRINGL
#define YAF_RETVAL_STRING                      RETVAL_STRING
#define yaf_zend_register_internal_class_ex    zend_register_internal_class_ex

#define yaf_zend_call_method_with_0_params     zend_call_method_with_0_params
#define yaf_zend_call_method_with_1_params     zend_call_method_with_1_params
#define yaf_zend_call_method_with_2_params     zend_call_method_with_2_params

typedef int zend_size_t;

#define YAF_HASHTABLE_FOREACH_START(ht, entry)\
    zval **tmp = NULL;\
    for (zend_hash_internal_pointer_reset(ht);\
        zend_hash_has_more_elements(ht) == SUCCESS; \
        zend_hash_move_forward(ht)) {\
        if (zend_hash_get_current_data(ht, (void**)&tmp) == FAILURE) {\
            continue;\
        }\
        entry = *tmp;

#if defined(HASH_KEY_NON_EXISTANT) && !defined(HASH_KEY_NON_EXISTENT)
#define HASH_KEY_NON_EXISTENT HASH_KEY_NON_EXISTANT
#endif

#define YAF_HASHTABLE_FOREACH_START2(ht, k, klen, ktype, entry)\
    zval **tmp = NULL; ulong_t idx;\
    for (zend_hash_internal_pointer_reset(ht); \
            (ktype = zend_hash_get_current_key_ex(ht, &k, &klen, &idx, 0, NULL)) != HASH_KEY_NON_EXISTENT; \
            zend_hash_move_forward(ht)\
        ) { \
    if (zend_hash_get_current_data(ht, (void**)&tmp) == FAILURE) {\
        continue;\
    }\
    entry = *tmp;\
    klen --;
    
#define YAF_HASHTABLE_FOREACH_END() }
#define yaf_zend_read_property                  zend_read_property
#define yaf_zend_hash_get_current_key(a,b,c,d)  zend_hash_get_current_key_ex(a,b,c,d,0,NULL)

static inline int YAF_Z_TYPE_P(zval *z)
{
    if (Z_TYPE_P(z) == IS_BOOL)
    {
        if ((uint8_t) Z_BVAL_P(z) == 1)
        {
            return IS_TRUE;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return Z_TYPE_P(z);
    }
}

#define yaf_php_var_serialize(a,b,c)         php_var_serialize(a,&b,c)
#define yaf_zend_get_executed_filename()     zend_get_executed_filename(TSRMLS_C)
#define IS_TRUE    1
inline int YAF_Z_TYPE_P(zval *z);
#define YAF_Z_TYPE_PP(z)        YAF_Z_TYPE_P(*z)
#define YAF_ZVAL_IS_NULL    ZVAL_IS_NULL

#else /*--------------------------------- PHP Version 7 --------------------------------------------*/
#define yaf_php_var_serialize                php_var_serialize
typedef size_t zend_size_t;
#define ZEND_SET_SYMBOL(ht,str,arr)         zval_add_ref(arr); zend_hash_str_update(ht, str, sizeof(str)-1, arr);

static yaf_inline int Z_BVAL_P(zval *v)
{
    if (Z_TYPE_P(v) == IS_TRUE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

#define yaf_add_assoc_stringl(__arg, __key, __str, __length, __duplicate)   add_assoc_stringl_ex(__arg, __key, strlen(__key), __str, __length)
static yaf_inline int yaf_add_assoc_stringl_ex(zval *arg, const char *key, size_t key_len, char *str, size_t length, int __duplicate)
{
    return add_assoc_stringl_ex(arg, key, key_len - 1, str, length);
}

#define yaf_add_next_index_stringl(arr, str, len, dup)    add_next_index_stringl(arr, str, len)

static yaf_inline int yaf_add_assoc_long_ex(zval *arg, const char *key, size_t key_len, long value)
{
    return add_assoc_long_ex(arg, key, key_len - 1, value);
}

static yaf_inline int yaf_add_assoc_double_ex(zval *arg, const char *key, size_t key_len, double value)
{
    return add_assoc_double_ex(arg, key, key_len - 1, value);
}

#define YAF_Z_ARRVAL_P(z)                          Z_ARRVAL_P(z)->ht

#define YAF_HASHTABLE_FOREACH_START(ht, _val) ZEND_HASH_FOREACH_VAL(ht, _val);  {
#define YAF_HASHTABLE_FOREACH_START2(ht, k, klen, ktype, _val) zend_string *_foreach_key;\
    ZEND_HASH_FOREACH_STR_KEY_VAL(ht, _foreach_key, _val);\
    if (!_foreach_key) {k = NULL; klen = 0; ktype = 0;}\
    else {k = _foreach_key->val, klen=_foreach_key->len; ktype = 1;} {

#define YAF_HASHTABLE_FOREACH_END()                 } ZEND_HASH_FOREACH_END();

#define Z_ARRVAL_PP(s)                             Z_ARRVAL_P(*s)
#define YAF_Z_TYPE_P                                Z_TYPE_P
#define YAF_Z_TYPE_PP(s)                            YAF_Z_TYPE_P(*s)
#define Z_STRVAL_PP(s)                             Z_STRVAL_P(*s)
#define Z_STRLEN_PP(s)                             Z_STRLEN_P(*s)
#define Z_LVAL_PP(v)                               Z_LVAL_P(*v)

#define yaf_zval_add_ref(p)   Z_TRY_ADDREF_P(*p)
#define yaf_zval_ptr_dtor(p)  zval_ptr_dtor(*p)


#define YAF_PHP_MAX_PARAMS_NUM     20

static yaf_inline int yaf_call_user_function_ex(HashTable *function_table, zval** object_pp, zval *function_name, zval **retval_ptr_ptr, uint32_t param_count, zval ***params, int no_separation, HashTable* ymbol_table TSRMLS_CC)
{
    zval real_params[YAF_PHP_MAX_PARAMS_NUM];
    int i = 0;
    for (; i < param_count; i++)
    {
        real_params[i] = **params[i];
    }
    
    zval *object_p = (object_pp == NULL) ? NULL : *object_pp;
    return call_user_function_ex(function_table, object_p, function_name, *retval_ptr_ptr, param_count, real_params, no_separation, NULL);
}

static yaf_inline int yaf_call_user_function_fast(zval *function_name, zend_fcall_info_cache *fci_cache, zval **retval_ptr_ptr, uint32_t param_count, zval ***params)
{
    zval real_params[YAF_PHP_MAX_PARAMS_NUM];
    int i = 0;
    for (; i < param_count; i++)
    {
        real_params[i] = **params[i];
    }

    zval phpng_retval;
    *retval_ptr_ptr = &phpng_retval;

    zend_fcall_info fci;
    fci.size = sizeof(fci);
#if PHP_MINOR_VERSION == 0
    fci.function_table = EG(function_table);
    fci.symbol_table = NULL;
#endif
    fci.object = NULL;
    ZVAL_COPY_VALUE(&fci.function_name, function_name);
    fci.retval = &phpng_retval;
    fci.param_count = param_count;
    fci.params = real_params;
    fci.no_separation = 0;

    return zend_call_function(&fci, fci_cache);
}

#define YAF_ZVAL_IS_NULL(z)   	(ZVAL_IS_NULL(z) || Z_TYPE_P(z) == IS_UNDEF)
#define yaf_php_var_unserialize(rval, p, max, var_hash)  php_var_unserialize(*rval, p, max, var_hash)
#define YAF_MAKE_STD_ZVAL(p)             zval _stack_zval_##p; p = &(_stack_zval_##p)
#define YAF_ALLOC_INIT_ZVAL(p)           do{p = (zval *)emalloc(sizeof(zval)); bzero(p, sizeof(zval));}while(0)
#define YAF_SEPARATE_ZVAL(p)             zval _##p;\
    memcpy(&_##p, p, sizeof(_##p));\
    p = &_##p
#define YAF_RETURN_STRINGL(s, l, dup)    do{RETVAL_STRINGL(s, l); if (dup == 0) efree(s);}while(0);return
#define YAF_RETVAL_STRINGL(s, l, dup)    do{RETVAL_STRINGL(s, l); if (dup == 0) efree(s);}while(0)
#define YAF_RETVAL_STRING(s, dup)        do{RETVAL_STRING(s); if (dup == 0) efree(s);}while(0)

#define YAF_ZEND_FETCH_RESOURCE_NO_RETURN(rsrc, rsrc_type, passed_id, default_id, resource_type_name, resource_type)        \
        (rsrc = (rsrc_type) zend_fetch_resource(Z_RES_P(*passed_id), resource_type_name, resource_type))
#define YAF_ZEND_REGISTER_RESOURCE(return_value, result, le_result)  ZVAL_RES(return_value,zend_register_resource(result, le_result))

#define YAF_RETURN_STRING(val, duplicate)     RETURN_STRING(val)
#define yaf_add_assoc_string(array, key, value, duplicate)   add_assoc_string(array, key, value)
#define yaf_zend_hash_copy(target,source,pCopyConstructor,tmp,size) zend_hash_copy(target,source,pCopyConstructor)
#define yaf_php_array_merge                                          php_array_merge
#define yaf_zend_register_internal_class_ex(entry,parent_ptr,str)    zend_register_internal_class_ex(entry,parent_ptr)
#define yaf_zend_get_executed_filename()                             zend_get_executed_filename()

#define yaf_zend_call_method_with_0_params(obj, ptr, what, method, retval) \
    zval __retval;\
    zend_call_method_with_0_params(*obj, ptr, what, method, &__retval);\
    if (ZVAL_IS_NULL(&__retval)) *(retval) = NULL;\
    else *(retval) = &__retval;

#define yaf_zend_call_method_with_1_params(obj, ptr, what, method, retval, v1)           \
    zval __retval;\
    zend_call_method_with_1_params(*obj, ptr, what, method, &__retval, v1);\
    if (ZVAL_IS_NULL(&__retval)) *(retval) = NULL;\
    else *(retval) = &__retval;

#define yaf_zend_call_method_with_2_params(obj, ptr, what, method, retval, v1, v2)    \
    zval __retval;\
    zend_call_method_with_2_params(*obj, ptr, what, method, &__retval, v1, v2);\
    if (ZVAL_IS_NULL(&__retval)) *(retval) = NULL;\
    else *(retval) = &__retval;

#define yaf_get_object_handle(object)    	Z_OBJ_HANDLE(*object)
#define YAF_DB_GET_TSRMLS
#define YAF_ZVAL_STRINGL(z, s, l, dup)         ZVAL_STRINGL(z, s, l)
#define YAF_ZVAL_STRING(z,s,dup)               ZVAL_STRING(z,s)
#define yaf_smart_str                          smart_string
#define zend_get_class_entry                  Z_OBJCE_P
#define yaf_copy_to_stack(a, b)                {zval *__tmp = a;\
    a = &b;\
    memcpy(a, __tmp, sizeof(zval));}

static yaf_inline zval* yaf_zval_dup(zval *val)
{
    zval *dup;
    YAF_ALLOC_INIT_ZVAL(dup);
    memcpy(dup, val, sizeof(zval));
    return dup;
}

static yaf_inline void yaf_zval_free(zval *val)
{
    yaf_zval_ptr_dtor(&val);
    efree(val);
}

static yaf_inline zval* yaf_zend_read_property(zend_class_entry *class_ptr, zval *obj, char *s, int len, int silent)
{
    zval rv;
    return zend_read_property(class_ptr, obj, s, len, silent, &rv);
}

static yaf_inline int yaf_zend_is_callable(zval *cb, int a, char **name)
{
    zend_string *key = NULL;
    int ret = zend_is_callable(cb, a, &key);
    char *tmp = estrndup(key->val, key->len);
    zend_string_release(key);
    *name = tmp;
    return ret;
}

static inline int yaf_zend_is_callable_ex(zval *callable, zval *object, uint check_flags, char **callable_name, int *callable_name_len, zend_fcall_info_cache *fcc, char **error TSRMLS_DC)
{
    zend_string *key = NULL;
    int ret = zend_is_callable_ex(callable, NULL, check_flags, &key, fcc, error);
    char *tmp = estrndup(key->val, key->len);
    zend_string_release(key);
    *callable_name = tmp;
    return ret;
}

static inline int yaf_zend_hash_del(HashTable *ht, char *k, int len)
{
    return zend_hash_str_del(ht, k, len - 1);
}

static inline int yaf_zend_hash_add(HashTable *ht, char *k, int len, void *pData, int datasize, void **pDest)
{
    return zend_hash_str_add(ht, k, len - 1, pData) ? SUCCESS : FAILURE;
}

static inline int yaf_zend_hash_index_update(HashTable *ht, int key, void *pData, int datasize, void **pDest)
{
    return zend_hash_index_update(ht, key, pData) ? SUCCESS : FAILURE;
}

static inline int yaf_zend_hash_update(HashTable *ht, char *k, int len, zval *val, int size, void *ptr)
{
    return zend_hash_str_update(ht, (const char*)k, len -1, val) ? SUCCESS : FAILURE;
}

static inline int yaf_zend_hash_find(HashTable *ht, char *k, int len, void **v)
{
		zval *value = zend_hash_str_find(ht, k, len);
    if (value == NULL)
    {
        return FAILURE;
    }
    else
    {
        *v = (void *) value;
        return SUCCESS;
    }
}

static inline int yaf_zend_hash_index_find(HashTable *ht, int index, void **pData)
{
		*pData = zend_hash_index_find(ht, index);
    if (*pData == NULL)
    {
        return FAILURE;
    }
    else
    {
        return SUCCESS;
    }
}

static inline int yaf_zend_hash_exists(HashTable *ht, char *k, int len)
{
    zval *value = zend_hash_str_find(ht, k, len - 1);
    if (value == NULL)
    {
        return FAILURE;
    }
    else
    {
        return SUCCESS;
    }
 }

#endif /* PHP Version */

//函数参数
ZEND_BEGIN_ARG_INFO_EX(arginfo_yaf_db_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yaf_db_construct_oo, 0, 0, 1)
ZEND_ARG_ARRAY_INFO(0, options, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yaf_db_select_oo, 0, 0, 2)
ZEND_ARG_INFO(0, table)
ZEND_ARG_INFO(0, join)
ZEND_ARG_INFO(0, columns)
ZEND_ARG_INFO(0, where)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yaf_db_exec_oo, 0, 0, 1)
ZEND_ARG_INFO(0, query)
ZEND_ARG_ARRAY_INFO(0, map, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yaf_db_query_oo, 0, 0, 1)
ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yaf_db_insert_oo, 0, 0, 2)
ZEND_ARG_INFO(0, table)
ZEND_ARG_ARRAY_INFO(0, data, 0)
ZEND_ARG_ARRAY_INFO(0, cache_info, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yaf_db_replace_oo, 0, 0, 2)
ZEND_ARG_INFO(0, table)
ZEND_ARG_ARRAY_INFO(0, data, 0)
ZEND_ARG_ARRAY_INFO(0, cache_info, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yaf_db_update_oo, 0, 0, 2)
ZEND_ARG_INFO(0, table)
ZEND_ARG_ARRAY_INFO(0, data, 0)
ZEND_ARG_INFO(0, where)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yaf_db_delete_oo, 0, 0, 1)
ZEND_ARG_INFO(0, table)
ZEND_ARG_INFO(0, where)
ZEND_END_ARG_INFO()

/////////////////////// 类声明 ///////////////////////
PHP_METHOD(yaf_db, __construct);
PHP_METHOD(yaf_db, __destruct);
PHP_METHOD(yaf_db, select);
PHP_METHOD(yaf_db, initialize);
PHP_METHOD(yaf_db, begin);
PHP_METHOD(yaf_db, commit);
PHP_METHOD(yaf_db, rollback);
PHP_METHOD(yaf_db, exec);
PHP_METHOD(yaf_db, query);
PHP_METHOD(yaf_db, insert);
PHP_METHOD(yaf_db, replace);
PHP_METHOD(yaf_db, insert_id);
PHP_METHOD(yaf_db, update);
PHP_METHOD(yaf_db, delete);
PHP_METHOD(yaf_db, errorCode);
PHP_METHOD(yaf_db, errorInfo);

//私有函数
char* select_context(char* table, zval* map, zval* join, zval* columns, zval* where, char** sql, zval** cache_info);
char* handle_join(zval* join, char* table, char** table_query);
char* column_quote(char* string, char* table_column);
char* get_join_type(char* type);
char* column_push(zval* columns, zval* map, char** column_query);
char* where_clause(zval* where, zval* map, char** where_query, zval** cache_info);
char* where_implode(char* key, zval* data, zval* map, char** where_clause, char* connector);
char* group_by_implode(zval* group, char** group_by_condition);
char* having_implode(zval* having, zval* map, char** having_condition);
char* order_implode(zval* order, char** order_condition);
char* limit_implode(zval* limit, char** limit_condition);
char* handle_where_not_in(zval* not_in_array, char** where_query, zval* map);
char* handle_like_array(zval* like_array, char** where_query, char* column, char* operator, zval* map, char* connector);
char* get_mapkey(char* mapkey);
char* get_mapkey_like(char* mapkey);
zval* add_map(zval* map, char* mapkey, zval* value);
void update_error_info(zval* obj, char* code, char* errmsg);
void update_pdo_error(zval* obj, zval* errorcode, zval* errorinfo);
int is_write_type(char* sql);
int is_insert(char* sql);
int set_cache(zval* cache_obj, zval* cache_key, zval* cache_expire, zval* cache_value);
zval* get_cache(zval* cache_obj, zval* cache_key);
void del_cache(zval* cache_obj, zval* cache_info);

//preg函数
int preg_join_match(char* key, char* join, char* table, char* alias); //匹配join表
int preg_operator_match(char* key, char* column, char* operator); //匹配列名和操作符
int preg_and_or_match(char* key, char* relation); //匹配是否 AND 或者 OR
int preg_table_match(char* key, char* table, char* alias);  //匹配表名和别名

//object函数
zend_class_entry* get_pdo_ce();

/////////////////////// 基础函数 ///////////////////////
#define yaf_php_fatal_error(level, fmt_str, ...)   php_error_docref(NULL TSRMLS_CC, level, fmt_str, ##__VA_ARGS__)
static yaf_inline void zval_debug(char* name, zval* p) {
    php_printf("----------%s----------\n", name);
    php_debug_zval_dump(p, 1);
}
static yaf_inline void string_debug(char* name, char* str) {
    php_printf("%s：[%s] (string)\n", name, str);
}
static yaf_inline void int_debug(char* name, int l) {
    php_printf("%s：[%d] (int)\n", name, l);
}
static yaf_inline void float_debug(char* name, float l) {
    php_printf("%s：[%f] (float)\n", name, l);
}
static yaf_inline void x_debug(char* name, void* l) {
    php_printf("%s：[%x] (point)\n", name, l);
}
static yaf_inline void info_debug(char* name) {
    php_printf("============%s============\n", name);
}
static yaf_inline void term_debug(char* term_info) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, term_info);
}

//////////////////// memory.c ///////////////////////////
static yaf_inline void yaf_emalloc_and_init(void **tmp, size_t size) {
    *tmp = emalloc(size);    //分配空间，并初始化为 0
    memset(*tmp, 0, size);
}

static yaf_inline zval* yaf_zval_copy(zval * source) {
	zval *copy;
	YAF_MAKE_STD_ZVAL(copy);
	*copy = *source;
	zval_copy_ctor(copy);
	return copy;
}

extern char* yaf_multi_memcpy(char * source, int n_str, ...);

//以32位对齐分配内存
static yaf_inline void yaf_string_emalloc_32(char **tmp, int len) {
    size_t real_size = MM_REAL_SIZE(len);
    *tmp = emalloc(real_size);
    memset(*tmp, 0, real_size);
    memcpy(*tmp, &real_size, sizeof(size_t));
    *tmp = *tmp + sizeof(size_t);
}

static yaf_inline void yaf_string_efree_32(char *tmp) {
    efree(tmp - sizeof(size_t));
}

extern char* yaf_multi_memcpy_auto_realloc(char** source, int n_str, ...); //自动扩容

//////////////////// string.c ///////////////////////////
#define yaf_is_space(p) (p != '\0' && (isspace(p) || (p) == '\n' || (p) == '\r' || (p) == '\t'))
#define yaf_is_string_not_empty(p) (p != NULL && p[0] != '\0')
#define yaf_is_string_empty(p) !yaf_is_string_not_empty(p)
#define yaf_init_stack_string(p, str)  YAF_MAKE_STD_ZVAL(p); YAF_ZVAL_STRING(p, str, 1);

#if defined(YAF_USE_JEMALLOC) || defined(YAF_USE_TCMALLOC)
extern yaf_inline char* yaf_strdup(const char *s);
extern yaf_inline char* yaf_strndup(const char *s, size_t n);
#else
#define yaf_strdup              strdup
#define yaf_strndup             strndup
#endif

extern zval* yaf_preg_match(const char* regex_str, char* subject_str);
extern int yaf_strpos(const char *haystack,const char *needle);
extern char* rtrim(char *str);
extern char* ltrim(char *str);
extern char* trim(char *str);
extern char* ltrim_str(char *str, char *remove);
extern char* rtrim_str(char *str, char *remove);
extern char* yaf_itoa(long num,char* str);
extern char* yaf_escape(char* sql);
extern char* strreplace(char* original, char const * const pattern, char const * const replacement);
extern void yaf_serialize(zval *return_value, zval *var TSRMLS_DC);
extern void yaf_unserialize(zval *return_value, zval *var TSRMLS_DC);

/////////////////// array.c ////////////////////////////
extern char* yaf_get_string_from_array_index(zval *array, ulong index);
extern HashTable* yaf_get_array_keys(zval* p);
extern char* yaf_get_array_key_index(zval* p, uint32_t index);
extern zval * yaf_get_element_by_hashtable_index(HashTable *ht, int index);
extern zval * php_yaf_array_get_value(HashTable *ht, char *key);
static yaf_inline void yaf_free_hash(HashTable *ht) {
    zend_hash_destroy(ht);    //释放hash
    FREE_HASHTABLE(ht);
}
extern void yaf_destroy_hash(HashTable *ht);
extern void yaf_destroy_array(zval **array);
extern void yaf_clean_hash(HashTable *ht);
extern void yaf_clean_array(zval **array);
extern int is_set_array_index(HashTable *ht, int index); //数组下标 index 是否为空
extern char* yaf_get_string_from_hashtable_index(HashTable *ht, int index);
extern char* yaf_get_string_from_hashtable(HashTable *ht, char* key);
extern char* yaf_implode(zval *arr, const char *delim_str, char** result);
extern void yaf_array_single_columns(zval** return_single_column_result, zval* data);

/////////////////// operator.c ////////////////////////////
extern int yaf_compare_strict_bool(zval *op1, zend_bool op2 TSRMLS_DC);

/////////////////// net.c ////////////////////////////
php_stream* unix_socket_conn(char *servername);

//////////////////// 公共宏定义 /////////////////////
#define YAF_IS_FALSE(var)        yaf_compare_strict_bool(var, 0 TSRMLS_CC)
#define YAF_IS_TRUE(var)         yaf_compare_strict_bool(var, 1 TSRMLS_CC)
#define YAF_IS_ARRAY(var)   (var != NULL && Z_TYPE_P(var) == IS_ARRAY)
#define YAF_IS_NOT_ARRAY(var)   (!YAF_IS_ARRAY(var))
#define YAF_IS_EMPTY(var)       (var == NULL || Z_TYPE_P(var) == IS_NULL || YAF_IS_FALSE(var) || (Z_TYPE_P(var) == IS_STRING && !Z_STRLEN_P(var)) || !zend_is_true(var))
#define YAF_IS_NOT_EMPTY(var)   (!YAF_IS_EMPTY(var))
#define YAF_IS_NULL(var)       (var == NULL || Z_TYPE_P(var) == IS_NULL)
#define YAF_IS_NOT_NULL(var)   (!YAF_IS_NULL(var))
#define YAF_HASHTABLE_IS_NOT_EMPTY(var)		(var != NULL && zend_hash_num_elements(var))
#define YAF_HASHTABLE_IS_EMPTY(var)   (!YAF_HASHTABLE_IS_NOT_EMPTY(var))

/////////////////// object.c ////////////////////////////
static inline zval* yaf_read_init_property(zend_class_entry *scope, zval *object, char *p, size_t pl TSRMLS_DC) {
    zval* property = yaf_zend_read_property(scope, object, p, pl, 1 TSRMLS_CC);
    if (property == NULL) {
        zend_update_property_null(scope, object, p, pl);
        return yaf_zend_read_property(scope, object, p, pl, 1 TSRMLS_CC);
    } else {
        return property;
    }
}

static inline int yaf_call_user_function_ex_fast(zval** object_pp, char* function_name, zval **retval_ptr_ptr, uint32_t param_count, zval ***params) {
    zval* _function_name;
    
    YAF_MAKE_STD_ZVAL(_function_name);
    YAF_ZVAL_STRING(_function_name, function_name, 1);
    
    if(*retval_ptr_ptr == NULL) YAF_ALLOC_INIT_ZVAL(*retval_ptr_ptr);
    
    int ret = yaf_call_user_function_ex(EG(function_table), object_pp, _function_name, retval_ptr_ptr, param_count, params, 0, NULL TSRMLS_CC);
    
    yaf_zval_ptr_dtor(&_function_name);
    
    return ret;
}

static yaf_inline int yaf_call_user_function_return_bool_or_unsigned(zval** object_pp, char* function_name, uint32_t param_count, zval ***params) {
	int result = 0;
    zval tmp_point;
    zval* retval = &tmp_point, *_function_name;
	
    YAF_MAKE_STD_ZVAL(_function_name);
    YAF_ZVAL_STRING(_function_name, function_name, 1);

    int ret = yaf_call_user_function_ex(EG(function_table), object_pp, _function_name, &retval, param_count, params, 0, NULL TSRMLS_CC);
    yaf_zval_ptr_dtor(&_function_name);
    if (ret == FAILURE) {
    	yaf_zval_ptr_dtor(&retval);
    	return FAILURE;
    }

    //返回 true
#if PHP_MAJOR_VERSION < 7
    if (Z_TYPE_P(retval) == IS_BOOL && Z_BVAL_P(retval))
#else
    if (Z_TYPE_P(retval) == IS_TRUE)
#endif
    {
    	result = 1;
    } else if (Z_TYPE_P(retval) == IS_LONG) { //返回无符号整数
        result = Z_LVAL_P(retval);
    }
    
	yaf_zval_ptr_dtor(&retval);
	return result;
}

static yaf_inline int yaf_call_user_function_construct_fast(zval** object_pp, uint32_t param_count, zval ***params) {
    zval tmp_point;
    zval *construct_ret = &tmp_point;
    
    zval* function_name;
    YAF_MAKE_STD_ZVAL(function_name);
    YAF_ZVAL_STRING(function_name, "__construct", 1);

    int ret = yaf_call_user_function_ex(EG(function_table), object_pp, function_name, &construct_ret, param_count, params, 0, NULL TSRMLS_CC);

    yaf_zval_ptr_dtor(&function_name);
    
    yaf_zval_ptr_dtor(&construct_ret);
    return ret;
}

//错误处理函数
#define RETURN_MY_ERROR(errmsg) update_error_info(thisObject, "E0001", errmsg);yaf_php_fatal_error(E_WARNING, errmsg);RETURN_LONG(-1);

#endif
