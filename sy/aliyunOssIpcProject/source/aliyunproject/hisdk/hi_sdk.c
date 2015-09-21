#include <stdio.h>
#include <sys/syscall.h>//Linux system call for thread id
#include <assert.h>
#include <pthread.h>
#include "hi_sdk.h"
#include "../LogOut/LogOut.h"
#include "../Common/GlobFunc.h"
#include "../Common/Typedef.h"
#include "../mp4v2/joseph_g711a_h264_to_mp4.h"
#include "../Common/Configdef.h"
#include "../Common/Queue.h"


static HI_U32 				u32HandleHight=0;
static HI_U32 				u32HandleMid=0;
static HI_U32 				u32HandleLow=0;
static HI_U32 				u32ChannelFlag=1;  // 1 hight 2 mid 3 low
static HI_S_Video_Ext 		curVideoParam;
static JOSEPH_ACC_CONFIG 	joseph_aac_config;
static JOSEPH_MP4_CONFIG 	joseph_mp4_config;
static HI_U32  				u32RecordCmd=RECORDIDLE;
static HI_Motion_Data 		motionData;
static HI_U32 				u32SendMediaToAliyunStatus=0;

static	HI_U32	 			u32Width=DEVICEWIDTHBIG;	 /* 视频宽 */
static	HI_U32 				u32Height=DEVICEHIGHHBIG;	 /* 视频高 */
static  INT32S				s32Mp4FailFlag=0;

static Queue *	 			g_quene=NULL;



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
 
static void getTimeNameString(int timeStamp,char *timeStr,int size)
{
	struct tm * timeinfo;
	struct tm utc_tm;;
	timeinfo = gmtime_r( (time_t *)&timeStamp,&utc_tm );
	strftime(timeStr,size,("%Y%m%d%H%M%S"),timeinfo);
	return;
}

HI_S32 takePicture(HI_U32 u32Handle,char fileName[256])
{
	char *sData = (char*)malloc(1024*1024);
	if(sData==NULL)
	{
		LOGOUT("malloc is erreor take picture");
		return -1;
	}
	int nSize = 0;
	HI_S32 s32Ret = -1;
	s32Ret = HI_NET_DEV_SnapJpeg(u32Handle,(HI_U8*)sData, 1024*1024, &nSize);
	if(s32Ret==HI_SUCCESS)
	{
		FILE *fp = fopen(fileName, "wb+");
		if(fp==NULL)
		{
			LOGOUT("fail open %s,take picture error is %s",fileName,strerror(errno));
		}
		else
		{
			printf("write phote %s is %d\n",fileName,nSize);
			fwrite((const char*)sData, 1, nSize, fp);
			fclose( fp );
		}
	}
	else
	{
		LOGOUT("HI_NET_DEV_SnapJpeg %s,take picture error is %s",fileName,strerror(errno));
	}
	free(sData);
	sData = NULL;
	return s32Ret;
}


void * sendMediaToAliyunThread(void* param)
{	
	while(0)
	{
		usleep(100);
	}
	return NULL;
}

HI_S32 creatRecordData(RecordData *v_pRD,JOSEPH_MP4_CONFIG *v_pJOSEPH_MP4_CONFIG,Motion_Data v_mMotionData)
{
	if(v_pRD==NULL || v_pJOSEPH_MP4_CONFIG==NULL)
	{
		LOGOUT("param is error");
		return -1;
	}
	memset(v_pRD,0,sizeof(RecordData));
	strncpy(v_pRD->m_server,g_stConfigCfg.m_unMasterServerCfg.m_objMasterServerCfg.m_szMasterIP,sizeof(v_pRD->m_server));
	strncpy(v_pRD->m_id,g_szServerNO,sizeof(v_pRD->m_id));
	strncpy(v_pRD->m_secret,g_stConfigCfg.m_unDevInfoCfg.m_objDevInfoCfg.m_szPassword,sizeof(v_pRD->m_secret));
	strncpy(v_pRD->m_videoPath,v_pJOSEPH_MP4_CONFIG->nOssVideoName,sizeof(v_pRD->m_videoPath));
	strncpy(v_pRD->m_jpgPath,v_pJOSEPH_MP4_CONFIG->nOssJpgName,sizeof(v_pRD->m_jpgPath));
	v_pRD->m_creatTimeInMilSecond=(long long)v_pJOSEPH_MP4_CONFIG->m_startTime*1000-CHINATIME*1000;
	v_pRD->m_mMotionData=v_mMotionData;
	memcpy(&v_pRD->m_mMotionData,&v_mMotionData,sizeof(v_pRD->m_mMotionData));
	v_pRD->m_videoFileSize=getFileSize(v_pJOSEPH_MP4_CONFIG->nFifoEndName)/1024;
	v_pRD->m_videoTimeLength=v_pJOSEPH_MP4_CONFIG->m_overTime-v_pJOSEPH_MP4_CONFIG->m_startTime;
	LOGOUT("v_pRD->m_videoTimeLength=%d",v_pRD->m_videoTimeLength);
	return 0;
}

