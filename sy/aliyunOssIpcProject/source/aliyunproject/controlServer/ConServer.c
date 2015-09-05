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
#include "ConServer.h"

size_t write_data(void* buffer,size_t size,size_t nmemb,void *stream)  
{  
    memcpy(stream, buffer, size*nmemb);
    return size*nmemb;  
} 

int Post_head(char *strUrl, char *strPost, char *strResponse)  
{  
	CURL *curl;  
    CURLcode res;  
    FILE* fptr;  
    struct curl_slist *http_header = NULL;  
    if ((fptr = fopen(CURLFILENAME,"w")) == NULL)  
    {  
        fprintf(stderr,"fopen file error:%s\n",CURLFILENAME);  
        return -1;  
    }  
  
    curl = curl_easy_init();  
    if (!curl)  
    {  
        fprintf(stderr,"curl init failed\n");  
        return -1;  
    }  
	curl_easy_setopt(curl,CURLOPT_URL,strUrl); //url地址  

	//http_header = curl_slist_append(NULL, "Content-Type:application/json;charset=UTF-8");
	http_header = curl_slist_append(NULL, "Content-Type:application/x-www-form-urlencoded");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_header); 

	//char sendBuf[] = "id=LB1GB29HYM3M8HJ7111C&pwd=123456";
	//printf("sendBuf-----%s----\n", sendBuf);
	//curl_easy_setopt(curl,CURLOPT_POSTFIELDS,strPost); //post参数  
	curl_easy_setopt(curl,CURLOPT_POSTFIELDS,strPost);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_data); //对返回的数据进行操作的函数地址  
	//curl_easy_setopt(curl,CURLOPT_WRITEDATA,fptr); //这是write_data的第四个参数值  
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
	return 0;
} 


void parseJson(char * pMsg)
{
	if(NULL == pMsg)
	{
		printf("pMsg null\n");
		return;
	}
	cJSON * pJson = cJSON_Parse(pMsg);
	if(NULL == pJson)																						 
	{
		printf("pJson null\n");
		return ;
	}
	cJSON * pSub = cJSON_GetObjectItem(pJson, "result");
	if(NULL == pSub)
	{

	}
	printf("result : %d\n", pSub->valueint);
	pSub = cJSON_GetObjectItem(pJson, "IPCCmdServerURL");
	if(NULL == pSub)
	{
		return;
	}
	printf("IPCCmdServerURL : %d\n", pSub->valueint);
	cJSON_Delete(pJson);
}

