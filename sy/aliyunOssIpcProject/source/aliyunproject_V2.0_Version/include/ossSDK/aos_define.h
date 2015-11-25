#ifndef LIBAOS_DEFINE_H
#define LIBAOS_DEFINE_H

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

#include <curl/curl.h>
#include <apr_time.h>
#include <apr_strings.h>
#include <apr_pools.h>
#include <apr_tables.h>
#include <apr_file_io.h>

#ifdef __cplusplus
# define AOS_CPP_START extern "C" {
# define AOS_CPP_END }
#else
# define AOS_CPP_START
# define AOS_CPP_END
#endif

typedef enum {
    HTTP_GET,
    HTTP_HEAD,
    HTTP_PUT,
    HTTP_POST,
    HTTP_DELETE
} http_method_e;

typedef enum {
    AOSE_OK = 0,
    AOSE_OUT_MEMORY = -1000,
    AOSE_OVER_MEMORY,
    AOSE_FAILED_CONNECT,
    AOSE_ABORT_CALLBACK,
    AOSE_INTERNAL_ERROR,
    AOSE_REQUEST_TIMEOUT,
    AOSE_INVALID_ARGUMENT,
    AOSE_INVALID_OPERATION,
    AOSE_CONNECTION_FAILED,
    AOSE_FAILED_INITIALIZE,
    AOSE_NAME_LOOKUP_ERROR,
    AOSE_FAILED_VERIFICATION,
    AOSE_WRITE_BODY_ERROR,
    AOSE_READ_BODY_ERROR,
    AOSE_SERVICE_ERROR,
    AOSE_OPEN_FILE_ERROR,
    AOSE_FILE_SEEK_ERROR,
    AOSE_FILE_INFO_ERROR,
    AOSE_FILE_READ_ERROR,
    AOSE_FILE_WRITE_ERROR,
    AOSE_XML_PARSE_ERROR,
    AOSE_UTF8_ENCODE_ERROR,
    AOSE_UNKNOWN_ERROR,
} aos_error_code_e;

typedef apr_pool_t aos_pool_t;
typedef apr_table_t aos_table_t;
typedef apr_table_entry_t aos_table_entry_t;
typedef apr_array_header_t aos_array_header_t;

#define aos_table_elts(t) apr_table_elts(t)
#define aos_is_empty_table(t) apr_is_empty_table(t)
#define aos_table_make(p, n) apr_table_make(p, n)
#define aos_table_add_int(t, key, value) do {       \
        char value_str[64];                             \
        snprintf(value_str, sizeof(value_str), "%d", value);\
        apr_table_add(t, key, value_str);               \
    } while(0)

#define aos_table_add_int64(t, key, value) do {       \
        char value_str[64];                             \
        snprintf(value_str, sizeof(value_str), "%" APR_INT64_T_FMT, value);\
        apr_table_add(t, key, value_str);               \
    } while(0)

#define aos_table_set_int64(t, key, value) do {       \
        char value_str[64];                             \
        snprintf(value_str, sizeof(value_str), "%" APR_INT64_T_FMT, value);\
        apr_table_set(t, key, value_str);               \
    } while(0)

#define aos_pool_create(n, p) apr_pool_create(n, p)
#define aos_pool_destroy(p) apr_pool_destroy(p)
#define aos_palloc(p, s) apr_palloc(p, s)
#define aos_pcalloc(p, s) apr_pcalloc(p, s)

#define AOS_INIT_WINSOCK 1
#define AOS_MD5_STRING_LEN 32
#define AOS_MAX_URI_LEN 2048
#define AOS_MAX_HEADER_LEN 1024
#define AOS_MAX_QUERY_ARG_LEN 1024
#define AOS_MAX_GMT_TIME_LEN 128

#define AOS_CONNECT_TIMEOUT 10
#define AOS_DNS_CACHE_TIMOUT 60
#define AOS_MIN_SPEED_LIMIT 1024
#define AOS_MIN_SPEED_TIME 15
#define AOS_MAX_MEMORY_SIZE 1024*1024*1024L;

#define aos_abs(value)       (((value) >= 0) ? (value) : - (value))
#define aos_max(val1, val2)  (((val1) < (val2)) ? (val2) : (val1))
#define aos_min(val1, val2)  (((val1) > (val2)) ? (val2) : (val1))

#define LF     (char) 10
#define CR     (char) 13
#define CRLF   "\x0d\x0a"

#define aos_version    0000001
#define AOS_VERSION    "0.0.3"
#define AOS_VER        "libaos_" AOS_VERSION

#endif
