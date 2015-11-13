#include <stdio.h>
#include <stdlib.h>
#include "json/cJSON.h"
#include "curl/curl.h"
#include <string.h>
#include <sys/types.h>     
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iconv.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "update.h"
#include "LogOut/LogOut.h"
#include "Common/DeviceConfig.h"


//#include "../LogOut/LogOut.h"
//#include "../Common/DeviceConfig.h"
//#define LOGOUT printf

extern char g_szHardVersion[128];
extern char g_szSoftVersion[128];
extern char g_szOtherServerNO[80];

//g_szOtherServerNO
//
int pos = 0;
//
/*void parseJson(char * pMsg)
{
   if(NULL == pMsg)
   {
		printf("pMsg null\n");
	   return;
   }
   cJSON * pJson = cJSON_Parse(pMsg);
   if(NULL == pJson)																						 
   {
	   // parse faild, return
	   printf("pJson null\n");
	 return ;
   }

   // get string from json
   cJSON * pSub = cJSON_GetObjectItem(pJson, "result");
   if(NULL == pSub)
   {
	   //get object named "hello" faild
   }
   printf("result : %d\n", pSub->valueint);

   
   // get string from json
  pSub = cJSON_GetObjectItem(pJson, "IPCCmdServerURL");
   if(NULL == pSub)
   {
	   //get object named "hello" faild
	   return;
   }
   printf("IPCCmdServerURL : %d\n", pSub->valueint);
   
   cJSON_Delete(pJson);
}*/
//
int getStrTime(char *buf)
{
	if(NULL == buf)
	{
		//LOGOUT("getStrTime param error\n");
		printf("getStrTime param error\n");
		return -1;
	}
	static int getTime = 0;
	struct tm nowTime;
	time_t   now;         
	time(&now);
	localtime_r(&now, &nowTime);
	sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", 
			nowTime.tm_year + 1900, nowTime.tm_mon+1, nowTime.tm_mday, nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);
	return 1;
}

//
int getServiceInfo(const char *strResponse, SERVICEVERSION *returnInfo)
{
	if(NULL == strResponse)
	{
		LOGOUT("pMsg null");
		return -1;
	}
	cJSON * pJson = cJSON_Parse(strResponse);
	if(NULL == pJson)																						 
	{
		// parse faild, return
		LOGOUT("pJson null");
		return -1;
	}

	// get string from json
	cJSON * pSub = cJSON_GetObjectItem(pJson, "result");
	if(NULL == pSub)
	{
	   //get object named "hello" faild
	   LOGOUT("result null");
	   return -1;
	}
	else
	{
		returnInfo->result = pSub->valueint;
		LOGOUT("result : %d", pSub->valueint);
	}
	// get string from json
	pSub = cJSON_GetObjectItem(pJson, "SwVersion");
	if(NULL == pSub)
	{
		//get object named "hello" faild
		 LOGOUT("SwVersion null");
	}
	else
	{
		sprintf(returnInfo->SwVersion, "%s", pSub->valuestring);
		LOGOUT("SwVersion : %s", pSub->valuestring);
	}
	// get OSSbucketName string from json
	pSub = cJSON_GetObjectItem(pJson, "imageFileBytes");
	if(NULL == pSub)
	{
		//get object named "hello" faild
		 LOGOUT("imageFileBytes null");
	}
	else
	{
		//sprintf(returnInfo->OSSbucketName, "%s", pSub->valuestring);
		returnInfo->imageFileBytes = atoi(pSub->valuestring);
		LOGOUT("imageFileBytes : %s", pSub->valuestring);
	}
	// get OSSEndpoint string from json
	pSub = cJSON_GetObjectItem(pJson, "imageFileUrl");
	if(NULL == pSub)
	{
		LOGOUT("imageFileUrl null");
	}
	else
	{
		sprintf(returnInfo->imageFileUrl, "%s", pSub->valuestring);
		LOGOUT("imageFileUrl : %s", returnInfo->imageFileUrl);
	}
	
	cJSON_Delete(pJson);
	return 1;
}


size_t write_data1(void* buffer,size_t size,size_t nmemb,void *stream)  
{  
    //FILE *fptr = (FILE*)stream;  
    //fwrite(buffer,size,nmemb,fptr); 
    memcpy(stream+pos, buffer, size*nmemb);
	pos += size*nmemb;
  /* memcpy(buf+pos, buffer, size*nmemb);
	pos += size*nmemb;
	printf("buf----%s----\n", buffer);
	printf("-------------write_data--------:%d----------\n", size*nmemb);*/
    return size*nmemb;  
} 

