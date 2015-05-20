#include "RouteServer.h"
#include "ControlServer.h"
#include "../Common/ClientSocket.h"
#include "../Common/NetFunc.h"
#include "../Common/GlobFunc.h"
#include "../LogOut/LogOut.h"
#include "../Common/md5.h"
#include "../rtmp/RtmpDLL.h"
//#include "../hisdk/source_sdk.h"
#include "../hisdk/hi_sdk.h"



PRTMPUnit pRtmpServer;
static BOOL startControlServer=TRUE;
PRTMPUnit pRtmpServer=NULL;
char g_serverNo[64]={0,};
static unsigned int beatPrintfTimes=0;

char	g_szServer[MATSERIPCOUNT][32];
int 	g_iMasterPort = ROUTESERVERPORT;
char 	g_server[200] = {0};


static int isValidPacket(LPTSTR recBuffer,DWORD recLength,MsgHead *vMsgHead,LPTSTR msgBuffer,DWORD msgLength)
{
	if(recBuffer==NULL || vMsgHead==NULL || msgBuffer==NULL)
		return -1;
	memcpy(vMsgHead,recBuffer,sizeof(MsgHead));
	if(vMsgHead->uHeadLen!=sizeof(MsgHead))
		return -2;
	if(recLength!=(vMsgHead->uBodyLen+sizeof(MsgHead)))
		return -3;
	if(vMsgHead->uHeadLen>msgLength)
		return -4;
	memcpy(msgBuffer,recBuffer+sizeof(MsgHead),vMsgHead->uBodyLen);
	return 0;
}
#define 	SERVERFILE					"/mnt/mtd/ipc/conf/config_platform.ini"

int getServerNo(char * path,char serverNo[64])
{
	FILE * fp;  
	char * line = NULL;  
	size_t len = 0;  
	ssize_t read;  
	int count=0;
	char *findPtr=NULL;
	int iRet=-1;
	fp = fopen(path, "r");  
	if(fp == NULL)  
		return -1;
	while((read = getline(&line, &len, fp)) != -1) 
	{    
		 if(strstr(line,"uid")==line)
		 {
			findPtr=strchr(line,'"');
			if(findPtr!=NULL)
			{
				iRet=sscanf(findPtr,"\"%s\"",serverNo);
				if(iRet==1)
				{
					printf("serverNo is %s\n",serverNo);
					iRet=0;
					break;
				}
				
			}
		 }
	}  
	if (line)  
	 	free(line);  
	if(fp != NULL)
		fclose(fp);
	return iRet;
}

static long long getTimeStamp(void)
{
	time_t time_of_day;
	time_of_day=time(NULL);
	struct timeval when;
	gettimeofday(&when,NULL);
	long long ltime=time_of_day*1000+when.tv_usec/1000;
	return ltime;
}

static int get_DEVICE_REGISTER_ACK(MsgHead *vMsgHead,LPCTSTR msgBuffer,MsgBody_DEVICE_REGISTER_ACK *pParam)
{
if(msgBuffer==NULL)
		return -MSG_ACK_ERROR_STATUS_PACKET;
	char leftString[SOCKET_PARKET_SIZE]={0,};
	char left2String[SOCKET_PARKET_SIZE]={0,};
	int iRet=sscanf(msgBuffer,"[%d:{%d:%s",&pParam->m_registerStatus,&pParam->m_channelId,leftString);
	if(iRet<3)
	{
		iRet=MSG_ACK_ERROR_STATUS_PACKET;
	}
	else
	{
		//LOGOUT("pParam->m_registerStatus is %d",pParam->m_registerStatus);
		//LOGOUT("pParam->m_channelId is %d",pParam->m_channelId);
		
		LPCTSTR where=strchr(leftString,PARAMEQUALSYMBOL);
		if(where==NULL)
		{
			iRet=sscanf(leftString,"%d}]",&pParam->m_channelNum);
			if(iRet!=1)
			{
				iRet=MSG_ACK_ERROR_STATUS_PACKET;
			}
			else
			{	
				iRet=0;
			}
		}
		else
		{
			iRet=sscanf(leftString,"%d}%s",&pParam->m_channelNum,left2String);
			if(iRet!=2)
			{
				iRet=MSG_ACK_ERROR_STATUS_PACKET;
			}
			else
			{	
				iRet=0;
			}
		}

	}
	if(iRet==0)
	{
		pParam->m_isValid=1;
	}
	else
	{
		pParam->m_isValid=0;
	}
	return iRet;
}
static int get_DEVICE_IDENTITY_VERIFY_ACK(MsgHead *vMsgHead,LPCTSTR msgBuffer,int *vLogStatus)
{
	if(msgBuffer==NULL)
		return MSG_ACK_ERROR_STATUS_PACKET;
	int iRet=MSG_ACK_ERROR_STATUS_PACKET;
	char left2String[SOCKET_PARKET_SIZE]={0,};
	LPCTSTR where=strchr((char *)vMsgHead,PARAMEQUALSYMBOL);
	if(!where)
	{
		iRet=sscanf(msgBuffer,"[%d]",vLogStatus);
		if(iRet!=1)
		{
			iRet=MSG_ACK_ERROR_STATUS_PACKET;
		}	
	}
	else
	{
		char temp[SOCKET_PARKET_SIZE]={0,};
		iRet=sscanf(msgBuffer,"[%d:%s",vLogStatus,left2String);
		if(iRet!=2)
		{
			iRet=MSG_ACK_ERROR_STATUS_PACKET;
		}	
	}
	return iRet;
}

