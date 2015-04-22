#include "hi_sdk.h"
#include "../LogOut/LogOut.h"
#include "../rtmp/RtmpDLL.h"



extern PRTMPUnit pRtmpServer;

#if 0
typedef struct HI_VideoEx
{
	HI_U32 u32Channel;      /* 通道 */
	HI_BOOL blFlag;          /* 主、次码流  */
	HI_U32 u32Bitrate;	/* 码流 */
	HI_U32 u32Frame;    /* 帧率 */
	HI_U32 u32Iframe;	/* 主帧间隔1-300 */
	HI_BOOL blCbr;	    /* 码流模式、HI_TRUE为固定码流，HI_FALSE为可变码流 */		
	HI_U32 u32ImgQuality;	/* 编码质量1-6 */
	HI_U32 u32Width;
	HI_U32 u32Height;
} HI_S_VideoEx;

const HI_VideoEx deviceVedeioEx[]= {
										{1,HI_TRUE,}
									  	{1280,720},
									  	{640,352},
									 	{320,176}
									};
#endif


HI_S32 OnEventCallback(HI_U32 u32Handle, /* 句柄 */
                                HI_U32 u32Event,      /* 事件 */
                                HI_VOID* pUserData  /* 用户数据*/
                                )
{
	return HI_SUCCESS;
}

void SaveRecordFile(HI_CHAR* pPath, HI_U8* pu8Buffer, HI_U32 u32Length)
{
	FILE* fp;
	fp = fopen(pPath, "ab+");
	fwrite(pu8Buffer, 1, u32Length, fp);
	fclose(fp);
}

HI_S32 OnStreamCallback(HI_U32 u32Handle, /* 句柄 */
                                HI_U32 u32DataType,     /* 数据类型，视频或音频数据或音视频复合数据 */
                                HI_U8*  pu8Buffer,      /* 数据包含帧头 */
                                HI_U32 u32Length,      /* 数据长度 */
                                HI_VOID* pUserData    /* 用户数据*/
                                )
{

	HI_S_AVFrame* pstruAV = HI_NULL;
	HI_S_SysHeader* pstruSys = HI_NULL;
	

	if (u32DataType == HI_NET_DEV_AV_DATA)
	{
		pstruAV = (HI_S_AVFrame*)pu8Buffer;

		if (pstruAV->u32AVFrameFlag == HI_NET_DEV_VIDEO_FRAME_FLAG)
		{
			//printf("Video %u PTS: %u \n", pstruAV->u32VFrameType, pstruAV->u32AVFramePTS);
			//SaveRecordFile("Video.hx", pu8Buffer, u32Length);
			if(pRtmpServer!=NULL)
			{
				SendVideoUnit(pRtmpServer,pu8Buffer+sizeof(HI_S_AVFrame),u32Length-sizeof(HI_S_AVFrame),pstruAV->u32AVFramePTS);
				if(pstruAV->u32VFrameType==1)
					LOGOUT("Get Video %u PTS: %u \n", pstruAV->u32VFrameType, pstruAV->u32AVFramePTS);
			}
		}
		else
		if (pstruAV->u32AVFrameFlag == HI_NET_DEV_AUDIO_FRAME_FLAG)
		{
			//printf("Audio %u PTS: %u \n", pstruAV->u32AVFrameLen, pstruAV->u32AVFramePTS);
			//SaveRecordFile("Video.hx", pu8Buffer, u32Length);			
		}
	}
	else
	if (u32DataType == HI_NET_DEV_SYS_DATA)
	{
		pstruSys = (HI_S_SysHeader*)pu8Buffer;
		printf("Video W:%u H:%u Audio: %u \n", pstruSys->struVHeader.u32Width, pstruSys->struVHeader.u32Height, pstruSys->struAHeader.u32Format);
	} 

	return HI_SUCCESS;
}

HI_S32 OnDataCallback(HI_U32 u32Handle, /* 句柄 */
                                HI_U32 u32DataType,       /* 数据类型*/
                                HI_U8*  pu8Buffer,      /* 数据 */
                                HI_U32 u32Length,      /* 数据长度 */
                                HI_VOID* pUserData    /* 用户数据*/
                                )
{
	return HI_SUCCESS;
}


