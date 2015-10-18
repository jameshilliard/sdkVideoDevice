#include "CuTest.h"
#include "aos_log.h"
#include "aos_util.h"
#include "aos_string.h"
#include "aos_status.h"
#include "oss_auth.h"
#include "oss_util.h"
#include "oss_xml.h"
#include "oss_api.h"
#include "oss_config.h"
#include "oss_test_util.h"

void test_bucket_setup(CuTest *tc)
{
    aos_pool_t *p;
    int is_oss_domain = 1;
    aos_status_t *s;
    oss_request_options_t *options;
    oss_acl_e oss_acl = OSS_ACL_PRIVATE;
    char *object_name1 = "oss_test_object1";
    char *object_name2 = "oss_test_object2";
    char *str = "test c oss sdk";
    aos_table_t *headers1;
    aos_table_t *headers2;

    //set log level, default AOS_LOG_WARN
    aos_log_set_level(AOS_LOG_WARN);

    //set log output, default stderr
    aos_log_set_output(NULL);

    //create test bucket
    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    init_test_request_options(options, is_oss_domain);
    s = aos_status_create(p);
    s = create_test_bucket(options, TEST_BUCKET_NAME, oss_acl);

    CuAssertIntEquals(tc, 200, s->code);

    //create test object
    headers1 = aos_table_make(p, 0);
    headers2 = aos_table_make(p, 0);
    create_test_object(options, TEST_BUCKET_NAME, object_name1, str, headers1);
    create_test_object(options, TEST_BUCKET_NAME, object_name2, str, headers2);

    aos_pool_destroy(p);
}

void test_bucket_cleanup(CuTest *tc)
{
    aos_pool_t *p;
    int is_oss_domain = 1;
    aos_string_t bucket;
    aos_status_t *s;
    oss_request_options_t *options;
    char *object_name1 = "oss_test_object1";
    char *object_name2 = "oss_test_object2";
    aos_table_t *resp_headers;

    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    init_test_request_options(options, is_oss_domain);
    s = aos_status_create(p);

    //delete test object
    delete_test_object(options, TEST_BUCKET_NAME, object_name1);
    delete_test_object(options, TEST_BUCKET_NAME, object_name2);

    //delete test bucket
    resp_headers = aos_table_make(p, 5);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    s= oss_delete_bucket(options, &bucket, &resp_headers);
    CuAssertIntEquals(tc, 204, s->code);

    aos_pool_destroy(p);
}

void test_create_bucket(CuTest *tc)
{
    aos_pool_t *p;
    int is_oss_domain = 1;
    aos_status_t *s;
    oss_request_options_t *options;
    oss_acl_e oss_acl;

    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    init_test_request_options(options, is_oss_domain);
    s = aos_status_create(p);
    oss_acl = OSS_ACL_PRIVATE;

    //create the same bucket twice with same bucket acl
    s = create_test_bucket(options, TEST_BUCKET_NAME, oss_acl);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertStrEquals(tc, NULL, s->error_code);

    //create the same bucket with different bucket acl
    oss_acl = OSS_ACL_PUBLIC_READ;
    s = create_test_bucket(options, TEST_BUCKET_NAME, oss_acl);
    CuAssertIntEquals(tc, 200, s->code);

    aos_pool_destroy(p);
}

void test_delete_bucket(CuTest *tc)
{
    aos_pool_t *p;
    aos_status_t *s;
    aos_string_t bucket;
    oss_acl_e oss_acl;
    int is_oss_domain = 1;
    oss_request_options_t *options;
    aos_table_t *resp_headers;

    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    init_test_request_options(options, is_oss_domain);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    oss_acl = OSS_ACL_PUBLIC_READ;
    s = aos_status_create(p);
    s = create_test_bucket(options, TEST_BUCKET_NAME, oss_acl);
    resp_headers = aos_table_make(p, 5);

    //delete bucket not empty
    s = oss_delete_bucket(options, &bucket, &resp_headers);
    CuAssertIntEquals(tc, 409, s->code);
    CuAssertStrEquals(tc, "BucketNotEmpty", s->error_code);

    aos_pool_destroy(p);
}

void test_get_bucket_acl(CuTest *tc)
{
    aos_pool_t *p;
    aos_string_t bucket;
    int is_oss_domain = 1;
    oss_request_options_t *options;
    aos_table_t *resp_headers;
    aos_status_t *s;
    aos_string_t oss_acl;

    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    init_test_request_options(options, is_oss_domain);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    resp_headers = aos_table_make(p, 5);
    s = aos_status_create(p);
    s = oss_get_bucket_acl(options, &bucket, &oss_acl, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertStrEquals(tc, "public-read", oss_acl.data);

    aos_pool_destroy(p);
}

void test_list_object(CuTest *tc)
{
    aos_pool_t *p;
    aos_string_t bucket;
    oss_request_options_t *options;
    int is_oss_domain = 1;
    aos_table_t *resp_headers;
    aos_status_t *s;
    oss_list_object_params_t *params;
    oss_list_object_content_t *content;
    int size = 0;
    char *key = NULL;

    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    init_test_request_options(options, is_oss_domain);
    params = oss_create_list_object_params(p);
    params->max_ret = 1;
    params->truncated = 0;
    aos_str_set(&params->prefix, "oss_test");
    resp_headers = aos_table_make(p, 5);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    s = aos_status_create(p);
    s = oss_list_object(options, &bucket, params, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertIntEquals(tc, 1, params->truncated);
    CuAssertStrEquals(tc, "oss_test_object1", params->next_marker.data);

    aos_list_for_each_entry(content, &params->object_list, node) {
        ++size;
        key = apr_psprintf(p, "%.*s", content->key.len, content->key.data);
    }
    CuAssertIntEquals(tc, 1 ,size);
    CuAssertStrEquals(tc, "oss_test_object1", key);
    
    size = 0;
    aos_list_init(&params->object_list);
    aos_str_set(&params->marker, params->next_marker.data);
    s = oss_list_object(options, &bucket, params, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertIntEquals(tc, 0, params->truncated);
    aos_list_for_each_entry(content, &params->object_list, node) {
        ++size;
        key = apr_psprintf(p, "%.*s", content->key.len, content->key.data);
    }
    CuAssertIntEquals(tc, 1 ,size);
    CuAssertStrEquals(tc, "oss_test_object2", key);

    aos_pool_destroy(p);
}

CuSuite *test_oss_bucket()
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_bucket_setup);
    SUITE_ADD_TEST(suite, test_create_bucket);
    SUITE_ADD_TEST(suite, test_get_bucket_acl);
    SUITE_ADD_TEST(suite, test_list_object);
    SUITE_ADD_TEST(suite, test_delete_bucket);
    SUITE_ADD_TEST(suite, test_bucket_cleanup);

    return suite;
}