static int get_OPEN_CHANNEL_PREVIEW(MsgHead *vMsgHead,LPCTSTR msgBuffer,MsgBody_OPEN_CHANNEL_PREVIEW_ACK *pParam)
{
	if(msgBuffer==NULL)
		return -MSG_ACK_ERROR_STATUS_PACKET;
	int iRet=0; 
	char leftString[256]={0,}; 
	char left2String[SOCKET_PARKET_SIZE]={0,};
	char rtmpPortString[32]={0,};
	char channelIdString[32]={0,};
	char blFlagString[32]={0,};
	char blCbrString[32]={0,};
	char u32WidthString[32]={0,};
	char u32HeightString[32]={0,};
	char u32BitrateString[32]={0,};
	char u32FrameString[32]={0,};
	char u32ImgQualityString[32]={0,};
	char u32IFrameString[32]={0,};
	
	//[112.124.113.127:1935:live:100:benxun123456:0:1080:720:32:6:12:60]
	
	//[121.43.234.112:1935:live:1739:12345678:1:320:176:0:100:1:8:128:1429778918924] 
	iRet=sscanf(msgBuffer,"[%[^':']:%[^':']:%[^':']:%[^':']:%[^':']:%[^':']:%[^':']:%[^':']:%[^':']:%[^':']:%[^':']:%[^':']:%[^':']:%[^']']",
				pParam->m_rtmpIp,rtmpPortString,pParam->m_rtmpPath,channelIdString,pParam->m_rtmpUUID,blFlagString
				,u32WidthString,u32HeightString,blCbrString,u32BitrateString,u32ImgQualityString,u32FrameString,u32IFrameString,leftString);
	if(iRet<12)
	{
		iRet=MSG_ACK_ERROR_STATUS_PACKET;
	}
	else
	{

		pParam->m_channelId=atoi(channelIdString);
		pParam->m_rtmpPort=atoi(rtmpPortString);
		pParam->m_video.u32Channel=atoi(channelIdString);
		pParam->m_video.u32Stream=atoi(blFlagString);
		pParam->m_video.u32Bitrate=atoi(u32BitrateString);
		pParam->m_video.u32Frame=atoi(u32FrameString);
		pParam->m_video.blCbr=(atoi(blCbrString)==0) ? HI_TRUE : HI_FALSE;
		pParam->m_video.u32ImgQuality=atoi(u32ImgQualityString);
		pParam->m_video.u32Width=atoi(u32WidthString);
		pParam->m_video.u32Height=atoi(u32HeightString);
		pParam->m_video.u32Iframe=atoi(u32IFrameString);
		
		LPCTSTR where=strchr(leftString,PARAMEQUALSYMBOL);
		if(where==NULL)
		{
			iRet=sscanf(leftString,"%['0-9']",pParam->m_serverStamp);
			if(iRet!=1)
			{
				iRet=MSG_ACK_ERROR_STATUS_PACKET;
			}
			else
				iRet=0;
		}
		else
		{
			
			iRet=sscanf(leftString,"%['0-9']:%s",pParam->m_serverStamp,left2String);
			if(iRet!=2)
			{
				iRet=MSG_ACK_ERROR_STATUS_PACKET;
			}
			else
				iRet=0;
		
		}

	}
	if(iRet==0)
	{
		pParam->m_isValid=1;
	}
	else
	{
		pParam->m_isValid=0;
	}
	return iRet;
}

static int get_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG(MsgHead *vMsgHead,LPCTSTR msgBuffer,MsgBody_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG *pParam)
{
	if(msgBuffer==NULL)
		return -MSG_ACK_ERROR_STATUS_PACKET;
	int iRet=0; 
	char leftString[256]={0,}; 
	char left2String[SOCKET_PARKET_SIZE]={0,};
	char channelIdString[32]={0,};
	char blFlagString[32]={0,};
	char blCbrString[32]={0,};
	char u32WidthString[32]={0,};
	char u32HeightString[32]={0,};
	char u32BitrateString[32]={0,};
	char u32FrameString[32]={0,};
	char u32ImgQualityString[32]={0,};
	
	//[通道ID:码流类型:分辨率长:分辨率宽:编码方式:码率:质量:帧率:帧间隔]
	iRet=sscanf(msgBuffer,"[%[^':']:%[^':']:%[^':']:%[^':']:%[^':']:%[^':']:%[^':']:%[^':']:%[^']']",
				channelIdString,blFlagString,u32WidthString,u32HeightString,blCbrString,
				u32BitrateString,u32ImgQualityString,u32FrameString,leftString);
	if(iRet<5)
	{
		iRet=MSG_ACK_ERROR_STATUS_PACKET;
	}
	else
	{

		pParam->m_channelId=atoi(channelIdString);
		pParam->m_video.u32Channel=atoi(channelIdString);
		pParam->m_video.u32Stream=atoi(blFlagString);
		pParam->m_video.u32Bitrate=atoi(u32BitrateString);
		pParam->m_video.u32Frame=atoi(u32FrameString);
		pParam->m_video.blCbr=(atoi(blCbrString)==0) ? HI_TRUE : HI_FALSE;
		pParam->m_video.u32ImgQuality=atoi(u32ImgQualityString);
		pParam->m_video.u32Width=atoi(u32WidthString);
		pParam->m_video.u32Height=atoi(u32HeightString);
		
		LPCTSTR where=strchr(leftString,PARAMEQUALSYMBOL);
		if(where==NULL)
		{
			iRet=sscanf(leftString,"%d",&pParam->m_video.u32Iframe);
			if(iRet!=1)
			{
				iRet=MSG_ACK_ERROR_STATUS_PACKET;
			}
			else
				iRet=0;
		}
		else
		{
			
			iRet=sscanf(leftString,"%d:%s",&pParam->m_video.u32Iframe,left2String);
			if(iRet!=2)
			{
				iRet=MSG_ACK_ERROR_STATUS_PACKET;
			}
			else
				iRet=0;
		
		}

	}
	if(iRet==0)
	{
		pParam->m_isValid=1;
	}
	else
	{
		pParam->m_isValid=0;
	}
	return iRet;
}
static int getDeviceIdenTityVerifyPacket(MsgBody_DEVICE_IDENTITY_VERIFY *pMsgBody_DEVICE_IDENTITY_VERIFY,LPTSTR packetBuffer,unsigned short *packetLength)
{
	if(pMsgBody_DEVICE_IDENTITY_VERIFY->m_user==NULL || pMsgBody_DEVICE_IDENTITY_VERIFY->m_secret==NULL || pMsgBody_DEVICE_IDENTITY_VERIFY->m_serverNo==NULL)
		return -2;
	if(strlen(pMsgBody_DEVICE_IDENTITY_VERIFY->m_user)>PARAM_MAXUSER || strlen(pMsgBody_DEVICE_IDENTITY_VERIFY->m_secret)>PARAM_MAXSECRET || strlen(pMsgBody_DEVICE_IDENTITY_VERIFY->m_serverNo)>PARAM_MAXSERVERNO)
		return -3;
	char *base64User=SY_base64Encode(pMsgBody_DEVICE_IDENTITY_VERIFY->m_user);
	if(base64User==NULL)
	{		
		LOGOUT("base64Encode error");
		return -CONTROL_PROTOCAL_DEVICE_IDENTITY_VERIFY;
	}
	MD5_CTX context;
	unsigned char digest[16];
	unsigned char digestStr[48];
	MD5Init(&context);
	MD5Update(&context,(unsigned char *)pMsgBody_DEVICE_IDENTITY_VERIFY->m_secret,strlen(pMsgBody_DEVICE_IDENTITY_VERIFY->m_secret));
	MD5Final(digest, &context);
	memset(digestStr,0,sizeof(digestStr));
	int i=0;
	for(i=0; i<16; i++)
 	{		
		sprintf((char *)digestStr+2*i,"%02x", digest[i]);
 	}	
	memset(packetBuffer,0,sizeof(digestStr));
	sprintf(packetBuffer,"[%s:%s:%s]",base64User,digestStr,pMsgBody_DEVICE_IDENTITY_VERIFY->m_serverNo);	
	//sprintf(socketBuffer+sizeof(MsgHead),"[%s:%s:%s]",base64User,"e10adc3949ba59abbe56e057f20f883e",serverNo);
	*packetLength=strlen(packetBuffer);
	return 0;
}

