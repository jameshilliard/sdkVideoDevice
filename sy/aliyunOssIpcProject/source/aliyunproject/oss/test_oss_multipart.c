#include <time.h>
#include "../LogOut/LogOut.h"
#include "test_oss_multipart.h"
#include "oss_api.h"

static char bucket_name[80]={0};// =  "zs-oss-test-go";
static char oss_endpoint[80]={0};// = "oss-cn-shenzhen.aliyuncs.com";
static char access_key_id[80]={0};// = "YCcIdLNNUAkA4d2K";
static char access_key_secret[80]={0};// = "jwVtojgl6hsxyj7oj86lz0X3T73x5v";


void create_bucket_name()
{
	aos_pool_t *p;
	int is_oss_domain = 1;//是否使用三级域名，可通过is_oss_domain函数初始化
	oss_request_options_t * oss_request_options;
	aos_status_t *s;
	aos_table_t *resp_headers;
	oss_acl_e oss_acl = OSS_ACL_PRIVATE;
	aos_string_t bucket;
	char bucket_name[] = "zs-oss-test-go";

	aos_pool_create(&p, NULL);
	//init_ oss_request_options
	
	char oss_endpoint[] = "oss-cn-shenzhen.aliyuncs.com";
	char access_key_id[] = "YCcIdLNNUAkA4d2K";
	char access_key_secret[] = "jwVtojgl6hsxyj7oj86lz0X3T73x5v";
	int oss_port = 80;
	oss_request_options = oss_request_options_create(p);
	oss_request_options->config = oss_config_create(oss_request_options ->pool);
	aos_str_set(&oss_request_options ->config->host,  oss_endpoint);
	oss_request_options->config->port=oss_port;
	aos_str_set(&oss_request_options ->config->id,  access_key_id);
	aos_str_set(&oss_request_options ->config->key,  access_key_secret );
	oss_request_options ->config->is_oss_domain = is_oss_domain;
	oss_request_options ->ctl = aos_http_controller_create(oss_request_options ->pool, 0);
	
	aos_str_set(&bucket, bucket_name);
	resp_headers = aos_table_make(p, 0);
	s = aos_status_create(p);
	printf("aos_status_create:%d---0\n", s->code);
	s = oss_create_bucket(oss_request_options, &bucket, oss_acl, &resp_headers);
	printf("oss_create_bucket:%d---0\n", s->code);
	aos_pool_destroy(p);

}

int test_oss_local_file(char *object_name,char *data, int size, int *upLoadSize)
{
	if (aos_http_io_initialize("oss_test", 0) != AOSE_OK)
	{
		LOGOUT("aos_http_io_initialize exit");
		return -1; //exit(1) 程序退出关闭所有的IO;
	}
	
	aos_pool_t *p;
	int is_oss_domain = 1;//是否使用三级域名，可通过is_oss_domain函数初始化
	oss_request_options_t * oss_request_options;
	aos_status_t *s;
	aos_table_t *headers;
	aos_table_t *resp_headers;
	aos_string_t bucket;
	aos_string_t object;
	char *bucket_name =  "zs-oss-test-go";
	aos_list_t buffer;
	aos_buf_t *content;

	aos_pool_create(&p, NULL);
	// init_ oss_request_options
	char oss_endpoint[] = "oss-cn-shenzhen.aliyuncs.com";
	char access_key_id[] = "YCcIdLNNUAkA4d2K";
	char access_key_secret[] = "jwVtojgl6hsxyj7oj86lz0X3T73x5v";
	int oss_port = 80;
	
	oss_request_options = oss_request_options_create(p);
	oss_request_options->config = oss_config_create(oss_request_options ->pool);
	aos_str_set(&oss_request_options ->config->host,  oss_endpoint);
	oss_request_options->config->port=oss_port;
	aos_str_set(&oss_request_options ->config->id,  access_key_id);
	aos_str_set(&oss_request_options ->config->key,  access_key_secret );
	oss_request_options ->config->is_oss_domain = is_oss_domain;
	oss_request_options ->ctl = aos_http_controller_create(oss_request_options ->pool, 0);

	aos_str_set(&object, object_name);
	aos_str_set(&bucket, bucket_name);
	headers = aos_table_make(p, 1);
	//apr_table_set(headers, "x-oss-meta-author", "oss"); //object user meta
	resp_headers = aos_table_make(p, 0);
	s = aos_status_create(p);
	printf("aos_status_create:%s--strlen(data)%d-\n", s->code, strlen(data));
	//read object content into buffer
	aos_list_init(&buffer);
	content = aos_buf_pack(oss_request_options->pool, data, size);
	aos_list_add_tail(&content->node, &buffer);
	s = oss_put_object_from_buffer (oss_request_options, &bucket, &object, &buffer, headers, &resp_headers);
	
	//s = oss_append_object_from_buffer(oss_request_options, &bucket, &object, size, &buffer, headers, &resp_headers);
	if(s->code != 200)
	{
		printf("oss_append_object_from_buffer:%d--size:%d----0\n", s->code, size);
		return -1;
	}

	aos_pool_destroy(p);
	aos_http_io_deinitialize();
    return 0;
}


