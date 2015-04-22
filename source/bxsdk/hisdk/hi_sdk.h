#ifndef __HI_SDK_H_
#define  __HI_SDK_H_

#include <stdio.h>
#include <unistd.h>
#include "hi_type.h"
#include "hi_net_dev_sdk.h"
#include "hi_net_dev_errors.h"


#define 	UNAME					"admin"
#define 	PWORD					"admin"
#define 	IPHOST					"192.168.1.117"
#define 	IPPORT					"80"
#define 	RECORD_FILE 			"test.hx"
#define 	UNAMEFILE				"/mnt/mtd/ipc/conf/config_user.ini"
#define 	NETFILE					"/mnt/mtd/ipc/conf/config_net.ini"


#define		DEVICEWIDTHBIG			1280
#define		DEVICEHIGHHBIG			720

#define		DEVICEWIDTHMID			640
#define		DEVICEHIGHHMID			352

#define		DEVICEWIDTHSMALL		320
#define		DEVICEHIGHHSMALL		176


#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
	int getUnameAndPassWord(char * path,char uname[64],char password[64]);
	int getIPAndPort(char * path,char ip[64],char port[64]);
	int InitHiSDKServer(HI_U32 *u32Handle);
	int ReleaseHiSDKServer(HI_U32 *u32Handle);
	int startVideoStream(HI_U32 *u32Handle,HI_S_Video_Ext sVideo);
	int stopVideoStream(HI_U32 *u32Handle);
	int setVideoParam(HI_U32 *u32Handle,HI_S_Video_Ext sVideo);
	int getVideoParam(HI_U32 *u32Handle,HI_S_Video_Ext *sVideo);

#  ifdef __cplusplus
}
#  endif /* __cplusplus */


#endif