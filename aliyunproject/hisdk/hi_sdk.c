#include <stdio.h>
#include <sys/syscall.h>//Linux system call for thread id
#include <assert.h>
#include <pthread.h>
#include "hi_sdk.h"
#include "../LogOut/LogOut.h"
#include "../Common/Typedef.h"
#include "../mp4v2/joseph_g711a_h264_to_mp4.h"

#if 0
#include "../Common/InnerDataMng.h"

Queue * g_videoQuene=NULL;

Queue * InitVideoQuene(int v_iCacheSize)
{
	int iRet=0;
	Queue *localQueue=NULL;
	localQueue = (Queue*)QueueListConstruction();
	if(localQueue)
	{	
		iRet=localQueue->initqueue(localQueue->_this,v_iCacheSize);
		if(iRet!=0)
		{
			LOGOUT("InitVideoQuene error\n");
			return NULL;
		}
		return localQueue;
	}
	else
	{
		LOGOUT("InitVideoQuene QueueListConstruction error\n");
		return NULL;
	}
}


int ReleaseVideoQuene(Queue *vQueue)
{
	int iRet=0;

	if(vQueue==NULL)
		return	-1;
	vQueue->release(vQueue->_this);
	free(vQueue);
	return 0;
}
#endif

#ifndef DEBUG_CPU_X86

//extern PRTMPUnit 	pRtmpServer;
static HI_U32 		u32HandleHight=0;
static HI_U32 		u32HandleMid=0;
static HI_U32 		u32HandleLow=0;
static HI_U32 		u32ChannelFlag=1;  // 1 hight 2 mid 3 low
static HI_S_Video_Ext curVideoParam;
static JOSEPH_ACC_CONFIG joseph_aac_config;
static JOSEPH_MP4_CONFIG joseph_mp4_config;
static HI_U32  		u32RecordCmd=RECORDSTART;

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
	//printf("u32Handle=%d,u32Event=%d,pUserData=%s\n",u32Handle,u32Event,pUserData);
	return HI_SUCCESS;
}

static void SaveRecordFile(HI_CHAR* pPath, HI_U8* pu8Buffer, HI_U32 u32Length)
{
	FILE* fp;
	fp = fopen(pPath, "ab+");
	fwrite(pu8Buffer, 1, u32Length, fp);
	fclose(fp);
}

#ifdef DEBUG_G711_FILE
char   g711aFileBuffer[1024*1024]={0};
#endif
 
static	HI_U32 u32Width=DEVICEWIDTHBIG;	 /* 视频宽 */
static	HI_U32 u32Height=DEVICEHIGHHBIG;	 /* 视频高 */

