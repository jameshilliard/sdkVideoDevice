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

#define		DEVICEWIDTHBIG			1280
#define		DEVICEHIGHHBIG			960

#define		DEVICEWIDTHMID			640
#define		DEVICEHIGHHMID			352

#define		DEVICEWIDTHSMALL		320
#define		DEVICEHIGHHSMALL		176
#define 	CHINATIME 				8*3600

#define 	VIDEOBUFFERSIZE			1024*1024*4

typedef struct 
{
	HI_U32 m_u32MotionCountPerSecond[20]; //��¼ÿ���ƶ����Ĵ���
	HI_U32 m_u32MotionStatus;  //0ֹͣ 1��ʼ
	DWORD  m_u32MotionStartTime;
	DWORD  m_u32MotionLastSecond;
	
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