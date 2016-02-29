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
		//zss++LOGOUT("oss_append_object_from_buffer failure:s->code:%d error_msg:%s size:%d", s->code,s->error_msg,filePos);
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

void init_test_config(oss_config_t *config, int is_oss_domain)
{
    aos_str_set(&config->host, oss_endpoint);
    config->port = 80; 
    aos_str_set(&config->id, access_key_id);
    aos_str_set(&config->key, access_key_secret);
    config->is_oss_domain = is_oss_domain;
}

void init_test_request_options(oss_request_options_t *options, int is_oss_domain)
{
    options->config = oss_config_create(options->pool);
    init_test_config(options->config, is_oss_domain);
    options->ctl = aos_http_controller_create(options->pool, 0);
}

unsigned long get_file_size(const char *file_path)
{
    unsigned long filesize = -1; 
    struct stat statbuff;

    if(stat(file_path, &statbuff) < 0){
        return filesize;
    } else {
        filesize = statbuff.st_size;
    }

    return filesize;
}

#define test_object_base() do {                                         \
        aos_str_set(&bucket, bucket_name);                              \
        aos_str_set(&object, object_name);                              \
    } while(0)

aos_status_t *init_test_multipart_upload(const oss_request_options_t *options, 
    const char *bucket_name, const char *object_name, aos_string_t *upload_id)
{
    char *utf8_object_name;
    aos_string_t bucket;
    aos_string_t object;
    aos_table_t *headers;
    aos_table_t *resp_headers;
    aos_status_t *s;
    oss_acl_e oss_acl;
    int ret;
    int len;

    test_object_base();
    headers = aos_table_make(options->pool, 5);
    oss_acl = OSS_ACL_PUBLIC_READ;

    s = oss_init_multipart_upload(options, &bucket, &object, headers, upload_id, &resp_headers);
    //free memory malloc by utf8proc
    free(utf8_object_name);

    return s;
}

int test_multipart_upload_from_file(char *file_path,char *object_name)
{
	if(aos_http_io_initialize("oss_test", 0) != AOSE_OK)
	{
		LOGOUT(" test_multipart_upload_from_file exit");
		exit(1);
	}	
	aos_pool_t *p;
    aos_string_t bucket;
    aos_string_t object;
    int is_oss_domain = 1;
    oss_request_options_t *options;
    aos_status_t *s;
    oss_upload_file_t *upload_file;
    aos_table_t *upload_part_resp_headers;
    oss_list_upload_part_params_t *params;
    aos_table_t *list_part_resp_headers;
    aos_string_t upload_id;
    aos_list_t complete_part_list;
    oss_list_part_content_t *part_content1;
    oss_complete_part_content_t *complete_content1;
    aos_table_t *complete_resp_headers;
    aos_string_t data;
    int part_num = 1;
    int part_num1 = 2;

    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    init_test_request_options(options, is_oss_domain);
    aos_str_set(&bucket, bucket_name);
    aos_str_set(&object, object_name);

    //init mulitipart
    s = init_test_multipart_upload(options, bucket_name, object_name, &upload_id);
	
    //upload part from file
    upload_file = oss_create_upload_file(p);
    aos_str_set(&upload_file->filename, file_path);
    upload_file->file_pos = 0;
    //upload_file->file_last = 200 * 1024; //200k
    
    //s = oss_upload_part_from_file(options, &bucket, &object, &upload_id,
    //    part_num, upload_file, &upload_part_resp_headers);
    
    //upload_file->file_pos = 200 *1024;//remain content start pos
    upload_file->file_last = get_file_size(file_path);
    
    s = oss_upload_part_from_file(options, &bucket, &object, &upload_id,
        part_num1, upload_file, &upload_part_resp_headers);
    
    //list part
    params = oss_create_list_upload_part_params(p);
    aos_str_set(&params->part_number_marker, "");
    params->max_ret = 10;
    params->truncated = 0;
    aos_list_init(&complete_part_list);
    
    s = oss_list_upload_part(options, &bucket, &object, &upload_id, params, &list_part_resp_headers);


    aos_list_for_each_entry(part_content1, &params->part_list, node) {
        complete_content1 = oss_create_complete_part_content(p);
        aos_str_set(&complete_content1->part_number, part_content1->part_number.data);
        aos_str_set(&complete_content1->etag, part_content1->etag.data);
        aos_list_add_tail(&complete_content1->node, &complete_part_list);
    }

    //complete multipart
    s = oss_complete_multipart_upload(options, &bucket, &object, &upload_id,
        &complete_part_list, &complete_resp_headers);
	
	int iRet=0;
	if(s->code!=200)
		iRet=-1;
	LOGOUT("test_multipart_upload_from_file ok s->code=%d",s->code);
    aos_pool_destroy(p);
	return iRet;
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
	int iRet=test_multipart_upload_from_file(filePath,fileName);
	if(iRet==0)
	{
		LOGOUT("filePath %s upload success %s",filePath,fileName);
	}
	else
	{
		LOGOUT("filePath %s upload failure %s",filePath,fileName);
	}
	return iRet;
}

int upLoadFileBak(char *filePath,char *fileName)
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
	int i=0;
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
					for(i=0;i<10;i++)
					{
						upResult=test_oss_local_from_buf(object_name, data, size, filePos);
						if(upResult==0 || upResult==409)//zss++
						{
							break;
						}
						usleep(100);
					}
				    if(upResult!=0 && upResult!=409)
					{
						LOGOUT("the file %s %s send oss failure %d",filePath,fileName,upResult);
						break;
				    }
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
int 	InitOSSConfig(const char *v_szBucketName,const char *v_szOssEndPoint,
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

int ReleaseOSSConfig()
{
	aos_http_io_deinitialize();
}

int delete_object(char *object_name)
{
	if (aos_http_io_initialize("oss_test", 0) != AOSE_OK)
	{
		exit(1);
	}
	int result = 0;
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
	//char *object_name = "oss_put_get_delete_object_from_file";
	aos_pool_create(&p, NULL);
	// init_ oss_request_options
	int oss_port = 80;
	printf("delete object oss_delete_object--0\n");
	oss_request_options = oss_request_options_create(p);
	oss_request_options->config = oss_config_create(oss_request_options ->pool);
	aos_str_set(&oss_request_options ->config->host,  oss_endpoint);
	oss_request_options->config->port=oss_port;
	aos_str_set(&oss_request_options ->config->id,  access_key_id);
	aos_str_set(&oss_request_options ->config->key,  access_key_secret );
	oss_request_options ->config->is_oss_domain = is_oss_domain;
	oss_request_options ->ctl = aos_http_controller_create(oss_request_options ->pool, 0);
	printf("delete object oss_delete_object--1\n");
	
	aos_str_set(&bucket, bucket_name);
	aos_str_set(&object, object_name);

	//
	printf("delete object oss_delete_object\n");
	 s = oss_delete_object(oss_request_options, &bucket, &object, &resp_headers);
    if (NULL != s && 204 == s->code) 
	{
		LOGOUT("delete object:%s succeeded", object_name);
    }
    else 
	{
		LOGOUT("delete object:%s failed", object_name);
		
    }    
	result = s->code;
	aos_pool_destroy(p);
	aos_http_io_deinitialize();
	return result;
	
}