void * makeMp4Task(void* param)
{
	char *buf=malloc(512*1024);
	DWORD length=0;
	DWORD sym=0;
	DWORD id=0;
	int iRet=-1;
	if(buf==NULL)
	{
		LOGOUT("malloc is error");
		exit(0);
	}
	while(1)
	{
		//memset(buf,0,sizeof(buf));
		sym=0;
		int ret = g_quene->popData(g_quene->_this, (void*)buf,512*1024, &length, &sym, &id);
		if(ret==0)
		{
			switch(sym)
			{
			case RECORDSTART:
			{
				iRet=MakeKeyFrame(1);
				if(iRet!=0)
				{
					LOGOUT("make key frame");
				}
				memset(&joseph_aac_config,0,sizeof(joseph_aac_config));
				memset(&joseph_mp4_config,0,sizeof(joseph_mp4_config));

				HI_S_Video_Ext sVideo;
				int iRet=GetMasterVideoStream(&sVideo);
				if(iRet!=0)
				{
					LOGOUT("GetMasterVideoStream is failure");
					sVideo.u32Frame=12;
					sVideo.u32Width=u32Width;
					sVideo.u32Height=u32Height;
				}
				else
				{
					LOGOUT("sVideo.u32Frame=%d,sVideo.u32Width=%d,sVideo.u32Height=%d",
						   sVideo.u32Frame,sVideo.u32Width,sVideo.u32Height);
				}
				joseph_mp4_config.m_vFrameDur = 0;
				joseph_mp4_config.timeScale = 90000;	
				joseph_mp4_config.fps = sVideo.u32Frame; 			 
				joseph_mp4_config.width = sVideo.u32Width;			
				joseph_mp4_config.height = sVideo.u32Height;
				joseph_mp4_config.video = MP4_INVALID_TRACK_ID; 
				joseph_mp4_config.audio = MP4_INVALID_TRACK_ID;
				joseph_mp4_config.hFile = NULL;
				memset(joseph_mp4_config.nFifoName,0,sizeof(joseph_mp4_config.nFifoName));
				char timeString[128]={0};
				time_t localTime;
				time((time_t *)&localTime);
				localTime+=CHINATIME;
				joseph_mp4_config.m_startTime=localTime;
				getTimeNameString(localTime,timeString,128);
				char day[10]={0};
				memset(day,0,sizeof(day));
				memcpy(day,timeString,8);
				if(0!=isDeviceAccess(SYSTEM_MEDIA_SAVEFILEPATH))
				{
					mkdir(SYSTEM_MEDIA_SAVEFILEPATH,0777);
					LOGOUT("mkidr %s",SYSTEM_MEDIA_SAVEFILEPATH);
				}
				if(0!=isDeviceAccess(SYSTEM_MEDIA_SENDFILEPATH))
				{
					mkdir(SYSTEM_MEDIA_SENDFILEPATH,0777);
					LOGOUT("mkidr %s",SYSTEM_MEDIA_SENDFILEPATH);
				}
				sprintf(joseph_mp4_config.nFifoName,"%s/%s.mp4",SYSTEM_MEDIA_SAVEFILEPATH,timeString);	
				sprintf(joseph_mp4_config.nPictureName,"%s/%s.jpg",SYSTEM_MEDIA_SAVEFILEPATH,timeString);
				sprintf(joseph_mp4_config.nFifoEndName,"%s/%s.mp4",SYSTEM_MEDIA_SENDFILEPATH,timeString);	
				sprintf(joseph_mp4_config.nPictureEndName,"%s/%s.jpg",SYSTEM_MEDIA_SENDFILEPATH,timeString);
				sprintf(joseph_mp4_config.nDataEndName,"%s/%s.dat",SYSTEM_MEDIA_SENDFILEPATH,timeString);
				sprintf(joseph_mp4_config.nOssVideoName,"%s/%s/%s/%s.mp4",
						g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szVideoPath,
						g_szServerNO,day,timeString);
				sprintf(joseph_mp4_config.nOssJpgName,"%s/%s/%s/%s.jpg",
						g_stConfigCfg.m_unAliyunOssCfg.m_objAliyunOssCfg.m_szJPGPath,
						g_szServerNO,day,timeString);		
				iRet=takePicture(u32HandleHight,joseph_mp4_config.nPictureName);
				LOGOUT("takePiture %s is %d",joseph_mp4_config.nPictureName,iRet);
				iRet=InitMp4Module(&joseph_aac_config,&joseph_mp4_config);
				if(iRet!=0)
				{
					LOGOUT("InitMp4Module is failure %d",iRet);
				}
				else
				{
					
					LOGOUT("InitMp4Module is success %d",iRet);
				}

			}
				break;
			case RECORDVIDEO:
			{
				int iRet=Mp4FileVideoEncode(&joseph_aac_config,&joseph_mp4_config,(unsigned char*)buf,length);
				if(iRet<0)
					s32Mp4FailFlag=-1;
			}
				break;
			case RECORDAUDIO:
			{
				Mp4FileAudioEncode(&joseph_aac_config,&joseph_mp4_config,(unsigned char*)buf,length);
			}
				break;
			case RECORDDELETE:
			case RECORDSTOP:
			{
				iRet=CloseMp4Module(&joseph_aac_config,&joseph_mp4_config);
				if(iRet!=0)
				{
					LOGOUT("CloseMp4Module is failure %d",iRet);
				}
				if(sym==RECORDSTOP)
				{				
					rename(joseph_mp4_config.nFifoName,joseph_mp4_config.nFifoEndName);
					rename(joseph_mp4_config.nPictureName,joseph_mp4_config.nPictureEndName);
					LOGOUT("rename(%s)",joseph_mp4_config.nFifoEndName);
					LOGOUT("rename(%s)",joseph_mp4_config.nPictureEndName);
					RecordData mRecordData;
					iRet=creatRecordData(&mRecordData,&joseph_mp4_config,motionData.m_stSumMotionData);
					if(iRet==0)
					{
						iRet=writeFile(joseph_mp4_config.nDataEndName,(LPCTSTR)(&mRecordData),sizeof(mRecordData));
					}
				}
				else
				{
					unlink(joseph_mp4_config.nFifoName);
					unlink(joseph_mp4_config.nPictureName);
					LOGOUT("unlink(%s)",joseph_mp4_config.nFifoName);
					LOGOUT("unlink(%s)",joseph_mp4_config.nPictureName);
				}
				iRet=rmDirFile(SYSTEM_MEDIA_SAVEFILEPATH);
				if(iRet>0)
				{
					LOGOUT("rmdir %s iRet=%d error=%s",SYSTEM_MEDIA_SAVEFILEPATH,iRet,strerror(errno));
				}
				motionData.m_u32MotionStatus=0;
			}
				break;
			default:
				break;
			}
			//usleep(10*1000);
		}
		else
		{
			usleep(100*1000);
		}
	}
	free(buf);
}
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
		switch(*u32Status)
		{
		case RECORDSTART:
		{
			*u32Status=RECORDVIDEO;
			u32WaitIFrame=1;
			if(g_quene)
			{
				int test=0;
				int ret=g_quene->pushData(g_quene->_this,(void *)&test,sizeof(test),RECORDSTART,0);
				if(ret!=0)
				{
					LOGOUT("pushData is error");
				}
			}
		}
			break;
		case RECORDVIDEO:
		{
			if(u32WaitIFrame==1&&pstruAV->u32AVFrameFlag == HI_NET_DEV_VIDEO_FRAME_FLAG)
			{
				if(pstruAV->u32VFrameType==1)
				{
					u32LastStartTPS=pstruAV->u32AVFramePTS;
					//motionData.m_u32MotionStartTime=getTickCountMs();
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
						//Mp4FileVideoEncode(&joseph_aac_config,&joseph_mp4_config,(unsigned char*)(h264HeaderPtr[i]),h264HeaderPtr[i+1]-h264HeaderPtr[i]);
						int ret=g_quene->pushData(g_quene->_this,(void*)(h264HeaderPtr[i]),h264HeaderPtr[i+1]-h264HeaderPtr[i],RECORDVIDEO,0);
						if(ret!=0)
						{
							LOGOUT("pushData is error");
						}

					}
					if((j-1)>0 && h264HeaderPtr[j-1]!=NULL)
					{
						//Mp4FileVideoEncode(&joseph_aac_config,&joseph_mp4_config,(unsigned char*)(h264HeaderPtr[j-1]),u32Length-(h264HeaderPtr[j-1]-pu8Buffer));
						int ret=g_quene->pushData(g_quene->_this,(void*)(h264HeaderPtr[j-1]),u32Length-(h264HeaderPtr[j-1]-pu8Buffer),RECORDVIDEO,0);
						if(ret!=0)
						{
							LOGOUT("pushData is error");
						}
					}
				}
				else
				{	
					//Mp4FileVideoEncode(&joseph_aac_config,&joseph_mp4_config,(unsigned char*)(pu8Buffer+sizeof(HI_S_AVFrame)),u32Length-sizeof(HI_S_AVFrame));
					int ret=g_quene->pushData(g_quene->_this,(void*)(pu8Buffer+sizeof(HI_S_AVFrame)),u32Length-sizeof(HI_S_AVFrame),RECORDVIDEO,0);
					if(ret!=0)
					{
						LOGOUT("pushData is error");
					}
				}
			}
			else if(pstruAV->u32AVFrameFlag==HI_NET_DEV_AUDIO_FRAME_FLAG)
			{

				int uSize=(u32Length-sizeof(HI_S_AVFrame)-4);
				//Mp4FileAudioEncode(&joseph_aac_config,&joseph_mp4_config,(unsigned char*)(pu8Buffer+sizeof(HI_S_AVFrame)+4),u32Length-sizeof(HI_S_AVFrame)-4);
				int ret=g_quene->pushData(g_quene->_this,(void*)(pu8Buffer+sizeof(HI_S_AVFrame)+4),u32Length-sizeof(HI_S_AVFrame)-4,RECORDAUDIO,0);
				if(ret!=0)
				{
					LOGOUT("pushData is error");
				}
			}	
		}
			break;
		case RECORDDELETE:
		case RECORDSTOP:
		{
			if(g_quene)
			{
				int test=0;
				int ret=g_quene->pushData(g_quene->_this,(void *)&test,sizeof(test),*u32Status,0);
				if(ret!=0)
				{
					LOGOUT("pushData is error");
				}
			}
			*u32Status=RECORDIDLE;
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
	//printf("time=%d,u32Handle=%d,u32DataType=%d,pu8Buffer=%s,u32Length=%d,pUserData=%s\n",
	//	    getTickCountMs(),u32Handle,u32DataType,pu8Buffer,u32Length,pUserData);
	if(g_stConfigCfg.m_unMotionCfg.m_objMotionCfg.m_bEnable==0)
		return HI_SUCCESS;
	if(strlen(g_szServerNO)==0)
		return HI_SUCCESS;
	if(!(g_iServerStatus==1 || g_iServerStatus==4))
	{
		int iRet=isFileSystemBigger(SYSTEM_SD_SAVEFILEPATH,50*1024);
		if(iRet!=0)
		{
			LOGOUT("%s isFileSystemBigger sd",SYSTEM_SD_SAVEFILEPATH);
			return HI_SUCCESS;
		}
	}
	//printf("u32RecordCmd=%d\n",u32RecordCmd);
	switch(u32RecordCmd)
	{
	case RECORDIDLE:
	{
		if(motionData.m_u32MotionStatus==0 && u32DataType==0)
		{
			int iRet=isFileSystemBigger(SYSTEM_SD_SAVEFILEPATH,3*1024);
			if(iRet!=0)
			{	
				system("echo 3 >/proc/sys/vm/drop_caches");
				LOGOUT("echo 3 >/proc/sys/vm/drop_caches");
				iRet=isFileSystemBigger(SYSTEM_SD_SAVEFILEPATH,3*1024);
				if(iRet!=0)
					break;
				LOGOUT("getFreeMemory start record");
			}
			memset(&motionData,0,sizeof(motionData));
			printf("motionData.m_u32MotionStatus=0\n");
			motionData.m_u32MotionStartTime=getTickCountMs();
			motionData.m_u32MotionFirstTime=motionData.m_u32MotionStartTime;
			motionData.m_u32MotionStatus=1;
			motionData.m_u32MotionLastSecond=g_stConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iBefRecLastTime;
			motionData.m_u32MotionTimesIsValid=g_stConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iBefRecTimes;
			motionData.m_u32MotionEndTime=g_stConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iEndRecTime;
			u32RecordCmd=RECORDSTART;
		}
	}
		break;
	case RECORDVIDEO:
	{
		DWORD nowTime=getTickCountMs();
		DWORD endTime=0;
		if(motionData.m_u32MotionStatus==1)
		{
			nowTime=nowTime-motionData.m_u32MotionStartTime;
			endTime=motionData.m_u32MotionLastSecond*1000;
			if(nowTime<=endTime)
			{
				if(u32DataType==0)
				{
					HI_S_ALARM_MD *pMd=(HI_S_ALARM_MD *)pu8Buffer;
					//printf("pMd->u32Area is %d---\n",pMd->u32Area);
					switch(pMd->u32Area)
					{
						case 1:
							motionData.m_stMotionData.videoMotionTotal_Dist1++;
							break;
						case 2:
							motionData.m_stMotionData.videoMotionTotal_Dist2++;
							break;
						case 3:
							motionData.m_stMotionData.videoMotionTotal_Dist3++;
							break;
						case 4:
							motionData.m_stMotionData.videoMotionTotal_Dist4++;
							break;	
						default:
							break;
					}
				}
			}
			else
			{
				HI_U32 motionCount=0;
				motionData.m_stSumMotionData.videoMotionTotal_Dist1+=motionData.m_stMotionData.videoMotionTotal_Dist1;
				motionData.m_stSumMotionData.videoMotionTotal_Dist2+=motionData.m_stMotionData.videoMotionTotal_Dist2;
				motionData.m_stSumMotionData.videoMotionTotal_Dist3+=motionData.m_stMotionData.videoMotionTotal_Dist3;
				motionData.m_stSumMotionData.videoMotionTotal_Dist4+=motionData.m_stMotionData.videoMotionTotal_Dist4;
				motionCount=motionData.m_stMotionData.videoMotionTotal_Dist1+motionData.m_stMotionData.videoMotionTotal_Dist2+
							motionData.m_stMotionData.videoMotionTotal_Dist3+motionData.m_stMotionData.videoMotionTotal_Dist4;
				LOGOUT("before:motionCount is %d",motionCount);
				if(motionCount>=motionData.m_u32MotionTimesIsValid)
				{
					motionData.m_u32MotionStatus=2;
					motionData.m_u32MotionStartTime=getTickCountMs();
					motionData.m_u32MotionLastSecond=g_stConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iConRecLastTime;
					motionData.m_u32MotionTimesIsValid=g_stConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iConRecTimes;
					motionData.m_u32MotionEndTime=g_stConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iEndRecTime;
					memset(&motionData.m_stMotionData,0,sizeof(motionData.m_stMotionData));
				}
				else
				{
					u32RecordCmd=RECORDDELETE;
				}
			}
		}
		else if(motionData.m_u32MotionStatus==2)
		{
			nowTime=nowTime-motionData.m_u32MotionStartTime;
			endTime=motionData.m_u32MotionLastSecond*1000;
			if(nowTime<=endTime)
			{
				if(u32DataType==0)
				{
					HI_S_ALARM_MD *pMd=(HI_S_ALARM_MD *)pu8Buffer;
					//printf("pMd->u32Area is %d---\n",pMd->u32Area);
					switch(pMd->u32Area)
					{
						case 1:
							motionData.m_stMotionData.videoMotionTotal_Dist1++;
							break;
						case 2:
							motionData.m_stMotionData.videoMotionTotal_Dist2++;
							break;
						case 3:
							motionData.m_stMotionData.videoMotionTotal_Dist3++;
							break;
						case 4:
							motionData.m_stMotionData.videoMotionTotal_Dist4++;
							break;	
						default:
							break;
					}
				}
			}
			else
			{
				HI_U32 motionCount=0;
				motionData.m_stSumMotionData.videoMotionTotal_Dist1+=motionData.m_stMotionData.videoMotionTotal_Dist1;
				motionData.m_stSumMotionData.videoMotionTotal_Dist2+=motionData.m_stMotionData.videoMotionTotal_Dist2;
				motionData.m_stSumMotionData.videoMotionTotal_Dist3+=motionData.m_stMotionData.videoMotionTotal_Dist3;
				motionData.m_stSumMotionData.videoMotionTotal_Dist4+=motionData.m_stMotionData.videoMotionTotal_Dist4;
				motionCount=motionData.m_stMotionData.videoMotionTotal_Dist1+motionData.m_stMotionData.videoMotionTotal_Dist2+
							motionData.m_stMotionData.videoMotionTotal_Dist3+motionData.m_stMotionData.videoMotionTotal_Dist4;
				LOGOUT("continue:motionCount is %d",motionCount);
				if(motionCount>=motionData.m_u32MotionTimesIsValid)
				{
					motionData.m_u32MotionStatus=2;
					motionData.m_u32MotionStartTime=getTickCountMs();
					motionData.m_u32MotionLastSecond=g_stConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iConRecLastTime;
					motionData.m_u32MotionTimesIsValid=g_stConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iConRecTimes;
					motionData.m_u32MotionEndTime=g_stConfigCfg.m_unMotionCfg.m_objMotionCfg.m_iEndRecTime;
					memset(&motionData.m_stMotionData,0,sizeof(motionData.m_stMotionData));
				}
				else
				{
					time_t localTime;
					time((time_t *)&localTime);
					localTime+=CHINATIME;
					joseph_mp4_config.m_overTime=localTime;
					u32RecordCmd=RECORDSTOP;
				}
			}
		}
		nowTime=getTickCountMs();
		endTime=0;
		nowTime=nowTime-motionData.m_u32MotionFirstTime;
		endTime=motionData.m_u32MotionEndTime*1000;
		if(nowTime>endTime)
		{
			time_t localTime;
			time((time_t *)&localTime);
			localTime+=CHINATIME;
			joseph_mp4_config.m_overTime=localTime-1;
			if(motionData.m_u32MotionStatus!=1)
				u32RecordCmd=RECORDSTOP;
			else
				u32RecordCmd=RECORDDELETE;
			break;
		}
		#if 0
		INT32U mem=getFreeMemory();
		if(mem<3*1024*1024)
		{	
			system("echo 3 >/proc/sys/vm/drop_caches");
			LOGOUT("echo 3 >/proc/sys/vm/drop_caches");
			INT32U mem=getFreeMemory();
			if(mem>3*1024*1024)
				break;
			time_t localTime;
			time((time_t *)&localTime);
			localTime+=CHINATIME;
			joseph_mp4_config.m_overTime=localTime;
			u32RecordCmd=RECORDSTOP;
			LOGOUT("getFreeMemory()=%d stop record",mem);
		}
		#endif
		int iRet=isFileSystemBigger(SYSTEM_SD_SAVEFILEPATH,1*1024);
		if(iRet!=0)
		{	
			system("echo 3 >/proc/sys/vm/drop_caches");
			LOGOUT("echo 3 >/proc/sys/vm/drop_caches");
			iRet=isFileSystemBigger(SYSTEM_SD_SAVEFILEPATH,1*1024);
			if(iRet==0)
				break;
			time_t localTime;
			time((time_t *)&localTime);
			localTime+=CHINATIME;
			joseph_mp4_config.m_overTime=localTime;
			if(motionData.m_u32MotionStatus!=1)
				u32RecordCmd=RECORDSTOP;
			else
				u32RecordCmd=RECORDDELETE;
			break;
			LOGOUT("getFreeMemory small then 1024K stop record Record cmd %d",u32RecordCmd);
		}
		if(s32Mp4FailFlag<0)
		{
			s32Mp4FailFlag=0;
			u32RecordCmd=RECORDDELETE;
			LOGOUT("make mp4 file is error");
		}
	}
		break;	
	default:
		break;
	}
	
	return HI_SUCCESS;
}

