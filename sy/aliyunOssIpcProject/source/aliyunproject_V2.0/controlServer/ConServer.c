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
#include "../hisdk/hi_sdk.h"


size_t write_data(void* buffer,size_t size,size_t nmemb,void *stream)  
{  
    memcpy(stream, buffer, size*nmemb);
    return size*nmemb;  
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
		returnInfo->resolutionRatio=-1;
		 LOGOUT("resolutionRatio null");
	}
	else
	{
		returnInfo->resolutionRatio = pSub->valueint;
		LOGOUT("resolutionRatio : %d", pSub->valueint);
	}
	// get resolutionRatio int from json
	pSub = cJSON_GetObjectItem(pJson, "codeStream");
	if(NULL == pSub)
	{
		 returnInfo->codeStream=-1;
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
		returnInfo->frameRate=-1;
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
		 returnInfo->streamType=-1;
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
		returnInfo->mainFrameGap=-1;
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
		returnInfo->isRequireToReboot=-1;
		LOGOUT("isRequireToReboot null");
	}
	else
	{
		returnInfo->isRequireToReboot = pSub->valueint;
		LOGOUT("isRequireToReboot : %d", pSub->valueint);
	}
	cJSON_Delete(pJson);
	return 0;
}

int loginCtrl(const char *server,const char *v_szId,const char *v_szPwd,LOGINRETURNINFO *returnInfo)
{
	if(server==NULL || v_szId==NULL || v_szPwd==NULL || returnInfo==NULL)
	{
		LOGOUT("param is error");
		return -1;
	}
	//char sendBuf[] = "id=LB1GB29HYM3M8HJ7111C&pwd=123456";
	//char strUrl[] = "http://cgtx.100memory.com/ipccmd.php?act=login";
	//3)HWVersion:IPC的硬件版本号
	//4)SWVersion:ipc 的固件版本号
	
	char sendBuf[1024]={0};
	char strUrl[1024]={0};
	char strResponse[1024] = {0};
	char allVersion[256]={0};
	char hardwareVersion[256]={0};
	char softwareVersion[256]={0};
	memset(sendBuf,0,sizeof(sendBuf));
	memset(strUrl,0,sizeof(strUrl));
	//设置软件版本信息
	int iRet= GetSoftVersion(DEVICECONFIGDIR, allVersion, sizeof(allVersion));
	if(0==iRet)
	{
		LOGOUT("GetSoftVersion success over iRet=%d %s",iRet,allVersion);
		sscanf(allVersion,"%[^_]_%s",hardwareVersion,softwareVersion);
		LOGOUT("hardwareVersion=%s and softwareVersion=%s",hardwareVersion,softwareVersion);
	}
	else
	{
		LOGOUT("SetSoftVersion failed over iRet=%d",iRet);
		return -1;
	}
	sprintf(sendBuf,"id=%s&pwd=%s&HWVersion=%s&SWVersion=%s",v_szId,v_szPwd,hardwareVersion,softwareVersion);
	sprintf(strUrl,STRING_LOGIN_CONSERVER,server);
	iRet=postHttpServer(strUrl, sendBuf, strResponse,write_data);
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
	memset(returnInfo,0,sizeof(LOGINRETURNINFO));
	iRet=getLoginInfo(strResponse, returnInfo);
	if(iRet==0)
	{
		if(returnInfo->result==1)
		{
			if(strlen(returnInfo->accessKeyId)!=0 && strlen(returnInfo->accesskeySecret)!=0
			   && strlen(returnInfo->OSSbucketName)!=0 && strlen(returnInfo->OSSEndpoint)!=0)
			{
				strncpy(g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szBuctetName,returnInfo->OSSbucketName,sizeof(g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szBuctetName));
				strncpy(g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szOssEndPoint,returnInfo->OSSEndpoint,sizeof(g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szOssEndPoint));
				strncpy(g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szAccessKeyId,returnInfo->accessKeyId,sizeof(g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szAccessKeyId));
				strncpy(g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szAccessKeySecret,returnInfo->accesskeySecret,sizeof(g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szAccessKeySecret));
				SetAliyunOssCfg(DEVICECONFIGDIR,g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg);
			}
			tagCapParamCfg objCapParamCfg;
			memset(&objCapParamCfg,0,sizeof(tagCapParamCfg));
			if(returnInfo->codeStream!=-1 && returnInfo->frameRate!=-1 && returnInfo->streamType!=-1
				&& returnInfo->mainFrameGap!=-1 && returnInfo->resolutionRatio!=-1)
			{
				
				if(returnInfo->resolutionRatio==2)
				{
					objCapParamCfg.m_wWidth=1280;
					objCapParamCfg.m_wHeight=960;	
				}
				else
				{
					objCapParamCfg.m_wWidth=1280;
					objCapParamCfg.m_wHeight=720;	
				}
				objCapParamCfg.m_wFrameRate=returnInfo->frameRate;
				objCapParamCfg.m_wKeyFrameRate=returnInfo->mainFrameGap;
				objCapParamCfg.m_wBitRate=returnInfo->codeStream;
				objCapParamCfg.m_CodeType=returnInfo->streamType;
				objCapParamCfg.m_wQpConstant=g_stConfigCfg.m_unCapParamCfg.m_objCapParamCfg[0].m_wQpConstant;
				HI_S_Video_Ext sVideo;
				int iRet=GetMasterVideoStream(&sVideo);
				sVideo.u32Bitrate=objCapParamCfg.m_wBitRate;
				sVideo.u32Frame=objCapParamCfg.m_wFrameRate;
				sVideo.u32Height=objCapParamCfg.m_wHeight;
				sVideo.u32Width=objCapParamCfg.m_wWidth;
				sVideo.u32ImgQuality=objCapParamCfg.m_wQpConstant;
				sVideo.u32Iframe=objCapParamCfg.m_wKeyFrameRate;
				sVideo.blCbr=objCapParamCfg.m_CodeType;
				SetCapParamCfg(DEVICECONFIGDIR,objCapParamCfg);
				SetMasterVideoStream(&sVideo);
				memcpy(&g_stConfigCfg.m_unCapParamCfg.m_objCapParamCfg,&objCapParamCfg,sizeof(objCapParamCfg));
			}

			if(returnInfo->isRequireToReboot!=-1)
			{
				if(returnInfo->isRequireToReboot==1)
				{
					LOGOUT("reboot ipc");
					system("reboot");
				}
			}
		}
	}
	return iRet;
}

