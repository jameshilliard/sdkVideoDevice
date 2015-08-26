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

#define		DEVICEWIDTHBIG						1280
#define		DEVICEHIGHHBIG						960

#define		DEVICEWIDTHMID						640
#define		DEVICEHIGHHMID						352

#define		DEVICEWIDTHSMALL					320
#define		DEVICEHIGHHSMALL					176
#define 	CHINATIME 							8*3600

#define 	BEFORE_RECORD_MOTION_LASTTIME		5
#define 	BEFORE_RECORD_MOTION_TIMES			3

#define 	CONTINUES_RECORD_MOTION_LASTTIME	5
#define 	CONTINUES_RECORD_MOTION_TIMES		3

#define		END_RECORD_MOTION_TIME				3*60

#define 	VIDEOBUFFERSIZE						1024*1024*4

#ifndef DEBUG_CPU_X86
#define     SYSTEM_MEDIA_SAVEFILEPATH			"/mnt/mtd/ipc/tmpfs/sd/mediaSave"
#else
#define     SYSTEM_MEDIA_SAVEFILEPATH			"mediaSave"
#endif

typedef struct 
{
	HI_U32 m_u32MotionCountPerSecond[20]; //记录每秒移动侦测的次数
	HI_U32 m_u32MotionStatus;  //0停止 1开始
	DWORD  m_u32MotionFirstTime;
	DWORD  m_u32MotionEndTime;
	DWORD  m_u32MotionStartTime;
	DWORD  m_u32MotionLastSecond;
	DWORD  m_u32MotionTimesIsValid;
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
	int MakeKeyFrame();
	//int stopVideoStream(HI_U32 *u32Handle);
	//int setVideoParam(HI_U32 *u32Handle,HI_S_Video_Ext sVideo);
	//int getVideoParam(HI_U32 *u32Handle,HI_S_Video_Ext *sVideo);

#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif
#endif