static int sendConServerMessage(int socketId,unsigned short uMsgType,void *pData)
{
	if(socketId<0)
		return -1;
	MsgHead msgHead;
	int iRet=-1;
	static unsigned short uMsgSeq;//消息序列号
	unsigned short packetLength=0;
	char socketBuffer[SOCKET_PARKET_SIZE]={0,};
	msgHead.uHeadLen=sizeof(MsgHead);
	msgHead.cMsgVersion=MSG_HEAD_VERSION;
	msgHead.cPlatform=MSG_HEAD_PLATFORM;
	msgHead.uMsgType=uMsgType;
	msgHead.uMsgSeq=uMsgSeq;
	uMsgSeq++;
	msgHead.uBodyLen=0;
	memset(socketBuffer,0,sizeof(socketBuffer));
	switch(uMsgType)
	{
		case CONTROL_PROTOCAL_DEVICE_IDENTITY_VERIFY:
		{
			if(pData==NULL) break;
			MsgBody_DEVICE_IDENTITY_VERIFY *pMsgBody_DEVICE_IDENTITY_VERIFY=(MsgBody_DEVICE_IDENTITY_VERIFY *)pData;
			if(pMsgBody_DEVICE_IDENTITY_VERIFY->m_uMsgType!=CONTROL_PROTOCAL_DEVICE_IDENTITY_VERIFY)
				return -CONTROL_PROTOCAL_DEVICE_IDENTITY_VERIFY;
			iRet=getDeviceIdenTityVerifyPacket(pMsgBody_DEVICE_IDENTITY_VERIFY,socketBuffer+sizeof(MsgHead),&packetLength);
		}
			break;
		case	CONTROL_PROTOCAL_KEEPALIVE:
		{
			packetLength=0;
			iRet=0;
		}
			break;
		case 	CONTROL_PROTOCAL_SETTIMEOUTDURATION:
		{
			if(pData==NULL) break;
			MsgBody_SETTIMEOUTDURATION *pMsgBody_SETTIMEOUTDURATION=(MsgBody_SETTIMEOUTDURATION *)pData;
			if(pMsgBody_SETTIMEOUTDURATION->m_uMsgType!=CONTROL_PROTOCAL_SETTIMEOUTDURATION)
					return -CONTROL_PROTOCAL_SETTIMEOUTDURATION;
			sprintf(socketBuffer+sizeof(MsgHead),"[%d]",pMsgBody_SETTIMEOUTDURATION->m_durationSecond);
			packetLength=strlen(socketBuffer+sizeof(MsgHead));
			iRet=0;
		}
			break;
		case 	CONTROL_PROTOCAL_DEVICE_REGISTER:
		{
			if(pData==NULL) break;
			MsgBody_DEVICE_REGISTER *pMsgBody_DEVICE_REGISTER=(MsgBody_DEVICE_REGISTER *)pData;
			if(pMsgBody_DEVICE_REGISTER->m_uMsgType!=CONTROL_PROTOCAL_DEVICE_REGISTER)
					return -CONTROL_PROTOCAL_DEVICE_REGISTER;
			sprintf(socketBuffer+sizeof(MsgHead),"[%s:%s:%s:%s:%s:%d:%d]",pMsgBody_DEVICE_REGISTER->m_deviceProduct,
					pMsgBody_DEVICE_REGISTER->m_deviceModel,pMsgBody_DEVICE_REGISTER->m_deviceServerNo,pMsgBody_DEVICE_REGISTER->m_deviceType,
					pMsgBody_DEVICE_REGISTER->m_deviceMacAddr,pMsgBody_DEVICE_REGISTER->m_deviceChannelNum,pMsgBody_DEVICE_REGISTER->m_deviceChannelStartNum);
			packetLength=strlen(socketBuffer+sizeof(MsgHead));
			iRet=0;
		}
			break;
		case 	CONTROL_PROTOCAL_UPDATE_CHANNEL_CONFIG:
		{	
			if(pData==NULL) break;
			MsgBody_UPDATE_CHANNEL_CONFIG *pMsgBody_UPDATE_CHANNEL_CONFIG=(MsgBody_UPDATE_CHANNEL_CONFIG *)pData;
			if(pMsgBody_UPDATE_CHANNEL_CONFIG->m_uMsgType!=CONTROL_PROTOCAL_UPDATE_CHANNEL_CONFIG)
					return -CONTROL_PROTOCAL_UPDATE_CHANNEL_CONFIG;
			sprintf(socketBuffer+sizeof(MsgHead),"[%d:%d:%d:%d]",pMsgBody_UPDATE_CHANNEL_CONFIG->m_channelId,
					pMsgBody_UPDATE_CHANNEL_CONFIG->m_channelNumber,pMsgBody_UPDATE_CHANNEL_CONFIG->m_imageLevel,
					pMsgBody_UPDATE_CHANNEL_CONFIG->m_armingSwitch);
			packetLength=strlen(socketBuffer+sizeof(MsgHead));
			iRet=0;
		}
			break; 
		case 	CONTROL_PROTOCAL_PUBLISH_CHANNEL_STATE:
		{
			if(pData==NULL) break;
			MsgBody_PUBLISH_CHANNEL_STATE *pMsgBody_PUBLISH_CHANNEL_STATE=(MsgBody_PUBLISH_CHANNEL_STATE *)pData;
			if(pMsgBody_PUBLISH_CHANNEL_STATE->m_uMsgType!=CONTROL_PROTOCAL_PUBLISH_CHANNEL_STATE)
					return -CONTROL_PROTOCAL_PUBLISH_CHANNEL_STATE;
			sprintf(socketBuffer+sizeof(MsgHead),"[{%d:%d}]",pMsgBody_PUBLISH_CHANNEL_STATE->m_channelId,pMsgBody_PUBLISH_CHANNEL_STATE->m_liveStatus);
			packetLength=strlen(socketBuffer+sizeof(MsgHead));
			iRet=0;
		}
			break;
		case 	CONTROL_PROTOCAL_GET_ENCODE_CONFIG_TABLE:
		{
			if(pData==NULL) break;
			MsgBody_GET_ENCODE_CONFIG_TABLE *pMsgBody_GET_ENCODE_CONFIG_TABLE=(MsgBody_GET_ENCODE_CONFIG_TABLE *)pData;
			if(pMsgBody_GET_ENCODE_CONFIG_TABLE->m_uMsgType!=CONTROL_PROTOCAL_GET_ENCODE_CONFIG_TABLE)
					return -CONTROL_PROTOCAL_GET_ENCODE_CONFIG_TABLE;
			sprintf(socketBuffer+sizeof(MsgHead),"[%ld]",pMsgBody_GET_ENCODE_CONFIG_TABLE->m_timeStamp);
			packetLength=strlen(socketBuffer+sizeof(MsgHead));
			iRet=0;
		}
			break;
		case 	CONTROL_PROTOCAL_PREVIEW_FAILURE_NOTIFY:
			if(pData==NULL) break;
			MsgBody_PREVIEW_FAILURE_NOTIFY *pMsgBody_PREVIEW_FAILURE_NOTIFY=(MsgBody_PREVIEW_FAILURE_NOTIFY *)pData;
			if(pMsgBody_PREVIEW_FAILURE_NOTIFY->m_uMsgType!=CONTROL_PROTOCAL_PREVIEW_FAILURE_NOTIFY)
				return -CONTROL_PROTOCAL_PREVIEW_FAILURE_NOTIFY;
			sprintf(socketBuffer+sizeof(MsgHead),"[{%d:%d:%s}]",pMsgBody_PREVIEW_FAILURE_NOTIFY->m_channelId,
					pMsgBody_PREVIEW_FAILURE_NOTIFY->m_failureType,pMsgBody_PREVIEW_FAILURE_NOTIFY->m_serverStamp);
			packetLength=strlen(socketBuffer+sizeof(MsgHead));
			iRet=0;
			break;
		case 	CONTROL_PROTOCAL_OPEN_CHANNEL_TALK:
			break;
		case 	CONTROL_PROTOCAL_CLOSE_CHANNEL_TALK:
			break; 
		case 	CONTROL_PROTOCAL_TALK_FAILURE_NOTIFY:
			break;
		case 	CONTROL_PROTOCAL_GET_CHANNEL_CONFIG:
			break; 
		case 	CONTROL_PROTOCAL_SET_CHANNEL_CONFIG:
			break; 
		case 	CONTROL_PROTOCAL_CHANGE_CHANNEL_IMAGE_QUALITY:
			break;
		case 	CONTROL_PROTOCAL_COLLECT_YUNTAI_CONTROL:
			break;
		case 	CONTROL_PROTOCAL_COLLECT_PRESET_CRUISE_CONTROL:
			break;	
		default:
		{
			iRet=-20;
		}
		break;
	}
	if(iRet==0)
	{
		msgHead.uBodyLen=packetLength;
		memcpy(socketBuffer,&msgHead,sizeof(MsgHead));
		if(msgHead.uMsgType!=CONTROL_PROTOCAL_KEEPALIVE)
		{
			LOGOUT("send:uHeadLen=0x%x,cMsgVersion=0x%x,cPlatform=0x%x,uMsgType=0x%x,uMsgSeq=0x%x,uBodyLen=0x%x,msgBody=%s",
					msgHead.uHeadLen,msgHead.cMsgVersion,msgHead.cPlatform,msgHead.uMsgType
					,msgHead.uMsgSeq,msgHead.uBodyLen,socketBuffer+sizeof(MsgHead));

		}
		else
		{
			beatPrintfTimes++;
			if(beatPrintfTimes==20)
			{
				LOGOUT("send:uHeadLen=0x%x,cMsgVersion=0x%x,cPlatform=0x%x,uMsgType=0x%x,uMsgSeq=0x%x,uBodyLen=0x%x,msgBody=%s",
						msgHead.uHeadLen,msgHead.cMsgVersion,msgHead.cPlatform,msgHead.uMsgType
						,msgHead.uMsgSeq,msgHead.uBodyLen,socketBuffer+sizeof(MsgHead));

			}
		}
	}
	else
		return iRet;
	iRet=SendSocketData(socketId,socketBuffer,msgHead.uBodyLen+sizeof(MsgHead));	
	return iRet;
}