int getUnameAndPassWord(char * path,char uname[64],char password[64])
{
	FILE * fp;  
	char * line = NULL;  
	size_t len = 0;  
	ssize_t read;  
	int count=0;
	int userCount=0;
	char *findPtr=NULL;
	int iRet=-1;
	fp = fopen(path, "r");  
	if(fp == NULL)  
		return -1;
	while((read = getline(&line, &len, fp)) != -1) 
	{    
		 if(strstr(line,"username")!=NULL)
		 {
			findPtr=strchr(line,'"');
			if(findPtr!=NULL)
			{
				iRet=sscanf(findPtr,"\"%s\"",uname);
				//printf("uname=%s--\n",uname);
				if(iRet==1)
					userCount=count;
			}
		 }
		 
		 if(strstr(line,"password")!=NULL)
		 {
			findPtr=strchr(line,'"');
			if(findPtr!=NULL)
			{
				iRet=sscanf(findPtr,"\"%s\"",password);
				//printf("password=%s--\n",password);
				if(count==(userCount+1))
				{
					iRet=0;
					break;
				}
			}
		 }
		 count++;
	}  
	if (line)  
	 	free(line);  
	if(fp != NULL)
		fclose(fp);
	return iRet;
}

int getIPAndPort(char * path,char ip[64],char port[64])
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
		 if(strstr(line,"ipaddr")!=NULL)
		 {
			findPtr=strchr(line,'"');
			if(findPtr!=NULL)
			{
				iRet=sscanf(findPtr,"\"%s\"",ip);
				//printf("uname=%s--\n",ip);
			}
		 }
		 
		 if(strstr(line,"httpport")!=NULL)
		 {
			findPtr=strchr(line,'"');
			if(findPtr!=NULL)
			{
				iRet=sscanf(findPtr,"\"%s\"",port);
				if(iRet==1)
				{
					iRet=0;
					break;
				}
				//printf("password=%s--\n",port);
			}
		 }
	}  
	if (line)  
	 	free(line);  
	if(fp != NULL)
		fclose(fp);
	return iRet;
}