int getLoginInfo(const char *strResponse, LOGINRETURNINFO *returnInfo)
{
	if(NULL == strResponse || NULL == returnInfo)
	{
		LOGOUT("param null");
		return -1;
	}
	cJSON * pJson = cJSON_Parse(strResponse);
	if(NULL == pJson)																						 
	{
		LOGOUT("pJson null");
		return -2;
	}
	cJSON * pSub = cJSON_GetObjectItem(pJson, "result");
	if(NULL == pSub)
	{
	   
	   LOGOUT("result null");
	   return -3;
	}
	else
	{
		returnInfo->result = pSub->valueint;
		LOGOUT("result:%d", pSub->valueint);
	}
	// get string from json
	pSub = cJSON_GetObjectItem(pJson, "IPCCmdServerURL");
	if(NULL == pSub)
	{
		 LOGOUT("IPCCmdServerURL null");
	}
	else
	{
		sprintf(returnInfo->IPCCmdServerURL, "%s", pSub->valuestring);
		LOGOUT("IPCCmdServerURL : %s", pSub->valuestring);
	}
	// get OSSbucketName string from json
	pSub = cJSON_GetObjectItem(pJson, "OSSbucketName");
	if(NULL == pSub)
	{
		 LOGOUT("OSSbucketName null");
	}
	else
	{
		sprintf(returnInfo->OSSbucketName, "%s", pSub->valuestring);
		LOGOUT("OSSbucketName : %s", pSub->valuestring);
	}
	// get OSSEndpoint string from json
	pSub = cJSON_GetObjectItem(pJson, "OSSEndpoint");
	if(NULL == pSub)
	{
		 printf("OSSEndpoint null");
	}
	else
	{
		sprintf(returnInfo->OSSEndpoint, "%s", pSub->valuestring);
		LOGOUT("OSSEndpoint : %s", pSub->valuestring);
	}
	// get accessKeyId string from json
	pSub = cJSON_GetObjectItem(pJson, "accessKeyId");
	if(NULL == pSub)
	{
		 LOGOUT("accessKeyId null");
	}
	else
	{
		sprintf(returnInfo->accessKeyId, "%s", pSub->valuestring);
		LOGOUT("accessKeyId : %s", pSub->valuestring);
	}
	// get accesskeySecret string from json
	pSub = cJSON_GetObjectItem(pJson, "accesskeySecret");
	if(NULL == pSub)
	{
		LOGOUT("accesskeySecret null");
	}
	else
	{
		sprintf(returnInfo->accesskeySecret, "%s", pSub->valuestring);
		LOGOUT("accesskeySecret : %s", pSub->valuestring);
	}
	// get resolutionRatio int from json
	pSub = cJSON_GetObjectItem(pJson, "resolutionRatio");
	if(NULL == pSub)
	{
		 LOGOUT("resolutionRatio null");
	}
	else
	{
		//sprintf(returnInfo->accesskeySecret, "%s", pSub->valuestring)
		returnInfo->resolutionRatio = pSub->valueint;
		LOGOUT("resolutionRatio : %d", pSub->valueint);
	}
	// get resolutionRatio int from json
	pSub = cJSON_GetObjectItem(pJson, "codeStream");
	if(NULL == pSub)
	{
		 LOGOUT("codeStream null");
	}
	else
	{
		returnInfo->codeStream = pSub->valueint;
		LOGOUT("codeStream : %d", pSub->valueint);
	}
	// get frameRate int from json
	pSub = cJSON_GetObjectItem(pJson, "frameRate");
	if(NULL == pSub)
	{
		 LOGOUT("frameRate null");
	}
	else
	{
		returnInfo->frameRate = pSub->valueint;
		LOGOUT("frameRate : %d", pSub->valueint);
	}
	// get streamType int from json
	pSub = cJSON_GetObjectItem(pJson, "streamType");
	if(NULL == pSub)
	{
		
		 LOGOUT("streamType null");
	}
	else
	{
		returnInfo->streamType = pSub->valueint;
		LOGOUT("streamType : %d", pSub->valueint);
	}
	// get mainFrameGap int from json
	pSub = cJSON_GetObjectItem(pJson, "mainFrameGap");
	if(NULL == pSub)
	{
		LOGOUT("mainFrameGap null");
	}
	else
	{
		returnInfo->mainFrameGap = pSub->valueint;
		LOGOUT("mainFrameGap : %d", pSub->valueint);
	}
	// get isRequireToReboot int from json
	pSub = cJSON_GetObjectItem(pJson, "isRequireToReboot");
	if(NULL == pSub)
	{
		LOGOUT("isRequireToReboot null");
	}
	else
	{
		returnInfo->mainFrameGap = pSub->valueint;
		LOGOUT("isRequireToReboot : %d", pSub->valueint);
	}
	cJSON_Delete(pJson);
	return 0;
}

int loginCtrl(const char *server,int port,const char *v_szId,const char *v_szPwd,LOGINRETURNINFO *returnInfo)
{
	if(server==NULL || v_szId==NULL || v_szPwd==NULL || returnInfo==NULL)
	{
		LOGOUT("param is error");
		return -1;
	}
	//char sendBuf[] = "id=LB1GB29HYM3M8HJ7111C&pwd=123456";
	//char strUrl[] = "http://cgtx.100memory.com/ipccmd.php?act=login";
	char sendBuf[1024]={0};
	char strUrl[1024]={0};
	char strResponse[1024] = {0};
	memset(sendBuf,0,sizeof(sendBuf));
	memset(strUrl,0,sizeof(strUrl));
	sprintf(sendBuf,"id=%s&pwd=%s",v_szId,v_szPwd);
	sprintf(strUrl,STRING_LOGIN_CONSERVER,server,port);
	int iRet=Post_head(strUrl, sendBuf, strResponse);
	if(iRet!=0)
	{
		LOGOUT("%s %d failure",strUrl,sendBuf);
		return -2;
	}
	if(strResponse == NULL)
	{
		LOGOUT("--------Post_head,return error---");
		return -3;
	}
	iRet=getLoginInfo(strResponse, returnInfo);
	return iRet;
}

