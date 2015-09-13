#ifndef __HI_SDK_H_
#define  __HI_SDK_H_

#ifndef BUG_CPU_X86

#include <stdio.h>
#include <unistd.h>
#include "hi_type.h"
#include "hi_net_dev_sdk.h"
#include "hi_net_dev_errors.h"
#include "hi_param.h"
#include "../Common/Typedef.h"
#include "../Common/GlobVariable.h"
#include "../controlServer/ConServer.h"


#define		DEVICEWIDTHBIG						1280
#define		DEVICEHIGHHBIG						960

#define		DEVICEWIDTHMID						640
#define		DEVICEHIGHHMID						352

#define		DEVICEWIDTHSMALL					320
#define		DEVICEHIGHHSMALL					176
#define 	CHINATIME 							8*3600

#define     CONNECTTIMEOUT						1000

#define 	VIDEOBUFFERSIZE						1024*1024*4

#ifndef DEBUG_CPU_X86
#define     SYSTEM_MEDIA_SAVEFILEPATH			"/mnt/mtd/ipc/tmpfs/sd/mediaSave"
#define     SYSTEM_MEDIA_SENDFILEPATH			"/mnt/mtd/ipc/tmpfs/sd/mediaSend"
#else
#define     SYSTEM_MEDIA_SAVEFILEPATH			"mediaSave"
#define     SYSTEM_MEDIA_SENDFILEPATH			"mediaSend"
#endif

#define 	VIDEO_FLAG			1
#define 	AUDIO_FLAG			2
#define 	START_FLAG			3
#define 	DROPSTOP_FLAG	    4
#define 	SAVESTOP_FLAG	    5

typedef struct 
{
	HI_U32 m_u32MotionStatus;  //0Í£Ö¹ 1¿ªÊ¼
	DWORD  m_u32MotionFirstTime;
	DWORD  m_u32MotionEndTime;
	DWORD  m_u32MotionStartTime;
	DWORD  m_u32MotionLastSecond;
	DWORD  m_u32MotionTimesIsValid;
	Motion_Data m_stMotionData;
	Motion_Data m_stSumMotionData;
} HI_Motion_Data;

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	//int getUnameAndPassWord(char * path,char uname[64],char password[64]);
	//int getIPAndPort(char * path,char ip[64],char port[64]);
	//int InitHiSDKServer(HI_U32 *u32Handle);
	int initHiSDKVideoAllChannel();
	int ReleaseHiSDKVideoAllChannel();
	//int ReleaseHiSDKServer(HI_U32 *u32Handle);
	int startVideoStream(HI_S_Video_Ext sVideo);
	int GetMasterVideoStream(HI_S_Video_Ext *sVideo);
	int SetMasterVideoStream(HI_S_Video_Ext *sVideo);
	int MakeKeyFrame();
	//int stopVideoStream(HI_U32 *u32Handle);
	//int setVideoParam(HI_U32 *u32Handle,HI_S_Video_Ext sVideo);
	//int getVideoParam(HI_U32 *u32Handle,HI_S_Video_Ext *sVideo);

#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif
#endif