void printfVideoParam(char *ptrString,HI_S_Video_Ext videoEx)
{

	LOGOUT("%s:channel:%d,blFlag:%d,u32Bitrate:%d,u32Frame:%d,u32Iframe=%d,blcbr=%d,u32ImgQuality=%d,u32Width=%d,u32Height=%d"
		   ,ptrString,videoEx.u32Channel,videoEx.u32Stream,videoEx.u32Bitrate,videoEx.u32Frame,
		   videoEx.u32Iframe,videoEx.blCbr,videoEx.u32ImgQuality,videoEx.u32Width,videoEx.u32Height);
}

static int getMasterIpAndPort()
{

	tagMasterServerCfg objGetMasterInfo;	
	int iRet = GetCfgFile(&objGetMasterInfo, sizeof(tagMasterServerCfg), offsetof(tagConfigCfg, m_unMasterServerCfg.m_objMasterServerCfg), sizeof(tagMasterServerCfg));
	if(iRet)
	{
		LOGOUT("Get Cfg File Fail!!");
		return -1;
	}
	memset(g_szServer,0,sizeof(g_szServer));
	memset(g_server,0,sizeof(g_server));
	char szSplitBuf[200] = {0};
	strcpy(szSplitBuf, objGetMasterInfo.m_szMasterIP);
	strcpy(g_server,objGetMasterInfo.m_szMasterIP);
	int i = 0;
	int iCount = 0;
	int isMasterValid=0;
	char *szMasterIps[MATSERIPCOUNT];

	iCount = split(szMasterIps, szSplitBuf, ";", MATSERIPCOUNT);
	for (i = 0; i< iCount; i++)
	{
		strcpy(g_szServer[i], szMasterIps[i]);
		if(strlen(g_szServer[i])!=0)
			isMasterValid=1;
			
	}
	LOGOUT("Local Master:%s,%s,%s,%s Port:%d",g_szServer[0],g_szServer[1],g_szServer[2],g_szServer[3],g_iMasterPort);
	if(isMasterValid==1)
	{
		strcpy(g_szServer[0],ROUTESERVER);
		g_iMasterPort=ROUTESERVERPORT;
	}	
	g_iMasterPort = objGetMasterInfo.m_iMasterPort;
	if(g_iMasterPort==0)
		g_iMasterPort=ROUTESERVERPORT;
}
int setMasterAndPort(char *server,int port)
{
	if(server==NULL)
		return -1;
	if(strcmp(g_server,server)==0 && port==g_iMasterPort)
		return  0;
	tagMasterServerCfg objSetMasterInfo = {0};
	strcpy(objSetMasterInfo.m_szMasterIP,server);
	objSetMasterInfo.m_iMasterPort = port;
	LOGOUT("test");
	int iRet=SetCfgFile(&objSetMasterInfo, offsetof(tagConfigCfg, m_unMasterServerCfg.m_objMasterServerCfg), sizeof(tagMasterServerCfg));
	getMasterIpAndPort();
	return iRet;
}