int test_oss_local_from_buf(char *object_name,char *data, int dataSize, int filePos)
{
	if (aos_http_io_initialize("oss_test", 0) != AOSE_OK)
	{
		LOGOUT("aos_http_io_initialize exit");
		return -1;
	}
	aos_pool_t *p;
	int is_oss_domain = 1;//是否使用三级域名，可通过is_oss_domain函数初始化
	oss_request_options_t * oss_request_options;
	aos_status_t *s;
	aos_table_t *headers;
	aos_table_t *resp_headers;
	aos_string_t bucket;
	aos_string_t object;
	aos_list_t buffer;
	aos_buf_t *content;

	aos_pool_create(&p, NULL);
	// init_ oss_request_options
	int oss_port = 80;
	
	oss_request_options = oss_request_options_create(p);
	oss_request_options->config = oss_config_create(oss_request_options ->pool);
	aos_str_set(&oss_request_options ->config->host,  oss_endpoint);
	oss_request_options->config->port=oss_port;
	aos_str_set(&oss_request_options ->config->id,  access_key_id);
	aos_str_set(&oss_request_options ->config->key,  access_key_secret );
	oss_request_options ->config->is_oss_domain = is_oss_domain;
	oss_request_options ->ctl = aos_http_controller_create(oss_request_options ->pool, 0);

	aos_str_set(&object, object_name);
	aos_str_set(&bucket, bucket_name);
	headers = aos_table_make(p, 1);
	//apr_table_set(headers, "x-oss-meta-author", "oss"); //object user meta
	resp_headers = aos_table_make(p, 0);
	s = aos_status_create(p);
	//printf("aos_status_create:%s--strlen(data)%d-\n", s->code, dataSize);
	//read object content into buffer
	aos_list_init(&buffer);
	content = aos_buf_pack(oss_request_options->pool, data, dataSize);
	aos_list_add_tail(&content->node, &buffer);
	//s = oss_put_object_from_buffer (oss_request_options, &bucket, &object, &buffer, headers, &resp_headers);
	
    s = oss_append_object_from_buffer(oss_request_options, &bucket, &object, filePos, &buffer, headers, &resp_headers);
	if(s->code != 200)
	{
		LOGOUT("oss_append_object_from_buffer failure:s->code:%d error_msg:%s size:%d", s->code,s->error_msg,filePos);
		return s->code;
	}
	else
	{
		//LOGOUT("oss_append_object_from_buffer success:s->code:%d error_msg:%s size:%d", s->code,s->error_msg,filePos);
	}
	aos_pool_destroy(p);
	//aos_http_io_deinitialize();
    return 0;
}

int upLoadFile(char *filePath,char *fileName)
{	
	if(fileName==NULL || filePath==NULL)
	{
		LOGOUT("upLoadFile fileName filePath is error\n");
		return -1;
	}
	if(strlen(filePath)==0 || strlen(fileName)==0)
	{
		LOGOUT("upLoadFile strlen fileName filePath is error\n");
		return -1;	
	}
	
	if(0==strlen(bucket_name) || 0==strlen(oss_endpoint) ||
	   0==strlen(access_key_id) || 0==strlen(access_key_secret))
	{
		LOGOUT("oss config  is error\n");
		return -1;	
	}
	
	char *object_name = fileName;
	char *data=malloc(64*1024);
	if(data==NULL)
	{
		LOGOUT("malloc data is error\n");
		return -1;	
	}
	int size = 0;
	int filePos = 0;
	int upLoadSize = 0;
	int upResult = -1;
	if(fileName != NULL)
	{
		int fd= open(filePath,O_RDWR);
		if(fd>0)
		{
			while(1) 
			{ 
				size = read(fd,data,64*1024);
				if(size>0)
				{
					//LOGOUT("fd=%d read size is %d,error=%d",fd,size,errno);
					upResult=test_oss_local_from_buf(object_name, data, size, filePos);
					filePos += size;
				}
				else
				{
					//LOGOUT("fd=%d read size is %d,error=%d",fd,size,errno);
					break;
				}
				usleep(100);
			}
			close(fd);
		}
		else
		{
			upResult = -1;
		}
		
	}
	else
	{
		upResult = -1;
	}
	
	free(data);
	return upResult;
}
void 	InitOSSConfig(const char *v_szBucketName,const char *v_szOssEndPoint,
					  const char *v_szAccessKeyId,const char *v_szAccessKeySecret)
{
	memset(bucket_name,0,sizeof(bucket_name));
	strncpy(bucket_name,v_szBucketName,sizeof(bucket_name));
	memset(oss_endpoint,0,sizeof(oss_endpoint));
	strncpy(oss_endpoint,v_szOssEndPoint,sizeof(oss_endpoint));
	memset(access_key_id,0,sizeof(access_key_id));
	strncpy(access_key_id,v_szAccessKeyId,sizeof(access_key_id));
	memset(access_key_secret,0,sizeof(access_key_secret));
	strncpy(access_key_secret,v_szAccessKeySecret,sizeof(access_key_secret));
	
}

