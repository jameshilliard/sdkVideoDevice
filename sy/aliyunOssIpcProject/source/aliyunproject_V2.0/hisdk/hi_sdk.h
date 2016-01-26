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
#define     SYSTEM_SD_SAVEFILEPATH			    "/mnt/mtd/ipc/tmpfs/sd"
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


#define 	MAX_MOTION_STRING	50*1024
#define     MAX_SOUND_STRING	20*1024
#define 	DETECT_MAXTIME		200
#define   	MAX_AUDIO_PACKETS	8000/5

#define P2PPROCESS			"tutk"
#define P2PWORKMINVALUE		3
#define STARTUPMINTIME		300*1000
#define AUDIOUSERLOGININ	"/mnt/mtd/ipc/tmpfs/syflash/audio/userLoginIn.g711"
#define AUDIOURGENCYEND		"/mnt/mtd/ipc/tmpfs/syflash/audio/urgencyEnd.g711"
#define AUDIOURGENCYSTART	"/mnt/mtd/ipc/tmpfs/syflash/audio/urgencyStart.g711"
#define AUDIOSTARTUP		"/mnt/mtd/ipc/tmpfs/syflash/audio/startUp.g711"

typedef struct 
{
	HI_U32 m_u32MotionStatus;  //0Í£Ö¹ 1¿ªÊ¼
	DWORD  m_u32MotionFirstTime;
	DWORD  m_u32MotionEndTime;
	DWORD  m_u32MotionStartTime;
	DWORD  m_u32MotionLastSecond;
	DWORD  m_u32MotionTimesIsValid;
	HI_U32 m_u32AreaTimes[16];
	Motion_Data m_stMotionData;
	Motion_Data m_stSumMotionData;
	
} HI_Motion_Data;


typedef struct 
{
	DWORD  		m_u32MotionStartTime;
	HI_U32 		m_u32StartAreaTimes[120][16];
	HI_U32 		m_u32SoundSize[120];
	HI_U32		m_uNowFlag;
	HI_U32		m_uLastFlag;
	HI_U32		m_uStartFlag;
	HI_U32		m_uStartSoundFlag;
	HI_U32		m_uOverFlag;
	HI_U32		m_uOverSoundFlag;
} HI_UrgencyMotion_Data;

typedef struct 
{
	INT32S m_channel;
	HI_U32 m_u32Handle;
	HI_U32 m_uFlag;
	BOOL   m_start;
} HI_CONTROLMD_DATA;

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