int InitHiSDKServer(HI_U32 *u32Handle,HI_U32 u32Stream)
{
    HI_S32 s32Ret = HI_SUCCESS;
	HI_U32 a;
	HI_S_STREAM_INFO_EXT struStreamInfo;
	
	char uname[64]={0,};
	char password[64]={0,};
	memset(&motionData,0,sizeof(motionData));
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
	//iRet=0;
	iRet=getIPAndPort(NETFILE,ip,port);
	#else
	iRet=getIPAndPort(NETFILE,ip,port);
	#endif
	if(iRet!=0)
	{
		memset(ip,0,sizeof(ip));
		memcpy(ip,IPHOST,MIN(sizeof(ip),strlen(IPHOST)));
		memset(port,0,sizeof(port));
		memcpy(port,IPPORT,MIN(sizeof(port),strlen(IPPORT)));
	}
	else
	{
		memset(ip,0,sizeof(ip));
		memcpy(ip,IPHOST,MIN(sizeof(ip),strlen(IPHOST)));
	}
	LOGOUT("iRet=%d ip %s port %s",iRet,ip,port);
	if(strlen(ip)==0)
	{
		memset(ip,0,sizeof(ip));
		memcpy(ip,IPHOST,MIN(sizeof(ip),strlen(IPHOST)));
	}
	if(strlen(port)==0)
	{
		memset(port,0,sizeof(port));
		memcpy(port,IPPORT,MIN(sizeof(port),strlen(IPPORT)));
	}
	
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

int SetMasterVideoStream(HI_S_Video_Ext *sVideo)
{	
	HI_S32 s32Ret = HI_SUCCESS;
	int iRet=-1;
	HI_U32 *u32Handle=&u32HandleHight;
	if(*u32Handle==0)
	{
		LOGOUT("GetVideoStream is failure handle:%u",*u32Handle);
		return -1;
	}
	LOGOUT("width=%d,heigth=%d",sVideo->u32Width,sVideo->u32Height);
	s32Ret=HI_NET_DEV_SetConfig(*u32Handle,HI_NET_DEV_CMD_VIDEO_PARAM_EXT,sVideo,sizeof(HI_S_Video_Ext));
	if (s32Ret != HI_SUCCESS)
	{
		LOGOUT("HI_NET_DEV_SetConfig is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
		return -3;
	} 
#if 0
	HI_NET_DEV_RESOLUTION_VGA 0 VGA：640x480
	HI_NET_DEV_RESOLUTION_QVGA 1 QVGA：320x240
	HI_NET_DEV_RESOLUTION_QQVGA 2 QQVGA：160x120，160x112
	HI_NET_DEV_RESOLUTION_D1 3 D1：704x576，704x480
	HI_NET_DEV_RESOLUTION_CIF 4 CIF：352x288，352x240
	HI_NET_DEV_RESOLUTION_QCIF 5 QCIF：176x144，176x120，176x112
	HI_NET_DEV_RESOLUTION_720P 6 720P：1280x720
	HI_NET_DEV_RESOLUTION_Q720 7 Q720：640x352
	HI_NET_DEV_RESOLUTION_ QQ72 8 QQ720：320x176
	HI_NET_DEV_RESOLUTION_ UXGA 9 UXGA：1600x1200
	HI_NET_DEV_RESOLUTION_ 960H 10 960H：960x576
	HI_NET_DEV_RESOLUTION_ Q960H 11 Q960H：480x288
	HI_NET_DEV_RESOLUTION_ QQ960H 12 QQ960H：240x144
	HI_NET_DEV_RESOLUTION_ 1080P 13 1080P：1920x1080
	HI_NET_DEV_RESOLUTION_ 960P 14 960P：1280x960
#endif
	HI_S_Resolution sResolution;
	sResolution.blFlag=HI_TRUE;
	sResolution.u32Channel = HI_NET_DEV_CHANNEL_1;
	switch(sVideo->u32Height)
	{
		case 960:
			sResolution.u32Resolution=HI_NET_DEV_RESOLUTION_960P;
			break;
		case 720:
			sResolution.u32Resolution=HI_NET_DEV_RESOLUTION_720P;
			break;
		//case 480:
		//	sResolution.u32Resolution=HI_NET_DEV_RESOLUTION_VGA;
		//	break;
		//case 352:
		//	sResolution.u32Resolution=HI_NET_DEV_RESOLUTION_Q720;
		//	break;
		default:
			sResolution.u32Resolution=HI_NET_DEV_RESOLUTION_720P;
			break;
	}
	LOGOUT("sResolution.u32Resolution=%d",sResolution.u32Resolution);
	s32Ret=	HI_NET_DEV_SetConfig(*u32Handle,HI_NET_DEV_CMD_RESOLUTION,&sResolution,sizeof(HI_S_Resolution));
	if (s32Ret != HI_SUCCESS)
	{
		LOGOUT("HI_NET_DEV_SetConfig is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
		return -4;
	}
	return 0;

}

int GetMasterVideoStream(HI_S_Video_Ext *sVideo)
{	
	HI_S32 s32Ret = HI_SUCCESS;
	int iRet=-1;
	HI_U32 *u32Handle=&u32HandleHight;
	if(*u32Handle==0)
	{
		LOGOUT("GetVideoStream is failure handle:%u",*u32Handle);
		return -1;
	}
	sVideo->u32Stream=HI_FALSE;
	sVideo->u32Channel=HI_NET_DEV_CHANNEL_1;
	s32Ret=HI_NET_DEV_GetConfig(*u32Handle,HI_NET_DEV_CMD_VIDEO_PARAM_EXT,sVideo,sizeof(HI_S_Video_Ext));
	if (s32Ret != HI_SUCCESS)
	{
		LOGOUT("HI_NET_DEV_GetConfig is failure handle:%u,failcode:0x%x",*u32Handle,s32Ret);
		return -3;
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

int MakeKeyFrame(int channel)
{
	HI_U32 *ptrU32Handle=NULL;
	HI_U32 u32Channel=0;
	HI_S32 s32Ret = HI_SUCCESS;
	
	if(channel==1)
	{
		ptrU32Handle=&u32HandleHight;
		u32Channel=11;
	}
	else if(channel==2)
	{
		ptrU32Handle=&u32HandleMid;
		u32Channel=12;
	}
	
	else if(channel==3)
	{
		ptrU32Handle=&u32HandleLow;
		u32Channel=13;
	}
	else
	{
		ptrU32Handle=&u32HandleHight;
		u32Channel=11;
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
	{
		LOGOUT("InitHiSDKServer Hight is faliure,iRet=%d",iRet);
		return -1;
	}
	g_quene = (Queue*)QueueListConstruction();
	if(g_quene)
	{	
		iRet=g_quene->initQueue(g_quene->_this,3*1024*1024);
		if(iRet!=0)
		{
			LOGOUT("InitNetwork initqueue error\n");
		}
	}
	else
	{
		LOGOUT("InitNetwork QueueListConstruction error\n");
		return -3;
	}	
	pthread_t m_makeMp4Task;//实时播放，过程控制线程
	iRet = pthread_create(&m_makeMp4Task, NULL, makeMp4Task, NULL);
	if(iRet != 0)
	{
		LOGOUT("can't create thread: %s",strerror(iRet));
		return -2;
	}

	#if 0 //zss++
	HI_S_Video_Ext sVideo;
	iRet=GetMasterVideoStream(&sVideo);
	sVideo.u32Bitrate=g_stConfigCfg.m_unCapParamCfg.m_objCapParamCfg[0].m_wBitRate;
	sVideo.u32Frame=g_stConfigCfg.m_unCapParamCfg.m_objCapParamCfg[0].m_wFrameRate;
	sVideo.u32Height=g_stConfigCfg.m_unCapParamCfg.m_objCapParamCfg[0].m_wHeight;
	sVideo.u32Width=g_stConfigCfg.m_unCapParamCfg.m_objCapParamCfg[0].m_wWidth;
	sVideo.u32ImgQuality=g_stConfigCfg.m_unCapParamCfg.m_objCapParamCfg[0].m_wQpConstant;
	sVideo.u32Iframe=g_stConfigCfg.m_unCapParamCfg.m_objCapParamCfg[0].m_wKeyFrameRate;
	sVideo.blCbr=g_stConfigCfg.m_unCapParamCfg.m_objCapParamCfg[0].m_CodeType;
	SetMasterVideoStream(&sVideo);
	#endif
	
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
	
	pthread_t m_sendMediaToAliyunThread;//实时播放，过程控制线程
	#if 0
	g_videoQuene=InitVideoQuene(VIDEOBUFFERSIZE);
	if(g_videoQuene==NULL)
	{
		LOGOUT("InitVideoQuene %d failure",VIDEOBUFFERSIZE);
	}
	#endif
	iRet = pthread_create(&m_sendMediaToAliyunThread, NULL, sendMediaToAliyunThread, NULL);
	if(iRet != 0)
	{
		LOGOUT("can't create thread: %s",strerror(iRet));
		return -4;
	}
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
	if(g_quene)
	{
		g_quene->release(g_quene->_this);
	}
	if(g_quene)
	{
		free(g_quene);
		g_quene = NULL;
	}
	return iRet;

}

