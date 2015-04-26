#include <stdio.h>
#include <sys/syscall.h>//Linux system call for thread id
#include <assert.h>
#include <pthread.h>
#include "hi_sdk.h"
#include "../LogOut/LogOut.h"
#include "../rtmp/RtmpDLL.h"



extern PRTMPUnit 	pRtmpServer;
static HI_U32 		u32HandleHight=0;
static HI_U32 		u32HandleMid=0;
static HI_U32 		u32HandleLow=0;
static HI_U32 		u32ChannelFlag=0;  // 1 hight 2 mid 3 low
static HI_S_Video_Ext curVideoParam;

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
	HI_U32 validU32Handle=0;

	if (u32DataType == HI_NET_DEV_AV_DATA)
	{
		pstruAV = (HI_S_AVFrame*)pu8Buffer;

		if (pstruAV->u32AVFrameFlag == HI_NET_DEV_VIDEO_FRAME_FLAG)
		{

			//printf("child thread lwpid=%u\n",syscall(SYS_gettid));
			//printf("child thread tid=%u\n",pthread_self());
			//printf("Video %u PTS: %u \n", pstruAV->u32VFrameType, pstruAV->u32AVFramePTS);
			//SaveRecordFile("Video.hx", pu8Buffer, u32Length);
			switch(u32ChannelFlag)
			{
				case 1:
						validU32Handle=u32HandleHight;
						break;
						
				case 2:
						validU32Handle=u32HandleMid;
						break;
				case 3:
						validU32Handle=u32HandleLow;
						break;	
				default:
						validU32Handle=u32HandleMid;
						break;
			}
			if(validU32Handle==u32Handle)
			{
				if(pRtmpServer!=NULL)
				{
					SendVideoUnit(pRtmpServer,pu8Buffer+sizeof(HI_S_AVFrame),u32Length-sizeof(HI_S_AVFrame),pstruAV->u32AVFramePTS);
					//if(pstruAV->u32VFrameType==1)
					//	LOGOUT("Get Video %u PTS: %u \n", pstruAV->u32VFrameType, pstruAV->u32AVFramePTS);
					/*if(GetRtmpConnectStatus() == 0)
					{
						
					}*/
				}

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
		 if(strstr(line,"username")==line)
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
		 
		 if(strstr(line,"password")==line)
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
		 if(strstr(line,"ipaddr")==line)
		 {
			findPtr=strchr(line,'"');
			if(findPtr!=NULL)
			{
				iRet=sscanf(findPtr,"\"%s\"",ip);
				//printf("uname=%s--\n",ip);
			}
		 }
		 
		 if(strstr(line,"httpport")==line)
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


int InitHiSDKServer(HI_U32 *u32Handle,HI_U32 u32Stream)
{
    HI_S32 s32Ret = HI_SUCCESS;
	HI_U32 a;
	HI_S_STREAM_INFO_EXT struStreamInfo;
	
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
	if(*u32Handle!=0)
	{
		s32Ret = HI_NET_DEV_Logout(*u32Handle);
		if (s32Ret != HI_SUCCESS)
		{
			LOGOUT("HI_NET_DEV_Logout is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);	
		}	
	}
    s32Ret = HI_NET_DEV_Login(u32Handle,uname,password,ip,u16Port);
    if(s32Ret != HI_SUCCESS)
    {
		*u32Handle=0;
		LOGOUT("HI_NET_DEV_Login is failure 0x%x",s32Ret);
		return -1;
    }
	
	s32Ret=HI_NET_DEV_SetEventCallBack(*u32Handle, OnEventCallback, &a);
	if(s32Ret != HI_SUCCESS)
	{
		LOGOUT("HI_NET_DEV_SetEventCallBack is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
		s32Ret = HI_NET_DEV_Logout(*u32Handle);
		if (s32Ret != HI_SUCCESS)
		{
			LOGOUT("HI_NET_DEV_Logout is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);	
		}	
		*u32Handle = 0;
		return -2;
	}
	
	s32Ret=HI_NET_DEV_SetStreamCallBack(*u32Handle, OnStreamCallback, &a);
	if(s32Ret != HI_SUCCESS)
	{
		LOGOUT("HI_NET_DEV_SetStreamCallBack is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
		s32Ret = HI_NET_DEV_Logout(*u32Handle);
		if (s32Ret != HI_SUCCESS)
		{
			LOGOUT("HI_NET_DEV_Logout is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);	
		}	
		*u32Handle = 0;
		return -3;
	}
	//#if 0
	s32Ret=HI_NET_DEV_SetDataCallBack(*u32Handle, OnDataCallback, &a);
	if(s32Ret != HI_SUCCESS)
	{
		LOGOUT("HI_NET_DEV_SetDataCallBack is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
		s32Ret = HI_NET_DEV_Logout(*u32Handle);
		if (s32Ret != HI_SUCCESS)
		{
			LOGOUT("HI_NET_DEV_Logout is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);	
		}	
		*u32Handle = 0;
		return -4;
	}
	//#endif
	struStreamInfo.u32Stream = u32Stream;
	struStreamInfo.u32Channel = HI_NET_DEV_CHANNEL_1;
	struStreamInfo.u32Mode = HI_NET_DEV_STREAM_MODE_TCP;
	struStreamInfo.u8Type = HI_NET_DEV_STREAM_VIDEO_AUDIO;
	s32Ret = HI_NET_DEV_StartStreamExt(*u32Handle, &struStreamInfo);
	if (s32Ret != HI_SUCCESS)
	{
		LOGOUT("HI_NET_DEV_StartStream is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
		iRet=ReleaseHiSDKServer(u32Handle);
		if(iRet!=0)
		{
			LOGOUT("InitHiSDKServer is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
			*u32Handle = 0;
			return -3;
		}
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
int setVideoParam(HI_U32 *u32Handle,HI_S_Video_Ext sVideo,HI_U32 u32Steam)
{
	HI_S32 s32Ret = HI_SUCCESS;
	int iRet=-1;
	
	s32Ret=HI_NET_DEV_SetConfig(*u32Handle,HI_NET_DEV_CMD_VIDEO_PARAM_EXT,&sVideo,sizeof(HI_S_Video_Ext));
	if (s32Ret != HI_SUCCESS)
	{
		LOGOUT("HI_NET_DEV_SetConfig is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
		iRet=ReleaseHiSDKServer(u32Handle);
		iRet=InitHiSDKServer(u32Handle,u32Steam);
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

int startVideoStream(HI_S_Video_Ext sVideo)
{
	HI_S32 s32Ret = HI_SUCCESS;
	int iRet=-1;
	HI_U32 a;
	HI_U32 *ptrU32Handle=NULL;

	if(sVideo.u32Width==DEVICEWIDTHBIG && sVideo.u32Height==DEVICEHIGHHBIG)
	{
		
		sVideo.u32Stream=0;
		sVideo.u32Width=DEVICEWIDTHBIG;
		sVideo.u32Height=DEVICEHIGHHBIG;
		u32ChannelFlag=1;
		ptrU32Handle=&u32HandleHight;
		
	}
	else if(sVideo.u32Width==DEVICEWIDTHMID && sVideo.u32Height==DEVICEHIGHHMID)
	{
		sVideo.u32Stream=1;
		u32ChannelFlag=2;
		ptrU32Handle=&u32HandleMid;
	}
	
	else if(sVideo.u32Width==DEVICEWIDTHSMALL && sVideo.u32Height==DEVICEHIGHHSMALL)
	{
		sVideo.u32Stream=2;
		u32ChannelFlag=3;
		ptrU32Handle=&u32HandleLow;
	}
	else
	{
		LOGOUT("set video invlid width=%d height=%d",sVideo.u32Width,sVideo.u32Height);
		return -1;
	}
	if(sVideo.u32ImgQuality<1 || sVideo.u32ImgQuality>6)
	{
		LOGOUT("set video invlid u32ImgQuality=%d",sVideo.u32ImgQuality);
		return -1;
	}
	if(sVideo.u32Bitrate==0)
	{
		LOGOUT("set video invlid u32Bitrate");
		return -1;
	}	
	if(sVideo.u32Frame==0)
	{
		LOGOUT("set video invlid u32Frame");
		return -1;
	}
	if(sVideo.u32Iframe==0)
	{
		LOGOUT("set video invlid u32Iframe");
		return -1;
	}	
	curVideoParam=sVideo;
	if(*ptrU32Handle == 0)
	{
		iRet=InitHiSDKServer(ptrU32Handle,sVideo.u32Stream);
		if(iRet!=0)
		{
			LOGOUT("InitHiSDKServer channle=%d is failure handle:%u,failcode:0x%x",u32ChannelFlag,*ptrU32Handle,iRet);
			ptrU32Handle = 0;
			return -2;
		}
		LOGOUT("InitHiSDKServer channle=%d over %u",u32ChannelFlag,*ptrU32Handle);
	}
	
	iRet=setVideoParam(ptrU32Handle,sVideo,sVideo.u32Stream);
	if(iRet!=0)
	{
		LOGOUT("setVideoParam channle=%d over %u",u32ChannelFlag,*ptrU32Handle);
		iRet=-3;
	}	
	iRet=MakeKeyFrame();
	if(iRet!=0)
	{	
		LOGOUT("MakeKeyFrame channle=%d over %u iRet=%d",u32ChannelFlag,*ptrU32Handle,iRet);
		iRet=-4;
	}
	return iRet;

}

int MakeKeyFrame()
{
	HI_U32 *ptrU32Handle=NULL;
	HI_S_Video_Ext sVideo;
	HI_U32 u32Channel=0;
	HI_S32 s32Ret = HI_SUCCESS;
	
	if( curVideoParam.u32Height==0)
	{
		return -1;
	}
	sVideo=curVideoParam;
	if(sVideo.u32Width==DEVICEWIDTHBIG && sVideo.u32Height==DEVICEHIGHHBIG)
	{
		ptrU32Handle=&u32HandleHight;
		u32Channel=11;
	}
	else if(sVideo.u32Width==DEVICEWIDTHMID && sVideo.u32Height==DEVICEHIGHHMID)
	{
		ptrU32Handle=&u32HandleMid;
		u32Channel=12;
	}
	
	else if(sVideo.u32Width==DEVICEWIDTHSMALL && sVideo.u32Height==DEVICEHIGHHSMALL)
	{
		ptrU32Handle=&u32HandleLow;
		u32Channel=13;
	}
	else
	{
		ptrU32Handle=&u32HandleMid;
		u32Channel=12;
	}
	s32Ret=HI_NET_DEV_MakeKeyFrame(*ptrU32Handle,u32Channel);
	if (s32Ret != HI_SUCCESS)
	{
		LOGOUT("HI_NET_DEV_MakeKeyFrame is failure handle:%u,failcode:0x%x",*ptrU32Handle,s32Ret);
		return -2;
	}
	else
	{
		return 0;
	}	
	
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

int InitHiSDKVideoAllChannel()
{
	HI_NET_DEV_Init();
	memset(&curVideoParam,0,sizeof(curVideoParam));
	int iRet=InitHiSDKServer(&u32HandleHight,0);
	if(iRet!=0)
		LOGOUT("InitHiSDKServer Hight is faliure,iRet=%d",iRet);
	iRet=InitHiSDKServer(&u32HandleMid,1);
	if(iRet!=0)
		LOGOUT("InitHiSDKServer Mid is faliure,iRet=%d",iRet);
	iRet=InitHiSDKServer(&u32HandleLow,2);
	if(iRet!=0)
		LOGOUT("InitHiSDKServer Low is faliure,iRet=%d",iRet);
	LOGOUT("hight handle 0x%x=%d,mid handle 0x%x=%d,low handle 0x%x=%d",&u32HandleHight,u32HandleHight,
			&u32HandleMid,u32HandleMid,&u32HandleLow,u32HandleLow);
	return iRet;
}  


int ReleaseHiSDKVideoAllChannel()
{
	int iRet=ReleaseHiSDKServer(&u32HandleHight);
	if(iRet!=0)
		LOGOUT("ReleaseHiSDKServer Hight is faliure,iRet=%d",iRet);
	iRet= ReleaseHiSDKServer(&u32HandleMid);
	if(iRet!=0)
		LOGOUT("ReleaseHiSDKServer Mid is faliure,iRet=%d",iRet);
	iRet=ReleaseHiSDKServer(&u32HandleLow);
	if(iRet!=0)
		LOGOUT(" ReleaseHiSDKServer Low is faliure,iRet=%d",iRet);
	HI_NET_DEV_DeInit();
	return iRet;

}