int exitCtrl(const char *server,const char *v_szId,const char *v_szPwd)
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
	sprintf(strUrl,STRING_LOGOUT_CONSERVER,server);
	
	int iRet=postHttpServer(strUrl, sendBuf, strResponse,write_data);
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

	

int dataRecord(const char *server,const char *v_szId, char *videoPath, 
			   long long creatTimeInMilSecond, int videoFileSize,char *jpgFilePath, 
			   int videoTimeLength,Motion_Data mMotionData)
{
	if(server==NULL || v_szId==NULL)
	{
		LOGOUT("param is error");
		return -1;
	}	

	int result = 0;
	char *sendBuf=malloc(20480);
	if(!sendBuf)
	{
		LOGOUT("malloc is error");
	}
	char strUrl[1024]={0};
	char strResponse[1024] = {0};
	memset(sendBuf,0,sizeof(sendBuf));
	memset(strUrl,0,sizeof(strUrl));
	/*sprintf(sendBuf,STRING_RECORD_CONSERVER, v_szId, videoPath, creatTimeInMilSecond, videoFileSize,jpgFilePath, videoTimeLength,
					 mMotionData.videoMotionTotal_Dist1,mMotionData.videoMotionTotal_Dist2,
		             mMotionData.videoMotionTotal_Dist3, mMotionData.videoMotionTotal_Dist4,mMotionData.voiceAlarmTotal);*/
	sprintf(sendBuf,STRING_RECORD_CONSERVER, v_szId, videoPath, creatTimeInMilSecond, videoFileSize,jpgFilePath, videoTimeLength,
					 mMotionData.motionDetectInfo, mMotionData.soundVolumeInfo);
	sprintf(strUrl,STRING_DATARECORD_CONSERVER,server);
	int iRet=postHttpServer(strUrl, sendBuf, strResponse,write_data);
	LOGOUT("sendBuf=%s strUrl=%s",sendBuf,strUrl);
	if(iRet!=0)
	{
		LOGOUT("%s %s failure",strUrl,sendBuf);
		if(sendBuf)
		{
			free(sendBuf);
		}
		return -2;
	}
	LOGOUT("strResponse:%s strUrl:%s", strResponse,strUrl);
	if(NULL == strResponse)
	{
		LOGOUT("pMsg null");
		if(sendBuf)
		{
			free(sendBuf);
		}
		return -3;
	}
	cJSON * pJson = cJSON_Parse(strResponse);
	if(NULL == pJson)																						 
	{
		LOGOUT("pJson null");
		if(sendBuf)
		{
			free(sendBuf);
		}
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
		//LOGOUT("result : %d", pSub->valueint);
	}
	cJSON_Delete(pJson);
	if(sendBuf)
	{
		free(sendBuf);
	}
	return result;
}

