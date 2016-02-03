#include <stdio.h>
#include <stdlib.h>
#include "../json/cJSON.h"
#include "../include/curl/curl.h"
#include <string.h>
#include <sys/types.h>     
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iconv.h>
#include <error.h>
#include "../hisdk/hi_sdk.h"
#include "../mp4v2/joseph_g711a_h264_to_mp4.h"
#include "../mp3ToPcm/mp3ToPcm.h"
#include "pollingCheck.h"
#include "../Common/GlobFunc.h"
#include "../Common/DeviceConfig.h"

int g_getFileSize = 0;

//int g711a_encode(void *pout_buf, int *pout_len, const void *pin_buf, const int in_len);
//reset.g711
//int getPCM(unsigned char *inBuf, int inSize, unsigned char *outBuf, int *outSize);
size_t write_data1(void* buffer,size_t size,size_t nmemb,void *stream)  
{  
    //FILE *fptr = (FILE*)stream;  
    //fwrite(buffer,size,nmemb,fptr); 
    memcpy(stream+g_getFileSize, buffer, size*nmemb);
	g_getFileSize += size*nmemb;
  /* memcpy(buf+g_getFileSize, buffer, size*nmemb);
	g_getFileSize += size*nmemb;
	printf("buf----%s----\n", buffer);
	printf("-------------write_data--------:%d----------\n", size*nmemb);*/
    return size*nmemb;  
} 
//
int Post_head1(char *strUrl, char *strPost, char *strResponse)  
{  
	printf("Post_head---%s---\n", strPost);
	g_getFileSize = 0; //
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
	g_getFileSize = 0; //
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
int GetCheckAndLoadFromServerInfo(const char *strResponse, CHECKSERVERINFO *returnInfo)
{
	int cmdType = 0;
	int voiceFilePlayDelay = 0;
	char voiceFilePath[1024] = {0};

	if(NULL == strResponse)
	{
		LOGOUT("pMsg null\n");
		return -1;
	}
	cJSON * pJson = cJSON_Parse(strResponse);
	if(NULL == pJson)																						 
	{
		// parse faild, return
		LOGOUT("pJson null\n");
		return -1;
	}

	// get string from json
	cJSON * pSub = cJSON_GetObjectItem(pJson, "cmdType");
	if(NULL == pSub)
	{
	   //get object named "hello" faild
	   LOGOUT("result null\n");
	   return -1;
	}
	else
	{
		cmdType = atoi(pSub->valuestring);
		LOGOUT("result : %d\n", atoi(pSub->valuestring));
	}
	switch (cmdType)
	{
		
		case IGNORE:// 0
		{
			returnInfo->cmdType = IGNORE;
		}
		break;

		case DOWNLOAD_AUDIOFILE:// 下载音频文件
		{
			//
			returnInfo->cmdType = DOWNLOAD_AUDIOFILE;
			pSub = cJSON_GetObjectItem(pJson, "voiceFilePath");
			if(NULL == pSub)
			{
				//get object named "hello" faild
				 LOGOUT("IPCCmdServerURL null\n");
			}
			else
			{
				//sprintf(voiceFilePath, "%s", pSub->valuestring);
				strcpy(returnInfo->voiceFilePath, pSub->valuestring);
				LOGOUT("voiceFilePath : %s\n", returnInfo->voiceFilePath);
			}
			//
			pSub = cJSON_GetObjectItem(pJson, "voiceFilePlayDelay");
			if(NULL == pSub)
			{
				//get object named "hello" faild
				 LOGOUT("IPCCmdServerURL null\n");
			}
			else
			{
				returnInfo->voiceFilePlayDelay = atoi(pSub->valuestring);
				LOGOUT("voiceFilePlayDelay : %d--\n", returnInfo->voiceFilePlayDelay);
			}
			//
			pSub = cJSON_GetObjectItem(pJson, "voiceFileSize");
			if(NULL == pSub)
			{
				//get object named "hello" faild
				 LOGOUT("IPCCmdServerURL null\n");
			}
			else
			{
				returnInfo->voiceFileSize = atoi(pSub->valuestring);
				LOGOUT("voiceFileSize : %d--\n", returnInfo->voiceFileSize);
			}	
		}
		break;

		case START_EMERGENCYCONDITION:// 设置紧急条件
		{
			//
			returnInfo->cmdType = START_EMERGENCYCONDITION;
			pSub = cJSON_GetObjectItem(pJson, "timeCycle");
			if(NULL == pSub)
			{
				//get object named "hello" faild
				 LOGOUT("timeCycle null\n");
			}
			else
			{
				//sprintf(voiceFilePath, "%s", pSub->valuestring);
				//strcpy(returnInfo->voiceFilePath, pSub->valuestring);
				returnInfo->timeCycle = atoi(pSub->valuestring);
				LOGOUT("timeCycle : %s\n", returnInfo->timeCycle);
			}
			//
			pSub = cJSON_GetObjectItem(pJson, "motionTotal");
			if(NULL == pSub)
			{
				//get object named "hello" faild
				 LOGOUT("motionTotal null\n");
			}
			else
			{
				returnInfo->motionTotal = atoi(pSub->valuestring);
				LOGOUT("motionTotal : %d--\n", returnInfo->motionTotal);
			}
			//
			pSub = cJSON_GetObjectItem(pJson, "motionDiscTotal");
			if(NULL == pSub)
			{
				//get object named "hello" faild
				 LOGOUT("motionDiscTotal null\n");
			}
			else
			{
				returnInfo->motionDiscTotal = atoi(pSub->valuestring);
				LOGOUT("motionDiscTotal : %d--\n", returnInfo->motionDiscTotal);
			}
			//
			pSub = cJSON_GetObjectItem(pJson, "volumeAverage");
			if(NULL == pSub)
			{
				//get object named "hello" faild
				 LOGOUT("volumeAverage null\n");
			}
			else
			{
				returnInfo->volumeAverage = atoi(pSub->valuestring);
				LOGOUT("volumeAverage : %d--\n", returnInfo->volumeAverage);
			}
			//
		}
		break;

		case END_EMERGENCYCONDITION:// 结束紧急条件
		{
			//
			returnInfo->cmdType = END_EMERGENCYCONDITION;
			pSub = cJSON_GetObjectItem(pJson, "timeCycle");
			if(NULL == pSub)
			{
				//get object named "hello" faild
				 LOGOUT("timeCycle null\n");
			}
			else
			{
				//sprintf(voiceFilePath, "%s", pSub->valuestring);
				//strcpy(returnInfo->voiceFilePath, pSub->valuestring);
				returnInfo->timeCycle = atoi(pSub->valuestring);
				LOGOUT("timeCycle : %s\n", returnInfo->timeCycle);
			}
			//
			pSub = cJSON_GetObjectItem(pJson, "motionTotal");
			if(NULL == pSub)
			{
				//get object named "hello" faild
				 LOGOUT("motionTotal null\n");
			}
			else
			{
				returnInfo->motionTotal = atoi(pSub->valuestring);
				LOGOUT("motionTotal : %d--\n", returnInfo->motionTotal);
			}
			//
			pSub = cJSON_GetObjectItem(pJson, "motionDiscTotal");
			if(NULL == pSub)
			{
				//get object named "hello" faild
				 LOGOUT("motionDiscTotal null\n");
			}
			else
			{
				returnInfo->motionDiscTotal = atoi(pSub->valuestring);
				LOGOUT("motionDiscTotal : %d--\n", returnInfo->motionDiscTotal);
			}
			//
			pSub = cJSON_GetObjectItem(pJson, "volumeAverage");
			if(NULL == pSub)
			{
				//get object named "hello" faild
				 LOGOUT("volumeAverage null\n");
			}
			else
			{
				returnInfo->volumeAverage = atoi(pSub->valuestring);
				LOGOUT("volumeAverage : %d--\n", returnInfo->volumeAverage);
			}
			//
			pSub = cJSON_GetObjectItem(pJson, "maxVideoRecordTime");
			if(NULL == pSub)
			{
				//get object named "hello" faild
				 LOGOUT("maxVideoRecordTime null\n");
			}
			else
			{
				returnInfo->maxVideoRecordTime = atoi(pSub->valuestring);
				LOGOUT("maxVideoRecordTime : %d--\n", returnInfo->maxVideoRecordTime);
			}
		}
		break;

	}
	// get string from json
	/*pSub = cJSON_GetObjectItem(pJson, "cmdType");
	if(NULL == pSub)
	{
		//get object named "hello" faild
		 LOGOUT("IPCCmdServerURL null\n");
	}
	else
	{
		sprintf(returnInfo->SwVersion, "%s", pSub->valuestring);
		LOGOUT("IPCCmdServerURL : %s\n", pSub->valuestring);
	}	
	*/
	cJSON_Delete(pJson);
	return 1;
}

int checkAndLoadCmdFromServer(CHECKSERVERINFO *type)
{
	//char sendBuf[] = {0};
	//sprintf(sendBuf, "ipc_id=%s&HwVersion=S0&SwVersion=%s", g_szServerNO, SDK_SYSTEM_FWVERSION) ;
	
	char sendBuf[256] = {0};
	char strUrl[] = "http://ipc.100memory.com/ipccmd_1p5.php?act=checkAndLoadCmdFromServer";
	char strResponse[1024] = {0};
	sprintf(sendBuf, "ipc_id=%s&pwd=%s",  g_szServerNO,g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword);
	Post_head1(strUrl, sendBuf, strResponse);
	if(strResponse == NULL)
	{
		LOGOUT("--------Post_head,return error---\n");
		return -1;
	}
	//LOGINRETURNINFO returnInfo;
	LOGOUT("--------------%s------\n", strResponse);
	GetCheckAndLoadFromServerInfo(strResponse, type);
	printf("----------tst---1\n");
	LOGOUT("type:%d--- \n", type->cmdType);
	return 1;
}

int getMp3(char *url, int fileSize, char *package)
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
	g_getFileSize = 0;
	Get_head(url, sendBuf, package);
	if(package == NULL)
	{
		LOGOUT("--------getIpcVersionPackage,strResponse error---\n");
		return -1;
	}
	FILE *fp = NULL;
	fp = fopen("PlaySound.mp3", "a+");
	if(fp != NULL)
	{
		int writeLen = fwrite(package, 1, g_getFileSize, fp);
		if(writeLen != g_getFileSize)
		{
			LOGOUT("write error----\n");
		}
		fclose(fp);
	}
	printf("g_getFileSize==%d----\n", g_getFileSize);
	if(g_getFileSize != fileSize)//下载下来的固件大小不匹配
	{
	//	free(strResponse);//释放空间
		system("rm -rf PlaySound.mp3");
		LOGOUT("--------getIpcVersionPackage, packageSize error,down size:%d, service version size:%d---\n", g_getFileSize, fileSize);
		return -1;
	}
	return 1;
}
//
long GetTickCount()
{
	struct timeval t_start; 
	//get start time 
	gettimeofday(&t_start, NULL); 
	//long start = ((long)t_start.tv_sec)*1000+(long)t_start.tv_usec/1000; 
	long start = (long)t_start.tv_sec;
	return start;
}
//
void PollingContent()
{
	CHECKSERVERINFO type;
	char *mp3Buf = NULL;
	unsigned char *outPutPcm = NULL;
	int pcmSize = 0;
	char *g9711a = NULL;
	int g9711aSize = 0;
	int ret = 0;
	checkAndLoadCmdFromServer(&type);
	LOGOUT("main type:%d--- \n", type.cmdType);
	LOGOUT("main type:%s--- \n", type.voiceFilePath);
	if(type.cmdType == DOWNLOAD_AUDIOFILE)
	{
		if(type.voiceFileSize > 5*1024*1024 || type.voiceFileSize < 10)
		{
			LOGOUT("voiceFileSize:%d size error--- \n", type.voiceFileSize);
			return -1;
		}
		mp3Buf = (char*)malloc(type.voiceFileSize);
		ret =getMp3(type.voiceFilePath, type.voiceFileSize, mp3Buf);
		LOGOUT("g9711a write size:%d-error----\n", type.voiceFileSize);
		if(ret == 1)
		{
			outPutPcm = (unsigned char *)malloc(type.voiceFileSize*8);
			ret =getPCM(mp3Buf, type.voiceFileSize, outPutPcm, &pcmSize);
			if(ret > type.voiceFileSize)
			{
				g9711a = (unsigned char *)malloc(pcmSize*2);
				g9711aSize = pcmSize*2;
				ret = g711a_encode(g9711a, &g9711aSize, mp3Buf, pcmSize);
				if(ret > 50)
				{
					FILE *fp = NULL;
					fp = fopen("reset.g711", "w+");
					if(fp != NULL)
					{
						int writeLen = fwrite(g9711a, 1, g9711aSize, fp);
						if(writeLen != g9711aSize)
						{
							LOGOUT("g9711a write size:%d-error----\n", writeLen);
						}
						fclose(fp);
					}
				}
				else
				{
					LOGOUT("g9711a error:%d-error----\n", ret);
				}
			}
			else
			{
				LOGOUT("getPCM error:%d-error----\n", ret);
			}
			//int g711a_encode(void *pout_buf, int *pout_len, const void *pin_buf, const int in_len);
			//reset.g711
		}
		else
		{
			LOGOUT("getMp3 error:%d-error----\n", ret);
		}
		if(outPutPcm != NULL)
		{
			free(outPutPcm);
			outPutPcm = NULL;
		}
		if(mp3Buf != NULL)
		{
			free(mp3Buf);
			mp3Buf = NULL;
		}
		if(g9711a != NULL)
		{
			free(g9711a);
			g9711a = NULL;
		}
	}
	else if(type.cmdType == START_EMERGENCYCONDITION)
	{
		//
	}
	else if(type.cmdType == END_EMERGENCYCONDITION)
	{
		//
	}

}
//
int isP2PFun()
{

	unsigned long long socketNums = 0;
	GetSendSocketTraffic("tutk", &socketNums);
	if(socketNums > 50)
	{
		return 1;
	}
	return 0;
}
//
void *PollingcheckThread(void *param)
{
	long nowSeconds = 0;
	long lastSeconds = GetTickCount();
	int isP2P = isP2PFun();
	while(1)
	{
		LOGOUT("PollingcheckThread---0 \n");
		isP2P = isP2PFun();
		nowSeconds = GetTickCount();
		sleep(1);
		if(nowSeconds - lastSeconds > 60 )
		{
			LOGOUT("PollingcheckThread---1 \n");
			lastSeconds = GetTickCount();
			PollingContent();
			
			sleep(100);
			continue;
			
		}
		else if(nowSeconds - lastSeconds > 2 && isP2P == 1)
		{
			lastSeconds = GetTickCount();
			PollingContent();
			sleep(100);
			continue;
		}
	}

}

