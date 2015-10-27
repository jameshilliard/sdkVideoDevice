#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "hisdk/hi_type.h"
#include "hisdk/hi_net_dev_sdk.h"
#include "hisdk/hi_net_dev_errors.h"
#include "hisdk/hi_param.h"

typedef unsigned long	DWORD;
#define DETECT_MAXTIME	200
// tagCapParamCfg摄像头信息默认参数
#define DE_BITRATE				512					// 码率
#define DE_FRAMERATE			12					// 帧率
#define DE_HEIGHT				720					// 视频高
#define DE_WIDTH				1280				// 视频高
#define DE_QPCONSTANT			1					// 视频质量
#define DE_KEYFRAMERATE			100					// 关键帧间隔
#define DE_CODETYPE				0					//


#define MIN(a,b) ((a)<(b))?(a):(b)
#define MAX(a,b) ((a)>(b))?(a):(b)

HI_BOOL g_main_start=HI_TRUE;

//获取开机到现在的毫秒数
DWORD getTickCountMs()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

int usSleep(long us) 
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = us;
    return select(0, NULL, NULL, NULL, &tv);
}

HI_S32 InitHiSDKServer(HI_U32 *u32Handle)
{
    HI_S32 s32Ret = HI_SUCCESS;
	HI_U32 a;
	HI_S_STREAM_INFO_EXT struStreamInfo;
	
	char uname[64]={0,};
	char password[64]={0,};
	memset(uname,0,sizeof(uname));
	memset(password,0,sizeof(password));
	int iRet=getUnameAndPassWord(UNAMEFILE,uname,password);
	printf("iRet=%d uname %s password %s\n",iRet,uname,password);
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
	printf("iRet=%d ip %s port %s\n",iRet,ip,port);
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
			printf("HI_NET_DEV_Logout is failure handle:%u,failcode:0x%x\n",*u32Handle,s32Ret);	
		}	
	}
    s32Ret = HI_NET_DEV_Login(u32Handle,uname,password,ip,u16Port);
    if(s32Ret != HI_SUCCESS)
    {
		*u32Handle=0;
		printf("HI_NET_DEV_Login is failure 0x%x\n",s32Ret);
		return -1;
    }
	return 0;
}

int ReleaseHiSDKServer(HI_U32 *u32Handle)
{
	
	HI_S32 s32Ret = HI_SUCCESS;
	if(*u32Handle != 0)
	{
		s32Ret = HI_NET_DEV_Logout(*u32Handle);
		if (s32Ret != HI_SUCCESS)
		{
			printf("HI_NET_DEV_Logout is failure handle:%u,failcode:0x%x\n",*u32Handle,s32Ret);	
		}	
		*u32Handle=0;
	}
	return 0;
}

int  controlMD(HI_U32 u32Handle,DWORD vTime)
{		
	static HI_S32 j=0;
	static DWORD  lastTickCountMs=0;
	DWORD  nowTickCountMs=getTickCountMs();
	if((nowTickCountMs-lastTickCountMs)>=vTime)
	{
		lastTickCountMs=nowTickCountMs;
	}
	else
	{
		return -1;
	}
	static HI_S_MD_PARAM mHI_S_MD_PARAM[4]=
	{
	 {HI_NET_DEV_CHANNEL_1,1,HI_TRUE,50,0			,DE_HEIGHT/8   ,DE_WIDTH/10,DE_HEIGHT/8},
	 {HI_NET_DEV_CHANNEL_1,2,HI_TRUE,50,DE_WIDTH*1/5,DE_HEIGHT*5/16,DE_WIDTH/10,DE_HEIGHT/8},
	 {HI_NET_DEV_CHANNEL_1,3,HI_TRUE,50,DE_WIDTH*2/5,DE_HEIGHT*9/16,DE_WIDTH/10,DE_HEIGHT/8},
	 {HI_NET_DEV_CHANNEL_1,4,HI_TRUE,50,DE_WIDTH*3/5,DE_HEIGHT*3/4 ,DE_WIDTH/10,DE_HEIGHT/8}
	};
	HI_S32 s32Ret=0;
	HI_S32 i=0;
	for(i=0;i<4;i++)
	{
		mHI_S_MD_PARAM[i].u32X=mHI_S_MD_PARAM[i].u32X+DE_WIDTH*1/5;
		if(mHI_S_MD_PARAM[i].u32X>=DE_WIDTH)
		{
			mHI_S_MD_PARAM[i].u32X=0;
			j=0;
		}
		HI_NET_DEV_SetConfig(u32Handle,HI_NET_DEV_CMD_MD_PARAM,&mHI_S_MD_PARAM[i],sizeof(HI_S_MD_PARAM));
		if(s32Ret != HI_SUCCESS)
		{
			printf("HI_NET_DEV_SetConfig is failure 0x%x\n",s32Ret);
		}
		//printf("area=%d,x=%d,y=%d\n",mHI_S_MD_PARAM[i].u32Area,mHI_S_MD_PARAM[i].u32X,mHI_S_MD_PARAM[i].u32Y);
	}
	printf("time=%010ld area=%d,x=%04d,y=%03d    area=%d,x=%04d,y=%03d    area=%d,x=%04d,y=%03d    area=%d,x=%04d,y=%03d\n"
	,getTickCountMs(),mHI_S_MD_PARAM[0].u32Area,mHI_S_MD_PARAM[0].u32X,mHI_S_MD_PARAM[0].u32Y
	,mHI_S_MD_PARAM[1].u32Area,mHI_S_MD_PARAM[1].u32X,mHI_S_MD_PARAM[1].u32Y
	,mHI_S_MD_PARAM[2].u32Area,mHI_S_MD_PARAM[2].u32X,mHI_S_MD_PARAM[2].u32Y
	,mHI_S_MD_PARAM[3].u32Area,mHI_S_MD_PARAM[3].u32X,mHI_S_MD_PARAM[3].u32Y);
	j++;
	return 0;
}


int main()
{
	int iRet = 0;
	HI_U32 u32Handle=0;
	HI_S32 s32Ret=HI_SUCCESS;
	DWORD  lastTickCountMs=0;
	DWORD  nowTickCountMs=0;
	int    sleepTime=0;
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGPIPE);
	sigprocmask(SIG_BLOCK, &set, NULL); 
	s32Ret=InitHiSDKServer(&u32Handle);
	if (s32Ret != HI_SUCCESS)
	{
		printf("InitHiSDKServer is failcode:0x%x",s32Ret);	
	}	
	
	while(g_main_start)
	{
		lastTickCountMs = getTickCountMs();
		controlMD(u32Handle,DETECT_MAXTIME);
		nowTickCountMs = getTickCountMs();
		sleepTime=(nowTickCountMs-lastTickCountMs)*1000;
		if(sleepTime>DETECT_MAXTIME*1000)
			sleepTime=DETECT_MAXTIME*1000-DETECT_MAXTIME*100;
		else if(sleepTime<0)
			sleepTime=0;
		//printf("nowTickCountMs=%d lastTickCountMs=%d sleepTime=%d\n",nowTickCountMs,lastTickCountMs,sleepTime);
		//usSleep(DETECT_MAXTIME*1000-sleepTime);
	}
	return 0;
}