int reportUrgencyRecord(const char *server,const char *v_szId,const char *v_szPwd, char *videoPath, 
			   long long creatTimeInMilSecond, int videoFileSize,char *jpgFilePath, 
			   int videoTimeLength,Motion_Data mMotionData)
{
	if(server==NULL || v_szId==NULL)
	{
		LOGOUT("param is error");
		return -1;
	}	

	int result = 0;
	char *sendBuf=malloc(20480);
	if(!sendBuf)
	{
		LOGOUT("malloc is error");
	}
	char strUrl[1024]={0};
	char strResponse[1024] = {0};
	memset(sendBuf,0,sizeof(sendBuf));
	memset(strUrl,0,sizeof(strUrl));
	/*sprintf(sendBuf,STRING_RECORD_CONSERVER, v_szId, videoPath, creatTimeInMilSecond, videoFileSize,jpgFilePath, videoTimeLength,
					 mMotionData.videoMotionTotal_Dist1,mMotionData.videoMotionTotal_Dist2,
		             mMotionData.videoMotionTotal_Dist3, mMotionData.videoMotionTotal_Dist4,mMotionData.voiceAlarmTotal);*/
	sprintf(sendBuf,STRING_URGENCYRECORD_CONSERVER, v_szId,v_szPwd,videoPath, creatTimeInMilSecond, videoFileSize,jpgFilePath, videoTimeLength,
					 mMotionData.motionDetectInfo, mMotionData.soundVolumeInfo);
	sprintf(strUrl,STRING_GENCYCORD_CONSERVER,server);
	int iRet=postHttpServer(strUrl, sendBuf, strResponse,write_data);
	printf("sendBuf=%s strUrl=%s",sendBuf,strUrl);
	if(iRet!=0)
	{
		LOGOUT("%s %s failure",strUrl,sendBuf);
		free(sendBuf);
		return -2;
	}
	free(sendBuf);
	LOGOUT("strResponse %s", strResponse);
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
		//LOGOUT("result : %d", pSub->valueint);
	}
	cJSON_Delete(pJson);
	return result;
}

