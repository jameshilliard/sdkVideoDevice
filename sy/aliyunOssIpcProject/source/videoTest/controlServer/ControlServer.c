#include <stdio.h>
#include <stdlib.h>
#include "../json/cJSON.h"
#include "../include/curl/curl.h"
#include <string.h>
#include <sys/types.h>     
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <netdb.h>
#include <iconv.h>
#include <error.h>
#include "ControlServer.h"
#include "../hisdk/hi_sdk.h"
#include "../Rtpsend/rtpManager.h"

static LoginInfo mLoginInfo;
static GetRtpSendInfo mGetRtpSendInfo;
// 转发和控制共用的全局结构体
extern RtpSendInfo g_stRtpSendInfo;
// 录像回放参数信息结构体
extern ReplayMsgInfo g_stReplayMsgInfo[MAXREPLAYNUM];

 
static size_t write_data(void* buffer,size_t size,size_t nmemb,void *stream)  
{  
    memcpy(stream, buffer, size*nmemb);
    return size*nmemb;  
} 

static int beatConServer(void)
{
	/*"cmdid": 1,
	"channel":"CAP-CTRLSVR",
		"cmdname": "2007",
	"clientver": "1.0.0.0",
	"cameraid":"1",
	"loginrelaystatus ":"1"
	"execmdid":"0"*/

	int iRet=0;
	cJSON* pRoot = cJSON_CreateObject();
	cJSON* pArray = cJSON_CreateArray();
	cJSON_AddNumberToObject(pRoot,"cmdid",		1);
	cJSON_AddStringToObject(pRoot,"channel",	"CAP-CTRLSVR");
	cJSON_AddStringToObject(pRoot,"cmdname",	"2007");
	cJSON_AddStringToObject(pRoot,"clientver",	"1.0.0.0");
	cJSON_AddStringToObject(pRoot,"cameraid",	mLoginInfo.m_cameraid);
	cJSON_AddStringToObject(pRoot,"loginrelaystatus","1");
	cJSON_AddStringToObject(pRoot,"execmdid",	"0");
	char *out;
	out=cJSON_Print(pRoot); 
	if(out==NULL)
	{
		LOGOUT("sendAttenceInfoToHttpServer cJSON_Print is error")
		iRet=-2;
		goto OUT3;
	}	

	char strUrl[1024]={0};
	char strResponse[1024] = {0};
	memset(strUrl,0,sizeof(strUrl));
	sprintf(strUrl,BEAT_CONSERVER_STRING,CONCTROLSERVER);
	iRet=postHttpServer(strUrl,out,strResponse,write_data);
	LOGOUT("sendBuf=%s strUrl=%s",out,strUrl);
	if(iRet!=0)
	{
		LOGOUT("%s %s failure",strUrl,out);
		return -2;
	}
	LOGOUT("strResponse:%s strUrl:%s", strResponse,strUrl);
	if(NULL == strResponse)
	{
		LOGOUT("pMsg null");
		return -3;
	}
OUT3:
	cJSON_Delete(pRoot);
	if(out) free(out);
	return iRet;

}