//
int Post_head1(char *strUrl, char *strPost, char *strResponse)  
{  
	printf("Post_head---%s---\n", strPost);
	pos = 0; //
	CURL *curl;  
    CURLcode res;  
    FILE* fptr;  
    struct curl_slist *http_header = NULL;  
  
   /* if ((fptr = fopen(FILENAME,"w")) == NULL)  
    {  
        fprintf(stderr,"fopen file error:%s\n",FILENAME);  
        return -1;  
    }  */
  
    curl = curl_easy_init();  
    if (!curl)  
    {  
        fprintf(stderr,"curl init failed\n");  
        return -1;  
    }  
	  curl_easy_setopt(curl,CURLOPT_URL,strUrl); //url地址  
	  
//	 http_header = curl_slist_append(NULL, "Content-Type:application/json;charset=UTF-8");
	 http_header = curl_slist_append(NULL, "Content-Type:application/x-www-form-urlencoded");
	 curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_header); 
	 
  	//char sendBuf[] = "id=LB1GB29HYM3M8HJ7111C&pwd=123456";
	//printf("sendBuf-----%s----\n", sendBuf);
   // curl_easy_setopt(curl,CURLOPT_POSTFIELDS,strPost); //post参数  
	curl_easy_setopt(curl,CURLOPT_POSTFIELDS,strPost);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_data1); //对返回的数据进行操作的函数地址  
   // curl_easy_setopt(curl,CURLOPT_WRITEDATA,fptr); //这是write_data的第四个参数值  
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,strResponse);
   	curl_easy_setopt(curl,CURLOPT_POST,1); //设置问非0表示本次操作为post  
    curl_easy_setopt(curl,CURLOPT_VERBOSE,1); //打印调试信息  
    curl_easy_setopt(curl,CURLOPT_HEADER,0); //将响应头信息和相应体一起传给write_data  
    curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1); //设置为非0,响应头信息location  
    curl_easy_setopt(curl,CURLOPT_COOKIEFILE,"/Users/zhu/CProjects/curlposttest.cookie");  
  
    res = curl_easy_perform(curl);  
  
    if (res != CURLE_OK)  
    {  
		printf("send error---\n");
		switch(res)  
        {  
            case CURLE_UNSUPPORTED_PROTOCOL:  
                fprintf(stderr,"不支持的协议,由URL的头部指定\n");  
            case CURLE_COULDNT_CONNECT:  
                fprintf(stderr,"不能连接到remote主机或者代理\n");  
            case CURLE_HTTP_RETURNED_ERROR:  
                fprintf(stderr,"http返回错误\n");  
            case CURLE_READ_ERROR:  
                fprintf(stderr,"读本地文件错误\n");  
            default:  
                fprintf(stderr,"返回值:%d\n",res);  
        }  
        return -1;  
    }  
  
    curl_easy_cleanup(curl);  
} 
//
int Get_head(char *strUrl, char *strPost, char *strResponse)  
{  
	printf("Post_head---%s---\n", strPost);
	pos = 0; //
	CURL *curl;  
    CURLcode res;  
    FILE* fptr;  
    struct curl_slist *http_header = NULL;  
  
  /*  if ((fptr = fopen(FILENAME,"w")) == NULL)  
    {  
        fprintf(stderr,"fopen file error:%s\n",FILENAME);  
        return -1;  
    } */ 
  
    curl = curl_easy_init();  
    if (!curl)  
    {  
        fprintf(stderr,"curl init failed\n");  
        return -1;  
    }  
	  curl_easy_setopt(curl,CURLOPT_URL,strUrl); //url地址  
	  
//	 http_header = curl_slist_append(NULL, "Content-Type:application/json;charset=UTF-8");
	 http_header = curl_slist_append(NULL, "Content-Type:application/x-www-form-urlencoded");
	 curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_header); 
	 
  	//char sendBuf[] = "id=LB1GB29HYM3M8HJ7111C&pwd=123456";
	//printf("sendBuf-----%s----\n", sendBuf);
   // curl_easy_setopt(curl,CURLOPT_POSTFIELDS,strPost); //post参数  
	curl_easy_setopt(curl,CURLOPT_HTTPGET,strPost);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_data1); //对返回的数据进行操作的函数地址  
   // curl_easy_setopt(curl,CURLOPT_WRITEDATA,fptr); //这是write_data的第四个参数值  
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,strResponse);
   	curl_easy_setopt(curl,CURLOPT_HTTPGET,1); //设置问非0表示本次操作为post  
    curl_easy_setopt(curl,CURLOPT_VERBOSE,1); //打印调试信息  
    curl_easy_setopt(curl,CURLOPT_HEADER,0); //将响应头信息和相应体一起传给write_data  
    curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1); //设置为非0,响应头信息location  
    curl_easy_setopt(curl,CURLOPT_COOKIEFILE,"/Users/zhu/CProjects/curlposttest.cookie");  
  
    res = curl_easy_perform(curl);  
  
    if (res != CURLE_OK)  
    {  
		printf("send error---\n");
		switch(res)  
        {  
            case CURLE_UNSUPPORTED_PROTOCOL:  
                fprintf(stderr,"不支持的协议,由URL的头部指定\n");  
            case CURLE_COULDNT_CONNECT:  
                fprintf(stderr,"不能连接到remote主机或者代理\n");  
            case CURLE_HTTP_RETURNED_ERROR:  
                fprintf(stderr,"http返回错误\n");  
            case CURLE_READ_ERROR:  
                fprintf(stderr,"读本地文件错误\n");  
            default:  
                fprintf(stderr,"返回值:%d\n",res);  
        }  
        return -1;  
    }  
  
    curl_easy_cleanup(curl);  
} 
//
int getServiceVersion(SERVICEVERSION *returnInfo)
{
	char sendBuf[256] = {0};
	sprintf(sendBuf, "ipc_id=%s&HwVersion=%s&SwVersion=%s",g_szOtherServerNO,g_szHardVersion,g_szSoftVersion);
	//char sendBuf[] = "ipc_id=LB1GB29HYM3M8HJ7111C&HwVersion=S0&SwVersion=123456";
	char strUrl[] = "http://ipc.100memory.com/ipccmd_1p4.php?act=checkTheLatestSwVersion";
	char strResponse[4096] = {0};
	Post_head1(strUrl, sendBuf, strResponse);
	if(strResponse == NULL)
	{
		LOGOUT("--------Post_head,return error---\n");
		return -1;
	}
	//LOGINRETURNINFO returnInfo;
	printf("strResponse=%s-", strResponse);
	getServiceInfo(strResponse, returnInfo);
	printf("result:%d, SwVersion:%s, imageFileBytes:%d, imageFileUrl:%s\n",returnInfo->result, returnInfo->SwVersion, returnInfo->imageFileBytes, returnInfo->imageFileUrl);
	return 1;
}
//
int reportImageUpdateResult(SERVICEVERSION returnInfo, char *operTime)
{
	//g_szOtherServerNO,g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword
	//
	int result = 0;
	char sendBuf[1024] = {0};//"id=LB1GB29HYM3M8HJ7111C&pwd=123456";
	sprintf(sendBuf, "ipc_id=%s&result=%d&HwVersion=%s&SwVersion=%s&updateTime=%s",g_szOtherServerNO, returnInfo.result, g_szHardVersion, g_szSoftVersion, operTime);
	char strUrl[] = "http://ipc.100memory.com/ipccmd_1p4.php?act=reportImageUpdateResult";
	char strResponse[1024] = {0};
	Post_head1(strUrl, sendBuf, strResponse);
	if(strResponse == NULL)
	{
		LOGOUT("--------Post_head,return error---\n");
		return -1;
	}
	return 1;
}
//
int ipcRestartReportImageUpdateResult(char *operTime)
{
	//g_szOtherServerNO,g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword
	//
	int result = 0;
	char sendBuf[1024] = {0};//"id=LB1GB29HYM3M8HJ7111C&pwd=123456";
	sprintf(sendBuf, "ipc_id=%s&result=%d&HwVersion=%s&SwVersion=%s&updateTime=%s",g_szOtherServerNO, 1, g_szHardVersion, g_szSoftVersion, operTime);
	char strUrl[] = "http://ipc.100memory.com/ipccmd_1p4.php?act=reportImageUpdateResult";
	char strResponse[1024] = {0};
	Post_head1(strUrl, sendBuf, strResponse);
	if(strResponse == NULL)
	{
		LOGOUT("--------Post_head,return error---\n");
		return -1;
	}
	return 1;
}