static void setUrgencyCondition(cJSON *pJson,int cmdType,const char *strResponse, ServerCmdInfo *returnInfo)
{
	cJSON * pSub=NULL;
	int value=-1;
	
	pSub = cJSON_GetObjectItem(pJson, "timeCycle");
	if(NULL == pSub)
	{
		value=-1;
		LOGOUT("timeCycle null"); 
	}
	else
	{
		value=atoi(pSub->valuestring);
		LOGOUT("timeCycle: %d", value);
	}
	
	if(cmdType==SERVERID_START_URGENCYCONDITION)
		returnInfo->m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iStartPeriod = value;
	else
		returnInfo->m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iOverPeriod = value;
	pSub = cJSON_GetObjectItem(pJson, "motionTotal");
	if(NULL == pSub)
	{
		value=-1;
		LOGOUT("motionTotal null");
	}
	else
	{
		value=atoi(pSub->valuestring);
		LOGOUT("motionTotal : %d",value);
	}
	
	if(cmdType==SERVERID_START_URGENCYCONDITION)
		returnInfo->m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iStartSumDetect = value;
	else
		returnInfo->m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iOverSumDetect = value;
	pSub = cJSON_GetObjectItem(pJson, "motionDiscTotal");
	if(NULL == pSub)
	{
		 value=-1;
		 LOGOUT("motionDiscTotal null");
	}
	else
	{
		value=atoi(pSub->valuestring);
		LOGOUT("motionDiscTotal : %d", value);
	}
	if(cmdType==SERVERID_START_URGENCYCONDITION)
		returnInfo->m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iStartSumArea = value;
	else
		returnInfo->m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iOverSumArea = value;
	pSub = cJSON_GetObjectItem(pJson, "volumeAverage");
	if(NULL == pSub)
	{
		value=-1;
		LOGOUT("volumeAverage null");
	}
	else
	{
		value=atoi(pSub->valuestring);
		LOGOUT("volumeAverage : %d", value);
	}
	if(cmdType==SERVERID_START_URGENCYCONDITION)
		returnInfo->m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iStartSoundSize = value;
	else
		returnInfo->m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iOverSoundSize = value;
	if(cmdType!=SERVERID_START_URGENCYCONDITION)
	{
		pSub = cJSON_GetObjectItem(pJson, "maxVideoRecordTime");
		if(NULL == pSub)
		{
			value=-1;
			LOGOUT("maxVideoRecordTime null"); 
		}
		else
		{
			value=atoi(pSub->valuestring);
			LOGOUT("maxVideoRecordTime: %d", value);
		}
		returnInfo->m_unServerCmdInfo.m_objUrgencyMotionCfg.m_iEndRecTime = value;
		pSub = cJSON_GetObjectItem(pJson, "isEnable");
		if(NULL == pSub)
		{
			value=-1;
			LOGOUT("isEnable null"); 
		}
		else
		{
			value=atoi(pSub->valuestring);
			LOGOUT("isEnable: %d", value);
		}
		returnInfo->m_unServerCmdInfo.m_objUrgencyMotionCfg.m_bEnable = value;
	}
}

static void setMotionRecordCondition(cJSON *pJson,int cmdType,const char *strResponse, ServerCmdInfo *returnInfo)
{
	cJSON * pSub=NULL;
	int value=-1;
	
	pSub = cJSON_GetObjectItem(pJson, "phase1TimeCycle");
	if(NULL == pSub)
	{
		value=-1;
		LOGOUT("phase1TimeCycle null"); 
	}
	else
	{
		value=atoi(pSub->valuestring);
		LOGOUT("phase1TimeCycle: %d", value);
	} 
	returnInfo->m_unServerCmdInfo.m_objMotionCfg.m_iBefRecLastTime = value;

	pSub = cJSON_GetObjectItem(pJson, "phase1MotionTotal");
	if(NULL == pSub)
	{
		value=-1;
		LOGOUT("phase1MotionTotal null"); 
	}
	else
	{
		value=atoi(pSub->valuestring);
		LOGOUT("phase1MotionTotal: %d", value);
	} 
	returnInfo->m_unServerCmdInfo.m_objMotionCfg.m_iBefRecTimes = value;

	pSub = cJSON_GetObjectItem(pJson, "phase2TimeCycle");
	if(NULL == pSub)
	{
		value=-1;
		LOGOUT("phase2TimeCycle null"); 
	}
	else
	{
		value=atoi(pSub->valuestring);
		LOGOUT("phase2TimeCycle: %d", value);
	} 
	returnInfo->m_unServerCmdInfo.m_objMotionCfg.m_iConRecLastTime = value;

	pSub = cJSON_GetObjectItem(pJson, "phase2MotionTotal");
	if(NULL == pSub)
	{
		value=-1;
		LOGOUT("phase2MotionTotal null"); 
	}
	else
	{
		value=atoi(pSub->valuestring);
		LOGOUT("phase2MotionTotal: %d", value);
	} 
	returnInfo->m_unServerCmdInfo.m_objMotionCfg.m_iConRecTimes = value;

	pSub = cJSON_GetObjectItem(pJson, "maxVideoRecordTime");
	if(NULL == pSub)
	{
		value=-1;
		LOGOUT("maxVideoRecordTime null"); 
	}
	else
	{
		value=atoi(pSub->valuestring);
		LOGOUT("maxVideoRecordTime: %d", value);
	} 
	returnInfo->m_unServerCmdInfo.m_objMotionCfg.m_iEndRecTime = value;

	pSub = cJSON_GetObjectItem(pJson, "isEnable");
	if(NULL == pSub)
	{
		value=-1;
		LOGOUT("isEnable null"); 
	}
	else
	{
		value=atoi(pSub->valuestring);
		LOGOUT("isEnable: %d", value);
	} 
	returnInfo->m_unServerCmdInfo.m_objMotionCfg.m_bEnable = value;
	
}

