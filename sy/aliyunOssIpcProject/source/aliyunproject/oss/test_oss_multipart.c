#include <time.h>
#include "test_oss_multipart.h"
#include "oss_api.h"
//#include "../LogOut/LogOut.h"

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
		exit(1);//zss++ why
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
		exit(1); //zss++ why
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
	//printf("aos_status_create:%s--strlen(data)%d-\n", s->code, dataSize);
	//read object content into buffer
	aos_list_init(&buffer);
	content = aos_buf_pack(oss_request_options->pool, data, dataSize);
	aos_list_add_tail(&content->node, &buffer);
	//s = oss_put_object_from_buffer (oss_request_options, &bucket, &object, &buffer, headers, &resp_headers);
	
    s = oss_append_object_from_buffer(oss_request_options, &bucket, &object, filePos, &buffer, headers, &resp_headers);
	if(s->code != 200)
	{
		printf("oss_append_object_from_buffer:%d--size:%d----0\n", s->code, filePos);
		return -1;
	}

	aos_pool_destroy(p);
	aos_http_io_deinitialize();
    return 0;
}

int getFilePath(char *fullName, char *fileName)
{
	if(fullName==NULL || fileName==NULL)
		return -1;
	time_t m_tNowSeconds = time (NULL);
	struct tm *ptm = localtime(&m_tNowSeconds);
	char *ptr=NULL;
	ptr=strstr(fileName,".mp4");
	if(ptr!=NULL)
	{
		sprintf(fullName, "Videos/Ipcid/%04d%02d%02d/%s",ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,fileName);	
		return 0;
	}
	else
	{
		ptr=strstr(fileName,".jpg");
		if(ptr!=NULL)
		{
			sprintf(fullName, "Photos/Ipcid/%04d%02d%02d/%s",ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,fileName);
			return 0;
		}
			
	}
	printf("file is no mp4 or jpg");
	return -2;
}


bool upLoadFile(char *fileName)
{
	//Videos/Ipcid/yymmdd/yyyymmddhhmmss.mp4
	if(fileName==NULL)
	{
		printf("upLoadFile fileName error\n");
		return false;
	}
	char pathBuf[1024] = {0};
	int iRet=getFilePath(pathBuf,fileName);
	if(iRet!=0)
	{
		printf("getFilePath error\n");
		return false;
	}
	
	char *object_name = pathBuf;
	char *data=malloc(256*1024);
	int size = 0;
	int filePos = 0;
	int upLoadSize = 0;
	bool upResult = false;
	if(fileName != NULL)
	{

		FILE* uploadFile = fopen(fileName,"r");
		if(uploadFile)
		{
			while (!feof(uploadFile)) 
			{ 
				size = fread(data, 1, 256*1024, uploadFile);
				if(size > 0)
				{
					test_oss_local_from_buf(object_name, data, size, filePos);
					filePos += size;
				}
			}
			upResult = true;
		}
		else
		{
			upResult = false;
		}
		
	}
	else
	{
		upResult =  false;
	}
	//printf("pathfilename %s---\n", pathBuf);
	free(data);
	return upResult;
}