int exitCtrl(const char *server,int port,const char *v_szId,const char *v_szPwd)
{
	if(server==NULL || v_szId==NULL || v_szPwd==NULL)
	{
		LOGOUT("param is error");
		return -1;
	}
	//char sendBuf[] = "id=LB1GB29HYM3M8HJ7111C&pwd=123456";
	//char strUrl[] = "http://cgtx.100memory.com/ipccmd.php?act=logout";
	int result=-1;
	char sendBuf[1024]={0};
	char strUrl[1024]={0};
	char strResponse[1024] = {0};
	memset(sendBuf,0,sizeof(sendBuf));
	memset(strUrl,0,sizeof(strUrl));
	sprintf(sendBuf,"id=%s&pwd=%s",v_szId,v_szPwd);
	sprintf(strUrl,STRING_LOGOUT_CONSERVER,server,port);
	
	int iRet=Post_head(strUrl, sendBuf, strResponse);
	if(iRet!=0)
	{
		LOGOUT("%s %d failure",strUrl,sendBuf);
		return -1;
	}
	if(strResponse == NULL)
	{
		LOGOUT("--------Post_head,return error---");
		return -1;
	}
	if(NULL == strResponse)
	{
		LOGOUT("pMsg null");
		return 0;
	}
	cJSON * pJson = cJSON_Parse(strResponse);
	if(NULL == pJson)																						 
	{
		LOGOUT("pJson null");
		return 0;
	}
	cJSON * pSub = cJSON_GetObjectItem(pJson, "result");
	if(NULL == pSub)
	{
		 LOGOUT("exitCtrl null");
	}
	else
	{
		result = pSub->valueint;
		LOGOUT("exitCtrl : %d", pSub->valueint);
	}
	cJSON_Delete(pJson);
	return result;
}

	

int dataRecord(const char *server,int port,const char *v_szId, char *videoPath, 
			   long creatTimeInMilSecond, int videoFileSize,char *jpgFilePath, 
			   int videoTimeLength,Motion_Data mMotionData)
{
	if(server==NULL || v_szId==NULL)
	{
		LOGOUT("param is error");
		return -1;
	}	

	int result = 0;
	char sendBuf[2048]={0};
	char strUrl[1024]={0};
	char strResponse[1024] = {0};
	memset(sendBuf,0,sizeof(sendBuf));
	memset(strUrl,0,sizeof(strUrl));
	sprintf(sendBuf,STRING_RECORD_CONSERVER, v_szId, videoPath, creatTimeInMilSecond, videoFileSize,jpgFilePath, videoTimeLength,
					 mMotionData.videoMotionTotal_Dist1,mMotionData.videoMotionTotal_Dist2,
		             mMotionData.videoMotionTotal_Dist3, mMotionData.videoMotionTotal_Dist4,mMotionData.voiceAlarmTotal);
	sprintf(strUrl,STRING_LOGIN_CONSERVER,server,port);
	int iRet=Post_head(strUrl, sendBuf, strResponse);
	if(iRet!=0)
	{
		LOGOUT("%s %d failure",strUrl,sendBuf);
		return -2;
	}
	printf("strResponse---%s---", strResponse);
	if(NULL == strResponse)
	{
		LOGOUT("pMsg null");
		return -3;
	}
	cJSON * pJson = cJSON_Parse(strResponse);
	if(NULL == pJson)																						 
	{
		LOGOUT("pJson null");
		return 0;
	}
	cJSON * pSub = cJSON_GetObjectItem(pJson, "result");
	if(NULL == pSub)
	{
		
		 LOGOUT("dataRecord return null");
	}
	else
	{
		result = pSub->valueint;
		LOGOUT("result : %d", pSub->valueint);
	}
	cJSON_Delete(pJson);
	return result;
}

int InitConServer()
{
	LOGINRETURNINFO returnInfo;
	Motion_Data mMotion;
	mMotion.videoMotionTotal_Dist1=3;
	mMotion.videoMotionTotal_Dist1=4;
	mMotion.videoMotionTotal_Dist1=5;
	mMotion.videoMotionTotal_Dist1=4;
	mMotion.videoMotionTotal_Dist1=2;
	memset(&returnInfo, 0, sizeof(LOGINRETURNINFO));
	loginCtrl(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP,
			  g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_iMasterPort,
			  g_szServerNO,g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword,
			  &returnInfo);
	dataRecord(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP,
			   g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_iMasterPort,
			   g_szServerNO, "video", 1441099285000, 20,"jpg",1024,mMotion);
	exitCtrl(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP,
			 g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_iMasterPort,
			 g_szServerNO,g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword);//退出
	return 0;
}