int GetCheckAndLoadFromServerInfo(const char *strResponse, ServerCmdInfo *returnInfo)
{
	int cmdType = 0;
	int voiceFilePlayDelay = 0;
	char voiceFilePath[1024] = {0};

	if(NULL == strResponse)
	{
		LOGOUT("pMsg null");
		return -1;
	}
	cJSON * pJson = cJSON_Parse(strResponse);
	if(NULL == pJson)																						 
	{
		LOGOUT("pJson null");
		return -2;
	}
	cJSON * pSub = cJSON_GetObjectItem(pJson, "cmdType");
	if(NULL == pSub)
	{
	   LOGOUT("result null");
	   return -3;
	}
	else
	{
		cmdType = atoi(pSub->valuestring);
		LOGOUT("result: %d", atoi(pSub->valuestring));
	}
	returnInfo->cmdType = cmdType;
	int iRet=0;
	int value=0;
	switch (cmdType)
	{
		case SERVERID_START_URGENCYCONDITION:
		case SERVERID_END_URGENCYCONDITION:
		{
			setUrgencyCondition(pJson,cmdType,strResponse,returnInfo);
		}
			break;	
		case SERVERID_MOTION_RECORDCONDITION:
		{
			setMotionRecordCondition(pJson,cmdType,strResponse,returnInfo);	
		}
			break;
		case SERVERID_IGNORE:			
		case SERVERID_DOWNLOAD_AUDIOFILE:
		default:
			iRet=-1;
			break;

	}
	cJSON_Delete(pJson);
	return iRet;
}

int checkAndLoadCmdFromServer(const char *server,const char *v_szId,const char *v_szPwd,ServerCmdInfo *type)
{
	char sendBuf[256] = {0};
	char strUrl[1024]={0,};
	char strResponse[1024] = {0};
	sprintf(strUrl,"http://%s/ipccmd_1p5.php?act=checkAndLoadCmdFromServer",server);
	sprintf(sendBuf, "ipc_id=%s&pwd=%s",v_szId,v_szPwd);
	int iRet=postHttpServer(strUrl, sendBuf, strResponse,write_data);
	if(strResponse == NULL)
	{
		LOGOUT("Post_head,return error,iRet=%d",iRet);
		return -1;
	}
	//memset(strResponse,0,sizeof(strResponse));
	//strcpy(strResponse,"{\"cmdType\":\"2\",\"timeCycle\":\"100\",\"motionTotal\":\"100\",\"motionDiscTotal\":\"100\",\"volumeAverage\":\"100\"}");
	LOGOUT("strResponse:%s", strResponse);
	iRet=GetCheckAndLoadFromServerInfo(strResponse, type);
	return iRet;
}

int InitConServer()
{
	LOGINRETURNINFO returnInfo;
	Motion_Data mMotion;
	mMotion.videoMotionTotal_Dist1=3;
	mMotion.videoMotionTotal_Dist2=4;
	mMotion.videoMotionTotal_Dist3=5;
	mMotion.videoMotionTotal_Dist4=4;
	mMotion.voiceAlarmTotal=0;
	LOGOUT("long is %d---\n",sizeof(long));
	memset(&returnInfo, 0, sizeof(LOGINRETURNINFO));
	loginCtrl(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP,//g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_iMasterPort,
			  g_szServerNO,g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword,
			  &returnInfo);
	dataRecord(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP,//g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_iMasterPort,
			   g_szServerNO, "video", (long long)1441099285000, 20,"jpg",1024,mMotion);
	exitCtrl(g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP,//g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_iMasterPort,
			 g_szServerNO,g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword);//退出
	return 0;
}