static void *P_CtrlSocketThread()
{
	INT8U clientStaus=StatusIdle;
	INT32S	iRet=0;
	char controlServer[128]={0,};
	BOOL reConFlag=TRUE;
	INT32U 	sleepTimeSecnod=0;
	int	socketId=0;
	DWORD v_dwSymb;
	char  recBuffer[SOCKET_PARKET_SIZE];
	char  recMsgBuffer[SOCKET_PARKET_SIZE];
	char  sendOtherMsgBuffer[SOCKET_PARKET_SIZE];
	char  curServerStampBuffer[SOCKET_PARKET_SIZE];
	unsigned short uOtherMsgType=0;//消息类型
	MsgHead recMsgHead;
	DWORD length;
	BOOL  beatTimeTwoSend=FALSE;
	BOOL  bSendBeatFlag=FALSE;
	//BOOL  timeStampFristFlag=FALSE;
	int  	loginStatus=MSG_ACK_ERROR_STATUS_PACKET;
	DWORD 	lastSendBeatTimeMs=0;	
	DWORD 	lastRecvBeatTimeMs=0;
	DWORD	nowTimeMs=0;
	Device_Net_Info deviceNetInfo;
	MsgGlobal_Device globalDeviceStatus;
	BOOL 	bSendErrorPlay=FALSE;
	int i=0;
	
	unsigned short curMsgType=0;//消息类型
	DWORD 	curMsgSendTime=0;//消息类型
	
	memset(&deviceNetInfo,0,sizeof(deviceNetInfo));
	iRet=GetDeviceNetInfo(deviceNetInfo.m_IPAddress, deviceNetInfo.m_Subnet,deviceNetInfo.m_MACAddress,deviceNetInfo.m_GateWay,deviceNetInfo.m_BcastAddr);
	if(iRet!=0)
	{
		memset(&deviceNetInfo,0,sizeof(deviceNetInfo));
		memcpy(&deviceNetInfo.m_MACAddress,DEFAULTDEVICEMACADDR,MIN(sizeof(deviceNetInfo.m_MACAddress),strlen(DEFAULTDEVICEMACADDR)));
	}
	while(startControlServer)
	{
		switch(clientStaus)
		{
		case StatusIdle:
		{
			clientStaus=StatusConnRoute;
			bSendBeatFlag=FALSE;
			curMsgType=0;
			beatPrintfTimes=0;
			if(sleepTimeSecnod!=0)
			{
				LOGOUT("cmd sleep %d sencod",sleepTimeSecnod);
				sleep(sleepTimeSecnod);
				sleepTimeSecnod=0;
			}	
		}
		break;
		case StatusConnRoute:
		{
			if(reConFlag)
			{
				for(i=0;i<MATSERIPCOUNT;i++)
				{
					if(strlen(g_szServer[i])>0 && g_iMasterPort>0)
					{
						iRet=GetControlServer(g_szServer[i],g_iMasterPort,USER,controlServer,sizeof(controlServer));
						//测试服务器121.43.234.112
						//memset(controlServer,0,sizeof(controlServer));
						//memcpy(controlServer,"121.43.234.112",strlen("121.43.234.112"));
						//iRet=0;
						//LOGOUT("use test Server");
						if(iRet==0)
						{	
							reConFlag=FALSE;
							clientStaus=StatusConnControl;
							LOGOUT("ControlServer Change statusConn %s start",controlServer);
							break;
						}
					}
				}
				iRet=GetControlServer(g_szServer[i],g_iMasterPort,USER,controlServer,sizeof(controlServer));
				if(iRet==0)
				{	
					reConFlag=FALSE;
					clientStaus=StatusConnControl;
					LOGOUT("ControlServer Change statusConn %s start",controlServer);
					break;
				}

			}
		}
		break;
		case StatusConnControl:
		{
			socketId=CreateConnect(controlServer,CONTROLSERVERPORT,128*1024,1000000000);
			if(socketId<0)
			{
				sleep(1);
				reConFlag=TRUE;
				clientStaus=StatusIdle;
				LOGOUT("connect ControlServer %s failure",controlServer);
			}
			else
			{	
				clientStaus=StatusWaitConn;
				LOGOUT("connect ControlServer %s success",controlServer);
			}	
		}
		break;
		case StatusWaitConn:
		{
			curMsgType=CONTROL_PROTOCAL_DEVICE_IDENTITY_VERIFY;
			curMsgSendTime=getTickCountMs();
			MsgBody_DEVICE_IDENTITY_VERIFY mMsgBody_DEVICE_IDENTITY_VERIFY;
			mMsgBody_DEVICE_IDENTITY_VERIFY.m_uMsgType=CONTROL_PROTOCAL_DEVICE_IDENTITY_VERIFY;
			mMsgBody_DEVICE_IDENTITY_VERIFY.m_user=USER;
			mMsgBody_DEVICE_IDENTITY_VERIFY.m_secret=SECRET;
			mMsgBody_DEVICE_IDENTITY_VERIFY.m_serverNo=g_serverNo;
			iRet=sendConServerMessage(socketId,CONTROL_PROTOCAL_DEVICE_IDENTITY_VERIFY,(void *)&mMsgBody_DEVICE_IDENTITY_VERIFY);
			if(iRet==0)
			{
				loginStatus=MSG_ACK_ERROR_STATUS_PACKET;
				clientStaus=StatusWorking;
				nowTimeMs=getTickCountMs();
				lastSendBeatTimeMs=nowTimeMs;
				lastRecvBeatTimeMs=nowTimeMs;
				LOGOUT("sendConnectServerLoginMessage %d %s %s %s",socketId,USER,SECRET,g_serverNo);
			}
			else
			{
				sleep(1);
				clientStaus=StatusDisConn;
				LOGOUT("sendConnectServerLoginMessage pushdata error %d %s %s %s",socketId,USER,SECRET,g_serverNo);
			}
		}
		break;
		case StatusWorking:
		{	
			nowTimeMs=getTickCountMs();
			iRet=GetSocketData(socketId,&v_dwSymb,sizeof(recBuffer),recBuffer,&length);
			if(iRet==0)
			{
				if(v_dwSymb==CMDCORRECT)
				{
					beatTimeTwoSend=FALSE;
					iRet=isValidPacket(recBuffer,length,&recMsgHead,recMsgBuffer,sizeof(recMsgBuffer));
					if(iRet==0)
					{
						if(recMsgHead.uMsgType!=CONTROL_PROTOCAL_KEEPALIVE_ACK)
						{
							LOGOUT("recv:uHeadLen=0x%x,cMsgVersion=0x%x,cPlatform=0x%x,uMsgType=0x%x,uMsgSeq=0x%x,uBodyLen=0x%x,msgBody=%s",
							recMsgHead.uHeadLen,recMsgHead.cMsgVersion,recMsgHead.cPlatform,recMsgHead.uMsgType
							,recMsgHead.uMsgSeq,recMsgHead.uBodyLen,recMsgBuffer);
						}
						else
						{
							if(beatPrintfTimes==20)
							{
								LOGOUT("recv:uHeadLen=0x%x,cMsgVersion=0x%x,cPlatform=0x%x,uMsgType=0x%x,uMsgSeq=0x%x,uBodyLen=0x%x,msgBody=%s",
								recMsgHead.uHeadLen,recMsgHead.cMsgVersion,recMsgHead.cPlatform,recMsgHead.uMsgType
								,recMsgHead.uMsgSeq,recMsgHead.uBodyLen,recMsgBuffer);
								beatPrintfTimes=0;
							}
						}
						switch(recMsgHead.uMsgType)
						{
							case 	CONTROL_PROTOCAL_DEVICE_IDENTITY_VERIFY_ACK:
							lastRecvBeatTimeMs=nowTimeMs;
							{
								if(curMsgType==CONTROL_PROTOCAL_DEVICE_IDENTITY_VERIFY)
									curMsgType=0;
								iRet=get_DEVICE_IDENTITY_VERIFY_ACK(&recMsgHead,recMsgBuffer,&loginStatus);
								if(iRet!=0) 
									LOGOUT("reloveConServerLoginMessageAck is %d",iRet);
								if(loginStatus==-1 || loginStatus==-2 || loginStatus==-3)
								{
									clientStaus=StatusDisConn;
									sleepTimeSecnod=60;
									break;
								}
								curMsgType=CONTROL_PROTOCAL_SETTIMEOUTDURATION;
								curMsgSendTime=nowTimeMs;
								MsgBody_SETTIMEOUTDURATION mMsgBody_SETTIMEOUTDURATION;
								mMsgBody_SETTIMEOUTDURATION.m_uMsgType=CONTROL_PROTOCAL_SETTIMEOUTDURATION;
								mMsgBody_SETTIMEOUTDURATION.m_durationSecond=BEATSERVERTIMEOUT;
								//timeStampFristFlag=TRUE;
								iRet=sendConServerMessage(socketId,CONTROL_PROTOCAL_SETTIMEOUTDURATION,&mMsgBody_SETTIMEOUTDURATION);
								if(iRet!=0) 
									LOGOUT("sendConServerMessage CONTROL_PROTOCAL_SETTIMEOUTDURATION is %d",iRet);
							}
								break;
							case	CONTROL_PROTOCAL_KEEPALIVE_ACK:
								lastRecvBeatTimeMs=nowTimeMs;
								//close(socketId);
								//LOGOUT("close socketId");
								break;
							case	CONTROL_PROTOCAL_SETTIMEOUTDURATION_ACK:
							{
								if(curMsgType==CONTROL_PROTOCAL_SETTIMEOUTDURATION)
									curMsgType=0;
								lastRecvBeatTimeMs=nowTimeMs;
								bSendBeatFlag=TRUE;
								MsgBody_DEVICE_REGISTER mMsgBody_DEVICE_REGISTER;
								curMsgType=CONTROL_PROTOCAL_DEVICE_REGISTER;
								curMsgSendTime=nowTimeMs;
								mMsgBody_DEVICE_REGISTER.m_uMsgType=CONTROL_PROTOCAL_DEVICE_REGISTER;
								mMsgBody_DEVICE_REGISTER.m_deviceProduct=DEVICEPRODUCT;
								mMsgBody_DEVICE_REGISTER.m_deviceModel=DEVICEMODEL;
								mMsgBody_DEVICE_REGISTER.m_deviceMacAddr=deviceNetInfo.m_MACAddress;
								mMsgBody_DEVICE_REGISTER.m_deviceServerNo=g_serverNo;
								mMsgBody_DEVICE_REGISTER.m_deviceChannelStartNum=DEVICECHANNELSTARTNUM;
								mMsgBody_DEVICE_REGISTER.m_deviceType=DEVICETYPE;
								mMsgBody_DEVICE_REGISTER.m_deviceChannelNum=DEVICECHANNELNUM;
								iRet=sendConServerMessage(socketId,CONTROL_PROTOCAL_DEVICE_REGISTER,&mMsgBody_DEVICE_REGISTER);
								if(iRet!=0) 
									LOGOUT("sendConServerMessage CONTROL_PROTOCAL_DEVICE_REGISTER is %d",iRet);
							}
								break;
							case	CONTROL_PROTOCAL_DEVICE_REGISTER_ACK:
							{
								if(curMsgType==CONTROL_PROTOCAL_DEVICE_REGISTER)
									curMsgType=0;
								lastRecvBeatTimeMs=nowTimeMs;
								//MsgBody_DEVICE_REGISTER_ACK mMSG_ACK_ERROR_STATUS_PACKET;
								memset(&globalDeviceStatus.m_deviceRegisterAck,0,sizeof(globalDeviceStatus.m_deviceRegisterAck));
								iRet=get_DEVICE_REGISTER_ACK(&recMsgHead,recMsgBuffer,&globalDeviceStatus.m_deviceRegisterAck);
								if(iRet!=0) 
								{
									LOGOUT("get_DEVICE_REGISTER_ACK is %d",iRet);
								}
								if(globalDeviceStatus.m_deviceRegisterAck.m_registerStatus==-1)
								{
									clientStaus=StatusDisConn;
									sleepTimeSecnod=60;
									break;
								}
								else if(globalDeviceStatus.m_deviceRegisterAck.m_registerStatus==0)
								{
									clientStaus=StatusDisConn;
									sleepTimeSecnod=1;
									break;
								}
								#if 0
								MsgBody_GET_ENCODE_CONFIG_TABLE mMsgBody_GET_ENCODE_CONFIG_TABLE;
								mMsgBody_GET_ENCODE_CONFIG_TABLE.m_uMsgType=CONTROL_PROTOCAL_GET_ENCODE_CONFIG_TABLE;
								mMsgBody_GET_ENCODE_CONFIG_TABLE.m_timeStamp=(timeStampFristFlag==TRUE)?0:getTimeStamp();
								iRet=sendConServerMessage(socketId,CONTROL_PROTOCAL_GET_ENCODE_CONFIG_TABLE,&mMsgBody_GET_ENCODE_CONFIG_TABLE);
								if(iRet!=0) 
									LOGOUT("sendConServerMessage CONTROL_PROTOCAL_GET_ENCODE_CONFIG_TABLE is %d",iRet);	
								timeStampFristFlag=FALSE;
								#endif
								curMsgType=CONTROL_PROTOCAL_PUBLISH_CHANNEL_STATE;
								curMsgSendTime=nowTimeMs;
								MsgBody_PUBLISH_CHANNEL_STATE mMsgBody_PUBLISH_CHANNEL_STATE;
								mMsgBody_PUBLISH_CHANNEL_STATE.m_uMsgType=CONTROL_PROTOCAL_PUBLISH_CHANNEL_STATE;
								mMsgBody_PUBLISH_CHANNEL_STATE.m_channelId=globalDeviceStatus.m_deviceRegisterAck.m_channelId;
								mMsgBody_PUBLISH_CHANNEL_STATE.m_liveStatus=0;
								iRet=sendConServerMessage(socketId,CONTROL_PROTOCAL_PUBLISH_CHANNEL_STATE,&mMsgBody_PUBLISH_CHANNEL_STATE);
								if(iRet!=0) 
									LOGOUT("sendConServerMessage CONTROL_PROTOCAL_PUBLISH_CHANNEL_STATE is %d",iRet);	
							}
								break;
							case	CONTROL_PROTOCAL_PUBLISH_CHANNEL_STATE_ACK:
								if(curMsgType==CONTROL_PROTOCAL_PUBLISH_CHANNEL_STATE)
									curMsgType=0;	
								break;
							case	CONTROL_PROTOCAL_GET_ENCODE_CONFIG_TABLE_ACK:
							{
								if(curMsgType==CONTROL_PROTOCAL_PUBLISH_CHANNEL_STATE)
									curMsgType=0;
								lastRecvBeatTimeMs=nowTimeMs;
								MsgBody_PUBLISH_CHANNEL_STATE mMsgBody_PUBLISH_CHANNEL_STATE;
								mMsgBody_PUBLISH_CHANNEL_STATE.m_uMsgType=CONTROL_PROTOCAL_PUBLISH_CHANNEL_STATE;
								mMsgBody_PUBLISH_CHANNEL_STATE.m_channelId=globalDeviceStatus.m_deviceRegisterAck.m_channelId;
								mMsgBody_PUBLISH_CHANNEL_STATE.m_liveStatus=0;
								iRet=sendConServerMessage(socketId,CONTROL_PROTOCAL_PUBLISH_CHANNEL_STATE,&mMsgBody_PUBLISH_CHANNEL_STATE);
								if(iRet!=0) 
									LOGOUT("sendConServerMessage CONTROL_PROTOCAL_PUBLISH_CHANNEL_STATE is %d",iRet);	
							}
								break;
							case 	CONTROL_PROTOCAL_OPEN_CHANNEL_PREVIEW:
							{
								lastRecvBeatTimeMs=nowTimeMs;
								//MsgBody_OPEN_CHANNEL_PREVIEW_ACK mMsgBody_OPEN_CHANNEL_PREVIEW;
								memset(&globalDeviceStatus.m_openChannelPreviewAck,0,sizeof(globalDeviceStatus.m_openChannelPreviewAck));
								iRet=get_OPEN_CHANNEL_PREVIEW(&recMsgHead,recMsgBuffer,&globalDeviceStatus.m_openChannelPreviewAck);
								if(iRet!=0) 
									LOGOUT("get_DEVICE_REGISTER_ACK is %d",iRet);
								printfVideoParam("Server Set Video Param",globalDeviceStatus.m_openChannelPreviewAck.m_video);
								LOGOUT("Server timeStamp:%s",globalDeviceStatus.m_openChannelPreviewAck.m_serverStamp);
								iRet=startVideoStream(globalDeviceStatus.m_openChannelPreviewAck.m_video);
								if(iRet!=0)
								{
									curMsgType=CONTROL_PROTOCAL_PREVIEW_FAILURE_NOTIFY;
									curMsgSendTime=nowTimeMs;
									MsgBody_PREVIEW_FAILURE_NOTIFY mMsgBody_PREVIEW_FAILURE_NOTIFY;
									memset(&mMsgBody_PREVIEW_FAILURE_NOTIFY,0,sizeof(mMsgBody_PREVIEW_FAILURE_NOTIFY));
									mMsgBody_PREVIEW_FAILURE_NOTIFY.m_uMsgType=CONTROL_PROTOCAL_PREVIEW_FAILURE_NOTIFY;
									mMsgBody_PREVIEW_FAILURE_NOTIFY.m_channelId=globalDeviceStatus.m_deviceRegisterAck.m_channelId;
									if(iRet==-1)
										mMsgBody_PREVIEW_FAILURE_NOTIFY.m_failureType=-2;
									else if(iRet==-2)
										mMsgBody_PREVIEW_FAILURE_NOTIFY.m_failureType=-1;
									strcpy(mMsgBody_PREVIEW_FAILURE_NOTIFY.m_serverStamp,globalDeviceStatus.m_openChannelPreviewAck.m_serverStamp);
									iRet=sendConServerMessage(socketId,CONTROL_PROTOCAL_PREVIEW_FAILURE_NOTIFY,&mMsgBody_PREVIEW_FAILURE_NOTIFY);
									if(iRet!=0) 
										LOGOUT("sendConServerMessage CONTROL_PROTOCAL_PREVIEW_FAILURE_NOTIFY is %d",iRet);	
									break;
								}
								char rtmpAddr[1256]={0,};
								sprintf(rtmpAddr,"rtmp://%s:%d/%s/%d",globalDeviceStatus.m_openChannelPreviewAck.m_rtmpIp,globalDeviceStatus.m_openChannelPreviewAck.m_rtmpPort
														 ,globalDeviceStatus.m_openChannelPreviewAck.m_rtmpPath,globalDeviceStatus.m_openChannelPreviewAck.m_channelId/*, globalDeviceStatus.m_openChannelPreviewAck.m_rtmpUUID*/);
								LOGOUT("rtmpAddr:%s\n", rtmpAddr);
								pRtmpServer=RtmpUnitOpen(globalDeviceStatus.m_openChannelPreviewAck.m_video.u32Width,globalDeviceStatus.m_openChannelPreviewAck.m_video.u32Height,rtmpAddr,0);
								if(pRtmpServer!=NULL)
								{
									InitRTMPUnit(pRtmpServer);
									bSendErrorPlay=TRUE;
								}
								else
								{
									LOGOUT("rtmpAddr error\n");
								}
								
							}
								break;
							case CONTROL_PROTOCAL_PREVIEW_FAILURE_NOTIFY_ACK://远程预览失败通知应答
							{
								if(curMsgType==CONTROL_PROTOCAL_PREVIEW_FAILURE_NOTIFY)
									curMsgType=0;
							}
								break;
							case 	CONTROL_PROTOCAL_CLOSE_CHANNEL_PREVIEW:
							{
								lastRecvBeatTimeMs=nowTimeMs;
								if(pRtmpServer!=NULL)
								{
									CloseRTMPUnit(pRtmpServer);
									pRtmpServer=NULL;
								}
							}
								break;
							case 	CONTROL_PROTOCAL_UPLOAD_CHANNEL_KEYFRAME:
								lastRecvBeatTimeMs=nowTimeMs;
								iRet=MakeKeyFrame();
								LOGOUT("make key Frame iRet:%d",iRet);
								break;
							case	CONTROL_PROTOCAL_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG:
							{
								lastRecvBeatTimeMs=nowTimeMs;
								MsgBody_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG mMsgBody_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG;
								memset(&mMsgBody_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG,0,sizeof(mMsgBody_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG));
								iRet=get_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG(&recMsgHead,recMsgBuffer,&mMsgBody_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG);
								if(iRet!=0) 
									LOGOUT("get_DEVICE_REGISTER_ACK is %d",iRet);
								//if(mMsgBody_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG.m_video.u32Stream>=0 && mMsgBody_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG.m_video.u32Stream<=2)
								//	mMsgBody_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG.m_video=deviceVedeioEx[2-mMsgBody_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG.m_video.u32Stream];
								printfVideoParam("Server Set Video Param",mMsgBody_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG.m_video);
								startVideoStream(mMsgBody_CHANGE_CHANNEL_IMAGEQUALITY_CONFIG.m_video);
								break;
							}
							#if 0
 							case	CONTROL_PROTOCAL_UPDATE_CHANNEL_CONFIG_ACK:
								#if 0
								MsgBody_UPDATE_CHANNEL_CONFIG mMsgBody_UPDATE_CHANNEL_CONFIG;
								mMsgBody_UPDATE_CHANNEL_CONFIG.m_uMsgType=CONTROL_PROTOCAL_UPDATE_CHANNEL_CONFIG;
								mMsgBody_UPDATE_CHANNEL_CONFIG.m_channelId=1739;
								mMsgBody_UPDATE_CHANNEL_CONFIG.m_channelNumber=1;
								mMsgBody_UPDATE_CHANNEL_CONFIG.m_imageLevel=1;
								mMsgBody_UPDATE_CHANNEL_CONFIG.m_armingSwitch=0;
								iRet=sendConServerMessage(socketId,CONTROL_PROTOCAL_UPDATE_CHANNEL_CONFIG,&mMsgBody_UPDATE_CHANNEL_CONFIG);
								if(iRet!=0) 
									LOGOUT("sendConServerMessage CONTROL_PROTOCAL_UPDATE_CHANNEL_CONFIG is %d",iRet);
								#endif
								break;
							case	CONTROL_PROTOCAL_PREVIEW_FAILURE_NOTIFY_ACK:
								break;
							case	CONTROL_PROTOCAL_OPEN_CHANNEL_TALK_ACK:
								break;
							case	CONTROL_PROTOCAL_TALK_FAILURE_NOTIFY_ACK:
								break;
							case	CONTROL_PROTOCAL_GET_CHANNEL_CONFIG_ACK:
								
								break;
							case	CONTROL_PROTOCAL_SET_CHANNEL_CONFIG_ACK:
								break;
							case	CONTROL_PROTOCAL_CHANGE_CHANNEL_IMAGE_QUALITY_ACK:
								break;
							case	CONTROL_PROTOCAL_COLLECT_YUNTAI_CONTROL_ERROR_ACK:
								break;
							case	CONTROL_PROTOCAL_COLLECT_PRESET_CRUISE_CONTROL_ACK:
								break;
							case	CONTROL_PROTOCAL_COLLECT_PRESET_CRUISE_CONTROL_ERROR_ACK:
								break;
							#endif  
							default: 
							{
								if(recMsgHead.uMsgType>=0x3000 && recMsgHead.uMsgType<=0x39FF)
								{
									lastRecvBeatTimeMs=nowTimeMs;
									LOGOUT("do not support cmd is 0x%x",recMsgHead.uMsgType);
									recMsgHead.uMsgType=recMsgHead.uMsgType+2;
									memset(sendOtherMsgBuffer,0,sizeof(0));
									memcpy(sendOtherMsgBuffer,&recMsgHead,sizeof(recMsgHead));
									memcpy(sendOtherMsgBuffer+sizeof(recMsgHead),recMsgBuffer,recMsgHead.uBodyLen);
									iRet=SendSocketData(socketId,sendOtherMsgBuffer,recMsgHead.uBodyLen+sizeof(MsgHead));	
									if(iRet!=0)
									{
										LOGOUT("do not support cmd is 0x%x send error iRet=%d",recMsgHead.uMsgType-2,iRet);
									}
								}
							}
								break;
						}
						memset(recBuffer,0,sizeof(recBuffer));
						memset(recMsgBuffer,0,sizeof(recMsgBuffer));
					}
				}
				else if(v_dwSymb==CMDRECONNECT)
				{
					sleep(1);
					clientStaus=StatusDisConn;
					LOGOUT("connect server socket %d is break out ",socketId);
					break;
				}
			}
			if(pRtmpServer!=NULL && GetRtmpConnectStatus() == 0 && bSendErrorPlay==TRUE)//zmt,当RTMP传输异常时，通知控制服务器
			{
				bSendErrorPlay=FALSE;
				curMsgType=CONTROL_PROTOCAL_PREVIEW_FAILURE_NOTIFY;
				curMsgSendTime=nowTimeMs;
				MsgBody_PREVIEW_FAILURE_NOTIFY mMsgBody_PREVIEW_FAILURE_NOTIFY;
				memset(&mMsgBody_PREVIEW_FAILURE_NOTIFY,0,sizeof(mMsgBody_PREVIEW_FAILURE_NOTIFY));
				mMsgBody_PREVIEW_FAILURE_NOTIFY.m_uMsgType=CONTROL_PROTOCAL_PREVIEW_FAILURE_NOTIFY;
				mMsgBody_PREVIEW_FAILURE_NOTIFY.m_channelId=globalDeviceStatus.m_deviceRegisterAck.m_channelId;
				if(iRet==-1)
					mMsgBody_PREVIEW_FAILURE_NOTIFY.m_failureType=-2;
				else if(iRet==-2)
					mMsgBody_PREVIEW_FAILURE_NOTIFY.m_failureType=-1;
				strcpy(mMsgBody_PREVIEW_FAILURE_NOTIFY.m_serverStamp,globalDeviceStatus.m_openChannelPreviewAck.m_serverStamp);
				iRet=sendConServerMessage(socketId,CONTROL_PROTOCAL_PREVIEW_FAILURE_NOTIFY,&mMsgBody_PREVIEW_FAILURE_NOTIFY);
				if(iRet!=0) 
					LOGOUT("sendConServerMessage CONTROL_PROTOCAL_PREVIEW_FAILURE_NOTIFY is %d",iRet);	
				break;
			}
			if(bSendBeatFlag)
			{
				if((nowTimeMs-lastSendBeatTimeMs)>BEATTIME)
				{
					lastSendBeatTimeMs=nowTimeMs;
					iRet=sendConServerMessage(socketId,CONTROL_PROTOCAL_KEEPALIVE,NULL);
				}
				if((nowTimeMs-lastRecvBeatTimeMs)>BEATTIMEOUT)
				{
					clientStaus=StatusDisConn;
					LOGOUT("StatusWorking beat time out");
				}

			}
			if(curMsgType!=0)
			{
				if((nowTimeMs-curMsgSendTime)>BEATTIME)
				{
					clientStaus=StatusDisConn;
					LOGOUT("receive cmd timeout,cmdId:%d",curMsgType);
				}
			}
			usleep(200*1000);
		}
		break;
		case StatusDisConn:
		{	
			if(socketId>0)
			{
				DelSocketId(socketId);
				socketId=-1;
			}
			reConFlag=1;
			clientStaus=StatusIdle;
			LOGOUT("ControlServer %s DisConnect",controlServer);
		}
		break;
		default:
			break;
		}
		usleep(200*1000);
	}
	return NULL;
}


int InitControlServer()
{
	int iRet;
	pthread_t pCtrlThreadWork;
	memset(g_serverNo,0,sizeof(g_serverNo));
	iRet=getServerNo(SERVERFILE,g_serverNo);
	if(iRet!=0)
	{
		LOGOUT("getServerNo is error iRet=%d",iRet);
	}
	iRet=getMasterIpAndPort();
	if(iRet!=0)
	{
		LOGOUT("getMasterIpAndPort is error iRet=%d",iRet);
	}
	startControlServer=TRUE;
	iRet = pthread_create(&pCtrlThreadWork, NULL, P_CtrlSocketThread, NULL);
	if(iRet)
	{
		LOGOUT("Create P_CtrlSocketThread Fail!!\n");
		return -3;
	}
	pthread_detach(pCtrlThreadWork);
	return 0;
}
int ReleaseControlServer()
{
	startControlServer=FALSE;
	sleep(1);
	return 0;
}