static int getRtpSendConServer(void)
{
	//"cmdid": 1,
	//"channel":"CAP-CTRLSVR",
	// "cmdname": "2004",
	//"clientver": "1.0.0.0",
	//"cameraid":"1"
	int iRet=0;
	cJSON* pRoot = cJSON_CreateObject();
	cJSON* pArray = cJSON_CreateArray();
	cJSON_AddNumberToObject(pRoot,"cmdid",		1);
	cJSON_AddStringToObject(pRoot,"channel",	"CAP-CTRLSVR");
	cJSON_AddStringToObject(pRoot,"cmdname",	"2004");
	cJSON_AddStringToObject(pRoot,"clientver",	"1.0.0.0");
	cJSON_AddStringToObject(pRoot,"cameraid",	mLoginInfo.m_cameraid);
	char *out;
	out=cJSON_Print(pRoot); 
	if(out==NULL)
	{
		LOGOUT("sendAttenceInfoToHttpServer cJSON_Print is error")
		iRet=-2;
		goto OUT3;
	}	

	char strUrl[1024]={0};
	char strResponse[1024] = {0};
	memset(strUrl,0,sizeof(strUrl));
	sprintf(strUrl,GETRTPSEND_CONSERVER_STRING,CONCTROLSERVER);
	iRet=postHttpServer(strUrl,out,strResponse,write_data);
	LOGOUT("sendBuf=%s strUrl=%s",out,strUrl);
	if(iRet!=0)
	{
		LOGOUT("%s %s failure",strUrl,out);
		return -2;
	}
	LOGOUT("strResponse:%s strUrl:%s", strResponse,strUrl);
	if(NULL == strResponse)
	{
		LOGOUT("pMsg null");
		return -3;
	}

	//"channel":"CAP-CTRLSVR",
	//"cmdname":"2004",
	//"msg":"成功获取转发",
	//"result":"0",
	//"videosvractive":"1",
	//"servername":"testzhu",
	//"videosvrhttpport":"9122",
	//"videosvrip":"60.173.9.137",
	//"videosvrport":"9121"
	memset(&mGetRtpSendInfo,0,sizeof(mGetRtpSendInfo));
	cJSON *json , *json_value; 
	json = cJSON_Parse(strResponse);
	if (!json)	
	{  
		LOGOUT("Error before: [%s]",cJSON_GetErrorPtr());
		return -2;
	}  
	json_value = cJSON_GetObjectItem(json,"videosvrip");
	if(json_value!=NULL)
	{	
		if( json_value->type == cJSON_String )	
		{  
			strcpy(mGetRtpSendInfo.m_rtpSendIp,json_value->valuestring);
			LOGOUT("mGetRtpSendInfo.m_rtpSendIp=%s",mGetRtpSendInfo.m_rtpSendIp);
			iRet=0;
		}
		else
		{
			iRet=-4;
			goto OUT3;
		}
	}
	json_value = cJSON_GetObjectItem(json,"videosvrport");
	if(json_value!=NULL)
	{	
		if( json_value->type == cJSON_String )	
		{  
			strcpy(mGetRtpSendInfo.m_rtpSendPort,json_value->valuestring);
			LOGOUT("mGetRtpSendInfo.m_rtpSendPort=%s",mGetRtpSendInfo.m_rtpSendPort);
			iRet=0;
			int j=0;
			for (j = 0; j < g_stRtpSendInfo.m_iTotalChn; j++)
			{
				g_stRtpSendInfo.m_stRtpSendCameraInfo[j].m_PicturePort = atoi(mGetRtpSendInfo.m_rtpSendIp);
				g_stRtpSendInfo.m_stRtpSendCameraInfo[j].m_iRemotePort = atoi(mGetRtpSendInfo.m_rtpSendPort);
				int iToRtpCmd = CTRL_MSG_SENDCAMINFO;
				SendBufferData(g_stRtpSendInfo.m_stRtpSendCameraInfo[j].m_iSocketID,iToRtpCmd,"cmd",4);
				LOGOUT("g_stRtpSendInfo.m_stRtpSendCameraInfo[j].m_iSocketID=%d,mGetRtpSendInfo.m_rtpSendIp=%s,mGetRtpSendInfo.m_rtpSendPort=%s",g_stRtpSendInfo.m_stRtpSendCameraInfo[j].m_iSocketID,mGetRtpSendInfo.m_rtpSendIp,mGetRtpSendInfo.m_rtpSendPort);
			}
		}
		else
		{
			iRet=-4;
			goto OUT3;
		}
	}
	json_value = cJSON_GetObjectItem(json,"videosvrhttpport");
	if(json_value!=NULL)
	{	
		if( json_value->type == cJSON_String )	
		{  
			strcpy(mGetRtpSendInfo.m_rtpSendHttpPort,json_value->valuestring);
			LOGOUT("mGetRtpSendInfo.m_rtpSendHttpPort=%s",mGetRtpSendInfo.m_rtpSendHttpPort);
			iRet=0;
	
		}
		else
		{
			iRet=-4;
			goto OUT3;
		}
	}

OUT3:
	cJSON_Delete(json);
	cJSON_Delete(pRoot);
	if(out) free(out);
	return iRet;	

}