HI_S32 onRecordTask(HI_U32 u32Handle, /* 句柄 */
                                HI_U32 u32DataType,     /* 数据类型，视频或音频数据或音视频复合数据 */
                                HI_U8*  pu8Buffer,      /* 数据包含帧头 */
                                HI_U32 u32Length,      /* 数据长度 */
                                HI_VOID* pUserData,    /* 用户数据*/
                                HI_U32 *u32Status)
{
	HI_S32 iRet=0;
	HI_U32 validU32Handle=0;
	static HI_U32 u32WaitIFrame=0;
	static HI_U32 u32FileTime=5*1000;
	static HI_U32 u32LastStartTPS=0;
	HI_U32 i=0;
	static HI_U8  g711Buffer[1000]={0};
	static HI_U32 g711BufferSize=0;
	static HI_U32 fileCount=0;
	static HI_U32 fileSum=0;
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
		HI_S_AVFrame* pstruAV  = (HI_S_AVFrame*)pu8Buffer;
		//if (pstruAV->u32AVFrameFlag == HI_NET_DEV_VIDEO_FRAME_FLAG)
		//{
			//printf("child thread lwpid=%u\n",syscall(SYS_gettid));
			//printf("child thread tid=%u\n",pthread_self());
			//printf("Video %u PTS: %u \n", pstruAV->u32VFrameType, pstruAV->u32AVFramePTS);
			//SaveRecordFile("Video.hx", pu8Buffer, u32Length);
		//}
		//else if (pstruAV->u32AVFrameFlag == HI_NET_DEV_AUDIO_FRAME_FLAG)
		//{
			//printf("Audio %u PTS: %u \n", pstruAV->u32AVFrameLen, pstruAV->u32AVFramePTS);
			//SaveRecordFile("Video.hx", pu8Buffer, u32Length);			
		//}
		switch(*u32Status)
		{
		case RECORDSTART:
		{
			u32WaitIFrame=1;
			memset(&joseph_aac_config,0,sizeof(joseph_aac_config));
			memset(&joseph_mp4_config,0,sizeof(joseph_mp4_config));

			joseph_mp4_config.m_vFrameDur = 0;
			joseph_mp4_config.timeScale = 90000;	
			joseph_mp4_config.fps = 25; 			 
			joseph_mp4_config.width = u32Width;			
			joseph_mp4_config.height = u32Height;
			joseph_mp4_config.video = MP4_INVALID_TRACK_ID; 
			joseph_mp4_config.audio = MP4_INVALID_TRACK_ID;
			joseph_mp4_config.hFile = NULL;
			iRet=InitMp4Module(&joseph_aac_config,&joseph_mp4_config);
			if(iRet!=0)
			{
				LOGOUT("InitMp4Module is failure %d",iRet);
			}
			else
			{
				*u32Status=RECORDVIDEO;
				LOGOUT("InitMp4Module is success %d",iRet);
			}
			#ifdef DEBUG_G711_FILE
			FILE *pG711a= fopen(JOSEPH_G711A_LOCATION, "rb");
			fileSum = fread(g711aFileBuffer, 1, sizeof(g711aFileBuffer),pG711a);
			fclose(pG711a);
			#endif
		}
			break;
		case RECORDVIDEO:
		{
			if(u32WaitIFrame==1&&pstruAV->u32AVFrameFlag == HI_NET_DEV_VIDEO_FRAME_FLAG)
			{
				if(pstruAV->u32VFrameType==1)
				{
					u32LastStartTPS=pstruAV->u32AVFramePTS;
					u32WaitIFrame=0;
				}
				else
					break;
			}
			if(u32WaitIFrame==1)
				break;
			if(pstruAV->u32AVFrameFlag==HI_NET_DEV_VIDEO_FRAME_FLAG)
			{
				//SaveRecordFile("Video.hx", (unsigned char*)(pu8Buffer+sizeof(HI_S_AVFrame)),u32Length-sizeof(HI_S_AVFrame));
				if(pstruAV->u32VFrameType==1)
				{
					HI_U8  *h264HeaderPtr[10];
					HI_U32 j=0;
					for(i=0;i<10;i++)
					{
						h264HeaderPtr[i]=NULL;
					}
					for(i=0;((i+4)<100) && ((i+4)<u32Length);i++)
					{
						if(pu8Buffer[i]==0x00)
						{
							if(pu8Buffer[i+1]==0x00 && pu8Buffer[i+2]==0x00 && pu8Buffer[i+3]==0x01)
							{
								if(j<10)
								{
									h264HeaderPtr[j]=&pu8Buffer[i];
									j++;i=i+3;
								}
							}
						}
					}
					for(i=0;i<(j-1) && h264HeaderPtr[i]!=NULL;i++)
					{
						Mp4FileVideoEncode(&joseph_aac_config,&joseph_mp4_config,(unsigned char*)(h264HeaderPtr[i]),h264HeaderPtr[i+1]-h264HeaderPtr[i]);
					}
					if((j-1)>0 && h264HeaderPtr[j-1]!=NULL)
					{
						Mp4FileVideoEncode(&joseph_aac_config,&joseph_mp4_config,(unsigned char*)(h264HeaderPtr[j-1]),u32Length-(h264HeaderPtr[j-1]-pu8Buffer));
					}
				}
				else
				{	
					Mp4FileVideoEncode(&joseph_aac_config,&joseph_mp4_config,(unsigned char*)(pu8Buffer+sizeof(HI_S_AVFrame)),u32Length-sizeof(HI_S_AVFrame));
				}
				#ifdef DEBUG_G711_FILE
				Mp4FileAudioEncode(&joseph_aac_config,&joseph_mp4_config,(unsigned char*)(g711aFileBuffer+fileCount+4),160);
				fileCount+=164;
				printf("fileCount %d --\n",fileCount);
				#endif
			}
			else if(pstruAV->u32AVFrameFlag==HI_NET_DEV_AUDIO_FRAME_FLAG)
			{

				int uSize=(u32Length-sizeof(HI_S_AVFrame)-4);
				printf("Audio %d %d--\n",pstruAV->u32AVFrameLen,uSize);
				Mp4FileAudioEncode(&joseph_aac_config,&joseph_mp4_config,(unsigned char*)(pu8Buffer+sizeof(HI_S_AVFrame)+4),u32Length-sizeof(HI_S_AVFrame)-4);
				if((pstruAV->u32AVFramePTS-u32LastStartTPS)>u32FileTime)
					*u32Status=RECORDSTOP;
			}	
		}
			break;
		case RECORDSTOP:
		{
			iRet=CloseMp4Module(&joseph_aac_config,&joseph_mp4_config);
			if(iRet!=0)
			{
				LOGOUT("CloseMp4Module is failure %d",iRet);
			}
			else
			{
				*u32Status=RECORDIDLE;
				LOGOUT("CloseMp4Module is success %d",iRet);
			}
			
		}
			break;
		default:
			break;
		}
	}
	return HI_SUCCESS;
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
		onRecordTask(u32Handle,u32DataType,pu8Buffer,u32Length,pUserData,&u32RecordCmd);
	}
	else if (u32DataType == HI_NET_DEV_SYS_DATA)
	{
		pstruSys = (HI_S_SysHeader*)pu8Buffer;
		u32Width=pstruSys->struVHeader.u32Width;
		u32Height=pstruSys->struVHeader.u32Height;
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
	//printf("u32Handle=%d,u32DataType=%d,pu8Buffer=%s,u32Length=%d,pUserData=%s\n",u32Handle,
	//	   u32DataType,pu8Buffer,u32Length,pUserData);
	return HI_SUCCESS;
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
	#ifdef CPU_ARM
	iRet=1;
	#else
	iRet=getIPAndPort(NETFILE,ip,port);
	#endif
	if(iRet!=0)
	{
		memcpy(ip,IPHOST,MIN(sizeof(ip),strlen(IPHOST)));
		memcpy(port,IPPORT,MIN(sizeof(port),strlen(IPPORT)));
	}
	LOGOUT("iRet=%d ip %s port %s",iRet,ip,port);
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
	if(u32Stream==0)
	{
		LOGOUT("HI_NET_DEV_SetDataCallBack is stream");
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
	}
	//#endif
	struStreamInfo.u32Stream = u32Stream;
	struStreamInfo.u32Channel = HI_NET_DEV_CHANNEL_1;
	struStreamInfo.u32Mode = HI_NET_DEV_STREAM_MODE_TCP;
	struStreamInfo.u8Type = HI_NET_DEV_STREAM_ALL;
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
	int iRet=-1;
	iRet=InitHiSDKServer(&u32HandleHight,0);
	if(iRet!=0)
		LOGOUT("InitHiSDKServer Hight is faliure,iRet=%d",iRet);
	#if 0
	iRet=InitHiSDKServer(&u32HandleMid,1);
	if(iRet!=0)
		LOGOUT("InitHiSDKServer Mid is faliure,iRet=%d",iRet);
	iRet=InitHiSDKServer(&u32HandleLow,2);
	if(iRet!=0)
		LOGOUT("InitHiSDKServer Low is faliure,iRet=%d",iRet);
	#endif
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

#else

int get_g711a_frame(JOSEPH_MP4_CONFIG* joseph_mp4_config, unsigned char *nVideoBuffer, int nVideoRet)
{		
	int nVideoSize = 0;
	char nVideoNum[32] = {0};
	memset(nVideoNum, 0, 32);
	sprintf(nVideoNum, "%s%d", JOSEPH_H264_LOCALTION, nVideoRet);
	joseph_mp4_config->fpInVideo = fopen(nVideoNum, "rb");
	if(joseph_mp4_config->fpInVideo==NULL)
	{
		return -1;
	}
	memset(nVideoBuffer, 0, nVideoRet);
	nVideoSize = fread(nVideoBuffer, 1, 512*1024, joseph_mp4_config->fpInVideo);

	fclose(joseph_mp4_config->fpInVideo);
	
	return nVideoSize;
}


/*==================================================================
//函数： playThread
//功能： 实时播放视频
//参数： LPVOID data，将对象传入线程
//返回：无
==================================================================*/
void * playThread(void* param)
{
	int cmd=0;
	int iRet=0;
	int i=0;
	JOSEPH_ACC_CONFIG joseph_aac_config;
	JOSEPH_MP4_CONFIG joseph_mp4_config;
	unsigned char *nBuffer=malloc(1024*512);
	if(nBuffer==NULL)
	{
		LOGOUT("nBuffer is NULL");
		return NULL;
	}
	cmd=RECORDSTART;	
	while(1)
	{
		switch(cmd)
		{
		case RECORDIDLE:
			{
				usleep(100*1000);
			}
			break;
		case RECORDSTART:
			{
				memset(&joseph_aac_config,0,sizeof(joseph_aac_config));
				memset(&joseph_mp4_config,0,sizeof(joseph_mp4_config));
				joseph_aac_config.nSampleRate = 8000;
				joseph_aac_config.nChannels = 1;
				joseph_aac_config.nPCMBitSize = 16;
				strcpy(joseph_aac_config.nFifoName,"test.mp4");
				joseph_aac_config.fpIn=joseph_aac_config.nFifoName;

				
				joseph_mp4_config.m_vFrameDur = 0;
				joseph_mp4_config.timeScale = 90000;	
				joseph_mp4_config.fps = 25; 			 
				joseph_mp4_config.width = 640;			
				joseph_mp4_config.height = 480;
				joseph_mp4_config.video = MP4_INVALID_TRACK_ID;	
				joseph_mp4_config.audio = MP4_INVALID_TRACK_ID;
				joseph_mp4_config.hFile = NULL;
				joseph_mp4_config.hFile = MP4Create(JOSEPH_MP4_FILE, 0);
				if(joseph_mp4_config.hFile == MP4_INVALID_FILE_HANDLE)
				{
			        printf("open file fialed.\n");
			        return NULL;
			    }	
				iRet=InitMp4Module(&joseph_aac_config,&joseph_mp4_config);
				if(iRet!=0)
				{
					LOGOUT("InitMp4Module is failure %d",iRet);
				}
				else
				{
					cmd=RECORDVIDEO;
				}
			}
			break;
		case RECORDVIDEO:
			{
				iRet=get_g711a_frame(&joseph_mp4_config,nBuffer,i++);
				if(iRet>0)
				{
					printf("=====%d=====\m",i);
					Mp4FileEncode(&joseph_aac_config,&joseph_mp4_config,RECORDVIDEO,nBuffer,iRet);
				}
				else
				{
			        cmd=RECORDSTOP;
					LOGOUT("get_g711a_frame is error %d",iRet);
				}
				usleep(10*1000);
			}
			break;
		case RECORDAUDIO:
			break;
		case RECORDSTOP:
			{
				iRet=CloseMp4Module(&joseph_aac_config,&joseph_mp4_config);
				if(iRet!=0)
				{
					LOGOUT("CloseMp4Module is failure %d",iRet);
				}
				else
				{
					cmd=RECORDIDLE;
				}

			}
			break;
		default:
			break;
		}
		
	}
	free(nBuffer);
	return NULL;
}


int InitHiSDKVideoAllChannel()
{
	int iRet;
	pthread_t m_playThread;//实时播放，过程控制线程
	#if 0
	g_videoQuene=InitVideoQuene(VIDEOBUFFERSIZE);
	if(g_videoQuene==NULL)
	{
		LOGOUT("InitVideoQuene %d failure",VIDEOBUFFERSIZE);
	}
	#endif
	iRet = pthread_create(&m_playThread, NULL, playThread, NULL);
	if(iRet != 0)
	{
		LOGOUT("can't create thread: %s",strerror(iRet));
		return -1;
	}
	return 0;

}


int ReleaseHiSDKVideoAllChannel()
{

}

int startVideoStream(HI_S_Video_Ext sVideo)
{
	return 0;
}

int MakeKeyFrame()
{
	return 0;
}

#endif