//
int updateFile(char *fileBuf, int fileSize, char *srcFile, char *destFile)
{
	if(srcFile == NULL || destFile == NULL || fileBuf == NULL || fileSize < 10)
	{
		LOGOUT("updateFile param error\n");
		return -1;
	}
	int ret = 0;
	char outFileBuf[1024] = {0};
	FILE* fptr;  
    if ((fptr = fopen(srcFile,"w")) == NULL)  
    {  
        LOGOUT("fopen file error:%s\n",srcFile);  
        return -1;  
    }
	fwrite(fileBuf,1,fileSize,fptr); 
	fclose(fptr);
	sprintf(outFileBuf, "/usr/bin/unzip -o %s -d %s",  srcFile, destFile);
	printf("outFileBuf-----%s-----\n", outFileBuf);
	ret = system("mv /mnt/mtd/ipc/sykj/aliyunOss.bin /mnt/mtd/ipc/sykj/test.bin");
	if(ret != 0)
	{
		LOGOUT("updateFile, mv update error, system return :%d---%s--\n", ret, strerror(errno));
		return -1;
	}
	ret = system(outFileBuf);
	if(ret != 0)
	{
		LOGOUT("updateFile, update error, system return :%d---%s--\n", ret, strerror(errno));
		ret = system("mv /mnt/mtd/ipc/sykj/test.bin /mnt/mtd/ipc/sykj/aliyunOss.bin");
		if(ret != 0)
		{
			LOGOUT("updateFile, mv update error, rest system return :%d---%s--\n", ret, strerror(errno));
			return -1;
		}
		return -1;
	}

	ret = system("rm -rf /mnt/mtd/ipc/sykj/test.bin");
	if(ret != 0)
	{
		LOGOUT("updateFile, update error, system return :%d---%s--\n", ret, strerror(errno));
		return -1;
	}
	return 1;
}
//
int getIpcVersionPackage(char *url, int fileSize, char *package)
{
	if(url == NULL || fileSize < 1 || package == NULL)
	{
		LOGOUT("getIpcVersionPackage, param error-\n");
		return -1;
	}
	char sendBuf[1024] = {0};
	//char *strResponse = NULL;
	int ret = 0;
	//strResponse = (char*)malloc(fileSize);//开辟固件包空间
	if(url == NULL)
	{
		LOGOUT("getIpcVersionPackage url is NULL\n");
		return -1;
	}
	
	Get_head(url, sendBuf, package);
	if(package == NULL)
	{
		LOGOUT("--------getIpcVersionPackage,strResponse error---\n");
		return -1;
	}
	printf("pos==%d----\n", pos);
	if(pos != fileSize)//下载下来的固件大小不匹配
	{
	//	free(strResponse);//释放空间
		LOGOUT("--------getIpcVersionPackage, packageSize error,down size:%d, service version size:%d---\n", pos, fileSize);
		return -1;
	}
	
	return 1;
}
//
int updateFun(char *version, SERVICEVERSION *returnInfo)
{
	int ret = 0;
	char *package = NULL;
	getServiceVersion(returnInfo);
	char ipcHardVersion[128]={0};
	char serverHardVersion[128]={0};
	char *ptr=strchr(version,'_');
	if(ptr)
	{	
		int flag=ptr-version;
		memcpy(ipcHardVersion,version,MIN(flag,sizeof(ipcHardVersion)));
	}
	else
	{
		LOGOUT("IPC version is error %s",version);
		return -1;
	}
	ptr=strchr(returnInfo->SwVersion,'_');
	if(ptr)
	{	
		int flag=ptr-returnInfo->SwVersion;
		memcpy(serverHardVersion,returnInfo->SwVersion,MIN(flag,sizeof(serverHardVersion)));
	}
	else
	{
		LOGOUT("server version is error %s",version);
		return -1;
	}		
	if(strcmp(ipcHardVersion,serverHardVersion) != 0)//服务器版本<=IPC当前版本
	{
		LOGOUT("IPC hardversion:%s != server hardversion:%s",ipcHardVersion,serverHardVersion);
		return -1;
	}
	if(strcmp(returnInfo->SwVersion, version) <= 0)//服务器版本<=IPC当前版本
	{
		LOGOUT("service version:%s <= IPC version:%s, not update",returnInfo->SwVersion, version);
		return -1;
	}
	else
	{
		LOGOUT("service version:%s > IPC version:%s, must update",returnInfo->SwVersion, version);
	}
	//
	if(returnInfo->result != 1 || returnInfo->imageFileBytes < 1 || returnInfo->imageFileUrl == NULL)
	{
		LOGOUT("returnInfo.result:%d, returnInfo.imageFileBytes:%d, returnInfo.imageFileUrl:%s\
		,returnInfo.SwVersion:%s--", returnInfo->result, returnInfo->imageFileBytes, returnInfo->imageFileUrl, returnInfo->SwVersion);
		return -1;
	}
	package = (char*)malloc(returnInfo->imageFileBytes);
	ret = getIpcVersionPackage(returnInfo->imageFileUrl, returnInfo->imageFileBytes, package);
	if(1 != ret)
	{
		free(package);
		LOGOUT("getIpcVersionPackage failed");
		return -1;
	}
	ret = updateFile(package, returnInfo->imageFileBytes, "/mnt/mtd/ipc/tmpfs/syflash/updateVersionFile.zip", "/");
	free(package);//释放空间
	if(ret != 1)
	{
		LOGOUT("--------getIpcVersionPackage,updateFile failed %d ---\n", ret);
		return -1;
	}
	system("chmod -R 777 /mnt/mtd/ipc/sykj");
	return 1;
}
//
int getTime()
{
	static int getTime = 0;
	struct tm nowTime;
	time_t   now;         
	time(&now);
	localtime_r(&now, &nowTime);
	//LOGOUT("%04d-%02d-%02d %02d:%02d:%02d\n", 
	//		nowTime.tm_year + 1900, nowTime.tm_mon+1, nowTime.tm_mday, nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);
	return nowTime.tm_hour;
}
//
void *P_UpdateThread()
{
	int isUpdate = 1;
	int ret  = 0;
	char version[80] = {0};
	char timeBuf[50] = {0};
	SERVICEVERSION returnInfo;
	int updateSecond = 0;
	while(1)
	{
		sleep(5);
		int hour = getTime();
		if(hour == 3 && isUpdate == 1)
		{
			updateSecond = 0;
			while(1)
			{
				memset(&returnInfo, 0, sizeof(returnInfo));
				memset(version, 0, sizeof(version));
				sprintf(version, "%s-%s", g_szHardVersion, g_szSoftVersion);
				LOGOUT("version:%s---\n", version);
				ret = updateFun(version, &returnInfo);
				if(ret == 1)
				{
					memset(timeBuf, 0, sizeof(timeBuf));
					getStrTime(timeBuf);
					reportImageUpdateResult(returnInfo, timeBuf);
					LOGOUT("execute %s",CMDREBOOT)
					system(CMDREBOOT);
				}
				updateSecond ++;
				if(ret == 1 || updateSecond>3)
				{
					break;
				}
			}
			isUpdate = 0;
		}
		else if(hour != 1)
		{
			isUpdate = 1;
		}
	}
}
//
int updateVersion()
{
	char version[80] = {0};
	char timeBuf[50] = {0};
	SERVICEVERSION returnInfo;
	int ret = 0;
	int updateSecond = 0;
	while(1)
	{
		sleep(5);
		memset(&returnInfo, 0, sizeof(returnInfo));
		memset(version, 0, sizeof(version));
		sprintf(version, "%s_%s", g_szHardVersion, g_szSoftVersion);
		printf("version:%s---\n", version);
		ret = updateFun(version, &returnInfo);
		if(ret == 1)
		{
			memset(timeBuf, 0, sizeof(timeBuf));
			getStrTime(timeBuf);
			reportImageUpdateResult(returnInfo, timeBuf);
			LOGOUT("execute %s",CMDREBOOT)
			system(CMDREBOOT);
			break;
		}
		updateSecond ++;
		if(ret == 1 || updateSecond>3)
		{
			break;
		}
	}
	return ret;
}
//
int InitUpdate()
{
	pthread_t pThreadUpdate;
	int iRet = pthread_create(&pThreadUpdate, NULL, P_UpdateThread, NULL);
	if(iRet)
	{
		LOGOUT("Create P_TcpServerSocketThread Fail!!\n");
		//return ERR_THREAD;
		return -1;
	}
	pthread_detach(pThreadUpdate);
	
	return 0;
}