int InitHiSDKServer(HI_U32 *u32Handle)
{
    HI_S32 s32Ret = HI_SUCCESS;
	HI_U32 a;
    HI_NET_DEV_Init();

	char uname[64]={0,};
	char password[64]={0,};
	memset(uname,0,sizeof(uname));
	memset(password,0,sizeof(password));
	int iRet=getUnameAndPassWord(UNAMEFILE,uname,password);
	LOGOUT("iRet=%d uname %s password %s",iRet,uname,password);
	if(iRet!=0)
	{
		memcpy(uname,UNAME,MIN(sizeof(uname),strlen(UNAME)));
		memcpy(password,PWORD,MIN(sizeof(password),strlen(PWORD)));
	}
	if(strlen(uname)==0)
		memcpy(uname,UNAME,MIN(sizeof(uname),strlen(UNAME)));

	char ip[64]={0,};
	char port[64]={0,};
	memset(ip,0,sizeof(ip));
	memset(port,0,sizeof(port));
	iRet=getIPAndPort(NETFILE,ip,port);
	LOGOUT("iRet=%d ip %s port %s",iRet,ip,port);
	if(iRet!=0)
	{
		memcpy(ip,IPHOST,MIN(sizeof(ip),strlen(IPHOST)));
		memcpy(port,IPPORT,MIN(sizeof(port),strlen(IPPORT)));
	}
	if(strlen(ip)==0)
		memcpy(ip,IPHOST,MIN(sizeof(ip),strlen(IPHOST)));
	if(strlen(port)==0)
		memcpy(port,IPPORT,MIN(sizeof(port),strlen(IPPORT)));
	HI_U16 u16Port=atoi(port);
    s32Ret = HI_NET_DEV_Login(u32Handle,uname,password,ip,u16Port);
    if(s32Ret != HI_SUCCESS)
    {
        HI_NET_DEV_DeInit();
		*u32Handle=0;
		LOGOUT("HI_NET_DEV_Login is failure 0x%x",s32Ret);
		return -1;
    }
	#if 0
	s32Ret=HI_NET_DEV_SetEventCallBack(*u32Handle, OnEventCallback, &a);
	if(s32Ret != HI_SUCCESS)
	{
		LOGOUT("HI_NET_DEV_SetEventCallBack is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
		s32Ret = HI_NET_DEV_Logout(*u32Handle);
		if (s32Ret != HI_SUCCESS)
		{
			LOGOUT("HI_NET_DEV_Logout is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);	
		}	
		s32Ret = HI_NET_DEV_DeInit();
		if (s32Ret != HI_SUCCESS)
		{
			LOGOUT("HI_NET_DEV_DeInit is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);	
		}	
		*u32Handle = 0;
		return -2;
	}
	#endif
	s32Ret=HI_NET_DEV_SetStreamCallBack(*u32Handle, OnStreamCallback, &a);
	if(s32Ret != HI_SUCCESS)
	{
		LOGOUT("HI_NET_DEV_SetStreamCallBack is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
		s32Ret = HI_NET_DEV_Logout(*u32Handle);
		if (s32Ret != HI_SUCCESS)
		{
			LOGOUT("HI_NET_DEV_Logout is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);	
		}	
		s32Ret = HI_NET_DEV_DeInit();
		if (s32Ret != HI_SUCCESS)
		{
			LOGOUT("HI_NET_DEV_DeInit is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);	
		}	
		*u32Handle = 0;
		return -3;
	}
	#if 0
	s32Ret=HI_NET_DEV_SetDataCallBack(*u32Handle, OnDataCallback, &a);
	if(s32Ret != HI_SUCCESS)
	{
		LOGOUT("HI_NET_DEV_SetDataCallBack is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
		s32Ret = HI_NET_DEV_Logout(*u32Handle);
		if (s32Ret != HI_SUCCESS)
		{
			LOGOUT("HI_NET_DEV_Logout is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);	
		}	
		s32Ret = HI_NET_DEV_DeInit();
		if (s32Ret != HI_SUCCESS)
		{
			LOGOUT("HI_NET_DEV_DeInit is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);	
		}	
		*u32Handle = 0;
		return -4;
	}
	#endif
	return 0;
}

int getVideoParam(HI_U32 *u32Handle,HI_S_Video_Ext *sVideo)
{
	HI_S32 s32Ret = HI_SUCCESS;
	int iRet=-1;
	s32Ret=HI_NET_DEV_GetConfig(*u32Handle,HI_NET_DEV_CMD_VIDEO_PARAM_EXT,sVideo,sizeof(HI_S_Video_Ext));
	if (s32Ret != HI_SUCCESS)
	{
		LOGOUT("HI_NET_DEV_GetConfig is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
		iRet=ReleaseHiSDKServer(u32Handle);
		iRet=InitHiSDKServer(u32Handle);
		if(iRet==0)
		{
			s32Ret=HI_NET_DEV_GetConfig(*u32Handle,HI_NET_DEV_CMD_VIDEO_PARAM_EXT,sVideo,sizeof(HI_S_Video_Ext));
			
			if (s32Ret != HI_SUCCESS)
			{
				LOGOUT("HI_NET_DEV_GetConfig is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
				iRet=ReleaseHiSDKServer(u32Handle);
				return -2;
			}
			else
			{
				return 0;
			}	
		}
		else
		{
			LOGOUT("InitHiSDKServer is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
			*u32Handle = 0;
			return -3;
		}
	}  	
	return 0;
}


int setVideoParam(HI_U32 *u32Handle,HI_S_Video_Ext sVideo)
{
	HI_S32 s32Ret = HI_SUCCESS;
	int iRet=-1;
	
	s32Ret=HI_NET_DEV_SetConfig(*u32Handle,HI_NET_DEV_CMD_VIDEO_PARAM_EXT,&sVideo,sizeof(HI_S_Video_Ext));
	if (s32Ret != HI_SUCCESS)
	{
		LOGOUT("HI_NET_DEV_SetConfig is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
		iRet=ReleaseHiSDKServer(u32Handle);
		iRet=InitHiSDKServer(u32Handle);
		if(iRet==0)
		{
			s32Ret=HI_NET_DEV_SetConfig(*u32Handle,HI_NET_DEV_CMD_VIDEO_PARAM_EXT,&sVideo,sizeof(HI_S_Video_Ext));
			
			if (s32Ret != HI_SUCCESS)
			{
				LOGOUT("HI_NET_DEV_SetConfig is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
				iRet=ReleaseHiSDKServer(u32Handle);
				return -2;
			}
			else
			{
				return 0;
			}	
		}
		else
		{
			LOGOUT("InitHiSDKServer is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
			*u32Handle = 0;
			return -3;
		}
	}  	
	return 0;
}

HI_U32 u32SaveStream=10; 
int startVideoStream(HI_U32 *u32Handle,HI_S_Video_Ext sVideo)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S_STREAM_INFO_EXT struStreamInfo;
	int iRet=-1;
	HI_U32 a;
	int isImageValid=0;

	if(sVideo.u32Width==DEVICEWIDTHBIG && sVideo.u32Height==DEVICEHIGHHBIG)
	{
		
		sVideo.u32Stream=0;
		struStreamInfo.u32Stream=0;
		sVideo.u32Width=DEVICEWIDTHBIG;
		sVideo.u32Height=DEVICEHIGHHBIG;
		isImageValid=1;
	}
	else if(sVideo.u32Width==DEVICEWIDTHMID && sVideo.u32Height==DEVICEHIGHHMID)
	{
		sVideo.u32Stream=1;
		struStreamInfo.u32Stream=0;
		isImageValid=1;
	}
	else if(sVideo.u32Width==DEVICEWIDTHSMALL && sVideo.u32Height==DEVICEHIGHHSMALL)
	{
		sVideo.u32Stream=1;
		struStreamInfo.u32Stream=2;
		isImageValid=1;
	}
	else
	{
		struStreamInfo.u32Stream=1;
		sVideo.u32Stream=1;
		sVideo.u32Width=DEVICEWIDTHMID;
		sVideo.u32Height=DEVICEHIGHHMID;
		isImageValid=1;
	}

	if(u32SaveStream!=struStreamInfo.u32Stream)
	{
		ReleaseHiSDKServer(u32Handle);
	}
	
	if(*u32Handle == 0)
	{
		iRet=InitHiSDKServer(u32Handle);
		if(iRet!=0)
		{
			LOGOUT("InitHiSDKServer is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
			*u32Handle = 0;
			return -1;
		}
		LOGOUT("InitHiSDKServer over %u",*u32Handle);
	}
	setVideoParam(u32Handle,sVideo);	
	
	struStreamInfo.u32Channel = HI_NET_DEV_CHANNEL_1;
	struStreamInfo.u32Mode = HI_NET_DEV_STREAM_MODE_TCP;
	struStreamInfo.u8Type = HI_NET_DEV_STREAM_VIDEO_AUDIO;
	s32Ret = HI_NET_DEV_StartStreamExt(*u32Handle, &struStreamInfo);
	u32SaveStream=struStreamInfo.u32Stream;
	if (s32Ret != HI_SUCCESS)
	{
		LOGOUT("HI_NET_DEV_StartStream is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
		iRet=ReleaseHiSDKServer(u32Handle);
		iRet=InitHiSDKServer(u32Handle);
		if(iRet==0)
		{
			s32Ret =  HI_NET_DEV_StartStreamExt(*u32Handle, &struStreamInfo);
			if (s32Ret != HI_SUCCESS)
			{
				LOGOUT("HI_NET_DEV_StartStream is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
				iRet=ReleaseHiSDKServer(u32Handle);
				return -2;
			}
			else
			{
				return 0;
			}	
		}
		else
		{
			LOGOUT("InitHiSDKServer is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
			*u32Handle = 0;
			return -3;
		}
	}   
	
	return 0;

}

int stopVideoStream(HI_U32 *u32Handle)
{
	HI_S32 s32Ret = HI_SUCCESS;
	s32Ret = HI_NET_DEV_StopStream(*u32Handle);
	if (s32Ret != HI_SUCCESS)
	{
		LOGOUT("HI_NET_DEV_StopStream is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);	
		*u32Handle=0;
		return -1;
	}
    return 0;
}

int ReleaseHiSDKServer(HI_U32 *u32Handle)
{
	
	HI_S32 s32Ret = HI_SUCCESS;
	if(*u32Handle != 0)
	{
		s32Ret = HI_NET_DEV_StopStream(*u32Handle);
		if (s32Ret != HI_SUCCESS)
		{
			LOGOUT("HI_NET_DEV_StopStream is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);	
		}
		s32Ret = HI_NET_DEV_Logout(*u32Handle);
		if (s32Ret != HI_SUCCESS)
		{
			LOGOUT("HI_NET_DEV_Logout is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);	
		}	
		*u32Handle=0;
	}
	return 0;
}
    