static int loginConServer(void)
{
	/*"cmdid": 1,
	"channel":"CAP-CTRLSVR",
	"cmdname": "2010",
	"clientver": "1.0.0.0",
	"passwd": "123456",
	"devkey":"6",
	"devtype": "1",
	"resolution":"cif",
	"allresolution":"cif,D1,720P,1080P"*/
	int iRet=0;
	cJSON* pRoot = cJSON_CreateObject();
	cJSON_AddNumberToObject(pRoot,"cmdid",		1);
	cJSON_AddStringToObject(pRoot,"channel",	"CAP-CTRLSVR");
	cJSON_AddStringToObject(pRoot,"cmdname",	"2010");
	cJSON_AddStringToObject(pRoot,"clientver",	"1.0.0.0");
	char *md5String=MDString("123456");
	//char tesMd5String="E10ADC3949BA59ABBE56E057F20F883E";
	cJSON_AddStringToObject(pRoot,"passwd",		md5String);
	cJSON_AddStringToObject(pRoot,"devkey",		"656565");
	cJSON_AddStringToObject(pRoot,"devtype",	"1");
	cJSON_AddStringToObject(pRoot,"resolution",	"cif");
	cJSON_AddStringToObject(pRoot,"allresolution",	"cif,D1,720P");
	char *out;
	out=cJSON_Print(pRoot); 
	if(out==NULL)
	{
		LOGOUT("sendAttenceInfoToHttpServer cJSON_Print is error")
		iRet=-2;
		goto OUT3;
	}	

	char strUrl[1024]={0};
	char strResponse[1024] = {0};
	memset(strUrl,0,sizeof(strUrl));
	sprintf(strUrl,LOGIN_CONSERVER_STRING,CONCTROLSERVER);
	iRet=postHttpServer(strUrl,out,strResponse,write_data);
	LOGOUT("sendBuf=%s strUrl=%s",out,strUrl);
	if(iRet!=0)
	{
		LOGOUT("%s %s failure",strUrl,out);
		return -2;
	}
	LOGOUT("strResponse:%s strUrl:%s", strResponse,strUrl);
	if(NULL == strResponse)
	{
		LOGOUT("pMsg null");
		return -3;
	}
	//strResponse:{"cameraid":"73","cameraname":"ipc","channel":"CAP-CTRLSVR",
	//"cmdid":"1","cmdname":"2010","devkey":"656565","maxonline":
	//"0","msg":"ipc login success","nodeid":"66","nodename":"ipc",
	//"result":"0","updateip":"http://112.74.86.237"}
	memset(&mLoginInfo,0,sizeof(mLoginInfo));
	cJSON *json , *json_value; 
	json = cJSON_Parse(strResponse);
	if (!json)	
	{  
		LOGOUT("Error before: [%s]",cJSON_GetErrorPtr());
		return -2;
	}  
	json_value = cJSON_GetObjectItem(json,"cameraid");
	if(json_value!=NULL)
	{	
		if( json_value->type == cJSON_String )	
		{  
			strcpy(mLoginInfo.m_cameraid,json_value->valuestring); 
			LOGOUT("mLoginInfo.m_cameraid=%s",mLoginInfo.m_cameraid);
			iRet=0;
		}
		else
		{
			iRet=-4;
			goto OUT3;
		}
	}
OUT3:
	cJSON_Delete(json);
	cJSON_Delete(pRoot);
	if(out) free(out);
	return iRet;

}

void *connTask(void* param)
{
	int status=0;
	while(1)
	{
		switch(status)
		{
		case 0:{
					sleep(1);
					status=1;
				}
				break;
		case 1:
				{
					loginConServer();
					status=2;
				}
				break;
		case 2:
				{
 					getRtpSendConServer();
					status=3;
				}
				break;
		case 3:
				{
					beatConServer();
					sleep(30);
				}
				break;
		default:
				break;
		}
		
		
		sleep(1);
	}
}

int InitControlServer()
{
	int iRet=0;
	pthread_t m_connPthread;//实时播放，过程控制线程
	iRet = pthread_create(&m_connPthread,NULL, connTask, NULL);
	if(iRet != 0)
	{
		LOGOUT("can't create makeMp4Task thread: %s",strerror(iRet));
		return -2;
	}
	LOGOUT("InitControlServe start");
}

